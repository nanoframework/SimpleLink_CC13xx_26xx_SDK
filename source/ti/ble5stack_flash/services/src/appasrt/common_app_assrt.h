/******************************************************************************

 @file  common_app_assrt.h

 @brief Application assert function definitions.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: TI_TEXT 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
#ifndef COMMON_APP_ASSRT_H
#define COMMON_APP_ASSRT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
* INCLUDES
*/

#include "hal_assert.h"

/*******************************************************************************
 * CONSTANTS
 */

#ifdef EXT_HAL_ASSERT
  #define ASSERT_CBACK                  &halAssertCback
#else // !EXT_HAL_ASSERT
  #define ASSERT_CBACK                  &appAssertCback
#endif // EXT_HAL_ASSERT

/*********************************************************************
 * FUNCTIONS
 */

extern assertCback_t appAssertCback;
extern assertCback_t halAssertCback;
extern void RegisterAssertCback(assertCback_t appAssertHandler);
extern void DefaultAssertCback(uint8_t assertCause, uint8_t assertSubcause);

#ifdef __cplusplus
}
#endif

#endif /* COMMON_APP_ASSRT_H */
