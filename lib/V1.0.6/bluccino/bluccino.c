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

  #define WHO  "bluccino:"

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
    static BL_oval A = NULL;           // output to app module
    static BL_oval D = bl_down;        // output to down gear
    static BL_oval T = bl_top;         // output to top gear
    static BL_oval U = bl_up;          // output to up gear

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <out>]
        A = bl_cb(o,(A),WHO"(A)");     // store output callback

          // first init emitter (bl_emit), since down gear can send early
          // messages which requires bl_top to be able to forward messages
          // to app via bl_emit();

        bl_init(bl_emit,(A));          // output non [SYS:] message to app

          // after that we initialize up gear, down gear and top gear
          // in exactly this order

        bl_init((U),(T));              // init up gear, output to top gear
        bl_init((D),(U));              // init down gear, output to up gear
        bl_init((T),(A));              // init top gear, output to app
        return 0;

      case BL_ID(_SYS,TICK_):
      case BL_ID(_SYS,TOCK_):
        bl_fwd(o,val,(D));             // tick/tock down gear
        bl_fwd(o,val,(T));             // tick/tock top gear
        return 0;

      case BL_ID(_SYS,OUT_):
        A = bl_cb(o,(A),"");
        return 0;

      default:
        return -1;                     // bad command
    }
  }
