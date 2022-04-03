//==============================================================================
// bl_core.h
// Bluccino default core, supporting hardware (HW) and wireless (WL) core
//
// Created by Hugo Pristauz on 2022-Apr-02
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================
//
// Module Hierarchie
//
// +- BL_CORE
//    +- BL_HW (hardware core)
//    |  +- BL_HWLED
//    |  +- BL_HWBUT
//    +- BL_WL (wireless core)
//       +- BL_COMP (mesh device composition)
//       +- BL_NVM  (non volatile memory)
//
//==============================================================================
// System Message Flow Diagrams
//==============================================================================
//
// Message Flow Diagram: Initializing
//
//     BL_DOWN        BL_CORE         BL_HW          BL_WL          BL_UP
//       (v)            (#)            (H)            (W)            (^)
//        |              |              |              |              |
//        |  [SYS:INIT]  |              |              |              |
//        o------------->|  [SYS:INIT]  |              |              |
//        |              o------------->|  [SYS:INIT]  |              |
//        |              o---------------------------->|              |
//        |              |              |              |              |
//
// Message Flow Diagram: Ticking
//
//     BL_DOWN        BL_CORE         BL_HW          BL_WL          BL_UP
//       (v)            (#)            (H)            (W)            (^)
//        |              |              |              |              |
//        |  [SYS:TICK]  |              |              |              |
//        o------------->|  [SYS:TICK]  |              |              |
//        |              o------------->|  [SYS:TICK]  |              |
//        |              o---------------------------->|              |
//        |              |              |              |              |
//
// Message Flow Diagram: Tocking
//
//     BL_DOWN         BL_HW        BL_HWBUT       BL_HWLED         BL_UP
//       (v)            (#)            (B)            (L)            (^)
//        |              |              |              |              |
//        |  [SYS:TOCK]  |              |              |              |
//        o------------->|  [SYS:TOCK]  |              |              |
//        |              o------------->|  [SYS:TOCK]  |              |
//        |              o---------------------------->|              |
//        |              |              |              |              |
//
//==============================================================================
// Hardware Core Message Flow Diagrams
//==============================================================================
//
// Message Flow Diagram: Setting LED ON/OFF State
//
//     BL_DOWN        BL_CORE         BL_HW          BL_WL          BL_UP
//       (v)            (#)            (H)            (W)            (^)
//        |              |              |              |              |
//        |  [LED:SET]   |              |              |              |
//        o------------->|              |              |              |
//        |   @id,onoff  | [LED:ONOFF]  |              |              |
//        |              o------------->|              |              |
//        |              |  @id,onoff   |              |              |
//        |              |              |              |              |
//
// Message Flow Diagram: Button Configuration
//
//     BL_DOWN        BL_CORE         BL_HW          BL_WL          BL_UP
//       (v)            (#)            (H)            (W)            (^)
//        |              |              |              |              |
//        | [BUTTON:CFG] |              |              |              |
//        o------------->|              |              |              |
//        |     flags    | [BUTTON:CFG] |              |              |
//        |              o------------->|              |              |
//        |              |    flags     |              |              |
//        |              |              |              |              |
//
// Message Flow Diagram: Button Click (Button Release within Grace Time)
//
//   BL_DOWN          BL_CORE           BL_HW            BL_WL            BL_UP
//     (v)              (#)              (H)              (W)              (^)
//      |                |                |                |                |
//      |                |        +-------v-------+        |                |
//      |                |        | press button  |        |                |
//      |                |        | time[@id] = 0 |        |                |
//      |                |        +-------v-------+        |                |
//      |                |                |                |                |
//      |                | [BUTTON:PRESS] |                |                |
//      |                |<---------------o                |                |
//      |                |      @id,0     |                | [BUTTON:PRESS] |
//      |                o------------------------------------------------->|
//      |                | [BUTTON:CLICK] |                |     @id,0      |
//      |                |<---------------o                |                |
//      |                |      @id,0     |                | [BUTTON:CLICK] |
//      |                o------------------------------------------------->|
//      |                |                |                |     @id,0      |
//      |                |        +-------v--------+       |                |
//      |                |        | onoff = !onoff |       |                |
//      |                |        | (toggle switch)|       |                |
//      |                |        +-------v--------+       |                |
//      |                |                |                |                |
//      :                :                :                :                :
//      |                |                |                |                |
//      |                |                - now < grace    |                |
//      |                |        +-------v--------+       |                |
//      |                |        | release button |       |                |
//      |                |        |ms=now-time[@id]|       |                |
//      |                |        +-------v--------+       |                |
//      |                |[BUTTON:RELEASE]|                |                |
//      |                |<---------------o                |                |
//      |                |      @id,ms    |                |[BUTTON:RELEASE]|
//      |                o------------------------------------------------->|
//      |                |                |                |                |
//      |                |       +--------v--------+       |                |
//      |                |       | ms < grace time |       |                |
//      |                |       | we had 1 click  |       |                |
//      |                |       |     cnt = 1     |       |                |
//      |                |       +--------v--------+       |                |
//      |                | [BUTTON:CLICK] |                |     @id,ms     |
//      |                |<---------------o                |                |
//      |                |     @id,cnt    |                | [BUTTON:CLICK] |
//      |                o------------------------------------------------->|
//      |                |                |                |    @id,cnt     |
//      |                |                |                |                |
//
// Message Flow Diagram: Button Hold (Button Release Exceeds Grace Time)
//
//   BL_DOWN          BL_CORE           BL_HW            BL_WL            BL_UP
//     (v)              (#)              (H)              (W)              (^)
//      |                |                |                |                |
//      |                |        +-------v-------+        |                |
//      |                |        | press button  |        |                |
//      |                |        | time[@id] = 0 |        |                |
//      |                |        +-------v-------+        |                |
//      |                | [BUTTON:PRESS] |                |                |
//      |                |<---------------o                |                |
//      |                |      @id,0     |                | [BUTTON:PRESS] |
//      |                o------------------------------------------------->|
//      |                | [BUTTON:CLICK] |                |     @id,0      |
//      |                |<---------------o                |                |
//      |                |      @id,0     |                | [BUTTON:CLICK] |
//      |                o------------------------------------------------->|
//      |                |                |                |     @id,0      |
//      |                |        +-------v--------+       |                |
//      |                |        | onoff = !onoff |       |                |
//      |                |        | (toggle switch)|       |                |
//      |                |        +-------v--------+       |                |
//      |                |                |                |                |
//      :                :                :                :                :
//      |                |                |                |                |
//      |                |                - now == grace   |                |
//      |                | [BUTTON:HOLD]  |                |                |
//      |                |<---------------o                |                |
//      |                |     @id,0      |                | [BUTTON:HOLD]  |
//      |                o------------------------------------------------->|
//      |                |                |                |     @id,0      |
//      |                |        +-------v--------+       |                |
//      |                |        | release button |       |                |
//      |                |        |ms=now-time[@id]|       |                |
//      |                |        +-------v--------+       |                |
//      |                |                |                |                |
//      |                |[BUTTON:RELEASE]|                |                |
//      |                |<---------------o                |                |
//      |                |      @id,ms    |                |[BUTTON:RELEASE]|
//      |                o------------------------------------------------->|
//      |                |                |                |                |
//      |                | [BUTTON:HOLD]  |                |     @id,ms     |
//      |                |<---------------o                |                |
//      |                |     @id,ms     |                | [BUTTON:HOLD]  |
//      |                o------------------------------------------------->|
//      |                |                |                |     @id,0      |
//      |                |                |                |                |
//
//==============================================================================
// Wireless Core Message Flow Diagrams
//==============================================================================
//
// Message Flow Diagram: Attention State on/off
//
//   BL_DOWN          BL_CORE           BL_HW            BL_WL            BL_UP
//     (v)              (#)              (H)              (W)              (^)
//      |                |                |                |                |
//      |                | []             |                |                |
//      |                |<--------------------------------o                |
//      |                |                |                |                |
//      |                |                |                |                |
//      |                |                |                |                |
//      |                |                |                |                |

//==============================================================================
// public module interface
//==============================================================================
//
// (B) := (BL_HWBUT);  (L) := (BL_HWLED);  (v) := (BL_DOWN);  (^) := (BL_UP)
//
//                  +--------------------+
//                  |      BL_CORE       |
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
// dummy interface for core module public interface (default/__weak)
//==============================================================================

#ifndef __BL_CORE_H__
#define __BL_CORE_H__

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

  int bl_core(BL_ob *o, int val); // public module interface

#endif // __BL_CORE_H__
