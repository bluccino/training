//==============================================================================
//  bl_log.c
//  bluccino debug logging
//
//  Created by Hugo Pristauz on 2021-11-19
//  Copyright © 2021 Bluenetics GmbH. All rights reserved.
//==============================================================================

#include <stdarg.h>
#include <stdint.h>

#include "bluccino.h"

#ifndef CFG_PRETTY_LOGGING
  #define CFG_PRETTY_LOGGING    1      // pretty text for class tag & opcode
#endif

  static BL_txt color = "";            // text color for time header
  static int debug = 1;                // debug level

//==============================================================================
// RTT log driver
//==============================================================================
#ifdef __NRF_SDK__

     // For some reason, this function, while not static, is not included
     // in the RTT header files

  int SEGGER_RTT_vprintf(unsigned, const char *, va_list *);

      // in Zephyr environment we have printk(...) function
      // here is the macro for usage in compatible style

  #ifdef printk
    #undef printk
  #endif

  #define printk(...)   bl_prt(__VA_ARGS__)

  void bl_vprintf(const char * format, va_list arguments)
  {
    BL_VPRINTF(0, format, &arguments);
  }

  void bl_prt(const char * format, ...)
  {
    va_list arguments;  // lint -save -esym(530,args) sym args not initialized
    va_start(arguments, format);
    bl_vprintf(format, arguments);
    va_end(arguments);  // lint -restore
  }

#endif // __NRF_SDK__
//==============================================================================
// set log color
//==============================================================================

  void bl_log_color(bool attention, bool provision)
  {
    color = attention ? BL_G : (provision ? BL_Y : "");
  }

  int bl_verbose(int verbose)              // set verbose level
  {
    int old = debug;
    debug = verbose;
    return old;
  }

//==============================================================================
// assertion
//==============================================================================

  void bl_assert(bool assertion)
  {
    if (!assertion)
    {
      bl_log(0,BL_R"assertion violated");
      for(;;)
        bl_sleep(10);                  // sleep to support SEGGER RTT function
    }
  }

//==============================================================================
// error message: error printing only for err != 0
// - usage: err = bl_err(err,msg)
//==============================================================================

  int bl_err(int err, BL_txt msg)
  {
    if (err)
    {
      if (bl_dbg(1))                            // errors come @ verbose level 1
        bl_prt(BL_R "error %d: %s\n",err,msg);  // in RED text!
    }
    return err;
  }

//==============================================================================
// get clock time as minutes, seconds, milliseconds
//==============================================================================

  static void now(int *pmin, int *psec, int *pms, int *pus)  // split us time
  {
    static int min = 0;
    static int sec = 0;
    static BL_ms offset = 0;
    BL_us us = bl_us();                    // clock time now in us

    *pus = us % 1000;                      // map us to range 0 .. 999
    *pms = us/1000 - offset;

      // calculate trace time tick

    for (; *pms >= 1000; offset += 1000, sec++)
      *pms -= 1000;

    for (; sec >= 60; min++)
      sec -= 60;

    *pmin = min;  *psec = sec;
  }

//==============================================================================
// debug tracing
//==============================================================================

  bool bl_dbg(int lev)
  {
    if (lev > debug)
      return false;

    int min, sec, ms, us;
    now(&min,&sec,&ms,&us);

      // print header in green if in attention mode,
      // yellow if node is provisioned, otherwise white by default

    printk("%s#%d[%03d:%02d:%03d.%03d] " BL_0, color,lev, min,sec,ms,us);

    for (int i=0; i < lev; i++)
    {
      printk("  ");                   // indentation
    }

    return true;
  }

//==============================================================================
// log helper
//==============================================================================
#if CFG_PRETTY_LOGGING

  static BL_txt cltext(BL_cl cl)
  {
    static BL_txt text[] = BL_CL_TEXT;
    return (cl < BL_LENGTH(text)) ? text[cl] : "???";
  }

  static BL_txt optext(int op)
  {
    static BL_txt text[] = BL_OP_TEXT;
    op = (op < 0) ? -op : op;
    return (op < BL_LENGTH(text)) ? text[op] : "???";
  }

#endif
//==============================================================================
// log messages
//==============================================================================

  void bl_logo(int lev, BL_txt msg, BL_ob *o, int value) // log event message
  {
    if ( !bl_dbg(lev) )
     return;

    BL_txt hash = BL_HASHED(o->op) ? "#" : "";
    BL_op op = BL_CLEAR(o->op);

    BL_txt col = (msg[0] != '@') ? "" : (value ? BL_G : BL_M);
    msg = (msg[0] == '@') ? msg+1 : msg;

    #if CFG_PRETTY_LOGGING             // pretty text for class tag & opcode
      bl_prt("%s%s [%s:%s%s @%d,%d]\n"BL_0, col,msg,
             cltext(o->cl),hash,optext(op),o->id,value);
    #else
      bl_prt("%s%s [%d:%s%d @%d,%d]\n"BL_0,col,msg,
             o->cl,hash,op,o->id,value);
    #endif
  }
