/******************************************************************************

@file  sig_models.h

@brief  Generic models defines and APIs

Group: WCS, BTS
Target Device: cc13x2_26x2

******************************************************************************

 Copyright (c) 2013-2021, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************


*****************************************************************************/

#ifndef APPLICATION_SIG_MODELS_H_
#define APPLICATION_SIG_MODELS_H_

/*********************************************************************
* INCLUDES
*/
#include <zephyr.h>
#include <sys/printk.h>
#include <settings/settings.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>

/*********************************************************************
* CONSTANTS
*/
#define STATE_OFF       0x00
#define STATE_ON        0x01
#define STATE_DEFAULT   0x01
#define STATE_RESTORE   0x02

/* Model Operation Codes */
#define BT_MESH_MODEL_OP_GEN_ONOFF_GET                BT_MESH_MODEL_OP_2(0x82, 0x01)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET                BT_MESH_MODEL_OP_2(0x82, 0x02)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK          BT_MESH_MODEL_OP_2(0x82, 0x03)
#define BT_MESH_MODEL_OP_GEN_ONOFF_STATUS             BT_MESH_MODEL_OP_2(0x82, 0x04)

#define BT_MESH_MODEL_OP_GEN_LEVEL_GET                BT_MESH_MODEL_OP_2(0x82, 0x05)
#define BT_MESH_MODEL_OP_GEN_LEVEL_SET                BT_MESH_MODEL_OP_2(0x82, 0x06)
#define BT_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK          BT_MESH_MODEL_OP_2(0x82, 0x07)
#define BT_MESH_MODEL_OP_GEN_LEVEL_STATUS             BT_MESH_MODEL_OP_2(0x82, 0x08)
#define BT_MESH_MODEL_OP_GEN_DELTA_SET                BT_MESH_MODEL_OP_2(0x82, 0x09)
#define BT_MESH_MODEL_OP_GEN_DELTA_SET_UNACK          BT_MESH_MODEL_OP_2(0x82, 0x0A)
#define BT_MESH_MODEL_OP_GEN_MOVE_SET                 BT_MESH_MODEL_OP_2(0x82, 0x0B)
#define BT_MESH_MODEL_OP_GEN_MOVE_SET_UNACK           BT_MESH_MODEL_OP_2(0x82, 0x0C)

#define BT_MESH_MODEL_GEN_DEF_TRANS_TIME_GET          BT_MESH_MODEL_OP_2(0x82, 0x0D)
#define BT_MESH_MODEL_GEN_DEF_TRANS_TIME_SET          BT_MESH_MODEL_OP_2(0x82, 0x0E)
#define BT_MESH_MODEL_GEN_DEF_TRANS_TIME_SET_UNACK    BT_MESH_MODEL_OP_2(0x82, 0x0F)
#define BT_MESH_MODEL_GEN_DEF_TRANS_TIME_STATUS       BT_MESH_MODEL_OP_2(0x82, 0x10)

#define BT_MESH_MODEL_GEN_ONPOWERUP_GET               BT_MESH_MODEL_OP_2(0x82, 0x11)
#define BT_MESH_MODEL_GEN_ONPOWERUP_STATUS            BT_MESH_MODEL_OP_2(0x82, 0x12)
#define BT_MESH_MODEL_GEN_ONPOWERUP_SET               BT_MESH_MODEL_OP_2(0x82, 0x13)
#define BT_MESH_MODEL_GEN_ONPOWERUP_SET_UNACK         BT_MESH_MODEL_OP_2(0x82, 0x14)

/* Generic Battery Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_BATTERY_GET             BT_MESH_MODEL_OP_2(0x82, 0x23)
#define BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS          BT_MESH_MODEL_OP_2(0x82, 0x24)

/* number of milliseconds per second */
#define MSEC_PER_SEC 1000U

#define UNKNOWN_LEVEL_VALUE 0x3F

enum transition_types_e {
    MOVE = 0x01,
    NON_MOVE,
    ONOFF,
    LEVEL,
    DELTA_LEVEL,
    MOVE_STATE,
};

struct state_properties_t {
    uint16_t current;
    uint16_t target;
    uint16_t range_min;
    uint16_t range_max;
    int delta;
};

struct transition_t {
    enum transition_types_e type;
    uint8_t trans_time;
    uint8_t remain_time;
    uint8_t delay;
    uint32_t quo_tt;
    uint32_t num_of_hops;
    uint32_t hop_duration;
    uint32_t total_duration;
    int64_t start_timestamp;
};

struct ctl_state_t {
    uint8_t default_trans_time;
    uint8_t last_tid;
    uint16_t last_src_addr;
    uint16_t last_dst_addr;
    int64_t last_msg_timestamp;
    struct transition_t *transition;
    struct state_properties_t *state_prop;
};

/*
 * GenOnOff structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint8_t onOff;
    uint8_t tid;
    uint8_t transitionTime;
    uint8_t delay;
} genOnOffSet_t;

/*
 * GenLevel structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t level;
    uint8_t  tid;
    uint8_t  transitionTime;
    uint8_t  delay;
} genLevelSet_t;

/*
 * DeltaLevel structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint32_t deltaLevel;
    uint8_t  tid;
    uint8_t  transitionTime;
    uint8_t  delay;
} genDeltaLevelSet_t;

/*
 * Move structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint16_t deltaLevel;
    uint8_t  tid;
    uint8_t  transitionTime;
    uint8_t  delay;
} genMoveSet_t;

/*
 * Transition time structures
 */
ZEPHYR__TYPEDEF_PACKED(struct)
{
    uint8_t  transTime;
} genTransTimeSet_t;

// Generic Battery Server State
ZEPHYR__PACKED(struct) batteryStatus_t
{
    uint8_t battery_level;           /* Value of Generic Battery Level state             */
    uint32_t time_to_discharge : 24; /* Value of Generic Battery Time to Discharge state */
    uint32_t time_to_charge    : 24; /* Value of Generic Battery Time to Charge state    */
    uint8_t flags;                   /* Value of Generic Battery Flags state             */
};

// Generic Power OnOff Server state
struct genOnPowerUpState_t{
    uint8_t onPowerUp;
};

/*********************************************************************
 * FUNCTIONS
 */

int MeshModels_init();

void gen_OnOff_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf);

void gen_OnOff_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct net_buf_simple *buf);

void gen_OnOff_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf);

void gen_OnOff_Status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct net_buf_simple *buf);

void gen_Level_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf);

void gen_Level_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct net_buf_simple *buf);

void gen_Level_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf);

void gen_Delta_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct net_buf_simple *buf);

void gen_Delta_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf);

void gen_Move_set_unack(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct net_buf_simple *buf);

void gen_Move_set(struct bt_mesh_model *model,
             struct bt_mesh_msg_ctx *ctx,
             struct net_buf_simple *buf);

void gen_Level_Status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct net_buf_simple *buf);

void gen_Default_Transition_Time_get(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct net_buf_simple *buf);

void gen_Default_Transition_Time_set_unack(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *ctx,
                     struct net_buf_simple *buf);

void gen_Default_Transition_Time_set(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *ctx,
                     struct net_buf_simple *buf);

void gen_Default_Transition_Time_Status(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf);

void gen_OnPowerUp_Status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct net_buf_simple *buf);

void gen_OnPowerUp_get(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf);

void gen_OnPowerUp_set(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf);

void gen_OnPowerUp_set_unack(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf);

void gen_Battery_status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct net_buf_simple *buf);
void gen_Battery_get(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf);
#endif /* APPLICATION_SIG_MODELS_H_ */
