/******************************************************************************

 @file  mac_main.c

 @brief This module is the main event handler. It contains the main OSAL event
        loop and the table-driven state machine

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

#include "hal_mcu.h"
#include "timac_api.h"
#include "mac_assert.h"
#include "mac_spec.h"
#include "mac_main.h"
#include "mac_data.h"
#include "mac_timer.h"
#include "mac_mgmt.h"
#include "mac_scan.h"
#include "mac_pwr.h"
#include "mac_pib.h"
#include "mac_security.h"
#ifndef OSAL_PORT2TIRTOS
#include "osal_pwrmgr.h"
#include "comdef.h"
#endif
#include <stddef.h>


/******************************************************************************
  ROM jump table will support
  1. TIMAC ROM image build
  2. TIMAC stack build without ROM image
  3. TIMAC stack with calling ROM image
 *****************************************************************************/
#include "rom_jt_154.h"

/* ------------------------------------------------------------------------------------------------
 *                                           Constants
 * ------------------------------------------------------------------------------------------------
 */

/* State table format */
#define MAC_ACTION_POS        0       /* action position */
#define MAC_NEXT_STATE_POS    1       /* next state position */
#define MAC_NUM_COLS          2       /* number of columns in state tables */

/* ------------------------------------------------------------------------------------------------
 *                                           Local Variables
 * ------------------------------------------------------------------------------------------------
 */


/* ------------------------------------------------------------------------------------------------
 *                                           Functions
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
 * @fn          macMainReset
 *
 * @brief       This function initializes the data structures for module main.
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
MAC_INTERNAL_API void macMainReset(void)
{
  uint8 *pMsg;

  /* clear pending message */
  if (macMain.pPending != NULL)
  {
    MAP_mac_msg_deallocate((uint8 **)&macMain.pPending);
  }
  macMain.state = MAC_IDLE_ST;

#ifndef OSAL_PORT2TIRTOS
  if (macTasksEvents != NULL)
#endif
  {
    /* clear out OSAL message queue only if OSAL is initialized */
    while ((pMsg = MAP_osal_msg_receive(macTaskId)) != NULL)
    {
      MAP_mac_msg_deallocate(&pMsg);
    }
  }

  /* Power management state may change. Hence, vote. */
  MAC_PWR_VOTE_NO_SWI(FALSE);
}

/**************************************************************************************************
 * @fn          macSetEvent
 *
 * @brief       This function sets and OSAL event to the MAC task.  It is also used as a MAC
 *              timer callback function.
 *
 * input parameters
 *
 * @param       event - OSAL event.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
MAC_INTERNAL_API void macSetEvent(uint8 event)
{
  MAP_osal_set_event(macTaskId, (uint16)event);
}

/**************************************************************************************************
 * @fn          macSetEvent
 *
 * @brief       This function sets and OSAL event to the MAC task.  It is also used as a MAC
 *              timer callback function.
 *
 * input parameters
 *
 * @param       event - OSAL event.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
MAC_INTERNAL_API void macSetEvent16(uint16 event)
{
  MAP_osal_set_event(macTaskId, event);
}

#if !defined(TIMAC_ROM_PATCH)
/**************************************************************************************************
 * @fn          macCbackForEvent
 *
 * @brief       This function looks up the callback event for the given internal event and
 *              calls the MAC callback function.
 *
 * input parameters
 *
 * @param       pEvent - Pointer to event data.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
MAC_INTERNAL_API void macCbackForEvent(macEvent_t *pEvent, uint8 status)
{
  macMcpsDataCnf_t cbackEvent;    /* use data cnf type for all events */

  cbackEvent.hdr.event = macCbackEventTbl[pEvent->hdr.event];
  if (cbackEvent.hdr.event == MAC_MLME_COMM_STATUS_IND)
  {
    /* handle comm status ind separately */
    pEvent->hdr.status = status;
    MAP_macCommStatusInd(pEvent);
  }
  else if (cbackEvent.hdr.event != 0)
  {
    /* if data cnf fill in additional fields */
    if (cbackEvent.hdr.event == MAC_MCPS_DATA_CNF)
    {
      if (status == MAC_NO_RESOURCES)
      {
        cbackEvent.pDataReq = NULL;
      }
      else
      {
        cbackEvent.pDataReq   = &(pEvent->dataReq);
        cbackEvent.msduHandle = pEvent->dataReq.mac.msduHandle;
        cbackEvent.timestamp  = pEvent->dataReq.internal.timestamp;
        cbackEvent.timestamp2 = pEvent->dataReq.internal.timestamp2;
        cbackEvent.retries = (pEvent->dataReq.internal.txOptions & MAC_TXOPTION_NO_RETRANS) ?
                             0 : pMacPib->maxFrameRetries - pEvent->dataReq.internal.retries;
        cbackEvent.mpduLinkQuality = pEvent->dataReq.internal.mpduLinkQuality;
        cbackEvent.correlation = pEvent->dataReq.internal.correlation;
        cbackEvent.rssi = pEvent->dataReq.internal.rssi;

#ifdef FEATURE_MAC_SECURITY
#if (defined TIMAC_ROM_IMAGE_BUILD)
    if (macPib.securityEnabled == TRUE)
    {
#endif
        cbackEvent.frameCntr = pEvent->dataReq.internal.frameCntr;
#if (defined TIMAC_ROM_IMAGE_BUILD)
    }
#endif
#endif

      }
    }
    else if (cbackEvent.hdr.event == MAC_MLME_WS_ASYNC_CNF )
    {
      MAP_osal_msg_deallocate( (uint8*)pEvent );
    }

    cbackEvent.hdr.status = status;
    MAP_MAC_CbackEvent((macCbackEvent_t *) &cbackEvent);
  }
}
#endif
/**************************************************************************************************
 * @fn          macStateIdle
 *
 * @brief       This function returns true if the MAC state is MAC_IDLE_ST.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      TRUE if the MAC state is MAC_IDLE_ST, FALSE otherwise.
 **************************************************************************************************
 */
MAC_INTERNAL_API bool macStateIdle(void)
{
  return (bool) (macMain.state == MAC_IDLE_ST);
}

/**************************************************************************************************
 * @fn          macStateIdleOrPolling
 *
 * @brief       This function returns true if the MAC state is MAC_IDLE_ST or MAC_POLLING_ST.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      TRUE if the MAC state is MAC_IDLE_ST or MAC_POLLING_ST, FALSE otherwise.
 **************************************************************************************************
 */
MAC_INTERNAL_API bool macStateIdleOrPolling(void)
{
  /* Used to allow the MAC to sleep while waiting for poll response. */
  return (bool) ((macMain.state == MAC_IDLE_ST) || (macMain.state == MAC_POLLING_ST));
}

/**************************************************************************************************
 * @fn          macStateScanning
 *
 * @brief       This function returns true if the MAC state is MAC_SCANNING_ST.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      TRUE if the MAC state is MAC_SCANNING_ST, FALSE otherwise.
 **************************************************************************************************
 */
MAC_INTERNAL_API bool macStateScanning(void)
{
  return (bool) (macMain.state == MAC_SCANNING_ST);
}

/**************************************************************************************************
 * @fn          macExecute
 *
 * @brief       This function executes the MAC state machine.  It looks up
 *              the state table for the current state in macStateTbl[].
 *              It first sets the state to the next state in the state table.
 *              Then it looks up the action enumeration for the event in
 *              the state table, looks up the action in macAction[]
 *              and executes the action function.
 *
 * input parameters
 *
 * @param       pEvent - Pointer to event structure.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
#ifdef SMDBG
uint8_t smEvent[MAC_NO_OF_EVENTS];
#endif
MAC_INTERNAL_API void macExecute(macEvent_t *pEvent)
{
  macStateTbl_t   stateTbl;
  macActionSet_t  actionSet;
  uint8           action;

  /* look up the state table for the current state */
  stateTbl = macStateTbl[macMain.state];

  /* Clear reserved bit in event */
  pEvent->hdr.event &= ~MAC_BUF_RESERVED;
#ifdef SMDBG
  smEvent[pEvent->hdr.event]++;
#endif

  /* look up action enumeration for event in the state table */
  action = stateTbl[pEvent->hdr.event][MAC_ACTION_POS];

  /* look up action set */
  actionSet = macActionSet[MAC_ACTION_SET(action)];

  /* if action set not present execute default action */
  if (actionSet == NULL)
  {
    MAP_macDefaultAction(pEvent);
  }
  else
  {
     /* look up the action in the action table and execute the action */
     if ((*actionSet[MAC_ACTION_ID(action)]) != NULL)
     {
        (*actionSet[MAC_ACTION_ID(action)])(pEvent);
     }
     else
     {
        MAC_ASSERT(0);
     }
  }
}

/**************************************************************************************************
 * @fn          macSendMsg
 *
 * @brief       This function sends an OSAL message to the MAC.  First it looks up the required
 *              message buffer length based on the event type. It then allocates an OSAL message
 *              buffer, copies the parameters to the buffer, and calls osal_msg_send().  If
 *              allocation fails, the MAC callback function is executed with event status
 *              MAC_NO_RESOURCES.
 *
 * input parameters
 *
 * @param       event - MAC event to send
 * @param       security - TRUE if MAC security is used
 * @param       pData - message parameters
 *
 * output parameters
 *
 * None.
 *
 * @return      MAC_SUCCESS if successful in creating and dispatching the message
 *              otherwise MAC error codes.
 **************************************************************************************************
 */
MAC_INTERNAL_API uint8 macSendMsg(uint8 event, void *pData)
{
  macEvent_t    *pMsg;
  macEventHdr_t hdr;
  uint8 status = MAC_SUCCESS;

  pMsg = (macEvent_t *) MAP_osal_msg_allocate(macSendMsgBufSize[event]);
  if (pMsg)
  {
    MAP_osal_memset( pMsg, 0x0, macSendMsgBufSize[event] );

    pMsg->hdr.event = event;
    pMsg->hdr.status = MAC_SUCCESS;
    MAP_osal_memcpy(&pMsg->api.mac, pData, macSendMsgDataSize[event]);
    if ( SUCCESS != MAP_osal_msg_send(macTaskId, (uint8 *) pMsg))
    {
      status = MAC_NO_RESOURCES;
    }
  }
  else
  {
    /* call callback with failure */
    hdr.event = event;
    MAP_macCbackForEvent((macEvent_t *) &hdr, MAC_NO_RESOURCES);
    status = MAC_NO_RESOURCES;
  }

  return (status);
}

/**************************************************************************************************
 * @fn          macSendDataMsg
 *
 * @brief       This function sends an OSAL message to the MAC with extra buffer space allocated
 *              for a command frame.  First it looks up the required message buffer length based
 *              on the event type.  If security is enabled the length is increased for security
 *              parameters.  It then allocates an OSAL message buffer, copies the parameters
 *              to the buffer, and calls osal_msg_send().  If allocation fails, the MAC callback
 *              function is executed with event status MAC_NO_RESOURCES.
 *
 * input parameters
 *
 * @param       event - MAC event to send
 * @param       pData - message parameters
 * @param       *sec  - pointer to MAC security parameters. This cannot be driven from pData since
 *                      the pData is coming from different structure type.
 *
 * output parameters
 *
 * None.
 *
 * @return      MAC_SUCCESS or MAC_NO_RESOURCES.
 **************************************************************************************************
 */
MAC_INTERNAL_API uint8 macSendDataMsg(uint8 event, void *pData, ApiMac_sec_t *sec)
{
  macEvent_t    *pMsg;
  uint16        len;
  macEventHdr_t hdr;
  uint8         micLen;

  /* we have table of cmd frame size and sizeof(msg) indexed by event */
  len = macSendMsgBufSize[event];
#ifdef FEATURE_FREQ_HOP_MODE
  if ( MAC_FH_ENABLED && MAC_FH_GET_IE_LEN_FN )
  {
    uint16 ieLen = 0;

    /**
     * Note: Only the following events maps to cmd correctly in
     * macCmdIncludeFHIe[]
     * MAC_API_ASSOCIATE_REQ_EVT, API_ASSOCIATE_RSP_EVT,
     * MAC_API_DISASSOCIATE_REQ_EVT. These are the events which need
     * FH ie's.
     */
    /* buffer space for FH header IE's if any for this command */
    ieLen = MAC_FH_GET_IE_LEN_FN( macCmdIncludeFHIe[event] & MAC_FH_HEADER_IE_MASK);
    /* buffer space for FH Payload IE's if any for this command */
    ieLen += MAC_FH_GET_IE_LEN_FN( macCmdIncludeFHIe[event] & MAC_FH_PAYLOAD_IE_MASK);

    if ( ieLen )
    {
      len += ieLen + MAC_HEADER_IE_HEADER_LEN + MAC_PAYLOAD_IE_HEADER_LEN;
    }
  }
#endif

#ifdef FEATURE_MAC_SECURITY
#if (defined TIMAC_ROM_IMAGE_BUILD)
    if (macPib.securityEnabled == TRUE)
    {
#endif
      if (pMacPib->securityEnabled == TRUE && sec->securityLevel > MAC_SEC_LEVEL_NONE)
      {
        /* Save local copy */
        micLen = macAuthTagLen[sec->securityLevel];

        /* Add security header length as data offset.
         * Do not add MIC length as it is not part of security header.
         */
        len += (MAC_SEC_CONTROL_FIELD_LEN + MAC_FRAME_COUNTER_LEN + macKeySourceLen[sec->keyIdMode]);
      }
      else
      {
        micLen = 0;
      }
#if (defined TIMAC_ROM_IMAGE_BUILD)
    }
    else
    {
        (void)sec;
        micLen = 0;
    }
#endif
#else
  (void)sec;
  micLen = 0;
#endif /* FEATURE_MAC_SECURITY */

  pMsg = (macEvent_t *) MAP_osal_msg_allocate(len + micLen);
  if (pMsg)
  {
    MAP_osal_memset( pMsg, 0x0, len );

    pMsg->hdr.event = event;
    pMsg->hdr.status = MAC_SUCCESS;

    /* set up data pointer to end of message (exclude MIC); to be built in reverse */
    pMsg->apiData.mpdu.p = (uint8 *) pMsg + len;
    pMsg->apiData.mpdu.len = micLen;

    MAP_osal_memcpy(&pMsg->apiData.mac, pData, macSendMsgDataSize[event]);
    MAP_osal_msg_send(macTaskId, (uint8 *) pMsg);
    return ( MAC_SUCCESS );
  }
  else
  {
    /* call callback with failure */
    hdr.event = event;
    MAP_macCbackForEvent((macEvent_t *) &hdr, MAC_NO_RESOURCES);
    return ( MAC_NO_RESOURCES );
  }
}

/**************************************************************************************************
 * @fn          macMainSetTaskId
 *
 * @brief       This function is called to set the OSAL Task Id.
 *
 * input parameters
 *
 * @param       taskId - OSAL task ID for MAC
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void macMainSetTaskId(uint8 taskId)
{
  macTaskId = taskId;
}

/**************************************************************************************************
 * @fn          macMainReserve
 *
 * @brief       This function "reserves" the memory pointed to by macMain.pBuf by setting
 *              macMain.pBuf to NULL.
 *
 * input parameters
 *
 * @param       ptr - Pointer to memory to be reserved (i.e. not freed by macEventLoop).
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
MAC_INTERNAL_API void macMainReserve( uint8 *ptr )
{
  if (macMain.pBuf == ptr)
  {
    macMain.pBuf = NULL;
  }
}

/**************************************************************************************************
 * @fn          mac_msg_deallocate
 *
 * @brief       This function is the MAC API to osal_msg_deallocate.
 *
 * input parameters
 *
 * @param       **msg_ptr - buffer pointer-pointer to be used to free the dyanmic memory and set the
 *                          pointer parameter to NULL for good housekeeping.
 *
 * output parameters
 *
 * @param       *msg_ptr - is set to NULL after the memory is freed.
 *
 * @return      None.
 **************************************************************************************************
 */
MAC_INTERNAL_API void mac_msg_deallocate(uint8 **msg_ptr)
{
  halIntState_t intState;

  MAC_ASSERT(msg_ptr);

  HAL_ENTER_CRITICAL_SECTION(intState);

  if (*msg_ptr != NULL)
  {
    uint8 *x = (uint8 *)((uint8 *)*msg_ptr - sizeof( MAP_osal_msg_hdr_t ));

    if (macMain.pBuf == *msg_ptr)
    {
      /* This keeps macEventLoop from making a double free of the same memory. */
      macMain.pBuf = NULL;
    }

    *msg_ptr = NULL;
    MAP_osal_mem_free((void *)x);
  }

  HAL_EXIT_CRITICAL_SECTION(intState);
}

