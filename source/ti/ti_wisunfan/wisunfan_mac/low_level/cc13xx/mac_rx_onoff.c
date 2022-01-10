/******************************************************************************

 @file  mac_rx_onoff.c

 @brief Describe the purpose and contents of the file.

 Group: WCS, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2006-2021, Texas Instruments Incorporated
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

/* hal */
#include "hal_defs.h"
#include "hal_types.h"

#include "mac.h"
#include "mac_pib.h"

/* exported low-level */
#include "mac_low_level.h"

/* low-level specific */
#include "mac_rx_onoff.h"
#include "mac_rx.h"
#include "mac_tx.h"
#include "mac_radio.h"

/* target specific */
#include "mac_radio_defs.h"

/* debug */
#include "mac_assert.h"

// SW Tracer
#include "dbg.h"
#ifdef DEBUG_SW_TRACE
#define DBG_ENABLE
#ifndef _DBGFILE
#define _DBGFILE mac_rx_onoff_c
#endif
#include "dbgid_sys_mst.h"
#endif // DEBUG_SW_TRACE

#include "rom_jt_154.h"

/* ------------------------------------------------------------------------------------------------
 *                                         Global Variables
 * ------------------------------------------------------------------------------------------------
 */
volatile uint8 macRxEnableFlags;


/**************************************************************************************************
 * @fn          macRxOnOffInit
 *
 * @brief       Initialize variables for rx on/off module.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macRxOnOffInit(void)
{
  macRxEnableFlags = 0;

  /* Power management state may change. Hence, vote. */
  MAC_PWR_VOTE_NO_SWI(FALSE);
}


/**************************************************************************************************
 * @fn          macRxEnable
 *
 * @brief       Set enable flags and then turn on receiver.
 *
 * @param       flags - byte containing rx enable flags to set
 *
 * @return      TRUE if successful, FALSE otherwise
 **************************************************************************************************
 */
MAC_INTERNAL_API bool macRxEnable(uint8 flags)
{
  MAC_ASSERT(flags != 0); /* rx flags not affected */
  
  /* set enable flags and then turn on receiver */
  macRxEnableFlags |= flags;
  DBG_PRINT2(DBGSYS, "macRxEnable(0x%02X), macRxEnableFlags=0x%02X", flags, macRxEnableFlags);

  return macRxOn();
}


/**************************************************************************************************
 * @fn          macRxSoftEnable
 *
 * @brief       Set enable flags but don't turn on the receiver.  Useful to leave the receiver
 *              on after a transmit, but without turning it on immediately.
 *
 * @param       flags - byte containing rx enable flags to set
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macRxSoftEnable(uint8 flags)
{
  halIntState_t  s;

  MAC_ASSERT(flags != 0); /* rx flags not affected */

  /* set the enable flags but do not turn on the receiver */
  HAL_ENTER_CRITICAL_SECTION(s);
  macRxEnableFlags |= flags;
  DBG_PRINT2(DBGSYS, "macRxSoftEnable(0x%02X), macRxEnableFlags=0x%02X", flags, macRxEnableFlags);
  HAL_EXIT_CRITICAL_SECTION(s);
}


/**************************************************************************************************
 * @fn          macRxDisable
 *
 * @brief       Clear indicated rx enable flags.  If all flags are clear, turn off receiver
 *              unless there is an active receive or transmit.
 *
 * @param       flags - byte containg rx enable flags to clear
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macRxDisable(uint8 flags)
{
  halIntState_t  s;

  MAC_ASSERT(flags != 0); /* rx flags not affected */
  
  /* clear the indicated flags */
  HAL_ENTER_CRITICAL_SECTION(s);
  macRxEnableFlags &= (flags ^ 0xFF);
  DBG_PRINT2(DBGSYS, "macRxDisable(0x%02X), macRxEnableFlags=0x%02X", flags, macRxEnableFlags);
  HAL_EXIT_CRITICAL_SECTION(s);

  /* turn off the radio if it is allowed */
  macRxOffRequest();

  /* Power management state may change. Hence, vote. */
  MAC_PWR_VOTE_NO_SWI(FALSE);
}

/**************************************************************************************************
 * @fn          macRxSoftDisable
 *
 * @brief       Clear given RX enable flags but don't turn off the receiver.
 *              Useful to turn the radio off when macRxOffRequest() is called later.
 *
 * @param       flags - byte containing rx enable flags to clear
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macRxSoftDisable(uint8 flags)
{
  halIntState_t  s;

  MAC_ASSERT(flags != 0); /* rx flags not affected */
  
  /* clear the indicated flags */
  HAL_ENTER_CRITICAL_SECTION(s);
  macRxEnableFlags &= (flags ^ 0xFF);
  DBG_PRINT2(DBGSYS, "macRxSoftDisable(0x%02X), macRxEnableFlags=0x%02X", flags, macRxEnableFlags);
  HAL_EXIT_CRITICAL_SECTION(s);
}

/**************************************************************************************************
 * @fn          macRxHardDisable
 *
 * @brief       Clear all enable flags and turn off receiver.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macRxHardDisable(void)
{
  DBG_PRINT0(DBGSYS, "macRxHardDisable()");
  macRxEnableFlags = 0;
  MAC_DEBUG_TURN_OFF_RX_LED();
  macRxOff();

  /* force off and clean up */
  macRxHaltCleanup();

  /* Power management state may change. Hence, vote. */
  MAC_PWR_VOTE_NO_SWI(FALSE);
}

/**************************************************************************************************
 * @fn          macRxOnRequest
 *
 * @brief       Turn on the receiver if any rx enable flag is set.
 *
 * @param       none
 *
 * @return      TRUE if successful, FALSE otherwise
 **************************************************************************************************
 */
MAC_INTERNAL_API bool macRxOnRequest(void)
{
  bool  rxOnSuccess = FALSE;

  DBG_PRINT1(DBGSYS, "macRxOnRequest(): macRxEnableFlags=0x%02X", macRxEnableFlags);

  if (macRxEnableFlags)
  {
    rxOnSuccess = macRxOn();
  }

  return rxOnSuccess;
}


/**************************************************************************************************
 * @fn          macRxOffRequest
 *
 * @brief       Turn off receiver if permitted.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macRxOffRequest(void)
{
  halIntState_t  s;
  uint8 doneFlag = 0;

  DBG_PRINT1(DBGSYS, "macRxOffRequest(): macRxEnableFlags=0x%02X", macRxEnableFlags);
  HAL_ENTER_CRITICAL_SECTION(s);
  if (!macRxEnableFlags)
  {
    doneFlag = !MAC_RX_IS_PHYSICALLY_ACTIVE() && !MAC_TX_IS_PHYSICALLY_ACTIVE();
    if (doneFlag)
    {
      macRxOff();
    }
  }
  HAL_EXIT_CRITICAL_SECTION(s);
}


/**************************************************************************************************
 * @fn          macRxOn
 *
 * @brief       Turn on the receiver if it's not already on.
 *
 * @param       none
 *
 * @return      TRUE if successful, FALSE otherwise
 **************************************************************************************************
 */
MAC_INTERNAL_API bool macRxOn(void)
{
  halIntState_t  s;
  bool rxOnSuccess;
  uint8_t macRxCmdActiveOrg;

  HAL_ENTER_CRITICAL_SECTION(s);
  macRxCmdActiveOrg = macRxCmdActive;

  if (macRxCmdActive != TRUE)
  {
    macRxCmdActive = TRUE;
    HAL_EXIT_CRITICAL_SECTION(s);
    
    /* Power management state may change. Hence, vote. */
    MAC_PWR_VOTE_NO_SWI(TRUE);
  }
  else
  {
    HAL_EXIT_CRITICAL_SECTION(s);
  }

  rxOnSuccess = MAC_RADIO_RX_ON();

  HAL_ENTER_CRITICAL_SECTION(s);
  if (rxOnSuccess != TRUE)
  {
    DBG_PRINT0(DBGSYS, "MAC_RADIO_RX_ON() failure");
    MAC_DEBUG_TURN_OFF_RX_LED();
    
    macRxCmdActive = macRxCmdActiveOrg;
    HAL_EXIT_CRITICAL_SECTION(s);
  }
  else
  {
    HAL_EXIT_CRITICAL_SECTION(s);
    DBG_PRINT0(DBGSYS, "MAC_RADIO_RX_ON()");
    MAC_DEBUG_TURN_ON_RX_LED();
  }
  
  /* Power management state may change. Hence, vote.
   * Note that even if radio is active for RX,
   * CM3 can go to sleep if there is nothing to do but
   * waiting for the RX callback.
   */
  MAC_PWR_VOTE_NO_SWI(FALSE);

  return rxOnSuccess;
}


/**************************************************************************************************
 * @fn          macRxOff
 *
 * @brief       Turn off the receiver if it's not already off.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macRxOff(void)
{
    if (MAC_FH_ENABLED)
    {
        // In FH mode, abort RX instead of gracefully stopping the Command
        MAC_RADIO_FH_RXTX_OFF();
    }
    else
    {
        MAC_RADIO_RXTX_OFF();
    }

    if(numRxPostCmd)
        numRxPostCmd--;
    MAC_DEBUG_TURN_OFF_RX_LED();
    DBG_PRINT0(DBGSYS, "MAC_RADIO_RXTX_OFF()");
}


/**************************************************************************************************
*/
