//==============================================================================
// main.c for 05-ledtoggle (simple message flow)
//==============================================================================

#ifndef __MAIN_H__
#define __MAIN_H__

//==============================================================================
// Block Diagram
//==============================================================================
//
//                +--------------+  [SYS:INIT <NULL>]
//                |     MAIN     |-------+
//                +--------------+       |
//                                       |
//                       +---------------v---------------+
//              +--------|              APP              |---------+
//              |        +------^-----------------v------+         |
//              |               |                 |                |
//      [SYS:INIT <APP>]  [BUTTON:PRESS]   [LED:SET onoff]   [SYS:INIT <NULL>]
//              |               |                 |                |
//              |        +------^------+   +------v------+         |
//              +------->|    BUTTON   |   |     LED     |<--------+
//                       +------^------+   +------v------+
//                              |                 |
//                       +------^-----------------v------+
//                       |         RTOS (Zephyr)         |
//                       +-------------------------------+
//
//==============================================================================
// Module Hierarchy (who initializes whom?)
//==============================================================================
//
//   +- MAIN                             // main function ("CEO")
//      +- APP                           // APP module ("COO")
//         +- BUTTON                     // BUTTON module (driver)
//         +- LED                        // LED module (driver)
//
//==============================================================================
// Message Flow Charts
//==============================================================================
//
// 1) Initializing Message Flow Chart
//
//   MAIN                  APP                  BUTTON                 LED
//    |  [SYS:INIT <NULL>]  |                     |                     |
//    o-------------------->|  [SYS:INIT <APP>]   |                     |
//    |                     o-------------------->|  [SYS:INIT <NULL>]  |                     |
//    |                     o------------------------------------------>|
//    |                     |                     |                     |
//
// 2) Button Event Flow Chart
//
//   MAIN                  APP                  BUTTON                 LED
//    |                     |                     |                     |
//    |                     |   [BUTTON:PRESS]    |                     |
//    |                     |<--------------------o                     |                     |
//    |                     |                     |                     |
//    |            +--------v--------+            |                     |
//    |            | onoff = !onoff  |            |                     |
//    |            +--------v--------+            |                     |
//    |                     |              [LED:SET onoff]              |
//    |                     o------------------------------------------>|
//    |                     |                     |                     |
//
#endif // __MAIN_H__
