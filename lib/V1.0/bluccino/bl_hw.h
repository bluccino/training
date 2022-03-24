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

  int bl_hw(BL_ob *o, int val);        // HW core module interface

//==============================================================================
// syntactic sugar: HW core init
// - usage: bl_hw_init(cb)
//==============================================================================

  static inline int bl_hw_init(BL_fct cb)
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
    return bl_down(&oo,val<0?0:val);
  }

//==============================================================================
// syntactic sugar: check if message is a button press message ([BUTTON:PRESS])
// - usage: pressed = bl_press(o)
//==============================================================================

  static inline bool bl_press(BL_ob *o)
  {
    return  bl_is(o,_BUTTON,PRESS_);
  }

//==============================================================================
// syntactic sugar: check if message is a button release msg ([BUTTON:RELEASE])
// - usage: released = bl_released(o)
//==============================================================================

  static inline bool bl_release(BL_ob *o)
  {
    return  bl_is(o,_BUTTON,RELEASE_);
  }

//==============================================================================
// syntactic sugar: check if message is a switch status update ([SWITCH:STS])
// - usage: switched = bl_switch(o)
//==============================================================================

  static inline bool bl_switch(BL_ob *o)
  {
    return  bl_is(o,_SWITCH,STS_);
  }

#endif // __BL_HW_H__
