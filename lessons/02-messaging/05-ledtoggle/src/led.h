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

#endif // __LED_H__
