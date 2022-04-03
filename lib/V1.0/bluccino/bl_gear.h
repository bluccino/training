//==============================================================================
//  bl_gear.h
//  Bluccino gear
//
//  Created by Hugo Pristauz on 2021-11-06
//  Copyright © 2021 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_GEAR_H__
#define __BL_GEAR_H__

//==============================================================================
// event message output (message emission of a module)
// - usage: bl_out(o,val,module)  // output to given module
// - important note: class tags are automatically un-augmented before posting
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================

  int bl_out(BL_ob *o, int val, BL_oval module);

//==============================================================================
// gear upward/downward and THE gear interface
//==============================================================================

  int bl_up(BL_ob *o, int value);      // gear input function
  int bl_down(BL_ob *o, int value);    // gear output function
  int bl_gear(BL_ob *o, int val);      // gear module interface

//==============================================================================
// BL_IN: input a message to Bluccino API
// - also housekeeps attention and provision status and controls log colors
// - note: bl_gear.c provides a weak implementation (redefinition possible)
//==============================================================================
//
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
//                  |....................|
//                  |        SYS:        | SYS input interface
// (v)<-     INIT <-|       <out>        | init module, store <out> callback
// (v)<-     TICK <-|      @id,cnt       | tick module
// (v)<-     TOCK <-|      @id,cnt       | tock module
//                  +--------------------+
//                  |       MESH:        | MESH upper interface
// (O)<-      ATT <-|        sts         | notiy attention status
// (O)<-      PRV <-|        sts         | notiy provision status
//                  |....................|
//                  |       MESH:        | MESH lower interface
// (^)->      ATT ->|        sts         | notiy attention status
// (^)->      PRV ->|        sts         | notiy provision status
//                  +--------------------+
//                  |        GET:        | GET upper interface
// (*)->      ATT ->|        sts         | get attention status
// (*)->      PRV ->|        sts         | get provision status
//                  |....................|
//                  |        GET:        | GET lower interface
// (v)<-      ATT <-|        sts         | get attention status
// (v)<-      PRV <-|        sts         | get provision status
//                  +--------------------+
//                  |        LED:        | LED upper interface
// (*)->      SET ->|     @id,onoff      | turn LED @id on/off
// (*)->   TOGGLE <-|        @id         | toggle LED @id
//                  |....................|
//                  |        LED:        | LED lower interface
// (v)<-      SET <-|     @id,onoff      | turn LED @id on/off
// (v)<-   TOGGLE <-|        @id         | toggle LED @id
//                  +--------------------+
//                  |        SCAN:       | SCAN upper interface
// (O)<-      ADV <-|      <BL_adv>      | forward advertising data
//                  |....................|
//                  |        SCAN:       | SCAN lower interface
// (^)->      ADV ->|      <BL_adv>      | forward advertising data
//                  +--------------------+
//                  |        NVM:        | NVM: upper interface
// (O)<-    READY <-|       ready        | notification that NVM is now ready
// (*)->    STORE ->|      @id,val       | store value in NVM at location @id
// (*)->   RECALL ->|        @id         | recall value in NVM at location @id
// (*)->     SAVE ->|                    | save NVM cache to NVM
//                  |....................|
//                  |        NVM:        | NVM lower interface
// (^)->    READY ->|       ready        | notification that NVM is now ready
// (v)<-    STORE <-|      @id,val       | store value in NVM at location @id
// (v)<-   RECALL <-|        @id         | recall value in NVM at location @id
// (v)<-     SAVE <-|                    | save NVM cache to NVM
//                  +--------------------+
//
//==============================================================================

  int bl_in(BL_ob *o, int val);        // public interface

#endif // __BL_GEAR_H__
