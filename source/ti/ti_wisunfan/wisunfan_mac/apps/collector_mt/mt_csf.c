/******************************************************************************

 @file mt_csf.h

 @brief TIMAC 2.0 Collector MT Interface

 Group: LPRF
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2021, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/******************************************************************************
 Includes
 *****************************************************************************/
#include <string.h>
#include <stdint.h>

#ifdef OSAL_PORT2TIRTOS
#include "macTask.h"
#include"rom_jt_154.h"
#else
#include "api_mac.h"
#include "icall.h"
#endif

#include "mt.h"

#include <ti/drivers/apps/Button.h>

#include "npi_task.h"
#include "mac_util.h"
#include "llc.h"
#include "csf.h"
#include "collector.h"
#include "cllc.h"
#include "ti_drivers_config.h"

#include "mt_csf.h"
/* use macNpiMTMsg_t */
#include "mac_api.h"
/******************************************************************************
 Constants and definitions
 *****************************************************************************/
/* MT Version Information */
#if !defined(MT_VER_PROTO)
#define MT_VER_PROTO 2
#endif
#if !defined(MT_VER_PROID)
#define MT_VER_PROID 0
#endif
#if !defined(MT_VER_MAJOR)
#define MT_VER_MAJOR 2
#endif
#if !defined(MT_VER_MINOR)
#define MT_VER_MINOR 0
#endif
#if !defined(MT_VER_MAINT)
#define MT_VER_MAINT 0
#endif

#define MT_SYS_SYSID   1
#define MT_CSF_SYSID   9    /*!< re-using the MT_RPC_SYS_APP SubSystem ID */

/*! Ping SREQ/SRSP message ID */
#define MT_SYS_CMDID_RESET                         0x00
#define MT_SYS_CMDID_PING                          0x01
#define MT_SYS_CMDID_VERSION                       0x02
#define MT_SYS_CMDID_RESET_IND                     0x80

/* MT_CAP_APP is defined in mt.h, use the local copy */
#undef MT_CAP_APP
#define MT_CAP_APP   0x0101

/* 1st byte is length of the data field, 2nd/3rd bytes are command fields */
#define RPC_FRAME_HDR_SZ   3

#define RPC_POS_LEN     0
#define RPC_POS_CMD0    1
#define RPC_POS_CMD1    2
#define RPC_POS_DATA0   3

#define RPC_CMD_SREQ    0x20
#define RPC_CMD_AREQ    0x40
#define RPC_CMD_SRSP    0x60

#define RPC_TYPE_MASK       0xE0
#define RPC_SUBSYSTEM_MASK  0x1F

#define RPC_STAT_SUCCESS         0x00     /* success */
#define RPC_STAT_ERR_SUBSYSTEM   0x01     /* invalid subsystem */
#define RPC_STAT_ERR_COMMAND_ID  0x02     /* invalid command ID */
#define RPC_STAT_ERR_PARAMETER   0x03     /* invalid parameter */
#define RPC_STAT_ERR_LENGTH      0x04     /* invalid length */
#define RPC_STAT_ERR_NOMEMORY    0x10     /* memory allocation failed */

#define CSF_STAT_SUCCESS 0
#define CSF_STAT_FAIL 1

#define CONFIG_REQ_LEN 12
#define TOGGLE_REQ_LEN 2

#define GET_STATS_RSP_LEN 65
#define BASIC_NET_INFO_LEN 14
#define DEVICE_INFO_RSP_LEN 22
#define DEVICE_INFO_IND_LEN 17
#define DEVICE_NOT_ACTIVE_IND_LEN 13
#define CONFIG_RSP_IND_LEN 15
#define BASIC_SENSOR_LEN 13
#define TOGGLE_RSP_IND_LEN 3

typedef struct
{
    uint16_t len;
    uint8_t type;
    uint8_t subSystemId;
    uint8_t cmdId;
    uint8_t *pData;
} msgHdr_t;

/******************************************************************************
 External variables
 *****************************************************************************/
#ifdef OSAL_PORT2TIRTOS
extern uint8_t appTaskId;
extern uint8_t npiAppEntityID;
#else
/* ICall thread entity for ApiMac */
extern ICall_EntityID ApiMac_appEntity;

/* ICall thread entity for NPI */
extern ICall_EntityID npiAppEntityID;
#endif

/* Key press parameters */
extern Button_Handle Csf_keys;

/* pending Csf_events */
extern uint16_t Csf_events;

/******************************************************************************
 Local variables
 *****************************************************************************/
/* Number of requests sent to MAC */
static uint16_t numRxMsgs = 0;

/* Number of requests sent to MAC */
static uint16_t numNPIRxMsgs = 0;

/* Number of responses sent to host */
static uint16_t numTxMsgs = 0;

static const uint8_t mtVersion[5] =
    {
    MT_VER_PROTO, /* Transport protocol */
      MT_VER_PROID, /* Product ID */
      MT_VER_MAJOR, /* Software major release number */
      MT_VER_MINOR, /* Software minor release number */
      MT_VER_MAINT /* Software maintenance release number */
    };
/* Copy of the left Button Handle for start indications */
static Button_Handle gleftBtnHandle = NULL;

/******************************************************************************
 Local function prototypes
 *****************************************************************************/

static void sendResetIndication(uint8_t reason);
static uint8_t parseHeader(msgHdr_t *pHdr, uint8_t *pBuf);
static void processPermitJoin(msgHdr_t *pHdr);
static void processGetStats(msgHdr_t *pHdr);
static void sendNetworkInfo( bool ind, uint8_t cmdId);
static void fillInDevInfo(Llc_deviceListItem_t *pItem, uint8_t *pBuf);
static void processDeviceInfo(msgHdr_t *pHdr);
static void processConfigRequest(msgHdr_t *pHdr);
static void processToggleRequest(msgHdr_t *pHdr);
static void sendNPI(uint8_t cmdType, uint8_t cmdId, uint8_t dataLen,
                    uint8_t *pData);
static void sendSRSP(uint8_t cmdId, uint8_t dataLen, uint8_t *pData);
static void sendAREQ(uint8_t cmdId, uint8_t dataLen, uint8_t *pData);
static void processMsg(uint16_t entityID, uint16_t param2, void *pMsg);
static void relayRxMsg(void *pMsg);
static void relayTxMsg(void *pMsg);

/******************************************************************************
 Public functions
 *****************************************************************************/

/*!
 Initialize the MTCSF system for use with NPI system

 Public function defined in mt_csf.h
 */
void MTCSF_init(uint8_t resetReason, Button_Handle leftBtnHandle)
{
    // Setup ApiMac callback to handle unprocessed messages */
    Collector_macCallbacks.pUnprocessedCb = processMsg;

    // Save the leftBtnHandle for start indications
    gleftBtnHandle = leftBtnHandle;

    /* Register callback function for incoming NPI messages */
    NPITask_registerIncomingRXEventAppCB((npiIncomingEventCBack_t)relayRxMsg,
                                         INTERCEPT);
    /* Register callback function for outgoing NPI messages */
    NPITask_registerIncomingTXEventAppCB((npiIncomingEventCBack_t)relayTxMsg,
                                         ECHO);

#ifdef OSAL_PORT2TIRTOS
    /* Configure and create the NPI task */
    NPITask_createTask(macTaskId);
#else
    /* Configure and create the NPI task */
    NPITask_createTask(0);
#endif
    /* Send the reset indication */
    sendResetIndication(resetReason);
}

/*!
 Process incoming buffer.

 Public function defined in mt_csf.h
 */
uint8_t MTCSF_processMsg(uint8_t *pBuf)
{
    uint8_t status;
    msgHdr_t hdr;

    status = parseHeader(&hdr, pBuf);

    if(status == RPC_STAT_SUCCESS)
    {
        if(hdr.subSystemId == MT_CSF_SYSID)
        {
            switch(hdr.cmdId)
            {
                case MT_CSF_CMDID_START:
#if !defined(AUTO_START)
                    /* Tell the collector to start */
                    Csf_events |= CSF_KEY_EVENT;
                    Csf_keys = gleftBtnHandle;
                    status = CSF_STAT_SUCCESS;
#else
                    status = CSF_STAT_FAIL;
#endif
                    /* Build and send back the response */
                    sendSRSP(hdr.cmdId, 1, &status);
                    break;

                case MT_CSF_CMDID_PERMIT_JOIN:
                    processPermitJoin(&hdr);
                    break;

                case MT_CSF_CMDID_GET_STATS:
                    processGetStats(&hdr);
                    break;

                case MT_CSF_CMDID_GET_NETWORK_INFO:
                    sendNetworkInfo(false, hdr.cmdId);
                    break;

                case MT_CSF_CMDID_GET_DEVICE_INFO:
                    processDeviceInfo(&hdr);
                    break;

                case MT_CSF_CMDID_SEND_CONFIG:
                    processConfigRequest(&hdr);
                    break;

                case MT_CSF_CMDID_SEND_TOGGLE:
                    processToggleRequest(&hdr);
                    break;
            }
        }
        else if(hdr.subSystemId == MT_SYS_SYSID)
        {
            /* Support some MT SYS commands just to get ZTool to work */

            if(hdr.cmdId == MT_SYS_CMDID_PING)
            {
                uint8_t retArray[2];

                retArray[0] = Util_loUint16(MT_CAP_APP);
                retArray[1] = Util_hiUint16(MT_CAP_APP);

                /* Build and send back the response */
                sendNPI(((uint8_t)RPC_CMD_SRSP | (uint8_t)MT_SYS_SYSID),
                MT_SYS_CMDID_PING,
                        2, retArray);
            }
            else if(hdr.cmdId == MT_SYS_CMDID_VERSION)
            {
                /* Send Version String */
                sendNPI(((uint8_t)RPC_CMD_SRSP | (uint8_t)MT_SYS_SYSID),
                MT_SYS_CMDID_VERSION,
                        sizeof(mtVersion), (uint8_t *)mtVersion);
            }
        }
    }

    return (status);
}

/*!
 Display the MT CSF statistics.

 Public function defined in mt_csf.h
 */
void MTCSF_displayStatistics(void)
{
#if defined(TI_DRIVERS_LCD_INCLUDED)
    LCD_WRITE_STRING_VALUE("MT TX Msgs:", numTxMsgs, 10, 7);
    LCD_WRITE_STRING_VALUE("MT NPIRx Msgs:", numNPIRxMsgs, 10, 5);
    LCD_WRITE_STRING_VALUE("MT RX Msgs:", numRxMsgs, 10, 6);
#endif
}

/*!
 Generate a Network Update Indication.

 Public function defined in mt_csf.h
 */
void MTCSF_networkUpdateIndCB(void)
{
    sendNetworkInfo(true, MT_CSF_CMDID_NETWORK_INFO_IND);
}

/*!
 Generate a Device Update Indication.

 Public function defined in mt_csf.h
 */
void MTCSF_deviceUpdateIndCB(ApiMac_deviceDescriptor_t *pDevInfo,
                             ApiMac_capabilityInfo_t *pCapInfo)
{
	Llc_deviceListItem_t item;
    uint8_t respBuf[DEVICE_INFO_IND_LEN];

    memcpy(&(item.devInfo), pDevInfo, sizeof(ApiMac_deviceDescriptor_t));
    memcpy(&(item.capInfo), pCapInfo, sizeof(ApiMac_capabilityInfo_t));
    item.rxFrameCounter = 0;

    fillInDevInfo(&item, respBuf);

    /* Send back the response */
    sendAREQ(MT_CSF_CMDID_DEVICE_UPDATE_IND, DEVICE_INFO_IND_LEN, respBuf);
}

/*!
 Generate the Device Not Active Indication.

 Public function defined in mt_csf.h
 */
void MTCSF_deviceNotActiveIndCB(ApiMac_deviceDescriptor_t *pDevInfo,
                                bool timeout)
{
    uint8_t respBuf[DEVICE_NOT_ACTIVE_IND_LEN];
    uint8_t *pBuf = respBuf;

    *pBuf++ = Util_loUint16(pDevInfo->panID);
    *pBuf++ = Util_hiUint16(pDevInfo->panID);
    *pBuf++ = Util_loUint16(pDevInfo->shortAddress);
    *pBuf++ = Util_hiUint16(pDevInfo->shortAddress);
    memcpy(pBuf, pDevInfo->extAddress, APIMAC_SADDR_EXT_LEN);
    pBuf += APIMAC_SADDR_EXT_LEN;
    *pBuf = timeout;

    /* Send back the response */
    sendAREQ(MT_CSF_CMDID_DEVICE_NOT_ACTIVE_IND, DEVICE_NOT_ACTIVE_IND_LEN,
             respBuf);
}

/*!
 Generate the Config Response Indication.

 Public function defined in mt_csf.h
 */
void MTCSF_configResponseIndCB(ApiMac_sAddr_t *pSrcAddr, int8_t rssi,
                               Smsgs_configRspMsg_t *pMsg)
{
    uint8_t respBuf[CONFIG_RSP_IND_LEN];
    uint8_t *pBuf = respBuf;

    *pBuf++ = Util_loUint16(pSrcAddr->addr.shortAddr);
    *pBuf++ = Util_hiUint16(pSrcAddr->addr.shortAddr);
    *pBuf++ = rssi;
    *pBuf++ = Util_loUint16(pMsg->status);
    *pBuf++ = Util_hiUint16(pMsg->status);
    *pBuf++ = Util_loUint16(pMsg->frameControl);
    *pBuf++ = Util_hiUint16(pMsg->frameControl);
    *pBuf++ = Util_breakUint32(pMsg->reportingInterval, 0);
    *pBuf++ = Util_breakUint32(pMsg->reportingInterval, 1);
    *pBuf++ = Util_breakUint32(pMsg->reportingInterval, 2);
    *pBuf++ = Util_breakUint32(pMsg->reportingInterval, 3);
    *pBuf++ = Util_breakUint32(pMsg->pollingInterval, 0);
    *pBuf++ = Util_breakUint32(pMsg->pollingInterval, 1);
    *pBuf++ = Util_breakUint32(pMsg->pollingInterval, 2);
    *pBuf = Util_breakUint32(pMsg->pollingInterval, 3);

    /* Send back the response */
    sendAREQ(MT_CSF_CMDID_DEVICE_CONFIG_UPDATE_IND, CONFIG_RSP_IND_LEN,
             respBuf);
}

/*!
 Generate the Device Toggle Indication.

 Public function defined in mt_csf.h
 */
void MTCSF_deviceToggleIndCB(uint16_t shortAddr, bool ledState)
{
    uint8_t respBuf[TOGGLE_RSP_IND_LEN];
    uint8_t *pBuf = respBuf;

    *pBuf++ = Util_loUint16(shortAddr);
    *pBuf++ = Util_hiUint16(shortAddr);
    *pBuf = ledState;

    /* Send back the response */
    sendAREQ(MT_CSF_CMDID_DEVICE_TOGGLE_IND, TOGGLE_RSP_IND_LEN,
             respBuf);
}

/*!
 Generate the Sensor Update Indication.

 Public function defined in mt_csf.h
 */
void MTCSF_sensorUpdateIndCB(ApiMac_sAddr_t *pSrcAddr, int8_t rssi,
                             Smsgs_sensorMsg_t *pMsg)
{
    uint8_t *pRespBuf;
    uint16_t len = BASIC_SENSOR_LEN;

    /* Figure out the length */
    if(pMsg->frameControl & Smsgs_dataFields_tempSensor)
    {
        len += SMSGS_SENSOR_TEMP_LEN;
    }
    if(pMsg->frameControl & Smsgs_dataFields_lightSensor)
    {
        len += SMSGS_SENSOR_LIGHT_LEN;
    }
    if(pMsg->frameControl & Smsgs_dataFields_humiditySensor)
    {
        len += SMSGS_SENSOR_HUMIDITY_LEN;
    }
    if(pMsg->frameControl & Smsgs_dataFields_msgStats)
    {
        len += sizeof(Smsgs_msgStatsField_t);
    }
    if(pMsg->frameControl & Smsgs_dataFields_configSettings)
    {
        len += SMSGS_SENSOR_CONFIG_SETTINGS_LEN;
    }

    pRespBuf = (uint8_t *)MAP_ICall_malloc(len);
    if(pRespBuf)
    {
        uint8_t *pBuf = pRespBuf;

        *pBuf++ = Util_loUint16(pSrcAddr->addr.shortAddr);
        *pBuf++ = Util_hiUint16(pSrcAddr->addr.shortAddr);
        *pBuf++ = rssi;
        memcpy(pBuf, pMsg->extAddress, SMGS_SENSOR_EXTADDR_LEN);
        pBuf += SMGS_SENSOR_EXTADDR_LEN;
        *pBuf++ = Util_loUint16(pMsg->frameControl);
        *pBuf++ = Util_hiUint16(pMsg->frameControl);

        if(pMsg->frameControl & Smsgs_dataFields_tempSensor)
        {
            *pBuf++ = Util_loUint16(pMsg->tempSensor.ambienceTemp);
            *pBuf++ = Util_hiUint16(pMsg->tempSensor.ambienceTemp);
            *pBuf++ = Util_loUint16(pMsg->tempSensor.objectTemp);
            *pBuf++ = Util_hiUint16(pMsg->tempSensor.objectTemp);
        }
        if(pMsg->frameControl & Smsgs_dataFields_lightSensor)
        {
            *pBuf++ = Util_loUint16(pMsg->lightSensor.rawData);
            *pBuf++ = Util_hiUint16(pMsg->lightSensor.rawData);
        }
        if(pMsg->frameControl & Smsgs_dataFields_humiditySensor)
        {
            *pBuf++ = Util_loUint16(pMsg->humiditySensor.temp);
            *pBuf++ = Util_hiUint16(pMsg->humiditySensor.temp);
            *pBuf++ = Util_loUint16(pMsg->humiditySensor.humidity);
            *pBuf++ = Util_hiUint16(pMsg->humiditySensor.humidity);
        }
        if(pMsg->frameControl & Smsgs_dataFields_msgStats)
        {
            *pBuf++ = Util_loUint16(pMsg->msgStats.joinAttempts);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.joinAttempts);
            *pBuf++ = Util_loUint16(pMsg->msgStats.joinFails);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.joinFails);
            *pBuf++ = Util_loUint16(pMsg->msgStats.msgsAttempted);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.msgsAttempted);
            *pBuf++ = Util_loUint16(pMsg->msgStats.msgsSent);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.msgsSent);
            *pBuf++ = Util_loUint16(pMsg->msgStats.trackingRequests);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.trackingRequests);
            *pBuf++ = Util_loUint16(pMsg->msgStats.trackingResponseAttempts);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.trackingResponseAttempts);
            *pBuf++ = Util_loUint16(pMsg->msgStats.trackingResponseSent);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.trackingResponseSent);
            *pBuf++ = Util_loUint16(pMsg->msgStats.configRequests);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.configRequests);
            *pBuf++ = Util_loUint16(pMsg->msgStats.configResponseAttempts);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.configResponseAttempts);
            *pBuf++ = Util_loUint16(pMsg->msgStats.configResponseSent);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.configResponseSent);
            *pBuf++ = Util_loUint16(pMsg->msgStats.channelAccessFailures);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.channelAccessFailures);
            *pBuf++ = Util_loUint16(pMsg->msgStats.macAckFailures);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.macAckFailures);
            *pBuf++ = Util_loUint16(pMsg->msgStats.otherDataRequestFailures);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.otherDataRequestFailures);
            *pBuf++ = Util_loUint16(pMsg->msgStats.syncLossIndications);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.syncLossIndications);
            *pBuf++ = Util_loUint16(pMsg->msgStats.rxDecryptFailures);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.rxDecryptFailures);
            *pBuf++ = Util_loUint16(pMsg->msgStats.txEncryptFailures);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.txEncryptFailures);
            *pBuf++ = Util_loUint16(pMsg->msgStats.resetCount);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.resetCount);
            *pBuf++ = Util_loUint16(pMsg->msgStats.lastResetReason);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.lastResetReason);
            *pBuf++ = Util_loUint16(pMsg->msgStats.joinTime);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.joinTime);
            *pBuf++ = Util_loUint16(pMsg->msgStats.interimDelay);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.interimDelay);
            *pBuf++ = Util_loUint16(pMsg->msgStats.numBroadcastMsgRcvd);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.numBroadcastMsgRcvd);
            *pBuf++ = Util_loUint16(pMsg->msgStats.numBroadcastMsglost);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.numBroadcastMsglost);
            *pBuf++ = Util_loUint16(pMsg->msgStats.avgE2EDelay);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.avgE2EDelay);
            *pBuf++ = Util_loUint16(pMsg->msgStats.worstCaseE2EDelay);
            *pBuf++ = Util_hiUint16(pMsg->msgStats.worstCaseE2EDelay);
        }
        if(pMsg->frameControl & Smsgs_dataFields_configSettings)
        {
            *pBuf++ = Util_breakUint32(pMsg->configSettings.reportingInterval,
                                       0);
            *pBuf++ = Util_breakUint32(pMsg->configSettings.reportingInterval,
                                       1);
            *pBuf++ = Util_breakUint32(pMsg->configSettings.reportingInterval,
                                       2);
            *pBuf++ = Util_breakUint32(pMsg->configSettings.reportingInterval,
                                       3);
            *pBuf++ = Util_breakUint32(pMsg->configSettings.pollingInterval, 0);
            *pBuf++ = Util_breakUint32(pMsg->configSettings.pollingInterval, 1);
            *pBuf++ = Util_breakUint32(pMsg->configSettings.pollingInterval, 2);
            *pBuf++ = Util_breakUint32(pMsg->configSettings.pollingInterval, 3);
        }

        sendAREQ(MT_CSF_CMDID_SENSOR_UPDATE_IND, len, pRespBuf);

        MAP_ICall_free(pRespBuf);
    }
}

/*!
 Generate the State Change Indication.

 Public function defined in mt_csf.h
 */
void MTCSF_stateChangeIndCB(Cllc_states_t state)
{
    uint8_t newState = (uint8_t)state;

    sendAREQ(MT_CSF_CMDID_COLLECTOR_STATE_CHANGE_IND, 1, &newState);
}

/******************************************************************************
 Local functions
 *****************************************************************************/

/*!
 * @breif   Send reset indication
 *
 * @param   resetReason - reason for reset
 *                     0 - Hard
 *                     2 - HAL/ICALL
 *                     3 - MAC
 *                     4 - TIRTOS
 */
static void sendResetIndication(uint8_t reason)
{
    uint8_t rsp[1 + sizeof(mtVersion)];

    rsp[0] = reason;

    /* MT_MAC revision info */
    memcpy(&rsp[1], mtVersion, sizeof(mtVersion));

    /* Send out Reset Response message */
    sendNPI(((uint8_t)RPC_CMD_AREQ | (uint8_t)MT_SYS_SYSID),
            MT_SYS_CMDID_RESET, sizeof(rsp), (uint8_t *)rsp);
}

/*!
 * @brief     CLLC State Changed callback.
 *
 * @param     state - CLLC new state
 */
static uint8_t parseHeader(msgHdr_t *pHdr, uint8_t *pBuf)
{
    uint8_t status = RPC_STAT_SUCCESS;
    uint8_t *pMsg = pBuf;

    /* for now, use the 1 byte */
    pHdr->len = (uint16_t)*pMsg++;

    pHdr->type = (uint8_t)((*pMsg & RPC_TYPE_MASK) >> 5);
    pHdr->subSystemId = (uint8_t)(*pMsg++ & RPC_SUBSYSTEM_MASK);
    pHdr->cmdId = (uint16_t)(*pMsg++);

    /* Store pointer to data section */
    pHdr->pData = pMsg;

    return (status);
}

/*!
 * @brief   Process the join permit request.
 *
 * @param  pHdr -  incoming message
 */
static void processPermitJoin(msgHdr_t *pHdr)
{
    uint8_t status = CSF_STAT_SUCCESS;
    uint8_t *pBuf = pHdr->pData;
    uint32_t duration;

    duration = Util_buildUint32(pBuf[0], pBuf[1], pBuf[2], pBuf[3]);

    /* Build and send back the response */
    sendSRSP(pHdr->cmdId, 1, &status);

    Cllc_setJoinPermit(duration);
}

/*!
 * @brief   Process the Get Statistics request.
 *
 * @param  pHdr -  incoming message
 */
static void processGetStats(msgHdr_t *pHdr)
{
    uint8_t respBuf[GET_STATS_RSP_LEN];
    uint8_t *pBuf = respBuf;

    *pBuf++ = CSF_STAT_SUCCESS;

    /* Update the collector statistics */
    Collector_updateStats();

    *pBuf++ = Util_breakUint32(Collector_statistics.trackingRequestAttempts, 0);
    *pBuf++ = Util_breakUint32(Collector_statistics.trackingRequestAttempts, 1);
    *pBuf++ = Util_breakUint32(Collector_statistics.trackingRequestAttempts, 2);
    *pBuf++ = Util_breakUint32(Collector_statistics.trackingRequestAttempts, 3);

    *pBuf++ = Util_breakUint32(Collector_statistics.trackingReqRequestSent, 0);
    *pBuf++ = Util_breakUint32(Collector_statistics.trackingReqRequestSent, 1);
    *pBuf++ = Util_breakUint32(Collector_statistics.trackingReqRequestSent, 2);
    *pBuf++ = Util_breakUint32(Collector_statistics.trackingReqRequestSent, 3);

    *pBuf++ = Util_breakUint32(Collector_statistics.trackingResponseReceived,
                               0);
    *pBuf++ = Util_breakUint32(Collector_statistics.trackingResponseReceived,
                               1);
    *pBuf++ = Util_breakUint32(Collector_statistics.trackingResponseReceived,
                               2);
    *pBuf++ = Util_breakUint32(Collector_statistics.trackingResponseReceived,
                               3);

    *pBuf++ = Util_breakUint32(Collector_statistics.configRequestAttempts, 0);
    *pBuf++ = Util_breakUint32(Collector_statistics.configRequestAttempts, 1);
    *pBuf++ = Util_breakUint32(Collector_statistics.configRequestAttempts, 2);
    *pBuf++ = Util_breakUint32(Collector_statistics.configRequestAttempts, 3);

    *pBuf++ = Util_breakUint32(Collector_statistics.configReqRequestSent, 0);
    *pBuf++ = Util_breakUint32(Collector_statistics.configReqRequestSent, 1);
    *pBuf++ = Util_breakUint32(Collector_statistics.configReqRequestSent, 2);
    *pBuf++ = Util_breakUint32(Collector_statistics.configReqRequestSent, 3);

    *pBuf++ = Util_breakUint32(Collector_statistics.configResponseReceived, 0);
    *pBuf++ = Util_breakUint32(Collector_statistics.configResponseReceived, 1);
    *pBuf++ = Util_breakUint32(Collector_statistics.configResponseReceived, 2);
    *pBuf++ = Util_breakUint32(Collector_statistics.configResponseReceived, 3);

    *pBuf++ = Util_breakUint32(Collector_statistics.sensorMessagesReceived, 0);
    *pBuf++ = Util_breakUint32(Collector_statistics.sensorMessagesReceived, 1);
    *pBuf++ = Util_breakUint32(Collector_statistics.sensorMessagesReceived, 2);
    *pBuf++ = Util_breakUint32(Collector_statistics.sensorMessagesReceived, 3);


    *pBuf++ = Util_breakUint32(Collector_statistics.channelAccessFailures, 0);
    *pBuf++ = Util_breakUint32(Collector_statistics.channelAccessFailures, 1);
    *pBuf++ = Util_breakUint32(Collector_statistics.channelAccessFailures, 2);
    *pBuf++ = Util_breakUint32(Collector_statistics.channelAccessFailures, 3);

    *pBuf++ = Util_breakUint32(Collector_statistics.ackFailures, 0);
    *pBuf++ = Util_breakUint32(Collector_statistics.ackFailures, 1);
    *pBuf++ = Util_breakUint32(Collector_statistics.ackFailures, 2);
    *pBuf++ = Util_breakUint32(Collector_statistics.ackFailures, 3);

    *pBuf++ = Util_breakUint32(Collector_statistics.otherTxFailures, 0);
    *pBuf++ = Util_breakUint32(Collector_statistics.otherTxFailures, 1);
    *pBuf++ = Util_breakUint32(Collector_statistics.otherTxFailures, 2);
    *pBuf++ = Util_breakUint32(Collector_statistics.otherTxFailures, 3);

    *pBuf++ = Util_breakUint32(Cllc_statistics.fhNumPASolicitReceived, 0);
    *pBuf++ = Util_breakUint32(Cllc_statistics.fhNumPASolicitReceived, 1);
    *pBuf++ = Util_breakUint32(Cllc_statistics.fhNumPASolicitReceived, 2);
    *pBuf++ = Util_breakUint32(Cllc_statistics.fhNumPASolicitReceived, 3);

    *pBuf++ = Util_breakUint32(Cllc_statistics.fhNumPASent, 0);
    *pBuf++ = Util_breakUint32(Cllc_statistics.fhNumPASent, 1);
    *pBuf++ = Util_breakUint32(Cllc_statistics.fhNumPASent, 2);
    *pBuf++ = Util_breakUint32(Cllc_statistics.fhNumPASent, 3);

    *pBuf++ =
        Util_breakUint32(Cllc_statistics.fhNumPANConfigSolicitsReceived, 0);
    *pBuf++ =
        Util_breakUint32(Cllc_statistics.fhNumPANConfigSolicitsReceived, 1);
    *pBuf++ =
        Util_breakUint32(Cllc_statistics.fhNumPANConfigSolicitsReceived, 2);
    *pBuf++ =
        Util_breakUint32(Cllc_statistics.fhNumPANConfigSolicitsReceived, 3);

    *pBuf++ = Util_breakUint32(Cllc_statistics.fhNumPANConfigSent, 0);
    *pBuf++ = Util_breakUint32(Cllc_statistics.fhNumPANConfigSent, 1);
    *pBuf++ = Util_breakUint32(Cllc_statistics.fhNumPANConfigSent, 2);
    *pBuf++ = Util_breakUint32(Cllc_statistics.fhNumPANConfigSent, 3);

    *pBuf++ = Util_breakUint32(Collector_statistics.rxDecryptFailures, 0);
    *pBuf++ = Util_breakUint32(Collector_statistics.rxDecryptFailures, 1);
    *pBuf++ = Util_breakUint32(Collector_statistics.rxDecryptFailures, 2);
    *pBuf++ = Util_breakUint32(Collector_statistics.rxDecryptFailures, 3);

    *pBuf++ = Util_breakUint32(Collector_statistics.txEncryptFailures, 0);
    *pBuf++ = Util_breakUint32(Collector_statistics.txEncryptFailures, 1);
    *pBuf++ = Util_breakUint32(Collector_statistics.txEncryptFailures, 2);
    *pBuf++ = Util_breakUint32(Collector_statistics.txEncryptFailures, 3);

    /* Send back the response */
    sendSRSP(pHdr->cmdId, GET_STATS_RSP_LEN, respBuf);
}

/*!
 * @brief  Send the Network Information message.
 *
 * @param  ind -  true if indication (AREQ), false if response (SRSP)
 * @param  cmdId - command ID of the message to send
 */
static void sendNetworkInfo( bool ind, uint8_t cmdId)
{
    uint8_t status;
    Llc_netInfo_t infoRec;

    memset(&infoRec, 0, sizeof(Llc_netInfo_t));

    status = Csf_getNetworkInformation(&infoRec);
    if(status == true)
    {
        uint8_t *pRespBuf;

        uint16_t len = BASIC_NET_INFO_LEN;

        /* Figure out the length */
        if(ind == false)
        {
            len += 3;
        }

        pRespBuf = (uint8_t *)MAP_ICall_malloc(len);
        if(pRespBuf)
        {
            uint8_t *pBuf = pRespBuf;

            memset(pRespBuf, 0, len);

            if(ind == false)
            {
                uint16_t numDevices = Csf_getNumDeviceListEntries();

                *pBuf++ = CSF_STAT_SUCCESS;
                *pBuf++ = Util_loUint16(numDevices);
                *pBuf++ = Util_hiUint16(numDevices);
            }

            *pBuf++ = Util_loUint16(infoRec.devInfo.panID);
            *pBuf++ = Util_hiUint16(infoRec.devInfo.panID);
            *pBuf++ = Util_loUint16(infoRec.devInfo.shortAddress);
            *pBuf++ = Util_hiUint16(infoRec.devInfo.shortAddress);

            memcpy(pBuf, infoRec.devInfo.extAddress, (APIMAC_SADDR_EXT_LEN));
            pBuf += APIMAC_SADDR_EXT_LEN;

            *pBuf++ = infoRec.channel;
            *pBuf++ = infoRec.fh;

            if(ind)
            {
                sendAREQ(cmdId, len, pRespBuf);
            }
            else
            {
                sendSRSP(cmdId, len, pRespBuf);
            }

            MAP_ICall_free(pRespBuf);
        }
        else
        {
            status = false;
        }
    }

    if(status == false)
    {
        status = CSF_STAT_FAIL;
        /* Send back the response */
        sendSRSP(cmdId, 1, &status);
    }
}

/*!
 * @brief  Process the Get Device Info request.
 *
 * @param  pItem -  Device info record
 * @param  pBuf - pointer to buffer to fill
 */
static void fillInDevInfo(Llc_deviceListItem_t *pItem, uint8_t *pBuf)
{
    *pBuf++ = Util_loUint16(pItem->devInfo.panID);
    *pBuf++ = Util_hiUint16(pItem->devInfo.panID);
    *pBuf++ = Util_loUint16(pItem->devInfo.shortAddress);
    *pBuf++ = Util_hiUint16(pItem->devInfo.shortAddress);
    memcpy(pBuf, pItem->devInfo.extAddress, APIMAC_SADDR_EXT_LEN);
    pBuf += APIMAC_SADDR_EXT_LEN;
    *pBuf++ = ApiMac_convertCapabilityInfo(&pItem->capInfo);
    *pBuf++ = Util_breakUint32(pItem->rxFrameCounter, 0);
    *pBuf++ = Util_breakUint32(pItem->rxFrameCounter, 1);
    *pBuf++ = Util_breakUint32(pItem->rxFrameCounter, 2);
    *pBuf++ = Util_breakUint32(pItem->rxFrameCounter, 3);
}

/*!
 * @brief   Process the Get Device Info request.
 *
 * @param  pHdr -  incoming message
 */
static void processDeviceInfo(msgHdr_t *pHdr)
{
    uint8_t *pBuf = pHdr->pData;
    uint16_t devIndex;
    uint16_t numDevs;
    Llc_deviceListItem_t item;

    devIndex = Util_buildUint16(pBuf[0], pBuf[1]);
    numDevs = Csf_getNumDeviceListEntries();
    if(Csf_getDeviceItem(devIndex, &item))
    {
        uint8_t respBuf[DEVICE_INFO_RSP_LEN];
        uint8_t *pBuf = respBuf;

        *pBuf++ = CSF_STAT_SUCCESS;
        *pBuf++ = Util_loUint16(numDevs);
        *pBuf++ = Util_hiUint16(numDevs);
        *pBuf++ = Util_loUint16(devIndex);
        *pBuf++ = Util_hiUint16(devIndex);
        fillInDevInfo(&item, pBuf);

        /* Send back the response */
        sendSRSP(pHdr->cmdId, DEVICE_INFO_RSP_LEN, respBuf);
    }
    else
    {
        uint8_t status = CSF_STAT_FAIL;

        /* Send back the response */
        sendSRSP(pHdr->cmdId, 1, &status);
    }
}

/*!
 * @brief   Process the Config Request request.
 *
 * @param  pHdr -  incoming message
 */
static void processConfigRequest(msgHdr_t *pHdr)
{
    Collector_status_t stat = Collector_status_invalid_state;

    if (pHdr->len == CONFIG_REQ_LEN)
    {
        uint8_t *pBuf = pHdr->pData;
        uint16_t shortAddr;
        uint16_t frameControl;
        uint32_t reportingInterval;
        uint32_t pollingInterval;
        ApiMac_sAddr_t dstAddr;

        shortAddr = Util_buildUint16(pBuf[0], pBuf[1]);
        pBuf += 2;
        frameControl = Util_buildUint16(pBuf[0], pBuf[1]);
        pBuf += 2;
        reportingInterval = Util_buildUint32(pBuf[0], pBuf[1], pBuf[2], pBuf[3]);
        pBuf += 4;
        pollingInterval = Util_buildUint32(pBuf[0], pBuf[1], pBuf[2], pBuf[3]);
        pBuf += 4;

        dstAddr.addrMode = ApiMac_addrType_short;
        dstAddr.addr.shortAddr = shortAddr;

        stat = Collector_sendConfigRequest(&dstAddr, frameControl,
                                           reportingInterval, pollingInterval);
    }

    /* Send back the response */
    sendSRSP(pHdr->cmdId, 1, (uint8_t*)&stat);
}

/*!
 * @brief   Process the Toggle Request request.
 *
 * @param  pHdr -  incoming message
 */
static void processToggleRequest(msgHdr_t *pHdr)
{
    Collector_status_t stat = Collector_status_invalid_state;

    if (pHdr->len == TOGGLE_REQ_LEN)
    {
        uint8_t *pBuf = pHdr->pData;
        uint16_t shortAddr;
        ApiMac_sAddr_t dstAddr;

        shortAddr = Util_buildUint16(pBuf[0], pBuf[1]);

        dstAddr.addrMode = ApiMac_addrType_short;
        dstAddr.addr.shortAddr = shortAddr;

        stat = Collector_sendToggleLedRequest(&dstAddr);
    }

    /* Send back the response */
    sendSRSP(pHdr->cmdId, 1, (uint8_t*)&stat);
}

/*!
 * @brief   Overload the MT function to Build and Send Response.
 *          This function relays outgoing MT messages to the NPI task for
 *          further handling.
 *
 * @param  type -  MT Command field
 * @param  cmd - MT Command ID
 * @param  len - lenght MT command
 * @param  pData - pointer to MT commnad buffer
 */
static void sendNPI(uint8_t type, uint8_t cmd, uint8_t len, uint8_t *pData)
{
    /* Allocate ICall message buffer to send message to NPI task */
    uint8_t *pRspMsg = MAP_ICall_allocMsg(RPC_FRAME_HDR_SZ + len);

    if(pRspMsg != NULL)
    {
        /* Populuate the MT header fields */
        pRspMsg[RPC_POS_LEN] = len;
        pRspMsg[RPC_POS_CMD0] = type;
        pRspMsg[RPC_POS_CMD1] = cmd;

        memcpy(pRspMsg + RPC_FRAME_HDR_SZ, pData, len);

#if defined(USE_ICALL) && !defined(USE_DMM)
        // Send the message to NPI
        ICall_sendServiceMsg(npiAppEntityID, ICALL_SERVICE_CLASS_NPI,
                             ICALL_MSG_FORMAT_KEEP,
                             pRspMsg);
#else
        OsalPort_msgSend(npiAppEntityID,pRspMsg);
#endif
    }
}

/*!
 * @brief   Send Response.
 *
 * @param  cmdId - MT Command ID
 * @param  datalen - lenght MT command
 * @param  pData - pointer to MT commnad buffer
 */
static void sendSRSP(uint8_t cmdId, uint8_t dataLen, uint8_t *pData)
{
    sendNPI(((uint8_t)RPC_CMD_SRSP | (uint8_t)MT_CSF_SYSID),
            cmdId,
            dataLen, pData);
}

/*!
 * @brief   Send Asynch Indication.
 *
 * @param  cmdId - MT Command ID
 * @param  datalen - lenght MT command
 * @param  pData - pointer to MT commnad buffer
 */
static void sendAREQ(uint8_t cmdId, uint8_t dataLen, uint8_t *pData)
{
    sendNPI(((uint8_t)RPC_CMD_AREQ | (uint8_t)MT_CSF_SYSID),
            cmdId,
            dataLen, pData);
}

/*!
 * @brief   Process a message from ApiMac
 *
 * @param   entityID - ICall application entity ID
 * @param   param2   - not used
 * @param   pMsg     - pointer to incoming message buffer
 *
 * @return  void
 */
static void processMsg(uint16_t entityID, uint16_t param2, void *pMsg)
{
    /* Intentionally not used */
    (void)param2;
    (void)entityID;

#ifdef OSAL_PORT2TIRTOS

#else
    /* Verify it's an NPI message */
    //if(entityID == npiAppEntityID)
#endif
    {
        /* Hand it to MT incoming message handler */
        MTCSF_processMsg(pMsg);

        /* Count an incoming MT message */
        numRxMsgs += 1;
    }
}

/*!
 * @brief   Relay an NPI task RX message to ApiMac task,
 *          allows MT message processing at ApiMac task context
 *
 * @param   pMsg - pointer to incoming message buffer
 *
 * @return  void
 */
static void relayRxMsg(void *pMsg)
{
    NPIMSG_msg_t *pNpiMsg = pMsg;
    macNpiMtMsg_t *pMacNpiMtMsg;
    uint16_t mtMsgLen;
    uint8_t *pData;

#ifdef OSAL_PORT2TIRTOS
    OsalPort_MsgHdr* hdr = (OsalPort_MsgHdr *) (pNpiMsg->pBuf) - 1;
#else
    ICall_MsgHdr *hdr = (ICall_MsgHdr *) (pNpiMsg->pBuf) - 1;
#endif
    mtMsgLen = hdr->len;

    /* Allocate memory for the data */
    pMacNpiMtMsg = (macNpiMtMsg_t *) MAP_ICall_allocMsg(sizeof(macNpiMtMsg_t) + mtMsgLen);

    if (pMacNpiMtMsg == NULL)
    {
        MAP_ICall_free(pNpiMsg);
        return;
    }

    /* set up the event */
    pMacNpiMtMsg->hdr.event = MAC_NCP_MT_MSG;

    /* copy message to new created MAC NPI MT Msg */
    pData = (uint8_t *)pMacNpiMtMsg + sizeof(macNpiMtMsg_t);
    memcpy(pData,pNpiMsg->pBuf,mtMsgLen);

    /* free old allocated MSg */
    MAP_ICall_freeMsg(pNpiMsg->pBuf);

#ifdef OSAL_PORT2TIRTOS
    /* Relay the message to ApiMac */
    OsalPort_msgSend(appTaskId, (uint8_t*)pMacNpiMtMsg );
#else
    /* Relay the message to ApiMac */
    ICall_send(npiAppEntityID, ApiMac_appEntity,
     //          ICALL_MSG_FORMAT_KEEP, pNpiMsg->pBuf);
               ICALL_MSG_FORMAT_KEEP, pMacNpiMtMsg);

#endif

    MAP_ICall_free(pNpiMsg);

    numNPIRxMsgs++;
}

/*!
 * @brief   Count an NPI task TX message from ApiMac task
 *
 * @param   pMsg - pointer to incoming message buffer
 *
 * @return  void
 */
static void relayTxMsg(void *pMsg)
{
    /* Intentionally not used */
    (void)pMsg;

    /* Count an outgoing MT message */
    numTxMsgs += 1;
}

