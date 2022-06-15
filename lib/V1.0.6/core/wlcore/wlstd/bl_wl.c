//==============================================================================
// bl_wl.c
// multi model mesh demo based wireless (mesh) core
//
// Created by Hugo Pristauz on 2022-Jan-02
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================
// mcore derived from:
// Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
// Copyright (c) 2018 Vikrant More
// SPDX-License-Identifier: Apache-2.0
//==============================================================================

//==============================================================================
// include all C-sources of module BL_WL
//==============================================================================

  #include "bl_mesh.c"
  #undef LOG
  #undef LOGO
  #undef LOG0

  #include "ble_mesh.c"
  #undef LOG
  #undef LOGO

  #include "bl_dcomp.c"
  #undef LOG
  #undef LOGO

  #include "notrans.c"
  #undef LOG
  #undef LOGO

  #include "publisher.c"
  #undef LOG
  #undef LOGO
  #undef LOG0
  #undef ONOFF

  #include "state_binding.c"
  #undef LOG
  #undef LOGO

  #define sys_init storage_sys_init
  #include "storage.c"
  #undef LOG
  #undef LOGO
  #undef LOG0
  #undef sys_init

  #include "transition.c"
  #undef LOG
  #undef LOGO

  #include "bl_trans.c"
  #undef LOG
  #undef LOGO

//==============================================================================
// start actual BL_WL module
//==============================================================================

  #include <drivers/gpio.h>

  #include "bluccino.h"
  #include "bl_core.h"
  #include "bl_wl.h"
  #include "bl_gonoff.h"

  #include "ble_mesh.h"
  #include "bl_dcomp.h"
  #include "notrans.h"
  #include "state_binding.h"
  #include "storage.h"
  #include "transition.h"
  #include "publisher.h"

//==============================================================================
// CORE level logging shorthands
//==============================================================================

  #define WHO                     "bl_core:"

  #define LOG                     LOG_CORE
  #define LOGO(lvl,col,o,val)     LOGO_CORE(lvl,col WHO,o,val)
  #define LOG0(lvl,col,o,val)     LOGO_CORE(lvl,col,o,val)

//==============================================================================
// overview code migration from original onoff_app sample to a bluccino core
//==============================================================================

#ifndef MIGRATION_STEP1
  #define MIGRATION_STEP1         1    // TODO introduce bl_core()
#endif
#ifndef MIGRATION_STEP2
  #define MIGRATION_STEP2         1    // TODO introduce bl_core()
#endif

//==============================================================================
// let's go ...
//==============================================================================

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
/*
	gpio_pin_set(led_device[1], DT_GPIO_PIN(DT_ALIAS(led1), gpios),
		     vnd_user_data.current == STATE_ON);
*/
#endif
}

void update_led_gpio(void)
{
/*
	uint8_t power, color;

	power = 100 * ((float) ctl->light->current / 65535);
	color = 100 * ((float) (ctl->temp->current - ctl->temp->range_min) /
		       (ctl->temp->range_max - ctl->temp->range_min));

  #if MIGRATION_STEP2
	  LOG(3,BL_G "power-> %d, color-> %d", power, color);
  #else
	  printk("power-> %d, color-> %d\n", power, color);
  #endif

	gpio_pin_set(led_device[0], DT_GPIO_PIN(DT_ALIAS(led0), gpios),
		     ctl->light->current);
#ifndef ONE_LED_ONE_BUTTON_BOARD
	gpio_pin_set(led_device[2], DT_GPIO_PIN(DT_ALIAS(led2), gpios),
		     power < 50);
	gpio_pin_set(led_device[3], DT_GPIO_PIN(DT_ALIAS(led3), gpios),
		     color < 50);
#endif
*/
}

void update_light_state(void)
{
	update_led_gpio();

	if (ctl->transition->counter == 0 || reset == false) {
		reset = true;
		k_work_submit(&no_transition_work);
	}
}

//==============================================================================
// unprovision node
//==============================================================================

  static int unprovision(BL_ob *o, int val) // uprovision mesh node
  {
    reset_counter = 0U;
    LOG(3,BL_B "unprovision node");
    bt_mesh_reset();
    return 0;                               // OK
  }

//==============================================================================
// multi reset unprovisioning
//==============================================================================

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
    		LOG(1,BL_M "reset counter -> %d", reset_counter);
      #else
  		  printk("Reset Counter -> %d\n", reset_counter);
      #endif
  		reset_counter++;
  	}

  	save_on_flash(RESET_COUNTER);
  }

//==============================================================================
// reset timer
//==============================================================================

static void reset_counter_timer_handler(struct k_timer *dummy)
{
	reset_counter = 0U;
	save_on_flash(RESET_COUNTER);
	LOG(3,BL_M "reset counter set to zero");

	_bl_msg(bl_wl,_RESET,DUE_, 0,NULL,0); // (BL_WL)<-[#RESET:DUE]
}

K_TIMER_DEFINE(reset_counter_timer, reset_counter_timer_handler, NULL);

//==============================================================================
// increment reset counter (set due timer, return reset counter after increment)
//==============================================================================

  static int increment(BL_ob *o, int ms)   // inc reset counter, set due timer
  {
		reset_counter++;
  	LOG(3,BL_M "reset counter: %d", reset_counter);

    k_timer_start(&reset_counter_timer, K_MSEC(ms<1000?1000:ms), K_NO_WAIT);

  	save_on_flash(RESET_COUNTER);
    return reset_counter;                   // return counter value after inc
  }

//==============================================================================
// worker: system init (fomer main())
//==============================================================================

  static int sys_init(BL_ob *o, int val)
  {
    static BL_oval W = bl_wl;          // wireless core

    light_default_var_init();

    #if defined(CONFIG_MCUMGR)
      smp_svr_init();
    #endif

    bl_init((bl_storage),(W));         // init NVM, output => bl_wl()
    bl_init((ble_mesh),(W));           // output of BLE_MESH goes to here!

/*
    LOG(3,BL_C "init BLE/Mesh...");

      // init Bluetooth subsystem

    int err = bt_enable(NULL);
    if (err)
    {
      bl_err(err,"Bluetooth init failed");
      return err;
    }

    bt_ready();
*/
    light_default_status_init();

    update_light_state();

    short_time_multireset_bt_mesh_unprovisioning();
    k_timer_start(&reset_counter_timer, K_MSEC(7000), K_NO_WAIT);

    #if defined(CONFIG_MCUMGR)
      // init Bluetooth mcumgr transport
      smp_bt_register();

      k_timer_start(&smp_svr_timer, K_NO_WAIT, K_MSEC(1000));
    #endif
    return 0;                          // OK
  }

//==============================================================================
// public module interface
//==============================================================================
//
// (N) := (BL_NVM);  (!) := (<parent>); (O) := (<out>); (B) := (BLE_MESH);
//                  +--------------------+
//                  |        BL_WL       | wireless core
//                  +--------------------+
//                  |        SYS:        | SYS: public interface
// (!)->     INIT ->|       @id,cnt      | init module, store <out> callback
// (!)->     TICK ->|       @id,cnt      | tick the module
// (!)->     TOCK ->|       @id,cnt      | tock the module
//                  +--------------------+
//                  |       MESH:        | MESH upper interface
// (U)<-      PRV <-|       onoff        | provision on/off
// (U)<-      ATT <-|       onoff        | attention on/off
//                  |....................|
//                  |       MESH:        | MESH lower interface
// (B)->      PRV ->|       onoff        | provision on/off
// (B)->      ATT ->|       onoff        | attention on/off
//                  +--------------------+
//                  |       RESET:       | RESET: public interface
// (U)<-      DUE <-|                    | reset timer is due
// (!)->      INC ->|         ms         | inc reset counter & set due timer
// (!)->      PRV ->|                    | unprovision node
//                  +--------------------+
//                  |        NVM:        | NVM: upper interface
// (U)<-    READY <-|       ready        | notification that NVM is now ready
// (!)->    STORE ->|      @id,val       | store value in NVM at location @id
// (!)->   RECALL ->|        @id         | recall value in NVM at location @id
// (!)->     SAVE ->|                    | save NVM cache to NVM
//                  |....................|
//                  |        NVM:        | NVM: lower interface
// (N)->    READY ->|       ready        | notification that NVM is now ready
// (N)<-    STORE <-|      @id,val       | store value in NVM at location @id
// (N)<-   RECALL <-|        @id         | recall value in NVM at location @id
// (N)<-     SAVE <-|                    | save NVM cache to NVM
//                  +--------------------+
//                  |       #SET:        | SET: private interface
// (#)->      PRV ->|       onoff        | provision on/off
// (#)->      ATT ->|       onoff        | attention on/off
//                  +--------------------+
//                  |      #RESET:       | RESET: private interface
// (#)->      DUE ->|                    | reset timer is due
//                  +--------------------+
//                  |      GOOCLI:       | GOOCLI: input interface
// (!)->      SET ->| @id,<BL_goo>,onoff | acknowledged generic on/off set
// (!)->      LET ->| @id,<BL_goo>,onoff | unacknowledged generic on/off set
// (!)->      GET ->|        @id         | request generic on/off server status
//                  +--------------------+
//                  |      GOOSRV:       | GOOSRV: output interface
// (U)<-      STS <-|  @id,<BL_goo>,sts  | notify server status change
//                  +--------------------+
//
//==============================================================================

  int bl_wl(BL_ob *o, int val)
  {
    static BL_oval U = bl_up;
    static BL_oval S = bl_storage;

    switch (bl_id(o))
    {
      case SYS_INIT_0_cb_0:          // [SYS:INIT <out>]
        U = bl_cb(o,(U),WHO"(U)");   // store output callback
        LOG(2,BL_B "init Bluccino core");
        bl_init(bl_dcomp,bl_wl);     // output of BL_DEVCMP goes to here!
        return sys_init(o,val);      // forward to sys_init() worker

      case SYS_TICK_id_BL_pace_cnt:        // [SYS:TICK @0,cnt]
        return 0;                    // OK - nothing to tick/tock

      case SYS_TOCK_id_BL_pace_cnt:        // [SYS:TICK @0,cnt]
        return bl_fwd(o,val,(S));    // bl_storage module to be tocked

      case MESH_PRV_0_0_sts:         // (B)->[MESH:PRV sts]->(O)  (provision)
      case MESH_ATT_0_0_sts:         // (B)->[MESH:ATT sts]->(O)  (attention)
        LOGO(3,"",o,val);
        return bl_out(o,val,(U));    // output to subscriber

      case BL_ID(_RESET,INC_):       // cnt = [RESET:INC <ms>]
        return increment(o,val);     // delegate to increment()

      case BL_ID(_RESET,PRV_):       // [RESET:PRV]
        return unprovision(o,val);   // unprovision node

      case _BL_ID(_RESET,DUE_):      // (O)<-[#RESET:DUE] reset timer is due
        return bl_out(o,val,(U));    // output message (strip off aug bit)

      case GOOCLI_LET_id_BL_goo_onoff:
      case GOOCLI_SET_id_BL_goo_onoff:
        return bl_pub(o,val);        // publish [GOOCLI:LET] or [GOOCLI:SET] msg

      case GOOSRV_STS_id_BL_goo_sts:
        return bl_out(o,val,(U));    // notify [GOOSRV:STS]

      case NVM_READY_0_0_sts:        // [NVM:READY] notify that NVM is ready
        return bl_out(o,val,(U));    // output [NVM:READY] to subscriber

      case NVM_STORE_id_0_val:
      case NVM_RECALL_id_0_0:
      case NVM_SAVE_0_BL_dac_0:
      case NVM_LOAD_0_BL_dac_0:
      case NVM_AVAIL_0_0_0:
        return bl_fwd(o,val,(S));    // forward to bl_storage module

      default:
        return -1;                   // bad input
    }
  }
