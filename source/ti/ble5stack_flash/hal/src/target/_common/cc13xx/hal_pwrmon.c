/******************************************************************************

 @file  hal_pwrmon.c

 @brief Implementation for CC26xx HAL Vdd monitoring subsystem.

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
#include "hal_pwrmon.h"

#include <driverlib/aon_batmon.h>

/* -----------------------------------------------------------------------------
 * Macros
 * -----------------------------------------------------------------------------
 */
#define BATMON_ENABLED (HWREG(AON_BATMON_BASE+AON_BATMON_O_CTL) ? true : false)

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
bool hal_pwrmon_chk(uint16_t threshold)
{
    if(BATMON_ENABLED && (AONBatMonBatteryVoltageGet() < threshold))
    {
        // Mesaured device voltage is below threshold
        return(false);
    }

    return(true);
}

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
void hal_pwrmon_init(void)
{
  // Enable the CC26xx Battery Monitor
  AONBatMonEnable();

  // Configure compare/measure every 32 AON clock cycles,
  // measurement results are only updated if changes occur.
  AONBatMonMeasurementCycleSet(AON_BATMON_CYCLE_32);
}

/*******************************************************************************
*/
