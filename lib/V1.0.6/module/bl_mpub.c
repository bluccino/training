//==============================================================================
// bl_mpub.c
// mesh publisher (repeated sending of scheduled mesh messages)
//
// Created by Hugo Pristauz on 2022-JUN-11
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_mesh.h"
  #include "bl_gonoff.h"
  #include "bl_mpub.h"

  #define PMI  bl_mpub                 // public module interface

//==============================================================================
// logging shorthands
//==============================================================================

  #define WHO                          "bl_mpub:"

  #define LOG                          LOG_MPUB
  #define LOGO(lvl,col,o,val)          LOGO_MPUB(lvl,col WHO,o,val)
  #define LOG0(lvl,col,o,val)          LOGO_MPUB(lvl,col,o,val)

//==============================================================================
// provide config defaults
//==============================================================================

  #ifndef CFG_MPUB_QUEUE_LENGTH
    #define CFG_MPUB_QUEUE_LENGTH  20  // 20 scheduling entries by default
  #endif

  #ifndef CFG_MPUB_REPEAT
    #define CFG_MPUB_REPEAT         2  // 2 repeats by default (3x message)
  #endif
  #ifndef CFG_MPUB_INTERVAL
    #define CFG_MPUB_INTERVAL      20  // 20 ms repeat interval by default
  #endif

//==============================================================================
// locals
//==============================================================================

  static int repeat = CFG_MPUB_REPEAT;      // publish (repeats+1) messages
  static int interval = CFG_MPUB_INTERVAL;  // repeat interval

  static int scheduled = 0;                 // number of scheduled messages

//==============================================================================
// message queue
//==============================================================================

  typedef union MQ_data
          {
            BL_gooset gooset;
          } MQ_data;

  typedef struct MQ_entry
  {
    BL_ob o;                           // copy of messaging object
    int val;                           // copy of value
    MQ_data data;                      // message queue data
    volatile BL_ms due;                // due (dispatch) time
  } MQ_entry;

//==============================================================================
// message queue
// - entry i is free iff queue[i].due == 0
//==============================================================================

  #define MQ_LEN   CFG_MPUB_QUEUE_LENGTH

  static MQ_entry queue[MQ_LEN];

//==============================================================================
// helper: init message queue
//==============================================================================

  static void init_queue(void)
  {
    for (int i=0; i < BL_LEN(queue); i++)
      queue[i].due = 0;
  }

//==============================================================================
// helper: allocate a free queue entry (return NULL if no entries free)
// - usage: q = alloc(o,val,due)  // copy data of o,val and due to entry
//==============================================================================

  static MQ_entry *alloc(BL_ob *o, int val, BL_ms due)
  {
    for (int i=0; i < BL_LEN(queue); i++)
    {
      MQ_entry *q = queue + i;

      if (q->due == 0)
      {
        q->o.cl = o->cl;               // copy message interface class
        q->o.op = o->op;               // copy opcode
        q->o.id = o->id;               // copy @id
        q->o.data = &q->data;          // set <data> equal to NULL

        q->val = val;                  // copy value
        q->due = due;                  // set due time (for being published)

        scheduled++;                   // one more entry going to be scheduled
        return q;                      // return pointer to queue entry
      }
    }
    return NULL;                       // no more entries free
  }

//==============================================================================
// helper: release (free-up) queue entry
// - usage: release(q)
//==============================================================================

  static void release(MQ_entry *q)
  {
    scheduled--;
    q->due = 0;                        // release queue entry (mark as free)
  }

//==============================================================================
// worker: schedule generic on/off SET/LET/GET messages
//==============================================================================

  static int goocli_any(BL_ob *o, int val)
  {
    BL_ms now = bl_ms();
    BL_goo *g = bl_data(o);

    static uint8_t tid = 0;
    tid++;

       // we schedule now (repeats+1) messages ...

    for (int i = 0; i <= repeat; i++)
    {
      BL_ms due = now + i*interval;
      MQ_entry *q = alloc(o,val!=0,due);  // allocate free queue entry

        // if queue entry pointer is empty we cannot proceed and we have to
        // drop-off message - emit error message to indicate message drop-off!

      if (!q)
      {
        bl_err(-1,"goocli_set: message drop due to full queue");
        return -1;
      }

        // message object and due time setup already done
        // assert o->data points to the intended data field of the union

      bl_assert((void*)q->o.data == (void*)&q->data.gooset);

        // setup payload data ...

      BL_gooset *s = &(q->data.gooset);

      s->target  = (val != 0);
      s->tid = tid;
      s->tt = bl_ms2mesh(g ? g->tt:0);
      s->delay = bl_delay_ticks(repeat, i, g ? g->delay:0);

      LOG(5,BL_C"schedule [GOOCLI:%s @%d,<#%d,/%dms,&%dms>,%d] (#%d,/%d,&%d) @%d",
          (o->op==SET_) ? "SET" : (o->op==LET_ ? "LET":"???"), o->id,
          tid, bl_tt2ms(s->tt), bl_delay2ms(s->delay), q->val,
          s->tid,s->tt,s->delay, (int)due);
    }

    return 0;
  }

//==============================================================================
// worker: system tick
//==============================================================================

  static int sys_tick(BL_ob *o, int val)
  {
    if (scheduled > 0)                 // in case of scheduled messages
    {
      BL_ms now = bl_ms();

//    LOG(1,BL_Y"sys_tick: %d entries scheduled",scheduled);

      for (int i=0; i < BL_LEN(queue); i++)
      {
        MQ_entry *q = queue + i;       // next message queue entry

        if (q->due && now >= q->due)
        {
          _bl_out(&q->o,q->val,(PMI)); // post scheduled message
          release(q);                  // release (free-up) queue entry
        }
      }
    }

    return 0;
  }

//==============================================================================
// worker: system init
//==============================================================================

  static int sys_init(BL_ob *o, int val)
  {
    LOG(2,BL_B "init mpub");

    init_queue();
    return 0;
  }

//==============================================================================
// public module interface
//==============================================================================
//
// (A) := (APP);  (D) := (bl_down);  (*) := (<any>);
//
//                  +--------------------+
//                  |        mpub        | mesh publisher
//                  +--------------------+
//                  |        SYS:        | SYS input interface
// (A)->     INIT ->|       <out>        | init module, store <out> callback
// (A)->     TICK ->|       @id,cnt      | tick the module
//                  +--------------------+
//                  |      GOOCLI:       | GOOCLI input interface
// (*)->      SET ->| @id,<BL_goo>,onoff | acknowledged generic on/off set
// (*)->      LET ->| @id,<BL_goo>,onoff | unacknowledged generic on/off set
// (*)->      GET ->|        @id         | request generic on/off server status
//                  +--------------------+
//                  |      #GOOCLI:      | GOOCLI output interface
// (D)<-      SET <-| @id,<BL_goo>,onoff | acknowledged generic on/off set
// (D)<-      LET <-| @id,<BL_goo>,onoff | unacknowledged generic on/off set
// (D)<-      GET <-|        @id         | request generic on/off server status
//                  +--------------------+
//                  |        SET:        | SET input interface
// (A)->   REPEAT ->|        cnt         | set number of message repeats
// (A)-> INTERVAL ->|         ms         | set repeat interval
//                  +--------------------+
//
//==============================================================================

  int bl_mpub(BL_ob *o, int val)
  {
    static BL_oval D = bl_down;        // down gear shorthand

    switch (bl_id(o))
    {
      case SYS_INIT_0_cb_0:
        return sys_init(o,val);        // delegate to sys_init() worker

      case SYS_TICK_id_BL_pace_cnt:
        return sys_tick(o,val);        // delegate to sys_tick() worker

      case GOOCLI_SET_id_BL_goo_onoff:
      case GOOCLI_LET_id_BL_goo_onoff:
      case GOOCLI_GET_id_0_0:
        LOGO(2,"(#)",o,val);
        return goocli_any(o,val);      // delegate to gonoff_any() worker

      case _GOOCLI_SET_id_BL_goo_onoff:
      case _GOOCLI_LET_id_BL_goo_onoff:
      case _GOOCLI_GET_id_0_0:
        return bl_out(o,val,(D));      // output to down gear

      case SET_REPEAT_0_0_cnt:
        repeat = val;
        return 0;

      case SET_INTERVAL_0_0_ms:
        interval = val;
        return 0;

      default:
        return -1;                     // bad input
    }
  }
