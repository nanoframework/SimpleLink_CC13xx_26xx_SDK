/******************************************************************************

 @file  hal_appasrt.c

 @brief This module contains the application assert functions.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/*******************************************************************************
 * INCLUDES
 */

#include "hal_appasrt.h"

/*******************************************************************************
 * LOCAL VARIABLES
 */

/* Pointers to App Assert Handler function */
static APPASRTINTF_assrtHndlerFunc_t *pAssrtFunc = NULL;

/*******************************************************************************
 * FUNCTIONS
 */

/*******************************************************************************
 * @brief   See header file for details.
 */
uint8_t HAL_APPASRT_register(APPASRTINTF_assrtHndlerFunc_t *pFptr)
{
  uint8_t status;

  //! Store parameters
  pAssrtFunc = pFptr;

  //! Make sure we have valid function pointers
  if ((pAssrtFunc != NULL) && (pAssrtFunc->callAssrtHndler))
  {
    status = APPASRTINTF_SUCCESS;
  }
  else
  {
    status = APPASRTINTF_FAILURE;
  }

  return status;
}

/*******************************************************************************
 * @brief   See header file for details.
 */
void HAL_APPASRT_callAssrtHanlder(void)
{
  pAssrtFunc->callAssrtHndler();
}

/*******************************************************************************
*******************************************************************************/
