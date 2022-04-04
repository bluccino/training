//==============================================================================
// bl_hw.c
// Bluccino HW core supporting basic functions for button & LED
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================
//
// Module Hierarchie
//
// +- BL_HW
//    +- BL_HWLED
//    +- BL_HWBUT
//
// interaction of BL_HW (HW core) with BL_HWBUT, BL_HWLED (drivers)
// (H) := (BL_HW); (B) := (BL_HWBUT); (L) := (BL_HWLED)
//
//       (^) (v)    +--------------------+
//        |   |     |       BL_HW        | HW core
//        |   |     +--------------------+
//        |  (v)===>|        SYS:        | SYS: interface
//        |  (v)===>|        LED:        | LED: input interface
//       (^)<=======|       BUTTON:      | BUTTON: output interface
//       (^)<=======|       SWITCH:      | SWITCH: output interface
//                  +- - - - - - - - - - +
//  (L)<============|        LED:        | LED: output interface
//   | (H)=========>|       BUTTON:      | BUTTON: input interface
//   |  ^(H)=======>|       SWITCH:      | SWITCH: input interface
//   |  | ^         +--------------------+
//   |  | |
//   |  | |  (v)    +--------------------+
//   |  | |   |     |     BL_HWBUT       | button driver
//   |  | |   |     +--------------------+
//   |  | |  (v)===>|        SYS:        | SYS: interface
//   |  |(H)<=======|       SWITCH:      | LED: output interface
//   | (H)<=========|       BUTTON:      | BUTTON: output interface
//   |              +--------------------+
//   |       (v)    +--------------------+
//   |        |     |     BL_HWLED       | LED driver
//   |        |     +--------------------+
//   v       (v)===>|        SYS:        | SYS: interface
//  (L)============>|        LED:        | LED: input interface
//                  +--------------------+
//
//==============================================================================
//
// Message Flow Diagram: Initializing
//
//     BL_DOWN         BL_HW        BL_HWBUT       BL_HWLED         BL_UP
//       (v)            (#)            (B)            (L)            (^)
//        |              |              |              |              |
//        |  [SYS:INIT]  |              |              |              |
//        o------------->#===========================================>|
//        |   <BL_UP>    |              |              |              |
//        |              |  [SYS:INIT]  |              |              |
//        |              o------------->|              |              |
//        |              |   <BL_HW>    |              |              |
//        |              |<=============#              |              |
//        |              |              |  [SYS:INIT]  |              |
//        |              o---------------------------->|              |
//        |              |              |   <BL_HW>    |              |
//        |              |<============================#              |
//        |              |              |              |              |
//
// Message Flow Diagram: Ticking
//
//     BL_DOWN         BL_HW        BL_HWBUT       BL_HWLED         BL_UP
//       (v)            (#)            (B)            (L)            (^)
//        |              |              |              |              |
//        |  [SYS:TICK]  |              |              |              |
//        o------------->|  [SYS:TICK]  |              |              |
//        |              o------------->|  [SYS:TICK]  |              |
//        |              o---------------------------->|              |
//        |              |              |              |              |
//
// Message Flow Diagram: Tocking
//
//     BL_DOWN         BL_HW        BL_HWBUT       BL_HWLED         BL_UP
//       (v)            (#)            (B)            (L)            (^)
//        |              |              |              |              |
//        |  [SYS:TOCK]  |              |              |              |
//        o------------->|  [SYS:TOCK]  |              |              |
//        |              o------------->|  [SYS:TOCK]  |              |
//        |              o---------------------------->|              |
//        |              |              |              |              |
//
// Message Flow Diagram: Setting LED ON/OFF State
//
//     BL_DOWN         BL_HW        BL_HWBUT       BL_HWLED         BL_UP
//       (v)            (#)            (B)            (L)            (^)
//        |              |              |              |              |
//        |  [LED:SET]   |              |              |              |
//        o------------->|              |              |              |
//        |   @id,onoff  |              | [LED:ONOFF]  |              |
//        |              o---------------------------->|              |
//        |              |              |  @id,onoff   |              |
//
// Message Flow Diagram: Button Configuration
//
//     BL_DOWN         BL_HW        BL_HWBUT       BL_HWLED         BL_UP
//       (v)            (#)            (B)            (L)            (^)
//        |              |              |              |              |
//        | [BUTTON:CFG] |              |              |              |
//        o------------->|              |              |              |
//        |     flags    | [BUTTON:CFG] |              |              |
//        |              o------------->|              |              |
//        |              |    flags     |              |              |
//
// Message Flow Diagram: Button Click (Button Release within Grace Time)
//
//   BL_DOWN           BL_HW          BL_HWBUT         BL_HWLED           BL_UP
//     (v)              (#)              (B)              (L)              (^)
//      |                |                |                |                |
//      |                |        +-------v-------+        |                |
//      |                |        | press button  |        |                |
//      |                |        | time[@id] = 0 |        |                |
//      |                |        +-------v-------+        |                |
//      |                |                |                |                |
//      |                | [BUTTON:PRESS] |                |                |
//      |                |<---------------o                |                |
//      |                |      @id,0     |                | [BUTTON:PRESS] |
//      |                o------------------------------------------------->|
//      |                | [BUTTON:CLICK] |                |     @id,0      |
//      |                |<---------------o                |                |
//      |                |      @id,0     |                | [BUTTON:CLICK] |
//      |                o------------------------------------------------->|
//      |                |                |                |     @id,0      |
//      |                |        +-------v--------+       |                |
//      |                |        | onoff = !onoff |       |                |
//      |                |        | (toggle switch)|       |                |
//      |                |        +-------v--------+       |                |
//      |                |                |                |                |
//      :                :                :                :                :
//      |                |                |                |                |
//      |                |                - now < grace    |                |
//      |                |        +-------v--------+       |                |
//      |                |        | release button |       |                |
//      |                |        |ms=now-time[@id]|       |                |
//      |                |        +-------v--------+       |                |
//      |                |[BUTTON:RELEASE]|                |                |
//      |                |<---------------o                |                |
//      |                |      @id,ms    |                |[BUTTON:RELEASE]|
//      |                o------------------------------------------------->|
//      |                |                |                |                |
//      |                |       +--------v--------+       |                |
//      |                |       | ms < grace time |       |                |
//      |                |       | we had 1 click  |       |                |
//      |                |       |     cnt = 1     |       |                |
//      |                |       +--------v--------+       |                |
//      |                | [BUTTON:CLICK] |                |     @id,ms     |
//      |                |<---------------o                |                |
//      |                |     @id,cnt    |                | [BUTTON:CLICK] |
//      |                o------------------------------------------------->|
//      |                |                |                |    @id,cnt     |
//      |                |                |                |                |
//
// Message Flow Diagram: Button Hold (Button Release Exceeds Grace Time)
//
//   BL_DOWN           BL_HW          BL_HWBUT         BL_HWLED           BL_UP
//     (v)              (#)              (B)              (L)              (^)
//      |                |                |                |                |
//      |                |        +-------v-------+        |                |
//      |                |        | press button  |        |                |
//      |                |        | time[@id] = 0 |        |                |
//      |                |        +-------v-------+        |                |
//      |                | [BUTTON:PRESS] |                |                |
//      |                |<---------------o                |                |
//      |                |      @id,0     |                | [BUTTON:PRESS] |
//      |                o------------------------------------------------->|
//      |                | [BUTTON:CLICK] |                |     @id,0      |
//      |                |<---------------o                |                |
//      |                |      @id,0     |                | [BUTTON:CLICK] |
//      |                o------------------------------------------------->|
//      |                |                |                |     @id,0      |
//      |                |        +-------v--------+       |                |
//      |                |        | onoff = !onoff |       |                |
//      |                |        | (toggle switch)|       |                |
//      |                |        +-------v--------+       |                |
//      |                |                |                |                |
//      :                :                :                :                :
//      |                |                |                |                |
//      |                |                - now == grace   |                |
//      |                | [BUTTON:HOLD]  |                |                |
//      |                |<---------------o                |                |
//      |                |     @id,0      |                | [BUTTON:HOLD]  |
//      |                o------------------------------------------------->|
//      |                |                |                |     @id,0      |
//      |                |        +-------v--------+       |                |
//      |                |        | release button |       |                |
//      |                |        |ms=now-time[@id]|       |                |
//      |                |        +-------v--------+       |                |
//      |                |                |                |                |
//      |                |[BUTTON:RELEASE]|                |                |
//      |                |<---------------o                |                |
//      |                |      @id,ms    |                |[BUTTON:RELEASE]|
//      |                o------------------------------------------------->|
//      |                |                |                |                |
//      |                | [BUTTON:HOLD]  |                |     @id,ms     |
//      |                |<---------------o                |                |
//      |                |     @id,ms     |                | [BUTTON:HOLD]  |
//      |                o------------------------------------------------->|
//      |                |                |                |     @id,0      |
//      |                |                |                |                |
//
//==============================================================================

  #include "bluccino.h"

  #define init  init_button
  #include "bl_hwbut.c"                // button core driver
  #undef init
  #undef LOG
  #undef LOGO

  #define init  init_led
  #include "bl_hwled.c"                // LED core driver
  #undef init
  #undef LOG
  #undef LOGO

//==============================================================================
// CORE level logging shorthands
//==============================================================================

  #define LOG                     LOG_CORE
  #define LOGO(lvl,col,o,val)     LOGO_CORE(lvl,col"bl_hw:",o,val)

//==============================================================================
// public module interface
//==============================================================================
//
// (B) := (BL_HWBUT);  (L) := (BL_HWLED);  (v) := (BL_DOWN);  (^) := (BL_UP)
//
//                  +--------------------+
//                  |       BL_HW        |
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (v)->     INIT ->|       <out>        | init module, store <out> callback
// (v)->     TICK ->|       @id,cnt      | tick the module
// (!)->      CFG ->|        mask        | config module
//                  |        SYS:        | SYS output interface
// (L,B)<-   INIT <-|       <out>        | init module, store <out> callback
// (L,B)->   TICK <-|       @id,cnt      | tick the module
// (B)<-      CFG <-|        mask        | config module
//                  +--------------------+
//                  |        LED:        | LED: input interface
// (v)->      SET ->|      @id,onoff     | set LED @id on/off (i=0..4)
// (v)->   TOGGLE ->|                    | toggle LED @id (i=0..4)
//                  |        LED:        | LED: output interface
// (L)<-      SET <-|      @id,onoff     | set LED @id on/off (i=0..4)
// (L)<-   TOGGLE <-|                    | toggle LED @id (i=0..4)
//                  +--------------------+
//                  |       BUTTON:      | BUTTON output interface
// (^)<-    PRESS <-|        @id,0       | button @id pressed (rising edge)
// (^)<-  RELEASE <-|        @id,ms      | button release after elapsed ms-time
// (^)<-    CLICK <-|       @id,cnt      | button @id clicked (cnt: nmb. clicks)
// (^)<-     HOLD <-|       @id,time     | button @id held (time: hold time)
// (B)<-      CFG <-|        mask        | config button event mask
//                  |       BUTTON:      | BUTTON input interface
// (B)->    PRESS ->|        @id,1       | button @id pressed (rising edge)
// (B)->  RELEASE ->|        @id,ms      | button release after elapsed ms-time
// (B)->    CLICK ->|       @id,cnt      | button @id clicked (cnt: nmb. clicks)
// (B)->     HOLD ->|       @id,time     | button @id held (time: hold time)
// (v)->      CFG ->|        mask        | config button event mask
//                  +--------------------+
//                  |       SWITCH:      | SWITCH: output interface
// (^)<-      STS <-|       @id,sts      | on/off status update of switch @id
//                  |       SWITCH:      | SWITCH: input interface
// (B)->      STS ->|       @id,sts      | on/off status update of switch @id
//                  +--------------------+
//
//==============================================================================

  int bl_hw(BL_ob *o, int val)         // HW core module interface
  {
    static BL_oval out = bl_up;        // <out> messages go to BL_UP by default
    static BL_oval led = bl_hwled;     // <led> callback to go to BL_HWLED

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <cb>]
      {
        LOG(3,BL_C "init HW core ...");
/*******************
        out = o->data;                 // store <out> callback
*****************/
        bl_init(bl_hwbut,bl_hw);       // init BL_HWBUT module, output goes here
        bl_init(bl_hwled,bl_hw);       // init BL_HWLED module, output goes here
      	return 0;                      // OK
      }

      case BL_ID(_SYS,TICK_):          // [SYS:TICK @id,cnt]
        return bl_hwbut(o,val);        // tick BL_HWBUT module

      case BL_ID(_LED,SET_):           // [LED:set @id,val]
      case BL_ID(_LED,TOGGLE_):        // [LED:toggle @id]
        return bl_out(o,val,led);      // forward to LED driver module

      case BL_ID(_BUTTON,PRESS_):      // [BUTTON:PRESS @id,val]
      case BL_ID(_BUTTON,RELEASE_):    // [BUTTON:RELEASE @id,val]
      case BL_ID(_BUTTON,CLICK_):      // [BUTTON:CLICK @id,edge]
      case BL_ID(_BUTTON,HOLD_):       // [BUTTON:HOLD @id,time]
      case BL_ID(_SWITCH,STS_):        // [SWITCH:STS @id]
        return bl_out(o,val,out);      // output message

      case BL_ID(_BUTTON,CFG_):        // [BUTTON:CFG flags]
        return bl_hwbut(o,val);        // config BL_HWBUT module

      default:
        return -1;                     // bad input
    }
  }
