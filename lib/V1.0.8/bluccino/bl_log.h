//==============================================================================
//  bl_log.h
//  bluccino logging and us/ms clock support
//
//  Created by Hugo Pristauz on 2021-11-06
//  Copyright © 2021 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_LOG_H__
#define __BL_LOG_H__

#include <stdio.h>
#include "bl_rtos.h"
#include "bl_type.h"
#include "bl_msg.h"

#if 0
//==============================================================================
// logging shorthands
//==============================================================================

  #define WHO "xyz:"              // who is logging?

  #define LOG                     LOG_XYZ
  #define LOGO(lvl,col,o,val)     LOGO_XYZ(lvl,col WHO,o,val)
  #define LOG0(lvl,col,o,val)     LOGO_XYZ(lvl,col,o,val)

#endif
//==============================================================================
// generic log function
// - the whole macro is a weird construction, but it fulfills what expected!
// - the outer do {..} while(0) construct seems weird, but it allows a syntax:
// - if (condition) BL_LOG(1,"..."); else BL_LOG(1,"...");
//==============================================================================
#if (CFG_BLUCCINO_RTL)

    //==============================================================================
    // BL_rtl (real time log)
    //==============================================================================

    typedef struct BL_rtl
    {
        char buf[200];          // log buffer
        volatile bool busy;
        volatile int len;
    } BL_rtl;

    extern BL_rtl bl_rtl;

    void bl_rtl_get(BL_rtl *p);
    void bl_rtl_put(BL_rtl *p);
    //==============================================================================
    // debug tracing
    //==============================================================================

    // bool bl_dbg(int lev);

    //==============================================================================
    // generic log function
    // - the whole macro is a weird construction, but it fulfills what expected!
    // - the outer do {..} while(0) construct seems weird, but it allows a syntax:
    // - if (condition) BL_LOG(1,"..."); else BL_LOG(1,"...");
    //==============================================================================

        #define BL_LOG(lvl,fmt,...)                  \
            do                                      \
            {                                       \
                if (bl_dbg(lvl))                     \
                {                                    \
                bl_irq(0);                         \
                bl_rtl.len = sprintf(bl_rtl.buf, fmt BL_0, ##__VA_ARGS__); \
                if (*fmt) sprintf(bl_rtl.buf+bl_rtl.len, "\n");  \
                bl_rtl_put(&bl_rtl);          \
                bl_irq(1);                         \
                }                                    \
            } while(0)

        #define bl_log(l,f,...)  BL_LOG(l,f,##__VA_ARGS__)  // always enabled

    //==============================================================================
    // bl_rtl_init: initializes real time logging.
    //==============================================================================

    void bl_rtl_init(void);

    
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

#else                                  // standard log macro version

        #define BL_LOG(lvl,fmt,...)                  \
            do                                      \
            {                                       \
                if (bl_dbg(lvl))                     \
                {                                    \
                    bl_prt(fmt BL_0, ##__VA_ARGS__); \
                    if (*fmt) bl_prt("\n");          \
                }                                    \
            } while(0)

        #define bl_log(l,f,...)  BL_LOG(l,f,##__VA_ARGS__)  // always enabled

#endif // CFG_BLUCCINO_RTL
//==============================================================================
// APP Logging
//==============================================================================

#ifndef CFG_LOG_APP
    #define CFG_LOG_APP    1           // APP logging is by default on
#endif

#if (CFG_LOG_APP)
    #define LOG_APP(l,f,...)    BL_LOG(CFG_LOG_APP-1+l,f,##__VA_ARGS__)
    #define LOGO_APP(l,f,o,v)   bl_logo(CFG_LOG_APP-1+l,f,o,v)
#else
    #define LOG_APP(l,f,...)    {}     // empty
    #define LOGO_APP(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// BUTTON Logging
//==============================================================================

#ifndef CFG_LOG_BUTTON
    #define CFG_LOG_BUTTON    1           // BUTTON logging is by default on
#endif

#if (CFG_LOG_BUTTON)
    #define LOG_BUTTON(l,f,...)    BL_LOG(CFG_LOG_BUTTON-1+l,f,##__VA_ARGS__)
    #define LOGO_BUTTON(l,f,o,v)   bl_logo(CFG_LOG_BUTTON-1+l,f,o,v)
#else
    #define LOG_BUTTON(l,f,...)    {}     // empty
    #define LOGO_BUTTON(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// CORE Logging
//==============================================================================

#ifndef CFG_LOG_CORE
    #define CFG_LOG_CORE    1           // CORE logging is by default on
#endif

#if (CFG_LOG_CORE)
    #define LOG_CORE(l,f,...)    BL_LOG(CFG_LOG_CORE-1+l,f,##__VA_ARGS__)
    #define LOGO_CORE(l,f,o,v)   bl_logo(CFG_LOG_CORE-1+l,f,o,v)
#else
    #define LOG_CORE(l,f,...)    {}     // empty
    #define LOGO_CORE(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// GEAR Logging
//==============================================================================

#ifndef CFG_LOG_GEAR
    #define CFG_LOG_GEAR    1           // GEAR logging is by default on
#endif

#if (CFG_LOG_GEAR)
    #define LOG_GEAR(l,f,...)    BL_LOG(CFG_LOG_GEAR-1+l,f,##__VA_ARGS__)
    #define LOGO_GEAR(l,f,o,v)   bl_logo(CFG_LOG_GEAR-1+l,f,o,v)
#else
    #define LOG_GEAR(l,f,...)    {}     // empty
    #define LOGO_GEAR(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// GPIO Logging
//==============================================================================

#ifndef CFG_LOG_GPIO
    #define CFG_LOG_GPIO    0           // GPIO logging is by default off
#endif

#if (CFG_LOG_GPIO)
    #define LOG_GPIO(l,f,...)    BL_LOG(CFG_LOG_GPIO-1+l,f,##__VA_ARGS__)
    #define LOGO_GPIO(l,f,o,v)   bl_logo(CFG_LOG_GPIO-1+l,f,o,v)
#else
    #define LOG_GPIO(l,f,...)    {}     // empty
    #define LOGO_GPIO(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// LED Logging
//==============================================================================

#ifndef CFG_LOG_LED
    #define CFG_LOG_LED    1           // LED logging is by default on
#endif

#if (CFG_LOG_LED)
    #define LOG_LED(l,f,...)    BL_LOG(CFG_LOG_LED-1+l,f,##__VA_ARGS__)
    #define LOGO_LED(l,f,o,v)   bl_logo(CFG_LOG_LED-1+l,f,o,v)
#else
    #define LOG_LED(l,f,...)    {}     // empty
    #define LOGO_LED(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// MAIN Logging
//==============================================================================

#ifndef CFG_LOG_MAIN
    #define CFG_LOG_MAIN    1           // MAIN logging is by default on
#endif

#if (CFG_LOG_MAIN)
    #define LOG_MAIN(l,f,...)    BL_LOG(CFG_LOG_MAIN-1+l,f,##__VA_ARGS__)
    #define LOGO_MAIN(l,f,o,v)   bl_logo(CFG_LOG_MAIN-1+l,f,o,v)
#else
    #define LOG_MAIN(l,f,...)    {}     // empty
    #define LOGO_MAIN(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// MESH Logging
//==============================================================================

#ifndef CFG_LOG_MESH
    #define CFG_LOG_MESH    1          // MESH logging is by default on
#endif

#if (CFG_LOG_MESH)
    #define LOG_MESH(l,f,...)    BL_LOG(CFG_LOG_MESH-1+l,f,##__VA_ARGS__)
    #define LOGO_MESH(l,f,o,v)   bl_logo(CFG_LOG_MESH-1+l,f,o,v)
#else
    #define LOG_MESH(l,f,...)    {}    // empty
    #define LOGO_MESH(l,f,o,v)   {}    // empty
#endif

//==============================================================================
// NVM Logging
//==============================================================================

#ifndef CFG_LOG_NVM
    #define CFG_LOG_NVM    1           // NVM logging is by default on
#endif

#if (CFG_LOG_NVM)
    #define LOG_NVM(l,f,...)    BL_LOG(CFG_LOG_NVM-1+l,f,##__VA_ARGS__)
    #define LOGO_NVM(l,f,o,v)   bl_logo(CFG_LOG_NVM-1+l,f,o,v)
#else
    #define LOG_NVM(l,f,...)    {}     // empty
    #define LOGO_NVM(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// TEST Logging
//==============================================================================

#ifndef CFG_LOG_TEST
    #define CFG_LOG_TEST    1           // TEST logging is by default on
#endif

#if (CFG_LOG_TEST)
    #define LOG_TEST(l,f,...)    BL_LOG(CFG_LOG_TEST-1+l,f,##__VA_ARGS__)
    #define LOGO_TEST(l,f,o,v)   bl_logo(CFG_LOG_TEST-1+l,f,o,v)
#else
    #define LOG_TEST(l,f,...)    {}     // empty
    #define LOGO_TEST(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// TIME Logging
//==============================================================================

#ifndef CFG_LOG_TIME
    #define CFG_LOG_TIME    1           // no TIME logging by default
#endif

#if (CFG_LOG_TIME)
    #define LOG_TIME(l,f,...)    BL_LOG(CFG_LOG_TIME-1+l,f,##__VA_ARGS__)
    #define LOGO_TIME(l,f,o,v)   bl_logo(CFG_LOG_TIME-1+l,f,o,v)
#else
    #define LOG_TIME(l,f,...)    {}     // empty
    #define LOGO_TIME(l,f,o,v)   {}     // empty
#endif

//==============================================================================
// ANSI color sequences
//==============================================================================

    #define BL_R     "\x1b[31m"        // red
    #define BL_G     "\x1b[32m"        // green
    #define BL_Y     "\x1b[33m"        // yellow
    #define BL_B     "\x1b[34m"        // blue
    #define BL_M     "\x1b[35m"        // magenta
    #define BL_C     "\x1b[36m"        // cyan
    #define BL_0     "\x1b[0m"         // reset

//==============================================================================
// formatted print - like printf("...",...)
//==============================================================================

  void bl_prt(const char * format, ...);

//==============================================================================
// debug tracing
//==============================================================================

  bool bl_dbg(int lev);                // if debug level trace timestamp
//void bl_log(int lev, BL_txt msg);
//void bl_log1(int lev, BL_txt msg, int value);
//void bl_log2(int lev, BL_txt msg, int id, int value);
  void bl_logo(int lev, BL_txt msg, BL_ob *o, int value);

  void bl_decorate(bool attention, bool provisioned);
  int bl_verbose(int verbose);        // set verbose level

//==============================================================================
// shut off after n calls (for n > 0)
// - note: function is deactivated for n=0
//==============================================================================

  static inline void bl_shut(int n)    // set verbose level 0 after n calls
  {
    static int count = 0;
    if (n == 0 || count > n)
       return;                         // function is deactivated for n=0
    if (bl_dbg(0))
      bl_prt(BL_Y"verbose countdown: %d",n-count);
    if (++count > n)
      bl_verbose(0);
  }

//==============================================================================
// assertion
//==============================================================================

  void bl_assert(bool assertion);

//==============================================================================
// error message: error printing only for err != 0
// - usage: err = bl_err(err,msg)
//==============================================================================

  int bl_err(int err, BL_txt msg);

#endif // __BL_LOG_H__
