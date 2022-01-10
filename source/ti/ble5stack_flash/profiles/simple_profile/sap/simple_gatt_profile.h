/******************************************************************************

 @file  simple_gatt_profile.h

 @brief This file contains the Simple GATT profile definitions and prototypes
        prototypes.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2010 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef SIMPLEGATTPROFILE_H
#define SIMPLEGATTPROFILE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "bcomdef.h"

/*********************************************************************
 * CONSTANTS
 */

#define SP_ID_CREATE(C,H)               (( C << 2 ) | H )
#define SP_ID_CHAR(ID)                  ( ID >> 2 )
#define SP_ID_CHARHTYPE(ID)             ( ID & 0x03 )

// Characteristic Types - These must be listed in order that they appear
//                        in service
#define SP_CHAR1                        0x00
#define SP_CHAR2                        0x01
#define SP_CHAR3                        0x02
#define SP_CHAR4                        0x03
#define SP_CHAR5                        0x04

// Characteristic Handle Types
#define SP_VALUE                        0x00
#define SP_USERDESC                     0x01
#define SP_CCCD                         0x02
#define SP_FORMAT                       0x03

#define SP_CHAR1_ID                     SP_ID_CREATE(SP_CHAR1,SP_VALUE)
#define SP_CHAR2_ID                     SP_ID_CREATE(SP_CHAR2,SP_VALUE)
#define SP_CHAR3_ID                     SP_ID_CREATE(SP_CHAR3,SP_VALUE)
#define SP_CHAR4_ID                     SP_ID_CREATE(SP_CHAR4,SP_VALUE)
#define SP_CHAR5_ID                     SP_ID_CREATE(SP_CHAR5,SP_VALUE)
#define SP_UNKNOWN_CHAR                 0xFF

// Simple Profile Service UUID
#define SIMPLEPROFILE_SERV_UUID               0xFFF0

// Simple Profile Characteristic UUIDs
#define SIMPLEPROFILE_CHAR1_UUID            0xFFF1
#define SIMPLEPROFILE_CHAR2_UUID            0xFFF2
#define SIMPLEPROFILE_CHAR3_UUID            0xFFF3
#define SIMPLEPROFILE_CHAR4_UUID            0xFFF4
#define SIMPLEPROFILE_CHAR5_UUID            0xFFF5

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has been written
typedef void (*simpleProfileWrite_t)( uint8 paramID );

// Callback when a cccd update request is received
typedef void (*simpleProfileCCCD_t)( uint8 paramID, uint16_t value );

/*********************************************************************
 * API FUNCTIONS
 */


/*
 * SimpleProfile_AddService- Initializes the Simple GATT Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */

extern bStatus_t SimpleProfile_AddService( void );

/*
 * SimpleProfile_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t SimpleProfile_RegisterAppCB( simpleProfileWrite_t writeCB,
                                              simpleProfileCCCD_t cccdCB );

/*
 * SimpleProfile_SetParameter - Set a Simple GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t SimpleProfile_SetParameter( uint8 param, uint8 len, void *value );

/*
 * SimpleProfile_GetParameter - Get a Simple GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t SimpleProfile_GetParameter( uint8 param, void *value );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEGATTPROFILE_H */
