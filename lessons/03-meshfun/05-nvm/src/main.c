//==============================================================================
// main.c
// main program for 05-nvm test
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
  #include "bl_hw.h"

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

  static volatile int id = 0;               // THE LED id, cycles 2->3->4->2->
  static int starts = 0;                    // counts system starts
  static bool provision = false;            // current provision status
	static bool attention = false;            // current attention status

//==============================================================================
// helper: blinker (let green status LED @0 attention blinking)
// - @id=0: dark, @id=1: status, @id=2: red, @id=3: green, @id=4: blue
//==============================================================================

  static int blink(BL_ob *o, int ticks)     // blinker to be ticked
  {
    static BL_ms due = 0;                   // need for periodic action

    if (ticks%25 == 0)
		{
		  if (attention)
			  bl_led(0,-1);                       // toggle status LED
			else
			  bl_led(0,provision);                // status LED show provision state
		}

    if (id <= 1 || !bl_due(&due,T_BLINK))   // no blinking if @id:off or not due
      return 0;                             // bye if LED off or not due

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
    switch (bl_id(o))                  // dispatch mesage ID
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <cb>]
        bl_led(0,0);                   // turn status LED initially off
        return 0;                      // nothing to int

      case BL_ID(_SYS,TICK_):          // [SYS:TICK @id,cnt]
        blink(o,val);                  // tick blinker
        return 0;                      // OK

      case BL_ID(_SYS,TOCK_):          // [SYS:TOCK @id,cnt]
        if (val % 20 == 0)             // log every 20th tock
          LOGO(1,"I'm alive! ",o,val); // log to see we are alife
        return 0;                      // OK

      case BL_ID(_SWITCH,STS_):        // button press to cause LED on off
        LOGO(1,BL_M,o,val);
        if ( bl_get(bl_top,PRV_))      // only if provisioned
          bl_msg(bl_top,_GOOCLI,SET_,  1,NULL,val);
        else
          bl_led(id,val);              // switch LED @id on/off
        return 0;                      // OK

      case BL_ID(_GOOSRV,STS_):        // [GOOSRV:STS] status update
        LOGO(1,BL_C,o,val);            // let us see!
        if (o->id == 1)                // only in case of GOOSRV @1
          bl_led(id,val);              // turn LED @id on/off
        return 0;                      // OK

      case BL_ID(_NVM,READY_):         // [GOOSRV:STS] status update
        LOGO(1,BL_M,o,val);            // let us see!
        starts = bl_recall(0);         // recall system starts from NVM @0
        id = 2 + (starts % 3);         // map starts -> 2:4
        bl_store(0,++starts);          // store back incremented value at NVM @0
        LOG(1,BL_M "system start #%d",starts);
        return 0;                      // OK

      case BL_ID(_MESH,PRV_):          // [MESH:PRV sts] provision status update
        provision = val;               // update provision status
        return bl_led(0,provision);    // OK

      case BL_ID(_MESH,ATT_):          // [MESH:ATT sts] attention status update
        attention = val;               // update attention status
        return 0;                      // OK

      default:
        return 0;                      // OK (ignore any other messages)
    }
  }

//==============================================================================
// main function
//==============================================================================

  void main(void)
  {
    bl_hello(VERBOSE,VERSION);         // set verbose level, print hello message
    bl_cfg(bl_down,_BUTTON,BL_SWITCH); // mask [BUTTON:SWITCH] events only
    bl_run(app,10,1000,app);           // run app with 10/1000 ms tick/tock
  }
