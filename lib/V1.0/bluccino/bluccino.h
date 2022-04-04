//==============================================================================
//  bluccino.h
//  Bluccino overall header
//
//  Created by Hugo Pristauz on 2021-11-06
//  Copyright © 2021 Bluenetics GmbH. All rights reserved.
//==============================================================================

#undef LOG                             // makes concatenating of .c files easier
#undef LOGO                            // makes concatenating of .c files easier
#undef LOG0                            // makes concatenating of .c files easier

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
	#include "bl_msg.h"

  #ifdef BL_LOGGING
    #include "logging.h"
  #endif

  #include "bl_log.h"
  #include "bl_time.h"
  #include "bl_gear.h"
  #include "bl_run.h"
	#include "bl_sugar.h"

#endif

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

  int bluccino(BL_ob *o, int val);

#endif // __BLUCCINO_H__
