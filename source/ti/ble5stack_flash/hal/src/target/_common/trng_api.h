/******************************************************************************

 @file  trng_api.h

 @brief Header for TRNG proxy for stack's interface to the TRNG driver.

 Group: WCS, LPC/BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef TRNG_API_H
#define TRNG_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#include <ti/drivers/TRNG.h>

extern uint32_t *trngDrvTblPtr;

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

// TRNG proxy index for TRNG driver API
#define TRNG_INIT               0
#define TRNG_OPEN               1
#define TRNG_GENERATE_ENTROPY   2
#define TRNG_CLOSE              3

/*
** TRNG API Proxy
*/

#define TRNG_TABLE(index)      (*((uint32_t *)((uint32_t)trngDrvTblPtr + (uint32_t)((index)*4))))

#define TRNG_init              ((void         (*)(void                                      )) TRNG_TABLE(TRNG_INIT))
#define TRNG_open              ((TRNG_Handle  (*)(uint_least8_t index,  TRNG_Params *params )) TRNG_TABLE(TRNG_OPEN))
#define TRNG_generateEntropy   ((int_fast16_t (*)(TRNG_Handle   handle, CryptoKey   *entropy)) TRNG_TABLE(TRNG_GENERATE_ENTROPY))
#define TRNG_close             ((void         (*)(TRNG_Handle   handle                      )) TRNG_TABLE(TRNG_CLOSE))

#ifdef __cplusplus
}
#endif

#endif /* TRNG_API_H */
