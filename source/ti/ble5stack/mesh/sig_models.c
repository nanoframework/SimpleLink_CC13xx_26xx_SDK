/******************************************************************************

@file  sig_models.c

@brief  Generic Models

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

/*********************************************************************
* INCLUDES
*/
#include <ti/display/Display.h>
#include "menu/two_btn_menu.h"
#include "simple_mesh_node_menu.h"
#include "simple_mesh_node.h"
#include "autoconf.h"
#include "sig_models.h"
#include "health_model.h"
#include "ti_device_composition.h"
//added for clock settings:
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include "stdint.h"

#define SET_LEVEL_EVT                  19
#define SET_ONOFF_EVT                  20

extern Display_Handle dispHandle;

static Clock_Struct modelTransitionClk;
static Clock_Struct modelDelayClk;
static Clock_Struct modelSetOnClk;
static Clock_Struct modelSetOffClk;
static Clock_Struct genericLevelDelayClk;
static Clock_Struct genericLevelSetClk;

/* According to SPEC 3.1.2 */
struct state_properties_t values = {
    .range_min = (uint16_t)(0x8000 - INT16_MIN), //0x0000
    .range_max = 0x7FFF - INT16_MIN, //0xFFFF
};

struct transition_t transition = {
    .trans_time = 0
};

struct ctl_state_t generic_state = {
    .default_trans_time = 0, /* Transition time shall be 0, if no default transition time server model defiened */
    .state_prop = &values,
    .transition = &transition,
};

// Generic Power OnOff Server state
struct genOnPowerUpState_t ponoff_state = {
    .onPowerUp = 0
};

// Generic Battery Server state
struct batteryStatus_t battery_server;


static void calculate_remain_time(struct transition_t *transition);
static uint16_t constrain_target_value(uint16_t target_value);
static void constrain_delta_target_value(void);
static void set_target(uint8_t type, void *dptr);
static bool set_transition_timeout(struct transition_t *transition);
static void set_transition_values(uint8_t type);
static int get_current(uint8_t type);
static int get_target(uint8_t type);
static void gen_level_publish(struct bt_mesh_model *model);
static void OnOffSetOff_clockHandler(UArg arg);
static void OnOffSetOn_clockHandler(UArg arg);
static void setDelay_clockHandler(UArg arg);
static void levelSet_clockHandler(UArg arg);
static void genericLevelDelay_clockHandler(UArg arg);

static void getBatteryState(struct batteryStatus_t *battery_status);

static void start_onoff_delay_clock(struct bt_mesh_model *model);
static void start_level_delay_clock(struct bt_mesh_model *model);
static void start_onoff_transition_clock(struct bt_mesh_model *model);
static void start_level_transition_clock(struct bt_mesh_model *model);

#ifdef CONFIG_BT_SETTINGS
int generic_power_onoff_settings_cb(struct bt_mesh_model *model,
                                    size_t len_rd, settings_read_cb read_cb,
                                    void *cb_arg);
int generic_default_transition_time_settings_cb(struct bt_mesh_model *model,
                                    size_t len_rd, settings_read_cb read_cb,
                                    void *cb_arg);
int generic_OnOff_settings_cb(struct bt_mesh_model *model,
                              size_t len_rd, settings_read_cb read_cb,
                              void *cb_arg);
int store_data(struct bt_mesh_model * model, uint8_t data);
#endif

extern Clock_Handle Util_constructClock(Clock_Struct *pClock,
                                 Clock_FuncPtr clockCB,
                                 uint32_t clockDuration,
                                 uint32_t clockPeriod,
                                 uint8_t startFlag,
                                 UArg arg);

extern void Util_startClock(Clock_Struct *pClock);
extern void Util_restartClock(Clock_Struct *pClock, uint32_t clockTimeout);

uint8_t get_PowerUp_mode(struct bt_mesh_model *model);
uint16_t get_PoweredDown_state(struct bt_mesh_model *model);
uint8_t get_def_trans_time(struct bt_mesh_model *model);

void set_OnOff_target(uint16_t target);
void set_OnOff_current(uint16_t current);
void set_OnOff_total_duration(uint32_t duration);

typedef struct
{
  uint8_t event;
  bool is_ack;
  struct bt_mesh_model *model;
}modelClockEventData_t;

typedef struct
{
  uint8_t event;
  struct bt_mesh_model *model;
}genericLevelClockEventData_t;

modelClockEventData_t onoffDelayClk_evtData =
{ .event = SET_LEVEL_EVT };

modelClockEventData_t onoffTransClk_evtData =
{ .event = SET_LEVEL_EVT };

modelClockEventData_t levelDelayClk_evtData =
{ .event = SET_LEVEL_EVT };

modelClockEventData_t levelTransClk_evtData =
{ .event = SET_LEVEL_EVT };

static bool is_onoff_delay_clock_constructed = false;
static bool is_on_clock_constructed = false;
static bool is_off_clock_constructed = false;
static bool is_level_delay_clock_constructed = false;
static bool is_level_trans_clock_constructed = false;

//Global variable to some the delta values from previous delta set and delta set_unack CB's:
int16_t gen_Delta_last_level_value = 0;

/*********************************************************************
 * FUNCTIONS
 */
int MeshModels_init()
{
    uint8_t PowerUp_mode;
    uint8_t PoweredDown_state;
    uint8_t def_trans_time;
    uint16_t target;
    struct bt_mesh_model *pModel_OnOff;
    struct bt_mesh_model *pModel_PowerUp;
    struct bt_mesh_model *pModel_DefTransTime;

    /* Initiating the health server model */
    healthSrv_init();

    /* Search for generic OnOff model in the sample App element */
    pModel_OnOff = bt_mesh_model_find(&comp.elem[SAMPLE_APP_MODELS_ELEMENT], BT_MESH_MODEL_ID_GEN_ONOFF_SRV);

    if(pModel_OnOff == NULL)
    {
        return 0;  /* There is no generic OnOff model on the element, so no need to restore the OnOff state */
    }

    /* Search for generic PowerOnOff model in the sample App element */
    pModel_PowerUp = bt_mesh_model_find(&comp.elem[SAMPLE_APP_MODELS_ELEMENT], BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV);

    if(pModel_PowerUp == NULL)
    {
        target = 0;
        set_OnOff_target(target); /* There is no generic PowerOnOff model on the element, so OnOff state should be set to OFF */
    }

    else
    {
        PowerUp_mode = get_PowerUp_mode(pModel_PowerUp);

        switch (PowerUp_mode) {
            case 0x00:      /* the Generic OnOff state shall be set to Off */
                target = 0;
                break;

            case 0x01:      /* the Generic OnOff state shall be set to On */
                target = 1;
                break;

            case 0x02:      /* the Generic OnOff state shall be set to the states the element was in,or in transition to, when powered down */
                PoweredDown_state = get_PoweredDown_state(pModel_OnOff);
                target = PoweredDown_state;
                break;

            default:
                return 0;
            }

        set_OnOff_target(target);
    }

    /* Search for default_transition_time model in the sample App element */
    pModel_DefTransTime = bt_mesh_model_find(&comp.elem[SAMPLE_APP_MODELS_ELEMENT], BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV);

    if(pModel_DefTransTime == NULL){
        def_trans_time = 0;  /* There is no default_transition_time model on the element, so the transition is instantaneous */
    }
    else{
        def_trans_time = get_def_trans_time(pModel_DefTransTime);
    }

    if(def_trans_time == 0){      /* Instantaneous transition */
        set_OnOff_current(target);
    }
    else{                         /* Transition to the target value with the defined transition time */
        set_OnOff_total_duration(def_trans_time);
        start_onoff_transition_clock(pModel_OnOff);
     }
    return 0;
}



const struct bt_mesh_model_cb generic_OnOff_srv_cb = {
#ifdef CONFIG_BT_SETTINGS
    .settings_set = generic_OnOff_settings_cb
#endif
};

/*Save default transition time to NV configurations */
const struct bt_mesh_model_cb generic_defualt_transition_time_srv_cb = {
#ifdef CONFIG_BT_SETTINGS
    .settings_set = generic_default_transition_time_settings_cb
#endif
};

const struct bt_mesh_model_cb generic_power_onoff_srv_cb = {
#ifdef CONFIG_BT_SETTINGS
    .settings_set = generic_power_onoff_settings_cb
#endif
};

#ifdef CONFIG_BT_SETTINGS
int generic_OnOff_settings_cb(struct bt_mesh_model *model,
                              size_t len_rd, settings_read_cb read_cb,
                              void *cb_arg)
{
    struct ctl_state_t *pData = (struct ctl_state_t *)model->user_data;

    if (read_cb(cb_arg, &pData->state_prop->target, sizeof(uint16_t)) != sizeof(uint16_t)) {
        return -EINVAL;
    }

    return 0;
}

int generic_default_transition_time_settings_cb(struct bt_mesh_model *model,
                                    size_t len_rd, settings_read_cb read_cb,
                                    void *cb_arg)
{
    struct ctl_state_t *pData = (struct ctl_state_t *)model->user_data;

    if (read_cb(cb_arg, &pData->default_trans_time, sizeof(uint8_t)) != sizeof(uint8_t)) {
        return -EINVAL;
     }

    return 0;
}

int generic_power_onoff_settings_cb(struct bt_mesh_model *model,
                                    size_t len_rd, settings_read_cb read_cb,
                                    void *cb_arg)
{
    struct genOnPowerUpState_t *data = (struct genOnPowerUpState_t *)model->user_data;

    if (read_cb(cb_arg, &data->onPowerUp, sizeof(uint8_t)) != sizeof(uint8_t)) {
        return -EINVAL;
    }

    return 0;
}

int store_data(struct bt_mesh_model * model, uint8_t data)
{
    return bt_mesh_model_data_store(model, false, &data, sizeof(data));
}
#endif

/* Generic OnOff Server message handlers */
void gen_OnOff_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    int err;
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 3 + 4);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_OnOff_get callback from addr=0x%x", ctx->addr);
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_GEN_ONOFF_STATUS);
    net_buf_simple_add_u8(msg, (uint8_t) get_current(ONOFF));

    if (generic_state.transition->num_of_hops) {
        calculate_remain_time(generic_state.transition);
        net_buf_simple_add_u8(msg, (uint8_t) get_target(ONOFF));
        net_buf_simple_add_u8(msg, generic_state.transition->remain_time);
    }

    err = bt_mesh_model_send(model, ctx, msg, NULL, NULL);
    if (err) {
        Display_printf(dispHandle, TBM_ROW_APP + 9, 0,"Unable to send GEN_ONOFF_SRV Status response\n");
    }
}

static void gen_onoff_publish(struct bt_mesh_model *model)
{
    int err;
    struct net_buf_simple *msg = model->pub->msg;
    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_GEN_ONOFF_STATUS);
    net_buf_simple_add_u8(msg, (uint8_t) get_current(ONOFF));

    if (generic_state.transition->num_of_hops) {
        calculate_remain_time(generic_state.transition);
        net_buf_simple_add_u8(msg, (uint8_t) get_target(ONOFF));
        net_buf_simple_add_u8(msg, generic_state.transition->remain_time);
    }

    err = bt_mesh_model_publish(model);
    if (err) {
        Display_printf(dispHandle, TBM_ROW_APP + 9, 0,"bt_mesh_model_publish err %d\n", err);
    }
}

void gen_OnOff_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct net_buf_simple *buf)
{
    struct ctl_state_t *pData = (struct ctl_state_t *)model->user_data;

    uint8_t tid, onoff, tt, delay;
    int64_t now;

    onoff = net_buf_simple_pull_u8(buf);
    tid = net_buf_simple_pull_u8(buf);
    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_OnOff_set_unack callback from addr=0x%x, onOff=%d", ctx->addr, onoff);

    if (onoff > STATE_ON) {
        return;
    }

    /* Indicating this message is a retransmission */
    now = k_uptime_get();
    if (generic_state.last_tid == tid &&
        generic_state.last_src_addr == ctx->addr &&
        generic_state.last_dst_addr == ctx->recv_dst &&
        (now - generic_state.last_msg_timestamp <= (6 * MSEC_PER_SEC))) {
        return;
    }

    /* for optional fields support: */
    switch (buf->len) {
    case 0x00:      /* No optional fields are available */
        tt = generic_state.default_trans_time;
        delay = 0;
        break;
    case 0x02:      /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    generic_state.transition->num_of_hops = 0;

    generic_state.last_tid = tid;
    generic_state.last_src_addr = ctx->addr;
    generic_state.last_dst_addr = ctx->recv_dst;
    generic_state.last_msg_timestamp = now;
    generic_state.transition->trans_time = tt;
    generic_state.transition->delay = delay;
    generic_state.transition->type = NON_MOVE;
    set_target(ONOFF, &onoff);

    if (generic_state.state_prop->target != generic_state.state_prop->current) {
        set_transition_values(ONOFF);
    } else {
        return;
    }

#ifdef CONFIG_BT_SETTINGS
    /* Save OnOff state to NV */
    store_data(model, pData->state_prop->target);
#endif

    /* For Instantaneous Transition */
    if (generic_state.transition->num_of_hops == 0) {
        generic_state.state_prop->current = generic_state.state_prop->target;
        gen_onoff_publish(model);
        return;
    }

    if(generic_state.transition->delay > 0){
        start_onoff_delay_clock(model);
    }
    else{
        start_onoff_transition_clock(model);
    }
}

void gen_OnOff_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    struct ctl_state_t *pData = (struct ctl_state_t *)model->user_data;

    uint8_t tid, onoff, tt, delay;
    int64_t now;

    onoff = net_buf_simple_pull_u8(buf);
    tid = net_buf_simple_pull_u8(buf);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_OnOff_set callback from addr=0x%x, onOff=%d", ctx->addr, onoff);
    if (onoff > STATE_ON) {
        return;
    }

    /* Indicating this message is a retransmission */
    now = k_uptime_get();
    if (generic_state.last_tid == tid &&
        generic_state.last_src_addr == ctx->addr &&
        generic_state.last_dst_addr == ctx->recv_dst &&
        (now - generic_state.last_msg_timestamp <= (6 * MSEC_PER_SEC))) {
        gen_OnOff_get(model, ctx, buf);
        return;
    }

    /* for optional fields support: */
    switch (buf->len) {
    case 0x00:      /* No optional fields are available */
        tt = generic_state.default_trans_time;
        delay = 0;
        break;
    case 0x02:      /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    generic_state.transition->num_of_hops = 0;

    generic_state.last_tid = tid;
    generic_state.last_src_addr = ctx->addr;
    generic_state.last_dst_addr = ctx->recv_dst;
    generic_state.last_msg_timestamp = now;
    generic_state.transition->trans_time = tt;
    generic_state.transition->delay = delay;
    generic_state.transition->type = NON_MOVE;
    set_target(ONOFF, &onoff);

    if (generic_state.state_prop->target != generic_state.state_prop->current) {
        set_transition_values(ONOFF);
    } else {
        gen_OnOff_get(model, ctx, buf);
        return;
    }

#ifdef CONFIG_BT_SETTINGS
    /* Save OnOff state to NV */
    store_data(model, pData->state_prop->target);
#endif

    /* For Instantaneous Transition */
    if (generic_state.transition->num_of_hops == 0) {
        generic_state.state_prop->current = generic_state.state_prop->target;

        gen_OnOff_get(model, ctx, buf);
        gen_onoff_publish(model);
        return;
    }

    gen_OnOff_get(model, ctx, buf); // Sending status message

    if(generic_state.transition->delay > 0){
        start_onoff_delay_clock(model);
    }
    else{
        start_onoff_transition_clock(model);
    }
}

/* Generic OnOff Client message handlers */
void gen_OnOff_Status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct net_buf_simple *buf)
{
    if (buf->len == 1) // only present onOff value
    {
        Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_OnOff_Status callback, Present OnOff=%1d",
                           net_buf_simple_pull_u8(buf));
    }
    else if (buf->len == 3) // all structure fields
    {
        Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_OnOff_Status callback, Present OnOff=%1d, Target OnOff=%1d, RemainingTime=0x%02x",
                           net_buf_simple_pull_u8(buf), net_buf_simple_pull_u8(buf), net_buf_simple_pull_u8(buf));
    }
    else // length error
    {
        Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_OnOff_Status callback, len error = %d",
                       buf->len);
    }
}

/* Generic Level Server message handlers */
void gen_Level_get(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 5 + 4);
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_GEN_LEVEL_STATUS);
    net_buf_simple_add_le16(msg, (int16_t) get_current(LEVEL));

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Level_get callback from addr=0x%x", ctx->addr);
    /*
     * If a server is in a process of changing the
     * Generic Level state, send the target Generic
     * Level state and the remaining transition time.
     *
     * See Mesh Model Specification 3.4.2.2.5
     */
    if (generic_state.transition->num_of_hops) {
        calculate_remain_time(generic_state.transition);
        net_buf_simple_add_le16(msg, (int16_t) get_target(LEVEL));
        net_buf_simple_add_u8(msg, generic_state.transition->remain_time);
    }

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        Display_printf(dispHandle, TBM_ROW_APP + 9, 0,"Unable to send GEN_LEVEL_SRV Status response\n");
    }

}

static void gen_level_publish(struct bt_mesh_model *model)
{
    int err;
    struct net_buf_simple *msg = model->pub->msg;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_GEN_LEVEL_STATUS);
    net_buf_simple_add_le16(msg, (int16_t) get_current(LEVEL));

    if (generic_state.transition->num_of_hops) {
        calculate_remain_time(generic_state.transition);
        net_buf_simple_add_le16(msg, (int16_t) get_target(LEVEL));
        net_buf_simple_add_u8(msg, generic_state.transition->remain_time);
    }

    err = bt_mesh_model_publish(model);
    if (err) {
        Display_printf(dispHandle, TBM_ROW_APP + 9, 0,"bt_mesh_model_publish err %d\n", err);
    }
}

void gen_Level_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct net_buf_simple *buf)
{
    uint8_t tid, tt, delay;
    int16_t level;
    int64_t now;

    level = (int16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Level_set_unack callback from addr=0x%x, level=0x%x", ctx->addr, level);
    /*
     * Indicating this message is a retransmission
     * See Mesh Model Specification 3.4.2.2.2
     */
    now = k_uptime_get();
    if (generic_state.last_tid == tid &&
        generic_state.last_src_addr == ctx->addr &&
        generic_state.last_dst_addr == ctx->recv_dst &&
        (now - generic_state.last_msg_timestamp <= (6 * MSEC_PER_SEC))) {
        return;
    }

    /* for optional fields support: */
    switch (buf->len) {
    case 0x00:      /* No optional fields are available */
        tt = generic_state.default_trans_time;
        delay = 0;
        break;
    case 0x02:      /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) { /* Unknown value, only values 0x00 - 0x3E shall be used */
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    generic_state.transition->num_of_hops = 0;

    generic_state.last_tid = tid;
    generic_state.last_src_addr = ctx->addr;
    generic_state.last_dst_addr = ctx->recv_dst;
    generic_state.last_msg_timestamp = now;
    generic_state.transition->trans_time = tt;
    generic_state.transition->delay = delay;
    generic_state.transition->type = NON_MOVE;
    set_target(LEVEL, &level);

    if (generic_state.state_prop->target != generic_state.state_prop->current) {
        set_transition_values(LEVEL);
    } else {
        return;
    }

    /* For Instantaneous Transition */
    if (generic_state.transition->num_of_hops == 0){
        generic_state.state_prop->current = generic_state.state_prop->target;
        gen_level_publish(model);
        return;
    }

    if(generic_state.transition->delay > 0){
        start_level_delay_clock(model);
    }
    else{
        start_level_transition_clock(model);
    }
}

void gen_Level_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    uint8_t tid, tt, delay;
    int16_t level;
    int64_t now;

    level = (int16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Level_set callback from addr=0x%x, level=0x%x", ctx->addr, level);
    /*
     * Indicating this message is a retransmission
     * See Mesh Model Specification 3.4.2.2.2
     */
    now = k_uptime_get();
    if (generic_state.last_tid == tid &&
        generic_state.last_src_addr == ctx->addr &&
        generic_state.last_dst_addr == ctx->recv_dst &&
        (now - generic_state.last_msg_timestamp <= (6 * MSEC_PER_SEC))) {
        gen_Level_get(model, ctx, buf);
        return;
    }

    /* for optional fields support: */
    switch (buf->len) {
    case 0x00:      /* No optional fields are available */
        tt = generic_state.default_trans_time;
        delay = 0;
        break;
    case 0x02:      /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    generic_state.transition->num_of_hops = 0;

    generic_state.last_tid = tid;
    generic_state.last_src_addr = ctx->addr;
    generic_state.last_dst_addr = ctx->recv_dst;
    generic_state.last_msg_timestamp = now;
    generic_state.transition->trans_time = tt;
    generic_state.transition->delay = delay;
    generic_state.transition->type = NON_MOVE;
    set_target(LEVEL, &level);

    if (generic_state.state_prop->target != generic_state.state_prop->current) {
        set_transition_values(LEVEL);
    } else {
        gen_Level_get(model, ctx, buf);
        return;
    }

    /* For Instantaneous Transition */
    if (generic_state.transition->num_of_hops == 0) {
        generic_state.state_prop->current = generic_state.state_prop->target;
        gen_Level_get(model, ctx, buf);
        gen_level_publish(model);
        return;
    }

    gen_Level_get(model, ctx, buf); // Sending status message

    if(generic_state.transition->delay > 0){
        start_level_delay_clock(model);
    }
    else{
        start_level_transition_clock(model);
    }
}

void gen_Delta_set_unack(struct bt_mesh_model *model,
                struct bt_mesh_msg_ctx *ctx,
                struct net_buf_simple *buf)
{
    uint8_t tid, tt, delay;
    int32_t target, delta;
    int64_t now;

    delta = (int32_t) net_buf_simple_pull_le32(buf);
    tid = net_buf_simple_pull_u8(buf);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Delta_set_unack callback from addr=0x%x, delta=0x%x", ctx->addr, delta);
    now = k_uptime_get();
    if (generic_state.last_tid == tid &&
        generic_state.last_src_addr == ctx->addr &&
        generic_state.last_dst_addr == ctx->recv_dst &&
        (now - generic_state.last_msg_timestamp <= (6 * MSEC_PER_SEC))) {

        if (generic_state.state_prop->delta == delta) {
            return;
        }
        target = gen_Delta_last_level_value + delta;

    } else {
        gen_Delta_last_level_value = (int16_t) get_current(LEVEL);
        target = gen_Delta_last_level_value + delta;
    }

    /* for optional fields support: */
    switch (buf->len) {
    case 0x00:      /* No optional fields are available */
        tt = generic_state.default_trans_time;
        delay = 0;
        break;
    case 0x02:      /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    generic_state.transition->num_of_hops = 0;

    generic_state.last_tid = tid;
    generic_state.last_src_addr = ctx->addr;
    generic_state.last_dst_addr = ctx->recv_dst;
    generic_state.last_msg_timestamp = now;
    generic_state.transition->trans_time = tt;
    generic_state.transition->delay = delay;
    generic_state.transition->type = NON_MOVE;

    if (target < INT16_MIN) {
        target = INT16_MIN;
    } else if (target > INT16_MAX) {
        target = INT16_MAX;
    }

    set_target(DELTA_LEVEL, &target);

    if (generic_state.state_prop->target != generic_state.state_prop->current) {
        set_transition_values(LEVEL);
    } else {
        return;
    }

    /* For Instantaneous Transition */
    if (generic_state.transition->num_of_hops == 0) {
        generic_state.state_prop->current = generic_state.state_prop->target;
        gen_level_publish(model);
        return;
    }

    if(generic_state.transition->delay > 0){
        start_level_delay_clock(model);
    }
    else{
       start_level_transition_clock(model);
    }
}

void gen_Delta_set(struct bt_mesh_model *model,
              struct bt_mesh_msg_ctx *ctx,
              struct net_buf_simple *buf)
{
    uint8_t tid, tt, delay;
    int32_t target, delta;
    int64_t now;

    delta = (int32_t) net_buf_simple_pull_le32(buf);
    tid = net_buf_simple_pull_u8(buf);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Delta_set callback from addr=0x%x, delta=0x%x", ctx->addr, delta);
    now = k_uptime_get();
    if (generic_state.last_tid == tid &&
        generic_state.last_src_addr == ctx->addr &&
        generic_state.last_dst_addr == ctx->recv_dst &&
        (now - generic_state.last_msg_timestamp <= (6 * MSEC_PER_SEC))) {

        if (generic_state.state_prop->delta == delta) {
            gen_Level_get(model, ctx, buf);
            return;
        }
        target = gen_Delta_last_level_value + delta;

    } else {
        gen_Delta_last_level_value = (int16_t) get_current(LEVEL);
        target = gen_Delta_last_level_value + delta;
    }

    /* for optional fields support: */
    switch (buf->len) {
    case 0x00:      /* No optional fields are available */
        tt = generic_state.default_trans_time;
        delay = 0;
        break;
    case 0x02:      /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    generic_state.transition->num_of_hops = 0;

    generic_state.last_tid = tid;
    generic_state.last_src_addr = ctx->addr;
    generic_state.last_dst_addr = ctx->recv_dst;
    generic_state.last_msg_timestamp = now;
    generic_state.transition->trans_time = tt;
    generic_state.transition->delay = delay;
    generic_state.transition->type = NON_MOVE;

    if (target < INT16_MIN) {
        target = INT16_MIN;
    } else if (target > INT16_MAX) {
        target = INT16_MAX;
    }

    set_target(DELTA_LEVEL, &target);

    if (generic_state.state_prop->target != generic_state.state_prop->current) {
        set_transition_values(LEVEL);
    } else {
        gen_Level_get(model, ctx, buf);
        return;
    }

    /* For Instantaneous Transition */
    if (generic_state.transition->num_of_hops == 0) {
        generic_state.state_prop->current = generic_state.state_prop->target;
        gen_Level_get(model, ctx, buf);
        gen_level_publish(model);
        return;
    }

    gen_Level_get(model, ctx, buf); // Sending status message

    if(generic_state.transition->delay > 0){
        start_level_delay_clock(model);
    }
    else{
        start_level_transition_clock(model);
    }
}

void gen_Move_set_unack(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct net_buf_simple *buf)
{
    uint8_t tid, tt, delay;
    int16_t delta;
    uint16_t target;
    int64_t now;

    delta = (int16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Move_set_unack callback from addr=0x%x, delta=0x%x", ctx->addr, delta);
    now = k_uptime_get();
    if (generic_state.last_tid == tid &&
        generic_state.last_src_addr == ctx->addr &&
        generic_state.last_dst_addr == ctx->recv_dst &&
        (now - generic_state.last_msg_timestamp <= (6 * MSEC_PER_SEC))) {
        return;
    }

    /* for optional fields support: */
    switch (buf->len) {
    case 0x00:      /* No optional fields are available */
        tt = generic_state.default_trans_time;
        delay = 0;
        break;
    case 0x02:      /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    generic_state.transition->num_of_hops = 0;

    generic_state.last_tid = tid;
    generic_state.last_src_addr = ctx->addr;
    generic_state.last_dst_addr = ctx->recv_dst;
    generic_state.last_msg_timestamp = now;
    generic_state.transition->trans_time = tt;
    generic_state.transition->delay = delay;
    generic_state.transition->type = MOVE;
    generic_state.state_prop->delta = delta;

    /* According to SPEC 3.3.2.2.4 */
    if (delta < 0) { /*indicates negative transition speed*/
        target = generic_state.state_prop->range_min;
    } else if (delta > 0) { /*indicates positive transition speed*/
        target = generic_state.state_prop->range_max;
    } else if (delta == 0) {
        target = generic_state.state_prop->current;
    }
    set_target(MOVE_STATE, &target);

    if (generic_state.state_prop->target != generic_state.state_prop->current) {
        set_transition_values(MOVE_STATE);
    } else {
        return;
    }

    if (generic_state.transition->num_of_hops == 0) {
        return;
    }

    if(generic_state.transition->delay > 0){
        start_level_delay_clock(model);
    }
    else{
        start_level_transition_clock(model);
    }
}

void gen_Move_set(struct bt_mesh_model *model,
             struct bt_mesh_msg_ctx *ctx,
             struct net_buf_simple *buf)
{
    uint8_t tid, tt, delay;
    int16_t delta;
    uint16_t target;
    int64_t now;

    delta = (int16_t) net_buf_simple_pull_le16(buf);
    tid = net_buf_simple_pull_u8(buf);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Move_set callback from addr=0x%x, delta=0x%x", ctx->addr, delta);
    now = k_uptime_get();
    if (generic_state.last_tid == tid &&
        generic_state.last_src_addr == ctx->addr &&
        generic_state.last_dst_addr == ctx->recv_dst &&
        (now - generic_state.last_msg_timestamp <= (6 * MSEC_PER_SEC))) {
        gen_Level_get(model, ctx, buf);
        return;
    }

    /* for optional fields support: */
    switch (buf->len) {
    case 0x00:      /* No optional fields are available */
        tt = generic_state.default_trans_time;
        delay = 0;
        break;
    case 0x02:      /* Optional fields are available */
        tt = net_buf_simple_pull_u8(buf);
        if ((tt & 0x3F) == 0x3F) {
            return;
        }

        delay = net_buf_simple_pull_u8(buf);
        break;
    default:
        return;
    }

    generic_state.transition->num_of_hops = 0;

    generic_state.last_tid = tid;
    generic_state.last_src_addr = ctx->addr;
    generic_state.last_dst_addr = ctx->recv_dst;
    generic_state.last_msg_timestamp = now;
    generic_state.transition->trans_time = tt;
    generic_state.transition->delay = delay;
    generic_state.transition->type = MOVE;
    generic_state.state_prop->delta = delta;

    if (delta < 0) {
        target = generic_state.state_prop->range_min;
    } else if (delta > 0) {
        target = generic_state.state_prop->range_max;
    } else if (delta == 0) {
        target = generic_state.state_prop->current;
    }
    set_target(MOVE_STATE, &target);

    if (generic_state.state_prop->target != generic_state.state_prop->current) {
        set_transition_values(MOVE_STATE);
    } else {
        gen_Level_get(model, ctx, buf);
        return;
    }

    if (generic_state.transition->num_of_hops == 0) {
        gen_Level_get(model, ctx, buf);
        return;
    }
    gen_Level_get(model, ctx, buf); // Sending status message

    if(generic_state.transition->delay > 0){
        start_level_delay_clock(model);
    }
    else{
         start_level_transition_clock(model);
    }
}


/* Generic Level Client message handlers */
void gen_Level_Status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct net_buf_simple *buf)
{
    if (buf->len == 2) // only present level value
    {
        Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Level_Status callback, Present level=0x%04x",
                       net_buf_simple_pull_le16(buf));
    }
    else if (buf->len == 5) // all structure fields
    {
        Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Level_Status callback, Present level=0x%04x, Target level=0x%04x, RemainingTime=0x%02x",
                       net_buf_simple_pull_le16(buf), net_buf_simple_pull_le16(buf), net_buf_simple_pull_u8(buf));
    }
    else // length error
    {
        Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Level_Status callback, len error = %d",
                       buf->len);
    }
}

/* Generic Default Transition Time Server message handlers */
void gen_Default_Transition_Time_get(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct net_buf_simple *buf)
{
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 1 + 4);

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_GEN_DEF_TRANS_TIME_STATUS);
    net_buf_simple_add_u8(msg, generic_state.default_trans_time);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        Display_printf(dispHandle, TBM_ROW_APP + 9, 0,"Unable to send GEN_DEF_TT_SRV Status response\n");
    }
}

void gen_def_trans_time_publish(struct bt_mesh_model *model)
{
    int err;
    struct net_buf_simple *msg = model->pub->msg;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_GEN_DEF_TRANS_TIME_STATUS);
    net_buf_simple_add_u8(msg, generic_state.default_trans_time);

    err = bt_mesh_model_publish(model);
    if (err) {
    }
}

void gen_Default_Transition_Time_set_unack(struct bt_mesh_model *model,
                     struct bt_mesh_msg_ctx *ctx,
                     struct net_buf_simple *buf)
{
    struct ctl_state_t *pData = (struct ctl_state_t *)model->user_data;

    uint8_t tt;

    tt = net_buf_simple_pull_u8(buf);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Default_Transition_Time_set_unack callback, TransTime=0x%02x",
                           tt);
    if ((tt & 0x3F) == 0x3F) { /* Unknown value, only values of 0x00 through 0x3E shall be used */
        return;
    }

    if (generic_state.default_trans_time != tt) {
        generic_state.default_trans_time = tt;


#ifdef CONFIG_BT_SETTINGS
        store_data(model, pData->default_trans_time);
#endif

        gen_def_trans_time_publish(model);
    }
}

void gen_Default_Transition_Time_set(struct bt_mesh_model *model,
                   struct bt_mesh_msg_ctx *ctx,
                   struct net_buf_simple *buf)
{
    struct ctl_state_t *pData = (struct ctl_state_t *)model->user_data;

    uint8_t tt;

    tt = net_buf_simple_pull_u8(buf);
    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Default_Transition_Time_set callback, TransTime=0x%02x",
                           tt);

    if ((tt & 0x3F) == 0x3F) { /* Unknown value, only values of 0x00 through 0x3E shall be used */
        return;
    }

    if (generic_state.default_trans_time != tt) {
        generic_state.default_trans_time = tt;

#ifdef CONFIG_BT_SETTINGS
        store_data(model, pData->default_trans_time);
#endif

        gen_Default_Transition_Time_get(model, ctx, buf);
        gen_def_trans_time_publish(model);

    } else {
        gen_Default_Transition_Time_get(model, ctx, buf);
    }
}

/* Generic Default Transition Time Client message handlers */
void gen_Default_Transition_Time_Status(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf)
{
    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_Default_Transition_Time_Status callback, TransTime=0x%02x",
                           net_buf_simple_pull_u8(buf));
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

uint8_t get_PowerUp_mode(struct bt_mesh_model *model)
{
    struct genOnPowerUpState_t *pData = (struct genOnPowerUpState_t *)model->user_data;
    return pData->onPowerUp;
}

uint16_t get_PoweredDown_state(struct bt_mesh_model *model)
{
    struct ctl_state_t *pData = (struct ctl_state_t *)model->user_data;
    return pData->state_prop->target;
}

uint8_t get_def_trans_time(struct bt_mesh_model *model)
{
    struct ctl_state_t *pData = (struct ctl_state_t *)model->user_data;
    return pData->default_trans_time;
}

void set_OnOff_target(uint16_t target){
    generic_state.state_prop->target = target;
}

void set_OnOff_current(uint16_t current){
    generic_state.state_prop->current = current;
}

void set_OnOff_total_duration(uint32_t duration){
    generic_state.transition->total_duration = duration;

}

/* Function to calculate Remaining Time According to SPEC 3.1.3 (Start) */
static void calculate_remain_time(struct transition_t *transition)
{
    uint8_t steps, resolution;
    int32_t duration_remainder;
    int64_t now;

    if (transition->type == MOVE) {
        transition->remain_time = UNKNOWN_LEVEL_VALUE;
        return;
    }

    now = k_uptime_get();
    if(transition->start_timestamp == 0){
        duration_remainder = transition->total_duration;
    }else{
        duration_remainder = transition->total_duration -
                     (now - transition->start_timestamp);
    }

    if (duration_remainder > 620000) {
        /* > 620 seconds -> resolution = 0b11 [10 minutes] */
        resolution = 0x03;
        steps = duration_remainder / 600000;
    } else if (duration_remainder > 62000) {
        /* > 62 seconds -> resolution = 0b10 [10 seconds] */
        resolution = 0x02;
        steps = duration_remainder / 10000;
    } else if (duration_remainder > 6200) {
        /* > 6.2 seconds -> resolution = 0b01 [1 seconds] */
        resolution = 0x01;
        steps = duration_remainder / 1000;
    } else if (duration_remainder > 0) {
        /* <= 6.2 seconds -> resolution = 0b00 [100 ms] */
        resolution = 0x00;
        steps = duration_remainder / 100;
    } else {
        resolution = 0x00;
        steps = 0x00;
    }
      transition->remain_time = (resolution << 6) | steps;
}

static uint16_t constrain_target_value(uint16_t target_value)
{
    if (target_value > 0 && target_value < generic_state.state_prop->range_min) {
        target_value = generic_state.state_prop->range_min;
    } else if (target_value > generic_state.state_prop->range_max) {
        target_value = generic_state.state_prop->range_max;
    }

    return target_value;
}

static void constrain_delta_target_value(void)
{
    /* This is as per Mesh Model Specification 3.3.2.2.3 */
    if (generic_state.state_prop->target > 0 &&
        generic_state.state_prop->target < generic_state.state_prop->range_min) {
        if (generic_state.state_prop->delta < 0) {
            generic_state.state_prop->target = 0;
        } else {
            generic_state.state_prop->target = generic_state.state_prop->range_min;
        }
    } else if (generic_state.state_prop->target > generic_state.state_prop->range_max) {
        generic_state.state_prop->target = generic_state.state_prop->range_max;
    }
}

static void set_target(uint8_t type, void *dptr)
{
    switch (type) {
    case ONOFF: {
        uint8_t onoff;

        onoff = *((uint8_t *) dptr);
        if (onoff == STATE_OFF) {
            generic_state.state_prop->target = 0;
        } else if (onoff == STATE_ON) {
            generic_state.state_prop->target = 1;
        }
    }
    break;
    case LEVEL:
        generic_state.state_prop->target = *((int16_t *) dptr) - INT16_MIN;
        generic_state.state_prop->target = constrain_target_value(generic_state.state_prop->target);
        break;
    case DELTA_LEVEL:
        generic_state.state_prop->target =  *((int16_t *) dptr) - INT16_MIN;
        constrain_delta_target_value();
        break;
    case MOVE_STATE:
        generic_state.state_prop->target = *((uint16_t *) dptr);
        break;
    default:
        return;
    }
}

static bool set_transition_timeout(struct transition_t *transition)
{
    uint8_t steps_multiplier, resolution;

    resolution = (transition->trans_time >> 6);
    steps_multiplier = (transition->trans_time & 0x3F);
    if (steps_multiplier == 0) {
        return false;
    }

    switch (resolution) {
    case 0: /* 100ms */
        transition->hop_duration = 100;
        break;
    case 1: /* 1 second */
        transition->hop_duration = 1000;
        break;
    case 2: /* 10 seconds */
        transition->hop_duration = 10000;
        break;
    case 3: /* 10 minutes */
        transition->hop_duration = 600000;
        break;
    }

    transition->num_of_hops = steps_multiplier;
    transition->total_duration = transition->hop_duration * transition->num_of_hops;

    return true;
}

static void set_transition_values(uint8_t type)
{
    if (!set_transition_timeout(generic_state.transition)) {
        return;
    }

    if (generic_state.transition->type == MOVE_STATE) {
        generic_state.transition->quo_tt = generic_state.transition->total_duration;
        return;
    }

    generic_state.transition->quo_tt = generic_state.transition->hop_duration;

    switch (type) {
    case ONOFF:
    case LEVEL:
        if(generic_state.state_prop->current > generic_state.state_prop->target){
            generic_state.state_prop->delta =
                        ((generic_state.state_prop->current - generic_state.state_prop->target) /
                         generic_state.transition->num_of_hops);
        }
        else{generic_state.state_prop->delta =
                        ((generic_state.state_prop->target - generic_state.state_prop->current) /
                        generic_state.transition->num_of_hops);
        }

        break;
    default:
        return;
    }
}

static int get_current(uint8_t type)
{
    switch (type) {
    case ONOFF:
        if (generic_state.state_prop->current != 0) { //If current state is on, return ON
            return STATE_ON;
        } else {                                      //If current state is off, return ON if target is ON, else return OFF, As per SPEC 3.1.1.1
            if (generic_state.state_prop->target) {
                return STATE_ON;
            } else {
                return STATE_OFF;
            }
        }
    case LEVEL:
        return (int16_t) (generic_state.state_prop->current + INT16_MIN);
    default:
        return 0;
    }
}

static int get_target(uint8_t type)
{
    switch (type) {
    case ONOFF:
        if (generic_state.state_prop->target != 0) {
            return STATE_ON;
        } else {
            return STATE_OFF;
        }
    case LEVEL:
        return (int16_t) (generic_state.state_prop->target + INT16_MIN);
    default:
        return 0;
    }
}

static void start_onoff_delay_clock(struct bt_mesh_model *model)
{
    // Assign the input model to the clock argument model
    onoffDelayClk_evtData.model = model;

    if(!is_onoff_delay_clock_constructed)
    {
        Util_constructClock(&modelDelayClk, setDelay_clockHandler,
                           (generic_state.transition->delay)*5, 0, false, (UArg)&onoffDelayClk_evtData);
        is_onoff_delay_clock_constructed = true;
        Util_startClock(&modelDelayClk);
    }
    else
    {
        Util_restartClock(&modelDelayClk, (generic_state.transition->delay)*5);
    }
}

static void start_level_delay_clock(struct bt_mesh_model *model)
{
    // Assign the input model to the clock argument model
    levelDelayClk_evtData.model = model;

    if(!is_level_delay_clock_constructed)
    {
        Util_constructClock(&genericLevelDelayClk, genericLevelDelay_clockHandler,
                           (generic_state.transition->delay)*5, 0, false, (UArg)&levelDelayClk_evtData);
        is_level_delay_clock_constructed = true;
        Util_startClock(&genericLevelDelayClk);
     }
    else
    {
        Util_restartClock(&genericLevelDelayClk, (generic_state.transition->delay)*5);
    }
}

static void start_onoff_transition_clock(struct bt_mesh_model *model)
{
    // Get the current time, this is the transition start time
    generic_state.transition->start_timestamp = k_uptime_get();

    // Assign the input model to the clock argument model
    onoffTransClk_evtData.model = model;

    if(generic_state.state_prop->target == 0){ /*As per SPEC 3.1.1.1 */
        if(!is_off_clock_constructed){
            Util_constructClock(&modelSetOffClk, OnOffSetOff_clockHandler,
                                generic_state.transition->total_duration, 0, false, (UArg)&onoffTransClk_evtData);
            is_off_clock_constructed = true;
            Util_startClock(&modelSetOffClk);
        }else{
            Util_restartClock(&modelSetOffClk, generic_state.transition->total_duration);
        }
    }
    else if(generic_state.state_prop->target == 1){ /*As per SPEC 3.1.1.1 */
        generic_state.state_prop->current = generic_state.state_prop->target;
        if(!is_on_clock_constructed){
            Util_constructClock(&modelSetOnClk, OnOffSetOn_clockHandler,
                                            generic_state.transition->total_duration, 0, false, (UArg)&onoffTransClk_evtData);
            is_on_clock_constructed = true;
            Util_startClock(&modelSetOnClk);
        }else{
            Util_restartClock(&modelSetOnClk, generic_state.transition->total_duration);
        }
    }
}

static void start_level_transition_clock(struct bt_mesh_model *model)
{
    // Get the current time, this is the transition start time
    generic_state.transition->start_timestamp = k_uptime_get();

    // Assign the input model to the clock argument model
    levelTransClk_evtData.model = model;

    if(!is_level_trans_clock_constructed){
        Util_constructClock(&genericLevelSetClk, levelSet_clockHandler,
                            generic_state.transition->quo_tt, 0, false, (UArg)&levelTransClk_evtData);
        is_level_trans_clock_constructed = true;
        Util_startClock(&genericLevelSetClk);
    }else{
        Util_restartClock(&genericLevelSetClk, generic_state.transition->quo_tt);
    }
}

/* Clock handlers for generic OnOff: */
static void setDelay_clockHandler(UArg arg)
{
    modelClockEventData_t *clockArgs = (modelClockEventData_t *)arg;

    // Reset the delay value
    generic_state.transition->delay = 0;

    start_onoff_transition_clock(clockArgs->model);
}

/* As per SPEC 3.1.1.1 */
static void OnOffSetOff_clockHandler(UArg arg)
{
    modelClockEventData_t *clockArgs = (modelClockEventData_t *)arg;
    generic_state.state_prop->current = generic_state.state_prop->target;
    generic_state.transition->num_of_hops = 0; //After the transition has completed, there are no more hops in OnOff
    generic_state.transition->start_timestamp = 0;

    gen_onoff_publish(clockArgs->model);
}

/* As per SPEC 3.1.1.1 */
static void OnOffSetOn_clockHandler(UArg arg)
{
    modelClockEventData_t *clockArgs = (modelClockEventData_t *)arg;
    generic_state.transition->num_of_hops = 0; //After the transition has completed, there are no more hops in OnOff
    generic_state.transition->start_timestamp = 0;

    gen_onoff_publish(clockArgs->model);
}

/* Clock handlers for generic Level: */
static void genericLevelDelay_clockHandler(UArg arg)
{
    modelClockEventData_t *clockArgs = (modelClockEventData_t *)arg;

    // Reset the delay value
    generic_state.transition->delay = 0;

    start_level_transition_clock(clockArgs->model);
}

static void levelSet_clockHandler(UArg arg)
{
    modelClockEventData_t *clockArgs = (modelClockEventData_t *)arg;
    generic_state.transition->num_of_hops = generic_state.transition->num_of_hops - 1;

    generic_state.state_prop->current += generic_state.state_prop->delta; //Increment gradually

    if(generic_state.state_prop->current != generic_state.state_prop->target){  //Did not reach the target value yet
        Util_startClock(&genericLevelSetClk); //Set another gradual increment of current value
    }
    else{
        generic_state.transition->start_timestamp = 0;
        gen_level_publish(clockArgs->model); //Finished the transition, publish new value
    }
}

/* Generic Power OnOff Server message handlers */
void gen_OnPowerUp_get(struct bt_mesh_model *model,
                  struct bt_mesh_msg_ctx *ctx,
                  struct net_buf_simple *buf)
{
    struct genOnPowerUpState_t *data = (struct genOnPowerUpState_t *)model->user_data;
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 1 + 4);

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_OnPowerUp_Get callback from addr=0x%x", ctx->addr);
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_GEN_ONPOWERUP_STATUS);
    net_buf_simple_add_u8(msg, data->onPowerUp);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "Unable to send GEN_POWER_ONOFF_SRV Status response");
    }
}

/* Generic Power OnOff Client message handlers */
void gen_OnPowerUp_Status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct net_buf_simple *buf)
{
    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_OnPowerUp_Status callback from addr=0x%x", ctx->addr);
    SimpleMeshMenu_turnGenOnOff(net_buf_simple_pull_u8(buf));
}

/* Generic Power OnOff Setup Server message handlers */

void gen_onpowerup_publish(struct bt_mesh_model *model)
{
    int err;
    struct genOnPowerUpState_t *data = (struct genOnPowerUpState_t *)model->user_data;
    struct net_buf_simple *msg = model->pub->msg;

    if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_GEN_ONPOWERUP_STATUS);
    net_buf_simple_add_u8(msg, data->onPowerUp);

    err = bt_mesh_model_publish(model);
    if (err) {
        Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "bt_mesh_model_publish err %d\n", err);
    }
}

void gen_OnPowerUp_set_unack(struct bt_mesh_model *model,
                    struct bt_mesh_msg_ctx *ctx,
                    struct net_buf_simple *buf)
{
    struct genOnPowerUpState_t *data = (struct genOnPowerUpState_t *)model->user_data;
    uint8_t onPowerUp;

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_OnPowerUp_Set_Unack callback from addr=0x%x", ctx->addr);
    onPowerUp = net_buf_simple_pull_u8(buf);

    if (onPowerUp > STATE_RESTORE) {
        return;
    }

    if (data->onPowerUp != onPowerUp) {
        data->onPowerUp = onPowerUp;
#ifdef CONFIG_BT_SETTINGS
        store_data(model, data->onPowerUp);
#endif
        gen_onpowerup_publish(model);
    }
}

void gen_OnPowerUp_set(struct bt_mesh_model *model,
                  struct bt_mesh_msg_ctx *ctx,
                  struct net_buf_simple *buf)
{
    struct genOnPowerUpState_t *data = (struct genOnPowerUpState_t *)model->user_data;
    uint8_t onPowerUp;

    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: gen_OnPowerUp_Set callback from addr=0x%x", ctx->addr);
    onPowerUp = net_buf_simple_pull_u8(buf);

    if (onPowerUp > STATE_RESTORE) {
        return;
    }

    if (data->onPowerUp != onPowerUp) {
        data->onPowerUp = onPowerUp;
#ifdef CONFIG_BT_SETTINGS
        store_data(model, data->onPowerUp);
#endif
        gen_OnPowerUp_get(model, ctx, buf);
        gen_onpowerup_publish(model);
    } else {
        gen_OnPowerUp_get(model, ctx, buf);
    }
}

void gen_Battery_status(struct bt_mesh_model *model,
                 struct bt_mesh_msg_ctx *ctx,
                 struct net_buf_simple *buf)
{
    uint32_t data;
    struct batteryStatus_t pBatteryStatus;

    // if the buf length is not 8 the msg is invalid
    if (buf->len != 8) {
        Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "Invalid Generic Battery Status length %d", buf->len);
        return;
    }

    // pull the first 4 bytes which contain the level and time to discharge values
    data = net_buf_simple_pull_le32(buf);
    pBatteryStatus.battery_level = (uint8_t)data;
    pBatteryStatus.time_to_discharge = data>>8;
    // pull the last 4 bytes which contain the time to charge and flags values
    data = net_buf_simple_pull_le32(buf);
    pBatteryStatus.time_to_charge = data & 0xffffff;
    pBatteryStatus.flags = (uint8_t)(data>>24);

    // display the msg content in the menu
    SimpleMeshMenu_setGenBatteryLineBytes(pBatteryStatus.battery_level, pBatteryStatus.time_to_discharge,
                                          pBatteryStatus.time_to_charge, pBatteryStatus.flags);

}

static void getBatteryState(struct batteryStatus_t *battery_status)
{
    struct batteryStatus_t data = {
       .battery_level = 0x64,
       .time_to_discharge = 0xFEFFFF,
       .time_to_charge = 0x0,
       .flags = 0x5F
    };
    battery_status->battery_level = data.battery_level;
    battery_status->time_to_discharge = data.time_to_discharge;
    battery_status->time_to_charge = data.time_to_charge;
    battery_status->flags = data.flags;
}

void gen_Battery_get(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf)
{
    struct batteryStatus_t *data = model->user_data;
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 8 + 4);

    // Get the battery state
    getBatteryState(data);

    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS);
    // Add the battery_level and time_to_discharge values to the buffer
    net_buf_simple_add_le32(msg, data->time_to_discharge << 8 | data->battery_level);
    // Add the time_to_charge and flags values to the buffer
    net_buf_simple_add_le32(msg, data->flags << 24 | data->time_to_charge);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "Unable to send GEN_BATTERY Status response");
    }
}
