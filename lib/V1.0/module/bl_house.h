//==============================================================================
// bl_house.h
// mesh node house keeping (startup, provision, attention)
//
// Created by Hugo Pristauz on 2022-Feb-21
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

#ifndef __BL_HOUSE_H__
#define __BL_HOUSE_H__

///==============================================================================
// public module interface
//==============================================================================
//
// (A) := (APP)
//                  +--------------------+
//                  |      BL_HOUSE      | mesh node house keeping
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (A)->     INIT ->|       <out>        | init module, store <out> callback
// (A)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |        GET:        | GET interface
// (A)->      PRV ->|        sts         | get provision status
// (A)->      ATT ->|        sts         | get attention status
// (A)->     BUSY ->|        sts         | get busy status
//                  +--------------------+
//                  |        MESH:       | MESH interface
// (A)->      PRV ->|       onoff        | receive provision status
// (A)->      ATT ->|       onoff        | receive attention status
//                  +--------------------+
//                  |       BUTTON:      | BUTTON interface
// (A)->    PRESS ->|        @id,1       | receive button press messages
//                  +--------------------+
//                  |        LED:        | LED interface
// (v)<-      SET <-|     @id,onoff      |
//                  +--------------------+
//                  |       RESET:       | RESET interface
// (A)->      DUE ->|                    | reset counter is due
//                  +--------------------+
//
//==============================================================================

  int bl_house(BL_ob *o, int val);

#endif // __BL_HOUSE_H__
