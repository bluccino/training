//==============================================================================
// bl_onebut.h
// Bluccino button driver supporting button basic functions
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================
// BUTTON interface
// - button presses notify with [BUTTON:PRESS @id 1] with @id = 1..4
// - button releases notify with [BUTTON:RELESE @id 0] with @id = 1..4
//==============================================================================

#ifndef __BL_ONEBUT_H__
#define __BL_ONEBUT_H__

//==============================================================================
// public module interface
//==============================================================================
//
// (!) := (<parent>); (O) := (<out>); (#) := (BL_HW)
//
//                  +--------------------+
//                  |       BL_HW        |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (!)->     INIT ->|       <out>        | init module, store <out> callback
//                  +--------------------+
//                  |       BUTTON:      | BUTTON interface
// (O)<-    PRESS <-|       @1,sts       | output button press event
// (O)<-  RELEASE <-|       @1,sts       | output button release event
//                  +====================+
//                  |      #BUTTON:      | Private BUTTON interface
// (#)->    PRESS ->|       @1,sts       | trigger button press event
// (#)->  RELEASE ->|       @1,sts       | trigger button release event
//                  +--------------------+
//
//==============================================================================

  int bl_onebut(BL_ob *o, int val);    // button module interface

//==============================================================================
// syntactic sugar: init onebut module
// - usage: ob_sys_init(cb)
//==============================================================================

  static inline int ob_sys_init(BL_oval cb)
  {
    return bl_init(bl_onebut,cb);      // init BL_ONEBUT, output goes to cb
  }

//==============================================================================
// syntactic sugar: pseudo-invoke button press event (@id:1 - only one button)
// - usage: ob_button_press(bl_onebut)
//==============================================================================

  static inline int ob_button_press(BL_oval module)
  {
    BL_ob oo = {BL_AUG(_BUTTON),PRESS_,1,NULL};
    return module(&oo,1);                // pass val=1 to indicate 'active'
  }

//==============================================================================
// syntactic sugar: pseudo-invoke button release event (@id:1 - only one button)
// - usage: ob_button_release(bl_onebut)
//==============================================================================

  static inline int bl_button_release(BL_oval module)
  {
    BL_ob oo = {BL_AUG(_BUTTON),RELEASE_,1,NULL};
    return module(&oo,0);                // pass val=0 to indicate 'inactive'
  }

#endif // __BL_ONEBUT_H__
