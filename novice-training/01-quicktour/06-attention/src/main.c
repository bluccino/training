//==============================================================================
// main.c for 06-attention demo
//==============================================================================

#include "bluccino.h"

  static BL_txt msg1 = BL_C "I'm a (provisoned) mesh node";
  static BL_txt msg2 = BL_C "I'm a (unprovisoned) mesh device";

  static bool attention = 0;           // local copy of attention state
  static bool provision = 0;           // local copy of provision state

//==============================================================================
// app init and app loop
//==============================================================================

  static int app(BL_ob *o, int val)    // OVAL interface of APP module
  {
    switch (bl_id(o))                  // dispatch message ID
    {
      case BL_ID(_SYS,INIT_):
        bl_log(1,"here we go ...");    // init log message
        return 0;                      // OK

      case BL_ID(_SYS,TOCK_):          // [SYS:TOCK @id,cnt]
        bl_log(1,"%s",provision?msg1:msg2);

        if (attention)
          bl_log(1,BL_M"attention state");
        return 0;                      // OK

      case BL_ID(_SET,ATT_):
        attention = val;               // store a copy updated attention state
        return 0;

      case BL_ID(_SET,PRV_):
        provision = val;               // store a copy updated provision state
        return 0;                      // OK

      default:
        return -1;                     // bad args
    }
  }

//==============================================================================
// Arduino style engine
//==============================================================================

  void main(void)
  {
    bl_hello(5,"06-attention");
    bl_run(app,10,1000,app);           // run 10/1000ms tick/tock engine
  }
