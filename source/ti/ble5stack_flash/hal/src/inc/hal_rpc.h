/******************************************************************************

 @file  hal_rpc.h

 @brief This file contains information required by services that use the
        Remote Procedure Call (RPC) standard.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2008 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef HAL_RPC_H
#define HAL_RPC_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                          Constants
 * ------------------------------------------------------------------------------------------------
 */

///////////////////////////////////////////////////////////////////////////////////////////////////
// Common
///////////////////////////////////////////////////////////////////////////////////////////////////

// RPC Command Field Type
#define RPC_CMD_POLL        0x00
#define RPC_CMD_SREQ        0x20
#define RPC_CMD_AREQ        0x40
#define RPC_CMD_SRSP        0x60
#define RPC_CMD_RES4        0x80
#define RPC_CMD_RES5        0xA0
#define RPC_CMD_RES6        0xC0
#define RPC_CMD_RES7        0xE0

// RPC Command Field Subsystem
#define RPC_SYS_RES0        0
#define RPC_SYS_SYS         1
#define RPC_SYS_MAC         2
#define RPC_SYS_NWK         3
#define RPC_SYS_AF          4
#define RPC_SYS_ZDO         5
#define RPC_SYS_SAPI        6    // Simple API
#define RPC_SYS_UTIL        7
#define RPC_SYS_DBG         8
#define RPC_SYS_APP         9
#define RPC_SYS_RCAF        10   // Remote Control Application Framework
#define RPC_SYS_RCN         11   // Remote Control Network Layer
#define RPC_SYS_RCN_CLIENT  12   // Remote Control Network Layer Client
#define RPC_SYS_BOOT        13   // Serial Bootloader
#define RPC_SYS_MAX         14   // Maximum value, must be last

/* 1st byte is the length of the data field, 2nd/3rd bytes are command field. */
#define RPC_FRAME_HDR_SZ    3

/* The 3 MSB's of the 1st command field byte are for command type. */
#define RPC_CMD_TYPE_MASK   0xE0

/* The 5 LSB's of the 1st command field byte are for the subsystem. */
#define RPC_SUBSYSTEM_MASK  0x1F

/* position of fields in the general format frame */
#define RPC_POS_LEN         0
#define RPC_POS_CMD0        1
#define RPC_POS_CMD1        2
#define RPC_POS_DAT0        3

/* Error codes */
#define RPC_SUCCESS         0     /* success */
#define RPC_ERR_SUBSYSTEM   1     /* invalid subsystem */
#define RPC_ERR_COMMAND_ID  2     /* invalid command ID */
#define RPC_ERR_PARAMETER   3     /* invalid parameter */
#define RPC_ERR_LENGTH      4     /* invalid length */

///////////////////////////////////////////////////////////////////////////////////////////////////
// UART Specific
///////////////////////////////////////////////////////////////////////////////////////////////////

#define RPC_UART_FRAME_OVHD 2

// Start of frame character value
#define RPC_UART_SOF        0xFE

///////////////////////////////////////////////////////////////////////////////////////////////////
// SPI Specific
///////////////////////////////////////////////////////////////////////////////////////////////////


/* ------------------------------------------------------------------------------------------------
 *                                           Typedefs
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
*/

#ifdef __cplusplus
};
#endif

#endif /* HAL_RPC_H */
