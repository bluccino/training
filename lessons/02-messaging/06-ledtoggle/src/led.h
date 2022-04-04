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
// - usage: led_LED_SET(led,onoff)     // (LED)<-[LED:SET onoff]
//==============================================================================

  static inline int led_LED_SET(BL_oval module, bool onoff)
  {
		return bl_msg(module,_LED,SET_, 1,NULL,onoff);
  }

#endif // __LED_H__
