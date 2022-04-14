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
  #include "attention.h"
  #include "provision.h"

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
    static bool prv = false;

    switch (bl_id(o))                  // dispatch message ID
    {
      case BL_ID(_SYS,INIT_):
        LOGO(1,BL_B,o,val);
        bl_init(attention,app);        // init (ATTENTION)
        bl_init(provision,app);        // init (PROVISION)
        return 0;

      case BL_ID(_SYS,TICK_):
        attention(o,val);              // forward tick to (ATTENTION)
	      provision(o,val);              // forward tick to (PROVISION)
 //     if (val % 500 == 0)            // every 500 tick (every 5s)
 //       LOGO(1,BL_G,o,val);
        return 0;

      case BL_ID(_SYS,TOCK_):
     	  if (val % 10 == 0)
          LOG(1,"%sprovisioned node",prv ? BL_C"I'm a " : "I'm a un");
        return 0;

      case BL_ID(_MESH,ATT_):
        return attention(o,val);       // forward to ATTENTION module

      case BL_ID(_MESH,PRV_):
        prv = val;                     // refresh provisioned state
	return provision(o,val);       // forward to PROVISION module

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
    bl_hello(3,"03-provision");        // set verbose level, print hello message
    bl_engine(app,10,1000);            // run app with 10ms/1000ms tick/tock
  }
