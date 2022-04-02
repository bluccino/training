//==============================================================================
// button.c
// Bluccino HW core s   upporting basic functions for button & LED
//
// Created by Hugo Pristauz on 2022-Feb-18
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================

  #include "bluccino.h"
  #include "bl_gpio.h"
  #include "button.h"

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
  static const GP_io button_io = GP_IO(SW0_NODE, gpios,{0});

//==============================================================================
// button work horse - posts [BUTTON:PRESS @id 1] or [BUTTON:RELEASE @id 0]
// - IRS routine sets id (button ID) and submits (button) work, which invokes
// - workhorse to process [BUTTON:PRESS @id 1] or [BUTTON:RELEASE @id,0]
//==============================================================================

  static void worker(struct k_work *work)
  {
    if (gp_pin_get(&button_io))
    {
      LOG(5,BL_Y "button press");

      // post button state to module interface for output

      button_press(button);      // (BUTTON)<-[#BUTTON:PRESS]
    }
  }

//==============================================================================
// provide button IRS callback (button pressed/released)
//==============================================================================

  static K_WORK_DEFINE(work, worker);     // assign work with worker

  static void button_irs(const GP_device *dev, GP_ctx *ctx, GP_pins pins)
  {
     k_work_submit(&work);                   // invoke worker()
  }

//==============================================================================
// configure button
// - check whether device is ready
// - configure GPIO
//==============================================================================

  static void config(void)
  {
    if (!device_is_ready(button_io.port))
    {
      LOG(1,BL_R "error -1: button device %s not ready", button_io.port->name);
      return;
    }

    gp_pin_cfg(&button_io, GPIO_INPUT | GPIO_INT_DEBOUNCE);
    gp_int_cfg(&button_io, GPIO_INT_EDGE_BOTH);
    gp_add_cb(&button_io, &context, button_irs);

    LOG(4,"set up button @1: %s pin %d", button_io.port->name, button_io.pin);
  }

//==============================================================================
// init
//==============================================================================

  static int init(BL_ob *o, int val)
  {
    LOG(3,BL_B "button init (1 button)");

    k_work_init(&work, worker);

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
//                  |       BUTTON       |
//                  +--------------------+
//                  |        SYS:        | SYS interface
// (!)->     INIT ->|       <out>        | init module, store <out> callback
//                  +--------------------+
//                  |       BUTTON:      | BUTTON interface
// (O)<-    PRESS <-|       @1,sts       | output button press event
//                  +====================+
//                  |      #BUTTON:      | Private BUTTON interface
// (#)->    PRESS ->|       @1,sts       | trigger button press event
//                  +--------------------+
//
//==============================================================================

  int button(BL_ob *o, int val)        // BUTTON core module interface
  {
    static BL_oval O = NULL;            // to store output callback

    switch (bl_id(o))                  // message ID? ([cl:op])
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <cb>]
        O = o->data;                   // store output callback
      	return init(o,val);            // delegate to init() worker

      case _BL_ID(_BUTTON,PRESS_):     // [#BUTTON:PRESS @id]
			  bl_logo(4,BL_Y "button",o,val);
        return bl_out(o,val,(O));      // post to output subscriber

      default:
	      return -1;                     // bad input
    }
  }
