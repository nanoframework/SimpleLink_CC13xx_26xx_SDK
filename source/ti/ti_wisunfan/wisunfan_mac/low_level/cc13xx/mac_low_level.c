/******************************************************************************

 @file  mac_low_level.c

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

/* PRCM */
#include "hw_prcm.h"

/* driverlib */
#include "hw_types.h"
#include "interrupt.h"

/* hal */
#include "hal_defs.h"
#include "hal_types.h"
#include "hal_mcu.h"

#include "saddr.h"

/* mailbox */
#include "mac.h"

/* exported low-level */
#include "mac_low_level.h"

/* low-level specific */
#include "mac_radio.h"
#include "mac_rx.h"
#include "mac_tx.h"
#include "mac_rx_onoff.h"
#include "mac_symbol_timer.h"

/* high-level specific */
#include "mac_main.h"
#include "mac_pib.h"

/* target specific */
#include "mac_radio_defs.h"

#ifndef OSAL_PORT2TIRTOS
#include <icall.h>
#include <icall_cc26xx_defs.h>
#include <hw_rfc_pwr.h>
#else
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <hw_rfc_pwr.h>
#endif /* OSAL_PORT2TIRTOS */

/* debug */
#include "mac_assert.h"

// SW Tracer
#include "dbg.h"
#ifdef DEBUG_SW_TRACE
#define DBG_ENABLE
#ifndef _DBGFILE
#define _DBGFILE mac_low_level_c
#endif
#include "dbgid_sys_mst.h"
#endif // DEBUG_SW_TRACE

#include "rf_mac_api.h"
#include "mac_settings.h"

#include "rom_jt_154.h"


/**************************************************************************************************
 * @fn          macLowLevelInit
 *
 * @brief       Initialize low-level MAC.  Called only once on system power-up.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macLowLevelInit(void)
{
  /* Radio hardware initialziation */
  macRadioInit();

  /* Radio software initialziation */
  macRadioSwInit();

  /* At least any one non-dir/imm command should be posted to the RF driver
   * after initialization in order to post a dir/imm command.
   */
  macSetupFsCmd(pMacPib->logicalChannel);
  MAP_macPwrVote(TRUE, FALSE);
    
  /* initialize mcu before anything else */
  MAC_RADIO_MCU_INIT();

  /* Power vote */
  macRxOnOffInit();
  /* Initialize symbol timers */
  macSymbolTimerInit();
}

/**************************************************************************************************
 * @fn          macLowLevelBufferInit
 *
 * @brief       Initialize low-level MAC.  Called only once on system power-up. OSAL must be
 *              initialized before this function is called.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macLowLevelBufferInit(void)
{
  macRxInit();
  macTxInit();
}


/**************************************************************************************************
 * @fn          macLowLevelReset
 *
 * @brief       Reset low-level MAC.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macLowLevelReset(void)
{
}


/**************************************************************************************************
 * @fn          macLowLevelResume
 *
 * @brief       Resume the low-level MAC after a successful return from macLowLevelYield.
 *              Note: assuming interrupts are disabled.
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
MAC_INTERNAL_API void macLowLevelResume(void)
{
#ifdef COMBO_MAC
  macLowLevelBufferInit();
#endif
  macRadioInit();
}


/**************************************************************************************************
 * @fn          macLowLevelYield
 *
 * @brief       Check whether or not the low-level MAC is ready to yield.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      TRUE or FALSE whether the low-level MAC is ready to yield.
 **************************************************************************************************
 */
MAC_INTERNAL_API bool macLowLevelYield(void)
{
  bool rtrn = TRUE;
  halIntState_t  s;
  HAL_ENTER_CRITICAL_SECTION(s);

  // If RX or TX is active or any RX enable flags are set, it's not OK to yield.
  if (macRxActive || macRxOutgoingAckFlag || macTxActive || (macRxEnableFlags & ~MAC_RX_WHEN_IDLE))
  {
    rtrn = FALSE;
  }

  HAL_EXIT_CRITICAL_SECTION(s);
  return rtrn;
}


/**************************************************************************************************
 * @fn          macLowLevelDiags
 *
 * @brief       Increments a specified diagnostic counter (stored in the PIB).
 *
 * @param       pibAttribute - PIB attribute to be incremented.
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macLowLevelDiags( uint8 pibAttribute )
{
#if defined ( FEATURE_SYSTEM_STATS )
  if ( ( pibAttribute >= MAC_DIAGS_RX_CRC_PASS ) &&
       ( pibAttribute <= MAC_DIAGS_TX_SEC_FAIL ) )
  {
    uint32 value;

    /* Update Diagnostics counter */
    MAP_MAC_MlmeGetReq( pibAttribute, &value );
    value++;
    MAP_MAC_MlmeSetReq( pibAttribute, &value );
  }
#endif
}


/**************************************************************************************************
*/
