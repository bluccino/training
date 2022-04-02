//==============================================================================
// main.c - Bluetooth mesh client/server app
//==============================================================================
//
//                   +--------------------+
//                   |        APP         |
//                   +--------------------+
//                   |      SWITCH:       |  SWITCH interface
//  (^)->      STS ->|      @id,val       |  receive switch @id status
//                   +--------------------+
//                   |      GOOCLI:       |  GOOCLI ifc. (generic on/off client)
//  (v)<-      SET <-|  @id,<goosrv>,val  |  publish generic on/off SET message
//                   +--------------------+
//
//==============================================================================

  #include "bluccino.h"

  int app(BL_ob *o, int val)           // public APP module interface
  {
    BL_ob cli = {_GOOCLI,SET_,o->id,NULL};  // generic on/off client object

    if (bl_is(o,_SYS,TOCK_) && val%5==0)
		  bl_log(1,"tock");

    if (bl_is(o,_SWITCH,STS_))         // switch status update
			bl_down(&cli,val);               // send generic on/off SET message
    return 0;                          // OK
  }

  void main(void)
  {
    bl_hello(4,"02-swicli");           // set verbose level, print hello message
    bl_engine(app,10,1000);            // run app with 10/1000ms ticks/tocks
  }
