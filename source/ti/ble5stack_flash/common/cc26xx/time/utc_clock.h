/******************************************************************************

 @file  utc_clock.h

 @brief UTC Clock types and functions prototypes.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2004 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef UTC_CLOCK_H
#define UTC_CLOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

// number of seconds since 0 hrs, 0 minutes, 0 seconds, on the
// 1st of January 2000 UTC
typedef uint32_t UTCTime;

// UTC time structs broken down until standard components.
typedef struct
{
  uint8_t seconds;  // 0-59
  uint8_t minutes;  // 0-59
  uint8_t hour;     // 0-23
  uint8_t day;      // 0-30
  uint8_t month;    // 0-11
  uint16_t year;    // 2000+
} UTCTimeStruct;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * @fn      UTC_init
 *
 * @brief   Initialize the UTC clock module.  Sets up and starts the
 *          clock instance.
 *
 * @param   None.
 *
 * @return  None.
 */
extern void UTC_init(void);

/*
 * Set the new time.  This will only set the seconds portion
 * of time and doesn't change the factional second counter.
 *     newTime - number of seconds since 0 hrs, 0 minutes,
 *               0 seconds, on the 1st of January 2000 UTC
 */
extern void UTC_setClock( UTCTime newTime );

/*
 * Gets the current time.  This will only return the seconds
 * portion of time and doesn't include the factional second counter.
 *     returns: number of seconds since 0 hrs, 0 minutes,
 *              0 seconds, on the 1st of January 2000 UTC
 */
extern UTCTime UTC_getClock( void );

/*
 * Converts UTCTime to UTCTimeStruct
 *
 * secTime - number of seconds since 0 hrs, 0 minutes,
 *          0 seconds, on the 1st of January 2000 UTC
 * tm - pointer to breakdown struct
 */
extern void UTC_convertUTCTime( UTCTimeStruct *tm, UTCTime secTime );

/*
 * Converts UTCTimeStruct to UTCTime (seconds since 00:00:00 01/01/2000)
 *
 * tm - pointer to UTC time struct
 */
extern UTCTime UTC_convertUTCSecs( UTCTimeStruct *tm );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* UTC_CLOCK_H */
