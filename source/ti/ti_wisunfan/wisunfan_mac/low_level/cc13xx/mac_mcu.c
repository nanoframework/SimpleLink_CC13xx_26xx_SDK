/******************************************************************************

 @file  mac_mcu.c

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

#include <stdint.h>
#ifdef __IAR_SYSTEMS_ICC__
#include "intrinsics.h"
#elif defined(__GNUC__) && !defined(__clang__)
/**
  \brief   Reverse bit order of value
  \details Reverses the bit order of the given value.
  \param [in]    value  Value to reverse
  \return               Reversed value
 */
__attribute__((always_inline)) static inline  uint32_t __RBIT(uint32_t value)
{
  uint32_t result;

#if       (__CORTEX_M >= 0x03U) || (__CORTEX_SC >= 300U)
   __ASM volatile ("rbit %0, %1" : "=r" (result) : "r" (value) );
#else
  int32_t s = 4 /*sizeof(v)*/ * 8 - 1; /* extra shift needed at end */

  result = value;                      /* r will be reversed bits of v; first get LSB of v */
  for (value >>= 1U; value; value >>= 1U)
  {
    result <<= 1U;
    result |= value & 1U;
    s--;
  }
  result <<= s;                        /* shift when v's highest bits are zero */
#endif
  return(result);
}

/**
  \brief   Reverse byte order (32 bit)
  \details Reverses the byte order in integer value.
  \param [in]    value  Value to reverse
  \return               Reversed value
 */
__attribute__((always_inline)) static inline uint32_t __REV(uint32_t value)
{
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
  return __builtin_bswap32(value);
#else
  uint32_t result;

  __ASM volatile ("rev %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
#endif
}
#endif

/* hal */
#include "hal_defs.h"
#include "hal_mcu.h"
#include <ti/drivers/utils/Random.h>

/* high-level */
#include "mac_pib.h"

/* low-level specific */
#include "mac_rx.h"
#include "mac_tx.h"
#include "mac_symbol_timer.h"
#include "mac_rx_onoff.h"
#include "mac_low_level.h"
#include "mac_radio.h"

/* target specific */
#include "mac_mcu.h"
#include "mac_radio_defs.h"
#include "rf_mac_api.h"

/* MAC */
#include "mac.h"

/* debug */
#include "mac_assert.h"

// SW Tracer
#include "dbg.h"
#ifdef DEBUG_SW_TRACE
#define DBG_ENABLE
#ifndef _DBGFILE
#define _DBGFILE mac_mcu_c
#endif
#include "dbgid_sys_mst.h"
#endif // DEBUG_SW_TRACE

#include "rom_jt_154.h"

/* ------------------------------------------------------------------------------------------------
 *                                           Defines
 * ------------------------------------------------------------------------------------------------
 */

/* for optimized indexing of uint32's */
#if HAL_MCU_LITTLE_ENDIAN()
#define UINT32_NDX0   0
#define UINT32_NDX1   1
#define UINT32_NDX2   2
#define UINT32_NDX3   3
#else
#define UINT32_NDX0   3
#define UINT32_NDX1   2
#define UINT32_NDX2   1
#define UINT32_NDX3   0
#endif

/* ------------------------------------------------------------------------------------------------
 *                                        Local Variables
 * ------------------------------------------------------------------------------------------------
 */

/* Function pointer for the random seed callback */
static macRNGFcn_t pRandomSeedCB = NULL;

/* ------------------------------------------------------------------------------------------------
 *                                       Local Prototypes
 * ------------------------------------------------------------------------------------------------
 */

void MAC_SetRandomSeedCB(macRNGFcn_t pCBFcn);
void macMcuRfErrIsr(void);
/**************************************************************************************************
 * @fn          MAC_SetRandomSeedCB
 *
 * @brief       Set the function pointer for the random seed callback.
 *
 * @param       pCBFcn - function pointer of the random seed callback
 *
 * @return      none
 **************************************************************************************************
 */
void MAC_SetRandomSeedCB(macRNGFcn_t pCBFcn)
{
  pRandomSeedCB = pCBFcn;
}

/**************************************************************************************************
 * @fn          macMcuInit
 *
 * @brief       Initialize the MCU.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macMcuInit(void)
{
 /*----------------------------------------------------------------------------------------------
  *  Initialize random seed value.
  */

  /* Read 32 random bytes and store them in flash for future use in random
   * key generation for CBKE key establishment
   */
  if ( pRandomSeedCB )
  {
    uint8 randomSeed[MAC_RANDOM_SEED_LEN];
    uint8 i;
    /* 32 random bytes read. */
    for ( i = 0;  i < MAC_RANDOM_SEED_LEN; i+= 4)
    {
      uint8 j;
      uint32 random32 = Random_getNumber();

      for ( j = 0; j < 4; ++j)
      {
        randomSeed[i + j] = BREAK_UINT32( random32, j );
      }
    }

    /* Store random bytes in flash. */
    pRandomSeedCB( randomSeed );
  }
}


/**************************************************************************************************
 * @fn          macMcuRandomByte
 *
 * @brief       Returns a random byte using a special hardware feature that generates new
 *              random values based on the truly random seed set earlier.
 *
 * @param       none
 *
 * @return      a random byte
 **************************************************************************************************
 */
MAC_INTERNAL_API uint8 macMcuRandomByte(void)
{
  return (Random_getNumber() & 0xFF);
}


/**************************************************************************************************
 * @fn          macMcuRandomWord
 *
 * @brief       Returns a random word using a special hardware feature that generates new
 *              random values based on the truly random seed set earlier.
 *
 * @param       none
 *
 * @return      a random word
 **************************************************************************************************
 */
MAC_INTERNAL_API uint16 macMcuRandomWord(void)
{
  return (Random_getNumber() & 0xFFFF);
}


/**************************************************************************************************
 * @fn          macMcuTimerCapture
 *
 * @brief       Returns the last timer capture.  This capture should have occurred at the
 *              receive time of the last frame (the last time SFD transitioned to active).
 *
 * @param       timeStamp - time stamp from RX or TX frames
 *
 * @return      last capture of hardware timer (full 16-bit value)
 **************************************************************************************************
 */
MAC_INTERNAL_API uint16 macMcuTimerCapture(uint32 timeStamp)
{
  /* The time, in RAT ticks, at which the frame was received */
  return( ( timeStamp - macPrevPeriodRatCount ) % MAC_SYMBOL_TO_RAT_RATIO );
}


/**************************************************************************************************
 * @fn          macMcuOverflowCapture
 *
 * @brief       Returns the last capture of the symbol counter.  A special hardware feature
 *              captures the symbol counter when the regular hardware timer is captured.
 *
 * @param       timeStamp - time stampe from RX or TX frames
 *
 * @return      last capture of symbol count
 **************************************************************************************************
 */
MAC_INTERNAL_API uint32 macMcuOverflowCapture(uint32 timeStamp)
{
  /* The time, in symbol units, at which the frame was sent. */
  return( ( timeStamp - macPrevPeriodRatCount ) / MAC_SYMBOL_TO_RAT_RATIO );
}


/**************************************************************************************************
 * @fn          macMcuOverflowSetCompare
 *
 * @brief       Set overflow count compare value.  An interrupt is triggered when the overflow
 *              count equals this compare value.
 *
 * @param       count - overflow count compare value
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macMcuOverflowSetCompare(uint32 count)
{
  uint32 compare, current;

  if (macRadioYielded != MAC_RADIO_ACTIVE)
  {
    /* Power-up is not complete yet.
     * Let the next call to macMcuOverfloSetPeriod() do the job
     */
    macSymbolTimerImpending = TRUE;
    return;
  }

  /* Note it is okay to have the compare wrapped around.
   * The timer will be triggered after the wrap-around.
   */
  compare = macPrevPeriodRatCount + count * MAC_SYMBOL_TO_RAT_RATIO;
  MAC_ASSERT( count <= MAC_SYMBOL_MAXIMUM_ROLLOVER );

  /* Note that critical section is not entered in lead time computation
   * since only in critical section of a caller,
   * the RAT compare might be close to the current RAT counter. */
  current = RF_getCurrentTime();
  if (((compare - current - 200 * MAC_RAT_MHZ) & 0x80000000) &&
      ((current - compare - MAC_BACKOFF_PAST_WINDOW) & 0x80000000))
  {
    /* Either time in the past for a second window
     * or not enough lead room to handle race condition. */
    compare = current + 200*MAC_RAT_MHZ;
  }

  DBG_PRINTL1(DBGSYS, "macPrevPeriodRatCount=%u", macPrevPeriodRatCount);
  DBG_PRINTL2(DBGSYS, "macMcuOverflowSetCompare(compare=%u, current=%u)", compare, current);

  /* Convert count to relative RAT count and set MAC Channel B Compare. */
  macSetupRATChanCompare(macRatCb_B, compare);
}


/**************************************************************************************************
 * @fn          macMcuOverflowSetPeriod
 *
 * @brief       Set overflow count period value.  An interrupt is triggered when the overflow
 *              count equals this period value.
 *
 * @param       count - overflow count compare value
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macMcuOverflowSetPeriod(uint32 count)
{
  uint32 compare, current;

  if (macRadioYielded != MAC_RADIO_ACTIVE)
  {
    /* Power-up is not complete yet.
     * Let the next call to macMcuOverfloSetPeriod() do the job
     */
    macSymbolTimerImpending = TRUE;
    return;
  }

  MAC_ASSERT( count <= MAC_SYMBOL_MAXIMUM_ROLLOVER );

  /* Note that critical section is not entered in lead time computation
   * since only in critical section of a caller,
   * the RAT compare might be close to the current RAT counter.
   * Also note it is okay to have the compare wrapped around.
   * The timer will be triggered after the wrap-around.
   */
  compare = macPrevPeriodRatCount + count * MAC_SYMBOL_TO_RAT_RATIO;
  current = RF_getCurrentTime();
  if (((compare - current - 200 * MAC_RAT_MHZ) & 0x80000000) &&
      ((current - compare - MAC_BACKOFF_PAST_WINDOW) & 0x80000000))
  {
    /* Either time in the past for a second window
     * or not enough lead room to handle race condition. */
    compare = current + 200*MAC_RAT_MHZ;
  }

  DBG_PRINTL2(DBGSYS, "macMcuOverflowSetPeriod(compare=%u, current=%u)", compare, current);

  /* Convert count to relative RAT count and set MAC Channel A Compare */
  macSetupRATChanCompare(macRatCb_A, compare);
}


/**************************************************************************************************
 * @fn          macMcuPrecisionCount
 *
 * @brief       This function is used by higher layer to read a free running counter driven by
 *              MAC timer.
 *
 * @param       none
 *
 * @return      MSW of MAC_RAT_COUNT
 **************************************************************************************************
 */
uint32 macMcuPrecisionCount(void)
{
  return ((uint16)(RF_getCurrentTime() & 0xFFFF));
}


/**************************************************************************************************
 * @fn          macMcuRfErrIsr
 *
 * @brief       Interrupt service routine that handles all RF Error interrupts.  Only the RX FIFO
 *              overflow condition is handled.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
void macMcuRfErrIsr(void)
{
  // TDOD: Add error handling and call macRxFifoOverflowIsr()
}

#if defined(COMBO_MAC) || !defined(FREQ_2_4G)
/**************************************************************************************************
 * @fn          macMcuBitReverse
 *
 * @brief       Reverise bits of each byte in the given memory range
 *
 * @param       pAddr - start address
 *              len  - length in byte
 *
 * @return      none
 **************************************************************************************************
 */
void macMcuBitReverse(uint8* pAddr, uint16 len)
{

}
#endif


/**************************************************************************************************
 * @fn          macMcuLongDiv
 *
 * @brief       Divides 64 bit value with a 32-bit value
 *
 * @param       pAddr - start address
 *              len  - length in byte
 *
 * @return      none
 **************************************************************************************************
 */
uint32 macMcuLongDiv(unsigned long long dividend, uint32 divisor)
{
    uint32 temp32;

    temp32 = (dividend + (divisor >> 1))/divisor;

    return (temp32);
}
/**************************************************************************************************
*/
