/******************************************************************************

@file  ble_stack_api.c

@brief This file contains the ble stack wrapper abovr icall

Group: WCS, BTS
$Target Device: DEVICES $

******************************************************************************
$License: BSD3 2013 $
******************************************************************************
$Release Name: PACKAGE NAME $
$Release Date: PACKAGE RELEASE DATE $
*****************************************************************************/

#include "ble_stack_api.h"
#include <icall_ble_api.h>
#include "ti_ble_config.h"
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

bStatus_t bleStack_initGap(uint8_t role, ICall_EntityID appSelfEntity, uint16_t paramUpdateDecision)
{
  if (role & (GAP_PROFILE_PERIPHERAL | GAP_PROFILE_CENTRAL))
  {
      // Pass all parameter update requests to the app for it to decide
      GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, paramUpdateDecision);
  }

  // Register with GAP for HCI/Host messages. This is needed to receive HCI
  // events. For more information, see the HCI section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  GAP_RegisterForMsgs(appSelfEntity);

  return SUCCESS;
}

void bleStack_initGapBondParams(GapBond_params_t *pGapBondParams)
{
    // Set Pairing Mode
    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pGapBondParams->pairMode);
    // Set MITM Protection
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &pGapBondParams->mitm);
    // Set IO Capabilities
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &pGapBondParams->ioCap);
    // Set Bonding
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &pGapBondParams->bonding);
    // Set Secure Connection Usage during Pairing
    GAPBondMgr_SetParameter(GAPBOND_SECURE_CONNECTION, sizeof(uint8_t), &pGapBondParams->secureConnection);
    // Set Authenticated Pairing Only mode
    GAPBondMgr_SetParameter(GAPBOND_AUTHEN_PAIRING_ONLY, sizeof(uint8_t), &pGapBondParams->authenPairingOnly);
    // Set Auto Whitelist Sync
    GAPBondMgr_SetParameter(GAPBOND_AUTO_SYNC_WL, sizeof(uint8_t), &pGapBondParams->autoSyncWL);
    // Set ECC Key Regeneration Policy
    GAPBondMgr_SetParameter(GAPBOND_ECCKEY_REGEN_POLICY, sizeof(uint8_t), &pGapBondParams->eccReGenPolicy);
    // Set Key Size used in pairing
    GAPBondMgr_SetParameter(GAPBOND_KEYSIZE, sizeof(uint8_t), &pGapBondParams->KeySize);
    // Set LRU Bond Replacement Scheme
    GAPBondMgr_SetParameter(GAPBOND_LRU_BOND_REPLACEMENT, sizeof(uint8_t), &pGapBondParams->removeLRUBond);
    // Set Key Distribution list for pairing
    GAPBondMgr_SetParameter(GAPBOND_KEY_DIST_LIST, sizeof(uint8_t), &pGapBondParams->KeyDistList);
    // Set Secure Connection Debug Keys
    GAPBondMgr_SetParameter(GAPBOND_SC_HOST_DEBUG, sizeof(uint8_t), &pGapBondParams->eccDebugKeys);
    // Set the Erase bond While in Active Connection Flag
    GAPBondMgr_SetParameter(GAPBOND_ERASE_BOND_IN_CONN, sizeof(uint8_t), &pGapBondParams->eraseBondWhileInConn);
}

bStatus_t bleStack_initGapBond(GapBond_params_t *pGapBondParams, void *bleApp_bondMgrCBs)
{
#if defined ( GAP_BOND_MGR )
    if (pGapBondParams == NULL)
    {
        // Setup the GAP Bond Manager. For more information see the GAP Bond Manager
        // section in the User's Guide
        // Todo: - remove setBondManagerParameters implementation from SysConfig
        // Todo: - set one call _all instaead of many calls to GAPBondMgr_SetParameter
        setBondManagerParameters();
    }
    else
    {
        bleStack_initGapBondParams(pGapBondParams);
    }

    if (bleApp_bondMgrCBs != NULL)
    {
        // Start Bond Manager and register callback
        VOID GAPBondMgr_Register((gapBondCBs_t *)bleApp_bondMgrCBs);
    }
#endif // GAP_BOND_MGR
    return SUCCESS;
}

bStatus_t bleStack_initGatt(uint8_t role, ICall_EntityID appSelfEntity, uint8_t *pAttDeviceName)
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
  GATT_InitClient("");

  if (role & (GAP_PROFILE_CENTRAL | GAP_PROFILE_OBSERVER))
  {
      // Register to receive incoming ATT Indications/Notifications
      GATT_RegisterForInd(appSelfEntity);
  }

  return SUCCESS;
}
