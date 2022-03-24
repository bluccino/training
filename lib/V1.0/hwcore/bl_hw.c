//==============================================================================
// bl_hw.c
// Bluccino HW core supporting basic functions for button & LED
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================
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

  #define init  init_button
  #include "bl_hwbut.c"                // button core driver

  #undef LOG
  #undef LOGO
  #undef LOG0
  #undef init

  #define init  init_led
  #include "bl_hwled.c"                // LED core driver

//==============================================================================
// public module interface
//==============================================================================
//
// (B) := (BL_HWBUT);  (L) := (BL_HWLED)
//                  +--------------------+
//                  |       BL_HW        |
//                  +--------------------+
//                  |        SYS:        | SYS: interface
// (v)->     INIT ->|       <out>        | init module, store <out> callback
// (v)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |        LED:        | LED: input interface
// (v)->      SET ->|      @id,onoff     | set LED @id on/off (i=0..4)
// (v)->   TOGGLE ->|                    | toggle LED @id (i=0..4)
//                  +--------------------+
//                  |       BUTTON:      | BUTTON: output interface
// (^)<-    PRESS <-|        @id,1       | button @id pressed (rising edge)
// (^)<-  RELEASE <-|        @id,0       | button @id released (falling edge)
// (^)<-    CLICK <-|       @id,cnt      | button @id clicked (cnt: nmb. clicks)
// (^)<-     HOLD <-|       @id,time     | button @id held (time: hold time)
//                  +--------------------+
//                  |       SWITCH:      | SWITCH: output interface
// (^)<-      STS <-|      @id,onoff     | on/off status update of switch @id
//                  +====================+
//                  |        LED:        | LED: output interface
// (L)<-      SET <-|      @id,onoff     | set LED @id on/off (i=0..4)
// (L)<-   TOGGLE <-|                    | toggle LED @id (i=0..4)
//                  +--------------------+
//                  |       BUTTON:      | BUTTON: input interface
// (B)->    PRESS ->|        @id,1       | button @id pressed (rising edge)
// (B)->  RELEASE ->|        @id,0       | button @id released (falling edge)
// (B)->    CLICK ->|       @id,cnt      | button @id clicked (cnt: nmb. clicks)
// (B)->     HOLD ->|       @id,time     | button @id held (time: hold time)
//                  +--------------------+
//                  |       SWITCH:      | SWITCH: input interface
// (B)->      STS ->|      @id,onoff     | on/off status update of switch @id
//                  +--------------------+
//
//==============================================================================

  int bl_hw(BL_ob *o, int val)         // HW core module interface
  {
    static BL_fct out = NULL;          // to store <out> callback
    static BL_fct led = bl_hwled;      // <led> callback to go to BL_HWLED

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <cb>]
      {
        LOG(3,BL_C "initialising HW core ...");
        out = o->data;                 // store <out> callback

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

      default:
        return -1;                     // bad input
    }
  }
