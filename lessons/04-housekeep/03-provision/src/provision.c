//==============================================================================
// provision.c
// provision house keeping
//
// Created by Hugo Pristauz on 2022-Apr-14
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "attention.h"
  #include "provision.h"

  static bool prv_mode = false;
  static int id = 1;                   // LED @1

//==============================================================================
// worker: tick()
//==============================================================================

  static int tick(BL_ob *o, int val)   // public attention interface
  {
    static bool onoff = false;
    int modulus = prv_mode ? 500 : 50;

      // att_mode = ( (ATTENTION)<-[GET:ATT] )
      // bool att_mode = bl_msg((attention), _GET,ATT_, 0,NULL,0);

    bool att_mode = bl_get(attention,ATT_);

    if ( !att_mode )
    {
      if (val % modulus == 0)
      {
        bl_led(id,1);
        onoff = true;
      }
      else
      {
        if (onoff == true)
	        bl_led(id,0);
        onoff = false;
      }
    }

    return 0;
  }

//==============================================================================
//
// (A) := (APP)
//                  +--------------------+
//                  |      PROVISION     |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (A)->     INIT ->|       <out>        | init module, store <out> callback
// (A)->     TICK ->|      @id,cnt       | tick module
//                  +--------------------+
//                  |       MESH:        | MESH interface
// (A)->     INIT ->|       <out>        | init module, store <out> callback
//                  +--------------------+
//
//==============================================================================

  int provision(BL_ob *o, int val)     // public interface
  {
    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <cb>]
        bl_logo(2,BL_C"provision:",o,val);
        return 0;                      // OK (nothing to init)

      case BL_ID(_SYS,TICK_):          // [SYS:TICK @id,cnt]
        return tick(o,val);            // forward to tick() worker

      case BL_ID(_MESH,PRV_):
	      if (prv_mode && !val)
	        bl_led(id,0);                // turn off status LED
        prv_mode = val;                // refresh attention mode
        return 0;                      // OK

      default:
        return -1;                     // bad input
    }
  }
