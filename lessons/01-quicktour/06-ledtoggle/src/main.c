//==============================================================================
// main.c for 06-ledtoggle (tiny LED toggle control by buttons)
//==============================================================================
// App Description
// - all LEDs are off at the beginning
// - any button press activites blinking of first LED for 5s
// - any further button press activates blinking of next LED for 10s
//
// Lessons to Learn
// - learn how to setup a standard Bluccino framework, reacting on standard
//   timing events (tick/tock), button events and controlling the board's LEDs
// - demonstration of role of main() - Bluccino main()'s have usually 2-3 lines
// - show standard implementation of an APP module with dispatcher and workers
// - demonstrate how module interfaces (of app) should be documented
// - demonstrate how worker functions should be documented
//==============================================================================

  #include "bluccino.h"
  #include "bl_hw.h"

  static int id = -1;                  // id of blinking LED (id=-1: invalid)
  static bool enable = false;          // blinking enabled?
  static BL_ms due = 0;                // due time for turning off blinker

//==============================================================================
// button worker (handles [BUTTON:PRESS] events)
// - turns off current selected LED @id
// - cycle increments @id (1->2->3->4 -> 1->2->...) to select next LED @id
// - enable blinking of selected LED @id, and setting up a timeout for blinking
//==============================================================================

  static int button(BL_ob *o, int val) // handle [BUTTON:PRESS @id] message
  {
    bl_led(id,0);                      // turn off current LED @id
    id = 1 + (id  % 4);                // cycle id through 1,2,3,4 -> 1,2,...
    due = bl_ms() + 5000;              // due at time (now + 5s)
    enable = true;                     // blinker enabled
    return 0;                          // OK
  }

//==============================================================================
// tick worker (handles [SYS:TICK] events) - tick() is called every 10ms
// - is passive if current LED @id is zero
// - otherwise toggles LED every 80-th tick (every 800ms, as tick period = 10ms)
//==============================================================================

  static int tick(BL_ob *o, int val)   // handle [SYS:TICK @id,cnt] message
  {
    if (enable && bl_ms() >= due)      // if blinker is enabled and we are due
    {
      bl_led(id,0);                    // turn off current LED @id
      enable = false;                  // disable blinker
    }
    return 0;                          // OK
  }

//==============================================================================
// tock worker (handles [SYS:TOCK] events)
// - behaves passive if blinking not enabled (enable=false)
// - otherwise toggles LED every 5-th tock (every 500ms, as tock period = 100ms)
//==============================================================================

  static int tock(BL_ob *o, int val)   // handle [SYS:TOCK @id,cnt] message
  {
    if (val % 5 == 0 && enable)        // every 5-th tick (500ms), if enabled
      bl_led(id,-1);                   // toggle selected LED
    return 0;                          // OK
  }

//==============================================================================
// init worker (handles [SYS:INIT <out>] events)
// - prints a log so we can cross check whether APP module is initialized
// - init id with value 0 (next LED selection to be @1) (note: blinker disabled)
// - <out> argument is ignored, since APP does not emit messages
//==============================================================================

  static int init(BL_ob *o, int val)   // handle [SYS:INIT <out>] message
  {
    bl_log(2,BL_B "init app");         // event message log in blue
    id = 0;                            // init LED @id=0 => starts with LED @1
    bl_log(1,"=> click button to cycle to next LED to be toggled!");
    return 0;                          // OK
  }

//==============================================================================
// public module interface
//==============================================================================
//
// (E) := (BL_ENGINE);
//                  +--------------------+
//                  |        APP         |
//                  +--------------------+
//                  |        SYS:        | SYS: interface
// (E)->     INIT ->|       <out>        | init module, store <out> callback
// (E)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |       BUTTON:      | BUTTON: output interface
// (^)->    PRESS ->|        @id,1       | button @id pressed (rising edge)
//                  +--------------------+
//
//==============================================================================

  int app(BL_ob *o, int val)           // app receiving event messages
  {
    switch (bl_id(o))                  // dispatch message ID
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <out>] => init module
        return init(o,val);            // forward to init() worker

      case BL_ID(_SYS,TICK_):          // [SYS:TICK @id,cnt] => handle sys tick
        return tick(o,val);            // forward to tick() worker

      case BL_ID(_SYS,TOCK_):          // [SYS:TOCK @id,cnt] => handle sys tick
        return tock(o,val);            // forward to tock() worker

      case BL_ID(_BUTTON,PRESS_):      // [BUTTON:PRESS @id] => putton pressed
        return button(o,val);          // forward to button() worker

      default:
        return 0;                      // OK - ignore anything else
    }
  }

//==============================================================================
// main program
// - sets verbose level, prints hello message
// - and runs app with init/tick/tock using tick/tock/engine
//==============================================================================

  void main(void)
  {
    bl_hello(2,"06-ledtoggle (click button to cycle to next LED to be toggled)");
    bl_cfg(bl_down,_BUTTON,BL_PRESS);  // configure only [BUTTON:PRESS] events 
    bl_engine(app,10,100);             // run APP with 10/100ms ticks/tocks
  }
