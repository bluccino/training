//==============================================================================
// bl_msg.h
// Bluccino messaging
//
// Created by Hugo Pristauz on 2022-Apr-03
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

#ifndef __BL_MSG_H__
#define __BL_MSG_H__

#include "bl_app.h"
#include "bl_log.h"
#include "bl_symb.h"
#include "bl_type.h"
#include "bl_vers.h"

  extern bool bl_attention;            // attention mode
  extern bool bl_provisioned;          // provisioned mode

//==============================================================================
// mesh opcodes
//==============================================================================

// #define BL_GOOLET 0x8203

//==============================================================================
// syntactic sugar: compound message identifier
// - usage: bl_id(o)                   // same as BL_PAIR(o->cl,o->op)
//==============================================================================

  static inline BL_id bl_id(BL_ob *o)
  {
    return BL_ID(o->cl,o->op);
  }

//==============================================================================
// syntactic sugar: is event of given <class,opcode>?
// - usage: bl_is(o,_SYS,TOCK_)
//==============================================================================

  static inline bool bl_is(BL_ob *o, BL_cl cl, BL_op op)
  {
    return ( bl_id(o) == BL_ID(cl,op) );
  }

//==============================================================================
// us/ms clock
//==============================================================================

  BL_us bl_zero(void);                 // reset clock
  BL_us bl_us(void);                   // get current clock time in us
  BL_ms bl_ms(void);                   // get current clock time in ms

//==============================================================================
// periode detection
// - usage: ok = bl_period(o,ms)        // is tick/tock time meeting a period?
//==============================================================================

  bool bl_period(BL_ob *o, BL_ms ms);

//==============================================================================
// timing & sleep, system halt
//==============================================================================

  bool bl_due(BL_ms *pdue, BL_ms ms);  // check if time if due & update
  void bl_sleep(BL_ms ms);             // deep sleep for given milliseconds
  void bl_halt(BL_txt msg, BL_ms ms);  // halt system

//==============================================================================
// post general message [CL:OP @id,<data>,val] to module
// - usage: bl_post((to),mid,id,data,val)
//          bl_post((to),SYS_INIT_0_cb_0, 0,cb,0)
//==============================================================================

  static inline int bl_post(BL_oval to, BL_id mid, int id,BL_data data,int val)
  {
    BL_ob oo = {BL_HW(mid),BL_LW(mid),id,data};
    return to(&oo,val);            // post message to module interface
  }

//==============================================================================
// post general message [CL:OP @id,<data>,val] to module
// - usage: bl_msg(module,cl,op,id,data,val)
//==============================================================================

  static inline
    int bl_msg(BL_oval to, BL_cl cl, BL_op op, int id, BL_data data, int val)
  {
    BL_ob oo = {cl,op,id,data};
    return to(&oo,val);            // post message to module interface
  }

//==============================================================================
// post general augmented message [CL:OP @id,<data>,val] to module
// - usage: _bl_msg(module,cl,op,id,data,val)
//==============================================================================

  static inline
    int _bl_msg(BL_oval to, BL_cl cl, BL_op op, int id, BL_data data, int val)
  {
    BL_ob oo = {BL_AUG(cl),op,id,data};// augmented class tag
    return to(&oo,val);            // post message to module interface
  }

//==============================================================================
// get module property
// - usage: val = bl_get(module,op)    // use opcodes for property names
//==============================================================================

  static inline int bl_get(BL_oval module, BL_op op)
  {
    BL_ob oo = {_GET,op,0,NULL};
    return module(&oo,0);              // post to test module via upward gear
  }

//==============================================================================
// set module property
// - usage: val = bl_set(module,op,val) // use opcodes for property names
//==============================================================================

  static inline int bl_set(BL_oval module, BL_op op, int val)
  {
    BL_ob oo = {_SET,op,0,NULL};
    return module(&oo,val);            // post to test module via upward gear
  }

//==============================================================================
// syntactic sugar: ping a module
// - usage: bl_ping(module,"hello!")
//==============================================================================

  static inline BL_txt bl_ping(BL_oval module, BL_txt msg)
  {
    BL_ob oo = {_SYS,PING_,0,msg};
    module(&oo,0);                     // send [SYS:PING <msg>] to MODULE
    return (BL_txt)oo.data;
  }

//==============================================================================
// syntactic sugar: ping a module
// - usage: bl_pong(o,msg)
//==============================================================================

  static inline int bl_pong(BL_ob *o, BL_txt msg)
  {
    o->data = msg;
    return 0;
  }

//==============================================================================
// bl_hello (syntactic sugar to set verbose level and print a hello message)
// - usage: bl_hello(verbose,msg)
//==============================================================================

  static inline void bl_hello(int verbose, BL_txt msg)
  {
    bl_verbose(verbose);               // set verbose level
		bl_prt("*** Bluccino v%s\n",BL_VERSION);

    if (bl_dbg(0))                     // always - start print with time stamp
      bl_prt(BL_R "%s\n" BL_0,msg);    // print hello message in red
  }

//==============================================================================
// bl_data (syntactic sugar to cast const void* data to non-const)
// - usage: MY_data *p = bl_data(o)
//==============================================================================

  static inline void *bl_data(BL_ob *o)
  {
    return (void*)o->data;
  }

//==============================================================================
// bl_cfg (syntactic sugar to config a given module)
// - usage: bl_cfg(module,mask)  // (<module>) <- [SYS:CFG mask]
//==============================================================================

  static inline int bl_cfg(BL_oval module, BL_cl cl, BL_word mask)
  {
    return bl_msg(module,cl,CFG_, 0,NULL,(int)mask);
  }

#endif // __BL_MSG_H__
