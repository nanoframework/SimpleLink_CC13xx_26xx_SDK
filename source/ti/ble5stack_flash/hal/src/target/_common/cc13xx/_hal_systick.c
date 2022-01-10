/******************************************************************************

 @file  _hal_systick.c

 @brief This module contains the HAL power management procedures for the CC2538.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2006 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "osal.h"
#include "osal_clock.h"
#include "hal_systick.h"
#include "hal_mcu.h"

#include <inc/hw_nvic.h>
#include <inc/hw_ints.h>
#include <inc/hw_sysctl.h>


/* ------------------------------------------------------------------------------------------------
 *                                          Macros
 * ------------------------------------------------------------------------------------------------
 */
#define SYSTICK 1000 /* 1 millisecond = 1/SYSTICK */
#define TICK_IN_MS 1 /* 1 millisecond */

void SysTickIntHandler(void);
/* ------------------------------------------------------------------------------------------------
 *                                      Function Prototypes
 * ------------------------------------------------------------------------------------------------
 */

/***************************************************************************************************
 * @fn      SysTickIntHandler
 *
 * @brief   The Systick Interrupt module
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
void SysTickIntHandler(void)
{
  /* Update OSAL timer and clock */
  osalAdjustTimer(TICK_IN_MS);

  /* Clear Sleep Mode */
  CLEAR_SLEEP_MODE();
}

/***************************************************************************************************
 * @fn      SysTickSetup
 *
 * @brief   Setup the Systick module
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
void SysTickSetup(void)
{
  unsigned long clockval;
  clockval = SysCtrlClockGet();
  clockval /= SYSTICK;

  /* 1ms period for systick */
  SysTickPeriodSet(clockval);

  /* Enable SysTick */
  SysTickEnable();

  /* Enable Systick interrupt */
  SysTickIntEnable();
}

/***************************************************************************************************
 * @fn      SysCtrlClockGet
 *
 * @brief   Get the Systick clock rate
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
unsigned long SysCtrlClockGet(void)
{
  return (GET_MCU_CLOCK);
}
