/******************************************************************************

   @file    button_service.h

   @brief   This file contains the Button_Service service definitions and
          prototypes.


   Group: WCS, BTS
   $Target Device: DEVICES $

 ******************************************************************************
   $License: BSD3 2015 $
 ******************************************************************************
   $Release Name: PACKAGE NAME $
   $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef _BUTTON_SERVICE_H_
#define _BUTTON_SERVICE_H_

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
#define BUTTON_SERVICE_SERV_UUID 0x1120
#define BUTTON_SERVICE_SERV_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), \
    0x00, 0xF0

// BUTTON0 Characteristic defines
#define BS_BUTTON0_ID                 0
#define BS_BUTTON0_UUID               0x1121
#define BS_BUTTON0_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0xF0
#define BS_BUTTON0_LEN                1
#define BS_BUTTON0_LEN_MIN            1

// BUTTON1 Characteristic defines
#define BS_BUTTON1_ID                 1
#define BS_BUTTON1_UUID               0x1122
#define BS_BUTTON1_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0xF0
#define BS_BUTTON1_LEN                1
#define BS_BUTTON1_LEN_MIN            1

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
typedef void (*ButtonServiceChange_t)(uint16_t connHandle, uint8_t paramID,
                                      uint16_t len, uint8_t *pValue);

typedef struct
{
    ButtonServiceChange_t pfnChangeCb;          // Called when characteristic value changes
    ButtonServiceChange_t pfnCfgChangeCb;       // Called when characteristic CCCD changes
} ButtonServiceCBs_t;

/*********************************************************************
 * API FUNCTIONS
 */

/*
 * ButtonService_AddService- Initializes the ButtonService service by registering
 *          GATT attributes with the GATT server.
 *
 *    rspTaskId - The ICall Task Id that should receive responses for Indications.
 */
extern bStatus_t ButtonService_AddService(uint8_t rspTaskId);

/*
 * ButtonService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t ButtonService_RegisterAppCBs(ButtonServiceCBs_t *appCallbacks);

/*
 * ButtonService_SetParameter - Set a ButtonService parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
extern bStatus_t ButtonService_SetParameter(uint8_t param,
                                            uint16_t len,
                                            void *value);

/*
 * ButtonService_GetParameter - Get a ButtonService parameter.
 *
 *    param - Profile parameter ID
 *    len   - pointer to a variable that contains the maximum length that can be written to *value.
              After the call, this value will contain the actual returned length.
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
extern bStatus_t ButtonService_GetParameter(uint8_t param,
                                            uint16_t *len,
                                            void *value);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _BUTTON_SERVICE_H_ */
