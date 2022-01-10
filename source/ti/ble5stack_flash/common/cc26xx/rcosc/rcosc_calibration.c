/******************************************************************************

 @file  rcosc_calibration.c

 @brief This file contains the RCOSC calibration routines.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifdef USE_RCOSC

/*********************************************************************
 * INCLUDES
 */

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/sysbios/knl/Clock.h>

#include "hci.h"

#include "util.h"
#include "rcosc_calibration.h"

/*********************************************************************
 * LOCAL VARIABLES
 */

// Clock instance for Calibration injections
static Clock_Struct  injectCalibrationClock;

// Power Notify Object for wake-up callbacks
Power_NotifyObj injectCalibrationPowerNotifyObj;

static uint8_t isEnabled = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void rcosc_injectCalibrationClockHandler(UArg arg);

static uint8_t rcosc_injectCalibrationPostNotify(uint8_t eventType,
                                                 uint32_t *eventArg,
                                                 uint32_t *clientArg);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      RCOSC_enableCalibration
 *
 * @brief   enable calibration.  calibration timer will start immediately.
 *
 * @param   none
 *
 * @return  none
 */
void RCOSC_enableCalibration(void)
{
  if (!isEnabled)
  {
    isEnabled = TRUE;

    // Create RCOSC clock - one-shot clock for calibration injections.
    Util_constructClock(&injectCalibrationClock,
                        rcosc_injectCalibrationClockHandler,
                        RCOSC_CALIBRATION_PERIOD, 0, false, 0);

    // Receive callback when device wakes up from Standby Mode.
    Power_registerNotify(&injectCalibrationPowerNotifyObj, PowerCC26XX_AWAKE_STANDBY,
                         (Power_NotifyFxn)rcosc_injectCalibrationPostNotify,
                         (uintptr_t) NULL);


    // Start clock for the RCOSC calibration injection.  Calibration must be
    // done once every second by either the clock or by waking up from StandyBy
    // Mode. To ensure that the device is always correctly calibrated the clock
    // is started now but is only allowed to expire when a wake up event does
    // not occur within the clock's duration.
    Util_startClock(&injectCalibrationClock);
  }
}

/*********************************************************************
 * @fn      rcosc_injectCalibrationClockHandler
 *
 * @brief   Handler function for RCOSC clock timeouts.  Executes in
 *          SWI context.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
static void rcosc_injectCalibrationClockHandler(UArg arg)
{
  // Restart clock.
  Util_startClock(&injectCalibrationClock);

  // Inject calibration.
  PowerCC26XX_injectCalibration();
}

/*********************************************************************
 * @fn      rcosc_injectCalibrationPostNotify
 *
 * @brief   Callback for Power module state change events.
 *
 * @param   eventType - The state change.
 * @param   clientArg - Not used.
 *
 * @return  Power_NOTIFYDONE
 */
static uint8_t rcosc_injectCalibrationPostNotify(uint8_t eventType,
                                                 uint32_t *eventArg,
                                                 uint32_t *clientArg)
{
  // If clock is active at time of wake up,
  if (Util_isActive(&injectCalibrationClock))
  {
    // Stop injection of calibration - the wakeup has automatically done this.
    Util_stopClock(&injectCalibrationClock);
  }

  // Restart the clock in case delta between now and next wake up is greater
  // than one second.
  Util_startClock(&injectCalibrationClock);

  return Power_NOTIFYDONE;
}
#endif //USE_RCOSC
