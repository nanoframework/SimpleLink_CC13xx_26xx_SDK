/******************************************************************************

 @file  pwrmon.c

 @brief Implementation for CC26xx Vdd monitoring subsystem.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/* -----------------------------------------------------------------------------
 * Includes
 * -----------------------------------------------------------------------------
 */
#include "pwrmon.h"

#include <driverlib/aon_batmon.h>

/* -----------------------------------------------------------------------------
 * Macros
 * -----------------------------------------------------------------------------
 */
#define BATMON_ON (HWREG(AON_BATMON_BASE + AON_BATMON_O_CTL) ? true : false)

/*******************************************************************************
 * @fn      PWRMON_check()
 *
 * @brief   Checks the caller supplied voltage threshold against the value read
 *          from the CC26xx BATMON register.
 *
 * @param   threshold - voltage to compare measured device Vdd to
 *
 * @return  false if device voltage less than limit, otherwise true
 *******************************************************************************
 */
bool PWRMON_check(uint16_t threshold)
{
    if(BATMON_ON && (AONBatMonBatteryVoltageGet() < threshold))
    {
        // Measured device voltage is below threshold
        return(false);
    }

    return(true);
}

/*******************************************************************************
 * @fn      PWRMON_init()
 *
 * @brief   Enable AON battery monitor and set update rate
 *
 * @param   None
 *
 * @return  None
 *******************************************************************************
 */
void PWRMON_init(void)
{
    // Enable the CC26xx Battery Monitor
    AONBatMonEnable();
}

/*******************************************************************************
*/
