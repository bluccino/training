//==============================================================================
// timer.c - a simple timer module
//==============================================================================

#include "bluccino.h"
#include "timer.h"

  static BL_fct notify = NULL;
  static BL_ms time = 0;

    // timer messaging object

  static BL_ob tim = {CL_TIMER,OP_TICK,0,NULL};

//==============================================================================
// public interface
//==============================================================================

  int timer(BL_ob *o, int val)
  {
    switch (o->op)
    {
      case OP_INIT:
        bl_logo(1,BL_B"timer",o,val);
        notify = o->data;
        return 0;

      case OP_LOOP:
        if (bl_ms() >= time)
        {
          bl_logo(3,"loop",&tim,time);
          bl_out(&tim,time,notify);        // emit tick message
          time += 1000;                    // next time = current time + 1000 ms
        }
        return 0;

      default:
        return 0;
    }
  }
