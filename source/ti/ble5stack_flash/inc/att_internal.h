/******************************************************************************

 @file  att_internal.h

 @brief This file contains internal interfaces for the Attribute
        Protocol (ATT) module.

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
 */


#ifndef ATT_INTERNAL_H
#define ATT_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "osal_cbtimer.h"

#include "l2cap.h"
#include "att.h"

/*********************************************************************
 * MACROS
 */

#define ATT_ADJUST_HDR( pPayload, size )   ( (pPayload) == NULL ? \
                                             NULL               : \
                                             MAP_osal_bm_adjust_header( (pPayload), (size) ) )

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

// Function prototype to build an attribute protocol message
typedef uint16 (*attBuildMsg_t)( uint8 *pBuf, uint8 *pMsg );

/*********************************************************************
 * VARIABLES
 */

// Used to throttle non-ATT responses when an ATT response retransmission is queued
extern uint8_t att_sendRspOnly;

/*********************************************************************
 * FUNCTIONS
 */

extern uint16 attBuildExecuteWriteRsp( uint8 *pBuf, uint8 *pMsg );

extern uint16 attBuildHandleValueCfm( uint8 *pBuf, uint8 *pMsg );

extern bStatus_t attSendMsg( uint16 connHandle, attBuildMsg_t pfnBuildMsg,
                             uint8 opcode, uint8 *pMsg, uint8 *pPayload );

extern bStatus_t attSendRspMsg( uint16 connHandle, attBuildMsg_t pfnBuildMsg,
                                uint8 opcode, uint8 *pMsg, uint8 *pPayload );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ATT_INTERNAL_H */
