/******************************************************************************

 @file  gap_advertiser_internal.h

 @brief This file contains internal interfaces for the GAP.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: TI_TEXT 2009 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR
 *
 * These API's shall begin with a lower-case letter
 */

#ifndef GAP_ADV_INTERNAL_H
#define GAP_ADV_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "ll.h"
#include "gap.h"

/*********************************************************************
 * MACROS
 */

#ifndef status_t
  #define status_t bStatus_t
#endif

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */
/** Advertising and Scan Response Data **/
typedef struct
{
  uint8   dataLen;                  // Number of bytes used in "dataField"
  uint8   dataField[B_MAX_ADV_LEN]; // Data field of the advertisement or SCAN_RSP
} gapAdvertisingData_t;

typedef struct
{
  uint8   dataLen;       // length (in bytes) of "dataField"
  uint8   dataField[1];  // This is just a place holder size
                         // The dataField will be allocated bigger
} gapAdvertRecData_t;

// Temporary advertising record
typedef struct
{
  uint8  eventType;               // Avertisement or SCAN_RSP
  uint8  addrType;                // Advertiser's address type
  uint8  addr[B_ADDR_LEN];        // Advertiser's address
  gapAdvertRecData_t *pAdData;    // Advertising data field. This space is allocated.
  gapAdvertRecData_t *pScanData;  // SCAN_RSP data field. This space is allocated.
} gapAdvertRec_t;

typedef enum
{
  GAP_ADSTATE_SET_PARAMS,     // Setting the advertisement parameters
  GAP_ADSTATE_SET_MODE,       // Turning on advertising
  GAP_ADSTATE_ADVERTISING,    // Currently Advertising
  GAP_ADSTATE_ENDING          // Turning off advertising
} gapAdvertStatesIDs_t;

// Lookup for a GAP Scanner parameter.
// Note: Offset is offset into controller aeSetParamCmd_t structure
// There is no need for range checking in the host as it is done in the
// controller
typedef struct {
  uint8_t  offset;
  uint8_t  len;
} gapAdv_ParamLookup_t;

/// Advertiser remove set Event
typedef struct {
  osal_event_hdr_t hdr;       //!< GAP_MSG_EVENT and status
  uint8_t          opcode;    //!< GAP_ADV_REMOVE_SET_EVENT
  uint8_t          handle;    //!< Handle of the adv set to remove
  aeSetParamCmd_t* pAdvParam; //!< pointer of the parameter to free
} gapAdv_removeAdvEvent_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GAP Advertiser Functions
 */
extern bStatus_t gapAdv_init(void);
extern void      gapAdv_controllerCb(uint8 cBackID, void *pParams);
extern bStatus_t gapAdv_searchForBufferUse(uint8 *pBuf, advSet_t *pAdvSet);
extern void      gapAdv_processRemoveSetEvt(uint8_t handle,
                                            aeSetParamCmd_t* pAdvParam);

/**
 * gapAdv_periodicAdvCmdCompleteCBs
 *
 * Process an incoming OSAL HCI Command Complete Event for
 * periodic advertising.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if processed and safe to deallocate, FALSE if passed
 *          off to another task.
 */
uint8_t gapAdv_periodicAdvCmdCompleteCBs( hciEvt_CmdComplete_t *pMsg );

uint8_t gapAdv_handleAdvHciCmdComplete( hciEvt_CmdComplete_t *pMsg );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GAP_INTERNAL_H */
