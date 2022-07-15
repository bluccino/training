//==============================================================================
//  bl_gear.c
//  Bluccino gear modules
//
//  Created by Hugo Pristauz on 2022-01-01
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_deco.h"
  #include "bl_core.h"

//==============================================================================
// logging shorthands
//==============================================================================

  #define WHO                     "gear:"

  #define LOG                     LOG_GEAR
  #define LOGO(lvl,col,o,val)     LOGO_GEAR(lvl,col WHO,o,val)
  #define LOG0(lvl,col,o,val)     LOGO_GEAR(lvl,col,o,val)

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

    return (O) ? (O)(o,val) : 0;
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
// - augmented messages posted to the top gear should be forwarded to the down
//   gear
// - bl_top() is defined as weak and can be overloaded
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
//                  |      <OTHER>:      | <OTHER> input/output interface
// (*)->        * ->|        ...         | any other input event message will
// (A)<-        * <-|        ...         | be forwarded to app
//                  +--------------------+
//
//==============================================================================

  __weak int bl_top(BL_ob *o, int val)
  {
    bl_deco(o,val);                    // handle [MESH:ATT]/[MESH:PRV] events
    return bl_emit(o,val);             // emit all messages except [SYS:] msg's
  }

//==============================================================================
// cleanup (needed for *.c file merge of the bluccino core)
//==============================================================================

  #include "bl_clean.h"
