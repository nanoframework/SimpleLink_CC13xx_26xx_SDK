/******************************************************************************

 @file  ll_user_config.h

 @brief This file contains user configurable variables for the BLE
        Controller.

        To change the default value of configurable variable:
          - Include "ll_userConfig.h" in your OSAL_ICallBle.c file.
          - Set the variables at the start of stack_main. Actually,
            it is okay to set the variables anywhere in stack_main
            as long as it is BEFORE osal_init_system, but best to
            set at the very start of stack_main.

        Note: User configurable variables are only used during the
              initialization of the Controller. Changing the values
              of these variables after this will have no effect.

        For example:
          int stack_main()
          {
            // user reconfiguration of Controller variables
            llUserConfig.maxNumConns  = 1;
            llUserConfig.numTxEntries = 10;
                       :

        Default values:
          maxNumConns  : 3
          numTxEntries : 6

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: TISD 2014 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR.
 *
 */

#ifndef LL_USER_CONFIG_H
#define LL_USER_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */
#include "ble_user_config.h"

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

typedef struct
{
  uint8                 maxNumConns;           // Max number of BLE connections
  uint8                 numTxEntries;          // Max number of BLE connection Tx buffers
  uint8                 maxPduSize;            // Max PDU data size
  uint8                 rfFeModeBias;          // RF Front End Mode and Bias
  regOverride_t         *rfRegPtr;             // RF Common Override Registers
  regOverride_t         *rfReg1MPtr;           // RF 1M Override Register Table
  regOverride_t         *rfReg2MPtr;            // RF 2M Override Register Table
  regOverride_t         *rfRegCodedPtr;         // RF Coded Override Register Table
  txPwrTbl_t            *txPwrTblPtr;           // Tx Power Table
  rfDrvTblPtr_t         *rfDrvTblPtr;           // Table of Rf Driver API
  eccDrvTblPtr_t        *eccDrvTblPtr;          // Table of ECC Driver API
  cryptoDrvTblPtr_t     *cryptoDrvTblPtr;       // Table of Crypto Driver API
  trngDrvTblPtr_t       *trngDrvTblPtr;         // Table of TRNG Driver API
  rtosApiTblPtr_t       *rtosApiTblPtr;         // Table of RTOS API
  uint32                startupMarginUsecs;     // Startup Margin in us
  uint32                inactivityTimeout;      // Inactivity timeout in us
  uint32                powerUpDuration;        // Powerup time in us
  RF_Callback           *pErrCb;                // RF Driver Error Callback
  uint8                 maxWlElems;             // Max elements in the white list
  uint8                 maxRlElems;             // Max elements in the resolving list
  ECCParams_CurveParams *eccCurveParams;        // ECC curve parameters
  pfnFastStateUpdate_t  fastStateUpdateCb;      // Fast state update callback
  uint32                bleStackType;           // BLE Stack Type
  uint32                extStackSettings;       // BLE misc stack settings
#if defined(CC13X2P)
  txPwrBackoffTbl_t     *txPwrBackoffTblPtr;    // Tx Power Table
  regOverride_t         *rfRegOverrideTx20Ptr;  // High gain overrides
  regOverride_t         *rfRegOverrideTxStdPtr; // Default PA overrides
#endif //CC13X2P
  regOverride_t         *rfRegOverrideCtePtr;   // CTE overrides
  cteAntProp_t          *cteAntProp;            // CTE antenna properties
  uint8                 privOverrideOffset;    // Privacy Override Offset
  coexUseCaseConfig_t   *coexUseCaseConfig;     // CoEx priority and RX request configuration
  uint8                 maxNumCteBufs;         // num of CTE samples buffers (each ~2.5KB) used for RF auto copy
} llUserCfg_t;

/*******************************************************************************
 * LOCAL VARIABLES
 */


/*******************************************************************************
 * GLOBAL VARIABLES
 */

extern llUserCfg_t llUserConfig;
extern uint16      llUserConfig_maxPduSize;

#ifdef __cplusplus
}
#endif

#endif /* LL_USER_CONFIG_H */
