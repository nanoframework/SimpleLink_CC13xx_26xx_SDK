/******************************************************************************

 @file  oad_reset_service.h

 @brief This file contains OAD Reset Service header file.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2017 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
#ifndef OAD_RESET_SERVICE_H
#define OAD_RESET_SERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "hal_types.h"

/*********************************************************************
 * CONSTANTS
 */

#define OAD_RESET_SERVICE_UUID          0xFFD0
#define OAD_RESET_CHAR_UUID             0xFFD1

#define OAD_RESET_CMD_RESET_TGT         0x00
#define OAD_RESET_CMD_START_OAD         0x01

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */


typedef struct
{
  uint16_t connHandle;
  uint16_t bim_var;
} oadResetWrite_t;


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*oadResetWriteCB_t)(uint16_t connHandle,
                                  uint16_t bim_var);

typedef struct
{
  oadResetWriteCB_t       pfnOadWrite; // Called when characteristic value changes.
} oadUsrAppCBs_t;

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      Reset_addService
 *
 * @brief   Initializes the Reset Service by registering GATT attributes
 *          with the GATT server. Only call this function once.
 *
 * @param   None.
 *
 * @return  Success or Failure
 */
extern uint8_t Reset_addService(oadUsrAppCBs_t *pfnOadCBs);

#ifdef __cplusplus
}
#endif

#endif /* OAD_RESET_SERVICE_H */
