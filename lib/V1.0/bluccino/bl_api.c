//==============================================================================
// bl_api.c - bluccino API layer
//==============================================================================
//
// The whole concept is based on having bluccino objects which control
// access to network interfaces and resources. Resource a grouped into the
// following categories:
//
//   1) Hardware
//        - general hardware control (e.g. low power control, watchdogs)
//        - LEDs
//        - buttons and/or switches
//        - General purpose I/O (GPIO)
//        - Timers
//        - Non-Volatile memory (NVM)
//   2) BLE System
//        - general BLE control (e.g. low power control of BLE sub units)
//        - advertiser
//        - scanner
//        - characteristics
//   3) MESH System
//        - general Mesh control (e.g. provisioning, attentioning)
//        - Mesh models (a kind of smart interfaces for mesh communication)
//          like client models (which) have no state and server models which
//          have state.
//
//   Each resource can have a state and for each resource there are certain
//   operations defined on it. Consider the following simple example.
//
//    +-----------+    +-----------+          +-----------+    +-----------+
//    |           |    |           |          |           |    |           |
//    +  SWITCH   +===>+  SENDER   +--------->+ RECEIVER  +===>+    LED    +
//    |           |    |           |          |           |    |           |
//    +-----------+    +-----------+          +-----------+    +-----------+
//
//   Resources can be treated as class instance objects of the different re-
//   source classes
//      :     :
//
//   Operations
//   ==========
//
//    Operation    SWITCH          CLIENT           SERVER         LED
//   -----------------------------------------------------------------------
//                  event        request to          event        change
//     SET        on state       change ser-       on state        LED
//                 change        ver's state        change        state
//
//==============================================================================

  #include "bluccino.h"

//==============================================================================
// API level logging shorthands
//==============================================================================

  #define LOG                     LOG_API
  #define LOGO(lvl,col,o,val)     LOGO_API(lvl,col"api:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_API(lvl,col,o,val)

//==============================================================================
// notification and driver callbacks
//==============================================================================

  static BL_oval test = NULL;           // test callback

//==============================================================================
// us/ms clock
//==============================================================================

  static BL_us offset = 0;             // offset for us clock

  static BL_us now_us()                // system clock in us
  {
    #if __ZEPHYR__
      uint64_t cyc = k_uptime_ticks();
      uint64_t f = sys_clock_hw_cycles_per_sec();
      return (BL_us)((1000000*cyc)/f);
    #else
      return (BL_us)timer_now();
    #endif
  }

  BL_us bl_zero(void)                  // reset clock
  {
    return offset = now_us();
  }

//==============================================================================
// get us clock time
//==============================================================================

  BL_us bl_us(void)                    // get current clock time in us
  {
    BL_us us = now_us();

    if (offset == 0)                   // first call always returns 0
      us = bl_zero();                  // reset clock

    return us  - offset;
  }

//==============================================================================
// get ms clock time
//==============================================================================

  BL_ms bl_ms(void)                    // get current clock time in ms
  {
    BL_us us = bl_us();
    return us/1000;
  }

//==============================================================================
// periode detection
// - usage: ok = bl_period(o,ms)        // is tick/tock time meeting a period?
//==============================================================================

  bool bl_period(BL_ob *o, BL_ms ms)
  {
    BL_pace *p = bl_data(o);
    return p ? ((p->time % ms) == 0) : 0;
  }

//==============================================================================
// timing & sleep
//==============================================================================

  bool bl_due(BL_ms *pdue, BL_ms ms)   // check if time if due & update
  {
    BL_ms now = bl_ms();

    if (now < *pdue)                   // time for tick action is due?
      return false;                    // no, not yet due!

    *pdue = now + ms;                  // catch-up due time
    return true;                       // yes, tick time due!
  }

//void bl_sleep(int ms)                // deep sleep for given milliseconds
//{
//  nrf_delay_ms(ms);
//}

  void bl_sleep(BL_ms ms)              // deep sleep for given milliseconds
  {
    if (ms > 0)
      BL_SLEEP((int)ms);
  }

  void bl_halt(BL_txt msg, BL_ms ms)   // halt system
  {
    LOG(0,BL_R"%s: system halted", msg);
    for (;;)
      bl_sleep(ms);
  }

//==============================================================================
// output a message from Bluccino API or in general
// - usage: bl_out(o,val,out)
// - any hashed opcode has to be de-hashed (clear hash bit of opcode)
//==============================================================================

  __weak int bl_out(BL_ob *o, int val, BL_oval call)
  {
    if (call)                          // is an app callback provided?
    {
      if ( !BL_ISAUG(o->cl) && !BL_HASHED(o->op) ) // easy!
        return call(o,val);            // forward event message to subscriber

        // hashed opcode! (hash bit set) => need to duplicate object with
        // de-hashed opcode before forwarding

      BL_ob oo = {BL_CLR(o->cl),BL_CLEAR(o->op),o->id,o->data};
      return call(&oo,val);            // forward with de-hashed opcode
    }
    return 0;
  }

//==============================================================================
// message downward posting to lower level / driver layer (default/__weak)
// - bl_down() is defined as weak and can be overloaded
// - by default all messages posted to BL_DOWN are forwarded to BL_CORE
//==============================================================================

  __weak int bl_down(BL_ob *o, int val)
  {
    bool nolog = bl_is(o,_LED,SET_) && o->id == 0;
    nolog = nolog || (o->cl == _SYS);

    if ( !nolog )
      bl_logo(3,"down:",o,val);         // not suppressed messages are logged

    return bl_core(o,val);             // forward down to BL_CORE module
  }

//==============================================================================
// message upward posting to API layer (default/__weak)
// - bl_up() is defined as weak and can be overloaded
// - by default all messages posted to BL_UP are forwarded to BL_IN
//==============================================================================

  __weak int bl_up(BL_ob *o, int val)
  {
    bl_logo(3,"up:",o,val);
    return bl_in(o,val);
  }

//==============================================================================
// setup initializing, ticking and tocking for a test module
// - usage: bl_test(module)            // controlled by bl_run()
//==============================================================================

  int bl_test(BL_oval module)
  {
    test = module;
    return 0;                          // OK
  }

//==============================================================================
// run app with given tick/tock periods and provided when-callback
// - usage: bl_run(app,10,100,when)   // run app with 10/1000 tick/tock periods
//==============================================================================

  void bl_run(BL_oval app, int tick_ms, int tock_ms, BL_oval when)
  {
    BL_pace tick_pace = {tick_ms,0};
    BL_pace tock_pace = {tock_ms,0};

    BL_ob oo_tick = {_SYS,TICK_,0,&tick_pace};
    BL_ob oo_tock = {_SYS,TOCK_,1,&tock_pace};

    int multiple = tock_ms / tick_ms;

    if (tock_ms % tick_ms != 0)
      bl_err(-1,"bl_engine: tock period no multiple of tick period");

      // init Bluccino library module and app init

    bl_init(bluccino,when);            // always use upward gear for core output
    if (app)
      bl_init(app,when);
    if (test)
      bl_init(test,when);

      // post periodic ticks and tocks ...

    for (int ticks=0;;ticks++)
    {
      static int tocks = 0;

        // post [SYS:TICK @id,cnt] events

      bluccino(&oo_tick,ticks);        // tick BLUCCINO module
      if (app)
        app(&oo_tick,ticks);           // tick APP module
      if (test)
        test(&oo_tick,ticks);          // tick TEST module

        // post [SYS:TOCK @id,cnt] events

      if (ticks % multiple == 0)       // time for tocking?
      {
        bluccino(&oo_tock,tocks);      // tock BLUCCINO module
        if (app)
          app(&oo_tock,tocks);         // tock APP module
        if (test)
          test(&oo_tock,tocks);        // tock TEST module
        tocks++;
        tock_pace.time += tock_ms;     // increase tock time
      }

      bl_sleep(tick_ms);               // sleep for one tick period
      tick_pace.time += tick_ms;
    }
  }
