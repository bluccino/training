//==============================================================================
// main.c for 04-rgbcycle (tiny RGB LED cycle control by buttons)
//==============================================================================

  #include "bluccino.h"                // access to Bluccino stuff
  #include "bl_hw.h"                   // access to hardware stuff

  int app(BL_ob *o, int val);          // forward declaration

    // define RGB color index @id (note: id=5 means invalid)

  static int id = 5;                   // RGB color index (R:@2, G:@3, B:@4)

//==============================================================================
// helper: next ID
//==============================================================================

  static int next(int id)              // get next ID (cycle through RGB LEDs)
  {
    return (id >= 5) ? 2 : id+1;       // next RGB color (R:@2, G:@3, B:@4)
  }

//==============================================================================
// worker: button pressed (note: OVAL interface!)
//==============================================================================

  static int button_press(BL_ob *o, int val)
  {
    bl_logo(1,"app:",o,val);           // log [BUTTON:PRESS @id,sts] message

    bl_led(id,0);                      // set LED(@id) off
    id = next(id);                     // select next ID
    bl_led(id,1);                      // set LED(@id) on
    return 0;
  }

//==============================================================================
// public APP module interface (note: OVAL interface!)
// - APP module dispatches only [BUTTON:PRESS sts] events
// - the switch() dispatcher here can be treated as THE Bluccino standard
//==============================================================================
//
// (B) := (BLUCCINO)
//                  +--------------------+
//                  |        APP         |
//                  +--------------------+
//                  |       BUTTON:      | BUTTON input interface
// (B)->    PRESS ->|        @id         | button @id pressed (rising edge)
//                  +--------------------+
//
//==============================================================================

  int app(BL_ob *o, int val)           // app module (with OVAL interface)
  {
    switch (bl_id(o))
    {
      case BUTTON_PRESS_id_0_0:        // [BUTTON:PRESS @id]
        return button_press(o,val);    // delegate to button_press worker

      default:
        return -1;                     // message not dispatched
    }
  }

//==============================================================================
// main function
// - set verbose level and print hello message
// - init bluccino module with all event output going to app() module
//==============================================================================

  void main(void)
  {
    bl_hello(4,PROJECT " (click any button to activate another LED)");
    bl_cfg(bl_down,_BUTTON,BL_PRESS);  // only want get [BUTTON:PRESS] events
    bl_init(bluccino,app);             // run - output goes to app()
  }
