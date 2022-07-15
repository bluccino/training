//==============================================================================
// fc_lfifo.h
// FIFO for logs
//
// Created by Chintan Parmar on 2022-July-05
// Copyright © 2022 fuseAware. All rights reserved.
//==============================================================================
//

#ifndef __FC_LFIFO_H__
#define __FC_LFIFO_H__

//==============================================================================
// get BL_rtl data from log fifo
// - usage: err = fc_bfifo_get(p)      // return 0:OK, -1:error
//==============================================================================

  int fc_lfifo_get(BL_rtl *p);

//==============================================================================
// put BL_rtl data to log fifo
// - usage: err = fc_bfifo_put(p)      // return 0:OK, -1:error
//==============================================================================

  int fc_lfifo_put(BL_rtl *p);

//==============================================================================
// how many <BL_rtl> data entries are in fifo?
// - usage: n = fc_bfifo_avail()            // return number of available
//==============================================================================

  int fc_lfifo_avail(void);            // return number of available <BL_rtl>

//==============================================================================
// is fifo full?
// - usage: full = fc_lfifo_full()            // return true if fifo is full
//==============================================================================

  bool fc_lfifo_full(void);

//==============================================================================
// increase the log drop counter
// - usage: drops = fc_lfifo_drop(0)     // read number of drops and clear drops
//          fc_lfifo_drop(1)             // increment drops counter
//==============================================================================

  int fc_lfifo_drop(int mode);

#endif // __FC_LFIFO_H__
