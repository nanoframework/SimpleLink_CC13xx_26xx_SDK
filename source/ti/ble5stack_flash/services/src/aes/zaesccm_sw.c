/******************************************************************************

 @file  zaesccm_sw.c

 @brief AES CCM service (OS dependent) implementation.

        This particular module is using software implementation of AES CCM.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2012 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#include "zaesccm_api.h"

#include "hal_types.h"
#include "ccm.h"
#include "aes.h"

/* see zaesccm_api.h */
signed char zaesccmAuthEncrypt(unsigned char encrypt,
                               unsigned char Mval, unsigned char *N,
                               unsigned char *M, unsigned short len_m,
                               unsigned char *A, unsigned short len_a,
                               unsigned char *AesKey,
                               unsigned char *MAC, unsigned char ccmLVal)
{
  zaesccmMutexLock();

  /* initialize AES engine with the designated key */
  ssp_HW_KeyInit(AesKey);

  /* This is required as legacy interface for the MSP430 platform */
  pSspAesEncrypt = sspAesEncryptHW;

  /* Run authentication + encryption */
  if (SSP_CCM_Auth_Encrypt(encrypt, Mval, N, M, len_m, A, len_a,
                           AesKey, MAC, ccmLVal) != 0)
  {
    zaesccmMutexUnlock();
    return -2;
  }

  zaesccmMutexUnlock();
  return 0;
}

/* see zaesccm_api.h */
signed char zaesccmDecryptAuth(unsigned char decrypt,
                               unsigned char Mval, unsigned char *N,
                               unsigned char *M, unsigned short len_m,
                               unsigned char *A, unsigned short len_a,
                               unsigned char *AesKey,
                               unsigned char *Cstate, unsigned char ccmLVal)
{
  zaesccmMutexLock();

  /* initialize AES engine with the designated key */
  ssp_HW_KeyInit(AesKey);

  /* This is required as legacy interface for MSP430 platform */
  pSspAesEncrypt = sspAesEncryptHW;

  /* Run decryption + inverse authentication */
  if (SSP_CCM_InvAuth_Decrypt(decrypt, Mval, N, M, len_m, A, len_a,
                              AesKey, Cstate, ccmLVal) != 0)
  {
    zaesccmMutexUnlock();
    return -2;
  }

  zaesccmMutexUnlock();
  return 0;
}
