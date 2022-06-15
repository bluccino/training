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

  #define WHO                     "gear:"

  #define LOG                     LOG_GEAR
  #define LOGO(lvl,col,o,val)     LOGO_GEAR(lvl,col WHO,o,val)
  #define LOG0(lvl,col,o,val)     LOGO_GEAR(lvl,col,o,val)

//==============================================================================
// worker: handle [MESH:ATT] and [MESH:PRV] events
// - usage: err = bl_node(o,val)
//==============================================================================

  __weak int bl_attprv(BL_ob *o, int val)
  {
    static bool attention = false;
    static bool provision = false;

    if (o->cl != _MESH)                // pre-check before actual dispatching
      return 0;                        // fast return

    switch (bl_id(o))                  // dispatch event
    {
      case BL_ID(_MESH,ATT_):          // attention state changed
        attention = (val != 0);
        bl_decor(attention,provision);
        LOG(2,BL_G "bl_top: attention %s",val?"on":"off");
        return 0;

      case BL_ID(_MESH,PRV_):          // provision state changed
        provision = (val != 0);
        bl_decor(attention,provision);
        LOG(2,BL_M"bl_top: node %sprovision",val?"":"un");
        return 0;

      default:
        LOGO(2,BL_R "bl_mesh: not handeled:",o,val);
        return -1;                     // did not handle
    }
  }

//==============================================================================
// event message output (message emission of a module)
// - usage: bl_out(o,val,(to))   // output to given module
// - important note: class tags are automatically un-augmented before posting
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================

  __weak int bl_out(BL_ob *o, int val, BL_oval to)
  {
    if (!to)                           // is a valid <out> callback provided?
      return 0;

      // augmented class tag? (aug bit set) => need to duplicate object with
      // de-augmented class tag before forwarding

    if ( !BL_ISAUG(o->cl) )            // easy for un-augmented class tags!
      return to(o,val);                // forward event message to subscriber
    else                               // forward with un-augmented class tag
      return bl_msg((to), BL_UNAUG(o->cl),o->op, o->id,o->data,val);
  }

//==============================================================================
// augmented event message output (message emission of a module)
// - usage: _bl_out(o,val,(to))   // output to given module
// - important note: class tags are automatically augmented before posting
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================

  __weak int _bl_out(BL_ob *o, int val, BL_oval to)
  {
    if (!to)                       // is a valid <out> callback provided?
      return 0;

      // augmented class tag? (aug bit set) => need to duplicate object with
      // de-augmented class tag before forwarding

    if ( BL_ISAUG(o->cl) )             // easy for un-augmented class tags!
      return to(o,val);            // forward event message to subscriber
    else                               // forward with un-augmented class tag
      return bl_msg((to), BL_AUG(o->cl),o->op, o->id,o->data,val);
  }

//==============================================================================
// auxillary emission function
// - used by bl_top to output to app module
// - all messages except [SYS:] messages to be forwarded to app
//==============================================================================

  int bl_emit(BL_ob *o, int val)
  {
    static BL_oval O = NULL;
    if (o->cl == _SYS)
    {
      if (o->op == INIT_)
        O = o->data;
      return 0;
    }

    return (O) ? O(o,val) : 0;
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
    static BL_oval T = bl_top;         // outputs to BL_TOP by default

    LOG0(3,"up:",o,val);

		switch (bl_id(o))
		{
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <out>]
        T = bl_cb(o,(T),"bl_up:(T)");  // store <out> callback
			  return 0;

			default:
        return bl_out(o,val,(T));      // output to bl_top by default
		}
  }

//==============================================================================
// BL_TOP: input a message to Bluccino API
// - top gear's role is to distribute messages between app level modules
// - any unaugmented message posted to the top gear is posted to the app level
// - augmented messages posted to the top gear are forwarded to the down gear
// - also housekeeps attention and provision status and controls log colors,
//   i.e. [MESH:ATT] and [MESH:PRV] messages, received from up gear, are pro-
//   cessed by the top gear in order to control log timestamp colors
// - bl_top() is defined as weak and can be overloaded
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================
//
// (A) := (app);  (M) := (main);  (*) := (<any>)
//
//                  +--------------------+
//                  |       bl_top       | Bluccino top gear
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (M)->     INIT ->|       <out>        | init module, store <out> callback
//                  +--------------------+
//                  |       MESH:        | MESH input interface
// (U)->      ATT ->|        sts         | notiy attention status
// (U)->      PRV ->|        sts         | notiy provision status
//                  |....................|
//                  |       #MESH:       | MESH output interface
// (A)<-      ATT <-|        sts         | notiy attention status
// (A)<-      PRV <-|        sts         | notiy provision status
//                  +--------------------+
//                  |      <OTHER>:      | <OTHER> input/output interface
// (*)->        * ->|        ...         | any other input event message will
// (A)<-        * <-|        ...         | be forwarded to app
//                  +--------------------+
//
//==============================================================================

  __weak int bl_top(BL_ob *o, int val)
  {
    bl_attprv(o,val);                  // handle [MESH:ATT]/[MESH:PRV] events
    return bl_emit(o,val);             // emit all messages except [SYS:] msg's
  }
