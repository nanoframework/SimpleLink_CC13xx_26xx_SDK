/*
 * Generated by erpcgen 1.7.3 on Tue Jan  5 17:50:11 2021.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#if !defined(_lprf_cbk_h_)
#define _lprf_cbk_h_

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#pragma anon_unions
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "erpc_version.h"
#include "mesh_erpc_wrapper.h"
#include "zephyr/types.h"
#include "bluetooth/mesh/main.h"

#if 10703 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif

#if !defined(ERPC_TYPE_DEFINITIONS)
#define ERPC_TYPE_DEFINITIONS

#endif // ERPC_TYPE_DEFINITIONS

/*! @brief BLEmesh_cbk_access identifiers */
enum _BLEmesh_cbk_access_ids
{
    kBLEmesh_cbk_access_service_id = 1,
    kBLEmesh_cbk_access_settings_commit_cb_id = 1,
    kBLEmesh_cbk_access_init_cb_id = 2,
    kBLEmesh_cbk_access_reset_cb_id = 3,
    kBLEmesh_cbk_access_update_cb_id = 4,
    kBLEmesh_cbk_access_func_cb_id = 5,
    kBLEmesh_cbk_access_hb_sub_cb_id = 6,
};

/*! @brief BLEmesh_cbk_health_srv identifiers */
enum _BLEmesh_cbk_health_srv_ids
{
    kBLEmesh_cbk_health_srv_service_id = 2,
    kBLEmesh_cbk_health_srv_fault_get_cur_cb_id = 1,
    kBLEmesh_cbk_health_srv_fault_get_reg_cb_id = 2,
    kBLEmesh_cbk_health_srv_fault_clear_cb_id = 3,
    kBLEmesh_cbk_health_srv_fault_test_cb_id = 4,
    kBLEmesh_cbk_health_srv_attn_on_cb_id = 5,
    kBLEmesh_cbk_health_srv_attn_off_cb_id = 6,
};

/*! @brief BLEmesh_cbk identifiers */
enum _BLEmesh_cbk_ids
{
    kBLEmesh_cbk_service_id = 4,
    kBLEmesh_cbk_output_number_cb_id = 1,
    kBLEmesh_cbk_output_string_cb_id = 2,
    kBLEmesh_cbk_input_cb_id = 3,
    kBLEmesh_cbk_input_complete_cb_id = 4,
    kBLEmesh_cbk_unprovisioned_beacon_cb_id = 5,
    kBLEmesh_cbk_link_open_cb_id = 6,
    kBLEmesh_cbk_link_close_cb_id = 7,
    kBLEmesh_cbk_complete_cb_id = 8,
    kBLEmesh_cbk_node_added_cb_id = 9,
    kBLEmesh_cbk_reset_prov_cb_id = 10,
    kBLEmesh_cbk_lpn_set_cb_id = 11,
};

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__cplusplus)
}
#endif

#endif // _lprf_cbk_h_
