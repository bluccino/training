//==============================================================================
// bl_core.c
// multi model mesh demo based mesh core
//
// Created by Hugo Pristauz on 2022-Jan-02
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================
// mcore derived from:
// Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
// Copyright (c) 2018 Vikrant More
// SPDX-License-Identifier: Apache-2.0
//==============================================================================

  #include <drivers/gpio.h>

  #include "bluccino.h"
  #include "bl_core.h"
  #include "bl_hw.h"

  #include "ble_mesh.h"
  #include "device_composition.h"
  #include "no_transition_work_handler.h"
  #include "state_binding.h"
  #include "storage.h"
  #include "transition.h"
  #include "publisher.h"

  #define _DUE_    BL_HASH(DUE_)       // hashed opcode for DUE

//==============================================================================
// CORE level logging shorthands
//==============================================================================

  #define LOG                     LOG_CORE
  #define LOGO(lvl,col,o,val)     LOGO_CORE(lvl,col"bl_core:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_CORE(lvl,col,o,val)
  #define ERR 1,BL_R

//==============================================================================
// let's go ...
//==============================================================================

#ifndef CFG_BT_ENABLE                  // define in config.h
  #define CFG_BT_ENABLE   1            // by default bluetooth will be enabled
#endif

#if defined(CONFIG_MCUMGR)
  #include <mgmt/mcumgr/smp_bt.h>
  #include "smp_svr.h"
#endif

static bool reset;

static void light_default_var_init(void)
{
	ctl->tt = 0x00;

	ctl->onpowerup = STATE_DEFAULT;

	ctl->light->range_min = LIGHTNESS_MIN;
	ctl->light->range_max = LIGHTNESS_MAX;
	ctl->light->last = LIGHTNESS_MAX;
	ctl->light->def = LIGHTNESS_MAX;
	ctl->light->target = ctl->light->def;

	ctl->temp->range_min = TEMP_MIN;
	ctl->temp->range_max = TEMP_MAX;
	ctl->temp->def = TEMP_MAX;
	ctl->temp->target = ctl->temp->def;

	ctl->duv->def = DELTA_UV_DEF;
	ctl->duv->target = ctl->duv->def;
}

/* This function should only get call after execution of settings_load() */
static void light_default_status_init(void)
{
	/* Retrieve Range of Lightness */
	if (ctl->light->range) {
		ctl->light->range_max = (uint16_t) (ctl->light->range >> 16);
		ctl->light->range_min = (uint16_t) ctl->light->range;
	}

	/* Retrieve Range of Temperature */
	if (ctl->temp->range) {
		ctl->temp->range_max = (uint16_t) (ctl->temp->range >> 16);
		ctl->temp->range_min = (uint16_t) ctl->temp->range;
	}

	ctl->light->last = constrain_lightness(ctl->light->last);
	ctl->light->def = constrain_lightness(ctl->light->def);
	ctl->light->target = constrain_lightness(ctl->light->target);

	ctl->temp->def = constrain_temperature(ctl->temp->def);
	ctl->temp->target = constrain_temperature(ctl->temp->target);

	ctl->temp->current = ctl->temp->def;
	ctl->duv->current = ctl->duv->def;

	switch (ctl->onpowerup) {
	case STATE_OFF:
		ctl->light->current = 0U;
		break;
	case STATE_DEFAULT:
		if (ctl->light->def == 0U) {
			ctl->light->current = ctl->light->last;
		} else {
			ctl->light->current = ctl->light->def;
		}
		break;
	case STATE_RESTORE:
		ctl->light->current = ctl->light->target;
		ctl->temp->current = ctl->temp->target;
		ctl->duv->current = ctl->duv->target;
		break;
	}

	ctl->light->target = ctl->light->current;
	ctl->temp->target = ctl->temp->current;
	ctl->duv->target = ctl->duv->current;
}

void update_vnd_led_gpio(void)
{
#ifndef ONE_LED_ONE_BUTTON_BOARD
  #if !MIGRATION_STEP4
	  gpio_pin_set(led_device[1], DT_GPIO_PIN(DT_ALIAS(led1), gpios),
		     vnd_user_data.current == STATE_ON);
  #endif
#endif
}

void update_led_gpio(void)
{
	uint8_t power, color;

	power = 100 * ((float) ctl->light->current / 65535);
	color = 100 * ((float) (ctl->temp->current - ctl->temp->range_min) /
		       (ctl->temp->range_max - ctl->temp->range_min));

  #if MIGRATION_STEP2
	  LOG(5,"power-> %d, color-> %d", power, color);
  #else
	  printk("power-> %d, color-> %d\n", power, color);
  #endif

  #if !MIGRATION_STEP4
  	gpio_pin_set(led_device[0], DT_GPIO_PIN(DT_ALIAS(led0), gpios),
		     ctl->light->current);
  #endif

#ifndef ONE_LED_ONE_BUTTON_BOARD
  #if !MIGRATION_STEP4
  	gpio_pin_set(led_device[2], DT_GPIO_PIN(DT_ALIAS(led2), gpios),
		     power < 50);
  #endif

  #if !MIGRATION_STEP4
  	gpio_pin_set(led_device[3], DT_GPIO_PIN(DT_ALIAS(led3), gpios),
		     color < 50);
  #endif
#endif
}

void update_light_state(void)
{
	update_led_gpio();

	if (ctl->transition->counter == 0 || reset == false) {
		reset = true;
		k_work_submit(&no_transition_work);
	}
}

#if !MIGRATION_STEP5             // explicite reset control by app
static void short_time_multireset_bt_mesh_unprovisioning(void)
{
	if (reset_counter >= 4U)
  {
		reset_counter = 0U;
    #if MIGRATION_STEP2
		  LOG(1,BL_M "BT Mesh reset");
    #else
		  printk("BT Mesh reset\n");
    #endif

	  bt_mesh_reset();
	}
  else
  {
    #if MIGRATION_STEP2
  		LOG(3,BL_M "reset counter -> %d", reset_counter);
    #else
		  printk("Reset Counter -> %d\n", reset_counter);
    #endif
		reset_counter++;
	}

	save_on_flash(RESET_COUNTER);
}
#endif // MIGRATION_STEP5
//==============================================================================
// unprovision node
//==============================================================================
#if MIGRATION_STEP5

  static int unprovision(BL_ob *o, int val) // uprovision mesh node
  {
		reset_counter = 0U;
		LOG(3,BL_B "unprovision node");
	  bt_mesh_reset();
    return 0;                               // OK
  }

#endif // MIGRATION_STEP5

//==============================================================================
// reset timer
//==============================================================================

static void reset_counter_timer_handler(struct k_timer *dummy)
{
	reset_counter = 0U;
	save_on_flash(RESET_COUNTER);
  #if MIGRATION_STEP2
  	LOG(3,BL_M "reset counter set to zero");
  #else
  	printk("Reset Counter set to Zero\n");
  #endif

  #if MIGRATION_STEP5
    BL_ob oo = {_RESET,_DUE_,0,NULL};
    bl_core(&oo,0);              // post [RESET:#DUE] to BL_CORE for output
  #endif
}

K_TIMER_DEFINE(reset_counter_timer, reset_counter_timer_handler, NULL);

//==============================================================================
// increment reset counter (set due timer, return reset counter after increment)
//==============================================================================
#if MIGRATION_STEP5

  static int increment(BL_ob *o, int ms)   // inc reset counter, set due timer
  {
		reset_counter++;
  	LOG(3,BL_M "reset counter: %d", reset_counter);

    k_timer_start(&reset_counter_timer, K_MSEC(ms<1000?1000:ms), K_NO_WAIT);

  	save_on_flash(RESET_COUNTER);
    return reset_counter;                   // return counter value after inc
  }

#endif // MIGRATION_STEP5
//==============================================================================
// init (fomer main())
//==============================================================================

#if MIGRATION_STEP1
static int init_mcore(void)
{
#else
void main(void)
{
#endif
	//int err;

	light_default_var_init();

  #if !MIGRATION_STEP5
	  app_gpio_init();
  #endif

	#if defined(CONFIG_MCUMGR)
		smp_svr_init();
	#endif

  #if MIGRATION_STEP2
	  LOG(3,BL_C "init BLE/Mesh ...");
  #else
	  printk("Initializing...\n");
  #endif

	ps_settings_init();

	  // Initialize the Bluetooth Subsystem */

  #if (CFG_BT_ENABLE)
  	int err = bt_enable(NULL);
  	if (err)
          {
      #if MIGRATION_STEP2
  		  LOG(ERR "Bluetooth init failed (err %d)", err);
      #else
  		  printk("Bluetooth init failed (err %d)\n", err);
      #endif
  		return err;
  	}
  #endif

  #if MIGRATION_STEP5
    blemesh_ready();
  #else
	  bt_ready();
  #endif

	light_default_status_init();

	update_light_state();

  #if !MIGRATION_STEP5
	  short_time_multireset_bt_mesh_unprovisioning();
	  k_timer_start(&reset_counter_timer, K_MSEC(7000), K_NO_WAIT);
  #endif

	#if defined(CONFIG_MCUMGR)
		/* Initialize the Bluetooth mcumgr transport. */
		smp_bt_register();

		k_timer_start(&smp_svr_timer, K_NO_WAIT, K_MSEC(1000));
	#endif
  #if MIGRATION_STEP1
    return 0;                          // OK
	#endif
}

#if MIGRATION_STEP1
//==============================================================================
// init bl_core modules
//==============================================================================

  static int init(BL_ob *o, int val)
  {
    LOG(2,BL_B"init Bluccino core");

    bl_init(blemesh,bl_core);          // output of BLEMESH goes to here!
    bl_init(bl_devcomp,bl_core);       // output of BL_DEVCMP goes to here!
    bl_init(bl_hw,bl_core);            // output of BL_HW goes to here!
    init_mcore();                      // init THIS module
    return 0;
  }

//==============================================================================
// public module interface
//==============================================================================
//
// BL_CORE Interfaces:
//   [] = SYS(INIT,TICK,TOCK)
//   [PRV,ATT] = SET(PRV,ATT)
//   [DUE] = RESET(#DUE,INC,PRV)
//   [] = BUTTON(INC,PRV)
//   [] = SWITCH(STS)
//                                +-------------+
//                                |   BL_CORE   |
//                                +-------------+
//                         INIT ->|     SYS:    |
//                         TICK ->|             |
//                         TOCK ->|             |
//                                +-------------+
//                          PRV ->|     SET:    |-> PRV
//                          ATT ->|             |-> ATT
//                                +-------------+
//                         #DUE ->|    RESET:   |-> DUE
//                          INC ->|             |
//                          PRV ->|             |
//                                +-------------+
//                                |   BUTTON:   |-> PRESS
//                                |             |-> RELEASE
//                                +-------------+
//                                |   SWITCH:   |-> STS
//                                +-------------+
//
//  Input Messages:
//    - [SYS:INIT <cb>]                init module
//    - [SYS:TICK @id cnt]             tick the module
//    - [SYS:TOCK @id cnt]             tock the module
//    - [SET:PRV val]                  provision on/off
//    - [SET:ATT val]                  attention on/off
//    - [RESET:#DUE]                   reset timer is due
//    - [RESET:INC <ms>]               inc reset counter & set due (<ms>) timer
//    - [RESET:PRV]                    unprovision node
//
//  Output Messages:
//    - [SET:PRV val]                  provision on/off
//    - [SET:ATT val]                  attention on/off
//    - [RESET:DUE]                    reset timer due notification
//    - [BUTTON:PRESS @id 1]           button press @ channel @id
//    - [BUTTON:RELEASE @id 1]         button release @ channel @id
//    - [SWITCH:STS @id,onoff]         output switch status update
//
//==============================================================================

  int bl_core(BL_ob *o, int val)
  {
    static BL_fct output = NULL;

    switch (BL_ID(o->cl,o->op))
    {
      case BL_ID(_SYS,INIT_):        // [SYS:INIT]
        output = o->data;            // store output callback
        return init(o,val);          // forward to init()

      case BL_ID(_SYS,TICK_):        // [SYS:TICK @0,cnt]
      case BL_ID(_SYS,TOCK_):        // [SYS:TICK @0,cnt]
        return 0;                    // OK - nothing to tick/tock

      case BL_ID(_SET,PRV_):         // [SET:PRV val]  (provision)
      case BL_ID(_SET,ATT_):         // [SET:ATT val]  (attention)
      case BL_ID(_BUTTON,PRESS_):    // [BUTTON:PRESS @id] (button pressed)
      case BL_ID(_BUTTON,RELEASE_):  // [BUTTON:RELEASE @id] (button release)
      case BL_ID(_SWITCH,STS_):      // [BUTTON:RELEASE @id] (button release)
        LOGO(3,"",o,val);
//return 0;
      return bl_out(o,val,output); // output to subscriber

      case BL_ID(_LED,SET_):         // [LED:SET @id,onoff]
      case BL_ID(_LED,TOGGLE_):      // [LED:SET @id,onoff]
        return bl_hw(o,val);         // delegate to MGPIO submodule

      case BL_ID(_RESET,INC_):       // cnt = [RESET:INC <ms>]
        return increment(o,val);     // delegate to increment()

      case BL_ID(_RESET,PRV_):       // [RESET:PRV]
        return unprovision(o,val);   // unprovision node

      case BL_ID(_RESET,_DUE_):      // [RESET:#DUE] reset timer is due
        return bl_out(o,val,output); // output [RESET:DUE] (strip off hash bit)

      case BL_ID(_GOOCLI,LET_):      // [GOOCLI:LET] publish unack'ed GOO msg
      case BL_ID(_GOOCLI,SET_):      // [GOOCLI:SET] publish ack'ed GOO msg
        return bl_pub(o,val);        // publish [GOOCLI:LET] or [GOOCLI:SET] msg

      case BL_ID(_GOOSRV,STS_):      // [GOOSRV:STS] post GOO status msg upward
        return bl_out(o,val,output); // publish [GOOCLI:LET] or [GOOCLI:SET] msg

      default:
        return -1;                   // bad input
    }
  }

#endif // MIGRATION_STEP1
