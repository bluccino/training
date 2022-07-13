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
#undef WHO                             // makes concatenating of .c files easier
#undef PMI                             // makes concatenating of .c files easier

#ifndef __BLUCCINO_H__
#define __BLUCCINO_H__

#ifdef __cplusplus
    #include "bl_class.h"               // include Bluccino C++ class definitions
#else

  #include "bl_type.h"
  #include "bl_app.h"                   // #include "config.h", "logging.h" ?

    // bl_app.h occasionaly includes config.h and logging.h, both of
    // them could define CFG_BLUCCINO_RTL. If no one defines CFG_BLUCCINO_RTL
    // then we povide a default here ...

	#ifndef CFG_BLUCCINO_RTL
	  #define CFG_BLUCCINO_RTL   0      // no Bluccino real time logging by default
	#endif

  #include "bl_symb.h"
	#include "bl_msg.h"
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
// (M) := (MAIN); (D) := (BL_DOWN);
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

  int bluccino(BL_ob *o, int val);

#endif // __BLUCCINO_H__
