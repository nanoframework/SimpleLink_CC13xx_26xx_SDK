/******************************************************************************

 @file  crypto_api.h

 @brief Header for Crypto proxy for stack's interface to the crypto driver.

 Group: WCS, LPC/BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef CRYPTO_API_H
#define CRYPTO_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */
#include <ti/drivers/AESCCM.h>
#include <ti/drivers/aesccm/AESCCMCC26XX.h>
#include <ti/drivers/AESECB.h>
#include <ti/drivers/aesecb/AESECBCC26XX.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#include <ti/drivers/cryptoutils/sharedresources/CryptoResourceCC26XX.h>

extern uint32_t *cryptoDrvTblPtr;

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

// Crypto proxy index for crypto driver API
#define AESCCM_INIT                       0
#define AESCCM_OPEN                       1
#define AESCCM_CLOSE                      2
#define AESCCM_PARAMS_INIT                3
#define AESCCM_OPERATION_INIT             4
#define AESCCM_ONESTEPENCRYPT             5
#define AESCCM_ONESTEPDECRYPT             6
#define AESECB_INIT                       7
#define AESECB_OPEN                       8
#define AESECB_CLOSE                      9
#define AESECB_PARAMS_INIT                10
#define AESECB_OPERATION_INIT             11
#define AESECB_ONESTEPENCRYPT             12
#define AESECB_ONESTEPDECRYPT             13
#define CRYPTOKEYPLAINTEXT_INITKEY        14
#define CRYPTOKEYPLAINTEXT_INITBLANKKEY   15

/*
** Crypto API Proxy
*/

#define CRYPTO_TABLE(index)             (*((uint32_t *)((uint32_t)cryptoDrvTblPtr + (uint32_t)((index)*4))))

#define AESCCM_init                     ((void                (*)(void))                               \
                                          CRYPTO_TABLE(AESCCM_INIT))
#define AESCCM_open                     ((AESCCM_Handle (*)(uint_least8_t, const AESCCM_Params*))      \
                                          CRYPTO_TABLE(AESCCM_OPEN))
#define AESCCM_close                    ((int16_t             (*)(AESCCM_Handle))                      \
                                         CRYPTO_TABLE(AESCCM_CLOSE))
#define AESCCM_Params_init              ((void                (*)(AESCCM_Params *))                    \
                                          CRYPTO_TABLE(AESCCM_PARAMS_INIT))
#define AESCCM_Operation_init           ((void                (*)(AESCCM_Operation *))                 \
                                         CRYPTO_TABLE(AESCCM_OPERATION_INIT))
#define AESCCM_oneStepEncrypt           ((int16_t             (*)(AESCCM_Handle, AESCCM_Operation *))  \
                                         CRYPTO_TABLE(AESCCM_ONESTEPENCRYPT))
#define AESCCM_oneStepDecrypt           ((int16_t             (*)(AESCCM_Handle, AESCCM_Operation *))  \
                                         CRYPTO_TABLE(AESCCM_ONESTEPDECRYPT))
#define AESECB_init                     ((void                (*)(void))                               \
                                          CRYPTO_TABLE(AESECB_INIT))
#define AESECB_open                     ((AESECB_Handle (*)(unsigned int, AESECB_Params*))             \
                                          CRYPTO_TABLE(AESECB_OPEN))
#define AESECB_close                    ((int                 (*)(AESECB_Handle))                      \
                                         CRYPTO_TABLE(AESECB_CLOSE))
#define AESECB_Params_init              ((void                (*)(AESECB_Params *))                    \
                                          CRYPTO_TABLE(AESECB_PARAMS_INIT))
#define AESECB_Operation_init           ((void                (*)(AESECB_Operation *))                 \
                                         CRYPTO_TABLE(AESECB_OPERATION_INIT))
#define AESECB_oneStepEncrypt           ((int                (*)(AESECB_Handle, AESECB_Operation *))   \
                                         CRYPTO_TABLE(AESECB_ONESTEPENCRYPT))
#define AESECB_oneStepDecrypt           ((int                (*)(AESECB_Handle, AESECB_Operation *))   \
                                         CRYPTO_TABLE(AESECB_ONESTEPDECRYPT))
#define CryptoKeyPlaintext_initKey      ((int                 (*)(CryptoKey *, uint8_t *, size_t))     \
                                          CRYPTO_TABLE(CRYPTOKEYPLAINTEXT_INITKEY))
#define CryptoKeyPlaintext_initBlankKey ((int                 (*)(CryptoKey *, uint8_t *, size_t))     \
                                          CRYPTO_TABLE(CRYPTOKEYPLAINTEXT_INITBLANKKEY))

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_API_H */
