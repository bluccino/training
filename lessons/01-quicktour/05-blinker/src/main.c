//==============================================================================
// main.c for 05-blinker (small blink program with 4 LEDs involved)
//==============================================================================

  #include "bluccino.h"

  static int id;                       // id of active LED

//==============================================================================
// helper: next ID
//==============================================================================

  static int next(int id)              // get next ID (cycle through RGB LEDs)
  {
    return (1 + (id % 4));             // next id (4 -> 1->2->3->4 -> 1->...)
  }

//==============================================================================
// tock worker (note: OVAL interface!)
//==============================================================================

  static int tock(BL_ob *o, int val)   // [SYS:TOCK] worker
  {
    bl_logo(1,"app:",o,val);           // log [SYS:TOCK @id,cnt] message

    bl_led(id,0);                      // set LED @id off
    id = next(id);                     // next id (4 -> 1->2->3->4 -> 1->...)
    bl_led(id,1);                      // set LED @id on
    return 0;
  }

//==============================================================================
// init worker (note: OVAL interface!)
//==============================================================================

  static int init(BL_ob *o, int val)   // [SYS:INIT] worker
  {
    bl_logo(1,"app:",o,val);           // log [SYS:INIT <out>>] message

    id = 4;                            // LED id = 4, next one is 1
    return 0;
  }

//==============================================================================
// public APP module interface (note: OVAL interface!)
// - APP module dispatches only [BUTTON:PRESS sts] events
// - the switch() dispatcher here can be treated as THE Bluccino standard
//==============================================================================
//
// (M) := (MAIN)
//                  +--------------------+
//                  |        APP         |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (M)->     INIT ->|       <out>        | init module, ignore <out> arg
// (M)->     TOCK ->|       @id,cnt      | receive system tocks (500ms period)
//                  +--------------------+
//
//==============================================================================

  static int app(BL_ob *o, int val)    // app function (Bluccino OVAL interface)
  {
    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <out>] init module
        return init(o,val);            // delegate to init() worker

      case BL_ID(_SYS,TOCK_):          // [SYS:TOCK @id,cnt] every 500ms tock
        return tock(o,val);            // delegate to tock() worker

      default:
        return -1;                     // message ignored
    }
  }

//==============================================================================
// main function
// - set verbose level and print hello message
// - init bluccino module with all event output going to app() module
//==============================================================================

  void main(void)
  {
    bl_hello(4,"05-binker");           // print hello message, set verbose level
    bl_init(bluccino,app);             // run - output goes to app()
    bl_init(app,NULL);                 // init APP, output goes nowhere

    for (int cnt=0;;cnt++)
    {
      bl_tock(app,0,cnt);              // send [SYS:TOCK @0,cnt] messages to APP
      bl_sleep(500);                   // sleep 500ms
    }
  }
