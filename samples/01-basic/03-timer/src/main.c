//==============================================================================
// main.c for 02-timer demo
//==============================================================================

#include "bluccino.h"
#include "timer.h"

//==============================================================================
// when callback
//==============================================================================

  int when(BL_ob *o, int val)
  {
    switch (o->op)
    {
      case OP_TICK:
        bl_logo(1,BL_G"when",o,val);
        return 0;
      default:
        return -1;
    }
  }

//==============================================================================
// mandatory init/loop functions for app
//==============================================================================

  static void init(void)
  {
  }

  static void loop(void)               // app loop
  {
     bl_sleep(10);                     // sleep 10 ms
  }

//==============================================================================
// framework engine
//==============================================================================

  void main(void)
  {
    bl_verbose(2);                     // set verbose mode
    bl_init(NULL,NULL);                // Bluccino init (no callback passing)
    bl_init(timer,when);               // TIMER init (pass 'when' callback)
    init();                            // app init

    for(;;)
    {
      bl_loop(NULL);                   // run Bluccino loop
      bl_loop(timer);                  // run TIMER loop
      loop();                          // run app loop
    }
  }
