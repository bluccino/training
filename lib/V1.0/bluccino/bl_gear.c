//==============================================================================
//  bl_gear.c
//  Bluccino gear modules
//
//  Created by Hugo Pristauz on 2022-01-01
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_core.h"

//==============================================================================
// GEAR level logging shorthands
//==============================================================================

  #define LOG                     LOG_GEAR
  #define LOGO(lvl,col,o,val)     LOGO_GEAR(lvl,col"gear:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_GEAR(lvl,col,o,val)

//==============================================================================
// event message output (message emission of a module)
// - usage: bl_out(o,val,module)  // output to given module
// - important note: class tags are automatically un-augmented before posting
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================

  __weak int bl_out(BL_ob *o, int val, BL_oval module)
  {
    if (!module)                       // is a valid <out> callback provided?
      return 0;

      // augmented class tag? (aug bit set) => need to duplicate object with
      // de-augmented class tag before forwarding

    if ( !BL_ISAUG(o->cl) )            // easy for un-augmented class tags!
      return module(o,val);            // forward event message to subscriber
    else                               // forward with un-augmented class tag
      return bl_msg(module,BL_UNAUG(o->cl),o->op, o->id,o->data,val);
  }

//==============================================================================
// message downward posting to lower level / driver layer (default/__weak)
// - bl_down() is defined as weak and can be overloaded
// - by default all messages posted to BL_DOWN are forwarded to BL_CORE
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================

  __weak int bl_down(BL_ob *o, int val)
  {
    bool nolog = bl_is(o,_LED,SET_) && o->id == 0;
    nolog = nolog || (o->cl == _SYS);

    if ( !nolog )
      LOGO(3,"down:",o,val);         // not suppressed messages are logged

    return bl_core(o,val);             // forward down to BL_CORE module
  }

//==============================================================================
// message upward posting to API layer (default/__weak)
// - bl_up() is defined as weak and can be overloaded
// - by default all messages posted to BL_UP are forwarded to BL_IN
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================

  __weak int bl_up(BL_ob *o, int val)
  {
		static BL_oval out = bl_in;        // outputs to BL_IN by default

    LOGO(3,"up:",o,val);

		switch (bl_id(o))
		{
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <out>]
        out = o->data;                 // store <out> callback
			  return 0;

			default:
        return bl_out(o,val,out);      // post to BL_IN by default
		}
  }

//==============================================================================
// BL_IN: input a message to Bluccino API
// - also housekeeps attention and provision status and controls log colors
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================
//
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
//                  |       MESH:        | MESH interface
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

      case BL_ID(_MESH,ATT_):          // attention state changed
        attention = val;
        bl_log_color(attention,provision);
        LOG(2,BL_G"attention %s",val?"on":"off");
        return bl_out(o,val,out);

      case BL_ID(_MESH,PRV_):          // provision state changed
        provision = val;
        bl_log_color(attention,provision);
        LOG(2,BL_M"node %sprovision",val?"":"un");
        return bl_out(o,val,out);

      case BL_ID(_GET,ATT_):           // [GET:ATT]
        return attention;              // return attention state

      case BL_ID(_GET,PRV_):           // [GET:PRV]
        return provision;              // return provision state

      case BL_ID(_SCAN,ADV_):          // [SCAN:ADV <BL_adv>]
        level = 5;
        break;

      case BL_ID(_LED,SET_):           // [LED:SET @id,onoff]
      case BL_ID(_LED,TOGGLE_):        // [LED:TOGGLE @id]
        LOGO(level,"@",o,val);
        return bl_down(o,val);         // post LED stuff down

      case BL_ID(_BUTTON,PRESS_):      // [BUTTON:PRESS @id,0]
      case BL_ID(_BUTTON,RELEASE_):    // [BUTTON:RELEASE @id,time]
      case BL_ID(_BUTTON,CLICK_):      // [BUTTON:CLICK @id,cnt]
      case BL_ID(_BUTTON,HOLD_):       // [BUTTON:HOLD @id,time]
      case BL_ID(_SWITCH,STS_):        // [SWITCH:STS @id,time]
        return bl_out(o,val,out);      // post BUTTON stuff without logging

      default:
        break;
    }

    LOGO(level,"@",o,val);
    return bl_out(o,val,out);          // forward message to subscriber
  }
