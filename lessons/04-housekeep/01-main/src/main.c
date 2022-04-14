//==============================================================================
// main.c - Bluetooth mesh client/server app
//==============================================================================
//
// (B) := (BLUCCINO)
//                   +--------------------+
//                   |        APP         |
//                   +--------------------+
//                   |        SYS:        |  SYS interface
//  (B)->     INIT ->|       <out>        |  init module, store <out> callback
//  (B)->     TICK ->|      @id,cnt       |  tick module
//  (B)->     TOCK ->|      @id,cnt       |  tock module
//                   +--------------------+
//
//==============================================================================

  #include "bluccino.h"

//==============================================================================
// APP level logging shorthands
//==============================================================================

  #define LOG                     LOG_APP
  #define LOGO(lvl,col,o,val)     LOGO_APP(lvl,col"xyz:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_APP(lvl,col,o,val)

//==============================================================================
// public APP interface
//==============================================================================

  int app(BL_ob *o, int val)                // public APP module interface
  {
    switch (bl_id(o))                       // dispatch message ID
    {
      case BL_ID(_SYS,INIT_):
        LOGO(1,BL_B,o,val);
        return 0;

      case BL_ID(_SYS,TICK_):
        if (val % 500 == 0)                 // every 500 tick (every 5s)
  	      LOGO(1,BL_G,o,val);
        return 0;

      case BL_ID(_SYS,TOCK_):
	LOGO(1,BL_M,o,val);
	return 0;

      default:
	LOGO(1,BL_R"app: not handeled:",o,val);
	return 0;
    }
  }

  void main(void)
  {
    bl_hello(4,"01-main");                // set verbose level, print hello message
    bl_engine(app,10,1000);               // run app with 10ms/1000ms tick/tock
  }
