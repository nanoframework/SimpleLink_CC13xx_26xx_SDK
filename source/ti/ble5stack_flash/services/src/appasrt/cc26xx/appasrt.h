/******************************************************************************

 @file  appasrt.h

 @brief Application assert function definitions.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
#ifndef APPASRT_H
#define APPASRT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "appasrtintf.h"

//*****************************************************************************
// Functions
//*****************************************************************************

/**
 *
 * @brief   Loads the applicaiton assert handler to function pointer for use
 *          by the stack.
 *
 * @param[in]   pAssrtHnldr : Pointer to assert handler function on app side.
 * @param[out]  None.
 *
 * @return  none
 */
extern void APPASRT_loadAssertHndlr(APPASRTINTF_assrtHndlerFunc_t *pAssrtHnldr);

/**
 * @fn       APPASRT_callAssrtHndler
 *
 * @brief   Calls the application assert handler.
 *
 * @param[in]   None.
 * @param[out]  None.
 *
 * @return  none
 */
extern void APPASRT_callAssrtHndler(void);

//*****************************************************************************
//*****************************************************************************

#ifdef __cplusplus
}
#endif

#endif /* APPASRT_H */
