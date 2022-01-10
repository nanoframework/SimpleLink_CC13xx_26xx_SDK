/******************************************************************************

 @file  mac_symbol_timer.c

 @brief Describe the purpose and contents of the file.

 Group: WCS, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2021, Texas Instruments Incorporated
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
 *                                           Includes
 * ------------------------------------------------------------------------------------------------
 */

#ifndef LIBRARY
#include "ti_wisunfan_features.h"
#endif

#include <stdint.h>

/* hal */
#include "hal_types.h"
#include "hal_mcu.h"

/* high-level specific */
#include "mac_spec.h"
#include "mac_timer.h"

/* exported low-level */
#include "mac_low_level.h"

/* low-level specific */
#include "mac_symbol_timer.h"
#include "mac_tx.h"
#include "mac_rx.h"
#include "mac_rx_onoff.h"
#include "mac_settings.h"

/* target specific */
#include "mac_radio_defs.h"

/* mac mcu */
#include "mac_mcu.h"

#include "mac.h"

/* debug */
#include "mac_assert.h"

/* access PIB */
#include "mac_pib.h"

// SW Tracer
#include "dbg.h"
#ifdef DEBUG_SW_TRACE
#define DBG_ENABLE
#ifndef _DBGFILE
#define _DBGFILE mac_symbol_timer_c
#endif
#include "dbgid_sys_mst.h"
#endif // DEBUG_SW_TRACE

#ifndef OSAL_PORT2TIRTOS
#include <icall.h>
#include <icall_cc26xx_defs.h>
#include <hw_rfc_pwr.h>
#include <osal.h>
#include <osal_pwrmgr.h>
#include "mac_radio.h"
#else
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/sysbios/knl/Clock.h>
#include <hw_rfc_pwr.h>
#include "mac_radio.h"
#endif /* OSAL_PORT2TIRTOS */
#include "mac_main.h"

#include "mac_mgmt.h"
#include "rom_jt_154.h"
/* ------------------------------------------------------------------------------------------------
 *                                            Defines
 * ------------------------------------------------------------------------------------------------
 */
#define COMPARE_STATE_ROLLOVER_BV                 BV(0)
#define COMPARE_STATE_TRIGGER_BV                  BV(1)
#define COMPARE_STATE_ARM_BV                      BV(2)

#define COMPARE_STATE_TRIGGER                     COMPARE_STATE_TRIGGER_BV
#define COMPARE_STATE_ROLLOVER                    COMPARE_STATE_ROLLOVER_BV
#define COMPARE_STATE_ROLLOVER_AND_TRIGGER        (COMPARE_STATE_ROLLOVER_BV | COMPARE_STATE_TRIGGER_BV)
#define COMPARE_STATE_ROLLOVER_AND_ARM_TRIGGER    (COMPARE_STATE_ROLLOVER_BV | COMPARE_STATE_ARM_BV)

#if 0 // TODO: Recalculate for subG
/*
 *  The datasheet mentions a small delay on both receive and transmit side of approximately
 *  two microseconds.  The precise characterization is given below.
 *  (This data is not given in rev 1.03 datasheet)
 */
#define RX_TX_PROP_DELAY_AVG_USEC         ((MAC_RADIO_RX_TX_PROP_DELAY_MIN_USEC + MAC_RADIO_RX_TX_PROP_DELAY_MAX_USEC) / 2)
#define RX_TX_PROP_DELAY_AVG_TIMER_TICKS  ((uint16)(MAC_RADIO_TIMER_TICKS_PER_USEC() * RX_TX_PROP_DELAY_AVG_USEC + 0.5))

/*
 *  For slotted receives, the SFD signal is expected to occur on a specifc symbol boundary.
 *  This does *not* correspond to the symbol boundary.  The SFD signal occurs at an
 *  offset from the symbol boundary.  This is done for efficiency of related algorithms.
 *
 *  Once transmit is strobed there is a fixed delay until the SFD signal occurs.  The frame
 *  does not start over-the-air transmit until after an internal radio delay of 12 symbols.
 *  Once transmitting over-the-air, the preamble is sent (8 symbols) followed by the
 *  SFD field (2 symbols). After the SFD field completes, the SFD signal occurs.  This
 *  adds up to a total of 22 symbols from strobe to SFD signal.
 *
 *  Since 22 symbols spans more than a symbol (20 symbols) the modulus operation is used
 *  to find the symbol offset which is 2 symbols.
 *
 *  This math is derived formally via the pre-processor.
 */
#define SYMBOLS_FROM_STROBE_TO_PREAMBLE   12 /* from datasheet */
#define SYMBOLS_FROM_PREAMBLE_TO_SFD      (MAC_SPEC_PREAMBLE_FIELD_LENGTH + MAC_SPEC_SFD_FIELD_LENGTH)
#define SYMBOLS_FROM_STROBE_TO_SFD        (SYMBOLS_FROM_STROBE_TO_PREAMBLE + SYMBOLS_FROM_PREAMBLE_TO_SFD)
#define SYMBOLS_EXPECTED_AT_SFD           (SYMBOLS_FROM_STROBE_TO_SFD % MAC_A_UNIT_BACKOFF_PERIOD)

/* after all that formal math, make sure the result is as expected */
#if (SYMBOLS_EXPECTED_AT_SFD != 2)
#error "ERROR! Internal problem with pre-processor math of slotted alignment."
#endif


/*
 *  The expected SFD signal occurs at the symbol offset *plus* a small internal propagation delay
 *  internal to the radio.  This delay is given as the sum of a receive side delay and a transmit
 *  side delay.  When this delay is subtracted from the internal timer, the internal time base
 *  actually becomes the actual receive time *minus* the transmit delay.  This works out though.
 *  The transmit logic does *not* take into account this delay.  Since the timer is skewed by the
 *  transmit delay already, the transmits go out precisely on time.
 */
#define TIMER_TICKS_EXPECTED_AT_SFD   ((SYMBOLS_EXPECTED_AT_SFD * MAC_RADIO_TIMER_TICKS_PER_SYMBOL()) \
                                          + RX_TX_PROP_DELAY_AVG_TIMER_TICKS)
#endif

#if defined USE_ICALL || defined OSAL_PORT2TIRTOS

#ifndef MAC_SYMBOL_TIMER_ADDITIONAL_WAKEUP_LATENCY
/** Additional wakeup latency in term of microseconds */
#define MAC_SYMBOL_TIMER_ADDITIONAL_WAKEUP_LATENCY    2300
#endif /* MAC_SYMBOL_TIMER_ADDITIONAL_WAKEUP_LATENCY */

#define MAC_SYMBOL_TIMER_UPDATE_WAKEUP()

#define MAC_SYMBOL_TIMER_BC_TIMER_EVENT                  0x08

#else /* defined USE_ICALL || defined OSAL_PORT2TIRTOS */
#define MAC_SYMBOL_TIMER_UPDATE_WAKEUP()
#endif /* defined USE_ICALL || defined OSAL_PORT2TIRTOS */

#define xDEBUG_SLEEP_LED

/* Use LED2 for sleep debug with MODULE_CC13XX_7X7 */
#ifdef DEBUG_SLEEP_LED
#define DEBUG_SLEEP_LED_ON()  HWREG(GPIO_BASE+GPIO_O_DOUTSET31_0) = (1UL<<IOID_27)
#define DEBUG_SLEEP_LED_OFF() HWREG(GPIO_BASE+GPIO_O_DOUTCLR31_0) = (1UL<<IOID_27)
#else
#define DEBUG_SLEEP_LED_ON()
#define DEBUG_SLEEP_LED_OFF()
#endif /* SLEEP_DEBUG_LED */


/* ------------------------------------------------------------------------------------------------
 *                                         External Variables
 * ------------------------------------------------------------------------------------------------
 */
#if 0
extern const uint32 CODE macTimerRolloverValue[];
#endif

/* ------------------------------------------------------------------------------------------------
 *                                         Global Variables
 * ------------------------------------------------------------------------------------------------
 */
uint32 macSymbolTimerRollover;
uint32 macPrevPeriodRatCount;
uint8  macSpecUsecsPerSymbol;

#if defined USE_ICALL || defined OSAL_PORT2TIRTOS
volatile uint8 macSymbolTimerImpending;
#endif /* defined USE_ICALL || defined OSAL_PORT2TIRTOS */


/* ------------------------------------------------------------------------------------------------
 *                                         Local Variables
 * ------------------------------------------------------------------------------------------------
 */
uint32 macSymbolTimerTrigger;

#ifndef OSAL_PORT2TIRTOS
/* ICall timer ID used to replicate symbol timer to affect power module */
static ICall_TimerID macSymbolTimerICallTimerID = ICALL_INVALID_TIMER_ID;

/* Data for power state transition notify function registration */
static ICall_PwrNotifyData macSymbolTimerICallPwrNotifyData;
#else
static Clock_Handle macSymbolWakeupClock = NULL;
#endif /* defined OSAL_PORT2TIRTOS */

#if defined USE_ICALL || defined OSAL_PORT2TIRTOS

/* Synchronization events */
static uint8 macSymbolTimerEvents;

/* ------------------------------------------------------------------------------------------------
 *                                         Forward References
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
 * @fn          macSymbolTimerICallTimerCback
 *
 * @brief       ICall timer callback function
 *
 * @param       arg   meaningless
 *
 * @return      none
 **************************************************************************************************
 */
void macSymbolTimerICallTimerCback(void *arg)
{
  /* Timer must be synchronized to the potential handling of
   * wakeup from sleep state in case the timer callback is made
   * as soon as the device wakes up before radio is turned back on.
   * Hence, signal the event to the OSAL thread. */
  halIntState_t is;

  HAL_ENTER_CRITICAL_SECTION(is);
  macSymbolTimerEvents |= MAC_SYMBOL_TIMER_EVENT_POWER_TIMER_EXP;
  HAL_EXIT_CRITICAL_SECTION(is);
#ifndef OSAL_PORT2TIRTOS
  ICall_signal(osal_semaphore);
#else
  MAP_osal_set_event(macTaskId, 0);
#endif /* OSAL_PORT2TIRTOS */
}
#endif /* defined USE_ICALL || defined OSAL_PORT2TIRTOS */

/**************************************************************************************************
 * @fn          macSymbolTimerInit
 *
 * @brief       Intializes symbol timer.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macSymbolTimerInit(void)
{
  macPrevPeriodRatCount = macSymbolTimerRollover = 0;

  /* macSymbolTimerTrigger has to be set to maximum possible value of
   * macSymbolTimerRollover value initially.
   * Otherwise, incorrect macSymbolTimerTrigger value shall be compared
   * all the time in macSymbolTimerUpdateWakeup() function, casting
   * incorrect vote.
   */
  macSymbolTimerTrigger = MAC_SYMBOL_TIMER_DEFAULT_ROLLOVER;

#if defined USE_ICALL || defined OSAL_PORT2TIRTOS
  // Clear events
  macSymbolTimerEvents = 0;

#ifndef OSAL_PORT2TIRTOS
  // Register hook function to handle events.
  if (!osal_eventloop_hook)
  {
    /* Don't overwrite if the hook is already set up.
     * Note that the hook might have been set up to perform other things
     * as well in which case the other hook function has to call
     * macSymbolTimerEventHandler.
     */
    osal_eventloop_hook = macSymbolTimerEventHandler;
  }
#endif /* USE_ICALL */

  macSymbolTimerImpending = FALSE;

#ifndef OSAL_PORT2TIRTOS
  /* Start timer just to initialize the timer ID to reuse in the module.
   * This also serves the purpose of allocating resources upfront,
   * in order to prevent a case of running out of timer resource
   * when the timer has to be started.
   * Note that macSymbolTimerSetRollover() may trigger setting timer
   * and hence the timer set up must happen before macSymbolTimerSetRollover()
   * call. */
  if (ICall_setTimer(1, macSymbolTimerICallTimerCback, NULL,
                     &macSymbolTimerICallTimerID) != ICALL_ERRNO_SUCCESS)
  {
    MAC_ASSERT(0);
  }
#else
  if (!macSymbolWakeupClock)
  {
    Clock_Params params;
    Clock_Params_init(&params);
    params.startFlag = FALSE;
    params.period = 0;
    macSymbolWakeupClock = Clock_create((Clock_FuncPtr) macSymbolTimerICallTimerCback,
                                         1, &params, NULL);
    MAC_ASSERT(macSymbolWakeupClock);
    Clock_start(macSymbolWakeupClock);
  }
  else
  {
      uint32_t key;
      key = OsalPort_enterCS();
      if(Clock_isActive(macSymbolWakeupClock))
      {
          Clock_stop(macSymbolWakeupClock);
      }
      Clock_setFunc(macSymbolWakeupClock, (Clock_FuncPtr) macSymbolTimerICallTimerCback, NULL);
      OsalPort_leaveCS(key);
      Clock_setTimeout(macSymbolWakeupClock, 1);
      Clock_start(macSymbolWakeupClock);
  }
#endif /* OSAL_PORT2TIRTOS */

  /* Note that MAC_PWR_VOTE_NO_SWI() is called done from macSymbolTimerSetRollover()
   * call and hence there is no need to make the call here. */
#endif /* defined USE_ICALL || defined OSAL_PORT2TIRTOS */
#if 1
  MAP_macTimerSetRollover(MAC_BO_NON_BEACON);
#else
  macSymbolTimerSetRollover(MAC_SYMBOL_TIMER_DEFAULT_ROLLOVER);
#endif
  /* Since interrupt disable/enable mechanism is not implemented for symbol
   * timer trigger interrupt for CC13xx, comparator value has to be set
   * so that the timer trigger interrupt is not triggered.
   * See comment inside macSymbolTimerCancelTrigger() for relevant
   * information */
  MAC_RADIO_SYMBOL_SET_COMPARE(macSymbolTimerTrigger);
}

/**************************************************************************************************
 * @fn          macSymbolTimerSetRollover
 *
 * @brief       Set rollover count of symbol timer.
 *
 * @param       rolloverSymbol - symbol count where count is reset to zero
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macSymbolTimerSetRollover(uint32 rolloverSymbol)
{
}

/**************************************************************************************************
 * @fn          macSymbolTimerCount
 *
 * @brief       Returns the current symbol count.
 *
 * @param       none
 *
 * @return      current symbol count
 **************************************************************************************************
 */
MAC_INTERNAL_API uint32 macSymbolTimerCount(void)
{
  halIntState_t  s;
  uint32 symbolCount;

  HAL_ENTER_CRITICAL_SECTION(s);
  symbolCount = MAC_RADIO_SYMBOL_COUNT();
  HAL_EXIT_CRITICAL_SECTION(s);

  return(symbolCount);
}

/*************************************************************************************************
 * @fn          macCsmaTimerExpiry
 *
 * @brief       Throw an event for timer expiry
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macCsmaTimerExpiry(void)
{
    MAP_macSetEvent16(MAC_CSMA_TIM_TASK_EVT);
}
/**************************************************************************************************
 * @fn          macSymbolTimerBCWakeUp
 *
 * @brief       properly use rollover value for the next wakeup time
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */

MAC_INTERNAL_API void macSymbolTimerBCWakeUp(void)
{
    halIntState_t is;

    /* Note that radio has to be turned on before updating the RAT timer. */
    MAC_PWR_VOTE_NO_SWI(TRUE);

    HAL_ENTER_CRITICAL_SECTION(is);
    DBG_PRINT0(DBGSYS, "macSymbolTimerEventHandler()");
    /* Update wakeup schedule, which most likely would vote not to enter
     * sleep state. */
    MAC_SYMBOL_TIMER_UPDATE_WAKEUP();
    HAL_EXIT_CRITICAL_SECTION(is);
    MAC_FH_BC_TIMER_EVT();
    return;
}
