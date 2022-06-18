//==============================================================================
//  bl_deco.c
//  log (time stamp) decoration according to mesh attention/provision state
//
//  Created by Hugo Pristauz on 2022-06-16
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_deco.h"

//==============================================================================
// logging shorthands
//==============================================================================

  #define WHO                     "bl_deco:"

  #define LOG                     LOG_GEAR
  #define LOGO(lvl,col,o,val)     LOGO_GEAR(lvl,col WHO,o,val)

//==============================================================================
// public module interface
//==============================================================================
//
// (A) := (app);  (M) := (main);  (*) := (<any>)
//
//                  +--------------------+
//                  |       bl_deco      | log decoration
//                  +--------------------+
//                  |       MESH:        | MESH input interface
// (T)->      ATT ->|        sts         | notiy attention status
// (T)->      PRV ->|        sts         | notiy provision status
//                  +--------------------+
//                  |        GET:        | GET input interface
// (T)->      ATT ->|        sts         | retrieve attention status
// (T)->      PRV ->|        sts         | retrieve provision status
//                  +--------------------+
//
// decorate loging according to [MESH:ATT] and [MESH:PRV] events
//==============================================================================

  __weak int bl_deco(BL_ob *o, int val)
  {
    static bool att = false;
    static bool prv = false;

    switch (bl_cl(o))                  // fast check of supported nterfaces
    {
      case _MESH: break;               // MESH: interface is supported
      case _GET:  break;               // GET: interface is supported
      default: return -1;              // other interfaces not supported
    }

    switch (bl_id(o))                  // dispatch event
    {
      case BL_ID(_MESH,ATT_):          // attention state changed
        att = (val != 0);
        bl_decorate(att,prv);
        LOG(2,BL_G "bl_top: attention %s",val?"on":"off");
        return 0;

      case BL_ID(_MESH,PRV_):          // provision state changed
        prv = (val != 0);
        bl_decorate(att,prv);
        LOG(2,BL_M"bl_top: node %sprovision",val?"":"un");
        return 0;

      case BL_ID(_GET,ATT_):           // retrieve attention status
        return att;

      case BL_ID(_GET,PRV_):           // retrieve provision status
        return prv;

      default:
        LOGO(2,BL_R "bl_mesh: not handeled:",o,val);
        return -1;                     // did not handle
    }
  }

//==============================================================================
// cleanup (needed for *.c file merge of the bluccino core)
//==============================================================================

  #include "bl_clean.h"
