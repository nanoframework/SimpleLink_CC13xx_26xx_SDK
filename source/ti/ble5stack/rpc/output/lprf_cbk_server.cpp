/*
 * Generated by erpcgen 1.7.3 on Tue Jan  5 17:50:11 2021.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#include "lprf_cbk_server.h"
#include <new>
#include "erpc_port.h"
#include "erpc_manually_constructed.h"

#if 10703 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif

using namespace erpc;
using namespace std;

#if ERPC_NESTED_CALLS_DETECTION
extern bool nestingDetection;
#endif

static ManuallyConstructed<BLEmesh_cbk_access_service> s_BLEmesh_cbk_access_service;

static ManuallyConstructed<BLEmesh_cbk_health_srv_service> s_BLEmesh_cbk_health_srv_service;

static ManuallyConstructed<BLEmesh_cbk_service> s_BLEmesh_cbk_service;


//! @brief Function to read struct bt_mesh_msg_ctx_raw
static void read_bt_mesh_msg_ctx_raw_struct(erpc::Codec * codec, bt_mesh_msg_ctx_raw * data);

//! @brief Function to read struct net_buf_simple_raw
static void read_net_buf_simple_raw_struct(erpc::Codec * codec, net_buf_simple_raw * data);


// Read struct bt_mesh_msg_ctx_raw function implementation
static void read_bt_mesh_msg_ctx_raw_struct(erpc::Codec * codec, bt_mesh_msg_ctx_raw * data)
{
    codec->read(&data->net_idx);

    codec->read(&data->app_idx);

    codec->read(&data->addr);

    codec->read(&data->recv_dst);

    codec->read(&data->recv_rssi);

    codec->read(&data->recv_ttl);

    codec->read(&data->send_rel);

    codec->read(&data->send_ttl);
}

// Read struct net_buf_simple_raw function implementation
static void read_net_buf_simple_raw_struct(erpc::Codec * codec, net_buf_simple_raw * data)
{
    uint32_t lengthTemp_0;
    codec->startReadList(&lengthTemp_0);
    data->len = lengthTemp_0;
    if (lengthTemp_0 > 0)
    {
        data->data = (uint8_t *) erpc_malloc(lengthTemp_0 * sizeof(uint8_t));
        if (data->data == NULL)
        {
            codec->updateStatus(kErpcStatus_MemoryError);
        }
        for (uint32_t listCount0 = 0; listCount0 < lengthTemp_0; ++listCount0)
        {
            codec->read(&data->data[listCount0]);
        }
    }
    else
    {
        data->data = NULL;
    }

    codec->read(&data->size);

    data->__buf = (uint8_t *) erpc_malloc(sizeof(uint8_t));
    if (data->__buf == NULL)
    {
        codec->updateStatus(kErpcStatus_MemoryError);
    }
    codec->read(&*data->__buf);
}


//! @brief Function to free space allocated inside struct net_buf_simple_raw
static void free_net_buf_simple_raw_struct(net_buf_simple_raw * data);


// Free space allocated inside struct net_buf_simple_raw function implementation
static void free_net_buf_simple_raw_struct(net_buf_simple_raw * data)
{
    if (data->data)
    {
        erpc_free(data->data);
    }


    if (data->__buf)
    {
        erpc_free(data->__buf);
    }
}



// Call the correct server shim based on method unique ID.
erpc_status_t BLEmesh_cbk_access_service::handleInvocation(uint32_t methodId, uint32_t sequence, Codec * codec, MessageBufferFactory *messageFactory)
{
    switch (methodId)
    {
#ifndef ERPC_REMOVE__settings_commit_cb
        case kBLEmesh_cbk_access_settings_commit_cb_id:
            return settings_commit_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__init_cb
        case kBLEmesh_cbk_access_init_cb_id:
            return init_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__reset_cb
        case kBLEmesh_cbk_access_reset_cb_id:
            return reset_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__update_cb
        case kBLEmesh_cbk_access_update_cb_id:
            return update_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__func_cb
        case kBLEmesh_cbk_access_func_cb_id:
            return func_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__hb_sub_cb
        case kBLEmesh_cbk_access_hb_sub_cb_id:
            return hb_sub_cb_shim(codec, messageFactory, sequence);
#endif

        default:
            return kErpcStatus_InvalidArgument;
    }
}

// Server shim for settings_commit_cb of BLEmesh_cbk_access interface.
#ifndef ERPC_REMOVE__settings_commit_cb
erpc_status_t BLEmesh_cbk_access_service::settings_commit_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t elem_idx;
    uint8_t is_vnd;
    uint16_t model_index;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&elem_idx);

    codec->read(&is_vnd);

    codec->read(&model_index);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        settings_commit_cb(elem_idx, is_vnd, model_index);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Server shim for init_cb of BLEmesh_cbk_access interface.
#ifndef ERPC_REMOVE__init_cb
erpc_status_t BLEmesh_cbk_access_service::init_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t elem_idx;
    uint8_t is_vnd;
    uint16_t model_index;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&elem_idx);

    codec->read(&is_vnd);

    codec->read(&model_index);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        init_cb(elem_idx, is_vnd, model_index);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Server shim for reset_cb of BLEmesh_cbk_access interface.
#ifndef ERPC_REMOVE__reset_cb
erpc_status_t BLEmesh_cbk_access_service::reset_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t elem_idx;
    uint8_t is_vnd;
    uint16_t model_index;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&elem_idx);

    codec->read(&is_vnd);

    codec->read(&model_index);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        reset_cb(elem_idx, is_vnd, model_index);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Server shim for update_cb of BLEmesh_cbk_access interface.
#ifndef ERPC_REMOVE__update_cb
erpc_status_t BLEmesh_cbk_access_service::update_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t elem_idx;
    uint8_t is_vnd;
    uint16_t model_index;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&elem_idx);

    codec->read(&is_vnd);

    codec->read(&model_index);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        update_cb(elem_idx, is_vnd, model_index);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Server shim for func_cb of BLEmesh_cbk_access interface.
#ifndef ERPC_REMOVE__func_cb
erpc_status_t BLEmesh_cbk_access_service::func_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint32_t opcode;
    uint16_t elem_idx;
    uint8_t is_vnd;
    uint16_t model_index;
    bt_mesh_msg_ctx_raw * ctx = NULL;
    net_buf_simple_raw * buf = NULL;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&opcode);

    codec->read(&elem_idx);

    codec->read(&is_vnd);

    codec->read(&model_index);

    uint32_t lengthTemp_0;
    codec->startReadList(&lengthTemp_0);
    if (lengthTemp_0 > 0)
    {
        ctx = (bt_mesh_msg_ctx_raw *) erpc_malloc(lengthTemp_0 * sizeof(bt_mesh_msg_ctx_raw));
        if (ctx == NULL)
        {
            codec->updateStatus(kErpcStatus_MemoryError);
        }
        for (uint32_t listCount0 = 0; listCount0 < lengthTemp_0; ++listCount0)
        {
            read_bt_mesh_msg_ctx_raw_struct(codec, &(ctx[listCount0]));
        }
    }
    else
    {
        ctx = NULL;
    }

    uint32_t lengthTemp_1;
    codec->startReadList(&lengthTemp_1);
    if (lengthTemp_1 > 0)
    {
        buf = (net_buf_simple_raw *) erpc_malloc(lengthTemp_1 * sizeof(net_buf_simple_raw));
        if (buf == NULL)
        {
            codec->updateStatus(kErpcStatus_MemoryError);
        }
        for (uint32_t listCount1 = 0; listCount1 < lengthTemp_1; ++listCount1)
        {
            read_net_buf_simple_raw_struct(codec, &(buf[listCount1]));
        }
    }
    else
    {
        buf = NULL;
    }

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        func_cb(opcode, elem_idx, is_vnd, model_index, ctx, buf);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    if (ctx)
    {
        erpc_free(ctx);
    }

    for (uint32_t listCount1 = 0; listCount1 < 1; ++listCount1)
    {
        free_net_buf_simple_raw_struct(&buf[listCount1]);
    }

    if (buf)
    {
        erpc_free(buf);
    }

    return err;
}
#endif

// Server shim for hb_sub_cb of BLEmesh_cbk_access interface.
#ifndef ERPC_REMOVE__hb_sub_cb
erpc_status_t BLEmesh_cbk_access_service::hb_sub_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint8_t hops;
    uint16_t feat;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&hops);

    codec->read(&feat);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        hb_sub_cb(hops, feat);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Call the correct server shim based on method unique ID.
erpc_status_t BLEmesh_cbk_health_srv_service::handleInvocation(uint32_t methodId, uint32_t sequence, Codec * codec, MessageBufferFactory *messageFactory)
{
    switch (methodId)
    {
#ifndef ERPC_REMOVE__fault_get_cur_cb
        case kBLEmesh_cbk_health_srv_fault_get_cur_cb_id:
            return fault_get_cur_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__fault_get_reg_cb
        case kBLEmesh_cbk_health_srv_fault_get_reg_cb_id:
            return fault_get_reg_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__fault_clear_cb
        case kBLEmesh_cbk_health_srv_fault_clear_cb_id:
            return fault_clear_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__fault_test_cb
        case kBLEmesh_cbk_health_srv_fault_test_cb_id:
            return fault_test_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__attn_on_cb
        case kBLEmesh_cbk_health_srv_attn_on_cb_id:
            return attn_on_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__attn_off_cb
        case kBLEmesh_cbk_health_srv_attn_off_cb_id:
            return attn_off_cb_shim(codec, messageFactory, sequence);
#endif

        default:
            return kErpcStatus_InvalidArgument;
    }
}

// Server shim for fault_get_cur_cb of BLEmesh_cbk_health_srv interface.
#ifndef ERPC_REMOVE__fault_get_cur_cb
erpc_status_t BLEmesh_cbk_health_srv_service::fault_get_cur_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t elem_idx;
    uint16_t model_index;
    uint8_t test_id;
    uint16_t company_id;
    uint8_t * faults = NULL;
    uint8_t fault_count;
    int32_t result;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&elem_idx);

    codec->read(&model_index);

    faults = (uint8_t *) erpc_malloc(256 * sizeof(uint8_t));
    if (faults == NULL)
    {
        codec->updateStatus(kErpcStatus_MemoryError);
    }

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        result = fault_get_cur_cb(elem_idx, model_index, &test_id, &company_id, faults, &fault_count);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (!err)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kBLEmesh_cbk_health_srv_service_id, kBLEmesh_cbk_health_srv_fault_get_cur_cb_id, sequence);

        codec->write(test_id);

        codec->write(company_id);

        codec->startWriteList(fault_count);
        for (uint32_t listCount0 = 0; listCount0 < fault_count; ++listCount0)
        {
            codec->write(faults[listCount0]);
        }

        codec->write(result);

        err = codec->getStatus();
    }

    if (faults)
    {
        erpc_free(faults);
    }

    return err;
}
#endif

// Server shim for fault_get_reg_cb of BLEmesh_cbk_health_srv interface.
#ifndef ERPC_REMOVE__fault_get_reg_cb
erpc_status_t BLEmesh_cbk_health_srv_service::fault_get_reg_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t elem_idx;
    uint16_t model_index;
    uint16_t company_id;
    uint8_t test_id;
    uint8_t * faults = NULL;
    uint8_t fault_count;
    int32_t result;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&elem_idx);

    codec->read(&model_index);

    codec->read(&company_id);

    faults = (uint8_t *) erpc_malloc(256 * sizeof(uint8_t));
    if (faults == NULL)
    {
        codec->updateStatus(kErpcStatus_MemoryError);
    }

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        result = fault_get_reg_cb(elem_idx, model_index, company_id, &test_id, faults, &fault_count);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (!err)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kBLEmesh_cbk_health_srv_service_id, kBLEmesh_cbk_health_srv_fault_get_reg_cb_id, sequence);

        codec->write(test_id);

        codec->startWriteList(fault_count);
        for (uint32_t listCount0 = 0; listCount0 < fault_count; ++listCount0)
        {
            codec->write(faults[listCount0]);
        }

        codec->write(result);

        err = codec->getStatus();
    }

    if (faults)
    {
        erpc_free(faults);
    }

    return err;
}
#endif

// Server shim for fault_clear_cb of BLEmesh_cbk_health_srv interface.
#ifndef ERPC_REMOVE__fault_clear_cb
erpc_status_t BLEmesh_cbk_health_srv_service::fault_clear_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t elem_idx;
    uint16_t model_index;
    uint16_t company_id;
    int32_t result;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&elem_idx);

    codec->read(&model_index);

    codec->read(&company_id);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        result = fault_clear_cb(elem_idx, model_index, company_id);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (!err)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kBLEmesh_cbk_health_srv_service_id, kBLEmesh_cbk_health_srv_fault_clear_cb_id, sequence);

        codec->write(result);

        err = codec->getStatus();
    }

    return err;
}
#endif

// Server shim for fault_test_cb of BLEmesh_cbk_health_srv interface.
#ifndef ERPC_REMOVE__fault_test_cb
erpc_status_t BLEmesh_cbk_health_srv_service::fault_test_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t elem_idx;
    uint16_t model_index;
    uint8_t test_id;
    uint16_t company_id;
    int32_t result;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&elem_idx);

    codec->read(&model_index);

    codec->read(&test_id);

    codec->read(&company_id);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        result = fault_test_cb(elem_idx, model_index, test_id, company_id);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (!err)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kBLEmesh_cbk_health_srv_service_id, kBLEmesh_cbk_health_srv_fault_test_cb_id, sequence);

        codec->write(result);

        err = codec->getStatus();
    }

    return err;
}
#endif

// Server shim for attn_on_cb of BLEmesh_cbk_health_srv interface.
#ifndef ERPC_REMOVE__attn_on_cb
erpc_status_t BLEmesh_cbk_health_srv_service::attn_on_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t elem_idx;
    uint16_t model_index;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&elem_idx);

    codec->read(&model_index);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        attn_on_cb(elem_idx, model_index);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Server shim for attn_off_cb of BLEmesh_cbk_health_srv interface.
#ifndef ERPC_REMOVE__attn_off_cb
erpc_status_t BLEmesh_cbk_health_srv_service::attn_off_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t elem_idx;
    uint16_t model_index;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&elem_idx);

    codec->read(&model_index);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        attn_off_cb(elem_idx, model_index);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Call the correct server shim based on method unique ID.
erpc_status_t BLEmesh_cbk_service::handleInvocation(uint32_t methodId, uint32_t sequence, Codec * codec, MessageBufferFactory *messageFactory)
{
    switch (methodId)
    {
#ifndef ERPC_REMOVE__output_number_cb
        case kBLEmesh_cbk_output_number_cb_id:
            return output_number_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__output_string_cb
        case kBLEmesh_cbk_output_string_cb_id:
            return output_string_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__input_cb
        case kBLEmesh_cbk_input_cb_id:
            return input_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__input_complete_cb
        case kBLEmesh_cbk_input_complete_cb_id:
            return input_complete_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__unprovisioned_beacon_cb
        case kBLEmesh_cbk_unprovisioned_beacon_cb_id:
            return unprovisioned_beacon_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__link_open_cb
        case kBLEmesh_cbk_link_open_cb_id:
            return link_open_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__link_close_cb
        case kBLEmesh_cbk_link_close_cb_id:
            return link_close_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__complete_cb
        case kBLEmesh_cbk_complete_cb_id:
            return complete_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__node_added_cb
        case kBLEmesh_cbk_node_added_cb_id:
            return node_added_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__reset_prov_cb
        case kBLEmesh_cbk_reset_prov_cb_id:
            return reset_prov_cb_shim(codec, messageFactory, sequence);
#endif

#ifndef ERPC_REMOVE__lpn_set_cb
        case kBLEmesh_cbk_lpn_set_cb_id:
            return lpn_set_cb_shim(codec, messageFactory, sequence);
#endif

        default:
            return kErpcStatus_InvalidArgument;
    }
}

// Server shim for output_number_cb of BLEmesh_cbk interface.
#ifndef ERPC_REMOVE__output_number_cb
erpc_status_t BLEmesh_cbk_service::output_number_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    bt_mesh_output_action act;
    uint32_t num;
    int32_t _tmp_local;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&_tmp_local);
    act = static_cast<bt_mesh_output_action>(_tmp_local);

    codec->read(&num);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        output_number_cb(act, num);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Server shim for output_string_cb of BLEmesh_cbk interface.
#ifndef ERPC_REMOVE__output_string_cb
erpc_status_t BLEmesh_cbk_service::output_string_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    char * str = NULL;

    // startReadMessage() was already called before this shim was invoked.

    uint32_t str_len;
    char * str_local;
    codec->readString(&str_len, &str_local);
    str = (char *) erpc_malloc((str_len + 1) * sizeof(char));
    if (str == NULL)
    {
        codec->updateStatus(kErpcStatus_MemoryError);
    }
    else
    {
        memcpy(str, str_local, str_len);
        (str)[str_len] = 0;
    }

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        output_string_cb(str);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    if (str)
    {
        erpc_free(str);
    }

    return err;
}
#endif

// Server shim for input_cb of BLEmesh_cbk interface.
#ifndef ERPC_REMOVE__input_cb
erpc_status_t BLEmesh_cbk_service::input_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    bt_mesh_input_action act;
    uint8_t size;
    int32_t _tmp_local;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&_tmp_local);
    act = static_cast<bt_mesh_input_action>(_tmp_local);

    codec->read(&size);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        input_cb(act, size);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Server shim for input_complete_cb of BLEmesh_cbk interface.
#ifndef ERPC_REMOVE__input_complete_cb
erpc_status_t BLEmesh_cbk_service::input_complete_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    // startReadMessage() was already called before this shim was invoked.

    // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = true;
#endif
    input_complete_cb();
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = false;
#endif
    return codec->getStatus();
}
#endif

// Server shim for unprovisioned_beacon_cb of BLEmesh_cbk interface.
#ifndef ERPC_REMOVE__unprovisioned_beacon_cb
erpc_status_t BLEmesh_cbk_service::unprovisioned_beacon_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint8_t uuid[16];
    bt_mesh_prov_oob_info oob_info;
    uint32_t * uri_hash = NULL;
    int32_t _tmp_local;

    // startReadMessage() was already called before this shim was invoked.

    for (uint32_t arrayCount0 = 0; arrayCount0 < 16; ++arrayCount0)
    {
        codec->read(&uuid[arrayCount0]);
    }

    codec->read(&_tmp_local);
    oob_info = static_cast<bt_mesh_prov_oob_info>(_tmp_local);

    uint32_t lengthTemp_0;
    codec->startReadList(&lengthTemp_0);
    if (lengthTemp_0 > 0)
    {
        uri_hash = (uint32_t *) erpc_malloc(lengthTemp_0 * sizeof(uint32_t));
        if (uri_hash == NULL)
        {
            codec->updateStatus(kErpcStatus_MemoryError);
        }
        for (uint32_t listCount0 = 0; listCount0 < lengthTemp_0; ++listCount0)
        {
            codec->read(&uri_hash[listCount0]);
        }
    }
    else
    {
        uri_hash = NULL;
    }

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        unprovisioned_beacon_cb(uuid, oob_info, uri_hash);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    if (uri_hash)
    {
        erpc_free(uri_hash);
    }

    return err;
}
#endif

// Server shim for link_open_cb of BLEmesh_cbk interface.
#ifndef ERPC_REMOVE__link_open_cb
erpc_status_t BLEmesh_cbk_service::link_open_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    bt_mesh_prov_bearer bearer;
    int32_t _tmp_local;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&_tmp_local);
    bearer = static_cast<bt_mesh_prov_bearer>(_tmp_local);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        link_open_cb(bearer);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Server shim for link_close_cb of BLEmesh_cbk interface.
#ifndef ERPC_REMOVE__link_close_cb
erpc_status_t BLEmesh_cbk_service::link_close_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    bt_mesh_prov_bearer bearer;
    int32_t _tmp_local;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&_tmp_local);
    bearer = static_cast<bt_mesh_prov_bearer>(_tmp_local);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        link_close_cb(bearer);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Server shim for complete_cb of BLEmesh_cbk interface.
#ifndef ERPC_REMOVE__complete_cb
erpc_status_t BLEmesh_cbk_service::complete_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t net_idx;
    uint16_t addr;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&net_idx);

    codec->read(&addr);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        complete_cb(net_idx, addr);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Server shim for node_added_cb of BLEmesh_cbk interface.
#ifndef ERPC_REMOVE__node_added_cb
erpc_status_t BLEmesh_cbk_service::node_added_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t net_idx;
    uint16_t addr;
    uint8_t num_elem;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&net_idx);

    codec->read(&addr);

    codec->read(&num_elem);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        node_added_cb(net_idx, addr, num_elem);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

// Server shim for reset_prov_cb of BLEmesh_cbk interface.
#ifndef ERPC_REMOVE__reset_prov_cb
erpc_status_t BLEmesh_cbk_service::reset_prov_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    // startReadMessage() was already called before this shim was invoked.

    // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = true;
#endif
    reset_prov_cb();
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = false;
#endif
    return codec->getStatus();
}
#endif

// Server shim for lpn_set_cb of BLEmesh_cbk interface.
#ifndef ERPC_REMOVE__lpn_set_cb
erpc_status_t BLEmesh_cbk_service::lpn_set_cb_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint16_t friend_addr;
    bool established;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&friend_addr);

    codec->read(&established);

    err = codec->getStatus();
    if (!err)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        lpn_set_cb(friend_addr, established);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}
#endif

erpc_service_t create_BLEmesh_cbk_access_service()
{
    s_BLEmesh_cbk_access_service.construct();
    return s_BLEmesh_cbk_access_service.get();
}

void destroy_BLEmesh_cbk_access_service()
{
    s_BLEmesh_cbk_access_service.destroy();
}

erpc_service_t create_BLEmesh_cbk_health_srv_service()
{
    s_BLEmesh_cbk_health_srv_service.construct();
    return s_BLEmesh_cbk_health_srv_service.get();
}

void destroy_BLEmesh_cbk_health_srv_service()
{
    s_BLEmesh_cbk_health_srv_service.destroy();
}

erpc_service_t create_BLEmesh_cbk_service()
{
    s_BLEmesh_cbk_service.construct();
    return s_BLEmesh_cbk_service.get();
}

void destroy_BLEmesh_cbk_service()
{
    s_BLEmesh_cbk_service.destroy();
}