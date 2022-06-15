//==============================================================================
// main.c
// main program for 06-meshnode (mesh node house keeping)
//
// Created by Hugo Pristauz on 2022-Jan-04
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================
//
// Event Flow in unprovisioned state ("device" state)
// - any button press toggles (local) LED @2 (red LED on 52840 dongle)
//
//     +-----------+
//     | BUTTON @1 |---+
//     +-----------+   |
//                     |
//     +-----------+   |
//     | BUTTON @2 |---+
//     +-----------+   |                                        +--------+
//                     +--------------------------------------->| LED @2 |
//     +-----------+   |                                        +--------+
//     | BUTTON @3 |---+
//     +-----------+   |
//                     |
//     +-----------+   |
//     | BUTTON @4 |---+
//     +-----------+
//
// Event Flow in provisioned state ("node" state)
// - any button switch state is posted via GOOCLI to mesh network
//
//     +-----------+
//     | BUTTON @1 |---+
//     +-----------+   |
//                     |
//     +-----------+   |
//     | BUTTON @2 |---+
//     +-----------+   |    +-----------+       +-----------+   +--------+
//                     +--->| GOOCLI @1 | ))))) | GOOSRV @1 |-->| LED @k |
//     +-----------+   |    +-----------+       +-----------|   +--------+
//     | BUTTON @3 |---+                                         k=2,3,4
//     +-----------+   |
//                     |
//     +-----------+   |
//     | BUTTON @4 |---+
//     +-----------+
//
// APP has to implement the following event message flow:
// - [SWITCH:STS @id,val] events are forwarded to [GOOCLI:SET @1,val] posts
// - mesh network has to be configured as follows:
//     -- GOOCLI @1 has to post to group G1
//     -- GOOSRV @1 has to subscribe group G1
// - [GOOSRV:STS @1,val] events are forwarded to [LED:SET @2,val] calls
// - bl_led(id,val) is a helper routine for posting [LED:SET @id,val] events
//
//==============================================================================

  #include "bluccino.h"
  #include "bl_node.h"
  #include "bl_hw.h"
  #include "bl_gonoff.h"

  #define PMI  app                     // public module interface
  int app(BL_ob *o, int val);          // forward declaration

//==============================================================================
// MAIN level logging shorthands
//==============================================================================

  #define LOG                     LOG_MAIN
  #define LOGO(lvl,col,o,val)     LOGO_MAIN(lvl,col"main:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_MAIN(lvl,col,o,val)

//==============================================================================
// syntactic sugar: short hands
//==============================================================================

  static inline int led(int id, int onoff) { return _bl_led(id,onoff,(PMI)); }
  static inline int get(BL_cl cl)          { return _bl_get(cl,(PMI)); }
  static inline int gooset(int id,int val) { return _bl_gooset(id,0,val,(PMI));}

//==============================================================================
// defines & locals
//==============================================================================

  #define T_BLINK    500                    // 500 ms RGB blink period

  static volatile int id = 0;               // THE LED id
  static int starts = 0;                    // counts system starts
  static bool blinking = true;              // blinking is initially activated

//==============================================================================
// helper: blinker (let green status LED @0 attention blinking)
// - @id=0: dark, @id=1: status, @id=2: red, @id=3: green, @id=4: blue
//==============================================================================

  static int blink(BL_ob *o, int ticks)     // blinker to be ticked
  {
    static BL_ms due = 0;                   // need for periodic action

    if (id <= 1 || !bl_due(&due,T_BLINK))   // no blinking if @id:off or not due
      return 0;                             // bye if LED off or not due

          // no blinking in attention mode or housekeeping blinker busy

    if ( get(ATT_) || get(BUSY_) || !blinking )
      return 0;                             // bye if attention mode or busy

    static bool toggle;
    toggle = !toggle;

    if (toggle)
      return led(id,1), led(2+id%3,0);      // flip LED pair
    else
      return led(id,0), led(2+id%3,1);      // flip back LED pair
  }

//==============================================================================
// helper: activate/deactivate blinker
// - usage: activate(onoff)
//==============================================================================

  static void activate(bool onoff)
  {
    LOG(1,BL_G "set blinker %s", onoff ? "on":"off");
    blinking = onoff;

    if ( !blinking )    // turn all RGB LEDs off
    {
      led(2,0);  led(3,0);  led(4,0);
    }

    bl_store(1,blinking);
  }

//==============================================================================
// public app module interface
//==============================================================================
//
// (U) := (bl_up);  (D) := (bl_down)
//                  +--------------------+
//                  |        APP         |
//                  +--------------------+
//                  |        SYS:        | SYS: interface
// (D)->     INIT ->|       @id,cnt      | init module, store <out> callback
// (D)->     TICK ->|       @id,cnt      | tick the module
// (D)->     TOCK ->|       @id,cnt      | tock the module
//                  +--------------------+
//                  |       SWITCH:      | SWITCH: output interface
// (U)->      STS ->|       @id,sts      | on/off status update of switch @id
//                  +--------------------+
//                  |       GOOSRV:      | GOOSRV: ifc. (generic on/off server)
// (U)->      STS ->| @id,<BL_goo>,onoff | on/off server status update
//                  +--------------------+
//                  |       GOOCLI:      | GOOCLI: ifc. (generic on/off client)
// (D)<-      SET <-| @id,<BL_goo>,onoff | publish generic on/off SET command
//                  +--------------------+
//                  |         NVM:       | NVM: interface (non volatile memory)
// (U)->    READY ->|                    | notify that NVM is ready
//                  +--------------------+
//
//==============================================================================

  int app(BL_ob *o, int val)           // public app module interface
  {
    static BL_oval D = bl_down;
    static BL_oval N = bl_node;        // mesh node house keeping

    switch (bl_id(o))                  // dispatch mesage ID
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <cb>]
        return led(0,0);               // turn status LED initially off

      case BL_ID(_SYS,TICK_):          // [SYS:TICK @id,cnt]
        return blink(o,val);           // tick blinker

      case BL_ID(_SYS,TOCK_):          // [SYS:TOCK @id,cnt]
        if (val % 20 == 0)             // log every 20th tock
          LOGO(1,"I'm alive! ",o,val); // log to see we are alive
        return 0;                      // OK

      case BL_ID(_SWITCH,STS_):        // button press to cause LED on off
        LOGO(1,BL_M,o,val);
        if ( get(PRV_) )               // go via mesh if provisioned
          gooset(1,val);               // post [GOOCLI:SET] message
        else                           // direct (de)activation if unprovisioned
          activate(val);               // activate/deactivate blinker
        return 0;                      // OK

      case BL_ID(_GOOSRV,STS_):        // [GOOSRV:STS] status update
        LOGO(1,BL_C,o,val);            // let us see!
        if (o->id == 1)                // only in case of GOOSRV @1
          activate(val);               // activate/deactivate blinker
        return 0;                      // OK

      case _BL_ID(_GOOCLI,SET_):       // [GOOCLI:SET]
        return bl_out(o,val,(D));

      case BL_ID(_NVM,READY_):         // [GOOSRV:STS] status update
        LOGO(1,BL_M,o,val);            // let us see!
        starts = bl_recall(0);         // recall system starts from NVM @0
        id = 2 + (starts % 3);         // map starts -> 2:4
        bl_store(0,++starts);          // store back incremented value at NVM @0
        LOG(1,BL_G "system start #%d",starts);

        blinking = (starts==1) ? 1 : bl_recall(1);
        return 0;                      // OK

      case _BL_ID(_GET,ATT_):
      case _BL_ID(_GET,PRV_):
        return bl_out(o,val,(N));

      case _BL_ID(_GET,BUSY_):
        return bl_out(o,val,(N));

      case _BL_ID(_LED,SET_):
        return bl_out(o,val,(D));

      default:
        return 0;                      // OK (ignore other messages)
    }
  }

//==============================================================================
// top gear
// - to integrate bl_node() for mesh node house keeping
// - at the end all non [SYS:] messages must be emitted to the subscriber (app)
//==============================================================================

  int bl_top(BL_ob *o, int val)        // overrides gear's weak bl_top() def.
  {
    bl_node(o,val);                    // mesh node house keeping
    return bl_emit(o,val);             // emit all messages except [SYS:] msg's
  }

//==============================================================================
// main function
//==============================================================================

  void main(void)
  {
    bl_hello(VERBOSE,PROJECT);         // set verbose level, print hello message
    bl_cfg(bl_down,_BUTTON,BL_SWITCH); // mask [BUTTON:SWITCH] events only
    bl_engine(app,10,1000);            // run app with 10/1000 ms tick/tock
  }
