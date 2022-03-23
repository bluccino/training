## 10-msgout - study Bluccino message output and block connection

* this sample program demonstrates the core mechanism of Bluccino messaging
* the sample comprises a SENDER and a RECEIVER module, where the SENDER module
  posts [MESH:ONOFF] messages to the RECEIVER module
* this time we are using bl_run() to setup a tick/tock generator which also
* initializes the SENDER module
* the SENDER module receives periodic [SYS:TOCK] events, dispatches them
  and emits a [MESH:ONOFF] message to its subscriber
* the receiver module dispatches and logs the received [MESH:ONOFF] messages


## Exercises

### Exercise 1 - Dispatching
* we use the Bluccino function bl_init() to setup the connection between SEND
  and RECEIVE module. Replace the call to bl_init() by a a call to a self
  written function my_init() with the same functionality.
