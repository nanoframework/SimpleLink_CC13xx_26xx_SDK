/******************************************************************************

 @file  zaesccm_api.h

 @brief AES CCM service (OS dependent) interface

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2012 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
#ifndef ZAESCCM_API_H
#define ZAESCCM_API_H

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
 * @param  MAC       MAC output buffer
 * @param  ccmLVal   ccm L value to be used
 *
 * @return Zero when successful. Non-zero, otherwise.
 */
extern signed char zaesccmAuthEncrypt(unsigned char encrypt,
                                      unsigned char Mval, unsigned char *Nonce,
                                      unsigned char *M, unsigned short len_m,
                                      unsigned char *A, unsigned short len_a,
                                      unsigned char *AesKey,
                                      unsigned char *MAC, unsigned char ccmLVal);

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
 * @param  MAC       MAC output buffer
 * @param  ccmLVal   ccm L value to be used
 *
 * @return Zero when successful. Non-zero, otherwise.
 */
extern signed char zaesccmDecryptAuth(unsigned char decrypt,
                                      unsigned char Mval, unsigned char *Nonce,
                                      unsigned char *M, unsigned short len_m,
                                      unsigned char *A, unsigned short len_a,
                                      unsigned char *AesKey,
                                      unsigned char *MAC, unsigned char ccmLVal);

/**
 * Locks mutex used by AES CCM service module.
 * Note that this function is not provided by AES CCM service module but instead it has
 * to be implemented in order to link and use AES CCM service.
 */
extern void zaesccmMutexLock(void);

/**
 * Unlocks mutex used by AES CCM service module.
 * Note that this function is not provided by AES CCM service module but instead it has
 * to be implemented in order to link and use AES CCM service.
 */
extern void zaesccmMutexUnlock(void);

#ifdef __cplusplus
}
#endif

#endif /* ZAESCCM_API_H */
