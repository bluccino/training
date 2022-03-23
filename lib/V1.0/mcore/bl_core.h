//==============================================================================
// bl_core.h
// multi model mesh demo based mesh core
//
// Created by Hugo Pristauz on 2022-Jan-02
// Copyright © 2022 Bluccino. All rights reserved.

//  Reset system
//  - after system init the app calls [RESET:INC <timeout>] which increments
//    the reset counter (non-volatile memory), while a timer with <timeout>
//    is started at the same time, which emits the event [RESET:DUE] after
//    timeout
//  - The call [RESET:INC] returns the current value of the reset counter. Based
//    on this value the application may give an indication to the user (e.g.
//    different kinf of LED pattern depending on the value, or may invoke a
//    mesh node reset [RESET:PRV] to unprovision the node
//==============================================================================
// mcore derived from:
// Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
// Copyright (c) 2018 Vikrant More
// SPDX-License-Identifier: Apache-2.0
//==============================================================================

#ifndef __BL_CORE_H__
#define __BL_CORE_H__

  #include "bluccino.h"

//==============================================================================
// migration defaults
//==============================================================================

  #ifndef MIGRATION_STEP1
    #define MIGRATION_STEP1         1
  #endif
  #ifndef MIGRATION_STEP2
    #define MIGRATION_STEP2         1
  #endif
  #ifndef MIGRATION_STEP3
    #define MIGRATION_STEP3         1
  #endif
  #ifndef MIGRATION_STEP4
    #define MIGRATION_STEP4         1
  #endif
  #ifndef MIGRATION_STEP5
    #define MIGRATION_STEP5         1
  #endif
  #ifndef MIGRATION_STEP6
    #define MIGRATION_STEP6         1
  #endif

//==============================================================================
// public module interface
//==============================================================================
//
// BL_CORE Interfaces:
//   [] = SYS(INIT,TICK,TOCK)
//   [PRV,ATT] = SET(PRV,ATT)
//   [DUE] = RESET(#DUE,INC,PRV)
//   [] = BUTTON(INC,PRV)
//   [] = SWITCH(STS)
//                                +-------------+
//                                |   BL_CORE   |
//                                +-------------+
//                         INIT ->|     SYS:    |
//                         TICK ->|             |
//                         TOCK ->|             |
//                                +-------------+
//                          PRV ->|     SET:    |-> PRV
//                          ATT ->|             |-> ATT
//                                +-------------+
//                         #DUE ->|    RESET:   |-> DUE
//                          INC ->|             |
//                          PRV ->|             |
//                                +-------------+
//                                |   BUTTON:   |-> PRESS
//                                |             |-> RELEASE
//                                +-------------+
//                                |   SWITCH:   |-> STS
//                                +-------------+
//
//  Input Messages:
//    - [SYS:INIT <cb>]                init module
//    - [SYS:TICK @id cnt]             tick the module
//    - [SYS:TOCK @id cnt]             tock the module
//    - [SET:PRV val]                  provision on/off
//    - [SET:ATT val]                  attention on/off
//    - [RESET:#DUE]                   reset timer is due
//    - [RESET:INC <ms>]               inc reset counter & set due (<ms>) timer
//    - [RESET:PRV]                    unprovision node
//
//  Output Messages:
//    - [SET:PRV val]                  provision on/off
//    - [SET:ATT val]                  attention on/off
//    - [RESET:DUE]                    reset timer due notification
//    - [BUTTON:PRESS @id 1]           button press @ channel @id
//    - [BUTTON:RELEASE @id 1]         button release @ channel @id
//    - [SWITCH:STS @id,onoff]         output switch status update
//
//==============================================================================

  int bl_core(BL_ob *o, int val);      // public module interface

#endif // __BL_CORE_H__
