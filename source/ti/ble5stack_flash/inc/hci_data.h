/******************************************************************************

 @file  hci_data.h

 @brief This file handles HCI data for the BLE Controller.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: TI_TEXT 2009 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef HCI_C_DATA_H
#define HCI_C_DATA_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*
** HCI Data API
*/


/*******************************************************************************
 * @fn          HCI_ReverseBytes
 *
 * @brief       This function is used to reverse the order of the bytes in
 *              an array in place.
 *
 * input parameters
 *
 * @param       *buf - Pointer to buffer containing bytes to be reversed.
 * @param       len  - Number of bytes in buffer.
 *
 *              Note: The length must be even.
 *
 *              Note: The maximum length is 128 bytes.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern void HCI_ReverseBytes( uint8 *buf,
                              uint8 len );


#ifdef __cplusplus
}
#endif

#endif /* HCI_C_DATA_H */
