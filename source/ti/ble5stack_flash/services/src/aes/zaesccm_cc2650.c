/******************************************************************************

 @file  zaesccm_cc2650.c

 @brief Support for CCM authentication using ICall and TIRTOS .

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

#include "zaesccm_api.h"
#include "hal_mcu.h"
#include <driverlib/crypto.h>
#include "crypto_board.h"

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
  char status;

  /* Initialize AES key */
  status = CryptoBoard_AesLoadKey(AesKey, AESKEY_0);

  if(status == AES_SUCCESS)
  {
    /* CCM encrypt and authenticate */
    status = CryptoBoard_CCMEncrypt(encrypt, Mval, Nonce, M, len_m, A,
                                    len_a, MAC, ccmLVal, AESKEY_0);
    CryptoBoard_AesReleaseKey(AESKEY_0);
  }

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
  char status;
  /* Initialize AES key */
  status = CryptoBoard_AesLoadKey(AesKey, AESKEY_0);

  if(status == AES_SUCCESS)
  {
    /* CCM inverse authenticate and decrypt  */
    status = CryptoBoard_CCMDecrypt(decrypt, Mval, Nonce, M, len_m, A,
                                    len_a, MAC, ccmLVal, AESKEY_0);
    CryptoBoard_AesReleaseKey(AESKEY_0);
  }
  return status;
}
