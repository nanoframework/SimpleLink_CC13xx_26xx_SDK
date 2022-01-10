/******************************************************************************

 @file fh_ie_patch.h

 @brief TIMAC 2.0 FH IE PATCH API

 Group: WCS LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2021, Texas Instruments Incorporated
 All rights reserved.

 IMPORTANT: Your use of this Software is limited to those specific rights
 granted under the terms of a software license agreement between the user
 who downloaded the software, his/her employer (which must be your employer)
 and Texas Instruments Incorporated (the "License"). You may not use this
 Software unless you agree to abide by the terms of the License. The License
 limits your use, and you acknowledge, that the Software may not be modified,
 copied or distributed unless embedded on a Texas Instruments microcontroller
 or used solely and exclusively in conjunction with a Texas Instruments radio
 frequency transceiver, which is integrated into your product. Other than for
 the foregoing purpose, you may not use, reproduce, copy, prepare derivative
 works of, modify, distribute, perform, display or sell this Software and/or
 its documentation for any purpose.

 YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
 PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
 NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
 TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
 NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
 LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
 INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
 OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
 OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

 Should you have any questions regarding your right to use this Software,
 contact Texas Instruments Incorporated at www.TI.com.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef _FHIE_PATCH_H_
#define _FHIE_PATCH_H_

/******************************************************************************
 Includes
 *****************************************************************************/

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/******************************************************************************
 Typedefs
 *****************************************************************************/

/******************************************************************************
 Global Externs
 *****************************************************************************/
#ifdef FEATURE_WISUN_SUPPORT
extern const FHPIB_DB_new_t FHPIB_defaults_new;
extern FHPIB_DB_new_t FHPIB_db_new;
#endif
/******************************************************************************
 Prototypes
 *****************************************************************************/

FHAPI_status FHIEPATCH_extractHie(uint8_t subIeId, uint16_t ieLength, uint8_t *pBuf, uint8_t *pOut);
FHAPI_status FHIEPATCH_extractPie(uint8_t subIeId, uint16_t ieLength, uint8_t *pBuf, uint8_t *pOut);
uint16_t FHIEPATCH_getHieLen(uint32_t ieBitmap);
uint8_t FHIEPATCH_getPieId(uint32_t ieBitmap, uint8_t *pType);
uint16_t FHIEPATCH_getPieContentLen(uint8_t id);
uint16_t FHIEPATCH_genPieContent(uint8_t *pData, uint8_t id);
#if defined(FEATURE_WISUN_EDFE_SUPPORT) || defined(FEATURE_WISUN_MIN_EDFE)
uint16_t FHIEPATCH_genHie(uint8_t *pData, uint32_t bitmap, macTx_t *pMacTx,
                            FHIE_ieInfo_new_t *pIeInfo);
#else
uint16_t FHIEPATCH_genHie(uint8_t *pData, uint32_t bitmap, macTx_t *pMacTx,
                            FHIE_ieInfo_t *pIeInfo);
#endif
FHAPI_status FHIEPATCH_parsePie(uint8_t subIeId, uint16_t subIeLength, uint8_t *pBuf, sAddrExt_t *pSrcAddr);
FHAPI_status FHIEPATCH_parseHie(uint8_t subIeId, uint16_t ieLength, uint8_t *pBuf, uint32_t ts, sAddrExt_t *pSrcAddr);

#if defined(TIMAC_ROM_PATCH)
MAC_INTERNAL_API void  FHAPI_sendData(void);
void FHDATA_startEDFEReq(void *pData);
void FHDATA_procEDFEReq(void *pData);
void FHDATA_compEDFEReq(uint8_t status);
uint16_t FHIE_getPieContentLen(uint8_t id);
uint16_t FHIE_getHieLen(uint32_t bitmap);
#ifdef FEATURE_WISUN_EDFE_SUPPORT
void FHIE_genUTIE(uint8_t *pBuf, uint8_t fhFrameType, uint32_t offset);
void FHIE_genBTIE(uint8_t *pBuf, uint8_t frameType, uint32_t offset);
#else
void FHIE_genUTIE(uint8_t *pBuf, uint8_t fhFrameType);
void FHIE_genBTIE(uint8_t *pBuf, uint8_t frameType);
#endif
uint16_t FHIE_genPieContent(uint8_t *pData, uint8_t id);

#if defined(FEATURE_WISUN_EDFE_SUPPORT) || defined(FEATURE_WISUN_MIN_EDFE)
uint16_t FHIE_genHie(uint8_t *pData, uint32_t bitmap, macTx_t *pMacTx,
                            FHIE_ieInfo_new_t *pIeInfo);
uint16_t FHIE_genPie(uint8_t *pData, uint32_t bitmap, macTx_t *pMacTx, FHIE_ieInfo_new_t *pIeInfo);
MAC_INTERNAL_API uint16_t FHIE_gen(uint8_t *pData, uint32_t bitmap,
                                   macTx_t *pMacTx, FHIE_ieInfo_new_t *pIeInfo);
#else
uint16_t FHIE_genHie(uint8_t *pData, uint32_t bitmap, macTx_t *pMacTx,
                            FHIE_ieInfo_t *pIeInfo);
uint16_t FHIE_genPie(uint8_t *pData, uint32_t bitmap, macTx_t *pMacTx, FHIE_ieInfo_t *pIeInfo);
MAC_INTERNAL_API uint16_t FHIE_gen(uint8_t *pData, uint32_t bitmap,
                                   macTx_t *pMacTx, FHIE_ieInfo_t *pIeInfo);
#endif
MAC_INTERNAL_API FHAPI_status FHIE_parseHie(uint8_t *pData,
                                            sAddrExt_t *pSrcAddr,
                                            uint32_t ts,
                                            uint16_t *pLen1,
                                            uint16_t *pLen2);
MAC_INTERNAL_API FHAPI_status FHIE_extractPie(uint8_t *pData,
                                              IE_TYPE_t ieType,
                                              uint8_t *pOut);
MAC_INTERNAL_API FHAPI_status FHIE_extractHie(uint8_t *pData,
                                              IE_TYPE_t ieType,
                                              uint8_t *pOut);
uint8_t FHUTIl_updateBTIE(uint32_t bfio, uint16_t slotIdx);
MAC_INTERNAL_API void FHAPI_completeTxCb(uint8_t status);
void FHMGR_bcTimerIsrCb(uint8_t parameter);
void FHMGR_updateRadioUCChannel(void *pData);

MAC_INTERNAL_API void FHPIB_reset(void);

extern uint16_t FHIE_getCommonInformationLen(uint8_t cp, uint8_t cf,
                                             uint8_t ecc,
                                             FHIE_channelPlan_t *pChPlan,
                                             uint8_t ucFlag);
extern uint16_t FHIE_genCommonInformation(uint8_t *pData, uint8_t cp, uint8_t cf, uint8_t ecc,
                                          FHIE_channelPlan_t *pChPlan, uint8_t ucFlag);
extern uint32 FHIE_getCcaSfdTime(uint8 fhFrameType);
extern uint8_t FHIE_getChannelPlan(FHIE_channelPlan_t *pChPlan);
extern uint8_t FHIE_getExcludedChannelControl(uint8_t id);
extern uint16_t FHIE_getSubPie(uint8_t *pData, FHIE_ie_t *subPie);
uint16_t FHIE_getHie(uint8_t *pData, FHIE_ie_t *hie);
extern uint16_t FHIE_parseCommonInformation(uint8_t *pData, uint8_t chinfo,
                                            usieParams_t *pUsIeParams,
                                            uint8_t ucFlag);
extern uint32_t FHUTIL_compBfioTxTime(void);
extern uint16_t FHIE_getPie(uint8_t *pData, FHIE_ie_t *pie);
#endif

#endif
