/******************************************************************************

 @file  aes_ccm_sw.c

 @brief AES CCM service (OS dependent) implementation

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2012 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#include <stdbool.h>
#include <stdint.h>

#include "ccm.h"
#include "aes.h"

#include "aes_ccm_api.h"

#include "rom.h"

/* see baesccm_api.h */
signed char AesCcm_authEncrypt_Sw(uint8_t encrypt,
                               uint8_t Mval, uint8_t *N,
                               uint8_t *M, unsigned short len_m,
                               uint8_t *A, unsigned short len_a,
                               uint8_t *AesKey,
                               uint8_t *MAC, uint8_t ccmLVal)
{
  /* initialize AES engine with the designated key */
  MAP_ssp_KeyInit_Sw(AesKey);

  /* This is required as legacy interface for the MSP430 platform */
  pSspAesEncrypt_Sw = MAP_sspAesEncrypt_Sw;

  /* Run authentication + encryption */
  if (MAP_SSP_CCM_Auth_Encrypt_Sw(encrypt, Mval, N, M, len_m, A, len_a,
                           AesKey, MAC, ccmLVal) != 0)
  {
    return -2;
  }

  return 0;
}

/* see baesccm_api.h */
signed char AesCcm_decryptAuth_Sw(uint8_t decrypt,
                               uint8_t Mval, uint8_t *N,
                               uint8_t *M, unsigned short len_m,
                               uint8_t *A, unsigned short len_a,
                               uint8_t *AesKey,
                               uint8_t *cState, uint8_t ccmLVal)
{
  /* initialize AES engine with the designated key */
  MAP_ssp_KeyInit_Sw(AesKey);

  /* This is required as legacy interface for MSP430 platform */
  pSspAesEncrypt_Sw = MAP_sspAesEncrypt_Sw;

  /* Run decryption + inverse authentication */
  if (MAP_SSP_CCM_InvAuth_Decrypt_Sw(decrypt, Mval, N, M, len_m, A, len_a,
                              AesKey, cState, ccmLVal) != 0)
  {
    return -2;
  }

  return 0;
}

/**
 * @fn     Aes_encrypt_Sw
 *
 * @brief  encrypts data with AES128 encryption.  Encrypted data is returned
 *         back in the same pointer.
 *
 * @param  plainText - Plain-text to be encrypted.
 *                     Output will be written to this pointer.
 *
 * @param  textLen   - length in bytes of plain-text.
 *
 * @param  pAesKey    - pointer to the AES key.
 *
 * @ret    none
 */
void Aes_encrypt_Sw( uint8_t *plainText, uint16_t textLen, uint8_t *pAesKey)
{
  uint16_t numBlocks = MAP_AesCcm_getNumBlocks_Sw(textLen);

  for (uint8_t i = 0; i < numBlocks; ++i)
  {
    MAP_sspAesEncryptBasic_Sw(pAesKey, &plainText[i*16]);
  }
}


/**
 * @fn     Aes_Decrypt
 *
 * @brief  decrypts data which was encrypted with AES128 encryption.  Plain-text
 *         data is returned back in the same pointer.
 *
 * @param  cypherText - Cypher-text to be decrypted.
 *                      Output will be written to this pointer.
 *
 * @param  textLen    - length of encrypted text.
 *
 * @param  pAesKey    - poitner to the AES key.
 *
 * @ret    none
 */
void Aes_decrypt_Sw( uint8_t *cypherText, uint16_t textLen, uint8_t* pAesKey)
{
  uint16_t numBlocks = MAP_AesCcm_getNumBlocks_Sw(textLen);

  for (uint8_t i = 0; i < numBlocks; ++i)
  {
    MAP_sspAesDecrypt_Sw(pAesKey, &cypherText[i*16]);
  }
}

/**
 * @fn     AesCcm_getNumBlocks_Sw
 *
 * @brief  Pass in the size, in bytes, of the text.
 *
 * @param  sizeOfText - size of text in bytes.
 *
 * @ret    number of AES 128bit blocks in a block of data.
 *
 */
uint16_t AesCcm_getNumBlocks_Sw(uint16_t textLength)
{
  //( bytes*(8bits/byte) )/(128bits/block) =  bytes/16 = number of blocks.
  return textLength >> 4;
}
