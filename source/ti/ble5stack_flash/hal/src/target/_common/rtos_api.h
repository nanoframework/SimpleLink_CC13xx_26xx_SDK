/******************************************************************************

 @file  rtos_api.h

 @brief Header for RTOS proxy for stack's interface to the RTOS commands.

 Group: WCS, LPC/BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef RTOS_API_H
#define RTOS_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

extern uint32_t *rtosApiTblPtr;

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

// RTOS proxy index for RTOS API
#define SWI_DISABLE               0
#define SWI_RESTORE               1

/*
** RTOS API Proxy
*/

#define RTOS_TABLE(index)      (*((uint32_t *)((uint32_t)rtosApiTblPtr + (uint32_t)((index)*4))))

#define SWI_disable              ((uint16_t (*)(void))                                                               RTOS_TABLE(SWI_DISABLE))
#define SWI_restore              ((void     (*)(uint16_t key))                                                       RTOS_TABLE(SWI_RESTORE))

#ifdef __cplusplus
}
#endif

#endif /* RTOS_API_H */
