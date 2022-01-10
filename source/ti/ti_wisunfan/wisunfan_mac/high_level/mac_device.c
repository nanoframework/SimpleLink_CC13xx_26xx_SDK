/******************************************************************************

 @file  mac_device.c

 @brief This module implements MAC device management procedures for a device.

 Group: WCS, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2005-2021, Texas Instruments Incorporated
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

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

#ifndef LIBRARY
#include "ti_wisunfan_features.h"
#endif

#include "timac_api.h"
#include "mac_low_level.h"
#include "mac_spec.h"
#include "mac_main.h"
#include "mac_timer.h"
#include "mac_mgmt.h"
#include "mac_pib.h"
#include "mac_security_pib.h"
#include "mac_data.h"
#include "mac_scan.h"
#include "mac_device.h"
#include "mac_ie_build.h"

#include "rom_jt_154.h"

// SW Tracer
#include "dbg.h"
#ifdef DEBUG_SW_TRACE
#define DBG_ENABLE
#ifndef _DBGFILE
#define _DBGFILE mac_device_c
#endif
#include "dbgid_sys_mst.h"
#endif // DEBUG_SW_TRACE

/******************************************************************************
  ROM jump table will support
  1. TIMAC ROM image build
  2. TIMAC stack build without ROM image
  3. TIMAC stack with calling ROM image
 *****************************************************************************/
#include "rom_jt_154.h"

/* ------------------------------------------------------------------------------------------------
 *                                           Local Variables
 * ------------------------------------------------------------------------------------------------
 */


/* ------------------------------------------------------------------------------------------------
 *                                           Local Functions
 * ------------------------------------------------------------------------------------------------
 */

void macAssociateCnf(uint8 status, uint16 addr);
void macPollCnf(uint8 status, uint8 framePending);
uint8 macBuildAssociateReq(macEvent_t *pEvent);

#if !defined(TIMAC_ROM_PATCH)
/**************************************************************************************************
 * @fn          macDeviceReset
 *
 * @brief       This function initializes the data structures for the device module.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
MAC_INTERNAL_API void macDeviceReset(void)
{
  /* Device action set 1 */
  macDeviceAction1[0] = MAP_macApiAssociateReq;
  macDeviceAction1[1] = MAP_macApiPollReq;
  macDeviceAction1[2] = MAP_macAssocDataReq;
  macDeviceAction1[3] = MAP_macAssocDataReqComplete;
  macDeviceAction1[4] = MAP_macAssocDataRxInd;
  macDeviceAction1[5] = MAP_macAssocFailed;
  macDeviceAction1[6] = MAP_macAssocFrameResponseTimeout;
  macDeviceAction1[7] = MAP_macPollDataReqComplete;
  macDeviceAction1[8] = MAP_macPollDataRxInd;
  macDeviceAction1[9] = MAP_macPollFrameResponseTimeout;
  macDeviceAction1[10] = MAP_macPollRxAssocRsp;
  macDeviceAction1[11] = MAP_macRxAssocRsp;
  macDeviceAction1[12] = MAP_macStartFrameResponseTimer;
  macDeviceAction1[13] = MAP_macStartResponseTimer;

  /* Device action set 2 */
  macDeviceAction2[0] = MAP_macAssocRxDisassoc;
  macDeviceAction2[1] = MAP_macPollRxDisassoc;
#if defined (FEATURE_BEACON_MODE) || defined (FEATURE_NON_BEACON_MODE)
  macDeviceAction2[2] = MAP_macScanRxCoordRealign;
#else
  macDeviceAction2[2] = NULL;
#endif /* (FEATURE_BEACON_MODE) || (FEATURE_NON_BEACON_MODE) */
  macDeviceAction2[3] = MAP_macRxCoordRealign;
  macDeviceAction2[4] = MAP_macPanConflictComplete;

  macDevice.sec.securityLevel = 0;
  macDevice.dataReqMask = 0;
  macDevice.frameTimer.pFunc = MAP_macSetEvent;
  macDevice.frameTimer.parameter = MAC_FRAME_RESPONSE_TASK_EVT;
  macDevice.responseTimer.pFunc = MAP_macSetEvent;
  macDevice.responseTimer.parameter = MAC_RESPONSE_WAIT_TASK_EVT;
}
#endif

/**************************************************************************************************
 * @fn          macAssociateCnf
 *
 * @brief       Call the MAC callback with a MAC_MLME_ASSOCIATE_CNF event.
 *
 * input parameters
 *
 * @param       status - Associate confirm status.
 * @param       addr - Associate confirm short address.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void macAssociateCnf(uint8 status, uint16 addr)
{
  macMlmeAssociateCnf_t associateCnf;

  associateCnf.hdr.event = MAC_MLME_ASSOCIATE_CNF;
  associateCnf.hdr.status = status;
  if (status != MAC_SUCCESS)
  {
    addr = MAC_SHORT_ADDR_NONE;
  }
  associateCnf.assocShortAddress = addr;

  DBG_PRINT0(DBGSYS, "macAssociateCnf");

  /* copy the purported security parameters */
  MAP_osal_memcpy(&associateCnf.sec, &macDevice.sec, sizeof(ApiMac_sec_t));

  MAP_MAC_CbackEvent((macCbackEvent_t *) &associateCnf);
}

/**************************************************************************************************
 * @fn          macPollCnf
 *
 * @brief       Call the MAC callback with a MAC_MLME_POLL_CNF event.
 *
 * input parameters
 *
 * @param       status - Poll confirm status.
 *              framePending - set if frame pending is set in data packet
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void macPollCnf(uint8 status, uint8 framePending)
{
  macMlmePollCnf_t pollCnf;

  pollCnf.hdr.event = MAC_MLME_POLL_CNF;
  pollCnf.hdr.status = status;
  pollCnf.framePending = framePending;

  MAP_MAC_CbackEvent((macCbackEvent_t *) &pollCnf);
}

/**************************************************************************************************
 * @fn          macBuildAssociateReq
 *
 * @brief       This function builds and sends a associate request command frame in buffer pEvent.
 *              The TX options and frame type are set and other parameters in the macTx_t
 *              structure are initialized.  Then macBuildHeader() is called to build the header
 *              with the address information from the API event structure.  If successful the
 *              capability information is built in the frame and macDataTxEnqueue() is called to
 *              queue the frame for transmission.
 *
 * input parameters
 *
 * @param       pEvent - Pointer to event data.
 *
 * output parameters
 *
 * None.
 *
 * @return      MAC_SUCCESS if successful, otherwise failure status.
 **************************************************************************************************
 */
uint8 macBuildAssociateReq(macEvent_t *pEvent)
{
  sAddr_t  coordAddress;
  uint16   coordPanId;
  uint8    status;

  /* set parameters */
  pEvent->tx.internal.txOptions = MAC_TXOPTION_ACK;
  pEvent->tx.internal.frameType = MAC_INTERNAL_ASSOC_REQ;

  /* Prepare a local copy */
  MAP_copyApiMacAddrToMacAddr(&coordAddress, &pEvent->apiData.mac.associateReq.coordAddress);
  coordPanId = pEvent->apiData.mac.associateReq.coordPanId;

  /* buffer is set up to build frame in reverse, so build capability information into frame */
  pEvent->tx.msdu.p--;
  *(pEvent->tx.msdu.p)-- = MAP_ApiMac_convertCapabilityInfo(&(pEvent->apiData.mac.associateReq.capabilityInformation));


#ifdef FEATURE_MAC_SECURITY
#if (defined TIMAC_ROM_IMAGE_BUILD)
    if (macPib.securityEnabled == TRUE)
    {
#endif
      /* Only the private payload field(Command Contents in case of command frame)
         should be secured. */
      pEvent->tx.internal.ptrMData = pEvent->tx.msdu.p + 1;

#if (defined TIMAC_ROM_IMAGE_BUILD)
    }
#endif
#endif


  /* Command Identifier */
  *pEvent->tx.msdu.p = pEvent->tx.internal.frameType - 1;

  pEvent->tx.msdu.len += MAC_ASSOC_REQ_PAYLOAD;

  /* initialize IE pointers */
  pEvent->tx.internal.ptrHeaderIE = NULL;
  pEvent->tx.internal.ptrPayloadIE = NULL;
#ifdef FEATURE_FREQ_HOP_MODE
  /* Frequency hopping enabled */
  if ( MAC_FH_ENABLED )
  {
    pEvent->tx.internal.fhFrameType = MAC_FH_FRAME_DATA;
    pEvent->tx.internal.nb = 0;
    pEvent->tx.internal.be = (pEvent->tx.internal.txOptions & MAC_TXOPTION_ALT_BE) ? pMacPib->altBe : pMacPib->minBe;

    /* Copy the address (needs to be extended) for FH module to use */
    MAP_osal_memcpy(&pEvent->tx.internal.dest.dstAddr, &pEvent->apiData.mac.associateReq.coordAddress.addr, sizeof(sAddr_t));

    if(pEvent->apiData.mac.associateReq.coordAddress.addrMode == SADDR_MODE_EXT)
    {
      MAP_sAddrExtCpy(pEvent->tx.internal.dest.destAddr, pEvent->apiData.mac.associateReq.coordAddress.addr.extAddr);
      if(pMacPib->rxOnWhenIdle)
      {
        /* include the necessary Payaload IE's */
        if ( macCmdIncludeFHIe[MAC_INTERNAL_ASSOC_REQ] & MAC_FH_PAYLOAD_IE_MASK )
        {
          pEvent->tx.internal.payloadIeInfo[0].gId = MAC_PAYLOAD_IE_WISUN_GID;
          pEvent->tx.internal.payloadIeInfo[0].subId.subIdBMap =
          macCmdIncludeFHIe[MAC_INTERNAL_ASSOC_REQ] & MAC_FH_PAYLOAD_IE_MASK;

          MAP_macBuildPayloadIes(&pEvent->tx, pEvent->tx.internal.payloadIeInfo, TRUE, FALSE);
        }

        /* include the necessary header IE's */
        if ( macCmdIncludeFHIe[MAC_INTERNAL_ASSOC_REQ] & MAC_FH_HEADER_IE_MASK )
        {
          pEvent->tx.internal.headerIeInfo[0].elementId = MAC_HEADER_IE_WISUN;
          pEvent->tx.internal.headerIeInfo[0].subIdBitMap =
          macCmdIncludeFHIe[MAC_INTERNAL_ASSOC_REQ] & MAC_FH_HEADER_IE_MASK;

          MAP_macBuildHeaderIes(&pEvent->tx, pEvent->tx.internal.headerIeInfo);
        }
      }
    }
  }
#endif
  status = MAP_macBuildHeader(&pEvent->tx, (SADDR_MODE_EXT | MAC_SRC_PAN_ID_BROADCAST),
                          &coordAddress, coordPanId, 0);
  if ( status == MAC_SUCCESS)
  {
    /* queue frame for transmission */
    MAP_macDataTxEnqueue(&pEvent->tx);
  }
  return status;
}
