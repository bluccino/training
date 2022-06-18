//==============================================================================
// bl_node.c
// mesh node house keeping (startup, provision, attention)
//
// Created by Hugo Pristauz on 2022-Feb-21
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_node.h"
  #include "bl_reset.h"
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
// messaging short hands
//==============================================================================

  static inline int led(int id,int val) { return _bl_led(id,val,(PMI)); }
  static int rgb(int onf) { led(2,onf); led(3,onf); led(4,onf); return 0; }
  static inline int get(BL_op op)       { return _bl_get(op,(PMI)); }

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

  static volatile int id = 0;          // THE LED id

//==============================================================================
//
// (N) := (bl_node)
//                  +--------------------+
//                  |      startup       |
//                  +--------------------+
//                  |        SYS:        | SYS: interface
// (N)->     INIT ->|       <out>        | init module, store <out> callback
// (N)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |        GET:        | GET:interface
// (N)->     BUSY ->|                    | get startup busy status
//                  +--------------------+
//                  |       BUTTON:      | BUTTON input interface
// (N)->     HOLD ->|       @id,ms       | receive button hold messages
//                  +--------------------+
//                  |       RESET:       | RESET input interface
// (N)->      DUE ->|                    | reset counter is due
//                  |....................|
//                  |      #RESET:       | RESET output interface
// (N)<-      INC <-|         ms         | inc reset counter & set due timer
// (N)<-      PRV <-|                    | unprovision node (node reset)
//                  +--------------------+
//
//==============================================================================

  static int startup(BL_ob *o, int val)     // public module interface
  {
    static volatile int count = 0;          // reset counter
    static BL_oval N = bl_node;             // mesh node house keeping
    static BL_oval S = startup;             // it's me

    static int8_t map[5] = {-1,1,4,3,2};    // LED @id map: [-,STATUS,B,G,R]

    switch (bl_id(o))
    {
      case SYS_INIT_0_cb_0:
      {
          // increment reset counter, start TS = 7000 ms due timer

        count = _bl_msg((S),_RESET,INC_, 0,NULL,T_STARTUP);
        LOG(2,BL_R "[RESET:INC %d] -> count:%d",T_STARTUP,count);

        if (count <= 4)                     // <= 4 times resetted?
          return led(map[count],1);         // indicate by turn on mapped LED

        LOG(1,BL_R "unprovision node");   // let us know
        count = 0;                        // clear counter

        led(1,1);                         // status LED on
        rgb(0);                           // all RGB LEDs off

        return _bl_post((S), RESET_PRV_0_0_0, 0,NULL,T_STARTUP);
      }

      case SYS_TICK_id_BL_pace_cnt:         // receive [RESET<DUE] event
        if (count>0 && count < BL_LEN(map)) // if startup in progress
        {
          if (bl_period(o,1000))
            led(map[count],1);              // turn on LED @count+1
          else if (bl_duty(o,500,1000))
            led(map[count],0);              // turn off LED @count+1
        }
        return 0;                           // OK

      case BUTTON_HOLD_id_0_ms:             // button press during startup
        if ( !get(BUSY_) || !get(PRV_) || val || o->id != 1)
          return 0;                         // in this case ignore button hold

        if (count >= 4)
        {
          LOG(1,BL_R "unprovision node");   // let us know
          count = 0;                        // clear counter

          led(1,1);                         // status LED on
          rgb(0);                           // all RGB LEDs off

          return _bl_post((S), RESET_PRV_0_0_0, 0,NULL,T_STARTUP);
        }

        if (count > 0)                      // if we are still in startup phase
        {
          led(map[count],0);                // turn off LED @map
          count = _bl_post((S), RESET_INC_0_0_ms, 0,NULL,T_STARTUP);
          LOG(2,BL_R "[RESET:INC %d] -> count:%d",T_STARTUP,count);
        }
        return 0;                           // OK

      case RESET_DUE_0_0_0:
        LOG(2,BL_B"clear reset counter");   // let us know
        led(map[count],0);                  // turn off LED @count+1
        count = 0;                          // deactivate startup.busy state
        return 0;                           // OK

      case _RESET_INC_0_0_ms:
      case _RESET_PRV_0_0_0:
        return _bl_fwd(o,val,(N));          // forward augmented to bl_node

      case BL_ID(_GET,BUSY_):               // busy with startup sequence?
        return (count != 0);                // return busy state

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
    static volatile bool att;               // attention state

    switch (bl_id(o))
    {
      case SYS_INIT_0_cb_0:                 // [SYS:INIT @id <cb>]
        return 0;                           // OK (nothing to init)

      case SYS_TICK_id_BL_pace_cnt:
        if (att && bl_period(o,T_ATT))      // attention state/period?
        {
LOG(2,BL_G "toggle");
          rgb(-1);                          // toggle all RGB LEDs
          led(0,-1);                        // toggle status LED @0
        }
        return 0;

      case BL_ID(_MESH,ATT_):
        att = val;                          // store attention state
        bl_led(0,0);                        // turn status LED off
        rgb(0);                             // turn all RGB LEDs off
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
        if ( !get(ATT_) && !get(BUSY_) )
        {
	        int ms = (state ? T_PRV:T_UNP);   // 2000 ms versus 350 ms period
	        int duty = 100;

	        if (bl_period(o,ms))
	          led(0,1);                       // status LED @0 on
	        else if (bl_duty(o,duty,ms))
	          led(0,0);                       // status LED @0 off
        }
        return 0;
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
// (T) := (bl_top);  (D) := (bl_down)
//
//                  +--------------------+
//                  |       bl_node      | mesh node house keeping
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (T)->     INIT ->|       <out>        | init module, store <out> callback
// (T)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |        GET:        | GET input interface
// (D)->      PRV ->|                    | get provision status
// (D)->      ATT ->|                    | get attention status
// (T)->     BUSY ->|                    | get busy status
//                  +--------------------+
//                  |       #GET:        | GET output interface
// (D)<-      PRV <-|                    | get provision status
// (D)<-      ATT <-|                    | get attention status
//                  +--------------------+
//                  |        MESH:       | MESH interface
// (T)->      PRV ->|        sts         | receive provision status
// (T)->      ATT ->|        sts         | receive attention status
//                  +--------------------+
//                  |       BUTTON:      | BUTTON interface
// (T)->     HOLD ->|       @id,ms       | receive button hold messages
//                  +--------------------+
//                  |        #LED:       | LED output interface
// (D)<-      SET <-|     @id,onoff      |
//                  +--------------------+
//                  |       RESET:       | RESET interface
// (T)->      DUE ->|                    | reset counter is due
//                  |....................|
//                  |      #RESET:       | #RESET interface
// (D)<-      INC <-|         ms         | inc reset counter & set due timer
// (D)<-      PRV <-|                    | unprovision node (node reset)
//                  +--------------------+
//                  |        NODE:       | NODE input interface
// (T)->    READY ->|                    | returns node's ready status
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
        bl_decorate(att,prv);
        LOG(2,BL_G "bl_node: attention %s",val?"on":"off");
        return bl_fwd(o,val,(A));      // set attention blinking on/off

      case BL_ID(_MESH,PRV_):          // [MESH:PRV sts]
        prv = (val != 0);
        bl_decorate(att,prv);
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

      case BL_ID(_BUTTON,HOLD_):       // button press increment reset counter
        LOGO(1,"@",o,val);
        if ( !_bl_get(PRV_,(PMI)) )    // if not provisioned
        {
          _bl_led(2,0,(PMI));          // turn off LED @2
          _bl_led(3,0,(PMI));          // turn off LED @3
          _bl_led(4,0,(PMI));          // turn off LED @4
          id = (id==0) ? 2 : (id+1)%5; // update THE LED id (=> 0 or 2,3,4)
        }
        return bl_fwd(o,val,(S));      // fwd [BUTTON:HOLD] to startup

      case _BL_ID(_LED,SET_):
      case _BL_ID(_LED,TOGGLE_):
        return bl_out(o,val,(D));

      case BL_ID(_RESET,DUE_):         // [RESET:DUE] - reset counter due
        return bl_fwd(o,val,(S));      // forward to startup module

      case _RESET_INC_0_0_ms:
      case _RESET_PRV_0_0_0:
        return bl_out(o,val,(D));      // startup reset interval

      case NODE_READY_0_0_0:
        return !get(BUSY_);           // output [GET:BUSY] to STARTUP

      default:
        return -1;                     // bad args
    }
  }
