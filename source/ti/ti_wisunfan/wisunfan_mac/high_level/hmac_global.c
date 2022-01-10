/******************************************************************************

 @file  hmac_global.c

 @brief This file contains the externs for FH ROM API initialization.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2021, Texas Instruments Incorporated
 All rights reserved.

 IMPORTANT: Your use of this Software is limited to those specific rights
 granted under the terms of a software license agreement between the user
 who downloaded the software, his/her employer (which must be your employer)
 and Texas Instruments Incorporated (the "License"). You may not use this
 Software unless you agree to abide by the terms of the License. The License
 limits your use, and you acknowledge, that the Software may not be modified,
 copied or distributed unless embedded on a Texas Instruments microcontroller
 or used solely and exclusively in conjunction with a Texas Instruments radio
 frequency transceiver, which is integrated into your product. Other than for
 the foregoing purpose, you may not use, reproduce, copy, prepare derivative
 works of, modify, distribute, perform, display or sell this Software and/or
 its documentation for any purpose.

 YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
 PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
 NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
 TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
 NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
 LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
 INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
 OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
 OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

 Should you have any questions regarding your right to use this Software,
 contact Texas Instruments Incorporated at www.TI.com.

 ******************************************************************************
 
 
 *****************************************************************************/

/*******************************************************************************
 * INCLUDES
 */

#ifndef LIBRARY
#include "ti_wisunfan_features.h"
#endif

#include <stddef.h>
#include "timac_api.h"
#include "mac_main.h"
#include "hal_types.h"
#include "mac_coord.h"
#include "mac_data.h"
#include "mac_device.h"
#include "mac_beacon.h"
#include "mac_beacon_device.h"
#include "mac_beacon_coord.h"
#include "mac_mgmt.h"
#include "mac_pib.h"
#include "mac_scan.h"
#include "mac_spec.h"
#include "mac_radio.h"
#include "mac_security_pib.h"
#include <crypto_mac_api.h>
#include "mac_pwr.h"
#include "mac_timer.h"

//#ifdef FEATURE_BEACON_MODE
/* Coordinator action set 1 */
macAction_t macBeaconCoordAction1[2];

macBeaconCoord_t macBeaconCoord;
//#endif

//#if defined (FEATURE_BEACON_MODE) || defined (FEATURE_NON_BEACON_MODE)
/* Device beacon action function table */
macAction_t macBeaconDeviceAction[4];

macBeaconDevice_t macBeaconDevice;
//#endif

/* mac_beacon data */
macBeacon_t macBeacon;

/* Coordinator action set 1 */
macAction_t macCoordAction1[2];

/* Coordinator action set 2 */
macAction_t macCoordAction2[7];

/* Coordinator action set 3 */
macAction_t macCoordAction3[4];

/* Coordinator info */
macCoord_t  macCoord;

/* Table of which IE's to be included for the specific command indexed by command identifier */
const uint32 macCmdIncludeFHIe [] =
{
  0,                                             /* MAC_INTERNAL_BEACON */
  0,                                             /* MAC_INTERNAL_ASYCH_REQ */
  0,                                             /* MAC_INTERNAL_ASYCH_REQ */
  0,                                             /* MAC_INTERNAL_ASYCH_REQ */
  MAC_FH_UT_IE + MAC_FH_RSL_IE + MAC_FH_BT_IE,   /* MAC_INTERNAL_ENH_ACK */
  MAC_FH_FC_IE + MAC_FH_UT_IE + MAC_FH_BT_IE     /* MAC_INTERNAL_EDFE_EFRM */
};

/* Table of command frame sizes indexed by command identifier */
const uint8 macCmdBufSize[] =
{
  0,                                                                    /* MAC_INTERNAL_BEACON */
  0,        /* MAC_INTERNAL_ZERO_DATA */
  0,        /* MAC_INTERNAL_ASSOC_REQ */
  MAC_GTS_REQ_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),          /* MAC_INTERNAL_ASYCH_REQ */
  MAC_ENH_ACK_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),          /* MAC_INTERNAL_ENH_ACK */
  MAC_EDFE_EFRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t)             /* MAC_INTERNAL_EDFE_EFRM */
};

/* Table of MAC command payload length by command identifier */
const uint8 macCmdLen[] =
{
  MAC_ASSOC_REQ_PAYLOAD,              /* MAC_ASSOC_REQ_FRAME */
  MAC_ASSOC_RSP_PAYLOAD,              /* MAC_ASSOC_RSP_FRAME */
  MAC_DISASSOC_NOTIF_PAYLOAD,         /* MAC_DISASSOC_NOTIF_FRAME */
  MAC_DATA_REQ_PAYLOAD,               /* MAC_DATA_REQ_FRAME */
  MAC_PAN_CONFLICT_PAYLOAD,           /* MAC_PAN_CONFLICT_FRAME */
  MAC_ORPHAN_NOTIF_PAYLOAD,           /* MAC_ORPHAN_NOTIF_FRAME */
  MAC_BEACON_REQ_PAYLOAD,             /* MAC_BEACON_REQ_FRAME */
  MAC_COORD_REALIGN_PAYLOAD,          /* MAC_COORD_REALIGN_FRAME */
  MAC_GTS_REQ_PAYLOAD                 /* MAC_GTS_REQ_FRAME */
};


/* TX frame success to event */
const uint8 macTxFrameSuccess[] =
{
  MAC_NO_ACTION_EVT                   /* MAC_INTERNAL_ENH_ACK */
};

/* TX frame failure to event */
const uint8 macTxFrameFailed[] =
{
  MAC_NO_ACTION_EVT                   /* MAC_INTERNAL_ENH_ACK */
};

/* mac_data data */
macData_t macData;

/* pointer to current tx frame */
macTx_t *pMacDataTx = NULL;

/* indirect data handling functions */
macDataTxFcn_t macDataTxIndirect = NULL;
macDataTxFcn_t macDataRequeueIndirect = NULL;

/* critical beacon handling function */
macRxBeaconCritical_t macDataRxBeaconCritical = NULL;

/* beacon tx complete function */
macTxBeaconComplete_t macDataTxBeaconComplete = NULL;

/* tx frame set schedule function */
macDataTxFcn_t macDataSetSched = NULL;

/* tx frame check schedule function */
macDataCheckSched_t macDataCheckSched = NULL;

/* tx frame check tx time function */
macDataCheckTxTime_t macDataCheckTxTime = NULL;

/* tx frame beacon requeue function */
macDataTxFcn_t macDataBeaconRequeue = NULL;

/* tx timer for Green Power delayed transmission */
macTimer_t macDataTxDelay;

/* Device action set 1 */
macAction_t macDeviceAction1[14];

/* Device action set 2 */
macAction_t macDeviceAction2[5];

#ifdef FEATURE_WISUN_SUPPORT
/* Device action set 3*/
macAction_t macDeviceAction3[1];
/* Device action set 4*/
macAction_t macDeviceAction4[5];
#endif

/* MAC device info */
macDevice_t macDevice;

/* State table for MAC_IDLE_ST */
const uint8 macIdleSt[][MAC_NUM_COLS] =
{
/* Event                         Action                             Next State */
/* API_DATA_REQ_EVT */          {MAC_API_DATA_REQ,                  MAC_IDLE_ST},
/* API_PURGE_REQ_EVT */         {MAC_API_PURGE_REQ,                 MAC_IDLE_ST},
/* API_START_REQ_EVT */         {MAC_API_START_REQ,                 MAC_IDLE_ST},
/* API_WS_ASYNC_REQ_EVT */      {MAC_API_WS_ASYNC_REQ,              MAC_IDLE_ST},
/* RX_DATA_IND_EVT */           {MAC_DATA_RX_IND,                   MAC_IDLE_ST},
/* INT_START_COMPLETE_EVT */    {MAC_START_COMPLETE,                MAC_IDLE_ST},
/* INT_TX_COMPLETE_EVT */       {MAC_DATA_SEND,                     MAC_IDLE_ST},
/* NO_ACTION_EVT */             {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* CSMA_TIM_EXP_EVT*/           {MAC_CSMA_TIM_EXPIRY,               MAC_IDLE_ST},
/* TX_BACKOFF_TIM_EXP_EVT*/     {TX_BACKOFF_TIM_EXPIRY,             MAC_IDLE_ST},
/* RX_BACKOFF_TIM_EXP_EVT*/     {RX_BACKOFF_TIM_EXPIRY,             MAC_IDLE_ST}
};

/* Message buffer size table used by macSendMsg */
const uint16 macSendMsgBufSize[] =
{
  0,                                                 /* API_DATA_REQ_EVT ignored */
  (uint16) offsetof(macApiEvent_t, mac) +             /* API_PURGE_REQ_EVT */
    sizeof(macMcpsPurgeReq_t),
  (uint16) offsetof(macApiEvent_t, mac) +             /* API_START_REQ_EVT */
    sizeof(ApiMac_mlmeStartReq_t),
  (uint16) offsetof(macApiDataEvent_t, mac) +          /* API_WS_ASYNC_REQ_EVT */
      sizeof(ApiMac_mlmeWSAsyncReq_t) + MAC_WS_ASYNC_REQ_FRAME_LEN
};

/* Parameter data size table used by macSendMsg */
const uint8 macSendMsgDataSize[] =
{
  0,                                          /* API_DATA_REQ_EVT ignored */
  (uint8) sizeof(macMcpsPurgeReq_t),          /* API_PURGE_REQ_EVT */
  (uint8) sizeof(ApiMac_mlmeStartReq_t),          /* API_START_REQ_EVT */
  (uint8) sizeof(ApiMac_mlmeWSAsyncReq_t)        /* API_WS_ASYNC_REQ_EVT */
};

/* Lookup callback event based on MAC event */
const uint8 macCbackEventTbl[] =
{
  MAC_MCPS_DATA_CNF,            /* API_DATA_REQ_EVT */
  MAC_MCPS_PURGE_CNF,           /* API_PURGE_REQ_EVT */
  MAC_MLME_START_CNF,           /* API_START_REQ_EVT */
  MAC_MLME_WS_ASYNC_CNF         /* API_WS_ASYNC_REQ_EVT */
};

/* OSAL task id */
uint8 macTaskId;

/* mac_main data */
macMain_t macMain;

/* State table */
macStateTbl_t macStateTbl[] =
{
  macIdleSt            /* MAC_IDLE_ST */
};


/* initialization functions */
macReset_t macReset[MAC_FEAT_MAX];

/* Table of state machine action function table pointers */
macActionSet_t macActionSet[MAC_ACTION_SET_MAX] =
{
  NULL,                 /* Mgmt set 1 */
  NULL,                 /* Mgmt set 2 */
  NULL,                 /* Scan */
  NULL,                 /* Device set 1 */
  NULL,                 /* Device set 2 */
  NULL,                 /* Coordinator set 1 */
  NULL,                 /* Coordinator set 2 */
  NULL,                 /* Coordinator set 3 */
  NULL                  /* Beacon device */
};


/* MAC mangement info */
macMgmt_t macMgmt;

/* TRUE if operating as a PAN coordinator */
bool macPanCoordinator;

/* Action set 1 */
macAction_t macMgmtAction1[12];

/* Action set 2 */
macAction_t macMgmtAction2[5];

/**
  Flag which identifies whether frequency hopping is enabled or
  not.
*/
//bool freqHopEnabled = FALSE;

/**
  frequency hopping object. Holds the function pointers related
  to frequency hopping module.
*/
fhObject_t fhObject = {0};

bool csmPhy = FALSE;

/* MAC PIB */
macPib_t macPib;

macTimerHeader_t timerUnalignedQ;
macTimerHeader_t timerAlignedQ;

int32 macTimerRollover = MAC_SYMBOL_TIMER_DEFAULT_ROLLOVER;
 int32 macTimerNewRollover = MAC_SYMBOL_TIMER_DEFAULT_ROLLOVER;
macTimer_t *macTimerActive;

/* timer rollover table indexed by beacon order */
const uint32 CODE macTimerRolloverValue[] =
{
  MAC_BEACON_INTERVAL(0),
  MAC_BEACON_INTERVAL(1),
  MAC_BEACON_INTERVAL(2),
  MAC_BEACON_INTERVAL(3),
  MAC_BEACON_INTERVAL(4),
  MAC_BEACON_INTERVAL(5),
  MAC_BEACON_INTERVAL(6),
  MAC_BEACON_INTERVAL(7),
  MAC_BEACON_INTERVAL(8),
  MAC_BEACON_INTERVAL(9),
  MAC_BEACON_INTERVAL(10),
  MAC_BEACON_INTERVAL(11),
  MAC_BEACON_INTERVAL(12),
  MAC_BEACON_INTERVAL(13),
  MAC_BEACON_INTERVAL(14),
  MAC_SYMBOL_TIMER_DEFAULT_ROLLOVER
};

//#if defined (FEATURE_BEACON_MODE) || defined (FEATURE_NON_BEACON_MODE)
#if 1
/* Scan action set */
macAction_t macScanAction[6];

macScan_t macScan;
#endif

#ifdef FEATURE_MAC_SECURITY
/* Security related MAC PIB */
macSecurityPib_t macSecurityPib;


#if defined( FEATURE_MAC_PIB_PTR )

/* Pointer to the mac security PIB */
macSecurityPib_t* pMacSecurityPib = &macSecurityPib;

#endif

/* Crypto driver handle */
#if !defined(DeviceFamily_CC13X2) && !defined(DeviceFamily_CC26X2) && !defined(DeviceFamily_CC13X2X7)
CryptoCC26XX_Handle Crypto_handle;
#else
AESCCM_Handle AESCCM_handle;
TRNG_Handle TRNG_handle;
#endif

/* Crypto driver function table */
uint32_t *macCryptoDrvTblPtr;

/* Length M of authentication tag indexed by security level */
CODE const uint8 macAuthTagLen[] =
{
  0,                                  /* MAC_SEC_LEVEL_NONE */
  MAC_MIC_32_LEN,                     /* MAC_SEC_LEVEL_MIC_32 */
  MAC_MIC_64_LEN,                     /* MAC_SEC_LEVEL_MIC_64 */
  MAC_MIC_128_LEN,                    /* MAC_SEC_LEVEL_MIC_128 */
  0,                                  /* MAC_SEC_LEVEL_ENC */
  MAC_MIC_32_LEN,                     /* MAC_SEC_LEVEL_ENC_MIC_32 */
  MAC_MIC_64_LEN,                     /* MAC_SEC_LEVEL_ENC_MIC_64 */
  MAC_MIC_128_LEN                     /* MAC_SEC_LEVEL_ENC_MIC_128 */
};

/* Length of key source indexed by key identifier mode */
CODE const uint8 macKeySourceLen[] =
{
  MAC_KEY_ID_IMPLICIT_LEN,            /* MAC_KEY_ID_MODE_IMPLICIT */
  MAC_KEY_ID_1_LEN,                   /* MAC_KEY_ID_MODE_1 */
  MAC_KEY_ID_4_LEN,                   /* MAC_KEY_ID_MODE_4 */
  MAC_KEY_ID_8_LEN                    /* MAC_KEY_ID_MODE_8 */
};
#endif

#if defined (FEATURE_BEACON_MODE) || defined (FEATURE_NON_BEACON_MODE) || defined (STACK_LIBRARY)

uint8 macBeaconPayload[MAC_PIB_MAX_BEACON_PAYLOAD_LEN] = {0};

#endif


#ifndef ENHACK_MALLOC
uint8 enhancedAckBuf[250];
#endif

#if defined(FEATURE_WISUN_EDFE_SUPPORT)
uint8_t edfeEfrmBuf[MAC_EDFE_MAX_FRAME];
macEdfeInfo_t macEdfeInfo;
#elif defined(FEATURE_WISUN_MIN_EDFE)
macEdfeInfo_t macEdfeInfo;
#endif
