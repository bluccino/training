================================================================================
02-blob - basic Bluccino messaging with Bluccino objects (stage 2)
================================================================================

App Description
- main() sends both a [LIGHT:CTRL @2,1] message and a [LIGHT:CTRL @3,0] message
  to public module interface app() of APP module
- upon dispatching of [LIGHT:CTRL @id,val] message the app() prints log output
  "[LIGHT:CTRL] message received :-)"
- in addition app prints "light @2 on" after receive of first event message, and
  "light @3 off" after receive of second event message
- a graphical interface documents the APP's public module interface
- it also shows, from which module (MAIN) the messages are being posted

Lessons to Learn
- learn how to transmit simple data with Bluccino messages, and how to use the
  @id argument
- learn how Bluccino module interfaces are graphically documented
- get familar with event message flow charts, and how to label message flow

Bluccino Primitives Used
- BL_ob: Bluccino object (used for Bluccino messaging)
- bl_hello(): setup verbose (logging) level and print a hello message
- bl_log(): print a log message to console

================================================================================
Exercices
================================================================================

Exercise 1
- the data element of the BL_ob structure, which is provided for the transmis-
  sion of more complex data or function adresses, has not be used so far
- transmit a pointer to string "kitchen" in the first message, and transmit
  a pointer to string "living room" in the second message
- modify the log messages of the app to show also the transmitted string argu-
  ment in the log
