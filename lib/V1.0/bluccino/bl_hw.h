//==============================================================================
// bl_hw.h
// Bluccino HW core supporting basic functions for button & LED
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================
//
// LED interface:
// - LED messages [LED:SET @id onoff] control the onoff state of one of the four
// - LEDs @1..@4. LED @0 is the status LED which will be remapped to LED @1
//
// BUTTON interface
// - button presses notify with [BUTTON:PRESS @id 1]   (@id = 1..4)
// - button releases notify with [BUTTON:RELESE @id 1] (@id = 1..4)
//
// SWITCH interface
// - each button (1..4) is assigned with a logical switch which is toggled
//   on [BUTTON:PRESS @id,val] events
// - each change of the logical switch state is notified by a
//   [SWITCH:SET @id,onoff] event message
//
//==============================================================================

#ifndef __BL_HW_H__
#define __BL_HW_H__

//==============================================================================
// public module interface
//==============================================================================
//
// BC_HW Interfaces:
//   SYS Interface:     [] = SYS(INIT)
//   LED Interface:     [] = LED(SET,TOGGLE)
//   BUTTON Interface:  [PRESS,RELEASE] = BUTTON(PRESS,RELEASE)
//   SWITCH Interface:  [STS] = SWITCH(STS)
//
//                             +-------------+
//                             |    BL_HW    |
//                             +-------------+
//                      INIT ->|    SYS:     |
//                      TICK ->|             |
//                             +-------------+
//                       SET ->|    LED:     |
//                    TOGGLE ->|             |
//                             +-------------+
//                     PRESS ->|   BUTTON:   |-> PRESS
//                   RELEASE ->|             |-> RELEASE
//==============================================================================
// public module interface
//==============================================================================
//
// BC_HW Interfaces:
//   SYS Interface:     [] = SYS(INIT)
//   LED Interface:     [] = LED(SET,TOGGLE)
//   BUTTON Interface:  [PRESS,RELEASE] = BUTTON(PRESS,RELEASE)
//   SWITCH Interface:  [STS] = SWITCH(STS)
//
//                             +-------------+
//                             |    BL_HW    |
//                             +-------------+
//                      INIT ->|    SYS:     |
//                      TICK ->|             |
//                             +-------------+
//                       SET ->|    LED:     |
//                    TOGGLE ->|             |
//                             +-------------+
//                     PRESS ->|   BUTTON:   |-> PRESS
//                   RELEASE ->|             |-> RELEASE
//                     CLICK ->|             |-> CLICK
//                      HOLD ->|             |-> HOLD
//                             +-------------+
//                       STS ->|   SWITCH:   |-> STS
//                             +-------------+
//  Input Messages:
//    - [SYS:INIT <cb>]              init module, provide output callback
//    - [SYS:TICK @id,cnt]           tick module
//    - [LED:SET @id onoff]          set LED @id on/off (id=0..4)
//    - [LED:TOGGLE @id]             toggle LED(@id), (id: 0..4)
//    - [BUTTON:PRESS @id,0]         forward button press event to output
//    - [BUTTON:RELEASE @id,time]    forward button release event to output
//                                   note: time is PRESS->RELEASE elapsed time
//    - [SWITCH:STS @id,onoff]       forward switch status update to output
//
//  Output Messages:
//    - [BUTTON:PRESS @id,1]         output button press event
//    - [BUTTON:RELEASE @id,0]       output button release event
//    - [SWITCH:STS @id,onoff]       output switch status update
//
//==============================================================================
//                             +-------------+
//                       STS ->|   SWITCH:   |-> STS
//                             +-------------+
//  Input Messages:
//    - [SYS:INIT <cb>]              init module, provide output callback
//    - [SYS:TICK @id,cnt]           tick module
//    - [LED:SET @id onoff]          set LED @id on/off (id=0..4)
//    - [LED:TOGGLE @id]             toggle LED(@id), (id: 0..4)
//    - [BUTTON:PRESS @id,0]         forward button press event to output
//    - [BUTTON:RELEASE @id,time]    forward button release event to output
//                                   note: time is PRESS->RELEASE elapsed time
//    - [SWITCH:STS @id,onoff]       forward switch status update to output
//
//  Output Messages:
//    - [BUTTON:PRESS @id,1]         output button press event
//    - [BUTTON:RELEASE @id,0]       output button release event
//    - [SWITCH:STS @id,onoff]       output switch status update
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
