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
// BL_HWBUT Interfaces:
//   SYS Interface:     [] = SYS(INIT)
//   BUTTON Interface:  [PRESS,RELEASE] = BUTTON(PRESS,RELEASE)
//   SWITCH Interface:  [STS] = SWITCH(STS)
//   SET Interface:     [] = SET(MS)
//
//                             +-------------+
//                             |  BL_HWBUT   |
//                             +-------------+
//                      INIT ->|    SYS:     |
//                      TICK ->|             |
//                             +-------------+
//                    #PRESS ->|   BUTTON:   |-> PRESS
//                  #RELEASE ->|             |-> RELEASE
//                    #CLICK ->|             |-> CLICK
//                     #HOLD ->|             |-> HOLD
//                             +-------------+
//                       STS ->|   SWITCH:   |-> STS
//                             +-------------+
//                        MS ->|    SET:     |
//                             +-------------+
//  Input Messages:
//   - [SYS:INIT <cb>]              init module, provide output callback
//   - [SYS:TICK @id,cnt]           tick module (for click/long detection)
//   - [BUTTON:PRESS @id,active]    forward button press event to output
//   - [BUTTON:RELEASE @id,active]  forward button release event to output
//   - [SWITCH:STS @id,onoff]       forward switch status update to output
//   - [SET:MS ms]                  set click grace time (default: 350 ms)
//
//  Output Messages:
//   - [BUTTON:PRESS @id 1]         output a button press event
//   - [BUTTON:RELEASE @id,time]    button release event received from driver
//                                  note: time is PRESS->RELEASE elapsed time
//   - [SWITCH:STS @id,onoff]       output switch status update
//
//==============================================================================

  int bl_hwbut(BL_ob *o, int val);    // button module interface

//==============================================================================
// syntactic sugar: HW core init
// - usage: bc_hwbut_init(cb)
//==============================================================================

  static inline int bl_hwbut_init(BL_fct cb)
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
