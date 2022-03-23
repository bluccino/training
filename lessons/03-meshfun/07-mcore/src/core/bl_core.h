//==============================================================================
// bl_core.h
// multi model mesh demo based mesh core
//
// Created by Hugo Pristauz on 2022-Jan-02
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================

#ifndef __BL_CORE_H__
#define __BL_CORE_H__

//==============================================================================
// public module interface
//==============================================================================
//
//                  +--------------------+
//                  |       BL_CORE      |
//                  +--------------------+
//                  |        SYS:        | SYS: public interface
// (v)->     INIT ->|       @id,cnt      | init module, store <out> callback
// (v)->     TICK ->|       @id,cnt      | tick the module
// (v)->     TOCK ->|       @id,cnt      | tock the module
//                  +--------------------+
//                  |        SET:        | SET: public interface
// (^)<-      PRV <-|       onoff        | provision on/off
// (^)<-      ATT <-|       onoff        | attention on/off
//                  +--------------------+
//                  |       RESET:       | RESET: public interface
// (^)<-      DUE <-|                    | reset timer is due
// (v)->      INC ->|         ms         | inc reset counter & set due timer
// (v)->      PRV ->|                    | unprovision node
//                  +--------------------+
//                  |        LED:        | LED: input interface
// (v)->      SET ->|      @id,onoff     | set LED @id on/off (i=0..4)
// (v)->   TOGGLE ->|                    | toggle LED @id (i=0..4)
//                  +--------------------+
//                  |       BUTTON:      | BUTTON: output interface
// (^)<-    PRESS <-|        @id,1       | button @id pressed (rising edge)
// (^)<-  RELEASE <-|        @id,0       | button @id released (falling edge)
// (^)<-    CLICK <-|       @id,cnt      | button @id clicked (cnt: nmb. clicks)
// (^)<-     HOLD <-|       @id,time     | button @id held (time: hold time)
//                  +--------------------+
//                  |       SWITCH:      | SWITCH: output interface
// (^)<-      STS <-|      @id,onoff     | on/off status update of switch @id
//                  +====================+
//
//==============================================================================

  int bl_core(BL_ob *o, int val);      // public module interface

#endif // __BL_CORE_H__
