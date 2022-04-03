//==============================================================================
// bl_core.c (weak Bluccino core - used as the default)
// Bluccino default core, supporting hardware (HW) and wireless (WL) core
//
// Created by Hugo Pristauz on 2022-Apr-02
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_hw.h"                   // hardware core
  #include "bl_wl.h"                   // wireless core

//==============================================================================
// CORE level logging shorthands
//==============================================================================

  #define LOG                     LOG_CORE
  #define LOGO(lvl,col,o,val)     LOGO_CORE(lvl,col"core:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_CORE(lvl,col,o,val)

//==============================================================================
// hardware core (weak defaults - public module interface)
//==============================================================================
//
// (B) := (BL_HWBUT);  (L) := (BL_HWLED);  (v) := (BL_DOWN);  (^) := (BL_UP)
//
//                  +--------------------+
//                  |       BL_HW        |
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (v)->     INIT ->|       <out>        | init module, store <out> callback
// (v)->     TICK ->|       @id,cnt      | tick the module
// (!)->      CFG ->|        mask        | config module
//                  |        SYS:        | SYS output interface
// (L,B)<-   INIT <-|       <out>        | init module, store <out> callback
// (L,B)->   TICK <-|       @id,cnt      | tick the module
// (B)<-      CFG <-|        mask        | config module
//                  +--------------------+
//                  |        LED:        | LED: input interface
// (v)->      SET ->|      @id,onoff     | set LED @id on/off (i=0..4)
// (v)->   TOGGLE ->|                    | toggle LED @id (i=0..4)
//                  |        LED:        | LED: output interface
// (L)<-      SET <-|      @id,onoff     | set LED @id on/off (i=0..4)
// (L)<-   TOGGLE <-|                    | toggle LED @id (i=0..4)
//                  +--------------------+
//                  |       BUTTON:      | BUTTON output interface
// (^)<-    PRESS <-|        @id,0       | button @id pressed (rising edge)
// (^)<-  RELEASE <-|        @id,ms      | button release after elapsed ms-time
// (^)<-    CLICK <-|       @id,cnt      | button @id clicked (cnt: nmb. clicks)
// (^)<-     HOLD <-|       @id,time     | button @id held (time: hold time)
// (B)<-      CFG <-|        mask        | config button event mask
//                  |       BUTTON:      | BUTTON input interface
// (B)->    PRESS ->|        @id,1       | button @id pressed (rising edge)
// (B)->  RELEASE ->|        @id,ms      | button release after elapsed ms-time
// (B)->    CLICK ->|       @id,cnt      | button @id clicked (cnt: nmb. clicks)
// (B)->     HOLD ->|       @id,time     | button @id held (time: hold time)
// (v)->      CFG ->|        mask        | config button event mask
//                  +--------------------+
//                  |       SWITCH:      | SWITCH: output interface
// (^)<-      STS <-|       @id,sts      | on/off status update of switch @id
//                  |       SWITCH:      | SWITCH: input interface
// (B)->      STS ->|       @id,sts      | on/off status update of switch @id
//                  +--------------------+
//
//==============================================================================

  __weak int bl_hwbut(BL_ob *o, int val) { return -1; }
  __weak int bl_hwled(BL_ob *o, int val) { return -1; }

  __weak int bl_hw(BL_ob *o, int val)
  {
    switch (o->cl)
    {
      case _SYS:
        bl_hwbut(o,val);
        bl_hwled(o,val);
        return 0;                      // OK

      case _LED:
        return bl_hwled(o,val);

      case _BUTTON:
			case _SWITCH:
        return bl_hwbut(o,val);

      default:
        return -1;                     // not supported by default
    }
    return -1;
  }

//==============================================================================
// wireless core (weak defaults)
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
//                  |      GOOCLI:       | GOOCLI public interface (gen. on/off)
// (v)->      SET ->|  @id,val,<BL_goo>  | publish [GOOCLI:SET] mesh message
// (v)->      LET ->|  @id,val,<BL_goo>  | publish [GOOCLI:LET] mesh message
// (v)->      GET ->|  @id,val,<BL_goo>  | publish [GOOCLI:GET] mesh message
// (^)<-      STS <-|  @id,val,<BL_goo>  | receive [GOOCLI:STS] mesh message
//                  +--------------------+
//                  |      GOOSRV:       | GOOSRV public interface (gen. on/off)
// (^)<-      STS <-|  @id,val,<BL_goo>  | notify GOOSRV status update
//                  +--------------------+
//                  |      GLVCLI:       | GLVCLI public interface (gen. level)
// (v)->      SET ->|  @id,val,<BL_glv>  | publish [GLVCLI:SET] mesh message
// (v)->      LET ->|  @id,val,<BL_glv>  | publish [GLVCLI:LET] mesh message
// (v)->      GET ->|  @id,val,<BL_glv>  | publish [GLVCLI:GET] mesh message
// (^)<-      STS <-|  @id,val,<BL_glv>  | receive [GLVCLI:STS] mesh message
//                  +--------------------+
//                  |      GLVSRV:       | GLVSRV public interface (gen. level)
// (^)<-      STS <-|  @id,val,<BL_glv>  | notify GLVSRV status update
//                  +====================+
//                  |      #GOOSRV:      | GOOSRV private ifc. (generic on/off)
// (#)->      STS ->|  @id,val,<BL_goo>  | notify GOOSRV status update
//                  +--------------------+
//                  |      #GLVSRV:      | GOOSRV private ifc. (generic level)
// (#)->      STS ->|  @id,val,<BL_goo>  | notify GLVSRV status update
//                  +--------------------+
//
//==============================================================================

  __weak int bl_wl(BL_ob *o, int val) { return -1; }

//==============================================================================
// public module interface
//==============================================================================
//
// (H) := (BL_HW);  (W) := (BL_WL);  (v) := (BL_DOWN);  (^) := (BL_UP)
//
//                  +--------------------+
//                  |      BL_CORE       |
//                  +--------------------+
// (v)->          ->|        SYS:        | SYS (system) input interface
// (H,W)<-        <-|        SYS:        | SYS (system) output interface
//                  +--------------------+
// (v)->          ->|        LED:        | LED input interface
// (H)<-          <-|        LED:        | LED output interface
//                  +--------------------+
// (^)->          ->|       BUTTON:      | BUTTON input interface
// (H)<-          <-|       BUTTON:      | BUTTON output interface
//                  +--------------------+
// (^)<-          <-|       SWITCH:      | SWITCH interface (output only)
//                  +--------------------+
// (^)<-          <-|        MESH:       | MESH output interface (to reset node)
// (W)->          ->|        MESH:       | MESH input interface (to reset node)
//                  +--------------------+
// (v)->          ->|       RESET:       | RESET input interface (to reset node)
// (W)<-          <-|       RESET:       | RESET output interface (to reset node)
//                  +--------------------+
// (v)->          ->|        NVM:        | NVM input ifc. (non volatile memory)
// (W)<-          <-|        NVM:        | NVM output ifc. (non volatile memory)
//                  +--------------------+
// (v)->          ->|       CFGCLI:      | CFGCLI interface (config client)
// (W)<-          <-|       CFGCLI:      | CFGCLI interface (config client)
//                  +--------------------+
// (^)<-          <-|       CFGSRV:      | CFGSRV interface (config server)
// (W)->          ->|       CFGSRV:      | CFGSRV interface (config server)
//                  +--------------------+
// (v)->          ->|       HEACLI:      | HEACLI interface (health client)
// (W)<-          <-|       HEACLI:      | HEACLI interface (health client)
//                  +--------------------+
// (^)<-          <-|       HEASRV:      | HEASRV interface (health server)
// (W)->          ->|       HEASRV:      | HEASRV interface (health server)
//                  +--------------------+
// (v)->          ->|       GOOCLI:      | GOOCLI interface (generic on/off cli)
// (W)<-          <-|       GOOCLI:      | GOOCLI interface (generic on/off cli)
//                  +--------------------+
// (^)<-          <-|       GOOSRV:      | GOOSRV interface (generic on/off srv)
// (W)->          ->|       GOOSRV:      | GOOSRV interface (generic on/off srv)
//                  +--------------------+
// (W)<-          <-|       GLVCLI:      | GLVCLI interface (generic level cli)
// (v)->          ->|       GLVCLI:      | GLVCLI interface (generic level cli)
//                  +--------------------+
// (^)<-          <-|       GLVSRV:      | GLVSRV interface (generic level srv)
// (W)->          ->|       GLVSRV:      | GLVSRV interface (generic level srv)
//                  +--------------------+
//
//==============================================================================

  __weak int bl_core(BL_ob *o, int val)
  {
    switch (o->cl)
    {
      case _SYS:                       // SYSTEM interface
        if (bl_is(o,_SYS,INIT_))
	  LOG(2,BL_B "init core ..."); // init Bluccino core

        bl_hw(o,val);                  // forward to hardware core
        bl_wl(o,val);                  // forward to wireless core
        return 0;                      // OK

      case _SET:                       // SET interface
      case _GET:                       // GET interface
        bl_hw(o,val);                  // forward to hardware core
        bl_wl(o,val);                  // forward to wireless core
        return 0;                      // OK

      case _LED:                       // LED interface
      case _BUTTON:                    // BUTTON interface
      case _SWITCH:                    // SWITCH interface
        return bl_hw(o,val);           // forward to hardware core

      case _RESET:                     // RESET interface (reset mesh node)
      case _NVM:                       // NVM interface (non volatile memory)
        return bl_wl(o,val);           // forward to wireless core

      case _CFGCLI:                    // config client
      case _CFGSRV:                    // config server
      case _HEACLI:                    // health client
      case _HEASRV:                    // health server
      case _GOOCLI:                    // generic on/off client
      case _GOOSRV:                    // generic on/off server
      case _GLVCLI:                    // generic level client
      case _GLVSRV:                    // generic level server
        return bl_wl(o,val);           // forward to wireless core

      default:
        return -1;                     // not supported by default
    }
  }
