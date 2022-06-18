//==============================================================================
//  bl_deco.h
//  log (time stamp) decoration according to mesh attention/provision state
//
//  Created by Hugo Pristauz on 2022-06-16
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_DECO_H__
#define __BL_DECO_H__

//==============================================================================
// message definitions
//==============================================================================
//
//                  +--------------------+
//                  |       MESH:        | MESH input interface
//            ATT --|        sts         | notiy attention status
//            PRV --|        sts         | notiy provision status
//                  +--------------------+
//                  |        GET:        | GET input interface
//            ATT --|        sts         | retrieve attention status
//            PRV --|        sts         | retrieve provision status
//                  +--------------------+
//
//==============================================================================

  #define MESH_ATT_0_0_sts   BL_ID(_MESH,ATT_)
  #define MESH_PRV_0_0_sts   BL_ID(_MESH,PRV_)
  #define GET_ATT_0_0_sts    BL_ID(_GET,ATT_)
  #define GET_PRV_0_0_sts    BL_ID(_GET,PRV_)

    // augmented messages

  #define _MESH_ATT_0_0_sts  _BL_ID(_MESH,ATT_)
  #define _MESH_PRV_0_0_sts  _BL_ID(_MESH,PRV_)
  #define _GET_ATT_0_0_sts   _BL_ID(_GET,ATT_)
  #define _GET_PRV_0_0_sts   _BL_ID(_GET,PRV_)

//==============================================================================
// public module interface
//==============================================================================

  int bl_deco(BL_ob *o, int val);

#endif // __BL_DECO_H__
