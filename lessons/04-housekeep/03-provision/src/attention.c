//==============================================================================
// attention.c
// attention house keeping
//
// Created by Hugo Pristauz on 2022-Apr-14
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "attention.h"

   static bool att_mode = false;
   static int id = 1;                  // LED @1

//==============================================================================
// worker: tick()
//==============================================================================

  static int tick(BL_ob *o, int val)   // public attention interface
  {
    static BL_ms timestamp = 0;
    BL_ms now = bl_ms();

    if ( now >= timestamp && att_mode) // 500 ms period and attention mode?
    {
      timestamp = now + 800;           // next time processing in 800ms
      bl_led(id,-1);                   // toggle status LED
    }

    return 0;
  }

//==============================================================================
//
// (A) := (APP)
//                  +--------------------+
//                  |      ATTENTION     |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (A)->     INIT ->|       <out>        | init module, store <out> callback
// (A)->     TICK ->|      @id,cnt       | tick module
//                  +--------------------+
//                  |       MESH:        | MESH interface
// (A)->     INIT ->|       <out>        | init module, store <out> callback
//                  +--------------------+
//                  |        GET:        | GET interface
// (P)->      ATT ->|                    | ok = [GET:PRV] // are we provisioned
//                  +--------------------+
//
//==============================================================================

  int attention(BL_ob *o, int val)     // public attention interface
  {
    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <cb>]
        bl_logo(2,BL_C"attention:",o,val);
        return 0;                      // OK (nothing to init)

      case BL_ID(_SYS,TICK_):          // [SYS:TICK @id,cnt]
        return tick(o,val);            // forward to tick() worker

      case BL_ID(_MESH,ATT_):
        if (att_mode && !val)
          bl_led(id,0);                // turn off status LED
        att_mode = val;                // refresh attention mode
	return 0;                      // OK

      case BL_ID(_GET,ATT_):           // ok = [GET:ATT]
        return att_mode;               // return attention mode

      default:
        return -1;                     // bad input
    }
  }
