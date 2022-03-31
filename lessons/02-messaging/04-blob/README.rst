================================================================================
04-blob - basic Bluccino messaging with Bluccino objects (stage 3)
================================================================================

App Description
- main() sends both a [LIGHT:CTRL @2,1,<room>] message, as well as a
  [LIGHT:CTRL @3,0,"kitchen"] message to public module interface of APP module
- sending is invoked by a 'syntactic sugar' function light_ctrl(), which is a
  means to express code in a more readable form
- upon dispatching of [LIGHT:CTRL @id,val,"kitchen"] message the app() prints
  log output "[LIGHT:CTRL] message received :-)"
- in addition app prints "kitchen light @2 on" after receive of first event
  message, and "living room light @3 off" after receive of second event message
- a graphical interface documents the APP's public module interface
- it also shows, from which module (MAIN) the messages are being posted

Lessons to Learn
- learn how to transmit complex data with Bluccino messages, and how to use the
  @id argument
- learn how to use syntactic sugar
- the standard way of dispatching

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
