/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * Copyright (c) 2018 Vikrant More
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _STORAGE_H
#define _STORAGE_H

enum ps_variables_id {
	RESET_COUNTER = 0x01,
	GEN_DEF_TRANS_TIME_STATE,
	GEN_ONPOWERUP_STATE,

	DEF_STATES,

	LIGHTNESS_LAST_STATE,

	LAST_TARGET_STATES,

	LIGHTNESS_RANGE,
	TEMPERATURE_RANGE,

  NVM_CACHE,               // added
};

extern uint8_t reset_counter;

extern struct k_work storage_work;

//int ps_settings_init(void);
void save_on_flash(uint8_t id);

//==============================================================================
// public module interface
//==============================================================================
//
// (C) := (BL_CORE)
//                  +--------------------+
//                  |       BL_NVM       | non volatile memory access
//                  +--------------------+
//                  |        SYS:        | SYS: public interface
// (C)->     INIT ->|       @id,cnt      | init module, store <out> callback
// (C)->     TOCK ->|       @id,cnt      | tock the module
//                  +--------------------+
//                  |        NVM:        | NVM: public interface
// (C)<-    READY <-|       ready        | notification that NVM is now ready
// (C)->    STORE ->|      @id,val       | store value in NVM at location @id
// (C)->   RECALL ->|        @id         | recall value in NVM at location @id
// (C)->     SAVE ->|                    | save NVM cache to NVM
//                  +--------------------+
//
//==============================================================================

  int bl_nvm(BL_ob *o, int val);

#endif
