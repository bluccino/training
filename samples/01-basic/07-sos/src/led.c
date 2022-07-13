//==============================================================================
// led.c - an LED dummy block
//==============================================================================

#include "bluccino.h"
#include "led.h"

  static BL_ob oo = {CL_LED,OP_SET,0,NULL};

//==============================================================================
// THE led interface function
//==============================================================================

  int led(BL_ob *o, int val)
  {
    switch (o->op)
    {
      case OP_INIT:
        oo.id = o->id + 1;                  // store (id+1) in message obj's id
        return 0;

      case OP_LEVEL:
        bl_logo(2,BL_G "led",o,val);
        bl_down(&oo,val);                   // send level (down) to LED driver
        return 0;

      default:
        return -1;                          // bad command
    }
  }
