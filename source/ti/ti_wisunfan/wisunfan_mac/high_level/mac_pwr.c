/******************************************************************************

 @file  mac_pwr.c

 @brief This module implements high level procedures for power management.

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
#include "ti_wisunfan_features.h"
#endif

#include "timac_api.h"
#include "mac_main.h"
#include "mac_pwr.h"
#include "mac_pib.h"
#include "mac_low_level.h"
#include "icall_osal_map_direct.h"
#include "hal_mcu.h"
#include "mac_radio.h"
#include "mac_symbol_timer.h"
#include "mac.h"
#include "mac_radio_defs.h"

/******************************************************************************
  ROM jump table will support
  1. TIMAC ROM image build
  2. TIMAC stack build without ROM image
  3. TIMAC stack with calling ROM image
 *****************************************************************************/
#include "rom_jt_154.h"

// SW Tracer
#include "dbg.h"
#ifdef DEBUG_SW_TRACE
#define DBG_ENABLE
#ifndef _DBGFILE
#define _DBGFILE mac_pwr_c
#endif
#include "dbgid_sys_mst.h"
#endif // DEBUG_SW_TRACE

/* RAT minimum remaining time */
#define RAT_MIN_REMAINING_TIME (300 * MAC_RAT_MHZ )

#define FEATURE_POWER_SAVING

/* ------------------------------------------------------------------------------------------------
 *                                           Typedefs
 * ------------------------------------------------------------------------------------------------
 */


/* ------------------------------------------------------------------------------------------------
 *                                           Local Variables
 * ------------------------------------------------------------------------------------------------
 */

macPwr_t macPwr;


/**************************************************************************************************
 * @fn          macPwrReset
 *
 * @brief       This function initializes the data structures for the pwr module.
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
MAC_INTERNAL_API void macPwrReset(void)
{
  macPwr.mode = MAC_PWR_ON;
}

/**************************************************************************************************
 * @fn          macApiPwrOnReq
 *
 * @brief       This function handles an API power on request event.  If macPwr.mode
 *              indicates the radio is already on it calls the MAC callback with MAC_PWR_ON_CNF
 *              immediately.
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
MAC_INTERNAL_API void macApiPwrOnReq(macEvent_t *pEvent)
{
  (void)pEvent;  // Intentionally unreferenced parameter
}

/**************************************************************************************************
 * @fn          MAC_PwrOffReq
 *
 * @brief       This direct execute function requests the MAC to power off and radio hardware
 *              and go to sleep.  It first checks if the mac is in idle or polling state by calling
 *              macStateIdleOrPolling() and also whether the receiver is off.  It also checks whether
 *              the next MAC timer expiration is greater than a minimum threshold value.  If
 *              all of the above indicates the MAC can be powered down it calls macRadioPwrOff(),
 *              sets macPwr.mode and returns MAC_SUCCESS.  Otherwise it returns MAC_DENIED.
 *              If macPwr.mode indicates the radio is already off MAC_SUCCESS is returned
 *              immediately.
 *
 * input parameters
 *
 * @param       mode - The desired low power mode.
 *
 * output parameters
 *
 * None.
 *
 * @return      The status of the request, as follows:
 *              MAC_SUCCESS  Operation successful; the MAC is powered off.
 *              MAC_DENIED  The MAC was not able to power off.
 **************************************************************************************************
 */
uint8 MAC_PwrOffReq(uint8 mode)
{
  /* Check if we are already powered down */
  if (macPwr.mode == mode)
  {
    return MAC_SUCCESS;
  }

  /* Verify it is ok to sleep */
  if ((macPwr.mode == MAC_PWR_ON) && MAP_macStateIdleOrPolling() && macSleep(mode))
  {
    macPwr.mode = mode;
    return MAC_SUCCESS;
  }

  return MAC_DENIED;
}

/**************************************************************************************************
 * @fn          MAC_PwrOnReq
 *
 * @brief       This function handles an API power on request event.
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
void MAC_PwrOnReq(void)
{
  if (macPwr.mode != MAC_PWR_ON)
  {
    macSleepWakeUp();
    macPwr.mode = MAC_PWR_ON;
  }  
}

/**************************************************************************************************
 * @fn          MAC_PwrMode
 *
 * @brief       This function returns the current power mode of the MAC.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      The current power mode of the MAC.
 **************************************************************************************************
 */
uint8 MAC_PwrMode(void)
{
  return macPwr.mode;
}

/**************************************************************************************************
 * @fn          macPwrVote
 *
 * @brief       This function votes for power management state based on entire
 *              MAC internal function states.
 *              This function is thread-safe.
 *
 * input parameters
 *
 * @param       pwrUp     - Indicates if radio power-up is needed
 *              bypassRAT - Indicates if RAT disabling and restoration need to be avoided
 *
 * output parameters
 *
 * None.
 *
 * @return      None
 **************************************************************************************************
 */
MAC_INTERNAL_API void macPwrVote(bool pwrUp, bool bypassRAT)
{
}


/**************************************************************************************************
 *                                  Compile Time Integrity Checks
 **************************************************************************************************
 */

#if (MAC_PWR_SLEEP_LITE != MAC_SLEEP_STATE_OSC_OFF)
#error "ERROR:  Mismatch in paramter values."
#endif

#if (MAC_PWR_SLEEP_DEEP != MAC_SLEEP_STATE_RADIO_OFF)
#error "ERROR:  Mismatch in paramter values."
#endif


/**************************************************************************************************
*/
