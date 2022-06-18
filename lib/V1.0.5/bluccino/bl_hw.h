//==============================================================================
// bl_hw.h
// Bluccino HW core supporting basic functions for button & LED
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================

#ifndef __BL_HW_H__
#define __BL_HW_H__

//==============================================================================
// enable masks for button events
// - usage:
//     bl_cfg(bl_hw,_BUTTON,BL_PRESS)          // enable [BUTTON:PRESS]
//     bl_cfg(bl_hw,_BUTTON,BL_RELEASE)        // enable [BUTTON:RELEASE]
//     bl_cfg(bl_hw,_BUTTON,BL_EDGE)           // enable [BUTTON:PRESS/RELEASE]
//     bl_cfg(bl_hw,_BUTTON,BL_SWITCH)         // enable [BUTTON:SWITCH]
//     bl_cfg(bl_hw,_BUTTON,BL_CLICK)          // enable [BUTTON:CLICK]
//     bl_cfg(bl_hw,_BUTTON,BL_HOLD)           // enable [BUTTON:HOLD]
//
//     bl_cfg(bl_hw,_BUTTON,0xffff)            // enable all [BUTTON:] events
//     bl_cfg(bl_hw,_BUTTON,0x0000)            // disable all [BUTTON:] events
//     bl_cfg(bl_hw,_BUTTON,BL_PRESS|BL_RELEASE) // press/release events
//     bl_cfg(bl_hw,_BUTTON,BL_EDGE)           // same as above
//     bl_cfg(bl_hw,_BUTTON,BL_CLICK|BL_HOLD)  // click/hold events
//     bl_cfg(bl_hw,_BUTTON,BL_MULTI)          // same as above
//==============================================================================

  #define BL_PRESS   0x0001  // mask for [BUTTON:PRESS] events
  #define BL_RELEASE 0x0002  // mask for [BUTTON:RELEASE] events
  #define BL_EDGE    0x0003  // mask for [BUTTON:PRESS],[BUTTON:RELEASE]
  #define BL_SWITCH  0x0004  // mask for [BUTTON:SWITCH] events
  #define BL_CLICK   0x0008  // mask for [BUTTON:CLICK] events
  #define BL_HOLD    0x0010  // mask for [BUTTON:HOLD] events
  #define BL_MULTI   0x0018  // mask for [BUTTON:CLICK],[BUTTON:HOLD]
  #define BL_ALL     0xFFFF  // mask for all [BUTTON:] events
  #define BL_NONE    0x0000  // mask for no [BUTTON:] events

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

  int bl_hw(BL_ob *o, int val);        // HW core module interface

//==============================================================================
// syntactic sugar: HW core init
// - usage: bl_hw_SYS_INIT(bl_hw,cb)
//==============================================================================

  static inline int bl_hw_SYS_INIT(BL_oval module,BL_oval cb)
  {
    return bl_init(module,cb);
  }

//==============================================================================
// syntactic sugar: set LED @id on/off,          // send augmented message
// - usage: _bl_hw_LED_SET(bl_hw,id,val)         // val 0:off, 1:on
//==============================================================================

  static inline int _bl_hw_LED_SET(BL_oval module,int id, int val)
  {
    return _bl_msg(module,_LED,SET_, id,NULL,val);
  }

//==============================================================================
// syntactic sugar: toggle LED @id (@id: 0..4),  // send augmented message
// - usage: _bl_hw_LED_TOGGLE(bl_hw,id)          // (BL_HW)<-[#LED:TOGGLE @id]
//==============================================================================

  static inline int _bl_hw_LED_TOGGLE(BL_oval module, int id)
  {
    return _bl_msg(module,_LED,TOGGLE_, id,NULL,0);
  }

#endif // __BL_HW_H__
