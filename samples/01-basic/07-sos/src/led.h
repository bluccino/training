//==============================================================================
// led.h - an LED dummy block
//==============================================================================
//
//  accepts binary LEVEL signals wich cause LED to turn om or off
//
//  LED interface: [] = LED(INIT,LEVEL)
//
//                               +-------------+
//                        INIT -->             |
//                               |     LED     |
//                       LEVEL -->             |
//                               +-------------+
//
//  Input Messages:
//    - [LED:INIT @id <cb>] init id, assign LED channel and subscriber callback
//    - [LED:LEVEL level]   binary input to switch LED on or off
//
//==============================================================================

#ifndef __LED_H__
#define __LED_H__

//==============================================================================
// public module interface
//==============================================================================

  int  led(BL_ob *o, int value);       // public module interface

//==============================================================================
// syntactic sugar: LED module init, provide @id & callback [LED:INIT @id <cb>]
// usage: led_init(id,cb)
//==============================================================================

  inline static int led_init(int id)
  {
    BL_ob oo = {CL_LED,OP_INIT, id, NULL};
    return led(&oo,0);                 // init LED module, provide id & callback
  }

//==============================================================================
// syntactic sugar: set LED level [LED:LEVEL level]
// usage: led_level()
//==============================================================================

  inline static int led_level(int level)
  {
    BL_ob oo = {CL_LED,OP_LEVEL, 0, NULL};
    return led(&oo,level);             // binary input to switch LED on or off
  }

#endif // __LED_H__
