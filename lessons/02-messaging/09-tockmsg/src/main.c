//==============================================================================
// main.c for 09-tockmsg (post tock messages)
//==============================================================================

  #include "bluccino.h"                // 09-tockmsg

//==============================================================================
// APP module
//==============================================================================
//
//                  +--------------------+
//                  |         APP        |
//                  +--------------------+
//                  |         SYS:       | SYS: interface
// (MAIN)->  TOCK ->|       @id,cnt      | tock the module
//                  +--------------------+

  int app(BL_ob *o, int val)           // app() catching Bluccino event messages
  {
    if ( bl_is(o,_SYS,TOCK_) )         // [SYS:TOCK @id,val] message?
      bl_logo(1,"app:",o,val);         // log event message
    return 0;                          // OK
  }

//==============================================================================
// main (post tick messages to app)
//==============================================================================

  void main(void)
  {
    bl_hello(4,"09-tickmsg (post tick messages)");

    BL_ob oo = {_SYS,TOCK_,0,NULL};    // declare [SYS:TOCK] message object
    for (int cnt=0;; cnt++)            // forever
    {
      app(&oo,cnt);                    // post [SYS,TOCK @0,val] to APP
      bl_sleep(1000);                  // sleep 1000ms
    }
  }
