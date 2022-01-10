/******************************************************************************

 @file  zaesccm_osal.c

 @brief Support for CCM authentication using osal.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2013 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/******************************************************************************
 * INCLUDES
 */

#include "osal.h"
#include "hal_aes.h"
#include "hal_ccm.h"
#include "hal_assert.h"
#include "zaesccm_api.h"
#include "hal_mcu.h"

/******************************************************************************
 * MACROS
 */
#define AES_NO_RESOURCES              0x1A

/******************************************************************************
 * CONSTANTS
 */

/******************************************************************************
 * TYPEDEFS
 */

/******************************************************************************
 * LOCAL VARIABLES
 */

/******************************************************************************
 * GLOBAL VARIABLES
 */

/* Flag to ensure AES not reentrant */
volatile uint8 aesInUse = FALSE;

/******************************************************************************
 * FUNCTION PROTOTYPES
 */

/* see zaesccm_api.h */
signed char zaesccmAuthEncrypt(unsigned char encrypt,
                               unsigned char Mval, unsigned char *Nonce,
                               unsigned char *M, unsigned short len_m,
                               unsigned char *A, unsigned short len_a,
                               unsigned char *AesKey,
                               unsigned char *MAC, unsigned char ccmLVal)
{

  unsigned char status;
  halIntState_t intState;

  HAL_ENTER_CRITICAL_SECTION( intState );

 /* Make sure AES is not already in-use */
  if(aesInUse)
  {
    HAL_EXIT_CRITICAL_SECTION( intState );
    return AES_NO_RESOURCES;
  }

  aesInUse = TRUE;

  /* Initialize AES key */
  ssp_HW_KeyInit( AesKey );

  /* This is required for the MSP430 platform */
  pSspAesEncrypt = sspAesEncryptHW;

  status = SSP_CCM_Auth_Encrypt (encrypt, Mval, Nonce, M, len_m, A,
                        len_a, AesKey, MAC, ccmLVal);

  aesInUse = FALSE;
  HAL_EXIT_CRITICAL_SECTION( intState );
  return status;
}


/* see zaesccm_api.h */
signed char zaesccmDecryptAuth(unsigned char decrypt,
                               unsigned char Mval, unsigned char *Nonce,
                               unsigned char *M, unsigned short len_m,
                               unsigned char *A, unsigned short len_a,
                               unsigned char *AesKey,
                               unsigned char *MAC, unsigned char ccmLVal)
{
  unsigned char status;
  halIntState_t intState;

  HAL_ENTER_CRITICAL_SECTION( intState );

  /* Make sure AES is not already in-use */
  if(aesInUse)
  {
    HAL_EXIT_CRITICAL_SECTION( intState );
    return AES_NO_RESOURCES;
  }
  aesInUse = TRUE;

  /* Initialize AES key */
  ssp_HW_KeyInit( AesKey );

  /* This is required for the MSP430 platform */
  pSspAesEncrypt = sspAesEncryptHW;

  status = SSP_CCM_InvAuth_Decrypt(decrypt, Mval, Nonce, M, len_m, A, len_a,
                          AesKey, MAC, ccmLVal);
  aesInUse = FALSE;
  HAL_EXIT_CRITICAL_SECTION( intState );
  return (status);
}
