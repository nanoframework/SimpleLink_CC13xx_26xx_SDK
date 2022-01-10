/******************************************************************************

 @file  time_clock.c

 @brief Time clock display and timekeeping.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2014 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include <string.h>

#include "bcomdef.h"
#include <ti/mw/display/Display.h>
#include <ti_drivers_config.h>

#include "utc_clock.h"
#include "time_clock.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
extern Display_Handle dispHandle;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Month string
static const char timeMonthStr[12][3] =
{
  {'J', 'a', 'n'},
  {'F', 'e', 'b'},
  {'M', 'a', 'r'},
  {'A', 'p', 'r'},
  {'M', 'a', 'y'},
  {'J', 'u', 'n'},
  {'J', 'u', 'l'},
  {'A', 'u', 'g'},
  {'S', 'e', 'p'},
  {'O', 'c', 't'},
  {'N', 'o', 'v'},
  {'D', 'e', 'c'}
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static char *num2Str(char *pStr, uint8_t num);
static char *year2Str(char *pStr, uint16_t year);

/*********************************************************************
 * @fn      Time_clockInit()
 *
 * @brief   Initialize the Time clock.
 *
 * @return  none
 */
void Time_clockInit(void)
{
  // Start the UTC clock.
  UTC_init();
}

/*********************************************************************
 * @fn      Time_clockDisplay()
 *
 * @brief   Write the clock time to the display.
 *
 * @return  none
 */
void Time_clockDisplay(void)
{
  char displayBuf[20];
  char *p = displayBuf;
  UTCTimeStruct time;

  memset(displayBuf, 0x00, 20);

  // Get time structure from UTC.
  UTC_convertUTCTime(&time, UTC_getClock());

  // Display is in the format:
  // HH:MM MmmDD YYYY

  p = num2Str(p, time.hour);
  *p++ = ':';
  p = num2Str(p, time.minutes);
  *p++ = ' ';

  *p++ = timeMonthStr[time.month][0];
  *p++ = timeMonthStr[time.month][1];
  *p++ = timeMonthStr[time.month][2];

  p = num2Str(p, time.day + 1);
  *p++ = ' ';

  p = year2Str(p, time.year);

  Display_print0(dispHandle, 2, 0, displayBuf);
}

/*********************************************************************
 * @fn      Time_clockSet()
 *
 * @brief   Set the clock.
 *
 * @param   pData - Pointer to a Date Time characteristic structure
 *
 * @return  none
 */
void Time_clockSet(uint8_t *pData)
{
  UTCTimeStruct time;

  // Parse time service structure to UTC time structure.
  time.year = BUILD_UINT16(pData[0], pData[1]);
  if (time.year == 0)
  {
    time.year = 2000;
  }
  pData += 2;
  time.month = *pData++;
  if (time.month > 0)
  {
   // time.month--;
  }
  time.day = *pData++;
  if (time.day > 0)
  {
  //  time.day--;
  }
  time.hour = *pData++;
  time.minutes = *pData++;
  time.seconds = *pData;

  // Update UTC time.
  UTC_setClock(UTC_convertUTCSecs(&time));
}

/*********************************************************************
 * @fn      num2Str()
 *
 * @brief   Convert unsigned int 0-99 to decimal digit string.
 *
 * @return  pointer to string
 */
static char *num2Str(char *pStr, uint8_t num)
{
  *pStr++ = (num / 10) + '0';
  *pStr++ = (num % 10) + '0';

  return pStr;
}

/*********************************************************************
 * @fn      num2Str()
 *
 * @brief   Convert a year [9999-0000] to decimal digit string.
 *          Note: this assumes the device's longevity will not surpass
 *          year 9999.
 *
 * @return  pointer to string
 */
static char *year2Str(char *pStr, uint16_t year)
{
  //thousands
  *pStr++ = ((year / 1000) % 10) + '0';
  //hundreds
  *pStr++ = ((year / 100) % 10) + '0';
  //tens
  *pStr++ = ((year / 10) % 10) + '0';
  //units
  *pStr++ = (year % 10) + '0';

  return pStr;
}


/*********************************************************************
*********************************************************************/
