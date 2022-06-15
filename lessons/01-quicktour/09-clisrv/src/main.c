//==============================================================================
// main.c for 09-clisrv (Bluetooth mesh client/server app)
//==============================================================================

  #include "bluccino.h"                     // access bluccino stuff
  #include "bl_gonoff.h"                    // generic on/off mesh models

//==============================================================================
//
// (U) := (bl_up);  (D) := (bl_down)
//
//                  +--------------------+
//                  |        app         |
//                  +--------------------+
//                  |      SWITCH:       |  SWITCH interface
// (U)->      STS ->|      @id,val       |  receive switch @id status
//                  +--------------------+
//                  |      GOOSRV:       |  GOOSRV input interface
// (U)->      STS ->|  @id,<BL_goo>,val  |  recieve generic on/off status update
//                  |....................|
//                  |     #GOOCLI:       |  GOOCLI output interface
// (D)<-      SET <-|  @id,<BL_goo>,val  |  publish generic on/off SET message
//                  +--------------------+
//                  |        LED:        |  LED interface
// (D)<-      SET <-|      @id,val       |  set LED @id on/off
//                  +--------------------+
//
//==============================================================================

  int app(BL_ob *o, int val)           // public APP module interface
  {
    switch (bl_id(o))
    {
      case SWITCH_STS_id_0_sts:        // [SWITCH:STS @id,sts]
        return bl_gooset(1,NULL,val);  // send generic on/off SET message

      case GOOSRV_STS_id_BL_goo_sts:   // [GONOFF:STS @id,<BL_goo>,sts]
        return bl_led(o->id,val);      // turn LED @id on/off

      default:
        return 0;                      // OK
    }
  }

//==============================================================================
// main function
// - set verbose level and print hello message
// - run init/tick/tock engine
//==============================================================================

  void main(void)
  {
    bl_hello(4,PROJECT);               // set verbose level, show project title
    bl_engine(app,10,1000);            // run 10/1000ms tick/tock engine
  }
