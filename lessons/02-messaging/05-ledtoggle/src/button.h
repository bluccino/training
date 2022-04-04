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

#endif // __BUTTON_H__
