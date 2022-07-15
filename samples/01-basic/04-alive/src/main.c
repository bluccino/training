//==============================================================================
// main.c for 03-alive demo
//==============================================================================

#include "bluccino.h"

  BL_txt msg;

//==============================================================================
// app init and app loop
//==============================================================================

  static void init(void)               // app init
  {
    msg = BL_Y"I'm alive!"BL_0;
  }

  static void loop(void)               // app loop
  {
    static int n = 1;                  // mind: must be static!

    if (bl_dbg(1))                     // this is to print the timing header
      bl_prt("[%03d] %s\n",n++,msg);   // print inviting message

    bl_sleep(1000);                    // sleep 1000 ms
  }

//==============================================================================
// Arduino style engine
//==============================================================================

  void main(void)
  {
    bl_init(NULL,NULL,1);              // Bluccino init (no callback passing)
    init();                            // app init

    for(;;)
    {
      bl_loop(NULL);                   // run Bluccino loop
      loop();                          // run app loop
    }
  }
