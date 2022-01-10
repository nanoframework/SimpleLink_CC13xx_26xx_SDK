/******************************************************************************

@file  ble_stack_api.h

@brief This file contains the ble stack wrapper abovr icall

Group: WCS, BTS
$Target Device: DEVICES $

******************************************************************************
$License: BSD3 2013 $
******************************************************************************
$Release Name: PACKAGE NAME $
$Release Date: PACKAGE RELEASE DATE $
*****************************************************************************/

#ifndef BLE_STACK_API_H
#define BLE_STACK_API_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <icall.h>
#include <bcomdef.h>
#include <icall_ble_api.h>

typedef ICall_EntityID          bleStack_entityId_t;
typedef ICall_Hdr               bleStack_msgHdt_t;
typedef ICall_Errno             bleStack_errno_t;

typedef struct
{
    uint8 pairMode;
    uint8 mitm;
    uint8 ioCap;
    uint8 bonding;
    uint8 secureConnection;
    uint8 authenPairingOnly;
    uint8 autoSyncWL;
    uint8 eccReGenPolicy;
    uint8 KeySize;
    uint8 removeLRUBond;
    uint8 KeyDistList;
    uint8 eccDebugKeys;
    uint8 eraseBondWhileInConn;
} GapBond_params_t;

#define bleStack_malloc         ICall_malloc
#define bleStack_free           ICall_free
#define bleStack_mallocMsg      ICall_mallocMsg
#define bleStack_freeMsg        ICall_freeMsg
#define BLE_STACK_ERRNO_SUCCESS ICALL_ERRNO_SUCCESS

#ifdef ICALL_NO_APP_EVENTS
bleStack_errno_t bleStack_register(uint8_t *selfEntity, appCallback_t appCallback);
#endif

void      bleStack_createTasks();
bStatus_t bleStack_initGap(uint8_t role, ICall_EntityID appSelfEntity, uint16_t paramUpdateDecision);
bStatus_t bleStack_initGapBond(GapBond_params_t *pGapBondParams, void *bleApp_bondMgrCBs);
bStatus_t bleStack_initGatt(uint8_t role, ICall_EntityID appSelfEntity, uint8_t *pAttDeviceName);

#ifdef __cplusplus
}
#endif

#endif /* BLE_STACK_API_H */
