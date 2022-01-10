/******************************************************************************

 @file  hal_ccm.h

 @brief Describe the purpose and contents of the file.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2007 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef     HAL_CCM_H_
#define     HAL_CCM_H_

Status_t SSP_CCM_Auth_Encrypt (bool, uint8, uint8 *, uint8 *, uint16, uint8 *, uint16, uint8 *, uint8 *, uint8);
Status_t SSP_CCM_InvAuth_Decrypt (bool, uint8, uint8 *, uint8 *, uint16, uint8 *, uint16, uint8 *, uint8 *, uint8);

#endif  // HAL_CCM_H_
