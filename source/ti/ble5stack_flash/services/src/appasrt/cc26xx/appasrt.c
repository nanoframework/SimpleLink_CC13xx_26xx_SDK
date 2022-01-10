/******************************************************************************

 @file  appasrt.c

 @brief Application assert handler functions

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

//*****************************************************************************
// Includes
//*****************************************************************************

#include <ti/sysbios/hal/Hwi.h>

#include "appasrt.h"

//*****************************************************************************
// API Functions
//*****************************************************************************

/**
 * @brief   See header file.
 */
void  APPASRT_loadAssertHndlr(APPASRTINTF_assrtHndlerFunc_t *pAssrtHnldr)
{
    //! Load caller's structure with pointers to the NV API functions
    pAssrtHnldr->callAssrtHndler = &APPASRT_callAssrtHndler;
}

/**
 * @brief   See header file.
 */
void APPASRT_callAssrtHndler(void)
{
    volatile uint8_t i = 1;
    Hwi_disable();
    while(i);
}

//*****************************************************************************
//*****************************************************************************
