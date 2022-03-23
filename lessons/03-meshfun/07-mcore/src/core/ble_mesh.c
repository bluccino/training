/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * Copyright (c) 2018 Vikrant More
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
  #define MIGRATION_STEP1         1    // TODO introduce bl_core()
#endif
#ifndef MIGRATION_STEP2
  #define MIGRATION_STEP2         1    // TODO introduce bl_core()
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
  #if MIGRATION_STEP2
	  BL_ob o = {_MESH,PRV_,0,NULL};
	  bl_core(&o,1);            // post [MESH:PRV 1] to core, which posts it up
  #endif // MIGRATION_STEP2
}

static void prov_reset(void)
{
	bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);
  #if MIGRATION_STEP2
	  BL_ob o = {_MESH,PRV_,0,NULL};
	  bl_core(&o,0);            // post [MESH:PRV 0] to core, which posts it up
  #endif // MIGRATION_STEP2
}

//==============================================================================
// provisioning link open/close callbacks
//==============================================================================

  static void link_open(bt_mesh_prov_bearer_t bearer)
  {
  	BL_ob o = {_MESH, ATT_, 1, NULL};
  	bl_core(&o,1);              // post [MESH:ATT 1] to core, which posts it up
  }

  static void link_close(bt_mesh_prov_bearer_t bearer)
  {
  	BL_ob o = {_MESH, ATT_, 0, NULL};
  	bl_core(&o,0);              // post [MESH:ATT 0] to core, which posts it up
  }

//==============================================================================
// provisioning table
//==============================================================================

  static uint8_t dev_uuid[16] = { 0xdd, 0xdd };

  static const struct bt_mesh_prov prov = {
  	.uuid = dev_uuid,

  #ifdef OOB_AUTH_ENABLE

  	.output_size = 6,
  	.output_actions = BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING,
  	.output_number = output_number,
  	.output_string = output_string,

  #endif
    .link_open = link_open,              // to activate attention mode
    .link_close = link_close,            // to deactivate attention mode
  	.complete = prov_complete,
  	.reset = prov_reset,
  };

  void bt_ready(void)
  {
  	int err;
  	struct bt_le_oob oob;

  	LOG(4,BL_B "Bluetooth initialized");

  	err = bt_mesh_init(&prov, &comp);
  	if (err)
  	{
  	  bl_err(err,"initializing mesh failed");
  		return;
  	}

  	if (IS_ENABLED(CONFIG_SETTINGS)) {
  		settings_load();
  	}

  	  // use identity address as device UUID

  	if (bt_le_oob_get_local(BT_ID_DEFAULT, &oob))
    	LOG(1,BL_R "Identity Address unavailable");
  	else
  		memcpy(dev_uuid, oob.addr.a.val, 6);

  	bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    LOG(4,BL_B"mesh initialized");
  }
