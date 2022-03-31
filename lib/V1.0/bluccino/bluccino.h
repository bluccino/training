//==============================================================================
//  bluccino.h
//  Bluccino overall header
//
//  Created by Hugo Pristauz on 2021-11-06
//  Copyright © 2021 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BLUCCINO_H__
#define __BLUCCINO_H__

  #ifdef __cplusplus
    #include "blclass.h"               // include Bluccino C++ class definitions
  #else

    #include "bl_type.h"

    #ifdef BL_CONFIG
      #include "config.h"
    #endif

    #include "bl_symb.h"

    #ifdef BL_LOGGING
      #include "logging.h"
    #endif

    #include "bl_api.h"
    #include "bl_log.h"
    #include "bl_hw.h"

  #endif

//==============================================================================
// public module interface
//==============================================================================
//
// (!) := (<parent>); (O) := (<out>); (#) := (BL_HW)
//
//                  +--------------------+
//                  |      BLUCCINO      |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (!)->     INIT ->|       <out>        | init module, store <out> callback
// (!)->     TICK ->|      @id,cnt       |
// (!)->     TOCK ->|      @id,cnt       |
// (!)->      OUT ->|       <out>        | set <out> callback
//                  +--------------------+
//
//==============================================================================

  int bluccino(BL_ob *o, int val);

#endif // __BLUCCINO_H__
