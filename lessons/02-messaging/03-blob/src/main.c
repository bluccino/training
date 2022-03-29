//==============================================================================
// main.c for 03-blob (basic Bluccino messaging with Bluccino objects - stage 2)
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
//                 |      @2,1,"kitchen"       |
//                 |                           |
//                 |       [LIGHT:CTRL]        |
//                 o-------------------------->|   turn light @3 off
//                 |     @3,0,"living room"    |
//                 |                           |
//
//==============================================================================

  #include "bluccino.h"                // access Bluccino stuff

    // let's consider an event message [LIGHT:CTRL] for "light-control"

  #define LIGHT   1234                 // some fantasy number
  #define CTRL    5678                 // another fantasy number

//==============================================================================
// utility functions
//==============================================================================

  #define ID(cl,ob)    (((cl) << 16) | (op))

  static int id(BL_obj *o)
  {
    return ID(o->cl,o->op);
  }

//==============================================================================
// public APP module interface
//==============================================================================
//
// (A) := (APP)
//                  +--------------------+
//                  |        APP         | APP module
//                  +--------------------+
//                  |       LIGHT:       | LIGHT interface
// (A)->     CTRL ->|  @id,onoff,<room>  | light on/off control
//                  +--------------------+
//
//==============================================================================

  int app(BL_ob *o, int val)           // APP's OVAL interface
  {
    BL_txt room = bl_data(o);          // access data

    switch(id(o))
    {
      case ID(LIGHT,CTRL):             // dispatch message [LIGHT:CTRL]
        bl_log(1,"[LIGHT:CTRL] message received :-)");
        bl_log(1,"%s light @%d %s",room, o->id, val?"on":"off");
        return 0;                      // OK

      default:
        return 0;                      // OK
    }
  }

//==============================================================================
// syntactic sugar: send [LIGHT:CTRL @id,onoff,<room>] message to any module
// - usage: light_ctrl(app,id,true,"kitchen")
//==============================================================================

  static inline int light_ctrl(BL_oval module, int id, bool onoff, BL_txt room)
  {
    BL_ob oo = {LIGHT,CTRL,id,room};
    return module(&oo,onoff);         // (MODULE)<-[LIGHT:CTRL @id,onoff,<room>]
  }

//==============================================================================
// main function
// - sets verbose level & prints hello message & inits Bluccino module
// - post two messages, [LIGHT:CTRL @2,1] and [LIGHT:CTRL @3,0] to app
//==============================================================================

  void main(void)
  {
    bl_hello(4,"03-blob - messaging with Blobs (Bluccino messaging objects)");

    light_ctrl(app,2,1,"kitchen");     // (APP)<-[LIGHT:CTRL @2,1,"kitchen"]
    light_ctrl(app,3,0,"living room"); // (APP)<-[LIGHT:CTRL @3,0,"living room"]
  }
