//===================================================================================================
//  bl_gonoff.h
//  generic onoff model
//
//  Created by Hugo Pristauz on 20.02.2022
//  Copyright © 2022 Bluenetics GmbH. All rights reserved.
//===================================================================================================

#ifndef __BL_GONOFF_H__
#define __BL_GONOFF_H__

#include "bl_mesh.h"
#include "bl_type.h"

    #define STATE_OFF        0x00
    #define STATE_ON         0x01
    #define STATE_DEFAULT    0x01
    #define STATE_RESTORE    0x02

        // mesh model ID and mesh model operation codes

    #define BL_GOO_CLI       BT_MESH_MODEL_ID_GEN_ONOFF_CLI
    #define BL_GOO_SRV       BT_MESH_MODEL_ID_GEN_ONOFF_SRV

    #define BL_GOO_GET       BT_MESH_MODEL_OP_2(0x82, 0x01)
    #define BL_GOO_SET       BT_MESH_MODEL_OP_2(0x82, 0x02)
    #define BL_GOO_LET       BT_MESH_MODEL_OP_2(0x82, 0x03)
    #define BL_GOO_STS       BT_MESH_MODEL_OP_2(0x82, 0x04)

    typedef struct BL_gonoff_set
    {
        uint8_t onoff;           // the target value of the generic OnOff state
        uint8_t tid;             // transaction identifier
        uint8_t tt;              // format as defined in section 3.1.3 (opt.)
        uint8_t delay;           // msg execution delay in 5 ms steps (C.1)
    } BL_gonoff_set;

    typedef struct BL_gonoff_status
    {
        uint8_t present;         // present value of generic OnOff state.
        uint8_t target;          // target value of generic OnOff state (opt.)
        uint8_t remain;          // format as defined in section 3.1.3 (C.1)
    } BL_gonoff_status;


    #define BL_OFF   ((uint8_t)0)   // symbolic value for OFF
    #define BL_ON    ((uint8_t)1)   // symbolic value for ON

    typedef struct BL_gonoff_state
    {
        uint8_t onoff;
        uint8_t target_onoff;

        uint8_t last_tid;
        uint16_t last_src_addr;
        uint16_t last_dst_addr;
        int64_t last_msg_timestamp;

        int32_t tt_delta;

        BL_trans *transition;
    } BL_gonoff_state;

//==============================================================================
// dynamic onoff model generation
//==============================================================================
/*
    extern uint8_t            zl_tid_onoff;          // TID for gonoff messages
    extern BL_gonoff_state zl_gonoff_state0;  // onoff state for GONOFF_CLI0

    BL_model *zl_gonoff_srv_add(BL_element *pel, BL_pub *ppub, BL_gonoff_state *pud);
    BL_model *zl_gonoff_cli_add(BL_element *pel, BL_pub *ppub, BL_gonoff_state *pud);
*/
//==============================================================================
// opcode map table
//==============================================================================

    extern const BL_ocmap zl_gonoff_srv_op[];
    extern const BL_ocmap zl_gonoff_cli_op[];

//==============================================================================
// publish generic onoff state
//==============================================================================

    void zl_tx_gonoff(BL_model *model);

//==============================================================================
// send generic onoff message
//==============================================================================

    int zl_tx_gonoff_set(BL_model *pmod, uint8_t on, uint8_t tid, uint8_t tt, uint8_t d);
    int zl_tx_gonoff_let(BL_model *pmod, uint8_t on, uint8_t tid, uint8_t tt, uint8_t d);
    int zl_tx_gonoff_get(BL_model *pmod);

#endif // __BL_GONOFF_H__
