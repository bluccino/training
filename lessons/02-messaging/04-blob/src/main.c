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

  #define BL_CL_TEXT {"VOID","SYS","LIGHT"}
  #define BL_OP_TEXT {"VOID","INIT","CTRL"}

  typedef enum BL_cl                   // class tag
          {
            _VOID = 0x7FFF,            // invalid message class (a big number!)
            _SYS = 1,                  // system
            _LIGHT,
          } BL_cl;

  typedef enum BL_op
          {
            VOID_ = 0x7FFF,            // invalid opcode (a big number)
            INIT_ = 1,                 // init function
            CTRL_,
          };

  #include "bluccino.h"                // access Bluccino stuff

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

    switch(bl_id(o))
    {
      case BL_ID(_LIGHT,CTRL_):        // dispatch message [LIGHT:CTRL]
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
    BL_ob oo = {_LIGHT,CTRL_,id,room};
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
