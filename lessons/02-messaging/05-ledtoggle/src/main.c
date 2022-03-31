//==============================================================================
// main.c for 05-ledtoggle (simple message flow)
//==============================================================================

  #include "main.h"                    // must be included before "bluccino.h"
  #include "bluccino.h"                // access Bluccino stuff
  #include "button.h"
  #include "led.h"

//==============================================================================
// public APP module interface
//==============================================================================
//
// (M) := (MAIN);  (#) := (APP);  (B) := (BUTTON);  (L) := (LED);
//
//                  +--------------------+
//                  |        APP         | APP module
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (M)->     INIT ->|       <out>        | init module, ignore <out> callback
//                  +--------------------+
//                  |      BUTTON:       | BUTTON interface
// (B)->    PRESS ->|                    | button pressed
//                  +--------------------+
//                  |        LED:        | LED interface
// (L)<-      SET <-|       onoff        | set LED's on/off state
//                  +====================+
//                  |       #LED:        | private LED interface
// (#)->      SET ->|       onoff        | set LED's on/off state
//                  +--------------------+
//
//==============================================================================

  int app(BL_ob *o, int val)           // APP's OVAL interface
  {
    static bool onoff = 0;             // LED onoff state
    static BL_oval L = led;            // (L) output, goes to LED

    switch(bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <out>], ignore <out>
	bl_log(2,BL_C "init app");
        bl_init(button,app);           // init BUTTON, output goes to app
        bl_init(led,NULL);             // init LED, output goes to nowhere
        return 0;                      // OK

      case BL_ID(_BUTTON,PRESS_):      // [SYS:INIT <out>], ignore <out>
			  bl_logo(1,BL_M "app:",o,val);  // log event message
        onoff = !onoff;                // toggle LED onoff state
        return led_set(app,onoff);     // post message (APP)<-[LED:SET onoff]

        // private message interface

      case _BL_ID(_LED,SET_):          // [#LED:SET toggle]
	bl_logo(1,BL_M "app:",o,val);  // log event message
        return bl_out(o,val,L);        // post message (L)<-[LED:SET toggle]

      default:
        return 0;                      // OK
    }
  }

//==============================================================================
// main function
// - sets verbose level, prints hello message
//==============================================================================

  void main(void)
  {
    bl_hello(4,"05-ledtoggle - toggling an LED by button press events");
    bl_init(app,NULL);
  }
