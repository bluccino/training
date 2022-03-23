//==============================================================================
// main.c for 10-msgput (Bluccino message output)
//==============================================================================

  #include "bluccino.h"                // 10-msgout

//==============================================================================
// RECEIVER module (public interface)
//==============================================================================
//
// (S) := (SENDER)
//                  +--------------------+
//                  |      RECEIVER      |
//                  +--------------------+
//                  |       MESH:        |
// (S)->    ONOFF ->|        val         |
//                  +--------------------+
//
//==============================================================================

  int receiver(BL_ob *o, int val)
  {
    if ( bl_is(o,_MESH,ONOFF_) )
      bl_logo(1,"receiver:",o,val);    // log received message
    return 0;
  }

//==============================================================================
// SENDER module (public interface)
//==============================================================================
//
// (M) := (MAIN); (R) := (RECEIVER)
//                  +--------------------+
//                  |       SENDER       |
//                  +--------------------+
//                  |        SYS:        | SYS: interface
// (M)->     INIT ->|       <out>        | init module, store <out> callback
// (M)->     TOCK ->|        cnt         | tock the module
//                  +--------------------+
//                  |       MESH:        | MESH: interface
// (R)<-    ONOFF <-|        val         | output [MESH:ONOFF val] message
//                  +--------------------+
//==============================================================================

  int sender(BL_ob *o, int val)
  {
    static BL_fct out = NULL;
    static BL_ob oo = {_MESH,ONOFF_,0,NULL};     // message object

    if ( bl_is(o,_SYS,INIT_) )                   // init module, store <out> cb
      out = o->data;
    else if ( bl_is(o,_SYS,TOCK_) )              // [SYS:TOCK val] msg received?
      bl_out(&oo,(val%2),out);                   // output (val%2) to RECEIVER
  }

//==============================================================================
// main function - init modules and setup connection
//==============================================================================

  void main(void)
  {
    bl_hello(4,"10-msgout (Bluccino message output)");
    bl_init(sender,receiver);          // init SENDER, output goes to RECEIVER

    BL_ob oo = {_SYS,TOCK_,0,NULL};
    for (int cnt=0; cnt<=50; cnt++)
      sender(&oo,cnt);                 // post [SYS,TOCK i] to SENDER
  }
