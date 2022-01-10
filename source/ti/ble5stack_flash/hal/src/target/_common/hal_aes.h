/******************************************************************************

 @file  hal_aes.h

 @brief Support for HW/SW AES encryption.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2007 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef     HAL_AES_H_
#define     HAL_AES_H_

#include "zcomdef.h"
#include <inc/hw_types.h>
#include "hal_board.h"
#include <driverlib/crypto.h>

/* AES and Keystore functions */
extern void HalAesInit( void );
extern void ssp_HW_KeyInit (uint8 *);
extern void sspAesDecryptHW (uint8 *, uint8 *);
extern void sspAesEncryptHW_keylocation( uint8 *, uint8 *, uint8 );
extern void sspAesDecryptHW_keylocation( uint8 *, uint8 *, uint8 );
extern void (*pSspAesEncrypt)( uint8 *, uint8 * );
extern void sspAesEncryptHW (uint8 *, uint8 *);

#define     STATE_BLENGTH   16      // Number of bytes in State
#define     KEY_BLENGTH     16      // Number of bytes in Key
#define     KEY_EXP_LENGTH  176     // Nb * (Nr+1) * 4

/* AES Engine is default to hardware AES. To turn on software AES, #define one of the followings:
 * #define SOFTWARE_AES TRUE, uses software aes  ( slowest setting )
 * #define SW_AES_AND_KEY_EXP TRUE, enables software aes with key expansion ( improves speed at the cost of 176 bytes of data (RAM) )
 */
#if ((defined SOFTWARE_AES) && (SOFTWARE_AES == TRUE)) && ((defined SW_AES_AND_KEY_EXP) && (SW_AES_AND_KEY_EXP == TRUE))
#error "SOFTWARE_AES and SW_AES_AND_KEY_EXP cannot be both defined."
#endif

extern void HalAesInit( void );
extern uint8 AesLoadKey( uint8 * );

extern void (*pSspAesEncrypt)( uint8 *, uint8 * );
extern void ssp_HW_KeyInit (uint8 *);


extern void sspAesEncryptHW (uint8 *, uint8 *);

#define AES_BUSY    0x08
#define ENCRYPT     0x00
#define DECRYPT     0x01

// _mode_ is one of
#define CBC         0x00000020
#define CTR         0x00000040
#define ECB         0x1FFFFFE0
#define CBC_MAC     0x00008000
#define CCM         0x00040000

// Macro for setting the mode of the AES operation
#define AES_SETMODE_ECB do { HWREG(AES_AES_CTRL) &= ~ECB; } while (0)
#define AES_SETMODE(mode) do { HWREG(AES_AES_CTRL) &= ~mode; HW_REG(AES_AES_CTRL) |= mode} while (0)



#endif  // HAL_AES_H_
