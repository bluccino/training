//==============================================================================
// main.c - Bluetooth mesh client/server app
//==============================================================================
//
// (D) := (bl_down);  (U) := (bl_up);
//
//                   +--------------------+
//                   |        APP         |
//                   +--------------------+
//                   |      GOOSRV:       |  GOOSRV ifc. (generic on/off server)
//  (U)->      STS ->|  @id,<goosrv>,val  |  recieve generic on/off SET message
//                   +--------------------+
//                   |        LED:        |  LED interface
//  (D)<-      SET <-|      @id,val       |  set LED @id on/off
//                   +--------------------+
//
//==============================================================================

  #include "bluccino.h"                // access bluccino stuff
  #include "bl_gonoff.h"               // generic on/off model

  int app(BL_ob *o, int val)           // public APP module interface
  {
    if (bl_is(o,_GOOSRV,STS_))         // generic on/off srv status update
      bl_led(o->id,val);               // turn LED @id on/off
    return 0;                          // OK
  }

  void main(void)
  {
    bl_hello(4,PROJECT);               // set verbose level, print hello message
    bl_init(bluccino,app);             // init bluccino, output to app()
  }
