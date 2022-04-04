//==============================================================================
// main.c for 10-msgput (Bluccino message output)
//==============================================================================
//
// Event Flow Chart
//
//        MAIN                   SENDER                 RECEIVER
//         |                       |                       |
//         | [SYS:INIT <receiver>] |                       |
//         o---------------------->#======================>|
//         |                       |                       |
//         |                       |                       |
//         |    [SYS:TOCK cnt]     |                       |
//         o---------------------->|   [MESH:ONOFF val]    |
//         |                       o---------------------->|
//         |                       |                       |
//
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
    static BL_oval out = NULL;

    if ( bl_is(o,_SYS,INIT_) )                   // init module, store <out> cb
      out = o->data;
    else if ( bl_is(o,_SYS,TOCK_) )              // [SYS:TOCK val] msg received?
    {
      bl_log(1,BL_M "sender:   [MESH:ONOFF @0,%d]",val%2);
      bl_msg(out,_MESH,ONOFF_, 0,NULL,(val%2));  // output (val%2) to RECEIVER
    }
    return 0;
  }

//==============================================================================
// main function - init modules and setup connection
//==============================================================================

  void main(void)
  {
    bl_hello(4,"10-msgout (Bluccino message output)");
    bl_init(sender,receiver);          // init SENDER, <out> goes to RECEIVER

    for (int cnt=0;; cnt++)
    {
      bl_msg(sender,_SYS,TOCK_, 0,NULL,cnt); // [SYS,TOCK cnt] -> (SENDER)
      bl_sleep(1000);
    }
  }
