//==============================================================================
// main.c for 02-blob (basic Bluccino messaging with Bluccino objects - stage 2)
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
//
// Event Message Flow Chart
//
//                MAIN                        APP
//                 |       [LIGHT:CTRL]        |
//                 o-------------------------->|   turn light @2 on
//                 |           @2,1            |
//                 |                           |
//                 |       [LIGHT:CTRL]        |
//                 o-------------------------->|   turn light @3 off
//                 |           @3,0            |
//                 |                           |
//
//==============================================================================

  #include "bluccino.h"                // access Bluccino stuff

    // let's consider an event message [LIGHT:CTRL] for "light-control"

  #define LIGHT   1234                 // some fantasy number
  #define CTRL    5678                 // another fantasy number

//==============================================================================
// public APP module interface
//==============================================================================
//
// (A) := (APP)
//                  +--------------------+
//                  |        APP         | APP module
//                  +--------------------+
//                  |       LIGHT:       | LIGHT interface
// (A)->     CTRL ->|     @id,onoff      | light on/off control
//                  +--------------------+
//
//==============================================================================

  int app(BL_ob *o, int val)           // APP's OVAL interface
  {
    if (o->cl==LIGHT && o->op==CTRL)   // dispatch message [LIGHT:CTRL]
      bl_log(1,"[LIGHT:CTRL] message received :-)");

    if (val)
      bl_log(1,"light @%d on",o->id);
    else
      bl_log(1,"light @%d off",o->id);

    return 0;                          // OK
  }

//==============================================================================
// main function
// - sets verbose level & prints hello message & inits Bluccino module
// - post two messages, [LIGHT:CTRL @2,1] and [LIGHT:CTRL @3,0] to app
//==============================================================================

  void main(void)
  {
    bl_hello(4,"02-blob - messaging with Blobs (Bluccino messaging objects)");

    BL_ob oo = {LIGHT,CTRL,0,NULL};

    oo.id = 2;
    app(&oo,1);                        // (APP)<-[LIGHT:CTRL @2,1]

    oo.id = 3;
    app(&oo,0);                        // (APP)<-[LIGHT:CTRL @2,1]
  }
