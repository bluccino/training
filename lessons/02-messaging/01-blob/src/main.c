//==============================================================================
// main.c for 01-blob (basic Bluccino messaging with Bluccino objects - stage 1)
//==============================================================================
//
// get familar with Buccino messaging object (BL_ob) structure:
//
// typedef struct BL_ob
//        {
//          BL_cl cl;                  // class tag
//          BL_op op;                  // opcode
//          int id;                    // object ID
//          const void *data;          // pointer to data
//        } BL_ob;
//
//==============================================================================

  #include "bluccino.h"                // access Bluccino stuff

    // let's consider an event message [LIGHT:CTRL] for "light-control"

  #define LIGHT   1234                 // some fantasy number
  #define CTRL    5678                 // another fantasy number

//==============================================================================
// public APP module interface
//==============================================================================

  int app(BL_ob *o, int val)           // APP's OVAL interface
  {
    if (o->cl==LIGHT && o->op==CTRL)   // dispatch message [LIGHT:CTRL]
      bl_log(1,"[LIGHT:CTRL] message received :-)");
    return 0;                          // OK
  }

//==============================================================================
// main function
// - sets verbose level & prints hello message & inits Bluccino module
// - post a message [LIGHT:CTRL] to app
//==============================================================================

  void main(void)
  {
    bl_hello(4,"01-blob - messaging with Blobs (Bluccino messaging objects)");

    BL_ob oo = {LIGHT,CTRL,0,NULL};
    app(&oo,0);                        // (APP)<-[LIGHT:CTRL] - post to app()
  }
