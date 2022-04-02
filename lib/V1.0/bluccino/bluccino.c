//==============================================================================
//  bluccino.c
//  Bluccino overall source
//
//  Created by Hugo Pristauz on 2022-01-01
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

  #include "bl_api.c"                  // Bluccino API stuff
  #include "bl_log.c"                  // Bluccino logging stuff
  #include "bl_core.c"                 // Bluccino default core (weak functions)

//==============================================================================
// input a message to Bluccino API
//==============================================================================
// (*) := (<any>); (^) := (BL_UP); (#) := (BL_HW); (v) := (BL_DOWN);
// (I) := (BL_IN); (O) := (<when>)
//
//                  +--------------------+
//                  |        BL_IN       | Bluccino message input
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (!)->     INIT ->|       <out>        | init module, store <out> callback
// (!)->     TICK ->|      @id,cnt       | tick module
// (!)->     TOCK ->|      @id,cnt       | tock module
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (v)<-     INIT <-|       <out>        | init module, store <out> callback
// (v)<-     TICK <-|      @id,cnt       | tick module
// (v)<-     TOCK <-|      @id,cnt       | tock module
//                  +--------------------+
//                  |        SET:        | SET interface
// (O)<-      ATT <-|        sts         | notiy attention status
// (O)<-      PRV <-|        sts         | notiy provision status
//                  +--------------------+
//                  |        GET:        | GET interface
// (*)->      ATT ->|        sts         | get attention status
// (*)->      PRV ->|        sts         | get provision status
//                  +--------------------+
//                  |        LED:        | LED output interface
// (v)<-      SET <-|     @id,onoff      | turn LED @id on/off
// (v)<-   TOGGLE <-|        @id         | toggle LED @id
//                  +--------------------+
//                  |        LED:        | LED input interface
// (*)->      SET ->|     @id,onoff      | turn LED @id on/off
// (*)->   TOGGLE <-|        @id         | toggle LED @id
//                  +--------------------+
//                  |        SCAN:       | SCAN output interface
// (O)<-      ADV <-|      <BL_adv>      | forward advertising data
//                  +--------------------+
//                  |        SCAN:       | SCAN input interface
// (^)->      ADV ->|      <BL_adv>      | forward advertising data
//                  +--------------------+
//
//==============================================================================

  __weak int bl_in(BL_ob *o, int val)
  {
		bool attention = false;
		bool provision = false;
		static BL_oval out = NULL;
    int level = 2;                     // default verbose level

    switch (bl_id(o))                  // dispatch event
    {
      case BL_ID(_SYS,INIT_):          // provision state changed
        out = o->data;                 // store <out> callback
				return 0;

      case BL_ID(_SYS,TICK_):
      case BL_ID(_SYS,TOCK_):
        level = 5;
        break;

      case BL_ID(_SET,ATT_):          // attention state changed
        attention = val;
        bl_log_color(attention,provision);
        LOG(2,BL_G"attention %s",val?"on":"off");
        return bl_out(o,val,out);

      case BL_ID(_SET,PRV_):           // provision state changed
        provision = val;
        bl_log_color(attention,provision);
        LOG(2,BL_M"node %sprovision",val?"":"un");
        return bl_out(o,val,out);

      case BL_ID(_GET,ATT_):          // [GET:ATT]
        return attention;             // return attention state

      case BL_ID(_GET,PRV_):          // [GET:PRV]
        return provision;             // return provision state

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
        bl_init(bl_in,out);            // init BL_IN, output goes
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
