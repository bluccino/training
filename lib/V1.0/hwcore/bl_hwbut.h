//==============================================================================
// bl_hwbut.h
// Bluccino button driver supporting button basic functions
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================
// BUTTON interface
// - button presses notify with [BUTTON:PRESS @id 1] with @id = 1..4
// - button releases notify with [BUTTON:RELEASE @id 0] with @id = 1..4
//
// SWITCH interface
// - each button (1..4) is assigned with a logical switch which is toggled
//   on [BUTTON:PRESS @id,sts] events
// - each change of the logical switch state is notified by a
//   [SWITCH:SET @id,onoff] event message
//==============================================================================

#ifndef __BL_HWBUT_H__
#define __BL_HWBUT_H__

//==============================================================================
// public module interface
//==============================================================================
//
// (!) := (<parent>);  (#) := BL_HWBUT;  (^) := (BL_UP);  (v) := (BL_DOWN);
//
//                  +--------------------+
//                  |      BL_HWBUT      | module BL_HWBUT
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (!)->     INIT ->|       <out>        | init module, ignore <out> callback
// (!)->     TICK ->|       @id,cnt      | tick module
// (!)->      CFG ->|        mask        | config module
//                  +--------------------+
//                  |       BUTTON:      | BUTTON interface
// (^)<-    PRESS <-|        @id,0       | button press at time 0
// (^)<-  RELEASE <-|        @id,ms      | button release after elapsed ms-time
// (^)<-    CLICK <-|        @id,n       | number of button clicks
// (^)<-     HOLD <-|       @id,ms       | button hold event at ms-time
//                  +--------------------+
//                  |       SWITCH:      | SWITCH interface
// (^)<-      STS <-|       @id,sts      | emit status of toggle switch event
//                  +--------------------+
//                  |        SET:        | SET interface
// (v)->       MS ->|         ms         | set grace time for click/hold events
//                  +--------------------+
//
//==============================================================================

  int bl_hwbut(BL_ob *o, int val);    // button module interface

//==============================================================================
// syntactic sugar: HW core init
// - usage: bc_hwbut_init(cb)
//==============================================================================

  static inline int bl_hwbut_init(BL_oval cb)
  {
    return bl_init(bl_hwbut,cb);
  }

//==============================================================================
// syntactic sugar: pseudo-invoke button press event (@id: 0..4)
// - usage: bl_hwbut_press(id)
//==============================================================================

  static inline int bl_hwbut_press(int id)
  {
    BL_ob oo = {_BUTTON,PRESS_,id,NULL};
    return bl_hwbut(&oo,1);                // pass 1 to indicate 'active'
  }

//==============================================================================
// syntactic sugar: pseudo-invoke button release event (@id: 0..4)
// - usage: bl_hwbut_release(id)
//==============================================================================

  static inline int bl_hwbut_release(int id)
  {
    BL_ob oo = {_BUTTON,RELEASE_,id,NULL};
    return bl_hwbut(&oo,0);                // pass 0 to indicate 'inactive'
  }

//==============================================================================
// syntactic sugar: set click grace time (ms), default: 350 ms
// - usage: bl_hwbut_ms(ms)   // ms discriminates between :CLICK and :HOLD event
//==============================================================================

  static inline int bl_hwbut_ms(int ms)
  {
    return bl_set(bl_hwbut,MS_,ms);        // set click grace time
  }

#endif // __BL_HWBUT_H__
