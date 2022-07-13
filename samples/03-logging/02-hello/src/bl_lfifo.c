//==============================================================================
// fc_lfifo.c
// FIFO for real time logging
//
// Created by Chintan Parmar on 2022-July-04
// Copyright © 2022 fuseAware. All rights reserved.
//==============================================================================

  #include <string.h>

  #include "bluccino.h"
  #include "bl_rtl.h"
  #include "bl_lfifo.h"

//==============================================================================
// CORE level logging shorthands
//==============================================================================

  #define LOG                     LOG_CORE
  #define LOGO(lvl,col,o,val)     LOGO_CORE(lvl,col"fc_lfifo:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_CORE(lvl,col,o,val)

//==============================================================================
// log fifo
//==============================================================================

  #define NLFIFO 50                              // number of logs in fifo

  typedef struct FA_lfifo                        // log fifo
          {
            BL_rtl fifo[NLFIFO];                 // to store log data
            uint8_t len;                         // fifo length
            uint8_t gdx;                         // get index
            uint8_t pdx;                         // put index
            volatile uint8_t free;               // number of free fifo items
          } FA_lfifo;                            // log fifo

    // THE log fifo

  static FA_lfifo lfifo = { len: NLFIFO, gdx:0, pdx:0, free: NLFIFO };
  static int drops = 0;                         // drop counter for log messages

//==============================================================================
// get FA_rtl data from log fifo
// - usage: err = fc_lfifo_get(p)      // return 0:OK, -1:error
//==============================================================================

  int fc_lfifo_get(BL_rtl *p)
  {
//    if (fl_dbg(6))
//      bl_prt(BL_C"lfifo: get(%d)\n",lfifo.len-lfifo.free);

    if (lfifo.free == lfifo.len)
      return -1;                            // no more log data in fifo

    BL_rtl *q = lfifo.fifo + lfifo.gdx;
    lfifo.gdx = (lfifo.gdx + 1) % lfifo.len;// cyclic increment of get index

    memcpy(p,q,sizeof(BL_rtl));             // copy log data from fifo
    lfifo.free++;                           // another one fifo cell went free
    return 0;                               // OK
  }

//==============================================================================
// put FA_rtl data to log fifo
// - usage: err = fc_lfifo_put(p)      // return 0:OK, -1:error
//==============================================================================

  int fc_lfifo_put(BL_rtl *p)
  {
//    if (fl_dbg(6))
//      bl_prt(BL_C"lfifo: put(%d)\n",lfifo.len-lfifo.free+1);

    if (lfifo.free == 0)
      return -1;                            // log fifo full

    BL_rtl *q = lfifo.fifo + lfifo.pdx;     // pointer to free fifo cell
    lfifo.pdx = (lfifo.pdx + 1) % NLFIFO;   // cyclic increment of put index

    memcpy(q,p,sizeof(BL_rtl));             // copy log data into fifo
    lfifo.free--;
    return 0;
  }

//==============================================================================
// how many <FA_rtl> data entries are in fifo?
// - usage: n = fc_lfifo_avail()            // return number of available
//==============================================================================

  int fc_lfifo_avail(void)
  {
    return (lfifo.len - lfifo.free);
  }

//==============================================================================
// is fifo full?
// - usage: full = fc_lfifo_full()            // return true if fifo is full
//==============================================================================

  bool fc_lfifo_full(void)
  {
    return (lfifo.free == 0);
  }

//==============================================================================
// increase the log drop counter
// - usage: drops = fc_lfifo_drop(0)     // read number of drops and clear drops
//          fc_lfifo_drop(1)             // increment drops counter
//==============================================================================

  int fc_lfifo_drop(int mode)
  {
    if(!mode)                           // read number of drops and clear drops
    {
      int drop_cnt = drops;
      drops = 0;
      return drop_cnt;
    }
    else                                // increment drops counter
    {
        drops++;
        return drops;
    }
  }
