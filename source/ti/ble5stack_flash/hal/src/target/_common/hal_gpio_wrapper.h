/******************************************************************************

 @file  hal_gpio_wrapper.h

 @brief This file contains a simplified GPIO mapping for debug.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2013 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef HAL_GPIO_WRAPPER_H
#define HAL_GPIO_WRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#include <driverlib/gpio.h>
#include <driverlib/ioc.h>

/*******************************************************************************
 * MACROS
 */

#ifdef DEBUG_GPIO

#define HAL_GPIO_INIT()                                                        \
  {                                                                            \
    IOCPinTypeGpioOutput( HAL_GPIO_1 );   /* SmartRF06EB LED3    P403.2 */     \
    IOCPinTypeGpioOutput( HAL_GPIO_2 );   /* SmartRF06EB LED4    P403.4 */     \
    IOCPinTypeGpioOutput( HAL_GPIO_3 );   /* SmartRF06EB LED1    P404.20 */    \
    IOCPinTypeGpioOutput( HAL_GPIO_4 );   /* SmartRF06EB LED2    P405.4 */     \
    IOCPinTypeGpioOutput( HAL_GPIO_5 );   /* SmartRF06EB NC      P403.6 */     \
    IOCPinTypeGpioOutput( HAL_GPIO_6 );   /* SmartRF06EB BTN_L   P403.8 */     \
    IOCPinTypeGpioOutput( HAL_GPIO_7 );   /* SmartRF06EB BTN_R   P403.10 */    \
    IOCPinTypeGpioOutput( HAL_GPIO_8 );   /* SmartRF06EB BTN_UP  P403.12 */    \
  }

#define HAL_GPIO_SET(dio)                                                      \
  {                                                                            \
    GPIO_writeDio((dio), 1);                                                   \
  }

#define HAL_GPIO_CLR(dio)                                                      \
  {                                                                            \
    GPIO_writeDio((dio), 0);                                                   \
  }

#else // !DEBUG_GPIO

#define HAL_GPIO_INIT()
#define HAL_GPIO_SET(dio)
#define HAL_GPIO_CLR(dio)

#endif // DEBUG_GPIO

/*******************************************************************************
 * CONSTANTS
 */

#define HAL_GPIO_1                IOID_7   // GPIO11 SmartRF06EB LED3    P403.2
#define HAL_GPIO_2                IOID_6   // GPI012 SmartRF06EB LED4    P403.4
#define HAL_GPIO_3                IOID_25  // GPIO24 SmartRF06EB LED1    P404.20
#define HAL_GPIO_4                IOID_27  // GPIO22 SmartRF06EB LED2    P405.4
#define HAL_GPIO_5                IOID_1   // GPIO17 SmartRF06EB NC      P403.6
#define HAL_GPIO_6                IOID_15  // GPIO3  SmartRF06EB BTN_L   P403.8
#define HAL_GPIO_7                IOID_18  // GPIO31 SmartRF06EB BTN_R   P403.10
#define HAL_GPIO_8                IOID_19  // GPIO30 SmartRF06EB BTN_UP  P403.12

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

#ifdef __cplusplus
}
#endif

#endif /* HAL_GPIO_WRAPPER_H */
