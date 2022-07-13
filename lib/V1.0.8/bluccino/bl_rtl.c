//==============================================================================
//  bl_rtl.c
//  Bluccino real time logging (supporting dongle logging)
//
//  Created by Chintan parmar on 2022-07-01
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

  #include <usb/usb_device.h>
  #include <drivers/uart.h>
  #include <stdio.h>

  #include "bluccino.h"
  #include "bl_rtl.h"
  //#include "..\core\fc_lfifo.h"

//==============================================================================
// CORE level logging shorthands
//==============================================================================

  #define LOG                     LOG_CORE
  #define LOGO(lvl,col,o,val)     LOGO_CORE(lvl,col"bl_rtl:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_CORE(lvl,col,o,val)

//===============================================================================

//static int debug = 6;                // debug level
//static BL_txt color = BL_Y;            // text color for time header
//static BL_oval test = NULL;

  BL_rtl bl_rtl, bl_rtl_prt;

//==============================================================================
// print work horse - send fifo logs data bl_prt
// - K_WORK_DEFINE(print_work,workhorse); // assign print work with work horse
//==============================================================================

  static void workhorse(struct k_work *work)
  {
    int drops = fc_lfifo_drop(0);       // read number of drops and clear drops
    if(drops)
      bl_prt(BL_R"*** %d messages dropped\n",drops);

    while(fc_lfifo_avail())           // number of available log messages in fifo
    {
      bl_rtl_get(&bl_rtl_prt);
      bl_prt("%s",bl_rtl_prt.buf);
    }
  }

  K_WORK_DEFINE(print_work, workhorse);    // assign work buffer with workhorse

  void bl_rtl_put(BL_rtl *p)
  {
    if(fc_lfifo_full())                 // cannot put message in the fifo?
      fc_lfifo_drop(1);                 // increment drops counter
    else
    {
      fc_lfifo_put(p);
      k_work_submit(&print_work);     // continue at button_workhorse()
    }
  }

  void bl_rtl_get(BL_rtl *p)
  {
    fc_lfifo_get(p);
  }

//==============================================================================
// bl_rtl_init: initializes real time logging.
//==============================================================================

  void bl_rtl_init(void)
  {
    const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
    uint32_t dtr = 0;

    if (usb_enable(NULL))
      return;

    // poll if the DTR flag was set
    while (!dtr)
    {
      uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
      // give CPU resources to low priority threads.
      bl_sleep(100);
    }
    k_work_init(&print_work, workhorse);
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
    bl_irq(0);
    sprintf(bl_rtl.buf, "%s#%d[%03d:%02d:%03d.%03d] " BL_0, color,lev, min,sec,ms,us);

    for (int i=0; i < lev; i++)
      strcat(bl_rtl.buf,"  ");                   // indentation

    bl_rtl_put(&bl_rtl);
    bl_irq(1);

    return true;
  }
