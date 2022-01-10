/******************************************************************************

   @file    led_service.h

   @brief   This file contains the LED_Service service definitions and
          prototypes.


   Group: WCS, BTS
   $Target Device: DEVICES $

 ******************************************************************************
   $License: BSD3 2015 $
 ******************************************************************************
   $Release Name: PACKAGE NAME $
   $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef _LED_SERVICE_H_
#define _LED_SERVICE_H_

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
#define LED_SERVICE_SERV_UUID 0x1110
#define LED_SERVICE_SERV_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, \
    0xF0

// LED0 Characteristic defines
#define LS_LED0_ID                 0
#define LS_LED0_UUID               0x1111
#define LS_LED0_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0xF0
#define LS_LED0_LEN                1
#define LS_LED0_LEN_MIN            1

// LED1 Characteristic defines
#define LS_LED1_ID                 1
#define LS_LED1_UUID               0x1112
#define LS_LED1_UUID_BASE128(uuid) 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0xB0, 0x00, 0x40, 0x51, 0x04, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0xF0
#define LS_LED1_LEN                1
#define LS_LED1_LEN_MIN            1

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
typedef void (*LedServiceChange_t)(uint16_t connHandle, uint8_t paramID,
                                   uint16_t len, uint8_t *pValue);

typedef struct
{
    LedServiceChange_t pfnChangeCb;          // Called when characteristic value changes
    LedServiceChange_t pfnCfgChangeCb;       // Called when characteristic CCCD changes
} LedServiceCBs_t;

/*********************************************************************
 * API FUNCTIONS
 */

/*
 * LedService_AddService- Initializes the LedService service by registering
 *          GATT attributes with the GATT server.
 *
 *    rspTaskId - The ICall Task Id that should receive responses for Indications.
 */
extern bStatus_t LedService_AddService(uint8_t rspTaskId);

/*
 * LedService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t LedService_RegisterAppCBs(LedServiceCBs_t *appCallbacks);

/*
 * LedService_SetParameter - Set a LedService parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
extern bStatus_t LedService_SetParameter(uint8_t param,
                                         uint16_t len,
                                         void *value);

/*
 * LedService_GetParameter - Get a LedService parameter.
 *
 *    param - Profile parameter ID
 *    len   - pointer to a variable that contains the maximum length that can be written to *value.
              After the call, this value will contain the actual returned length.
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
extern bStatus_t LedService_GetParameter(uint8_t param,
                                         uint16_t *len,
                                         void *value);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _LED_SERVICE_H_ */
