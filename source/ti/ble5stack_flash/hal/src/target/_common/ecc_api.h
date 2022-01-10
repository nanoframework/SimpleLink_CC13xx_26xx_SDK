/******************************************************************************

 @file  ecc_api.h

 @brief Header for ECC proxy for stack's interface to the ECC driver.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef ECC_API_H
#define ECC_API_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <ti/drivers/ECDH.h>

#if !defined(DeviceFamily_CC26X1)
#include <ti/drivers/ecdh/ECDHCC26X2.h>
#else
#include <ti/drivers/ecdh/ECDHCC26X1.h>
#endif

#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#include <ti/drivers/cryptoutils/ecc/ECCParams.h>
#include <ti/drivers/cryptoutils/sharedresources/CryptoResourceCC26XX.h>

/*******************************************************************************
 * INCLUDES
 */


extern uint32_t *eccDrvTblPtr;

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

// ECC proxy index for ECC driver API
#define ECDH_INIT                        0
#define ECDH_PARAMS_INIT                 1
#define ECDH_OPEN                        2
#define ECDH_CLOSE                       3
#define ECDH_GEN_PUBKEY_INIT             4
#define ECDH_COMP_SHAREDSEC_INIT         5
#define ECDH_GEN_PUBKEY                  6
#define ECDH_COMP_SHAREDSECKEY           7

/*
** ECC API Proxy
*/

#define ECDH_TABLE( index )   (*((uint32_t *)((uint32_t)eccDrvTblPtr + (uint32_t)((index)*4))))

#define ECDH_init                               ((void (*)(void))                                                      \
                                                                      ECDH_TABLE(ECDH_INIT))
#define ECDH_Params_init                        ((void (*)(ECDH_Params *))                                             \
                                                                      ECDH_TABLE(ECDH_PARAMS_INIT))
#define ECDH_open                               ((ECDH_Handle (*)(uint_least8_t, const ECDH_Params *))                 \
                                                                      ECDH_TABLE(ECDH_OPEN))
#define ECDH_close                              ((void (*)(ECDH_Handle))                                               \
                                                                      ECDH_TABLE(ECDH_CLOSE))
#define ECDH_OperationGeneratePublicKey_init    ((void (*)(ECDH_OperationGeneratePublicKey *))                         \
                                                                      ECDH_TABLE(ECDH_GEN_PUBKEY_INIT))
#define ECDH_OperationComputeSharedSecret_init  ((void (*)(ECDH_OperationComputeSharedSecret *))                       \
                                                                      ECDH_TABLE(ECDH_COMP_SHAREDSEC_INIT))
#define ECDH_generatePublicKey                  ((int_fast16_t (*)(ECDH_Handle, ECDH_OperationGeneratePublicKey  *))   \
                                                                      ECDH_TABLE(ECDH_GEN_PUBKEY))
#define ECDH_computeSharedSecret                ((int_fast16_t (*)(ECDH_Handle, ECDH_OperationComputeSharedSecret  *)) \
                                                                      ECDH_TABLE(ECDH_COMP_SHAREDSECKEY))

#ifdef __cplusplus
}
#endif

#endif /* ECC_API_H */
