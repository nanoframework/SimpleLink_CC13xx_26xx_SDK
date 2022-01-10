/******************************************************************************

 @file  appasrtintf.h

 @brief Application assert handler data structs.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
#ifndef APPASRTINTF_H
#define APPASRTINTF_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// Constants and definitions
//*****************************************************************************

// ASSERTINTF status codes
#define APPASRTINTF_SUCCESS      0
#define APPASRTINTF_FAILURE      1

//*****************************************************************************
// Typedefs
//*****************************************************************************

//! Function pointer definition for the callAssrtHndler() function
typedef void (*APPASRTINTF_callAssrtHndler)( void );

//! Structure of the app assert handler function pointer.
typedef struct appasrtintf_assrthndlerfunc_t
{
    //! Call the app assert handler.
    APPASRTINTF_callAssrtHndler callAssrtHndler;
} APPASRTINTF_assrtHndlerFunc_t;

//*****************************************************************************
//*****************************************************************************

#ifdef __cplusplus
}
#endif

#endif /* APASSRTINTF_H */
