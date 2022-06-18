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
//    +- BL_HWNVM
//
// interaction of BL_HW (HW core) with BL_HWBUT, BL_HWLED (drivers)
// (H) := (BL_HW); (B) := (BL_HWBUT); (L) := (BL_HWLED)
//
//       (U) (D)    +--------------------+
//        |   |     |       BL_HW        | HW core
//        |   |     +--------------------+
//        |  (D)===>|        SYS:        | SYS: interface
//        |  (D)===>|        LED:        | LED: input interface
//       (U)<=======|       BUTTON:      | BUTTON: output interface
//       (U)<=======|       SWITCH:      | SWITCH: output interface
//                  +- - - - - - - - - - +
//  (L)<============|        LED:        | LED: output interface
//   | (H)=========>|       BUTTON:      | BUTTON: input interface
//   |  ^(H)=======>|       SWITCH:      | SWITCH: input interface
//   |  | ^         +--------------------+
//   |  | |
//   |  | |  (D)    +--------------------+
//   |  | |   |     |     BL_HWBUT       | button driver
//   |  | |   |     +--------------------+
//   |  | |  (D)===>|        SYS:        | SYS: interface
//   |  |(H)<=======|       SWITCH:      | LED: output interface
//   | (H)<=========|       BUTTON:      | BUTTON: output interface
//   |              +--------------------+
//   |       (D)    +--------------------+
//   |        |     |     BL_HWLED       | LED driver
//   |        |     +--------------------+
//   v       (D)===>|        SYS:        | SYS: interface
//  (L)============>|        LED:        | LED: input interface
//                  +--------------------+
//
//==============================================================================
//
// Message Flow Diagram: Initializing
//
//     BL_DOWN         BL_HW        BL_HWBUT       BL_HWLED         BL_UP
//       (D)            (#)            (B)            (L)            (U)
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
//       (D)            (#)            (B)            (L)            (U)
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
//       (D)            (#)            (B)            (L)            (U)
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
//       (D)            (#)            (B)            (L)            (U)
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
//       (D)            (#)            (B)            (L)            (U)
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
//     (D)              (#)              (B)              (L)              (U)
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
//     (D)              (#)              (B)              (L)              (U)
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

  #define sys_init  sys_init_button
  #include "bl_hwbut.c"                // button core driver
  #undef sys_init

  #define sys_init  sys_init_led
  #include "bl_hwled.c"                // LED core driver
  #undef sys_init

  #define sys_init  sys_init_nvm
  #include "bl_hwnvm.c"                // NVM core driver
  #undef sys_init_nvm

//==============================================================================
// logging shorthands
//==============================================================================

  #define WHO  "bl_hw:"                // who is logging

  #define LOG                          LOG_CORE
  #define LOGO(lvl,col,o,val)          LOGO_CORE(lvl,col WHO,o,val)

//==============================================================================
// public module interface
//==============================================================================
//
// (B) := (BL_HWBUT);  (L) := (BL_HWLED);  (U) := (BL_UP);  (D) := (BL_DOWN);
//
//                  +--------------------+
//                  |       BL_HW        |
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (D)->     INIT ->|       <out>        | init module, store <out> callback
// (D)->     TICK ->|       @id,cnt      | tick the module
// (D)->      CFG ->|        mask        | config module
//                  |....................|
//                  |        SYS:        | SYS output interface
// (L,B)<-   INIT <-|       <out>        | init module, store <out> callback
// (L,B)<-   TICK <-|       @id,cnt      | tick the module
// (B)<-      CFG <-|        mask        | config module
//                  +--------------------+
//                  |        LED:        | LED: input interface
// (D)->      SET ->|      @id,onoff     | set LED @id on/off (i=0..4)
// (D)->   TOGGLE ->|                    | toggle LED @id (i=0..4)
//                  |....................|
//                  |        LED:        | LED: output interface
// (L)<-      SET <-|      @id,onoff     | set LED @id on/off (i=0..4)
// (L)<-   TOGGLE <-|                    | toggle LED @id (i=0..4)
//                  +--------------------+
//                  |       BUTTON:      | BUTTON input interface
// (B)->    PRESS ->|        @id,1       | button @id pressed (rising edge)
// (B)->  RELEASE ->|        @id,ms      | button release after elapsed ms-time
// (B)->    CLICK ->|       @id,cnt      | button @id clicked (cnt: nmb. clicks)
// (B)->     HOLD ->|       @id,time     | button @id held (time: hold time)
// (D)->      CFG ->|        mask        | config button event mask
// (D)->       MS ->|         ms         | set click/hold discrimination time
//                  |....................|
//                  |       BUTTON:      | BUTTON output interface
// (U)<-    PRESS <-|        @id,0       | button @id pressed (rising edge)
// (U)<-  RELEASE <-|        @id,ms      | button release after elapsed ms-time
// (U)<-    CLICK <-|       @id,cnt      | button @id clicked (cnt: nmb. clicks)
// (U)<-     HOLD <-|       @id,time     | button @id held (time: hold time)
// (B)<-      CFG <-|        mask        | config button event mask
// (B)<-       MS <-|         ms         | set click/hold discrimination time
//                  +--------------------+
//                  |       SWITCH:      | SWITCH: input interface
// (B)->      STS ->|       @id,sts      | on/off status update of switch @id
//                  |....................|
//                  |       SWITCH:      | SWITCH: output interface
// (U)<-      STS <-|       @id,sts      | on/off status update of switch @id
//                  +--------------------+
//                  |        NVM:        | NVM input interface
// (D)->     LOAD ->|      <BL_nvm>      | load NVM data
// (D)->     SAVE ->|      <BL_nvm>      | save NVM data
// (D)->      CFG ->|                    | is NVM configured (available)?
//                  +--------------------+
//
//==============================================================================

  int bl_hw(BL_ob *o, int val)         // HW core module interface
  {
    static BL_oval U = bl_up;          // <out> messages go to BL_UP by default
    static BL_oval L = bl_hwled;       // (L) callback to go to BL_HWLED
    static BL_oval B = bl_hwbut;       // (B) callback to go to BL_HWBUT
    static BL_oval N = bl_hwnvm;       // (N) callback to go to BL_HWNVM

    switch (bl_id(o))
    {
      case SYS_INIT_0_cb_0:
      {
        LOG(3,BL_C "init HW core ...");
        U = bl_cb(o,(U),WHO"(U)");     // store output callback
        bl_init((B),(U));              // init bl_hwbut module, output goes up
        bl_init((L),(U));              // init bl_hwled module, output goes up
        bl_init((N),(U));              // init bl_hwnvm module, output goes up
      	return 0;                      // OK
      }

      case SYS_TICK_id_BL_pace_cnt:
        return bl_fwd(o,val,(B));      // tick bl_hwbut module

      case SYS_TOCK_id_BL_pace_cnt:
        return bl_fwd(o,val,(N));      // tock bl_hwnvm module

      case LED_SET_id_0_onoff:
      case LED_TOGGLE_id_0_0:
        return bl_fwd(o,val,(L));      // forward to LED driver module

      case BUTTON_PRESS_id_0_0:
      case BUTTON_RELEASE_id_0_ms:
      case BUTTON_CLICK_id_0_cnt:
      case BUTTON_HOLD_id_0_ms:
      case SWITCH_STS_id_0_sts:
        return bl_fwd(o,val,(U));      // forward message to up gear

      case BUTTON_CFG_0_0_mask:
      case BUTTON_MS_0_0_ms:
        return bl_fwd(o,val,(B));      // config bl_hwbut module

      case NVM_LOAD_0_BL_dac_0:
      case NVM_SAVE_0_BL_dac_0:
      case NVM_STORE_id_0_val:
      case NVM_RECALL_id_0_0:
      case NVM_AVAIL_0_0_0:
        return bl_fwd(o,val,(N));      // forward to bl_hwnvm module

      case _NVM_READY_0_0_sts:
        return bl_out(o,val,(U));      // forward to up gear

      default:
        return -1;                     // bad input
    }
  }

//==============================================================================
// cleanup (needed for *.c file merge of the bluccino core)
//==============================================================================

  #include "bl_clean.h"
