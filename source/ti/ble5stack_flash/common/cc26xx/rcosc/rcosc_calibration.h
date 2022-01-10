 /******************************************************************************

 @file  rcosc_calibration.h

 @brief This file contains the RCOSC calibration routines definitions
        and prototypes.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef RCOSC_CALIBRATION_H
#define RCOSC_CALIBRATION_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */


/*********************************************************************
 * CONSTANTS
 */

// 1000 ms
#define RCOSC_CALIBRATION_PERIOD              1000

/*********************************************************************
 * FUNCTIONS
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
extern void RCOSC_enableCalibration(void);


#ifdef __cplusplus
}
#endif

#endif /* RCOSC_CALIBRATION_H */
