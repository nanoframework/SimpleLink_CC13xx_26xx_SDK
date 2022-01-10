/******************************************************************************

 @file  mac_rf4ce.c

 @brief This module implements RF4CE specific MAC functionality.

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
#include "ti_154stack_features.h"
#endif

/* hal */

/* osal */
#include "osal.h"

/* mac */
#include "mac_api.h"
#include "mac_low_level.h"
#include "mac_spec.h"
#include "mac_main.h"
#include "mac_timer.h"
#include "mac_mgmt.h"
#include "mac_pib.h"
#include "mac_data.h"
#include "mac_scan.h"
#include "mac_device.h"
#include "mac_coord.h"
#include "mac_assert.h"
#include "mac_pwr.h"

/* mac_rf4ce */
#include "mac_rf4ce.h"


#ifndef RCN_FEATURE_TARGET
#define RCN_FEATURE_TARGET TRUE
#endif


static void macNullAction(macEvent_t *pEvent);
extern void MAC_SetRandomSeedCB(macRNGFcn_t pCBFcn);
static void macSeedCB(uint8 *seed);
uint8 macStrongRandomSeed[MAC_RANDOM_SEED_LEN];

/* ------------------------------------------------------------------------------------------------
 *                                           Local Variables
 * ------------------------------------------------------------------------------------------------
 */

/* Mgmt action set 2 */
const macAction_t macMgmtRf4ceAction2[] =
{
  macApiPwrOnReq,
  macNullAction,
  macNullAction,
  macNoAction, // Beacon message event - we don't want any warnings on receiving a beacon message
  macNullAction
};

/* Coordinator action set 1 */
static const macAction_t macRf4ceCoordAction1[] =
{
  macApiStartReq,
  macStartContinue
};

/* Coordinator action set 2 */
static const macAction_t macRf4ceCoordAction2[] =
{
  macNullAction,
  macNullAction,
  macRxBeaconReq,
  macNullAction,
  macStartComplete,
  macNullAction
};

/* Device action set 1 */
static const macAction_t macRf4ceDeviceAction1[] =
{
  macNullAction,
  macNullAction,
  macNullAction,
  macNullAction,
  macNullAction,
  macNullAction,
  macNullAction,
  macNullAction,
  macNullAction,
  macNullAction,
  macNullAction,
  macNullAction,
  macStartFrameResponseTimer,
  macStartResponseTimer
};

/* Device action set 2 */
static const macAction_t macRf4ceDeviceAction2[] =
{
  macNullAction,
  macNullAction,
  macNullAction,
  macNullAction,
  macNullAction
};


/**************************************************************************************************
 * @fn          MAC_InitRf4ce
 *
 * @brief       This function initializes MAC for RF4CE.
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
void MAC_InitRf4ce(void)
{
  macActionSet[MAC_MGMT_ACTION_1] = macMgmtAction1;

  macMain.featureMask = 0;

  /* Must be called before macLowLevelInit(). */
  MAC_SetRandomSeedCB(macSeedCB);  
  macLowLevelInit();
}


/**************************************************************************************************
 * @fn          MAC_InitRf4ceDevice
 *
 * @brief       This function intializes the RF4CE MAC as a device.
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
void MAC_InitRf4ceDevice(void)
{
  macActionSet[MAC_MGMT_ACTION_2]   = macMgmtRf4ceAction2;

  macActionSet[MAC_DEVICE_ACTION_1] = macRf4ceDeviceAction1;
  macActionSet[MAC_DEVICE_ACTION_2] = macRf4ceDeviceAction2;

  macReset[MAC_FEAT_DEVICE] = &macDeviceReset;

  macMain.featureMask |= MAC_FEAT_DEVICE_MASK;

  if (RCN_FEATURE_TARGET)
  {
    macActionSet[MAC_SCAN_ACTION] = macScanAction;
    MAC_InitRf4ceCoord();
  }
}


/**************************************************************************************************
 * @fn          MAC_InitRf4ceCoord
 *
 * @brief       This function initializes the RF4CE MAC as a coordinator.  It loads coordinator
 *              action functions into the state machine action table.  It also configures the
 *              state machine to use state tables required for operation as a device.
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
void MAC_InitRf4ceCoord(void)
{
  macActionSet[MAC_COORD_ACTION_1] = macRf4ceCoordAction1;
  macActionSet[MAC_COORD_ACTION_2] = macRf4ceCoordAction2;

  macStateTbl[MAC_STARTING_ST]     = macStartingSt;

  macReset[MAC_FEAT_COORD] = &macCoordReset;

  macMain.featureMask |= MAC_FEAT_COORD_MASK;
}


/**************************************************************************************************
 * @fn          macNullAction
 *
 * @brief       This function asserts when an unexpected event occurs.
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
static void macNullAction(macEvent_t *pEvent)
{
  (void) pEvent; // unused argument
  
  MAC_ASSERT_FORCED();
}


/**************************************************************************************************
 * @fn          macSeedCB
 *
 * @brief       This function copies the random seed to macStrongRandomSeed
 *
 * input parameters
 *
 * @param       seed - Random seed generated by the macLowLevelInit().
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
static void macSeedCB(uint8 *seed)
{
  (void)MAP_osal_memcpy(macStrongRandomSeed, seed, MAC_RANDOM_SEED_LEN);
}

