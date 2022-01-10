/******************************************************************************

 @file fh_data.h

 @brief TIMAC 2.0 FH module data API

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

#ifndef FH_DATA_H
#define FH_DATA_H

/******************************************************************************
 Includes  
 *****************************************************************************/
#include "hal_types.h"
#include "fh_pib.h"
#include "mac_low_level.h"

#if defined(TIMAC_ROM_IMAGE_BUILD)
#include "icall_osal_rom_jt.h"
#else
#include "icall_osal_map_direct.h"
#endif

/******************************************************************************
 Defines
 *****************************************************************************/

//#define MAC_FRAME_TYPE_UNICAST            (12)        /*!< FH Unicast data type  */
//#define MAC_FRAME_TYPE_BROADCAST          (13)        /*!< FH broadcast data type  */
//#define MAC_FRAME_TYPE_ASYNC              (14)        /*!< FH ASYNC data type  */
//#define MAC_FRAME_TYPE_EDFE               (15)        /*!< FHEDFE data type  */

#define MAC_FRAME_TYPE_UNICAST            (0x15)        /*!< FH Unicast data type  */
#define MAC_FRAME_TYPE_BROADCAST          (0x25)        /*!< FH broadcast data type  */
#define MAC_FRAME_TYPE_ASYNC              (0x35)        /*!< FH ASYNC data type  */
#define MAC_FRAME_TYPE_EDFE               (0x45)        /*!< FH EDFE data type  */
#define MAC_FRAME_TYPE_EDFE_IFRM          (0x46)        /*!< FH EDFE data type  */
#define MAC_FRAME_TYPE_EDFE_RFRM          (0x47)        /*!< FH EDFE data type  */
#define MAC_FRAME_TYPE_EDFE_FFRM          (0x48)        /*!< FH EDFE data type  */
#define MAC_FRAME_TYPE_EDFE_IDFRM         (0x49)        /*!< FH EDFE data type  */
#define MAC_FRAME_TYPE_EDFE_RDFRM         (0x4a)        /*!< FH EDFE data type  */
#define MAC_UNIT_BACKOFF_TIME             (1160)        /*!< FHEDFE back of time uint in us  */

#define FH_USCH_NUM_SLOTS                 (65536)
//units in us
#define FH_PHY_SFD_TIME                   (3000)
#define FH_PHY_CCA_TIME                   (160)
#define FH_PHY_RADIO_CH_TUNE_DELAY        (200)
#define FH_PHY_RADIO_FIXED_DELAY          (FH_PHY_CCA_TIME + FH_PHY_RADIO_CH_TUNE_DELAY)
#define FH_SLOT_EDGE_PROTECTION_TIME      (FH_PHY_SFD_TIME + FH_PHY_CCA_TIME + FH_PHY_RADIO_CH_TUNE_DELAY + 1000)
#define FH_SLOT_EDGE_PROTECTION_TIME_MS   ((FH_SLOT_EDGE_PROTECTION_TIME + 500)/1000)
#define FH_SLOT_ERR_EST_TIME              (2000)
#define FH_SLOT_ERR_EST_TIME_MS           (2)
/* need to add CCA time in RTA resolution
 *  0.1 ms == 400 RTA
 *  SFD = 1.6 ms ==> 6400
 */
//#define FH_PHY_SFD_TIME_MS in RAT ticks
#define FH_PHY_RADIO_DELAY                (4000)
#define FH_PHY_SFD_TIME_MS                (6400)
//200Kbps:
//Preamble = 80 bits(56 preamble + 24 sync) = 400us
//CCA = 150us
#define FH_PHY_SFD_TIME_200k_MS           (1600)
#define FH_LRM_PHY_SFD_TIME_MS            (38400)
#define FH_LRM_SLOT_EDGE_PROTECTION_TIME  (13000)
#define FH_LRM_SLOT_EDGE_PROTECTION_TIME_MS  (13)
/*temp def. till LMAC timer def is avilable */
#define FH_LBT_ENABLE_FLAG                  (1)
#define FH_LBT_DISABLE_FLAG                 (0)
#define FH_SUPPORTED_FRAME_TYPES          (4)
/* FH supported data type */
#define FH_SUUPORTED_PACKET_FRAMES                          \
        {MAC_FRAME_TYPE_ASYNC,                              \
         MAC_FRAME_TYPE_EDFE,                               \
         MAC_FRAME_TYPE_BROADCAST,                          \
         MAC_FRAME_TYPE_UNICAST}
/*!
 FH Timer - FH timer object to handle UC/BC timeout
 */
typedef struct FH_macFHTimer_s
{
  struct FH_macFHTimer_s   *pNext;                       /*!< next timer in queue */
  uint32_t                  duration;                     /*!< timer duration in us */
  uint32_t                  prevTimerDuration;            /*!< keep last timer duration history in us */
  void                     (*pFunc)(uint8_t parameter);  /*!< user defined timer callback function */
  uint32_t                 clockTickCnt;                 /*!< clock tick count */
  uint32_t                 timerId;                      /*!< timerId returned by iCall*/
  uint8_t                  parameter;                    /*!< callback function parameter */
  uint8_t                  compTime;                     /*!< callback function parameter */
} FH_macFHTimer_t;

/*!
 * @brief
   FH Handle - used to store the FH module internal data structure
 */
typedef struct fh_hnd_s
{
  uint8_t                  pendUcChHop;           /*<! UC channel hopping (change channel) pending flag */
  uint8_t                  pendBcChHop;           /*<! BC channel hopping (change channel) pending flag */
  uint16_t                 ucSlotIdx;             /*<! UC slot index */
  uint16_t                 bcSlotIdx;             /*<! BC slot index */
  uint32_t                 ufsi;                  /*<! ?? */
  uint32_t                 bfio;                  /*<! ?? */
  uint16_t                 rxSlotIdx;             /*<! rx slot index */
  uint32_t                 btie;                  /*<! extracted BTIE value */
  uint32_t                 btieTs;                /*<! BTIE TS */
  uint32_t                 rxSfdTs;               /*<! RX SFD time stamp */
  uint8_t                  bcDwellActive;         /*<! BC dwell active flag */
  uint8_t                  bcPktPending;          /*<! BC packet pending flag */
  uint8_t                  pktPending;            /*<! packet pending flag */
  uint8_t                  asyncStop;             /*<! ASYNC stop flag */
  uint8_t                  bsStarted;             /*<! Broadcast started flag */
  uint8_t                  btiePresent;           /*<! BT IE is present flag */
  uint8_t                  ucNumChannels;         /*<! total number of UC channel */
  uint8_t                  bcNumChannels;         /*<! total number of BC channel */
  FH_macFHTimer_t          ucTimer;               /*<! unicast timer */
  FH_macFHTimer_t          bcTimer;               /*<! broadcast timer */
  FH_macFHTimer_t          minTxOffTimer;         /*<! min TX off  timer */
  uint16_t                 lastChannel;           /*<! last setup channel */
  uint8_t                  asyncChIdx;            /*<! ASYNC channel index */
  uint8_t                  regSFDFlag;            /*<! SFD registration flag */

  uint8_t                  LBT;                   /*<! LBT feature */
  uint8_t                  lastTxChannel;         /*<! last TX channel */
  uint16_t                 ccaTime;               /*<! Radio CCA time */
  uint8_t                  fhStarted;             /*<! FH module is started */
  uint8_t                  edfePending;           /*<! EDFE packet pending flag */
#ifdef FEATURE_WISUN_SUPPORT
  uint8_t                   fhBSRcvd;             /*<! Valid FH BS Received */
#endif
} FH_HND_s;

/* FH module debug count */
/*!
 FH debug - store the FH module debug counts
 */
#ifdef FH_HOP_DEBUG
typedef struct fh_debug_s
{
    uint32_t                numRxSFD_detected;                      /*<! number SFD is detected */
    uint32_t                numRxSFD_detected_not_in_hop;           /*<! number SFD is detected */
    uint32_t                numRxSFD_detected_in_tx;                /*<! number SFD is detected in TX state*/
    uint32_t                numRxSFD_received;                      /*<! number of RX done from SFD callback */
    uint32_t                numRxSFD_unknown;                       /*<! number of LMAC in unknown state */
    uint32_t                numRxDone;                              /*<! number of LMAC RX done callback */
    uint32_t                numRxDoneBroadcast;                     /*<! number of LMAC RX broadcast packet */
    uint32_t                numRxLMACErr;                           /*<! number of LMAC RX error */
    uint32_t                numRxFhFrameType[8];                    /*<! number of RX data in fhType */
    uint16_t                errRxUTIE;                              /*<! number of UT IE in RX data */
    uint16_t                err_HIEPtr;                             /*<! number of HIE pointer error */
    uint16_t                err_PIEPtr;                             /*<! number of PIE pointer error */

    uint32_t                numTxDataSend;                          /*<! number of TX data packet are sent */
    uint16_t                numTxInProgess;                         /*<! number of TX in progress when calling FHAPI_senddata */
    uint16_t                numTxQueueEmpty;                        /*<! number of TX queue is empty when calling FHAPI_sendata  */
    uint16_t                numTxinRxMode;                          /*<! number of time FH in RX mode when calling FHAPI_senddata */
    uint16_t                numTxBCinEDFE;                          /*<! number of time FH in EDFE mode when calling FHAPI_senddata */
    uint16_t                numTxUCinEDFE;                          /*<! number of time FH in EDFE mode when calling FHAPI_senddata */
    uint16_t                numTxCnf;                               /*<! number of TX confirm message */
    uint16_t                numTxMacTxOk  ;                         /*<! number of MAC TX OK */
    uint16_t                numTxMacNoTime;                         /*<! number of MAC TX NO TIME */
    uint16_t                numTxMacNoTimeQueFull;                  /*<! number of MAC TX NO TIME, queue is full*/
    uint16_t                numTxMacNoACK;                          /*<! number of MAC TX NO ACK */
    uint16_t                numTxMacCCAFail;                        /*<! number of MAC TX CCA Fail */
    uint16_t                numTxMacAbort;                          /*<! number of MAC TX Abort */
    uint16_t                numTxMacNoResource;                     /*<! number of MAC TX No Resource */
    uint16_t                numTxMacOther;                          /*<! number of MAC TX other error */
    uint16_t                numTxCCAbusy;                           /*<! number of TX CCA busy */
    uint16_t                numTxRssiBusy;                          /*<! number of TX RSSI busy */
    uint16_t                numTxPushToQue;                         /*<! number of TX push to queue (in RX) */
    uint16_t                numAsyncStartReq;                       /*<! number of TX ASYNC request messages */
    uint16_t                numBroadcastStart;                      /*<! number of TX broadcast packets */
    uint16_t                numBroadcastDrop ;                      /*<! number of TX broadcast packets dropped */
    uint16_t                numUnicastStart;                        /*<! number of TX unicast packets */
    uint16_t                numUnicast_entryNotInNT_1;              /*<! number of destination address is not in NT case 1 */
    uint16_t                numUnicast_entryNotInNT_2;              /*<! number of destination address is not in NT case 2 */
    uint16_t                numAsyncStopReq;                        /*<! number of ASYNC stop request */
    uint16_t                numPurge;                               /*<! number of ASYNC packets are purged */
    uint16_t                numUnSupportedTypes;                    /*<! number of unsupported frames purged */
    uint16_t                numTxAsyncDelay;                        /*<! number ofTX ASYNC delay */
    uint16_t                numTxAsyncCCAError;                     /*<! number ofTX ASYNC CCA error */
    uint16_t                numTxMinOffDelay;                       /*<! number ofTX minimum off delay */
    uint16_t                numRxEDFEinDFE;                         /*<! number of RX EDFE while in DFE */
    uint16_t                numEdfeStart;                           /*<! number of edfe packets */
#ifdef FEATURE_WISUN_SUPPORT
    uint16_t                numFramecounterErr;                     /*<! number of frame counter error */
    uint16_t                numKeyRetrievalErr;                     /*<! number of key retrival error */
    uint16_t                numDecryptionErr;                       /*<! number of decryption errors */
    uint16_t                numEDFEFrRcvd;                          /*<! number of EDFE Frame Rcvd */
    uint16_t                numEDFEFinFrSent;                       /*<! number of EDFE FIN Frame Sent */
#endif
} FH_DEBUG_s;
extern FH_DEBUG_s FH_Dbg;
#endif

#define FH_HALF_MS                          (2000)
#define FH_DEFAULT_CCA_TIME                 (1000)                  /*<! corresponding to rxBw of 35 plus some radio delay */
#define FH_LRM_DEFAULT_CCA_TIME             (1600)                  /*<! corresponding to rxBw of 33 plus some radio delay */
#define FH_LBT_CCA_TIME                     (20000)                 /*<! corresponding to 5 ms */
#define FH_TX_TIMING_TABLE_SIZE             (10)

#define FH_LBT_MIN_TX_OFF_TIME              (100)               /*<! 100 ms */
/* FH module TX done timing info */
/*!
 FH tx done timing info - store the TX done timing and channel info
 */
typedef struct __attribute__((__packed__))  fh_tx_timing_s
{
    uint8_t     txChannel;          /*<! tx channel */
    uint32_t    txTimeStamp;        /*<! tx done time stamp */
}FH_TX_TIMING_s;

/*! TX timing Table */
typedef struct fh_tx_timing_table
{
   uint16_t         wrIndex;
   uint16_t         maxNumEntry;
   FH_TX_TIMING_s   *pTxPkt;

} FH_TX_TIMING_TABLE_s;

/* ------------------------------------------------------------------------------------------------
 *                                           Global Externs
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                           Prototypes
 * ------------------------------------------------------------------------------------------------
 */


MAP_osal_msg_q_t FHDATA_checkPktFromQueue(MAP_osal_msg_q_t *txQueue, uint8_t frameType);

MAP_osal_msg_q_t FHDATA_getPktFromQueue(MAP_osal_msg_q_t *txQueue, uint8_t frameType);

void FHDATA_procASYNCReq(void *pData);

void FHDATA_startEDFEReq(void *pData);

void FHDATA_procEDFEReq(void *pData);

void FHDATA_startEDFETimer(void *pData);

void FHDATA_compEDFEReq(uint8_t status);

FHAPI_status FHDATA_purgePktFromQueue(MAP_osal_msg_q_t *txQueue, uint8_t frameType);

void FHDATA_purgeUnSupportedFrameTypes(MAP_osal_msg_q_t *txQueue);
void FHDATA_requeuePkt(void *pData);

/*!
 * @brief       This function is used to update the FH CCA timing. For FFC band, its value is
 *              fixe. When LBT mode is enabled (ETSI), it will compute the CCA time based on
 *              packet type (ASYNC, Data and ACK).
 *
 * @param macFHTxType - FH TX packet type
 */
void FHDATA_updateCCA(uint32_t ccaTime);

/*!
 * @brief       This function is used to check if the min TX off time is met in LBT mode.
 *              when LBT mode is disabled (in FCC band), it also assumes the minimum TX off time is met.
 *
 * @param txChan - FH TX channel number
 */

uint32_t FHDATA_CheckMinTxOffTime(uint8_t txChan,uint32_t csmaDelay);

/*!
 * @brief       This function is used to check if the min TX off time for very first ASYNC packet
 *              It is only used in enabled LBT mode
 *
 */
uint32_t FHDATA_getASYNCDelay(void);

/*!
 * @brief       This function is timeout callback function for ASYNC minimum TX delay
 *
 */

void FHDATA_asyncMinTxOffTimerCb(uint8_t parameter);

void FHDATA_chkASYNCStartReq(void);
#endif
