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
// provision & attention state
//==============================================================================

  static bool provision = 0;
  static bool attention = 0;

//==============================================================================
// notification and driver callbacks
//==============================================================================

  static BL_fct out = NULL;            // <out> callback
  static BL_fct test = NULL;           // test callback

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

  __weak int bl_out(BL_ob *o, int val, BL_fct call)
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
// input a message to Bluccino API
//==============================================================================

  __weak int bl_in(BL_ob *o, int val)
  {
    int level = 2;                          // default verbose level

    switch (bl_id(o))                           // dispatch event
    {
      case BL_ID(_SET,PRV_):            // provision state changed
        provision = val;
        bl_log_color(attention,provision);
        LOG(2,BL_M"node %sprovision",val?"":"un");
        return bl_out(o,val,out);

      case BL_ID(_SET,ATT_):          // attention state changed
        attention = val;
        bl_log_color(attention,provision);
        LOG(2,BL_G"attention %s",val?"on":"off");
        return bl_out(o,val,out);

      case BL_ID(_SYS,TICK_):
      case BL_ID(_SYS,TOCK_):
      case BL_ID(_SCAN,ADV_):
        level = 5;
        break;

      case BL_ID(_LED,SET_):
      case BL_ID(_LED,TOGGLE_):
        LOGO(level,"@",o,val);
        return bl_down(o,val);

      default:
        break;
    }

    LOGO(level,"@",o,val);
    return bl_out(o,val,out);            // forward message to subscriber
  }

//==============================================================================
// dummy interface for core module public interface (default/__weak)
//==============================================================================

  __weak int bl_hwbut(BL_ob *o, int val) { return -1; }
  __weak int bl_hwled(BL_ob *o, int val) { return -1; }

  __weak int bl_core(BL_ob *o, int val)
  {
    switch (o->cl)
    {
      case _SYS:
        bl_hwbut(o,val);
        bl_hwled(o,val);
        return 0;                      // OK

      case _LED:
        return bl_hwled(o,val);

      default:
        return -1;                     // not supported by default
    }
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
// post general message [CL:OP @id,<data>,val] to module
// - usage: bl_msg(module,cl,op,id,data,val)
//==============================================================================

  int bl_msg(BL_fct module, BL_cl cl, BL_op op, int id, void* data, int val)
  {
    BL_ob oo = {cl,op,id,data};
    return module(&oo,val);            // post message to module interface
  }

//==============================================================================
// post system message to module
// - usage: bl_sys(module,op,cb,val)   // post [SYS:op @0,<cb>,val] to module
//==============================================================================

  int bl_sys(BL_fct module, BL_op op, BL_fct cb, int val)
  {
    BL_ob oo = {_SYS,op,0,cb};
    return module(&oo,val);            // post message to module interface
  }

//==============================================================================
// emit message to be handeled to <out> subscriber
// - usage: bl_emit(o,cl,op,val,out)  // post [cl:op o->id,val] to <out>
//==============================================================================

  int bl_emit(BL_ob *o, BL_cl cl, BL_op op, int val, BL_fct out)
  {
    BL_ob oo = {cl,op,o->id,o->data};
    return bl_out(&oo,val,out);
  }

//==============================================================================
// post message (with main parameters to SYS interface of given module)
// - usage: bl_post(module,opcode,id,val)  // class=_SYS, data=NULL
//==============================================================================

  int bl_post(BL_fct module, BL_op op, int id, int val)
  {
    BL_ob oo = {_SYS,op,id,NULL};
    return module(&oo,val);            // post message to module interface
  }

//==============================================================================
// forward a received message to an interface of a given module
// - usage: bl_fwd(module,cl,o,val)    // only interface class to be changed
//==============================================================================

  int bl_fwd(BL_fct module, BL_cl cl, BL_ob *o, int val)
  {
    BL_ob oo = {cl,o->op,o->id,o->data};
    return module(&oo,val);            // forward message to module interface
  }

//==============================================================================
// subscribe to message output
// - usage: bl_when(module,cb)         // class=_SYS, val=0, data=NULL
//==============================================================================

  int bl_when(BL_fct module, BL_fct cb)
  {
    BL_ob oo = {_SYS,WHEN_,0,cb};
    return module(&oo,0);              // post [SYS:WHEN <cb>] to module
  }

//==============================================================================
// setup initializing, ticking and tocking for a test module
// - usage: bl_test(module)            // controlled by bl_run()
//==============================================================================

  int bl_test(BL_fct module)
  {
    test = module;
    return 0;                          // OK
  }

//==============================================================================
// Blucino init                             // init Blucino system
// usage:  bl_init(NULL,when,verbose)       // init Bluccino
//         bl_init(bl_gear,when_gear,0)     // init submodule
//==============================================================================

  int bl_init(BL_fct module,BL_fct cb)
  {
    return bl_sys(module,INIT_,cb,0);     // init module
  }

//==============================================================================
// Bluccino tick: send [SYS:TICK count] message to given module
// - usage: bl_tick(module,count)           // ticking a given module
//==============================================================================

  int bl_tick(BL_fct module, int id, int cnt)
  {
    return bl_post(module,TICK_,id,cnt);  // post [SYS:TICK cnt] message
  }

//==============================================================================
// Bluccino tock: send [SYS:TOCK count] message to given module
// - usage: bl_tock(module,count)           // tocking a given module
//==============================================================================

  int bl_tock(BL_fct module, int id, int cnt)
  {
    return bl_post(module,TOCK_,id,cnt);  // post [SYS:TOCK cnt] message
  }

//==============================================================================
// run app with given tick/tock periods and provided when-callback
// - usage: bl_run(app,10,100,when)   // run app with 10/1000 tick/tock periods
//==============================================================================

  void bl_run(BL_fct app, int tick_ms, int tock_ms, BL_fct when)
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

//==============================================================================
// BLUCCINO public module interface
//==============================================================================
//
// (!) := (<parent>); (O) := (<out>); (#) := (BL_HW)
//
//                  +--------------------+
//                  |      BLUCCINO      |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (!)->     INIT ->|       <out>        | init module, store <out> callback
// (!)->     TICK ->|      @id,cnt       |
// (!)->     TOCK ->|      @id,cnt       |
// (!)->      OUT ->|       <out>        | set <out> callback
//                  +--------------------+
//
//==============================================================================

  int bluccino(BL_ob *o, int val)
  {
    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):
        out = o->data;

          // we need to initialize everything what is downstairs (on driver
          // level). All stuff downstairs has to send messages through the
          // upward gear

        return bl_init(bl_down,bl_up); // forward to BL_DOWN gear, output=>BL_UP

      case BL_ID(_SYS,TICK_):
      case BL_ID(_SYS,TOCK_):
        return bl_down(o,val);         // forward to BL_CORE module

      case BL_ID(_SYS,OUT_):
        out = o->data;
        return 0;

      default:
        return -1;                     // bad command
    }
  }
