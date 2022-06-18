//==============================================================================
// bl_time.h
// Time related Bluccino stuff
//
// Created by Hugo Pristauz on 2022-Apr-03
// Copyright © 2022 Bluenetics. All rights reserved.
//==============================================================================

#ifndef __BL_TIME_H__
#define __BL_TIME_H__

#include "bl_type.h"
#include "bl_log.h"

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

#endif // __BL_TIME_H__
