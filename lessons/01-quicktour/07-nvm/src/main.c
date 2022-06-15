//==============================================================================
// main.c
// main program for 07-nvm sample
//
// Created by Hugo Pristauz on 2022-Jan-04
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_hw.h"

  #define PMI  app

//==============================================================================
// MAIN level logging shorthands
//==============================================================================

  #define WHO  "main"

  #define LOG                     LOG_MAIN
  #define LOGO(lvl,col,o,val)     LOGO_MAIN(lvl,col WHO ":",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_MAIN(lvl,col,o,val)

//==============================================================================
// defines & locals
//==============================================================================

  #define T_BLINK   1000                    // 1000 ms RGB blink period

  int app(BL_ob *o, int val);               // forward declaration
  static int id = 2;                        // THE LED id, cycles 2->3->4->2->

//==============================================================================
// helper: blinker (let green status LED @0 attention blinking)
// - @id=0: dark, @id=1: status, @id=2: red, @id=3: green, @id=4: blue
//==============================================================================

  static int blink(BL_ob *o, int ticks)     // blinker to be ticked
  {
    if (id <= 1 || !bl_period(o,T_BLINK))   // no blinking if @id:off or not due
      return 0;                             // bye if LED off or not due

    static bool toggle;
    toggle = !toggle;

    if (toggle)
      return (_bl_led(id,1,(PMI)), _bl_led(2+id%3,0,(PMI)));  // flip LED pair
    else
      return (_bl_led(id,0,(PMI)), _bl_led(2+id%3,1,(PMI)));  // flip back LEDs
  }

//==============================================================================
// worker: switch status update
//==============================================================================

  static int switch_sts(BL_ob *o, int val)
  {
    LOGO(1,BL_M,o,val);

    for (int i=1; i<4; i++)
      _bl_led(i,0,(PMI));          // all RGB LEDs off

    id = 2 + (id % 3);             // cyclical decrement of LED id

    LOG(1,BL_R "saving LED @id = %d to NVM ...",id); 
    bl_save("id",&id,sizeof(id));  // store LED @id to NVM

    return 0;
  }

//==============================================================================
// worker: system init
//==============================================================================

  static int sys_init(BL_ob *o, int val)
  {
    static int starts = 0;             // counts system starts
    _bl_led(0,0,(PMI));                // turn status LED initially off

      // load system starts counter from NVM

    bl_load("starts", &starts, sizeof(starts));
    starts++;                          // increment number of system starts
    LOG(1,BL_M "system start: #%d",starts);

      // save system starts counter to NVM

    bl_save("starts", &starts, sizeof(starts));

      // load LED id

    bl_load("id", &id, sizeof(id));
    LOG(1,BL_R "loaded LED @id = %d from NVM",id);

    return 0;                          // OK
  }

//==============================================================================
// public app module interface
//==============================================================================
//
// (M) := (main);  (D) := (bl_down);  (U) := (bl_up)
//
//                  +--------------------+
//                  |        app         |
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (M)->     INIT ->|       @id,cnt      | init module, store <out> callback
// (M)->     TICK ->|       @id,cnt      | tick the module
// (M)->     TOCK ->|       @id,cnt      | tock the module
//                  +--------------------+
//                  |       SWITCH:      | SWITCH input interface
// (U)->      STS ->|       @id,sts      | on/off status update of switch @id
//                  +--------------------+
//                  |        LED:        | LED: output interface
// (D)<-      SET <-|      @id,onoff     | set LED @id on/off (i=0..4)
// (D)<-   TOGGLE <-|        @id         | toggle LED @id (i=0..4)
//                  +--------------------+
//
//==============================================================================

  int app(BL_ob *o, int val)           // public APP module interface
  {
    static BL_oval D  = bl_down;

    switch (bl_id(o))                  // dispatch mesage ID
    {
      case SYS_INIT_0_cb_0:            // [SYS:INIT <cb>]
        return sys_init(o,val);        // delegate to sys_init() worker

      case SYS_TICK_id_BL_pace_cnt:          // [SYS:TICK @id,cnt]
        return blink(o,val);           // tick blinker

      case SYS_TOCK_id_BL_pace_cnt:          // [SYS:TOCK @id,cnt]
        if (val % 20 == 0)             // log every 20th tock
          LOGO(1,"I'm alive! ",o,val); // log to see we are alife
        return 0;                      // OK

      case SWITCH_STS_id_0_sts:        // button press to cause LED on off
        return switch_sts(o,val);      // delegate to switch_sts() worker

      case _LED_SET_id_0_onoff:
      case _LED_TOGGLE_id_0_0:
        return bl_out(o,val,(D));      // output to down gear

      default:
        return 0;                      // OK (ignore any other messages)
    }
  }

//==============================================================================
// main function
//==============================================================================

  void main(void)
  {

    bl_hello(VERBOSE,PROJECT);         // set verbose level, print hello message
    bl_cfg(bl_down,_BUTTON,BL_SWITCH); // mask [BUTTON:SWITCH] events only
    bl_engine(app,10,1000);            // run engine with 10/1000 ms tick/tock
  }
