================================================================================
01-blob - basic Bluccino messaging with Bluccino objects (stage 1)
================================================================================

App Description
- main() sends a [LIGHT:CTRL] message to public module interface app() of
  APP module
- upon dispatching of [LIGHT:CTRL] message the app() prints the log output
  "[LIGHT:CTRL] message received :-)"

Lessons to Learn
- get started with Bluccino messaging basics
- get familar with the structure of Buccino messaging objects (BL_ob's)
- learn that a Bluccino message ID cl:op comprises two parts
    1) class tag (for message interface class)
    2) operation code (opcode)
- learn that a Bluccino message object reserve a field for an instance ID
- learn that a Bluccino message can transport two kinds of data: simple data
  (which is always an integer) and complex data (which can be any pointer to
  data or function, e.g. pointer to character string, pointer to a structure or
  callback function address)
- learn how to setup a messaging object for the message example [LIGHT:CTRL]
- learn how to post the message using the app's OVAL interface
- learn how to dispatch a Bluccino message on the basis level
- note that a Bluccino message can be posted to any function supporting an OVAL
  interface
- mind the notation (APP)<-[LIGHT:CTRL] for posting a [LIGHT:CTRL] to module APP
- get experience with the Bluccino rapid prototyping process

Bluccino Primitives Used
- bl_hello(): setup verbose (logging) level and print a hello message
- bl_log(): print a log message to console

================================================================================
Exercices
================================================================================

Exercise 1
- Bluccino provides a function bl_is() for dispatching of messages
- replace the expession of the if clause by a proper call to bl_is() and check
  whether the program shows the same behavior
