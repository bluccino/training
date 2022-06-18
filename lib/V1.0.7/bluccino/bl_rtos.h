//==============================================================================
//  bl_rtos.h
//  realtime OS dependent includes
//
//  Created by Hugo Pristauz on 2021-12-11
//  Copyright © 2021 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_RTOS_H__
#define __BL_RTOS_H__

  #ifdef ZEPHYR
    #ifndef __ZEPHYR__
      #define __ZEPHYR__
    #endif
  #endif

  #ifndef __ZEPHYR__
    #define __NRF_SDK__ 1
  #endif

//==============================================================================
// Zephyr support
//==============================================================================

#ifdef __ZEPHYR__

  #include <zephyr.h>

  #define bl_prt             printk
  #define BL_SLEEP(ms)       k_msleep(ms)
  #define BL_VPRINTF(...)    // empty

#endif // __ZEPHYR__

//==============================================================================
// Nordic SDK support
//==============================================================================

#ifdef __NRF_SDK__

  #include <nrf_error.h>
  #include <nrf_delay.h>     // "nrf_delay.h"
  #include "log.h"
  #include "timer.h"

  #define __weak             __WEAK

  #define bl_prt             bl_printf
  #define printk(...)        bl_printf(__VA_ARGS__)

  #define BL_SLEEP(ms)       nrf_delay_ms(ms)
//  #define BL_VPRINTF(...)    SEGGER_RTT_vprintf(...)
  #define BL_VPRINTF         SEGGER_RTT_vprintf

  int BL_VPRINTF(unsigned, const char *, va_list *);

#endif

//==============================================================================
//
//==============================================================================

#endif // __BL_RTOS_H__
