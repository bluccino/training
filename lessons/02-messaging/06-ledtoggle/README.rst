================================================================================
05-toggle - basic Bluccino messaging with BUTTON, APP and LED module
================================================================================

App Description
- pressing button 1 posts [BUTTON:PRESS] messages to APP module
- APP toggles an on/off state upon arrival of [BUTTON:PRESS] messages and posts
  an [LED:SET onoff] message to LED module to turn LED on/off accordingly.

================================================================================
Block Diagram
================================================================================

                +--------------+  [SYS:INIT <NULL>]
                |     MAIN     |-------+
                +--------------+       |
                                       |
                       +---------------v---------------+
                       |              APP              |
                       +------^-----------------v------+
                              |                 |
                        [BUTTON:PRESS]   [LED:SET onoff]
                              |                 |
                       +------^------+   +------v------+
                       |    BUTTON   |   |     LED     |
                       +------^------+   +------v------+
                              |                 |
                       +------^-----------------v------+
                       |         RTOS (Zephyr)         |
                       +-------------------------------+

================================================================================
Module Hierarchy
================================================================================

   +- MAIN                             // main function ("CEO")
      +- APP                           // APP module ("COO")
         +- BUTTON                     // BUTTON module (driver)
         +- LED                        // LED module (driver)

================================================================================
Message Flow Charts
================================================================================

1) Initializing Message Flow Chart

   MAIN                  APP                  BUTTON                 LED
    |  [SYS:INIT <NULL>]  |                     |                     |
    o-------------------->|  [SYS:INIT <NULL>]  |                     |
    |                     o-------------------->|                     |                     |
    |                     |                     |                     |
    |                     |                     |                     |

2) Button Event Flow Chart

   MAIN                  APP                  BUTTON                 LED
    |                     |                     |                     |
    |                     |   [BUTTON:PRESS]    |                     |
    |                     |<--------------------o                     |                     |
    |                     |                     |                     |
    |            +--------v--------+            |                     |
    |            | onoff = !onoff  |            |                     |
    |            +--------v--------+            |                     |
    |                     |              [LED:SET onoff]              |
    |                     o------------------------------------------>|
    |                     |                     |                     |

Lessons to Learn
- block diagram
- module hierarchy
- message flow charts
- message IDs
- message arguments
- module interface diagrams
- transmission functions
- OVAL interfaces and OVAL transmission
- learn about translation of module interfaces to a dispatcher dispatching
- learn about worker and helper functions
- learn about syntactic sugar

Bluccino Primitives Used
- BL_ob: Bluccino object (used for Bluccino messaging)
- bl_hello(): setup verbose (logging) level and print a hello message
- bl_log(): print a log message to console

================================================================================
Exercices
================================================================================

Exercise 1
- make yourself a coffee (or tea) and relax
