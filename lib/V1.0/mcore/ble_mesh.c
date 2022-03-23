//==============================================================================
// ble_mesh.h
// multi model mesh demo based mesh core
//
// Adopted to Bluccino by Hugo Pristauz on 2022-Jan-07
// Copyright © 2022 Bluccino. All rights reserved.
//==============================================================================
// Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
// Copyright (c) 2018 Vikrant More
// SPDX-License-Identifier: Apache-2.0
//==============================================================================

#include "ble_mesh.h"
#include "device_composition.h"

//==============================================================================
// CORE level logging shorthands
//==============================================================================

  #define LOG                     LOG_CORE
  #define LOGO(lvl,col,o,val)     LOGO_CORE(lvl,col"mcore:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_CORE(lvl,col,o,val)
  #define ERR 1,BL_R

//==============================================================================
// overview code migration from original onoff_app sample to a bluccino core
//==============================================================================

#include "bluccino.h"

#ifndef MIGRATION_STEP1
  #define MIGRATION_STEP1         0    // TODO introduce bl_core()
#endif
#ifndef MIGRATION_STEP2
  #define MIGRATION_STEP2         0    // TODO introduce bl_core()
#endif

//==============================================================================
// let's go ...
//==============================================================================

#ifdef OOB_AUTH_ENABLE

static int output_number(bt_mesh_output_action_t action, uint32_t number)
{
  #if MIGRATION_STEP2
   	LOG(1,BL_M "OOB Number: %u", number);
	#else
   	printk("OOB Number: %u\n", number);
	#endif
	return 0;
}

static int output_string(const char *str)
{
  #if MIGRATION_STEP2
  	LOG(1,BL_M "OOB String: %s", str);
	#else
  	printk("OOB String: %s\n", str);
	#endif
	return 0;
}

#endif

static void prov_complete(uint16_t net_idx, uint16_t addr)
{
  #if MIGRATION_STEP2   // pimped in MIGRATION_STEP5
    BL_ob oo = {_SET,PRV_,0,NULL};
    blemesh(&oo,1);         // post [SET:PRV 1] to pub ifc, which posts it up
  #endif // MIGRATION_STEP2
}

static void prov_reset(void)
{
	bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);
  #if MIGRATION_STEP2  // pimped in MIGRATION_STEP5
    BL_ob oo = {_SET,PRV_,0,NULL};
    blemesh(&oo,0);         // post [SET:PRV 0] to pub ifc, which posts it up
  #endif // MIGRATION_STEP2
}

#if MIGRATION_STEP2  // pimped in MIGRATION_STEP5
//==============================================================================
// provisioning link open/close callbacks
//==============================================================================

static void link_open(bt_mesh_prov_bearer_t bearer)
{
  BL_ob oo = {_SET,ATT_, 1, NULL};
  blemesh(&oo,1);             // post [SET:ATT 1] to pub ifc, which posts it up
}

static void link_close(bt_mesh_prov_bearer_t bearer)
{
  BL_ob oo = {_SET,ATT_, 0, NULL};
  blemesh(&oo,0);            // post [MESH:ATT 0] pub ifc, which posts it up
}

//==============================================================================
// provisioning table
//==============================================================================
#endif // MIGRATION_STEP2

static uint8_t dev_uuid[16] = { 0xdd, 0xdd };

static const struct bt_mesh_prov prov = {
	.uuid = dev_uuid,

#ifdef OOB_AUTH_ENABLE

	.output_size = 6,
	.output_actions = BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING,
	.output_number = output_number,
	.output_string = output_string,

#endif
#if MIGRATION_STEP2
  .link_open = link_open,              // to activate attention mode
  .link_close = link_close,            // to deactivate attention mode
#endif // MIGRATION_STEP2
	.complete = prov_complete,
	.reset = prov_reset,
};

#if MIGRATION_STEP5
static void bt_ready(void)
#else
void bt_ready(void)
#endif
{
	int err;
	struct bt_le_oob oob;

  #if MIGRATION_STEP2
  	LOG(4,BL_B "Bluetooth initialized");
	#else
  	printk("Bluetooth initialized\n");
  #endif

	err = bt_mesh_init(&prov, &comp);
	if (err)
	{
    #if MIGRATION_STEP2
	  	LOG(ERR "Initializing mesh failed (err %d)", err);
		#else
	  	printk("Initializing mesh failed (err %d)\n", err);
		#endif
		return;
	}

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	/* Use identity address as device UUID */
	if (bt_le_oob_get_local(BT_ID_DEFAULT, &oob))
	{
          #if MIGRATION_STEP2
  		LOG(1,BL_R "Identity Address unavailable");
	  #else
  		printk("Identity Address unavailable\n");
	  #endif
	} else {
		memcpy(dev_uuid, oob.addr.a.val, 6);
	}

	bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);

	#if MIGRATION_STEP2
    LOG(4,BL_B"Mesh initialized");
	#else
    printk("Mesh initialized\n");
	#endif
}

//==============================================================================
// public module interface
//==============================================================================
//
// BLEMESH Interfaces:
//   SYS interface: [] = SYS(INIT,READY)
//   SET interface: [PRV,ATT] = SET(#PRV,#ATT)
//
//                          +---------------+
//                          |    BLEMESH    |
//                          +---------------+
//                   INIT ->|               |
//                          |      SYS      |
//                  READY ->|               |
//                          +---------------+
//                   #PRV ->|               |-> PRV
//                          |      SET      |
//                   #ATT ->|               |-> ATT
//                          +---------------+
//
//  Input Messages:
//    - [SYS:INIT <cb>]     init module
//    - [SYS:READY]         init BLE/Mesh when Bluetooth is ready
//    - [SET:#PRV val]      post provisioning on/off to output
//    - [SET:#ATT val]      post attentioning on/off to output
//
//  Output Messages:
//    - [SET:PRV val]       provisioning on/off
//    - [SET:ATT val]       attentioning on/off
//
//==============================================================================

  int blemesh(BL_ob *o, int val)
  {
    static BL_fct output = NULL;       // output callback

    switch (bl_id(o))
    {
      case BL_ID(_SYS,INIT_):          // [SYS:INIT <cb>] init sub module
        output = o->data;              // store output cb
        return 0;                      // OK

      case BL_ID(_SYS,READY_):         // [SYS:READY] BLE/MESH init
        bt_ready();                    // init BLE/Mesh when Bluetooth is ready
        return 0;                      // OK

      case BL_ID(_SET,ATT_):           // [HDL:ATT] handle attention state
      case BL_ID(_SET,PRV_):           // [HDL:PRV] handle provision state
        bl_out(o,val,output);          // output message to subscriber
        return 0;                      // OK

      default:
        return -1;                     // bad args
    }
  }
