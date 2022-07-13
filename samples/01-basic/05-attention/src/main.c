//==============================================================================
// main.c for 04-attention demo
//==============================================================================

#include "bluccino.h"

  BL_txt msg1, msg2;

//==============================================================================
// when a message is emitted
//==============================================================================

  static int when(BL_ob *o, int val)
  {
    switch (o->op)
    {
      case OP_ATT:
      case OP_PRV:
        bl_logo(1,BL_R"when",o,val);
        break;
    }
    return 0;
  }

//==============================================================================
// app init and app loop
//==============================================================================

  static void init()                   // app init
  {
    msg1 = BL_C"I'm a (provisoned) mesh node"BL_0;
    msg2 = BL_C"I'm a (unprovisoned) mesh device"BL_0;
  }

  static void loop()                   // app loop
  {
    bl_log(1,bl_provisioned?msg1:msg2);

    if (bl_attention)
      bl_log(1,BL_M"attention state"BL_0);

    bl_sleep(1000);                    // sleep 1000 ms
  }

//==============================================================================
// Arduino style engine
//==============================================================================

  void main(void)
  {
    bl_init(NULL,when,4);              // Bluccino init (when-callback passing)
    init();                            // app init

    for(;;)
    {
      bl_loop(NULL);                   // run Bluccino loop
      loop();                          // run app loop
    }
  }
