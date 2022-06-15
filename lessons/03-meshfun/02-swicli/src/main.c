//==============================================================================
// 02-swicli - Bluetooth mesh client/server app
//==============================================================================
//
//                   +--------------------+
//                   |        APP         |
//                   +--------------------+
//                   |      SWITCH:       |  SWITCH interface
//  (U)->      STS ->|      @id,sts       |  receive switch @id status
//                   +--------------------+
//                   |      GOOCLI:       |  GOOCLI ifc. (generic on/off client)
//  (D)<-      SET <-| @id,<BL_goo>,onoff |  publish generic on/off SET message
//                   +--------------------+
//
//==============================================================================

  #include "bluccino.h"                // access bluccino stuff
  #include "bl_gonoff.h"               // generic on/off model

  int app(BL_ob *o, int val)           // public APP module interface
  {
    if (bl_is(o,_SWITCH,STS_))         // switch status update
      bl_gooset(o->id,NULL,val);       // send generic on/off SET message
    return 0;                          // OK
  }

  void main(void)
  {
    bl_hello(4,PROJECT);               // set verbose level, print hello message
    bl_engine(app,10,1000);            // run app with 10/1000ms ticks/tocks
  }
