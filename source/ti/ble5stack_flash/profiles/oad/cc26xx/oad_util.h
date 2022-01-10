/******************************************************************************

 @file  oad_util.h

 @brief This file contains OAD functions that can be shared between
        persistent application and user applications running either the
        oad_reset_service or the oad service.

        Note: these functions should be used in a TI-RTOS application
        with a BLE-Stack running

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2017 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
#ifndef OAD_UTIL_H
#define OAD_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <profiles/oad/cc26xx/oad.h>
#include <common/cc26xx/sha2/SHA2CC26XX.h>
 /*********************************************************************
 * @fn      OADUtil_signCommand
 *
 * @brief   Verify the signer of an OAD command
 *
 * @param   payload - pointer to command payload
 * @param   len - length of payload
 * @param   inputECCWorkzone - pointer to ECC Workzone RAM
 * @param   inputSHAWorkzone - pointer to SHA Workzone RAM
 *
 * @return  OAD_SUCCESS or OAD_AUTH_FAIL
 */
#ifdef SECURITY
extern uint8_t OADUtil_signCommandECDSA(securityHdr_t * secHdr,
                                        signPld_ECDSA_P256_t *signPld,
                                        uint8_t *payload, uint16_t len);
#endif

#ifdef __cplusplus
}
#endif

#endif /* OAD_UTIL_H */
