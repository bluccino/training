//==============================================================================
//  bl_model.c
//  common mesh model definitions
//
//  Created by Hugo Pristauz on 19.02.2022
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//==============================================================================

#ifndef __BL_MODEL_H__
#define __BL_MODEL_H__

    #include "bl_type.h"

//==============================================================================
//  BNX Vendor Mesh model IDs
//==============================================================================

    #define BL_MID_VSIMP_SRV        0x8000    // zlvsimp.h

    #define BL_MID_VMONI_SRV        0x8001    // zlvmoni.h
    #define BL_MID_VMONI_CLI        0x8002    // zlvmoni.h

//==============================================================================
// Zephyr vendor model (fronm zephyr 'multi' sample)
//==============================================================================

    #define BL_OC_VSIMP_GET  BT_MESH_MODEL_OP_3(0x01, BL_BNX_CID) // <07c6.00c1>
    #define BL_OC_VSIMP_SET  BT_MESH_MODEL_OP_3(0x02, BL_BNX_CID) // <07c6.00c2>
    #define BL_OC_VSIMP_LET  BT_MESH_MODEL_OP_3(0x03, BL_BNX_CID) // <07c6.00c3>
    #define BL_OC_VSIMP_STS  BT_MESH_MODEL_OP_3(0x04, BL_BNX_CID) // <07c6.00c4>

//==============================================================================
// BNX monitoring vendor model
//==============================================================================

    #define BL_OC_VMONI_GET  BT_MESH_MODEL_OP_3(0x05, BL_BNX_CID) // <07c6.00c5>
    #define BL_OC_VMONI_STS  BT_MESH_MODEL_OP_3(0x06, BL_BNX_CID) // <07c6.00c6>

//==============================================================================
// transition type
//==============================================================================

		typedef enum BL_ttype   // level transition type
		{
				LEVEL_TT,
				LEVEL_TT_DELTA,
				LEVEL_TT_MOVE,

				LEVEL_TEMP_TT,
				LEVEL_TEMP_TT_DELTA,
				LEVEL_TEMP_TT_MOVE,
		} BL_ttype;

		typedef struct BL_trans    // transition
	  {
				bool just_started;
				uint8_t tt;
				uint8_t rt;
				uint8_t delay;
				uint32_t quo_tt;
				uint32_t counter;
				uint32_t total_duration;
				int64_t start_timestamp;

				struct k_timer timer;
		} BL_trans;

//==============================================================================
// handling de-ja-vu's
//==============================================================================

    bool bl_is_dejavu(BL_dejavu *pd, BL_ctx *ctx, uint8_t tid); // TID seen before?
    void bl_dejavu(BL_dejavu *pd, BL_ctx *ctx, uint8_t tid);    // update de-ja-vu

#endif // __BL_MODEL_H__
