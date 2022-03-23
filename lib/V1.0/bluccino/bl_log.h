//==============================================================================
//  bl_log.h
//  bluccino logging and us/ms clock support
//
//  Created by Hugo Pristauz on 2021-11-06
//  Copyright © 2021 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_LOG_H__
#define __BL_LOG_H__

#include "bl_rtos.h"
#include "bl_type.h"
#include "bl_api.h"

#if 0
//==============================================================================
// XYZ level logging shorthands
//==============================================================================

  #define LOG                     LOG_XYZ
  #define LOGO(lvl,col,o,val)     LOGO_XYZ(lvl,col"xyz:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_XYZ(lvl,col,o,val)

#endif
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
                bl_prt(fmt BL_0, ##__VA_ARGS__); \
                if (*fmt) bl_prt("\n");          \
            }                                    \
         } while(0)

    #define bl_log(l,f,...)  BL_LOG(l,f,##__VA_ARGS__)  // always enabled

//==============================================================================
// API Logging
//==============================================================================

#ifndef CFG_LOG_API
    #define CFG_LOG_API    1           // no API logging by default
#endif

#if (CFG_LOG_API)
    #define LOG_API(l,f,...)    BL_LOG(CFG_LOG_API-1+l,f,##__VA_ARGS__)
    #define LOGO_API(l,f,o,v)   bl_logo(CFG_LOG_API-1+l,f,o,v)
#else
    #define LOG_API(l,f,...)    {}     // empty
    #define LOGO_API(l,f,o,v)   {}     // empty
#endif

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
// BASIS Logging
//==============================================================================

#ifndef CFG_LOG_BASIS
    #define CFG_LOG_BASIS    0         // BASIS logging is by default off
#endif

#if (CFG_LOG_BASIS)
    #define LOG_BASIS(l,f,...)    BL_LOG(CFG_LOG_BASIS-1+l,f,##__VA_ARGS__)
    #define LOGO_BASIS(l,f,o,v)   bl_logo(CFG_LOG_BASIS-1+l,f,o,v)
#else
    #define LOG_BASIS(l,f,...)    {}     // empty
    #define LOGO_BASIS(l,f,o,v)   {}     // empty
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
// MESH Logging
//==============================================================================

#ifndef CFG_LOG_MESH
    #define CFG_LOG_MESH    1           // MESH logging is by default on
#endif

#if (CFG_LOG_MESH)
    #define LOG_MESH(l,f,...)    BL_LOG(CFG_LOG_MESH-1+l,f,##__VA_ARGS__)
    #define LOGO_MESH(l,f,o,v)   bl_logo(CFG_LOG_MESH-1+l,f,o,v)
#else
    #define LOG_MESH(l,f,...)    {}     // empty
    #define LOGO_MESH(l,f,o,v)   {}     // empty
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

  void bl_log_color(bool attention, bool provisioned);
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
