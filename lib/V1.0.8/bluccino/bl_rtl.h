//==============================================================================
//  bl_rtl.h
//  Bluccino real time logging (supporting dongle logging)
//
//  Created by Chintan parmar on 2022-07-01
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_RTL_H__
#define __BL_RTL_H__

  #include <stdio.h>

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

#endif // __BL_RTL_H__
