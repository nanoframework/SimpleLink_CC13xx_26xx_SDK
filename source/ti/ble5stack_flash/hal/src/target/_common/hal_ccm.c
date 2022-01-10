/******************************************************************************

 @file  hal_ccm.c

 @brief Support for Hardware CCM authentication.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2011 $
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
#include <driverlib/crypto.h>


/******************************************************************************
 * MACROS
 */

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

/******************************************************************************
 * FUNCTION PROTOTYPES
 */

/******************************************************************************
 * @fn      SSP_CCM_Auth_Encrypt
 *
 * @brief   Generates CCM Authentication tag U.
 *
 * input parameters
 * @param encrypt if set to 'true' then run encryption and set to 'flase' for
 * authentication only.
 * @param   Mval    - Length of authentication field in octets [0,2,4,6,8,10,12,14 or 16]
 * @param   N       - Pointer to 13-byte Nonce
 * @param   M       - Pointer to octet string 'm'
 * @param   len_m   - Length of M[] in octets
 * @param   A       - Pointer to octet string 'a'
 * @param   len_a   - Length of A[] in octets
 * @param   AesKey  - Pointer to AES Key or Pointer to Key Expansion buffer.
 * @param   Cstate  - Pointer to output buffer
 * @param   ccmLVal - ccm L Value to be used.
 *
 * output parameters
 *
 * @param   Cstate[]    - The first Mval bytes contain Authentication Tag T
 *
 * @return  ZStatus_t
 *
 */
uint8 SSP_CCM_Auth_Encrypt (bool encrypt, uint8 Mval, uint8 *N, uint8 *M, uint16 len_m, uint8 *A,
                    uint16 len_a, uint8 *AesKey, uint8 *Cstate, uint8 ccmLVal)
{

  unsigned char status;

  if((status = CRYPTOCcmAuthEncrypt(encrypt, Mval, (uint32_t *)N, (uint32_t *)M,  len_m, (uint32_t *)A, len_a, 0,
                                            ccmLVal, false)) != AES_SUCCESS)
  {
    return status;
  }

  do
  {
    ASM_NOP;
  }while((CRYPTOCcmAuthEncryptStatus()));

  if((status = CRYPTOCcmAuthEncryptResultGet(Mval, (uint32_t *)Cstate)) != AES_SUCCESS)
  {
    return status;
  }
  return AES_SUCCESS;
}


/******************************************************************************
 * @fn      SSP_CCM_InvAuth_Decrypt
 *
 * @brief   Verifies CCM authentication.
 *
 * input parameters
 * @param decrypt if set to 'true' then run decryption and set to 'flase' for
 * authentication only.
 * @param   Mval    - Length of authentication field in octets [0,2,4,6,8,10,12,14 or 16]
 * @param   N       - Pointer to 13-byte Nonce
 * @param   C       - Pointer to octet string 'c' = 'm' || auth tag T
 * @param   len_c   - Length of C[] in octets
 * @param   A       - Pointer to octet string 'a'
 * @param   len_a   - Length of A[] in octets
 * @param   AesKey  - Pointer to AES Key or Pointer to Key Expansion buffer.
 * @param   Cstate  - Pointer to AES state buffer (cannot be part of C[])
 * @param   ccmLVal - ccm L Value to be used.
 *
 * output parameters
 *
 * @param   Cstate[]    - The first Mval bytes contain computed Authentication Tag T
 *
 * @return  0 = Success, 1 = Failure
 *
 */
uint8 SSP_CCM_InvAuth_Decrypt (bool decrypt, uint8 Mval, uint8 *N, uint8 *C, uint16 len_c, uint8 *A,
                       uint16 len_a, uint8 *AesKey, uint8 *Cstate, uint8 ccmLVal)
{

  unsigned char status;
  if((status = CRYPTOCcmInvAuthDecrypt(decrypt, Mval, (uint32_t *)N, (uint32_t *)C, len_c, (uint32_t *)A, len_a, 0,
                                         ccmLVal, false))!= AES_SUCCESS )
  {
    return status;
  }

  /* wait for completion of the operation */
  do
  {
    ASM_NOP;
  }while((CRYPTOCcmInvAuthDecryptStatus()));


  if((status = CRYPTOCcmInvAuthDecryptResultGet(Mval, (uint32_t *)C, len_c, (uint32_t *)Cstate)) != AES_SUCCESS)
  {
    return status;
  }

  return AES_SUCCESS;
}
