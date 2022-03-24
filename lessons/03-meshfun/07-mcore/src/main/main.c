//==============================================================================
// main.c
// main program for mcore test
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
//                     +--->| GOOCLI @1 | ))))) | GOOSRV @1 |-->| LED @2 |
//     +-----------+   |    +-----------+       +-----------|   +--------+
//     | BUTTON @3 |---+
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
  #include "bl_basis.h"
  #include "bl_core.h"

//==============================================================================
// MAIN level logging shorthands
//==============================================================================

  #define LOG                     LOG_MAIN
  #define LOGO(lvl,col,o,val)     LOGO_MAIN(lvl,col"main:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_MAIN(lvl,col,o,val)

//==============================================================================
// defines & locals
//==============================================================================

  #define VERSION  CFG_APP_VERSION
  #define VERBOSE  CFG_APP_VERBOSE          // verbose level for application

  #define T_BLINK   1000                    // 1000 ms RGB blink period

  static volatile int id = 0;               // THE LED id
  static int starts = 0;                    // counts system starts

//==============================================================================
// helper: attention blinker (let green status LED @0 attention blinking)
// - @id=0: dark, @id=1: status, @id=2: red, @id=3: green, @id=4: blue
//==============================================================================

  static int blink(BL_ob *o, int ticks)     // attention blinker to be ticked
  {
    static BL_ms due = 0;                   // need for periodic action

    if (id <= 1 || !bl_due(&due,T_BLINK))   // no blinking if @id:off or not due
      return 0;                             // bye if LED off or not due

    if ( bl_get(bl_basis,ATT_) ||           // no blinking in attention mode
         bl_get(bl_basis,BUSY_))            // no blinking during startup
      return 0;                             // bye if attention state

    static bool toggle;
    toggle = !toggle;

    if (toggle)
      return (bl_led(id,1), bl_led(2+id%3,0));    // flip LED pair
    else
      return (bl_led(id,0), bl_led(2+id%3,1));    // flip back LED pair
  }

//==============================================================================
// public app module interface
//==============================================================================
//
// (B) := (BL_HWBUT);  (L) := (BL_HWLED)
//                  +--------------------+
//                  |        APP         |
//                  +--------------------+
//                  |        SYS:        | SYS: interface
// (v)->     INIT ->|       @id,cnt      | init module, store <out> callback
// (v)->     TICK ->|       @id,cnt      | tick the module
// (v)->     TOCK ->|       @id,cnt      | tock the module
//                  +--------------------+
//                  |       SWITCH:      | SWITCH: output interface
// (^)->      STS ->|       @id,sts      | on/off status update of switch @id
//                  +--------------------+
//                  |       GOOSRV:      | GOOSRV: ifc. (generic on/off server)
// (^)->      STS ->|       @id,sts      | on/off server status update
//                  +--------------------+
//                  |       GOOCLI:      | GOOCLI: ifc. (generic on/off client)
// (v)<-      SET <-|      @id,onoff     | publish generic on/off SET command
//                  +--------------------+
//                  |         NVM:       | NVM: interface (non volatile memory)
// (^)->    READY ->|                    | notify that NVM is ready
//                  +--------------------+
//
//==============================================================================

  int app(BL_ob *o, int val)           // public APP module interface
  {
    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <cb>]
        return bl_init(bl_basis,bl_down);  // init BL_BASIS, output goes down

      case BL_ID(_SYS,TICK_):          // [SYS:TICK @id,cnt]
        blink(o,val);                  // tick blinker
        bl_basis(o,val);               // tick BL_BASIS module
        return 0;                      // OK

      case BL_ID(_SYS,TOCK_):          // [SYS:TOCK @id,cnt]
        if (val % 5 == 0)              // log every 5th tock
          LOGO(1,"I'm alive! ",o,val); // log to see we are alife
        return 0;                      // OK

      case BL_ID(_SWITCH,STS_):        // button press to cause LED on off
        LOGO(1,"@",o,val);
        if ( bl_get(bl_basis,PRV_))    // only if provisioned
        {
          BL_ob oo = {_GOOCLI,SET_,1,NULL};
          bl_down(&oo,val);            // post via generic on/off client
        }
        else
          bl_led(2,val);               // switch LED @2 on/off
        return 0;                      // OK

      case BL_ID(_GOOSRV,STS_):        // [GOOSRV:STS] status update
        LOGO(1,BL_R,o,val);
        if (o->id == 1)
          bl_led(2,val);               // switch LED @2
        return 0;                      // OK

      case BL_ID(_NVM,READY_):         // [GOOSRV:STS] status update
        starts = bl_recall(0);         // recall system starts from NVM @0
        bl_store(0,++starts);          // store back incremented value at NVM @0
        LOG(1,BL_M "system start #%d",starts);
        return 0;

      default:
        return bl_basis(o,val);        // else forward event to BL_BASE module
    }
  }

//==============================================================================
// main function
//==============================================================================

  void main(void)
  {
    bl_hello(VERBOSE,VERSION);         // set verbose level, print hello message
    bl_run(app,10,1000,app);           // run app with 10/1000 ms tick/tock
  }
