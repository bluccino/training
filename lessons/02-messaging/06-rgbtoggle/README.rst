================================================================================
05-toggle - basic Bluccino messaging with BUTTON, APP and LED module
================================================================================

App Description
- pressing button 1 posts [BUTTON:PRESS] messages to APP module
- APP toggles an on/off state upon arrival of [BUTTON:PRESS] messages and posts
  an [LED:SET onoff] message to LED module to turn LED on/off accordingly.

Lessons to Learn
- learn about module hierarchy
- learn about message flow charts
- learn about module interface diagrams
- learn about translation of module interfaces to a dispatcher dispatching
- learn about worker functions
- learn how to use syntactic sugar

Bluccino Primitives Used
- BL_ob: Bluccino object (used for Bluccino messaging)
- bl_hello(): setup verbose (logging) level and print a hello message
- bl_log(): print a log message to console

================================================================================
Exercices
================================================================================

Exercise 1
- make yourself a coffee (or tea) and relax
