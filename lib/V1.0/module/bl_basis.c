//==============================================================================
// bl_basis.c
// basis functions of a mesh node (startup, provision, attention)
//
// Created by Hugo Pristauz on 2022-Feb-21
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_basis.h"

  #define T_TICK      10               // 10 ms ticks
  #define T_ATT      750               // 750 ms attention blink period
  #define T_PRV     2000               // 2000 ms provisioned blink period
  #define T_UNP      350               // 350 ms unprovisioned blink period
  #define T_STARTUP 5000               // 5000 ms startup reset interval

  #define _INC_     BL_HASH(INC_)      // hashed INC_ opcode

//==============================================================================
// MAIN level logging shorthands
//==============================================================================

  #define LOG                     LOG_BASIS
  #define LOGO(lvl,col,o,val)     LOGO_BASIS(lvl,col"bl_basis:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_BASIS(lvl,col,o,val)

//==============================================================================
// locals
//==============================================================================

  static volatile int id = 0;               // THE LED id

//==============================================================================
//
// (A) := (APP)
//                  +--------------------+
//                  |      STARTUP       |
//                  +--------------------+
//                  |        SYS:        | SYS: interface
// (#)->     INIT ->|       <out>        | init module, store <out> callback
// (#)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |        GET:        | GET:interface
// (#)->     BUSY ->|        sts         | get startup busy status
//                  +--------------------+
//                  |       BUTTON:      | BUTTON: interface
// (#)<-    PRESS <-|        @id,1       | receive button press messages
//                  +--------------------+
//                  |        LED:        | LED: interface
// (v)<-       SET<-|     @id,onoff      |
//                  +--------------------+
//                  |       RESET:       | RESET: interface
// (A)->      DUE ->|                    | reset counter is due
// (v)<-      PRV <-|                    | unprovision node (node reset)
//                  +--------------------+
//
//==============================================================================
// module startup (optionally reset mesh node during startup)
// - usage: cnt = startup(o,val)
// - interfaces: []=STARTUP.SYS(INIT);[]=STARTUP.RESET(DUE);
//               []=STARTUP.GET(BUSY);[]=STARTUP.BUTTON(PRESS)
// - [SYS<INIT @id,<cb>]   // init system, provide output <cb>
// - [SYS<TICK @id,ticks]  // init system, provide output <cb>
// - [HDL<INC]             // trigger to emit a [RESET:INC] event with 5s <due>
// - [RESET:INC due]       // inc/get reset counter, set reset <due> timer
// - [RESET:DUE]           // receive [RESET:DUE] message (from core)
// - [GET:BUSY]            // get startup busy state
// - [BUTTON:PRESS @id]    // receive button press event during startup
//==============================================================================

  static int startup(BL_ob *o, int val)     // public module interface
  {
    BL_fct output = NULL;                   // output callback
    static volatile int count = 0;          // reset counter
    static int8_t map[4] = {-1,3,4,2};      // LED @id map: [-,GREEN,BLUE,RED]

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):               // [SYS:INIT <cb>] init module
      {
        output = o->data;                   // store output callback

          // increment reset counter, start TS = 7000 ms due timer

        BL_ob oo = {_RESET,_INC_,0,NULL};
        startup(&oo,0);                     // post [HDL:#INC] to startup ifc

        if (count <= 3)                     // <= 3 times resetted?
          return bl_led(map[count],1);      // indicate by turn on LED @count+1

        LOG(1,BL_R"unprovision node");      // let us know
        return bl_msg(bl_down,_RESET,PRV_,0,NULL,0); // unprovision node
      }

      case BL_ID(_SYS,TICK_):               // receive [RESET<DUE] event
        if (count > 0)                      // if startup in progress
        {
          static bool old;
          int rem = val % (1000/T_TICK);    // remainder modulo (ticks/period)
          bool onoff = rem < (900/T_TICK);  // 90% of time on

          if (onoff != old)                 // LED on/odd state changed?
            bl_led(map[count],onoff);       // turn off LED @count+1

          old = onoff;                      // saveLED on/off state
        }
        return 0;                           // OK

      case BL_ID(_RESET,_INC_):             // [RESET:#INC] entry point
        count = bl_out(o,T_STARTUP,bl_down);// startup reset interval
        return 0;                           // OK

      case BL_ID(_RESET,DUE_):              // receive [RESET:DUE] event
        LOG(2,BL_B"clear reset counter");   // let us know
        bl_led(map[count],0);               // turn off LED @count+1
        count = 0;                          // deactivate startup.busy state
        return 0;                           // OK

      case BL_ID(_GET,BUSY_):               // reset procedure busy
        return (count != 0);                // return busy state

      case BL_ID(_BUTTON,PRESS_):           // button press during startup
        if (count > 3)
        {
          LOG(1,BL_R"unprovision node");    // let us know
          BL_ob oo = {_RESET,PRV_,0,NULL};  // [RESET:PRV] message
          return bl_down(&oo,0);            // unprovision node
        }

        if (count > 0)                      // if we are still in startup phase
        {
          BL_ob oo = {_RESET,_INC_,0,NULL}; // [RESET:#INC]
          startup(&oo,0);                   // post [RESET:#INC] to STARTUP mod.
        }
        return 0;                           // OK

      default:
        return 0;
    }
  }

//==============================================================================
//
// (A) := (APP)
//                  +--------------------+
//                  |      ATTENTION     |
//                  +--------------------+
//                  |        SYS:        | SYS: interface
// (#)->     INIT ->|       <out>        | init module, store <out> callback
// (#)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |        GET:        | GET:interface
// (#)->      ATT ->|        sts         | get attention status
//                  +--------------------+
//                  |        SET:        | SET: interface
// (#)->      ATT ->|        sts         | receive & store attention status
//                  +--------------------+
//
//==============================================================================
// module attention (handle attention state changes and perform blinking)
// - usage: state = attention(o,val)
// - interfaces: []=SYS(INIT,TICK); []=SET(ATT); []=GET(ATT);
// - [SYS:INIT <cb>]    // init module
// - [SYS:TICK @id,cnt] // tick input for attention blinking
// - [SET:ATT state]    // update attention state which is received from mesh
// - state = [GET:ATT]  // return current attention state
//==============================================================================

  static int attention(BL_ob *o, int val)   // public attention interface
  {
    static volatile bool state;             // attention state
    static BL_ms due = 0;

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):               // [SYS:INIT @id <cb>]
        return 0;                           // OK (nothing to init)

      case BL_ID(_SYS,TICK_):
        if (!state || !bl_due(&due,T_ATT))  // attention state? due?
          return 0;                         // neither attention state nor due
        return bl_led(0,-1);                // toggle status LED @0

      case BL_ID(_SET,ATT_):
        state = val;                        // store attention state
        bl_led(0,0);                        // turn status LED off
        bl_led(id,0);                       // turn current LED off
        return 0;

      case BL_ID(_GET,ATT_):                // state = ATTENTION[GET:ATT]
        return state;

      default:
        return -1;                          // bad args
    }
  }

//==============================================================================
//
// (A) := (APP)
//                  +--------------------+
//                  |     PROVISION      |
//                  +--------------------+
//                  |        SYS:        | SYS: interface
// (A)->     INIT ->|       <out>        | init module, store <out> callback
// (A)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |        GET:        | GET:interface
// (A)->      PRV ->|        sts         | get provision status
// (A)->      ATT ->|        sts         | get attention status
// (A)->     BUSY ->|        sts         | get busy status
//                  +--------------------+
//                  |        SET:        | SET: interface
// (#)->      PRV ->|       onoff        | receive and store provision status
//                  +--------------------+
//                  |        GET:        | GET: interface
// (#)->      PRV ->|       onoff        | retrieve provision status
//                  +--------------------+
//
//==============================================================================
// module provision (handle provision state changes and perform blinking)
// - usage: state = provision(o,val)
// - interfaces: []=SYS(INIT,TICK); []=SET(PRV); []=GET(PRV);
// - [SYS:INIT <cb>]    // init module
// - [SYS:TICK @id,cnt] // tick input for attention blinking
// - [SET:PRV state]    // set provision state which is received from mesh
// - state = [GET:PRV]  // return current provision state
//==============================================================================

  static int provision(BL_ob *o, int val)   // public provision interface
  {
    static volatile bool state = 0;         // provision state

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):               // [SYS:INIT @id <cb>]
        return 0;                           // OK (nothing to init)

      case BL_ID(_SYS,TICK_):               // [SYS:TICK @id <val>]
      {
        int ms = (state ? T_PRV:T_UNP);     // 2000 ms versus 350 ms period
        int rem = val % (ms/T_TICK);        // remainder modulo (ticks/period)

        if (rem > 1 || bl_get(attention,ATT_) || bl_get(startup,BUSY_))
          return 0;                         // no provision blinking during att!
        else
          return bl_led(0,rem == 0);        // update status LED @1
      }

      case BL_ID(_SET,PRV_):                // [SET:PRV val] change prov state
        state = val;                        // store attention state
        bl_led(0,0);                        // turn status LED @1 off
        bl_led(id,0);                       // turn current LED @id off
        return 0;

      case BL_ID(_GET,PRV_):                // state = ATTENTION[GET:ATT]
        return state;

      default:
        return -1;                          // bad args
    }
  }

//==============================================================================
// public module interface
//==============================================================================
//
// (A) := (APP)
//                  +--------------------+
//                  |      BL_BASIS      |
//                  +--------------------+
//                  |        SYS:        | SYS: interface
// (A)->     INIT ->|       <out>        | init module, store <out> callback
// (A)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |        GET:        | GET:interface
// (A)->      PRV ->|        sts         | get provision status
// (A)->      ATT ->|        sts         | get attention status
// (A)->     BUSY ->|        sts         | get busy status
//                  +--------------------+
//                  |        SET:        | SET: interface
// (A)->      PRV ->|       onoff        | receive provision status
// (A)->      ATT ->|       onoff        | receive attention status
//                  +--------------------+
//                  |       BUTTON:      | BUTTON: interface
// (A)->    PRESS ->|        @id,1       | receive button press messages
//                  +--------------------+
//                  |        LED:        | LED: interface
// (v)<-      SET <-|     @id,onoff      |
//                  +--------------------+
//                  |       RESET:       | RESET: interface
// (A)->      DUE ->|                    | reset counter is due
//                  +--------------------+
//
//==============================================================================

  int bl_basis(BL_ob *o, int val)
  {
    BL_fct output = NULL;              // to store output callback

    switch (bl_id(o))                  // dispatch message ID
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT state] - init system command
        output = o->data;              // store output callback
        startup(o,val);                // forward to startup() worker
        provision(o,val);              // forward to provision() worker
        attention(o,val);              // forward to attention() worker
        return 0;                      // OK

      case BL_ID(_SYS,TICK_):          // [SYS:TICK @id,cnt] - tick module
        startup(o,val);                // forward to startup() worker
        attention(o,val);              // forward to startup() worker
        provision(o,val);              // forward to provision() worker
        return 0;                      // OK

      case BL_ID(_SET,ATT_):           // set attention blinking on/off
        return attention(o,val);       // change attention state

      case BL_ID(_SET,PRV_):           // [MESH:ATT state]
        return provision(o,val);       // change provision state

      case BL_ID(_GET,BUSY_):          // [MESH:ATT state]
        return startup(o,val);         // change provision state

      case BL_ID(_BUTTON,PRESS_):      // button press to cause LED on off
        LOGO(1,"@",o,val);
        if ( !bl_get(provision,PRV_))  // if not provisioned
        {
          bl_led(2,0); bl_led(3,0); bl_led(4,0);// turn off current LED
          id = (id==0) ? 2 : (id+1)%5; // update THE LED id (=> 0 or 2,3,4)
        }
        return startup(o,val);         // fwd [BUTTON:PRESS] to startup

      case BL_ID(_RESET,DUE_):         // [RESET:DUE] - reset counter due
        return startup(o,val);         // forward to startup module

      default:
        return -1;                     // bad args
    }
  }
