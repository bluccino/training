//==============================================================================
// main.c - Bluetooth mesh client/server app
//==============================================================================
//
// (B) := (BLUCCINO);  (U) := (BL_UP);  (A) := (ATTENTION)
//
//                   +--------------------+
//                   |        APP         |
//                   +--------------------+
//                   |        SYS:        |  SYS interface
// (B)->      INIT ->|       <out>        |  init module, store <out> callback
// (B)->      TICK ->|      @id,cnt       |  tick module
// (B)->      TOCK ->|      @id,cnt       |  tock module
//                   +--------------------+
//                   |       MESH:        |  MESH output interface
// (U)->       ATT ->|        sts         |  attention status update
//                   |....................|
//                   |       MESH:        |  MESH output interface
// (A)<-       ATT <-|        sts         |  attention status update
//                   +--------------------+
//
//==============================================================================

  #include "bluccino.h"
  #include "attention.h"

//==============================================================================
// APP level logging shorthands
//==============================================================================

  #define LOG                     LOG_APP
  #define LOGO(lvl,col,o,val)     LOGO_APP(lvl,col"app:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_APP(lvl,col,o,val)

//==============================================================================
// public APP interface
//==============================================================================

  int app(BL_ob *o, int val)           // public APP module interface
  {
    static BL_oval A = attention;      // attention output
     
    switch (bl_id(o))                  // dispatch message ID
    {
      case BL_ID(_SYS,INIT_):
        LOGO(1,BL_B,o,val);
        bl_init(attention,app);
        return 0;

      case BL_ID(_SYS,TICK_):
        attention(o,val);              // forward tick to (ATTENTION)
        if (val % 500 == 0)            // every 500 tick (every 5s)
          LOGO(1,BL_G,o,val);
        return 0;

      case BL_ID(_SYS,TOCK_):
     	LOGO(1,BL_M,o,val);
	return 0;

      case BL_ID(_MESH,ATT_):
	return bl_out(o,val,(A));      // output to ATTENTION module

      default:
	LOGO(1,BL_R"app: not handeled:",o,val);
	return 0;
    }
  }

//==============================================================================
// main entry point
//==============================================================================

  void main(void)
  {
    bl_hello(3,"02-attention");        // set verbose level, print hello message
    bl_engine(app,10,1000);            // run app with 10ms/1000ms tick/tock
  }
