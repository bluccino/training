//==============================================================================
// bl_hw.h
// Bluccino HW core supporting basic functions for button & LED
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================

#ifndef __BL_HW_H__
#define __BL_HW_H__

//==============================================================================
// enable masks for button events
// - usage:
//     bl_cfg(bl_hw,_BUTTON,BL_PRESS)          // enable [BUTTON:PRESS]
//     bl_cfg(bl_hw,_BUTTON,BL_RELEASE)        // enable [BUTTON:RELEASE]
//     bl_cfg(bl_hw,_BUTTON,BL_EDGE)           // enable [BUTTON:PRESS/RELEASE]
//     bl_cfg(bl_hw,_BUTTON,BL_SWITCH)         // enable [BUTTON:SWITCH]
//     bl_cfg(bl_hw,_BUTTON,BL_CLICK)          // enable [BUTTON:CLICK]
//     bl_cfg(bl_hw,_BUTTON,BL_HOLD)           // enable [BUTTON:HOLD]
//
//     bl_cfg(bl_hw,_BUTTON,0xffff)            // enable all [BUTTON:] events
//     bl_cfg(bl_hw,_BUTTON,0x0000)            // disable all [BUTTON:] events
//     bl_cfg(bl_hw,_BUTTON,BL_PRESS|BL_RELEASE) // press/release events
//     bl_cfg(bl_hw,_BUTTON,BL_EDGE)           // same as above
//     bl_cfg(bl_hw,_BUTTON,BL_CLICK|BL_HOLD)  // click/hold events
//     bl_cfg(bl_hw,_BUTTON,BL_MULTI)          // same as above
//==============================================================================

  #define BL_PRESS   0x0001  // mask for [BUTTON:PRESS] events
  #define BL_RELEASE 0x0002  // mask for [BUTTON:RELEASE] events
  #define BL_EDGE    0x0003  // mask for [BUTTON:PRESS],[BUTTON:RELEASE]
  #define BL_SWITCH  0x0004  // mask for [BUTTON:SWITCH] events
  #define BL_CLICK   0x0008  // mask for [BUTTON:CLICK] events
  #define BL_HOLD    0x0010  // mask for [BUTTON:HOLD] events
  #define BL_MULTI   0x0018  // mask for [BUTTON:CLICK],[BUTTON:HOLD]
  #define BL_ALL     0xFFFF  // mask for all [BUTTON:] events
  #define BL_NONE    0x0000  // mask for no [BUTTON:] events

//==============================================================================
// [SYS:CFG mask] message definition
// - [SYS:CFG mask]  config module
//==============================================================================

  #define SYS_CFG_0_0_mask        BL_ID(_SYS,CFG_)

    // augmented messages

  #define _SYS_CFG_0_0_mask       _BL_ID(_SYS,CFG_)

//==============================================================================
// [LED:op] message definition
// - [LED:SET @id,onoff] set LED @id on/off (i=0..4)
// - [LED:TOGGLE @id] toggle LED @id (i=0..4)
//==============================================================================

  #define LED_SET_id_0_onoff      BL_ID(_LED,SET_)
  #define LED_TOGGLE_id_0_0       BL_ID(_LED,TOGGLE_)

    // augmented messages

  #define _LED_SET_id_0_onoff     _BL_ID(_LED,SET_)
  #define _LED_TOGGLE_id_0_0      _BL_ID(_LED,TOGGLE_)

//==============================================================================
// [BUTTON:op] message definition
// - [BUTTON:PRESS @id] button @id pressed (rising edge)
// - [BUTTON:RELEASE @id,ms] button @id released after ms-time
// - [BUTTON:CLICK @id,cnt] button @id clicked (cnt: number of clicks)
// - [BUTTON:HOLD @id,ms] button @id held (ms: hold ms-time)
// - [BUTTON:CFG mask] config button module
// - [BUTTON:CFG mask] set click/hold discrimination time
//==============================================================================

  #define BUTTON_PRESS_id_0_0     BL_ID(_BUTTON,PRESS_)
  #define BUTTON_RELEASE_id_0_ms  BL_ID(_BUTTON,RELEASE_)
  #define BUTTON_CLICK_id_0_cnt   BL_ID(_BUTTON,CLICK_)
  #define BUTTON_HOLD_id_0_ms     BL_ID(_BUTTON,HOLD_)
  #define BUTTON_CFG_0_0_mask     BL_ID(_BUTTON,CFG_)
  #define BUTTON_MS_0_0_ms        BL_ID(_BUTTON,MS_)

    // augmented messages

  #define _BUTTON_PRESS_id_0_0    _BL_ID(_BUTTON,PRESS_)
  #define _BUTTON_RELEASE_id_0_ms _BL_ID(_BUTTON,RELEASE_)
  #define _BUTTON_CLICK_id_0_cnt  _BL_ID(_BUTTON,CLICK_)
  #define _BUTTON_HOLD_id_0_ms    _BL_ID(_BUTTON,HOLD_)
  #define _BUTTON_CFG_0_0_mask    _BL_ID(_BUTTON,CFG_)
  #define _BUTTON_MS_0_0_ms       _BL_ID(_BUTTON,MS_)

//==============================================================================
// [SWITCH:STS @id,sts] message definition
// - [SWITCH:STS @id,sts] on/off status update of switch @id
//==============================================================================

  #define SWITCH_STS_id_0_sts     BL_ID(_SWITCH,STS_)

    // augmented messages

  #define _SWITCH_STS_id_0_sts    _BL_ID(_SWITCH,STS_)

//==============================================================================
// [NVM:] non volatile memory (NVM) message definitions
// - [NVM:LOAD <BL_nvm>] load nvm data
// - [NVM:SAVE <BL_nvm>] load nvm data
// - [NVM:READY sts] notification that NVM is now ready
// - [NVM:STORE @id,val] store value in NVM at location @id
// - [NVM:RECALL @id] recall value in NVM at location @id
// - [NVM:AVAIL] is NVM functionality available? (return ok value >= 0)
//==============================================================================

  #define NVM_LOAD_0_BL_dac_0     BL_ID(_NVM,LOAD_)
  #define NVM_SAVE_0_BL_dac_0     BL_ID(_NVM,SAVE_)
  #define NVM_STORE_id_0_val      BL_ID(_NVM,STORE_)
  #define NVM_RECALL_id_0_0       BL_ID(_NVM,RECALL_)
  #define NVM_READY_0_0_sts       BL_ID(_NVM,READY_)
  #define NVM_AVAIL_0_0_0         BL_ID(_NVM,AVAIL_)

    // augmented messages

  #define _NVM_LOAD_0_BL_dac_0    _BL_ID(_NVM,LOAD_)
  #define _NVM_SAVE_0_BL_dac_0    _BL_ID(_NVM,SAVE_)
  #define _NVM_STORE_id_0_val     _BL_ID(_NVM,STORE_)
  #define _NVM_RECALL_id_0_0      _BL_ID(_NVM,RECALL_)
  #define _NVM_READY_0_0_sts      _BL_ID(_NVM,READY_)
  #define _NVM_AVAIL_0_0_0        _BL_ID(_NVM,AVAIL_)

//==============================================================================
// public module interface
//==============================================================================
//
// (B) := (bl_hwbut);  (L) := (bl_hwled);  (D) := (bl_down);  (U) := (bl_up)
//
//                  +--------------------+
//                  |       bl_hw        |
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (D)->     INIT ->|       <out>        | init module, store <out> callback
// (D)->     TICK ->|       @id,cnt      | tick the module
// (*)->      CFG ->|        mask        | config module
//                  |....................|
//                  |        SYS:        | SYS output interface
// (L,B)<-   INIT <-|       <out>        | init module, store <out> callback
// (L,B)->   TICK <-|       @id,cnt      | tick the module
// (B)<-      CFG <-|        mask        | config module
//                  +--------------------+
//                  |        LED:        | LED: input interface
// (D)->      SET ->|      @id,onoff     | set LED @id on/off (i=0..4)
// (D)->   TOGGLE ->|        @id         | toggle LED @id (i=0..4)
//                  |....................|
//                  |        LED:        | LED: output interface
// (L)<-      SET <-|      @id,onoff     | set LED @id on/off (i=0..4)
// (L)<-   TOGGLE <-|        @id         | toggle LED @id (i=0..4)
//                  +--------------------+
//                  |       BUTTON:      | BUTTON input interface
// (B)->    PRESS ->|        @id         | button @id pressed (rising edge)
// (B)->  RELEASE ->|        @id,ms      | button release after elapsed ms-time
// (B)->    CLICK ->|       @id,cnt      | button @id clicked (cnt: nmb. clicks)
// (B)->     HOLD ->|       @id,ms       | button @id held (ms: hold ms-time)
// (D)->      CFG ->|        mask        | config button event mask
// (D)->       MS ->|         ms         | set click/hold discrimination time
//                  |....................|
//                  |       BUTTON:      | BUTTON output interface
// (U)<-    PRESS <-|        @id,ms      | button @id pressed (rising edge)
// (U)<-  RELEASE <-|        @id,ms      | button release after elapsed ms-time
// (U)<-    CLICK <-|       @id,cnt      | button @id clicked (cnt: nmb. clicks)
// (U)<-     HOLD <-|       @id,time     | button @id held (time: hold time)
// (B)<-      CFG <-|        mask        | config button event mask
// (B)<-       MS <-|         ms         | set click/hold discrimination time
//                  +--------------------+
//                  |       SWITCH:      | SWITCH input interface
// (B)->      STS ->|       @id,sts      | on/off status update of switch @id
//                  |....................|
//                  |       SWITCH:      | SWITCH output interface
// (U)<-      STS <-|       @id,sts      | on/off status update of switch @id
//                  +--------------------+
//                  |        NVM:        | NVM input interface
// (D)->     LOAD ->|      <BL_dac>      | load NVM data
// (D)->     SAVE ->|      <BL_dac>      | save NVM data
//                  +--------------------+
//
//==============================================================================

  int bl_hw(BL_ob *o, int val);        // HW core module interface

#endif // __BL_HW_H__
