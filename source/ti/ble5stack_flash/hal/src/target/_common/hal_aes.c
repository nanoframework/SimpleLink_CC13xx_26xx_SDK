/******************************************************************************

 @file  hal_aes.c

 @brief Support for Hardware AES encryption.

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
#include "hal_mcu.h"
#include "hal_ccm.h"
#include <inc/hw_crypto.h>
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
void (*pSspAesEncrypt)( uint8 *, uint8 * ) = (void*)NULL;

/******************************************************************************
 * FUNCTION PROTOTYPES
 */

/******************************************************************************
 * @fn      HalAesInit
 *
 * @brief   Initilize AES engine
 *
 * input parameters
 *
 * @param   None
 *
 * @return  None
 */
void HalAesInit( void )
{
  HWREG(CRYPTO_BASE + CRYPTO_O_IRQTYPE) |= 0x00000001;
  HWREG(CRYPTO_BASE + CRYPTO_O_IRQEN) |= 0x00000003;
}


/******************************************************************************
 * @fn      ssp_HW_KeyInit
 *
 * @brief   Writes the key into AES engine
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 *
 * @return  None
 */
void ssp_HW_KeyInit( uint8 *AesKey )
{
  /* Load the AES key
   * KeyStore has rentention after PM2
   */
  CRYPTOAesLoadKey( (uint32_t *)AesKey, 0);
}


/******************************************************************************
 * @fn      sspAesEncryptHW
 *
 * @brief   Encrypts 16 byte block using AES encryption engine
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 * @param   Cstate  - Pointer to input data.
 *
 * output parameters
 *
 * @param   Cstate  - Pointer to encrypted data.
 *
 * @return  None
 *
 */
void sspAesEncryptHW( uint8 *AesKey, uint8 *Cstate )
{
  /* please note that ssp_HW_KeyInit(AesKey); should have already
   * been called prior to using this function
   */
  CRYPTOAesEcb( (uint32_t *)Cstate, (uint32_t *)Cstate, 0, true, false);

  /* wait for completion of the operation */
  do
  {
    ASM_NOP;
  }while(CRYPTOAesEcbStatus());

  CRYPTOAesEcbFinish();
}


/******************************************************************************
 * @fn      sspAesDecryptHW
 *
 * @brief   Decrypts 16 byte block using AES encryption engine
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 * @param   Cstate  - Pointer to input data.
 *
 * output parameters
 *
 * @param   Cstate  - Pointer to decrypted data.
 *
 * @return  None
 *
 */
void sspAesDecryptHW( uint8 *AesKey, uint8 *Cstate )
{
  /* please note that ssp_HW_KeyInit(AesKey); should have already
   * been called prior to using this function
   */
  CRYPTOAesEcb( (uint32_t *)Cstate, (uint32_t *)Cstate, 0, false, false);

  /* wait for completion of the operation */
  do
  {
    ASM_NOP;
  }while(!(CRYPTOAesEcbStatus()));

  CRYPTOAesEcbFinish();
}


/******************************************************************************
 * @fn      sspAesEncryptHW_keylocation
 *
 * @brief   Encrypts 16 byte block using AES encryption engine
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 * @param   Cstate  - Pointer to input data.
 *
 * output parameters
 *
 * @param   Cstate  - Pointer to encrypted data.
 *
 * @return  None
 *
 */
void sspAesEncryptHW_keylocation(uint8 *msg_in, uint8 *msg_out, uint8 key_location )
{
  /* please note that ssp_HW_KeyInit(AesKey); should have already
   * been called prior to using this function
   */
  CRYPTOAesEcb( (uint32_t *)msg_in, (uint32_t *)msg_out, key_location, true, false);

  /* wait for completion of the operation */
  do
  {
    ASM_NOP;
  }while(!(CRYPTOAesEcbStatus()));

  CRYPTOAesEcbFinish();
}


/******************************************************************************
 * @fn      sspAesDecryptHW_keylocation
 *
 * @brief   Decrypts 16 byte block using AES decryption engine
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 * @param   Cstate  - Pointer to input data.
 *
 * output parameters
 *
 * @param   Cstate  - Pointer to encrypted data.
 *
 * @return  None
 *
 */
void sspAesDecryptHW_keylocation( uint8 *msg_in, uint8 *msg_out, uint8 key_location )
{
  /* please note that ssp_HW_KeyInit(AesKey); should have already
   * been called prior to using this function
   */
  CRYPTOAesEcb( (uint32_t *)msg_in, (uint32_t *)msg_out, key_location, false, false);

  /* wait for completion of the operation */
  do
  {
    ASM_NOP;
  }while(CRYPTOAesEcbStatus());

  CRYPTOAesEcbFinish();
}
