//==============================================================================
// bl_api.c - bluccino API layer
//==============================================================================

  #include "bluccino.h"

//==============================================================================
// API level logging shorthands
//==============================================================================

  #define LOG                     LOG_API
  #define LOGO(lvl,col,o,val)     LOGO_API(lvl,col"api:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_API(lvl,col,o,val)

//==============================================================================
// us/ms clock
//==============================================================================

  static BL_us offset = 0;             // offset for us clock

  static BL_us now_us()                // system clock in us
  {
    #if __ZEPHYR__
      uint64_t cyc = k_uptime_ticks();
      uint64_t f = sys_clock_hw_cycles_per_sec();
      return (BL_us)((1000000*cyc)/f);
    #else
      return (BL_us)timer_now();
    #endif
  }

  BL_us bl_zero(void)                  // reset clock
  {
    return offset = now_us();
  }

//==============================================================================
// get us clock time
//==============================================================================

  BL_us bl_us(void)                    // get current clock time in us
  {
    BL_us us = now_us();

    if (offset == 0)                   // first call always returns 0
      us = bl_zero();                  // reset clock

    return us  - offset;
  }

//==============================================================================
// get ms clock time
//==============================================================================

  BL_ms bl_ms(void)                    // get current clock time in ms
  {
    BL_us us = bl_us();
    return us/1000;
  }

//==============================================================================
// periode detection
// - usage: ok = bl_period(o,ms)        // is tick/tock time meeting a period?
//==============================================================================

  bool bl_period(BL_ob *o, BL_ms ms)
  {
    BL_pace *p = bl_data(o);
    return p ? ((p->time % ms) == 0) : 0;
  }

//==============================================================================
// timing & sleep
//==============================================================================

  bool bl_due(BL_ms *pdue, BL_ms ms)   // check if time if due & update
  {
    BL_ms now = bl_ms();

    if (now < *pdue)                   // time for tick action is due?
      return false;                    // no, not yet due!

    *pdue = now + ms;                  // catch-up due time
    return true;                       // yes, tick time due!
  }

//void bl_sleep(int ms)                // deep sleep for given milliseconds
//{
//  nrf_delay_ms(ms);
//}

  void bl_sleep(BL_ms ms)              // deep sleep for given milliseconds
  {
    if (ms > 0)
      BL_SLEEP((int)ms);
  }

  void bl_halt(BL_txt msg, BL_ms ms)   // halt system
  {
    LOG(0,BL_R"%s: system halted", msg);
    for (;;)
      bl_sleep(ms);
  }
