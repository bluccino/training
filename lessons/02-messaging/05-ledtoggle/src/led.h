//==============================================================================
// led.h
//==============================================================================

#ifndef __LED_H__
#define __LED_H__

//==============================================================================
// public module interface
//==============================================================================
//
// (A) := (APP);
//
//                  +--------------------+
//                  |        LED         |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (A)->     INIT ->|       <out>        | init module, ignore <out> callback
//                  +--------------------+
//                  |        LED:        | LED interface
// (A)->      SET ->|      @1,onoff      | set LED's onoff state
//                  +--------------------+
//
//==============================================================================

  int led(BL_ob *o, int val);          // public module interface

//==============================================================================
// syntactic sugar: set LED's on/off state (only one LED)
// - usage: led_set(led,onoff)         // (LED)<-[LED:SET onoff]
//==============================================================================

  static inline int led_set(BL_oval module, bool onoff)
  {
    BL_ob oo = {BL_AUG(_LED),SET_,1,NULL};
    return module(&oo,onoff);          // post (<module>)<-[LED:SET onoff]
  }

#endif // __LED_H__
