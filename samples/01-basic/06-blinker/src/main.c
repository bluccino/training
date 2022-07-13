//==============================================================================
// main.c for 05-blinker demo
//==============================================================================

  #include "bluccino.h"

  BL_ob o1 = {CL_GOOSRV,OP_SET,1,NULL};
  BL_ob o2 = {CL_GOOSRV,OP_SET,2,NULL};
  BL_ob o3 = {CL_GOOSRV,OP_SET,3,NULL};
  BL_ob o4 = {CL_GOOSRV,OP_SET,4,NULL};

//==============================================================================
// when a message is emitted
//==============================================================================

  static int when(BL_ob *o, int val)
  {
    switch (BL_PAIR(o->cl,o->op))
    {
      case BL_PAIR(CL_TIMER,OP_TICK):
      {
        bl_logo(1,BL_Y"when",o,val);

        bl_in(&o1,val%2);                  // change GOOSRV state
        bl_in(&o4,val%2);                  // change GOOSRV state
        val++;
        bl_in(&o2,val%2);                  // change GOOSRV state
        bl_in(&o3,val%2);                  // change GOOSRV state

        break;
      }
    }
    return 0;
  }

//==============================================================================
// app init and app loop
//==============================================================================

  static void init()                   // app init
  {
  }

  static void loop(void)               // app loop
  {
    static BL_ms tick = 0;             // next time stamp for tick emission
    static BL_ob oo = {CL_TIMER,OP_TICK,0,NULL};

    bl_wait(tick);                     // sleep 1000 ms
    bl_out(&oo,(int)(tick/1000),when); // emit message to (subscribed!) when-cb
    tick += 1000;                      // next time stamp to emit tick event
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
