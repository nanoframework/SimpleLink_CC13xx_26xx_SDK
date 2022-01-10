/******************************************************************************

 @file  urfi.h

 @brief This file contains the RF Interface.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: TISD 2009 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef URFI_H
#define URFI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <driverlib/rf_ble_cmd.h>

/*********************************************************************
 * CONSTANTS
 */

/* Invalid RF command handle */
#define URFI_CMD_HANDLE_INVALID   RF_ALLOC_ERROR

/*********************************************************************
 * TYPEDEFS
 */


/* There are four different types of command struct depending on ADV type.
   However, they are actually all the same. We can use a common struct.
*/
typedef rfc_CMD_BLE_ADV_t  rfc_CMD_BLE_ADV_COMMON_t;


/*****************************************************************************
 * FUNCTION PROTOTYPES
 */

/**
 * @brief  Initialize RF Interface for Micro BLE Stack
 *
 * @param  none
 *
 * @return SUCCESS - RF driver opened successfully
 *         INVALIDPARAMETER - Failed to open RF driver
 */
bStatus_t urfi_init(void);

/*********************************************************************
 * @fn      urfi_getTxPowerVal
 *
 * @brief   Get the value, corresponding with the given TX Power,
 *          to be used to setup the radio accordingly.
 *
 * @param   txPower - TX Power in dBm.
 *
 * @return  The register value correspondign with txPower, if found.
 *          UBLE_TX_POWER_INVALID otherwise.
 */
uint16 urfi_getTxPowerVal(int8 dBm);

/*********************************************************************
 *  EXTERNAL VARIABLES
 */

/* RF driver handle */
extern RF_Handle    urfiHandle;
#if defined(FEATURE_ADVERTISER)
/* ADV command handle */
extern RF_CmdHandle urfiAdvHandle;
/* ADV command parameter */
extern rfc_bleAdvPar_t urfiAdvParams;
#endif /* FEATURE_ADVERTISER */

#if defined(FEATURE_SCANNER)
/* Scan command handle */
extern RF_CmdHandle urfiScanHandle;
/* Scan command parameter */
extern rfc_bleScannerPar_t urfiScanParams;
#endif /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
/* Monitor command handle */
extern RF_CmdHandle urfiGenericRxHandle;
/* Monitor command parameter */
extern rfc_bleGenericRxPar_t urfiGenericRxParams;
#endif /* FEATURE_MONITOR */

#ifdef __cplusplus
}
#endif

#endif /* URFI_H */
