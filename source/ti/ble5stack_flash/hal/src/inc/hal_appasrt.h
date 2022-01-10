/******************************************************************************

 @file  hal_appasrt.h

 @brief This module defines the OSAL application assert function

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2014 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "appasrtintf.h"
#include "hal_types.h"

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @brief   Register application assert handler function pointer for use on
 *          stack side.
 *
 * @param  *pFptr Pointer to app assert handler function
 *
 * @return  APPASRTINTF_SUCCESS if registration succeeds. APPASRTINTF_FAILURE,
 *          otherwise.
 */
extern uint8_t HAL_APPASRT_register(APPASRTINTF_assrtHndlerFunc_t *pFptr);

/**
 * @brief   Wrapper function call to application assert via funciton pointer.
 *
 * @param    none
 *
 * @return  none
 */
extern void HAL_APPASRT_callAssrtHanlder(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif
