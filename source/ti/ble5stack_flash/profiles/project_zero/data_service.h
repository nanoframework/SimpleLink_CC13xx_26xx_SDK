/******************************************************************************

   @file    data_service.h

   @brief   This file contains the Data_Service service definitions and
          prototypes.


   Group: WCS, BTS
   $Target Device: DEVICES $

 ******************************************************************************
   $License: BSD3 2015 $
 ******************************************************************************
   $Release Name: PACKAGE NAME $
   $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef _DATA_SERVICE_H_
#define _DATA_SERVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <bcomdef.h>

/*********************************************************************
 * CONSTANTS
 */
// Service UUID
#define DATA_SERVICE_SERV_UUID 0x1130
#define DATA_SERVICE_SERV_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, \
    0xF0

// String Characteristic defines
#define DS_STRING_ID                 0
#define DS_STRING_UUID               0x1131
#define DS_STRING_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0xF0
#define DS_STRING_LEN                40
#define DS_STRING_LEN_MIN            0

// Stream Characteristic defines
#define DS_STREAM_ID                 1
#define DS_STREAM_UUID               0x1132
#define DS_STREAM_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0xF0
#define DS_STREAM_LEN                20
#define DS_STREAM_LEN_MIN            0

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*DataServiceChange_t)(uint16_t connHandle, uint8_t paramID,
                                    uint16_t len, uint8_t *pValue);

typedef struct
{
    DataServiceChange_t pfnChangeCb;          // Called when characteristic value changes
    DataServiceChange_t pfnCfgChangeCb;       // Called when characteristic CCCD changes
} DataServiceCBs_t;

/*********************************************************************
 * API FUNCTIONS
 */

/*
 * DataService_AddService- Initializes the DataService service by registering
 *          GATT attributes with the GATT server.
 *
 *    rspTaskId - The ICall Task Id that should receive responses for Indications.
 */
extern bStatus_t DataService_AddService(uint8_t rspTaskId);

/*
 * DataService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t DataService_RegisterAppCBs(DataServiceCBs_t *appCallbacks);

/*
 * DataService_SetParameter - Set a DataService parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
extern bStatus_t DataService_SetParameter(uint8_t param,
                                          uint16_t len,
                                          void *value);

/*
 * DataService_GetParameter - Get a DataService parameter.
 *
 *    param - Profile parameter ID
 *    len   - pointer to a variable that contains the maximum length that can be written to *value.
              After the call, this value will contain the actual returned length.
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
extern bStatus_t DataService_GetParameter(uint8_t param,
                                          uint16_t *len,
                                          void *value);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _DATA_SERVICE_H_ */
