/******************************************************************************

 @file  time_ind.c

 @brief Time indication and notification handling routines.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2011 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "string.h"
#include "bcomdef.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "time_clock.h"
#include "bletime.h"

/*********************************************************************
 * MACROS
 */

// Maximum category ID value
#define ALERT_CAT_ID_MAX            9

// Parse major category
#define ALERT_MAJOR_CAT(x)          ((x) >> 3)

// Parse subcategory
#define ALERT_SUBCAT(x)             ((x) & 0x07)

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      Time_indGattMsg
 *
 * @brief   Handle indications and notifications.
 *
 * @param   pMsg - GATT message.
 *
 * @return  none
 */
void Time_indGattMsg(gattMsgEvent_t *pMsg)
{
  uint8_t i;

  // Look up the handle in the handle cache
  for (i = 0; i < HDL_CACHE_LEN; i++)
  {
    if (pMsg->msg.handleValueNoti.handle == Time_handleCache[i])
    {
      break;
    }
  }

  // Perform processing for this handle
  switch (i)
  {
    case HDL_CURR_TIME_CT_TIME_START:
      // Set clock to time read from time server
      Time_clockSet(pMsg->msg.handleValueNoti.pValue);
      break;


     default:
      break;
  }

  // Send confirm for indication
  if (pMsg->method == ATT_HANDLE_VALUE_IND)
  {
    ATT_HandleValueCfm(pMsg->connHandle);
  }
}


/*********************************************************************
*********************************************************************/
