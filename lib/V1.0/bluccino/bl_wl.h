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
// (N) := (BL_NVM);  (!) := (<parent>); (O) := (<out>); (B) := (BLE_MESH);
//                  +--------------------+
//                  |        BL_WL       | wireless core
//                  +--------------------+
//                  |        SYS:        | SYS: public interface
// (!)->     INIT ->|       @id,cnt      | init module, store <out> callback
// (!)->     TICK ->|       @id,cnt      | tick the module
// (!)->     TOCK ->|       @id,cnt      | tock the module
//                  +--------------------+
//                  |       MESH:        | MESH upper interface
// (O)<-      PRV <-|       onoff        | provision on/off
// (O)<-      ATT <-|       onoff        | attention on/off
//                  |....................|
//                  |       MESH:        | MESH lower interface
// (B)->      PRV ->|       onoff        | provision on/off
// (B)->      ATT ->|       onoff        | attention on/off
//                  +--------------------+
//                  |       RESET:       | RESET: public interface
// (O)<-      DUE <-|                    | reset timer is due
// (!)->      INC ->|         ms         | inc reset counter & set due timer
// (!)->      PRV ->|                    | unprovision node
//                  +--------------------+
//                  |        NVM:        | NVM: upper interface
// (O)<-    READY <-|       ready        | notification that NVM is now ready
// (!)->    STORE ->|      @id,val       | store value in NVM at location @id
// (!)->   RECALL ->|        @id         | recall value in NVM at location @id
// (!)->     SAVE ->|                    | save NVM cache to NVM
//                  |....................|
//                  |        NVM:        | NVM: lower interface
// (N)->    READY ->|       ready        | notification that NVM is now ready
// (N)<-    STORE <-|      @id,val       | store value in NVM at location @id
// (N)<-   RECALL <-|        @id         | recall value in NVM at location @id
// (N)<-     SAVE <-|                    | save NVM cache to NVM
//                  +====================+
//                  |       #SET:        | SET: private interface
// (#)->      PRV ->|       onoff        | provision on/off
// (#)->      ATT ->|       onoff        | attention on/off
//                  +--------------------+
//                  |      #RESET:       | RESET: private interface
// (#)->      DUE ->|                    | reset timer is due
//                  +--------------------+
//
//==============================================================================

  int bl_wl(BL_ob *o, int val);        // HW core module interface

//==============================================================================
// syntactic sugar: store value to non volatile memory (NVM)
// - usage: bl_wl_NVM_STORE(bl_in,id,val)  // store value at NVM location @id
//==============================================================================

  static inline int bl_wl_NVM_STORE(BL_oval module,int id, int val)
  {
    return bl_msg(module,_NVM,STORE_, id,NULL,val);
  }

//==============================================================================
// syntactic sugar: recall value from non volatile memory (NVM)
// - usage: val = bl_wl_NVM_RECALL(bl_in,id)    // recall val from NMV loc. @id
//==============================================================================

  static inline int bl_wl_NVM_RECALL(BL_oval module, int id)
  {
    return bl_msg(module,_NVM,RECALL_, id,NULL,0);
  }

#endif // __BL_HW_H__
