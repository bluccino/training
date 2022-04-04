//==============================================================================
// main.c - Bluetooth mesh client/server app
//==============================================================================
//
//                   +--------------------+
//                   |        APP         |
//                   +--------------------+
//                   |      GOOSRV:       |  GOOSRV ifc. (generic on/off server)
//  (^)->      STS ->|  @id,<goosrv>,val  |  recieve generic on/off SET message
//                   +--------------------+
//                   |        LED:        |  LED interface
//  (v)<-      SET <-|      @id,val       |  set LED @id on/off
//                   +--------------------+
//
//==============================================================================

  #include "bluccino.h"

  int app(BL_ob *o, int val)                // public APP module interface
  {
    BL_ob led = {_LED,SET_,o->id,NULL};     // LED object

    if (bl_is(o,_GOOSRV,STS_))              // generic on/off srv status update
      bl_down(&led,val);                    // turn LED @id on/off
    return 0;                               // OK
  }

  void main(void)
  {
    bl_hello(4,"03-ledsrv");                // set verbose level, print hello message
    bl_init(bluccino,app);                  // init bluccino, output to app()
  }
