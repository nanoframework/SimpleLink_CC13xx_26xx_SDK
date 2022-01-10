/******************************************************************************

 @file  find_stack_entry.h

 @brief This module finds th entry point of stack application from stack image
 header.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2012 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
#ifndef FIND_STACK_ENTRY_H
#define FIND_STACK_ENTRY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* -----------------------------------------------------------------------------
 *                                          Includes
 * -----------------------------------------------------------------------------
 */

#include <common/cc26xx/oad/oad_image_header.h>

/*********************************************************************
 * @fn      findStackBoundaryAddr
 *
 * @brief   This function is used to find the active address of the app/stack
 *          boundary for on-chip OAD applications
 *
 * @return  stackBoundaryAddr - The address of the stack boundary information
 */
extern uint32_t findStackBoundaryAddr();

#ifdef __cplusplus
}
#endif

#endif /* FIND_STACK_ENTRY_H */

/**************************************************************************************************
*/
