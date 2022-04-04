//==============================================================================
//  bl_run.h
//  Bluccino init/tic/toc engine and integration of test modules
//
//  Created by Hugo Pristauz on 2022-04-03
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================
//
// Example 1: use bl_run() for separate app() module and when() callback
//
//   #include "bluccino.h"
//
//   int when(BL_ob *o, int val);      // separate callback to handle events
//   int app(BL_ob *o, int val);       // public APP module interface
//
//   void main(void)                   // typical main() function
//   {
//     bl_hello(4,"Bluccino engine for app/when case");
//     bl_run(app,10,1000,when);       // run app with 10/1000 ms tick/tock
//   }
//
//==============================================================================
//
// Example 2: use bl_run() for separate app() module and when() callback
// with additional integration of a main test module test()
//
//   #include "bluccino.h"
//
//   int test(BL_ob *o, int val);      // main test module
//   int when(BL_ob *o, int val);      // separate callback to handle events
//   int app(BL_ob *o, int val);       // public APP module interface
//
//   void main(void)                   // typical main() function
//   {
//     bl_hello(4,"Bluccino engine for app/when case");
//     bl_test(test);                  // config test module inclusion
//     bl_run(app,10,1000,when);       // run app with 10/1000 ms tick/tock
//   }
//
//==============================================================================
//
// Example 3: (standard scenario) use bl_run() if app() handles also callbacks
// with additional integration of a main test module test()
//
//   #include "bluccino.h"
//
//   #if CFG_SUPPORT_TEST
//     #define TEST  ft_test           // test module: ft_test()
//   #else
//     #define TEST  NULL              // test module not supported
//   #endif
//
//   int test(BL_ob *o, int val);      // main test module
//   int app(BL_ob *o, int val);       // public APP module interface
//
//     // note: the call bl_engine(app,ms_tick,ms_tock) is nothing but syntactic
//     // sugar for the call bl_run(app,ms_tick,ms_tock,app)
//
//   void main(void)                   // typical main() function
//   {
//     bl_hello(4,"Bluccino engine for app/when case");
//     bl_test(TEST);                  // config test module inclusion
//     bl_engine(app,10,1000);         // run app with 10/1000 ms tick/tock
//   }
//
//==============================================================================
//
// Typical Hierarchy and output message flow with test module integration,
// demonstrating that
//
// +- MAIN => bl_run(app,tick_ms,tock_ms,when)
//     |
//     +- BLUCCINO =======================>|    (Bluccino module)
//     |   |                               |
//     |   +- BL_IN ======================>|    (input interface)
//     |   |   (^)<====================|   |
//     |   |                           |   |
//     |   +- BL_UP ==================>|   |    (upstream gear)
//     |   |   (^)<================|   |   |
//     |   |                       |   |   |
//     |   +- BL_DOWN ========>|   |   |   |    (downstream gear)
//     |       |               |   |   |   |
//     |       |   (v)<========|   |   |   |
//     |       +- BL_CORE ========>|   |   |    (core system)
//     |                               |   |
//     +- APP ============================>|    (application)
//     |  (^)<=============================|
//     |                               |   |
//     |   (v)<========================|   |
//     +- TEST ===========================>|    (main test module)
//
// Message Flow Chart: Initializing using bl_engine()
// Note that test() may occasionally reconfigure message flow!
//
//  MAIN  BLUCCINO     BL_DOWN BL_CORE  BL_UP  BL_IN        APP     TEST
//   |       |           |       |       |       |           |       |
//   |[SYS:INIT <app>]   |       |       |       |           |       |
//   o------>#==============================================>|       |
//   |       |           |       |       |       |           |       |
//   |       |       [SYS:INIT <app>]    |       |           |       |
//   |       o---------------------------------->#==========>|       |
//   |       |           |       |       |       |           |       |
//   |       |       [SYS:INIT <bl_in>]  |       |           |       |
//   |       o-------------------------->#======>|           |       |
//   |       |           |       |       |       |           |       |
//   |       |[SYS:INIT <bl_up>] |       |       |           |       |
//   |       o---------->|       |       |       |           |       |
//   |       |           |       |       |       |           |       |
//   |       |           |[SYS:INIT <*>] |       |           |       |
//   |       |           o------>#======>|       |           |       |
//   |       |           |       |       |       |           |       |
//   |       | [SYS:INIT <app>]  |       |       |           |       |
//   o------------------------------------------------------>|       |
//   |       |           |       |       |       |       +===#       |
//   |       |           |       |       |       |       |   |       |
//   |       |           |       |       |       |       +==>|       |
//   |       | [SYS:INIT <app>]  |       |       |           |       |
//   o-------------------------------------------------------------->|
//   |       |           |       |       |       |           |<======#
//   |       |           |       |       |       |           |       |
//
//==============================================================================

#ifndef __BL_RUN_H__
#define __BL_RUN_H__

//==============================================================================
// setup a initializing, ticking and tocking for a test module
// - usage: bl_test(module)            // controlled by bl_run()
//==============================================================================

  int bl_test(BL_oval module);

//==============================================================================
// run app with given tick/tock periods and provided when-callback
// - usage: bl_run(app,10,100,when)   // run app with 10/1000 tick/tock periods
//==============================================================================

  void bl_run(BL_oval app, int tick_ms, int tock_ms, BL_oval when);

//==============================================================================
// syntactic sugar: run app where app()and when() function are the same
// - usage: bl_engine(app,10,100)   // run app with 10/1000 tick/tock periods
//==============================================================================

  static inline void bl_engine(BL_oval app, int tick_ms, int tock_ms)
  {
    bl_run(app, tick_ms,tock_ms, app); // callbacks are the same
  }

#endif // __BL_RUN_H__
