//==============================================================================
// main.c - Bluetooth mesh client/server app
//==============================================================================
//
//                   +--------------------+
//                   |        app         |
//                   +--------------------+
//                   |      SWITCH:       |  SWITCH interface
//  (U)->      STS ->|      @id,val       |  receive switch @id status
//                   +--------------------+
//                   |      GOOCLI:       |  GOOCLI ifc. (generic on/off client)
//  (D)<-      SET <-|  @id,<goosrv>,val  |  publish generic on/off SET message
//                   +--------------------+
//                   |      GOOSRV:       |  GOOSRV ifc. (generic on/off server)
//  (U)->      STS ->|  @id,<goosrv>,val  |  recieve generic on/off SET message
//                   +--------------------+
//                   |        LED:        |  LED interface
//  (D)<-      SET <-|      @id,val       |  set LED @id on/off
//                   +--------------------+
//
//==============================================================================

  #include "bluccino.h"                     // access bluccino stuff
  #include "bl_gonoff.h"                    // generic on/off model

  int app(BL_ob *o, int val)                // public APP module interface
  {
    if (bl_is(o,_SWITCH,STS_))              // switch status update
      bl_gooset(o->id,NULL,val);            // send generic on/off SET message
    else if (bl_is(o,_GOOSRV,STS_))         // generic on/off srv status update
      bl_led(o->id,val);                    // turn LED @id on/off
    return 0;                               // OK
  }

  void main(void)
  {
    bl_hello(4,PROJECT);                    // set verbose level, hello message
    bl_init(bluccino,app);                  // init bluccino, output to app()
  }
