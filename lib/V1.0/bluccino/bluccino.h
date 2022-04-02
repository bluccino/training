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

  #endif

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

  int bl_in(BL_ob *o, int val);        // public interface

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
