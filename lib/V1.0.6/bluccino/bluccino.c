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
// (M) := (MAIN); (D) := (BL_DOWN); (T) := (BL_TOP);  (A) := (APP)
//
//                  +--------------------+
//                  |      BLUCCINO      |
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (M)->     INIT ->|       <out>        | init module, store <out> callback
// (M)->     TICK ->|      @id,cnt       | tick module
// (M)->     TOCK ->|      @id,cnt       | tock module
// (M)->      OUT ->|       <out>        | set <out> callback
//                  +--------------------+
//                  |        SYS:        | SYS output interface
// (D)<-     INIT <-|       <out>        | init module, store <out> callback
// (D)<-     TICK <-|      @id,cnt       | tick module
// (D)<-     TOCK <-|      @id,cnt       | tock module
//                  +--------------------+
//
//==============================================================================

  int bluccino(BL_ob *o, int val)
  {
    static BL_oval A = NULL;           // store <out> callback

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <out>]
        A = o->data;                   // store <out> callback
        bl_init(bl_top,A);             // init (BL_TOP), output goes to (A)
        bl_init(bl_up,bl_top);         // init (BL_UP), output goes to (BL_TOP)
        bl_init(bl_down,bl_up);        // forward to (BL_DOWN), <out> to (BL_UP)
        return 0;

      case BL_ID(_SYS,TICK_):
      case BL_ID(_SYS,TOCK_):
        return bl_down(o,val);         // forward to down gear

      case BL_ID(_SYS,OUT_):
        A = bl_cb(o,(A),"");
        return 0;

      default:
        return -1;                     // bad command
    }
  }
