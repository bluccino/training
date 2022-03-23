## 09-tockmsg - tiny sample program (main() posting tock messages to APP module)

* this sample program demonstrates the core mechanism of Bluccino messaging
* function main() provides a for-loop which posts periodically event messages
  [SYS:TOCK @0,cnt] to an APP module represented by function app()
* function app() dispatches and simply logs the message
* the study is based purely on debug logging, neither access to the HW layer
  nor access to the mesh stack is required
* the sample is prepared for nrf52dk_nrf52832 and nrf52dk_nrf52840 boards
  but can easily be built for other boards as long as logging is available

## Appendix - Logging Output

#0[000:00:000.000] 09-msgstudy (Bluccino messaging study)
#1[000:00:000.183]   app: [SYS:TICK @0,0]
#1[000:00:000.457]   app: [SYS:TICK @0,1]
#1[000:00:000.701]   app: [SYS:TICK @0,2]
#1[000:00:000.946]   app: [SYS:TICK @0,3]
#1[000:00:001.159]   app: [SYS:TICK @0,4]
#1[000:00:001.403]   app: [SYS:TICK @0,5]
#1[000:00:001.617]   app: [SYS:TICK @0,6]
#1[000:00:001.861]   app: [SYS:TICK @0,7]
#1[000:00:002.105]   app: [SYS:TICK @0,8]
#1[000:00:002.319]   app: [SYS:TICK @0,9]
          :           :         :

## Exercises

### Exercise 1 - Dispatching
* function app() uses Bluccino function bl_is() to dispatch the message ID,
  which is stored in o->cl (interface class) and o->op (operation code)
* rewrite the if-condition in function app() to avoid a call to function bl_is()
  by using an elementary logical expression

### Exercise 2 - Message ID Identification
* write a function with function prototype bool is(BL_ob *o, BL_cl cl, BL_op op)
  which returns the logical value true if and only if the message interface
  class o->cl and opcode o->op referenced by object pointer o matches a message
  ID given by [cl:op]
* rewrite and test the sample program by replacing the bl_is() call by
  calling is() instead

### Exercise 3 - Standard Tick/Tock Generator
* Sending ticking/tocking modules is so common that there is a Bluccino function
  bl_run() which initializes both an APP module and the driver layer and sends
  tick/tock events to both an APP module and the installed drivers.
* Lookup the Bluccino documentation and find out how to setup a tick/tock mes-
  sage generator with 10ms tick period, and 1000ms tock period.
* Replace the body of the main() function by two code lines, the first one
  logging a hello message, and the second one using bl_run() to start the
  tick/tock generator.
* The tick/tock generator started by bl_run() will send also [SYS:INIT] events
  to the APP module. Dispatch and log the [SYS:INIT] event.
