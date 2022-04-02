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
//     bl_cfg(bl_hw,BL_BUTTON_PRESS)   // enable [BUTTON:PRESS]
//     bl_cfg(bl_hw,BL_BUTTON_RELEASE) // enable [BUTTON:RELEASE]
//     bl_cfg(bl_hw,BL_BUTTON_EDGE)    // enable [BUTTON:PRESS],[BUTTON:RELEASE]
//     bl_cfg(bl_hw,BL_BUTTON_SWITCH)  // enable [BUTTON:SWITCH]
//     bl_cfg(bl_hw,BL_BUTTON_CLICK)   // enable [BUTTON:CLICK]
//     bl_cfg(bl_hw,BL_BUTTON_HOLD)    // enable [BUTTON:HOLD]
//     bl_cfg(bl_hw,BL_BUTTON_ALL)     // enable all [BUTTON:] events
//     bl_cfg(bl_hw,BL_BUTTON_NONE)    // disable all [BUTTON:] events
//
//     bl_cfg(bl_hw,BL_BUTTON_PRESS|BL_BUTTON_RELEASE) // press/release events
//     bl_cfg(bl_hw,BL_BUTTON_EDGE)                    // same as above
//     bl_cfg(bl_hw,BL_BUTTON_CLICK|BL_BUTTON_HOLD)    // click/hold events
//     bl_cfg(bl_hw,BL_BUTTON_MULTI)                   // same as above
//==============================================================================

  #define BL_BUTTON_PRESS   0x0001  // mask for [BUTTON:PRESS] events
  #define BL_BUTTON_RELEASE 0x0002  // mask for [BUTTON:RELEASE] events
  #define BL_BUTTON_EDGE    0x0003  // mask for [BUTTON:PRESS],[BUTTON:RELEASE]
  #define BL_BUTTON_SWITCH  0x0004  // mask for [BUTTON:SWITCH] events
  #define BL_BUTTON_CLICK   0x0008  // mask for [BUTTON:CLICK] events
  #define BL_BUTTON_HOLD    0x0010  // mask for [BUTTON:HOLD] events
  #define BL_BUTTON_MULTI   0x0018  // mask for [BUTTON:CLICK],[BUTTON:HOLD]
  #define BL_BUTTON_ALL     0xFFFF  // mask for all [BUTTON:] events
  #define BL_BUTTON_NONE    0x0000  // mask for no [BUTTON:] events

//==============================================================================
// public module interface
//==============================================================================
//
// (B) := (BL_HWBUT);  (L) := (BL_HWLED);  (v) := (BL_DOWN);  (^) := (BL_UP)
//
//                  +--------------------+
//                  |       BL_HW        |
//                  +--------------------+
//                  |        SYS:        | SYS: interface
// (v)->     INIT ->|       <out>        | init module, store <out> callback
// (v)->     TICK ->|       @id,cnt      | tick the module
// (!)->      CFG ->|        mask        | config module
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
//                  |       BUTTON:      | BUTTON input interface
// (B)->    PRESS ->|        @id,1       | button @id pressed (rising edge)
// (B)->  RELEASE ->|        @id,ms      | button release after elapsed ms-time
// (B)->    CLICK ->|       @id,cnt      | button @id clicked (cnt: nmb. clicks)
// (B)->     HOLD ->|       @id,time     | button @id held (time: hold time)
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
// - usage: bl_hw_init(cb)
//==============================================================================

  static inline int bl_hw_init(BL_oval cb)
  {
    return bl_init(bl_hw,cb);
  }

//==============================================================================
// syntactic sugar: set LED @id on/off or togggle LED @id (@id: 0..4)
// - usage: bl_led(id,val)   // val 0:off, 1:on, -1:toggle
//==============================================================================

  static inline int bl_led(int id, int val)
  {
    BL_ob oo = {_LED,val<0?TOGGLE_:SET_,id,NULL};
    return bl_in(&oo,val<0?0:val);
  }

//==============================================================================
// syntactic sugar: set LED @id on/off
// - usage: bl_led_set(bl_in,id,val)   // val 0:off, 1:on
//==============================================================================

  static inline int bl_led_set(BL_oval module,int id, int val)
  {
    BL_ob oo = {_LED,BL_HASH(SET_),id,NULL};
    return module(&oo,val);
  }

//==============================================================================
// syntactic sugar: togggle LED @id (@id: 0..4)
// - usage: bl_led_toggle(bl_in,id)
//==============================================================================

  static inline int bl_led_toggle(BL_oval module, int id)
  {
    BL_ob oo = {_LED,BL_HASH(TOGGLE_),id,NULL};
    return bl_in(&oo,0);
  }

//==============================================================================
// syntactic sugar: check if message is a button press message ([BUTTON:PRESS])
// - usage: pressed = bl_pressed(o)
//==============================================================================

  static inline bool bl_pressed(BL_ob *o)
  {
    return  bl_is(o,_BUTTON,PRESS_);
  }

//==============================================================================
// syntactic sugar: check if message is a button release msg ([BUTTON:RELEASE])
// - usage: released = bl_released(o)
//==============================================================================

  static inline bool bl_released(BL_ob *o)
  {
    return  bl_is(o,_BUTTON,RELEASE_);
  }

//==============================================================================
// syntactic sugar: check if message is a switch status update ([SWITCH:STS])
// - usage: switched = bl_switched(o)
//==============================================================================

  static inline bool bl_switched(BL_ob *o)
  {
    return  bl_is(o,_SWITCH,STS_);
  }

#endif // __BL_HW_H__
