//==============================================================================
//  bl_run.c
//  Bluccino init/tic/toc engine and integration of test modules
//
//  Created by Hugo Pristauz on 2022-04-03
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_run.h"

  static BL_oval test = NULL;

//==============================================================================
// logging shorthands
//==============================================================================

  #define WHO "bl_run:"           // who is logging?

  #define LOG                     LOG_RUN
  #define LOGO(lvl,col,o,val)     LOGO_RUN(lvl,col WHO,o,val)
  #define LOG0(lvl,col,o,val)     LOGO_RUN(lvl,col,o,val)

//==============================================================================
// run time monitoring structure
//==============================================================================

  #ifndef CFG_RUN_LOG_PERIOD
    #define CFG_RUN_LOG_PERIOD 5000   // 5000 ms assessment interval
  #endif

//==============================================================================
// run time monitoring (data structures)
//==============================================================================
#if (CFG_RUN_LOG_PERIOD)

  #define PERIOD CFG_RUN_LOG_PERIOD

  BL_run run = {0,0,0,0,0,PERIOD};          // run monitoring data

#endif
//==============================================================================
// run time monitoring (control functions)
// - usage:
//     moni_start(now);
//     for(tick=0;;tick++)
//     {
//       moni_suspend();
//       bl_sleep(delay);
//       moni_log(now);
//       moni_resume();
//     }
//==============================================================================
#if (CFG_RUN_LOG_PERIOD)

  static void moni_start(BL_ms now, int tick, int tock)  // start run monitoring
  {
    run.tick = tick;  run.tock = tock;
    run.due = now + run.period;
    run.duty = run.total = 0;
    run.start = run.tic = bl_us();
  }

  static void moni_suspend(void)
  {
    BL_us toc = bl_us();                    // toc (end) time of asses. segment
    run.duty += (toc-run.tic);              // add up segment time to duty time
    run.tic = bl_us();                      // refresh segment tic time
  }

  static void moni_resume(void)
  {
    run.tic = bl_us();                      // refresh segment tic time
  }

  static void moni_stop(void)
  {
    moni_suspend();
    run.total = bl_us() - run.start;
  }

  static void moni_log(BL_ms now)           // log if due
  {
    if (now >= run.due)
    {
      moni_stop();                          // stop monitoring and log

      int permill = run.total ? (int)((1000*run.duty) / run.total) : 0;

      LOG(1,BL_C "run time duty: %d.%d%% @tick/tock %d/%d ms (%ld/%ld us)",
	      permill/10,permill%10,run.tick,run.tock,(long)run.duty,(long)run.total);

        // finally post a run monitoring message using top gear

      bl_post((bl_top),SYS_RUN_0_BL_run_permill, 0,&run,permill);

      moni_start(now,run.tick,run.tock);    // restart monitoring
    }
  }

#else

  #define moni_start(now,tick,tock)         // empty
  #define moni_suspend()                    // empty
  #define moni_resume()                     // empty
  #define moni_stop(report)                 // empty
  #define moni_log(now)                     // empty

#endif
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
// - usage: bl_run(app,10,100,when)    // run app with 10/1000 tick/tock periods
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

    moni_start(tick_pace.time,tick_ms,tock_ms);

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

        // calculate next reference time stamp and sleep until
        // this time

      BL_ms now = bl_ms();             // current time
      tick_pace.time += tick_ms;

      if (now < tick_pace.time)
      {
        moni_suspend();                // suspend run monitoring
        bl_sleep(tick_pace.time-now);  // sleep for one tick period
        moni_log(tick_pace.time);      // log results if due
        moni_resume();                 // resume run monitoring
      }
    }
  }

//==============================================================================
// cleanup (needed for *.c file merge of the bluccino core)
//==============================================================================

  #include "bl_clean.h"
