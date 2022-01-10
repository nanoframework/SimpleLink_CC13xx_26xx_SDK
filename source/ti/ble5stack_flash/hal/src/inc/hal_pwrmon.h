/******************************************************************************

 @file  hal_pwrmon.h

 @brief Declarations for CC26xx HAL Vdd monitoring subsytstem.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
#ifndef HAL_PWRMON_H
#define HAL_PWRMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------------
 * Includes
 * -----------------------------------------------------------------------------
 */
#include "hal_types.h"

/* -----------------------------------------------------------------------------
 * Default Voltage Thresholds
 * -----------------------------------------------------------------------------
 */
#if !defined(MIN_VDD_INIT)
#define MIN_VDD_INIT   0x1CE  // 1.80 volts (0.80=206/256 -> 206=0xCE)
#endif

#if !defined(MIN_VDD_POLL)
#define MIN_VDD_POLL   0x1DA  // 1.85 volts (0.85=218/256 -> 218=0xDA)
#endif

#if !defined(MIN_VDD_FLASH)
#define MIN_VDD_FLASH  0x180  // 1.50 volts (0.50=128/256 -> 128=0x80)
//#define MIN_VDD_FLASH  0x200  // 2.00 volts (0.00=  0/256 ->   0=0x00)
#endif

/*******************************************************************************
 * @fn      hal_pwrmon_chk()
 *
 * @brief   Checks the caller supplied voltage threshold against the value read
 *          from the CC26xx BATMON register.
 *
 * @param   threshold - voltage to compare measured device Vdd to
 *
 * @return  FALSE if device voltage less that limit, otherwise TRUE
 *******************************************************************************
 */
bool hal_pwrmon_chk(uint16_t threshold);

/*******************************************************************************
 * @fn      hal_pwrmon_init()
 *
 * @brief   Enable AON battery monitor and set update rate
 *
 * @param   None
 *
 * @return  None
 *******************************************************************************
 */
void hal_pwrmon_init(void);

/*******************************************************************************
*/

#ifdef __cplusplus
};
#endif

#endif
