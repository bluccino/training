//==============================================================================
// bl_hw.h
// Bluccino wireless core supporting mesh model access, reset logic and NVM
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================

#ifndef __BL_WL_H__
#define __BL_WL_H__

//==============================================================================
// public module interface
//==============================================================================
//
// (v) := (BL_DOWN);  (^) := (BL_UP);  (#) := (BL_WL)
//                  +--------------------+
//                  |       BL_WL        | wireless core module
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
// (v)->      INC ->|         ms         | inc reset counter & set due timer
// (v)->      PRV ->|                    | unprovision node
// (^)<-      DUE <-|                    | reset timer is due
//                  +--------------------+
//                  |        NVM:        | NVM: public interface
// (v)->    STORE ->|      @id,val       | store value in NVM at location @id
// (v)->   RECALL ->|        @id         | recall value in NVM at location @id
// (v)->     SAVE ->|                    | save NVM cache to NVM
// (^)<-    READY <-|       ready        | notification that NVM is now ready
//                  +--------------------+
//                  |      GOOSRV:       |  GOOSRV interface
// (^)<-      STS <-| @id,onoff,<BL_goo> |  output generic on/off status
//                  +--------------------+
//                  |      GOOCLI:       |  GOOSRV interface
// (v)->      SET ->| @id,<BL_goo>,onoff |  punlish ack'ed generic on/off SET
// (v)->      LET ->| @id,<BL_goo>,onoff |  publish unack'ed generic on/off SET
// (v)->      GET ->|        @id         |  request GOO server status
// (^)<-      STS <-|  @id,<BL_goo>,sts  |  notify  GOO server status
//                  +--------------------+
//
//==============================================================================

  int bl_wl(BL_ob *o, int val);        // HW core module interface

#endif // __BL_HW_H__
