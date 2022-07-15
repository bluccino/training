//==============================================================================
// sos.h - an SOS sequencer block
//==============================================================================
//
//  generate a sequence of boolean 0's and 1's at output LEVELs to control an
//  LED with an SOS signal (3x short, 3x long, 3x short, pause, and repeat ...)
//
//  SOS interface: [LEVEL] = SOS(INIT,TICK)
//
//                               +-------------+
//                        INIT -->             |
//                               |     SOS     >-- LEVEL
//                        TICK -->             |
// LEVEL                        +-------------+
//  ^
//  |== +== +==   +======     +======     +======   +== +== +==        +==
//  | | | | | |   |     |     |     |     |     |   | | | | | |        | |
// -+-+==-+==-+====-----+======-----+======-----+====-+==-+==-+===...===-+=...>
//
//  Message input:
//    - [SOS:INIT <cb>]      init module, provide callback
//    - [SOS:TICK count]     ticking the module (defines pace of output pattern)
//
//  Message output:
//    - [SOS:LEVEL]          boolean SOS pattern (sequence of 0's and 1's)
//
//==============================================================================

#ifndef __SOS_H__
#define __SOS_H__

//==============================================================================
// public module interface
//==============================================================================

  int  sos(BL_ob *o, int value);       // public module interface

//==============================================================================
// syntactic sugar: SOS module init & provide callback [SOS:INIT <cb>]
// usage: tst_led_init()
//==============================================================================

  inline static int sos_init(BL_fct cb)
  {
    BL_ob oo = {CL_SOS,OP_INIT, 0, cb};
    return sos(&oo,0);                 // init SOS module, provide callback
  }

//==============================================================================
// syntactic sugar: ticking the SOS module [SOS:TICK]
// usage: sos_tick()
//==============================================================================

  inline static int sos_tick(int count)
  {
    BL_ob oo = {CL_SOS,OP_TICK,0,NULL};
    return sos(&oo,count);             // [SOS:TICK val]
  }

#endif // __SOS_H__
