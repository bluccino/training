//==============================================================================
//  bluccino.c
//  Bluccino overall source
//
//  Created by Hugo Pristauz on 2022-01-01
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

  #include "bl_time.c"                 // Bluccino API stuff
  #include "bl_log.c"                  // Bluccino logging stuff
	#include "bl_gear.c"                 // Bluccino gear
  #include "bl_run.c"                  // Bluccino engine (weak functions)
  #include "bl_core.c"                 // Bluccino default core (weak functions)

//==============================================================================
// BLUCCINO public module interface
//==============================================================================
//
// (!) := (<parent>); (v) := (BL_DOWN); (I) := (BL_IN);
//
//                  +--------------------+
//                  |      BLUCCINO      |
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (!)->     INIT ->|       <out>        | init module, store <out> callback
// (!)->     TICK ->|      @id,cnt       | tick module
// (!)->     TOCK ->|      @id,cnt       | tock module
// (!)->      OUT ->|       <out>        | set <out> callback
//                  +--------------------+
//                  |        SYS:        | SYS output interface
// (v,I)<-   INIT <-|       <out>        | init module, store <out> callback
// (v)<-     TICK <-|      @id,cnt       | tick module
// (!)<-     TOCK <-|      @id,cnt       | tock module
//                  +--------------------+
//
//==============================================================================

  int bluccino(BL_ob *o, int val)
  {
    static BL_oval out = NULL;         // store <out> callback

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <out>]
        out = o->data;                 // store <out> callback
        bl_init(bl_in,out);            // init BL_IN, output goes to <when>
				bl_init(bl_up,bl_in);          // init BL_UP, output goes to BL_IN
        bl_init(bl_down,bl_up);        // forward to BL_DOWN gear, output=>BL_UP
				return 0;

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
