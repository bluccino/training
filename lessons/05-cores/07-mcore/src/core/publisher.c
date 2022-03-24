/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * Copyright (c) 2018 Vikrant More
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <drivers/gpio.h>

#include "ble_mesh.h"
#include "device_composition.h"
#include "publisher.h"

#include "bluccino.h"
#include "bl_mesh.h"
#include "bl_gonoff.h"

//==============================================================================
// CORE level logging shorthands
//==============================================================================

  #define LOG                     LOG_CORE
  #define LOGO(lvl,col,o,val)     LOGO_CORE(lvl,col"publisher:",o,val)
  #define LOG0(lvl,col,o,val)     LOGO_CORE(lvl,col,o,val)

//==============================================================================
// Let's go
//==============================================================================

#define ONOFF
#define GENERIC_LEVEL

static uint8_t tid;

//==============================================================================
// legacy publisher
//==============================================================================
#ifdef LEGACY_PUBLISHER // never

void publish(struct k_work *work)
{
	int err = 0;
LOG(4,"publish ...");
#ifndef ONE_LED_ONE_BUTTON_BOARD

    // SW0 event

	if (gpio_pin_get(button_device[0],
			 DT_GPIO_PIN(DT_ALIAS(sw0), gpios)) == 1) {
#if defined(ONOFF)
		bt_mesh_model_msg_init(root_models[3].pub->msg,
				       BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK);
		net_buf_simple_add_u8(root_models[3].pub->msg, 0x01);
		net_buf_simple_add_u8(root_models[3].pub->msg, tid++);
LOG(4,BL_R"snd [GOOCLI:LET @1,1]");
		err = bt_mesh_model_publish(&root_models[3]);
#elif defined(ONOFF_TT)
		bt_mesh_model_msg_init(root_models[3].pub->msg,
				       BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK);
		net_buf_simple_add_u8(root_models[3].pub->msg, 0x01);
		net_buf_simple_add_u8(root_models[3].pub->msg, tid++);
		net_buf_simple_add_u8(root_models[3].pub->msg, 0x45);
		net_buf_simple_add_u8(root_models[3].pub->msg, 0x28);
		err = bt_mesh_model_publish(&root_models[3]);
#elif defined(VND_MODEL_TEST)
		bt_mesh_model_msg_init(vnd_models[0].pub->msg,
				       BT_MESH_MODEL_OP_3(0x03, CID_ZEPHYR));
		net_buf_simple_add_le16(vnd_models[0].pub->msg, 0x0001);
		net_buf_simple_add_u8(vnd_models[0].pub->msg, tid++);
		err = bt_mesh_model_publish(&vnd_models[0]);
#endif
	}

    // SW1 event

  else if (gpio_pin_get(button_device[1],
				DT_GPIO_PIN(DT_ALIAS(sw1), gpios)) == 1)
  {
#if defined(ONOFF)
		bt_mesh_model_msg_init(root_models[3].pub->msg,
				       BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK);
		net_buf_simple_add_u8(root_models[3].pub->msg, 0x00);
		net_buf_simple_add_u8(root_models[3].pub->msg, tid++);
LOG(4,BL_R"snd [GOOCLI:LET @1,0]");
		err = bt_mesh_model_publish(&root_models[3]);
#elif defined(ONOFF_TT)
		bt_mesh_model_msg_init(root_models[3].pub->msg,
				       BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK);
		net_buf_simple_add_u8(root_models[3].pub->msg, 0x00);
		net_buf_simple_add_u8(root_models[3].pub->msg, tid++);
		net_buf_simple_add_u8(root_models[3].pub->msg, 0x45);
		net_buf_simple_add_u8(root_models[3].pub->msg, 0x28);
		err = bt_mesh_model_publish(&root_models[3]);
#elif defined(VND_MODEL_TEST)
		bt_mesh_model_msg_init(vnd_models[0].pub->msg,
				       BT_MESH_MODEL_OP_3(0x03, CID_ZEPHYR));
		net_buf_simple_add_le16(vnd_models[0].pub->msg, 0x0000);
		net_buf_simple_add_u8(vnd_models[0].pub->msg, tid++);
		err = bt_mesh_model_publish(&vnd_models[0]);
#endif
	} else if (gpio_pin_get(button_device[2],
				DT_GPIO_PIN(DT_ALIAS(sw2), gpios)) == 1) {
#if defined(GENERIC_LEVEL)
		bt_mesh_model_msg_init(root_models[5].pub->msg,
				       BT_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK);
		net_buf_simple_add_le16(root_models[5].pub->msg, LEVEL_S25);
		net_buf_simple_add_u8(root_models[5].pub->msg, tid++);
		err = bt_mesh_model_publish(&root_models[5]);
#elif defined(ONOFF_GET)
		bt_mesh_model_msg_init(root_models[3].pub->msg,
				       BT_MESH_MODEL_OP_GEN_ONOFF_GET);
		err = bt_mesh_model_publish(&root_models[3]);
#elif defined(GENERIC_DELTA_LEVEL)
		bt_mesh_model_msg_init(root_models[5].pub->msg,
				       BT_MESH_MODEL_OP_GEN_DELTA_SET_UNACK);
		net_buf_simple_add_le32(root_models[5].pub->msg, 100);
		net_buf_simple_add_u8(root_models[5].pub->msg, tid++);
		err = bt_mesh_model_publish(&root_models[5]);
#elif defined(GENERIC_MOVE_LEVEL_TT)
		bt_mesh_model_msg_init(root_models[5].pub->msg,
				       BT_MESH_MODEL_OP_GEN_MOVE_SET_UNACK);
		net_buf_simple_add_le16(root_models[5].pub->msg, 655);
		net_buf_simple_add_u8(root_models[5].pub->msg, tid++);
		net_buf_simple_add_u8(root_models[5].pub->msg, 0x41);
		net_buf_simple_add_u8(root_models[5].pub->msg, 0x00);
		err = bt_mesh_model_publish(&root_models[5]);
#elif defined(LIGHT_LIGHTNESS_TT)
		bt_mesh_model_msg_init(root_models[13].pub->msg,
				       BT_MESH_MODEL_LIGHT_LIGHTNESS_SET_UNACK);
		net_buf_simple_add_le16(root_models[13].pub->msg, LEVEL_U25);
		net_buf_simple_add_u8(root_models[13].pub->msg, tid++);
		net_buf_simple_add_u8(root_models[13].pub->msg, 0x45);
		net_buf_simple_add_u8(root_models[13].pub->msg, 0x28);
		err = bt_mesh_model_publish(&root_models[13]);
#elif defined(LIGHT_CTL)
		bt_mesh_model_msg_init(root_models[16].pub->msg,
				       BT_MESH_MODEL_LIGHT_CTL_SET_UNACK);
		/* Lightness */
		net_buf_simple_add_le16(root_models[16].pub->msg, LEVEL_U25);
		/* Temperature (value should be from 0x0320 to 0x4E20 */
		/* This is as per 6.1.3.1 in Mesh Model Specification */
		net_buf_simple_add_le16(root_models[16].pub->msg, 0x0320);
		/* Delta UV */
		net_buf_simple_add_le16(root_models[16].pub->msg, 0x0000);
		net_buf_simple_add_u8(root_models[16].pub->msg, tid++);
		err = bt_mesh_model_publish(&root_models[16]);
#elif defined(LIGHT_CTL_TT)
		bt_mesh_model_msg_init(root_models[16].pub->msg,
				       BT_MESH_MODEL_LIGHT_CTL_SET_UNACK);
		/* Lightness */
		net_buf_simple_add_le16(root_models[16].pub->msg, LEVEL_U25);
		/* Temperature (value should be from 0x0320 to 0x4E20 */
		/* This is as per 6.1.3.1 in Mesh Model Specification */
		net_buf_simple_add_le16(root_models[16].pub->msg, 0x0320);
		/* Delta UV */
		net_buf_simple_add_le16(root_models[16].pub->msg, 0x0000);
		net_buf_simple_add_u8(root_models[16].pub->msg, tid++);
		net_buf_simple_add_u8(root_models[16].pub->msg, 0x45);
		net_buf_simple_add_u8(root_models[16].pub->msg, 0x00);
		err = bt_mesh_model_publish(&root_models[16]);
#elif defined(LIGHT_CTL_TEMP)
		bt_mesh_model_msg_init(root_models[16].pub->msg,
				       BT_MESH_MODEL_LIGHT_CTL_TEMP_SET_UNACK);
		/* Temperature (value should be from 0x0320 to 0x4E20 */
		/* This is as per 6.1.3.1 in Mesh Model Specification */
		net_buf_simple_add_le16(root_models[16].pub->msg, 0x0320);
		/* Delta UV */
		net_buf_simple_add_le16(root_models[16].pub->msg, 0x0000);
		net_buf_simple_add_u8(root_models[16].pub->msg, tid++);
		err = bt_mesh_model_publish(&root_models[16]);
#endif
	} else if (gpio_pin_get(button_device[3],
				DT_GPIO_PIN(DT_ALIAS(sw3), gpios)) == 1) {
#if defined(GENERIC_LEVEL)
		bt_mesh_model_msg_init(root_models[5].pub->msg,
				       BT_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK);
		net_buf_simple_add_le16(root_models[5].pub->msg, LEVEL_S100);
		net_buf_simple_add_u8(root_models[5].pub->msg, tid++);
		err = bt_mesh_model_publish(&root_models[5]);
#elif defined(GENERIC_DELTA_LEVEL)
		bt_mesh_model_msg_init(root_models[5].pub->msg,
				       BT_MESH_MODEL_OP_GEN_DELTA_SET_UNACK);
		net_buf_simple_add_le32(root_models[5].pub->msg, -100);
		net_buf_simple_add_u8(root_models[5].pub->msg, tid++);
		err = bt_mesh_model_publish(&root_models[5]);
#elif defined(GENERIC_MOVE_LEVEL_TT)
		bt_mesh_model_msg_init(root_models[5].pub->msg,
				       BT_MESH_MODEL_OP_GEN_MOVE_SET_UNACK);
		net_buf_simple_add_le16(root_models[5].pub->msg, -655);
		net_buf_simple_add_u8(root_models[5].pub->msg, tid++);
		net_buf_simple_add_u8(root_models[5].pub->msg, 0x41);
		net_buf_simple_add_u8(root_models[5].pub->msg, 0x00);
		err = bt_mesh_model_publish(&root_models[5]);
#elif defined(LIGHT_LIGHTNESS_TT)
		bt_mesh_model_msg_init(root_models[13].pub->msg,
				       BT_MESH_MODEL_LIGHT_LIGHTNESS_SET_UNACK);
		net_buf_simple_add_le16(root_models[13].pub->msg, LEVEL_U100);
		net_buf_simple_add_u8(root_models[13].pub->msg, tid++);
		net_buf_simple_add_u8(root_models[13].pub->msg, 0x45);
		net_buf_simple_add_u8(root_models[13].pub->msg, 0x28);
		err = bt_mesh_model_publish(&root_models[13]);
#elif defined(LIGHT_CTL)
		bt_mesh_model_msg_init(root_models[16].pub->msg,
				       BT_MESH_MODEL_LIGHT_CTL_SET_UNACK);
		/* Lightness */
		net_buf_simple_add_le16(root_models[16].pub->msg, LEVEL_U100);
		/* Temperature (value should be from 0x0320 to 0x4E20 */
		/* This is as per 6.1.3.1 in Mesh Model Specification */
		net_buf_simple_add_le16(root_models[16].pub->msg, 0x4E20);
		/* Delta UV */
		net_buf_simple_add_le16(root_models[16].pub->msg, 0x0000);
		net_buf_simple_add_u8(root_models[16].pub->msg, tid++);
		err = bt_mesh_model_publish(&root_models[16]);
#elif defined(LIGHT_CTL_TT)
		bt_mesh_model_msg_init(root_models[16].pub->msg,
				       BT_MESH_MODEL_LIGHT_CTL_SET_UNACK);
		/* Lightness */
		net_buf_simple_add_le16(root_models[16].pub->msg, LEVEL_U100);
		/* Temperature (value should be from 0x0320 to 0x4E20 */
		/* This is as per 6.1.3.1 in Mesh Model Specification */
		net_buf_simple_add_le16(root_models[16].pub->msg, 0x4E20);
		/* Delta UV */
		net_buf_simple_add_le16(root_models[16].pub->msg, 0x0000);
		net_buf_simple_add_u8(root_models[16].pub->msg, tid++);
		net_buf_simple_add_u8(root_models[16].pub->msg, 0x45);
		net_buf_simple_add_u8(root_models[16].pub->msg, 0x00);
		err = bt_mesh_model_publish(&root_models[16]);
#elif defined(LIGHT_CTL_TEMP)
		bt_mesh_model_msg_init(root_models[16].pub->msg,
				       BT_MESH_MODEL_LIGHT_CTL_TEMP_SET_UNACK);
		/* Temperature (value should be from 0x0320 to 0x4E20 */
		/* This is as per 6.1.3.1 in Mesh Model Specification */
		net_buf_simple_add_le16(root_models[16].pub->msg, 0x4E20);
		/* Delta UV */
		net_buf_simple_add_le16(root_models[16].pub->msg, 0x0000);
		net_buf_simple_add_u8(root_models[16].pub->msg, tid++);
		err = bt_mesh_model_publish(&root_models[16]);
#endif
	}
#else
	if (gpio_pin_get(button_device[0],
			 DT_GPIO_PIN(DT_ALIAS(sw0), gpios)) == 1) {
#if defined(ONOFF)
		static uint8_t state = STATE_ON;

		bt_mesh_model_msg_init(root_models[3].pub->msg,
				       BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK);
		net_buf_simple_add_u8(root_models[3].pub->msg,
				      state = state ^ 0x01);
		net_buf_simple_add_u8(root_models[3].pub->msg, tid++);
		err = bt_mesh_model_publish(&root_models[3]);
#endif
	}
#endif

	if (err) {
		printk("bt_mesh_model_publish: err: %d\n", err);
	}
}

#endif // LEGACY_PUBLISHER
//==============================================================================
// transmit generic onoff set
//==============================================================================

  static int tx_gooset(BL_model *pmod, BL_u8 on, BL_u8 tid, BL_u8 tt, BL_u8 delay)
  {
    LOG(5,"tx_gooset($%d[%d|%d],%d,#%d,/%d,&%d)",
  	      bl_iid(pmod), pmod->elem_idx, pmod->mod_idx, on, tid,
  			  bl_mesh2ms(tt), bl_tick2ms(delay));

    bt_mesh_model_msg_init(pmod->pub->msg, BL_GOO_SET);

    net_buf_simple_add_u8(pmod->pub->msg, on);
    net_buf_simple_add_u8(pmod->pub->msg, tid);
    net_buf_simple_add_u8(pmod->pub->msg, tt);
    net_buf_simple_add_u8(pmod->pub->msg, delay);

    return bt_mesh_model_publish(pmod);
  }

//==============================================================================
// transmit generic onoff let
//==============================================================================

  static int tx_goolet(BL_model *pmod, BL_u8 on, BL_u8 tid, BL_u8 tt, BL_u8 delay)
  {
    LOG(5,"tx_goolet($%d[%d|%d], %d,#%d,/%d,&%d)",
  	      bl_iid(pmod), pmod->elem_idx, pmod->mod_idx, on, tid,
  			  bl_mesh2ms(tt), bl_tick2ms(delay));

    bt_mesh_model_msg_init(pmod->pub->msg, BL_GOO_LET);

    net_buf_simple_add_u8(pmod->pub->msg, on);
    net_buf_simple_add_u8(pmod->pub->msg, tid);
    net_buf_simple_add_u8(pmod->pub->msg, tt);
    net_buf_simple_add_u8(pmod->pub->msg, delay);

    return bt_mesh_model_publish(pmod);
  }

//==============================================================================
// GOOCLI publisher
//==============================================================================

  static int goocli_pub(BL_ob *o, int val)
  {
    static BL_iid goocli[4] = {GONOFF_CLI0,GONOFF_CLI0,GONOFF_CLI0,GONOFF_CLI0};

    LOG0(4,BL_R "pub",o,val);
    bl_assert(o->id > 0 && o->id <= 4);

    BL_model *pmod = bl_model(goocli[o->id-1]);
    bool onoff = (val != 0);
    static BL_u8 tid = 0;           // must be static
    BL_u8 tt = 0;
    BL_u8 delay = 0;

    switch (bl_id(o))
    {
      case BL_ID(_GOOCLI,LET_):
        tx_goolet(pmod, onoff, tid++, tt, delay);
        return 0;

      case BL_ID(_GOOCLI,SET_):
        tx_gooset(pmod, onoff, tid++, tt, delay);
        return 0;                      // OK

      default:
        return -1;                     // bad arg
    }
  }

//==============================================================================
// new publisher
//==============================================================================

  int bl_pub(BL_ob *o, int val)
  {
    if (o->id < 1 || o->id > 4)
      return -1;                       // bad args

    switch (bl_id(o))
    {
      case BL_ID(_GOOCLI,LET_):
      case BL_ID(_GOOCLI,SET_):
        return goocli_pub(o,val);      // publisg generic onoff LET or SET

      default:
        return -1;                     // not supported
    }
  }
