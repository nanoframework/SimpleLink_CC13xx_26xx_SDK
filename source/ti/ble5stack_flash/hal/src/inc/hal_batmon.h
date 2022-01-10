/******************************************************************************

 @file  hal_batmon.h

 @brief This file contains the declarations for the HAL BATMON subsytstem.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
#ifndef HAL_BATMON_H
#define HAL_BATMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "hal_defs.h"

/**************************************************************************************************
 * @fn          HalBatMonRead
 *
 * @brief       This function is the subsystem utility read function and should be called before
 *              any Vdd-critical operation (e.g. flash write or erase operations.)
 *
 * input parameters
 *
 * @param       vddMask - A valid BATTMON_VOLTAGE mask.
 *
 * output parameters
 *
 * None.
 *
 * @return      TRUE if the measured Vdd exceeds the paramter level; FALSE otherwise.
 **************************************************************************************************
 */
uint8 HalBatMonRead(uint8 vddMask);

/**************************************************************************************************
*/

#ifdef __cplusplus
};
#endif

#endif
