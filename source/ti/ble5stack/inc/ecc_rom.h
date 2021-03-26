/******************************************************************************

 @file  ecc_rom.h

 @brief This header file is the API to the ECC module built into
        ROM on the CC26xx.

 Group: WCS, BTS
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2014-2021, Texas Instruments Incorporated
 All rights reserved.

 IMPORTANT: Your use of this Software is limited to those specific rights
 granted under the terms of a software license agreement between the user
 who downloaded the software, his/her employer (which must be your employer)
 and Texas Instruments Incorporated (the "License"). You may not use this
 Software unless you agree to abide by the terms of the License. The License
 limits your use, and you acknowledge, that the Software may not be modified,
 copied or distributed unless embedded on a Texas Instruments microcontroller
 or used solely and exclusively in conjunction with a Texas Instruments radio
 frequency transceiver, which is integrated into your product. Other than for
 the foregoing purpose, you may not use, reproduce, copy, prepare derivative
 works of, modify, distribute, perform, display or sell this Software and/or
 its documentation for any purpose.

 YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
 PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
 NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
 TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
 NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
 LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
 INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
 OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
 OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

 Should you have any questions regarding your right to use this Software,
 contact Texas Instruments Incorporated at www.TI.com.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef ECC_ROM_H
#define ECC_ROM_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

/* Window size, valid values are 2,3,4,5.
 * Higher the value, faster the computation at the expense of memory usage.
 *
 * Recommended workzone size (in 4-byte words)
 * Window size: 3, Workzone size: 275
 *
 */

/*
 * ECC Supported Curves, define one:
 * ECC_PRIME_NIST256_CURVE
 */
#define ECC_PRIME_NIST256_CURVE

/*
 * ECC Return Status Flags.
 */
// Scalar multiplication status
#define ECC_MODULUS_EVEN                   0xDC
#define ECC_MODULUS_LARGER_THAN_255_WORDS  0xD2
#define ECC_MODULUS_LENGTH_ZERO            0x08
#define ECC_MODULUS_MSW_IS_ZERO	           0x30
#define ECC_SCALAR_TOO_LONG                0x35
#define ECC_SCALAR_LENGTH_ZERO	           0x53
#define ECC_ORDER_TOO_LONG                 0xC6
#define ECC_ORDER_LENGTH_ZERO	           0x6C
#define ECC_X_COORD_TOO_LONG	           0x3C
#define ECC_X_COORD_LENGTH_ZERO	           0xC3
#define ECC_Y_COORD_TOO_LONG	           0x65
#define ECC_Y_COORD_LENGTH_ZERO	           0x56
#define ECC_A_COEF_TOO_LONG                0x5C
#define ECC_A_COEF_LENGTH_ZERO	           0xC5
#define ECC_BAD_WINDOW_SIZE                0x66
#define ECC_SCALAR_MUL_OK                  0x99

// ECDSA and ECDH status
#define ECC_ORDER_LARGER_THAN_255_WORDS	   0x28
#define ECC_ORDER_EVEN                     0x82
#define ECC_ORDER_MSW_IS_ZERO              0x23
#define ECC_ECC_KEY_TOO_LONG               0x25
#define ECC_ECC_KEY_LENGTH_ZERO            0x52
#define ECC_DIGEST_TOO_LONG                0x27
#define ECC_DIGEST_LENGTH_ZERO             0x72
#define ECC_ECDSA_SIGN_OK                  0x32
#define ECC_ECDSA_INVALID_SIGNATURE        0x5A
#define ECC_ECDSA_VALID_SIGNATURE          0xA5
#define ECC_SIG_P1_TOO_LONG                0x11
#define ECC_SIG_P1_LENGTH_ZERO             0x12
#define ECC_SIG_P2_TOO_LONG                0x22
#define ECC_SIG_P2_LENGTH_ZERO             0x21

#define ECC_ECDSA_KEYGEN_OK                ECC_SCALAR_MUL_OK
#define ECC_ECDH_KEYGEN_OK                 ECC_SCALAR_MUL_OK
#define ECC_ECDH_COMMON_KEY_OK             ECC_SCALAR_MUL_OK


/*********************************************************************
 * API FUNCTIONS
 */

/*
 * Function:    ECC_initialize
 *
 * Description: pass pointer to ECC memory allocation to ECC engine.  This
 *              function can be called again to point the ECC workzone at
 *              a different memory buffer.
 *
 * Parameters:
 *              pWorkzone  - pointer to memory allocated for computations, input.
 *                           See description at beginning of ECC section for
 *                           memory requirements.
 *              windowSize - operation window, determines speed vs RAM tradeoff.
 *                           2 is the most RAM efficient and slowest.  5 is
 *                           quickest but with the greatest RAM tradeoff.
 *
 * Return:      None
 */
extern void ECC_initialize(uint32_t *pWorkzone, uint8_t windowSize);

 /*
 * Function:    ECC_generateKey
 *
 * Description: generates a key.  This is used for both ECDH and ECDSA.
 *
 * Parameters:
 *              randString  - random string, input.
 *
 *              privateKey  - the private key, output.
 *              publicKey_x - public key X-coordinate, output.
 *              publicKey_y - public key Y-coordinate, output.
 *
 * Return:      Status
 */
extern uint8_t ECC_generateKey(uint32_t *randString, uint32_t *privateKey,
                               uint32_t *publicKey_x, uint32_t *publicKey_y);

/*
 * Function:    ECC_ECDH_computeSharedSecret
 *
 * Description: compute the shared secret.
 *
 * Parameters:
 *              privateKey     - private key, input.
 *              publicKey_x    - public key X-coordinate, input.
 *              publicKey_y    - public key Y-coordinate, input.
 *
 *              sharedSecret_x - shared secret X-coordinate, output.
 *              sharedSecret_y - shared secret Y-coordinate, output.
 *
 * Return:      Status
 */
extern uint8_t ECC_ECDH_computeSharedSecret(uint32_t *privateKey,
                                            uint32_t *publicKey_x,
                                            uint32_t *publicKey_y,
                                            uint32_t *sharedSecret_x,
                                            uint32_t *sharedSecret_y);

#ifdef __cplusplus
}
#endif

#endif /* ECC_ROM_H */
