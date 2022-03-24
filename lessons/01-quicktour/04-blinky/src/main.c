//==============================================================================
// main.c for 04-blinky (small blink program with 4 LEDs involved)
//==============================================================================

  #include "bluccino.h"

  static int app(BL_ob *o, int val)    // app function (Bluccino OVAL interface)
  {
    static int id = 4;

    switch (bl_id(o))                  // dispatch event message
    {
      case BL_ID(_SYS,TOCK_):          // every 500ms tock
        bl_led(id,0);                  // set LED @id off
        id = 1 + (id % 4);             // next id (4 -> 1->2->3->4 -> 1->...)
        bl_led(id,1);                  // set LED @id on
        return 0;                      // OK

      default:
        return -1;                     // bad args
    }
  }

  void main(void)
  {
    bl_hello(5,"03-binky");            // print hello message, set verbose level
    bl_run(app,10,500,NULL);           // run engine @ 10/500ms tick/tock timing
  }
