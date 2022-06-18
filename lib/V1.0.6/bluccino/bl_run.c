//==============================================================================
//  bl_run.c
//  Bluccino init/tic/toc engine and integration of test modules
//
//  Created by Hugo Pristauz on 2022-04-03
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

  #include "bluccino.h"

  static BL_oval test = NULL;

//==============================================================================
// set callback value (warn if new callback deviates from provided default)
// - usage: out = bl_cb(o,out,"app:O");
//==============================================================================

  BL_oval bl_cb(BL_ob *o, BL_oval def, BL_txt msg)
  {
    BL_oval cb = o->data;              // fetch callback from object's data ref
    if (def && cb != def && bl_dbg(2)) // does callback deviate from default?
      bl_prt(BL_R "warning: change of default callback %s\n" BL_0, msg);
    return cb;
  }

//==============================================================================
// setup initializing, ticking and tocking for a test module
// - usage: bl_test(module)            // controlled by bl_run()
//==============================================================================

  __weak int bl_test(BL_oval module)
  {
    test = module;
    return 0;                          // OK
  }

//==============================================================================
// run app with given tick/tock periods and provided when-callback
// - usage: bl_run(app,10,100,when)   // run app with 10/1000 tick/tock periods
//==============================================================================

  __weak void bl_run(BL_oval app, int tick_ms, int tock_ms, BL_oval when)
  {
    BL_oval A = app;                   // short hand
    BL_oval B = bluccino;              // shorthand for bluccino module
    BL_oval W = when;                  // short hand for when callback
    BL_oval T = test;                  // short hand for top test module

    BL_pace tick_pace = {tick_ms,0};
    BL_pace tock_pace = {tock_ms,0};

    BL_ob oo_tick = {_SYS,TICK_,0,&tick_pace};
    BL_ob oo_tock = {_SYS,TOCK_,1,&tock_pace};

    int multiple = tock_ms / tick_ms;

    if (tock_ms % tick_ms != 0)
      bl_err(-1,"bl_engine: tock period no multiple of tick period");

      // init Bluccino library module and app init

    bl_init((B),(W));                  // init bluccino module, output to <when>
    if ((A))
      bl_init((A),(W));                // init app
    if ((T))
      bl_init((T),(W));

      // post periodic ticks and tocks ...

    for (int ticks=0;;ticks++)
    {
      static int tocks = 0;

        // post [SYS:TICK @id,cnt] events

      bl_fwd(&oo_tick,ticks,(B));      // tick bluccino module
      if ((A))
        bl_fwd(&oo_tick,ticks,(A));    // tick APP module
      if ((T))
        bl_fwd(&oo_tick,ticks,(T));    // tick TEST module

        // post [SYS:TOCK @id,cnt] events

      if (ticks % multiple == 0)       // time for tocking?
      {
        bl_fwd(&oo_tock,tocks,(B));    // tock BLUCCINO module
        if ((A))
          bl_fwd(&oo_tock,tocks,(A));  // tock APP module
        if ((T))
          bl_fwd(&oo_tock,tocks,(T));  // tock TEST module
        tocks++;
        tock_pace.time += tock_ms;     // increase tock time
      }

      bl_sleep(tick_ms);               // sleep for one tick period
      tick_pace.time += tick_ms;
    }
  }
