/******************************************************************************

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2010 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/**
 *  @defgroup RTLSSrv RTLSSrv Internal
 *  This module describes internal API's that RTLS Services will use
 *  @{
 *  @file       rtls_srv_internal.h
 *       RTLS Services Internal header file
 */

#ifndef RTLSSRVINTERNAL_H
#define RTLSSRVINTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------------------------------------------------
 * INCLUDES
 */

#include "map_direct.h"
#include "gap_internal.h"

/*********************************************************************
 * MACROS
 */
#define RTLSSrv_malloc ICall_malloc
#define RTLSSrv_free   ICall_free

#define BUILD_UINT16(loByte, hiByte) \
          ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

/*-------------------------------------------------------------------
 * CONSTANTS
 */

#define BIT_x(x)          (1 << (x))

#define HCI_CTE_MAX_SAMPLES_PER_EVENT            (96)  //!< Max samples data length in one HCI event

/*-------------------------------------------------------------------
 * TYPEDEFS
 */

/// @brief Structure holding the state of an ongoing Extented I/Q Report
typedef struct
{
  uint8_t                      evtStarted;         //!< I/Q event is ongoing
  uint8_t                      evtIndex;           //!< Index chunk number
  uint16_t                     remainingDataSize;  //!< Remaining size until we have all data
  rtlsSrv_connectionIQReport_t *pTmpExtIqEvt;      //!< This will temporarily hold the event that will eventually be sent to the user
} rtlsSrv_extIqEvtState_t;

typedef struct
{
  uint8_t              evtStarted;         //!< I/Q event is ongoing
  uint8_t              evtIndex;           //!< Index chunk number
  uint16_t             remainingDataSize;  //!< Remaining size until we have all data
  rtlsSrv_clIQReport_t *pTmpCLExtIqEvt;      //!< This will temporarily hold the event that will eventually be sent to the user
} rtlsSrv_extCLIqEvtState_t;

/// CL AoA number of antennas - linked list
typedef struct rtlsSrv_clNumAnt_t
{
  uint16_t syncHandle;              //!< Handle identifying the periodic advertising train
  uint8_t numAnt;                   //!< Number of antennas in the anternna pattern
  struct rtlsSrv_clNumAnt_t *next;
} rtlsSrv_clNumAnt_t;

/*-------------------------------------------------------------------
 * API's
 */

/*********************************************************************
 * @fn          RTLSSrv_handleError
 *
 * @brief       Handles errors by translating from HCI format to RTLS Services format
 *              Errors may result from incorrect user usage of API's
 *
 * @param       errSrc - which command/opcode caused the error
 * @param       errCause - what was the actual error
 * @param       connHandle - connection handle on which the error happened
 *
 * @return      TRUE = success, FALSE = failure
 */
bStatus_t RTLSSrv_handleError(uint16_t errSrc, uint16_t errCause, uint16_t connHandle);

/*********************************************************************
 * @fn          RTLSSrv_callAppCb
 *
 * @brief       Allocate and send a message to the application
 *
 * @param       evtType - opcode of the event
 * @param       evtSize - size of evtData
 * @param       pEvtData - actual information to pass
 *
 * @return      TRUE = success, FALSE = failure
 */
bStatus_t RTLSSrv_callAppCb(uint8_t evtType, uint16_t evtSize, uint8_t *pEvtData);

/*********************************************************************
 * @fn          RTLSSrv_handleConnIqEvent
 *
 * @brief       Handle a Connection IQ Event sent by HCI
 *
 * @param       pEvtData - actual information to pass
 *
 * @return      TRUE = success, FALSE = failure
 */
bStatus_t RTLSSrv_handleConnIqEvent(uint8_t *pEvtData);

/*********************************************************************
 * @fn          RTLSSrv_handleExtConnIqEvent
 *
 * @brief       Handle an Extended Connection IQ Event sent by HCI
 *
 * @param       evtData - actual information to pass
 *
 * @return      TRUE = success, FALSE = failure
 */
bStatus_t RTLSSrv_handleExtConnIqEvent(uint8_t *pEvtData);

/*********************************************************************
 * @fn          RTLSSrv_handleCteReqFail
 *
 * @brief       Handle a failed CTE request
 *
 * @param       pEvtData - actual information to pass
 *
 * @return      TRUE = success, FALSE = failure
 */
bStatus_t RTLSSrv_handleCteReqFail(uint8_t *pEvtData);

/*********************************************************************
 * @fn          RTLSSrv_handReadAntInfo
 *
 * @brief       Handle read antenna info event
 *
 * @param       pEvtData - actual information to pass
 *
 * @return      TRUE = success, FALSE = failure
 */
bStatus_t RTLSSrv_handleReadAntInfo(uint8_t *pEvtData);

/*********************************************************************
 * @fn          RTLSSrv_handleCLIqEvent
 *
 * @brief       Handle a Connectionless IQ Event sent by HCI
 *
 * @param       pEvtData - actual information to pass
 *
 * @return      TRUE if processed and safe to deallocate, FALSE if not processed.
 */
bStatus_t RTLSSrv_handleCLIqEvent(uint8_t *pEvtData);

/*********************************************************************
 * @fn          RTLSSrv_handleExtCLIqEvent
 *
 * @brief       Handle an Extended Connection IQ Event sent by HCI
 *
 * @param       evtData - actual information to pass
 *
 * @return      TRUE if processed and safe to deallocate, FALSE if not processed.
 */
bStatus_t RTLSSrv_handleExtCLIqEvent(uint8_t *pEvtData);

/**
 * RTLSSrv_processPeriodicAdvEvent
 *
 * Process incoming periodic advertising OSAL HCI BLE events:
 * translate to RTLS Services event and dispatch to user
 * registered callback
 *
 * @param       pMsg - message to process
 *
 * @return      TRUE if processed and safe to deallocate, FALSE if not processed.
 */
uint8 RTLSSrv_processPeriodicAdvEvent(osal_event_hdr_t *pMsg);

/**
 * RTLSSrv_getNumAnt
 *
 * Returns the number of antannas in the antenna patters
 * matching the given syncHandle
 *
 * @param       handle - connection/sync handle
 *
 * @return      0    - the handle was not found
 *              2-75 - number of antennas in the antenna pattern
 */
uint8_t RTLSSrv_getNumAnt(uint16_t handle);

/**
 * RTLSSrv_removeClNumAntNode
 *
 * Removes the node matching the given syncHandle
 *
 * @param       syncHandle - Handle identifying the periodic advertising train
 *
 * @return      None
 */
void RTLSSrv_removeClNumAntNode( uint16_t syncHandle );

/*********************************************************************
 * RTLSSrv_getClAntNode
 *
 * Find the CL AoA antenna node matching the given syncHandle
 *
 * @param       syncHandle - sync handle
 *
 * @return      If found - pointer to the node, else NULL
 */
rtlsSrv_clNumAnt_t* RTLSSrv_getClAntNode(uint16_t syncHandle);

#ifdef __cplusplus
}
#endif

#endif /* RTLSSRVINTERNAL_H */

/** @} End RTLSSrv */
