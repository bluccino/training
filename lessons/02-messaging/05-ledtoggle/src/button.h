//==============================================================================
// button.h
//==============================================================================

#ifndef __BUTTON_H__
#define __BUTTON_H__

//==============================================================================
// public module interface
//==============================================================================
//
// (!) := (<parent>); (O) := (<out>); (#) := (BL_HW)
//
//                  +--------------------+
//                  |       BUTTON       |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (!)->     INIT ->|       <out>        | init module, store <out> callback
//                  +--------------------+
//                  |       BUTTON:      | BUTTON interface
// (O)<-    PRESS <-|       @1,sts       | output button press event
//                  +--------------------+
//
//==============================================================================

  int button(BL_ob *o, int val);       // button module interface

//==============================================================================
// syntactic sugar: pseudo-invoke button press event (@id:1 - only one button)
// - usage: button_press(button)
//==============================================================================

  static inline int button_press(BL_oval module)
  {
    BL_ob oo = {BL_AUG(_BUTTON),PRESS_,1,NULL};
    return module(&oo,1);                // pass val=1 to indicate 'active'
  }

#endif // __BUTTON_H__
