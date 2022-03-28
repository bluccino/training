//==============================================================================
// main.c for 04-rgbcycle (tiny RGB LED cycle control by buttons)
//==============================================================================

  #include "bluccino.h"

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
// button worker (note: OVAL interface!)
//==============================================================================

  static int button(BL_ob *o, int val) // [BUTTON:PRESS] worker function
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
//                  |       BUTTON:      | BUTTON: output interface
// (B)->    PRESS ->|       @id,sts      | button @id pressed (rising edge)
//                  +--------------------+
//
//==============================================================================

  int app(BL_ob *o, int val)           // app module (with OVAL interface)
  {
    switch (bl_id(o))
    {
      case BL_ID(_BUTTON,PRESS_):      // [BUTTON:PRESS @id,sts]
        return button(o,val);          // delegate to button worker

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
    bl_hello(4,"06-rgbcycle (click any button to activate another LED");
    bl_init(bluccino,app);             // run - output goes to app()
  }
