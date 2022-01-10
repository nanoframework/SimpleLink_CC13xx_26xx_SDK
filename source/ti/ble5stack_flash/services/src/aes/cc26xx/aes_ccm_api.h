/******************************************************************************

 @file  aes_ccm_api.h

 @brief AES CCM service (OS dependent) interface

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2012 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef AESCCM_API_H
#define AESCCM_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Authenticates and encrypts a text using an MLE key.<br>
 * This function is thread-safe.
 *
 * @param  encrypt   set to TRUE to encrypt. FALSE to just authenticate without encryption.
 * @param  Mval      length of authentication field in octets
 * @param  N         13 byte nonce
 * @param  M         octet string 'm'
 * @param  len_m     length of M[] in octets
 * @param  A         octet string 'a'
 * @param  len_a     length of A[] in octets
 * @param  AesKey    Pointer to AES Key or Pointer to Key Expansion buffer
 * @param  MAC      MAC output buffer
 * @param  ccmLVal   ccm L value to be used
 *
 * @return Zero when successful. Non-zero, otherwise.
 */
extern signed char AesCcm_authEncrypt_Sw(uint8_t encrypt,
                                      uint8_t Mval, uint8_t *Nonce,
                                      uint8_t *M, unsigned short len_m,
                                      uint8_t *A, unsigned short len_a,
                                      uint8_t *AesKey,
                                      uint8_t *MAC, uint8_t ccmLVal);

/**
 * Decrypts and authenticates an encrypted text using an MLE key.
 * This function is thread-safe.
 *
 * @param  decrypt   set to TRUE to decrypt. Set to FALSE to authenticate without decryption.
 * @param  Mval      length of authentication field in octets
 * @param  N         13 byte nonce
 * @param  M         octet string 'm'
 * @param  len_m     length of M[] in octets
 * @param  A         octet string 'a'
 * @param  len_a     length of A[] in octets
 * @param  AesKey    Pointer to AES Key or Pointer to Key Expansion buffer.
 * @param  MAC    MAC output buffer
 * @param  ccmLVal   ccm L value to be used
 *
 * @return Zero when successful. Non-zero, otherwise.
 */
extern signed char AesCcm_decryptAuth_Sw(uint8_t decrypt,
                                      uint8_t Mval, uint8_t *Nonce,
                                      uint8_t *M, unsigned short len_m,
                                      uint8_t *A, unsigned short len_a,
                                      uint8_t *AesKey,
                                      uint8_t *MAC, uint8_t ccmLVal);
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
extern void Aes_encrypt_Sw( uint8_t *plainText, uint16_t textLen, uint8_t *pAesKey);

/**
 * @fn     aes_Decrypt
 *
 * @brief  decrypts data which was encrypted with AES128 encryption.  Plain-text
 *         data is returned back in the same pointer.
 *
 * @param  cypherText - Cypher-text to be decrypted.
 *                      Output will be written to this pointer.
 *
 * @param  textLen    - length of encrypted text.
 *
 * @param  pAesKey    - pointer to the AES key.
 *
 * @ret    none
 */
extern void Aes_decrypt_Sw( uint8_t *cypherText, uint16_t textLen, uint8_t* pAesKey);

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
extern uint16_t AesCcm_getNumBlocks_Sw(uint16_t textLength);


#ifdef __cplusplus
}
#endif

#endif /* BAESCCM_API_H */
