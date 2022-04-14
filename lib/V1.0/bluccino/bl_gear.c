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
      LOG0(3,"down:",o,val);           // not suppressed messages are logged

    return bl_core(o,val);             // forward down to BL_CORE module
  }

//==============================================================================
// message upward posting to API layer (default/__weak)
// - bl_up() is defined as weak and can be overloaded
// - by default all messages posted to BL_UP are forwarded to BL_TOP
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================

  __weak int bl_up(BL_ob *o, int val)
  {
		static BL_oval out = bl_top;        // outputs to BL_TOP by default

    LOG0(3,"up:",o,val);

		switch (bl_id(o))
		{
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <out>]
        out = o->data;                 // store <out> callback
			  return 0;

			default:
        return bl_out(o,val,out);      // post to BL_TOP by default
		}
  }

//==============================================================================
// BL_TOP: input a message to Bluccino API
// - bl_top() is defined as weak and can be overloaded
// - also housekeeps attention and provision status and controls log colors
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================
//
// (*) := (<any>); (U) := (BL_UP); (#) := (BL_HW); (D) := (BL_DOWN);
// (I) := (BL_TOP); (O) := (<when>);  (B) := (BLUCCINO)
//
//                  +--------------------+
//                  |       BL_TOP       | Bluccino top gear
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (B)->     INIT ->|       <out>        | init module, store <out> callback
// (B)->     TICK ->|      @id,cnt       | tick module
// (B)->     TOCK ->|      @id,cnt       | tock module
//                  |....................|
//                  |        SYS:        | SYS input interface
// (D)<-     INIT <-|       <out>        | init module, store <out> callback
// (D)<-     TICK <-|      @id,cnt       | tick module
// (D)<-     TOCK <-|      @id,cnt       | tock module
//                  +--------------------+
//                  |       MESH:        | MESH upper interface
// (O)<-      ATT <-|        sts         | notiy attention status
// (O)<-      PRV <-|        sts         | notiy provision status
//                  |....................|
//                  |       MESH:        | MESH lower interface
// (U)->      ATT ->|        sts         | notiy attention status
// (U)->      PRV ->|        sts         | notiy provision status
//                  +--------------------+
//                  |        GET:        | GET upper interface
// (*)->      ATT ->|        sts         | get attention status
// (*)->      PRV ->|        sts         | get provision status
//                  |....................|
//                  |        GET:        | GET lower interface
// (D)<-      ATT <-|        sts         | get attention status
// (D)<-      PRV <-|        sts         | get provision status
//                  +--------------------+
//                  |        LED:        | LED upper interface
// (*)->      SET ->|     @id,onoff      | turn LED @id on/off
// (*)->   TOGGLE <-|        @id         | toggle LED @id
//                  |....................|
//                  |        LED:        | LED lower interface
// (D)<-      SET <-|     @id,onoff      | turn LED @id on/off
// (D)<-   TOGGLE <-|        @id         | toggle LED @id
//                  +--------------------+
//                  |        SCAN:       | SCAN upper interface
// (O)<-      ADV <-|      <BL_adv>      | forward advertising data
//                  |....................|
//                  |        SCAN:       | SCAN lower interface
// (U)->      ADV ->|      <BL_adv>      | forward advertising data
//                  +--------------------+
//                  |        NVM:        | NVM: upper interface
// (O)<-    READY <-|       ready        | notification that NVM is now ready
// (*)->    STORE ->|      @id,val       | store value in NVM at location @id
// (*)->   RECALL ->|        @id         | recall value in NVM at location @id
// (*)->     SAVE ->|                    | save NVM cache to NVM
//                  |....................|
//                  |        NVM:        | NVM lower interface
// (U)->    READY ->|       ready        | notification that NVM is now ready
// (D)<-    STORE <-|      @id,val       | store value in NVM at location @id
// (D)<-   RECALL <-|        @id         | recall value in NVM at location @id
// (D)<-     SAVE <-|                    | save NVM cache to NVM
//                  +--------------------+
//                  |       GOOCLI:      | GOOCLI upper interface
// (*)->      SET ->| @id,<BL_goo>,onoff | post generic on/off SET msg. via mesh
// (*)->      LET ->| @id,<BL_goo>,onoff | post generic on/off LET msg. via mesh
// (*)->      GET ->|        @id         | post generic on/off GET msg. via mesh
// (O)<-      STS <-| @id,<BL_goo>,onoff | receive gen. on/off status from mesh
//                  |....................|
//                  |       GOOCLI:      | GOOCLI lower interface
// (D)<-      SET <-| @id,<BL_goo>,onoff | post generic on/off SET msg. via mesh
// (D)<-      LET <-| @id,<BL_goo>,onoff | post generic on/off LET msg. via mesh
// (D)<-      GET <-|        @id         | post generic on/off GET msg. via mesh
// (U)->      STS ->| @id,<BL_goo>,onoff | receive gen. on/off status from mesh
//                  +--------------------+
//                  |       GOOSRV:      | GOOSRV upper interface
// (O)<-      STS <-| @id,<BL_goo>,onoff | receive gen. on/off status from mesh
//                  |....................|
//                  |       GOOSRV:      | GOOSRV lower interface
// (U)->      STS ->| @id,<BL_goo>,onoff | receive gen. on/off status from mesh
//                  +--------------------+
//
//==============================================================================

  __weak int bl_top(BL_ob *o, int val)
  {
		bool attention = false;
		bool provision = false;
		static BL_oval out = NULL;         // output a message
		static BL_oval down = bl_down;     // down-post a message
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
        LOG(2,BL_M"top: node %sprovision",val?"":"un");
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
			  if (o->id > 0)                 // @id > 0?
          LOG0(level,"@in:",o,val);    // log only for @id = 1..4
        return bl_out(o,val,down);     // post LED stuff down

      case BL_ID(_BUTTON,PRESS_):      // [BUTTON:PRESS @id,0]
      case BL_ID(_BUTTON,RELEASE_):    // [BUTTON:RELEASE @id,time]
      case BL_ID(_BUTTON,CLICK_):      // [BUTTON:CLICK @id,cnt]
      case BL_ID(_BUTTON,HOLD_):       // [BUTTON:HOLD @id,time]
      case BL_ID(_SWITCH,STS_):        // [SWITCH:STS @id,time]
        return bl_out(o,val,out);      // post BUTTON stuff without logging

      case BL_ID(_NVM,READY_):         // (U)->[NVM:READY ready]->(O)
        return bl_out(o,val,out);      // output message

      case BL_ID(_NVM,STORE_):         // (*) ->[NVM:STORE  @id,val]-> (D)
      case BL_ID(_NVM,RECALL_):        // (*) ->[NVM:RECALL @id,val]-> (D)
      case BL_ID(_NVM,SAVE_):          // (*) ->[NVM:SAVE   @id,val]-> (D)
        return bl_out(o,val,down);     // post message down

      case BL_ID(_GOOCLI,SET_):        // (*)->[GOOCLI:SET @id,<BL_goo>,v]->(D)
      case BL_ID(_GOOCLI,LET_):        // (*)->[GOOCLI:LET @id,<BL_goo>,v]->(D)
      case BL_ID(_GOOCLI,GET_):        // (*)->[GOOCLI:GET]->(D)
        return bl_out(o,val,down);     // post message down

      case BL_ID(_GOOCLI,STS_):        // (U)->[GOOCLI:STS]->(O)
      case BL_ID(_GOOSRV,STS_):        // (U)->[GOOSRV:STS]->(O)
			  LOG0(2,"top:",o,val);
        return bl_out(o,val,out);      // output message down

      default:
        break;
    }

    LOG0(level,"@in:",o,val);
    return bl_out(o,val,out);          // forward message to subscriber
  }
