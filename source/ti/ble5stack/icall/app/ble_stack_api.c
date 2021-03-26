/******************************************************************************

@file  ble_stack_api.c

@brief This file contains the ble stack wrapper abovr icall

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

#include "ble_stack_api.h"
#include <icall_ble_api.h>
#include <gapbondmgr.h>

#ifdef ICALL_NO_APP_EVENTS
bleStack_errno_t bleStack_register(uint8_t *selfEntity, appCallback_t appCallback)
{
  ICall_EntityID   localSelfEntity;
  ICall_Errno status;
  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  status = ICall_registerAppCback(&localSelfEntity, appCallback);

  // applicatios should use the task entity ID
  *selfEntity = localSelfEntity;

  return status;
}
#endif // ICALL_NO_APP_EVENTS

void bleStack_createTasks()
{
    /* Initialize ICall module */
    ICall_init();

    /* Start tasks of external images - Priority 5 */
    ICall_createRemoteTasks();

}

int bleStack_initGap(uint8_t role, ICall_EntityID appSelfEntity, uint16_t paramUpdateDecision, void *bleApp_bondMgrCBs)
{
  // Pass all parameter update requests to the app for it to decide
  GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, paramUpdateDecision);

#if 0
  // Setup the GAP Bond Manager. For more information see the GAP Bond Manager
  // section in the User's Guide
  // ToDo - remove implementation from SysConfig
  // ToDo - set one call _all instaead of many calls to GAPBondMgr_SetParameter
  setBondManagerParameters();
#endif

  // Start Bond Manager and register callback
  VOID GAPBondMgr_Register((gapBondCBs_t *)bleApp_bondMgrCBs);

  // Register with GAP for HCI/Host messages. This is needed to receive HCI
  // events. For more information, see the HCI section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  GAP_RegisterForMsgs(appSelfEntity);

  return 0;
}

int bleStack_initGatt(uint8_t role, ICall_EntityID appSelfEntity, uint8_t *pAttDeviceName)
{
  // Set the Device Name characteristic in the GAP GATT Service
  // For more information, see the section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, (void *)pAttDeviceName);

  // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);           // GAP GATT Service
  GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT Service

  // Register for GATT local events and ATT Responses pending for transmission
  GATT_RegisterForMsgs(appSelfEntity);

  if (role & (GAP_PROFILE_PERIPHERAL | GAP_PROFILE_BROADCASTER))
  // Set default values for Data Length Extension
  // Extended Data Length Feature is already enabled by default
  {
    // Set initial values to maximum
    #define BLEAPP_SUGGESTED_PDU_SIZE 251
    #define BLEAPP_SUGGESTED_TX_TIME 2120 //default is 328us(TX)

    // This API is documented in hci.h
    // See the LE Data Length Extension section in the BLE5-Stack User's Guide for information on using this command:
    // http://software-dl.ti.com/lprf/ble5stack-latest/
    HCI_LE_WriteSuggestedDefaultDataLenCmd(BLEAPP_SUGGESTED_PDU_SIZE, BLEAPP_SUGGESTED_TX_TIME);
  }
  else
  //Set default values for Data Length Extension
  //Extended Data Length Feature is already enabled by default
  //in build_config.opt in stack project.
  {
    //Change initial values of RX/TX PDU and Time, RX is set to max. by default(251 octets, 2120us)
    #define APP_SUGGESTED_RX_PDU_SIZE 251     //default is 251 octets(RX)
    #define APP_SUGGESTED_RX_TIME     17000   //default is 17000us(RX)
    #define APP_SUGGESTED_TX_PDU_SIZE 27      //default is 27 octets(TX)
    #define APP_SUGGESTED_TX_TIME     328     //default is 328us(TX)

    //This API is documented in hci.h
    //See the LE Data Length Extension section in the BLE5-Stack User's Guide for information on using this command:
    //http://software-dl.ti.com/lprf/ble5stack-latest/
    HCI_EXT_SetMaxDataLenCmd(APP_SUGGESTED_TX_PDU_SIZE, APP_SUGGESTED_TX_TIME, APP_SUGGESTED_RX_PDU_SIZE, APP_SUGGESTED_RX_TIME);
  }

  // Initialize GATT Client
  GATT_InitClient();

  if (role & (GAP_PROFILE_CENTRAL | GAP_PROFILE_OBSERVER))
  {
      // Register to receive incoming ATT Indications/Notifications
      GATT_RegisterForInd(appSelfEntity);
  }

  return 0;
}
