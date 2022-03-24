//==============================================================================
// main.c for 05-ledtoggle (tiny LED toggle control by buttons)
//==============================================================================

  #include "bluccino.h"

  static int app(BL_ob *o, int val)    // app receiving event messages
  {
    switch (bl_id(o))                  // dispatch message ID
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <out>] message?
        bl_logo(1,BL_B,o,val);         // event message log in blue
    
      case BL_ID(_SYS,TOCK_):          // [SYS:TOCK @id,cnt] message?
        bl_logo(1,BL_G,o,val);         // event message log in green
    
      case BL_ID(_SWITCH,STS_):        // [SWITCH:STS @id,val] message?
        bl_led(o->id,val);             // set LED(@id) on/off

      default:
        return 0;                      // OK
    }
  }

  void main(void)
  {
    bl_hello(5,"05-ledtoggle (click button to toggle related LED");
    bl_run(app,10,1000,app);           // run Bluccino engine, output => APP
  }
