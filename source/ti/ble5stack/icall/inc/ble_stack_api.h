/******************************************************************************

@file  ble_stack_api.h

@brief This file contains the ble stack wrapper abovr icall

Group: WCS, BTS
Target Device: cc13xx_cc26xx

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
