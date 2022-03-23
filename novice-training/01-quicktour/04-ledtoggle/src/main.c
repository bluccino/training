//==============================================================================
// main.c for 04-ledtoggle (tiny LED toggle control by buttons)
//==============================================================================

  #include "bluccino.h"

  static int when(BL_ob *o, int val)   // callback, catching Bluccino events
  {                                    // o->id represents button @id
    if ( bl_switch(o) )                // [SWITCH:STS @id,val] message?
      bl_led(o->id,val);               // set LED(@id) on/off
    return 0;                          // OK
  }

  void main(void)
  {
    bl_hello(5,"03-ledtoggle (click button to toggle related LED");
    bl_run(NULL,10,1000,when);         // run Bluccino engine, output => <when>
  }
