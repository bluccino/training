//==============================================================================
// bl_api.h - bluccino API
//==============================================================================

#ifndef __BL_API_H__
#define __BL_API_H__

#include "bl_log.h"
#include "bl_type.h"
#include "bl_symb.h"
#include "bl_vers.h"

  extern bool bl_attention;            // attention mode
  extern bool bl_provisioned;          // provisioned mode

//==============================================================================
// mesh opcodes
//==============================================================================

  #define BL_GOOLET 0x8203

//==============================================================================
// syntactic sugar: compound message identifier
// - usage: bl_id(o)                   // same as BL_PAIR(o->cl,o->op)
//==============================================================================

  static inline int bl_id(BL_ob *o)
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
// tic/toc functions (elapsed time benchmarking)
// - usage: bl_tic(o,10000);           // run 10000 loops
// -        for (int i=0; i < o->id; i++)
// -          { ... }  // do some work (e.g. call function with OVAL interface)
// -        bl_toc(o,"OVAL call");
//==============================================================================

  static inline int bl_tic(BL_ob *o, int n)
  {
    BL_us now = bl_us();
//bl_prt("now: %d us\n",(int)now);
    o->data = (const void*)(int)now;
    return (o->id = n);                // save number of loops in object's @id
  }

  static inline void bl_toc(BL_ob *o, BL_txt msg)
  {
    int now = (int)bl_us();
    int begin = (int)o->data;
    int elapsed = (100*(now - begin)) / o->id;    // devide by number of runs
//bl_prt("begin: %d us, now: %d us\n",begin,now);
    if (bl_dbg(1))
      bl_prt("%s: %d.%02d us\n", msg, elapsed/100, elapsed%100);
  }

//==============================================================================
// post general message [CL:OP @id,<data>,val] to module
// - usage: bl_msg(module,cl,op,id,data,val)
//==============================================================================

  static inline
	  int bl_msg(BL_oval module, BL_cl cl,BL_op op, int id, BL_data data, int val)
  {
    BL_ob oo = {cl,op,id,data};
    return module(&oo,val);            // post message to module interface
  }

//==============================================================================
// post general augmented message [CL:OP @id,<data>,val] to module
// - usage: _bl_msg(module,cl,op,id,data,val)
//==============================================================================

  static inline
	  int _bl_msg(BL_oval module, BL_cl cl,BL_op op, int id, BL_data data,int val)
  {
    BL_ob oo = {BL_AUG(cl),op,id,data};// augmented class tag
    return module(&oo,val);            // post message to module interface
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
// syntactig sugar: init a module
// usage:  bl_init(module,cb)       // init module, <out> goes to callback
//==============================================================================

  static inline int bl_init(BL_oval module,BL_oval cb)
  {
    return bl_msg(module,_SYS,INIT_, 0,cb,0);  // init module
  }

//==============================================================================
// syntactic sugar: tick a module
// - usage: bl_tick(module,count)   // (MODULE)<-[SYS:TICK @id,count]
//==============================================================================

  static inline int bl_tick(BL_oval module, int id, int cnt)
  {
    return bl_msg(module,_SYS,TICK_, id,NULL,cnt);
  }

//==============================================================================
// syntactic sugar: tock a module
// - usage: bl_tock(module,count)   // (MODULE)<-[SYS:TOCK @id,count]
//==============================================================================

  static inline int bl_tock(BL_oval module, int id, int cnt)
  {
    return bl_msg(module,_SYS,TOCK_, id,NULL,cnt);
  }

//==============================================================================
// bl_cfg (syntactic sugar to config a given module)
// - usage: bl_cfg(module,mask)  // (<module>) <- [SYS:CFG mask]
//==============================================================================

  static inline int bl_cfg(BL_oval module, BL_cl cl, BL_word mask)
  {
    return bl_msg(module,cl,CFG_, 0,NULL,(int)mask);
  }

#endif // __BL_API_H__
