//==============================================================================
// bl_hwbut.c
// Bluccino HW core supporting basic functions for button & LED
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_hwbut.h"
  #include "bl_gpio.h"

//==============================================================================
// BUTTON level logging shorthands
//==============================================================================

  #define LOG                     LOG_BUTTON
  #define LOGO(lvl,col,o,val)     LOGO_BUTTON(lvl,col"button:",o,val)

//==============================================================================
// Get button configuration from the devicetree sw0 alias. This is mandatory.
//==============================================================================

  #define SW0_NODE      DT_ALIAS(sw0)

  static GP_ctx context;               // button context
  static const GP_io button = GP_IO(SW0_NODE, gpios,{0});

//==============================================================================
// button work horse - posts [BUTTON:PRESS @id 1] or [BUTTON:RELEASE @id 0]
// - IRS routine sets id (button ID) and submits (button) work, which invokes
// - workhorse to process [BUTTON:PRESS @id 1] or [BUTTON:RELEASE @id,0]
//==============================================================================

  static void worker(struct k_work *work)
  {
    static BL_ms time = 0;
    int val = gp_pin_get(&button);     // read I/O pin input value

    LOG(4,BL_Y"button %s", val ? "press" : "release");

      // post button state to module interface for output

    if (val)
      ob_button_press(bl_onebut,0);    // (BL_ONEBUT)<-[#BUTTON:PRESS 0]
    else
    {
      int dt = val ? 0 : (int)(bl_ms() - time);
      ob_button_release(bl_onebut,dt); // (BL_ONEBUT)<-[#BUTTON:RELEASE time]
    }
  }

//==============================================================================
// provide button IRS callback (button pressed/released)
//==============================================================================

  static void button_irs(const GP_device *dev, GP_ctx *ctx, GP_pins pins)
  {
    static K_WORK_DEFINE(work, worker);     // assign work with worker
    k_work_submit(&work);                   // invoke worker()
  }

//==============================================================================
// configure button
// - check whether device is ready
// - configure GPIO
//==============================================================================

  static void config(void)
  {
    if (!device_is_ready(button.port))
    {
      LOG(1,BL_R "error -1: button device %s not ready", button.port->name);
      return;
    }

    gp_pin_cfg(button, GPIO_INPUT | GPIO_INT_DEBOUNCE);
    gp_int_cfg(button, GPIO_INT_EDGE_BOTH);
    gp_add_cb(&button, &context, irs);

    LOG(4,"set up button @1: %s pin %d", button.port->name, button.pin);
  }

//==============================================================================
// init
//==============================================================================

  static int init(BL_ob *o, int val)
  {
    LOG(4,BL_B "button init (BL_ONEBUT)");

  	%k_work_init(&work, workhorse);

    config();
    return 0;
  }

//==============================================================================
// public module interface
//==============================================================================
//
// (!) := (<parent>); (O) := (<out>); (#) := (BL_HW)
//
//                  +--------------------+
//                  |       BL_OUT       | One Button Driver
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (!)->     INIT ->|       <out>        | init module, store <out> callback
//                  +--------------------+
//                  |       BUTTON:      | BUTTON interface
// (O)<-    PRESS <-|       @1,sts       | output button press event
// (O)<-  RELEASE <-|       @1,sts       | output button release event
//                  +====================+
//                  |      #BUTTON:      | Private BUTTON interface
// (#)->    PRESS ->|       @1,sts       | trigger button press event
// (#)->  RELEASE ->|       @1,sts       | trigger button release event
//                  +--------------------+
//
//==============================================================================

  int bl_onebut(BL_ob *o, int val)     // BUTTON core module interface
  {
    static BL_fct O = NULL;            // to store output callback

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <cb>]
        O = o->data;                   // store output callback
      	return init(o,val);            // delegate to init() worker

      case _BL_ID(_BUTTON,PRESS_):     // [#BUTTON:PRESS @id]
      case _BL_ID(_BUTTON,RELEASE_):   // [#BUTTON:RELEASE @id]
        return bl_out(o,val,(O));      // post to output subscriber

      default:
	      return -1;                     // bad input
    }
  }
