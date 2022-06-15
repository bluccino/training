//==============================================================================
// bl_node.c
// mesh node house keeping (startup, provision, attention)
//
// Created by Hugo Pristauz on 2022-Feb-21
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_node.h"
  #include "bl_hw.h"

  #define PMI  bl_node                 // public module interface

//==============================================================================
// logging shorthands
//==============================================================================

  #define WHO                     "bl_node:"

  #define LOG                     LOG_NODE
  #define LOGO(lvl,col,o,val)     LOGO_NODE(lvl,col WHO,o,val)
  #define LOG0(lvl,col,o,val)     LOGO_NODE(lvl,col,o,val)

//==============================================================================
// defines (timing)
//==============================================================================

  #define T_TICK      10               // 10 ms ticks
  #define T_ATT      750               // 750 ms attention blink period
  #define T_PRV     2000               // 2000 ms provisioned blink period
  #define T_UNP      350               // 350 ms unprovisioned blink period
  #define T_STARTUP 5000               // 5000 ms startup reset interval

//==============================================================================
// locals
//==============================================================================

  static volatile int id = 0;               // THE LED id

//==============================================================================
//
// (A) := (APP)
//                  +--------------------+
//                  |      startup       |
//                  +--------------------+
//                  |        SYS:        | SYS: interface
// (#)->     INIT ->|       <out>        | init module, store <out> callback
// (#)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |       #GET:        | GET:interface
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

  static int startup(BL_ob *o, int val)     // public module interface
  {
    BL_oval output = NULL;                  // output callback
    static volatile int count = 0;          // reset counter
    static int8_t map[4] = {-1,3,4,2};      // LED @id map: [-,GREEN,BLUE,RED]

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):               // [SYS:INIT <cb>] init module
      {
        output = o->data;                   // store output callback

          // increment reset counter, start TS = 7000 ms due timer

        _bl_msg(startup,_RESET,INC_, 0,NULL,0);

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

      case _BL_ID(_RESET,INC_):             // [#RESET:INC] entry point
        count = bl_out(o,T_STARTUP,bl_down);// startup reset interval
        return 0;                           // OK

      case BL_ID(_RESET,DUE_):              // receive [RESET:DUE] event
        LOG(2,BL_B"clear reset counter");   // let us know
        bl_led(map[count],0);               // turn off LED @count+1
        count = 0;                          // deactivate startup.busy state
        return 0;                           // OK

      case _BL_ID(_GET,BUSY_):              // reset procedure busy
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
          _bl_msg(startup,_RESET,INC_, 0,NULL,0);
        }
        return 0;                           // OK

      default:
        return 0;
    }
  }

//==============================================================================
//
// (N) := (bl_node)
//                  +--------------------+
//                  |      attention     |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (N)->     INIT ->|       <out>        | init module, store <out> callback
// (N)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |        MESH:       | MESH interface
// (N)->      ATT ->|        sts         | receive & store attention status
//                  +--------------------+
//
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

      case BL_ID(_MESH,ATT_):
        state = val;                        // store attention state
        bl_led(0,0);                        // turn status LED off
        bl_led(id,0);                       // turn current LED off
        return 0;

      default:
        return -1;                          // bad args
    }
  }

//==============================================================================
// module provision (handle provision state changes and perform blinking)
//==============================================================================
//
// (N) := (bl_node)
//                  +--------------------+
//                  |     PROVISION      |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (N)->     INIT ->|       <out>        | init module, store <out> callback
// (N)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |        MESH:       | MESH interface
// (N)->      PRV ->|       onoff        | receive and store provision status
//                  +--------------------+
//
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

        if (rem > 1 || _bl_get(ATT_,(PMI)) || _bl_get(BUSY_,(PMI)))
          return 0;                         // no provision blinking during att!
        else
          return bl_led(0,rem == 0);        // update status LED @1
      }

      case BL_ID(_MESH,PRV_):               // [MESH:PRV stat] change prov state
        state = val;                        // store provision state
        bl_led(0,0);                        // turn status LED @1 off
        bl_led(id,0);                       // turn current LED @id off
        return 0;

      default:
        return -1;                          // bad args
    }
  }

//==============================================================================
// public module interface
//==============================================================================
//
// (A) := (app);  (D) := (bl_down)
//
//                  +--------------------+
//                  |       bl_node      | mesh node house keeping
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (A)->     INIT ->|       <out>        | init module, store <out> callback
// (A)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |        GET:        | GET input interface
// (D)->      PRV ->|                    | get provision status
// (D)->      ATT ->|                    | get attention status
// (A)->     BUSY ->|                    | get busy status
//                  +--------------------+
//                  |       #GET:        | GET output interface
// (D)<-      PRV <-|                    | get provision status
// (D)<-      ATT <-|                    | get attention status
//                  +--------------------+
//                  |        MESH:       | MESH interface
// (A)->      PRV ->|        sts         | receive provision status
// (A)->      ATT ->|        sts         | receive attention status
//                  +--------------------+
//                  |       BUTTON:      | BUTTON interface
// (A)->    PRESS ->|        @id,1       | receive button press messages
//                  +--------------------+
//                  |        #LED:       | LED output interface
// (D)<-      SET <-|     @id,onoff      |
//                  +--------------------+
//                  |       RESET:       | RESET interface
// (A)->      DUE ->|                    | reset counter is due
//                  +--------------------+
//
//==============================================================================

  int bl_node(BL_ob *o, int val)
  {
    static bool att = false;           // attention mode
    static bool prv = false;           // provision mode

    static BL_oval A = attention;      // attention module
    static BL_oval D = bl_down;        // down gear
    static BL_oval P = provision;      // provision module
    static BL_oval S = startup;        // startup module

    switch (bl_id(o))                  // dispatch message ID
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT state] - init system command
      case BL_ID(_SYS,TICK_):          // [SYS:TICK @id,cnt] - tick module
        bl_fwd(o,val,(S));             // forward to startup() worker
        bl_fwd(o,val,(P));             // forward to provision() worker
        bl_fwd(o,val,(A));             // forward to attention() worker
        return 0;                      // OK

      case BL_ID(_MESH,ATT_):          // [MESH,ATT sts] change attention state
        att = (val != 0);
        bl_decor(att,prv);
        LOG(2,BL_G "bl_node: attention %s",val?"on":"off");
        return bl_fwd(o,val,(A));      // set attention blinking on/off

      case BL_ID(_MESH,PRV_):          // [MESH:PRV sts]
        prv = (val != 0);
        bl_decor(att,prv);
        LOG(2,BL_M"bl_node: %sprovision",val?"":"un");
        return bl_fwd(o,val,(P));      // change provision state

      case BL_ID(_GET,BUSY_):          // busy = [GET:BUSY]
        return bl_fwd(o,val,(S));      // forward [GET:BUSY] to STARTUP

      case _BL_ID(_GET,BUSY_):         // busy = [#GET:BUSY]
        return bl_out(o,val,(S));      // output [GET:BUSY] to STARTUP

      case _BL_ID(_GET,ATT_):
      case BL_ID(_GET,ATT_):
        return att;                    // return attention state

      case _BL_ID(_GET,PRV_):
      case BL_ID(_GET,PRV_):
        return prv;                    // return provision state

      case BL_ID(_BUTTON,PRESS_):      // button press to cause LED on off
        LOGO(1,"@",o,val);
        if ( !_bl_get(PRV_,(PMI)) )    // if not provisioned
        {
          _bl_led(2,0,(PMI));          // turn off LED @2
          _bl_led(3,0,(PMI));          // turn off LED @3
          _bl_led(4,0,(PMI));          // turn off LED @4
          id = (id==0) ? 2 : (id+1)%5; // update THE LED id (=> 0 or 2,3,4)
        }
        return bl_fwd(o,val,(S));      // fwd [BUTTON:PRESS] to startup

      case _BL_ID(_LED,SET_):
        return bl_out(o,val,(D));

      case BL_ID(_RESET,DUE_):         // [RESET:DUE] - reset counter due
        return bl_fwd(o,val,(S));      // forward to startup module

      default:
        return -1;                     // bad args
    }
  }
