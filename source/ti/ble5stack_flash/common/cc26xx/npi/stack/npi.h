/******************************************************************************

 @file  npi.h

 @brief This file contains the Network Processor Interface (NPI), which
        abstracts the physical link between the Application
        Processor (AP) and the Network Processor (NP). The NPI serves as the
        HAL's client for the SPI and UART drivers, and provides
        API and callback services for its client.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2008 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef NPI_H
#define NPI_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"
#include "osal.h"

/*******************************************************************************
 * CONSTANTS
 */

#define NPI_UART_RX_BUF_SIZE           270//  This should be common to app/stack
#define NPI_UART_TX_BUF_SIZE           270//  This should be common to app/stack

/*******************************************************************************
 * TYPEDEFS
 */

typedef void (*npiCBack_t)(uint8 port, uint8 event);

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * FUNCTIONS
 */

//
// Network Processor Interface APIs
//

extern void   NPI_InitTransport(npiCBack_t npiCallback);
extern uint16 NPI_ReadTransport(uint8 *buf, uint16 len);
extern uint16 NPI_WriteTransport(uint8 *, uint16);
extern uint16 NPI_RxBufLen(void);
extern uint16 NPI_GetMaxRxBufSize(void);
extern uint16 NPI_GetMaxTxBufSize(void);
extern void NPI_RegisterTask(uint8_t taskId);

/*******************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif /* NPI_H */
