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
  #include "bl_node.h"
  #include "bl_core.h"
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
// defines & locals
//==============================================================================

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

    if ( _bl_get(ATT_,(PMI)) ||             // no blinking in attention mode
         _bl_get(BUSY_,(PMI)) )             // no blinking during startup
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
// (B) := (bl_hwbut);  (L) := (bl_hwled);  (U) := (bl_up);  (D) := (bl_down);
// (N) := (bl_node);
//
//                  +--------------------+
//                  |        app         |
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (M)->     INIT ->|       @id,cnt      | init module, store <out> callback
// (M)->     TICK ->|  @id,<BL_pace>,cnt | tick the module
// (M)->     TOCK ->|  @id,<BL_pace>,cnt | tock the module
//                  |....................|
//                  |       #SYS:        | SYS output interface
// (N)<-     INIT <-|       @id,cnt      | init module, store <out> callback
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
//                  |        NVM:        | NVM: interface (non volatile memory)
// (U)->    READY ->|       ready        | notify that NVM is ready
//                  +--------------------+
//                  |       #GET:        | GET output interface
// (D)<-      ATT <-|                    | get attention status
// (D)<-      PRV <-|                    | get provision status
//                  +--------------------+
//                  |       #LED:        | LED output interface
// (D)<-      SET <-|     @id,onoff      | set LED @id on/off state
// (D)<-   TOGGLE <-|        @id         | toggle LED @id state
//                  +--------------------+
///
//==============================================================================

  int app(BL_ob *o, int val)           // public APP module interface
  {
    static BL_oval D = bl_down;        // down gear
    static BL_oval N = bl_node;        // mesh node house keeping module

    switch (bl_id(o))
    {
      case SYS_INIT_0_cb_0:
        return bl_fwd(o,val,(N));      // init mesh node house keeping

      case SYS_TICK_id_BL_pace_cnt:
        blink(o,val);                  // tick blinker
        return bl_fwd(o,val,(N));      // tick mesh node house keeping module

      case SYS_TOCK_id_BL_pace_cnt:          // [SYS:TOCK @id,cnt]
        if (val % 20 == 0)             // log every 20th tock
          LOGO(1,"I'm alive! ",o,val); // log to see we are alife
        return 0;                      // OK

      case SWITCH_STS_id_0_sts:
        LOGO(1,"@",o,val);
        if ( _bl_get(PRV_,(PMI)) )     // only if provisioned
          bl_msg((D),_GOOCLI,SET_, 1,NULL,val);
        else
          _bl_led(id,val,(PMI));       // switch LED @id on/off
        return 0;                      // OK

      case GOOSRV_STS_id_BL_goo_sts:   // generic on/off server status update
        LOGO(1,BL_R,o,val);
        if (o->id == 1)
          _bl_led(id,val,(D));         // switch LED @id
        return 0;                      // OK

      case NVM_READY_0_0_sts:
        LOGO(1,BL_M,o,val);
        starts = bl_recall(0);         // recall system starts from NVM @0
        id = 2 + (starts % 3);         // map starts -> 2:4
        bl_store(0,++starts);          // store back incremented value at NVM @0
        LOG(1,BL_M "system start #%d",starts);
        return 0;

      case _GET_ATT_0_0_0:
      case _GET_PRV_0_0_0:
        bl_out(o,val,(D));             // post to down gear

      case _LED_SET_id_0_onoff:
      case _LED_TOGGLE_id_0_0:
        bl_out(o,val,(D));

      default:
        return bl_fwd(o,val,(N));      // else forward event to house keeping
    }
  }

//==============================================================================
// main function
//==============================================================================

  void main(void)
  {
    bl_hello(VERBOSE,PROJECT);         // set verbose level, print hello message
    bl_run(app,10,1000,app);           // run app with 10/1000 ms tick/tock
  }
