/******************************************************************************

 @file  hal_trng_wrapper.h

 @brief This file contains header information for a True Random
        Number Generator driver until one is provided elsewhere.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2009 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef HAL_TRNG_WRAPPER_H
#define HAL_TRNG_WRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#include "hal_types.h"
#include <inc/hw_types.h>
#include <driverlib/trng.h>

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
* TYPEDEFS
*/

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

extern void   HalTRNG_InitTRNG( void );
extern void   HalTRNG_WaitForReady( void );
extern uint32 HalTRNG_GetTRNG( void );

#ifdef __cplusplus
}
#endif

#endif /* HAL_TRNG_WRAPPER_H */
