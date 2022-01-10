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
#ifndef MT_CSF_H
#define MT_CSF_H

/******************************************************************************
 Includes
 *****************************************************************************/

/*!
 @mainpage TIMAC 2.0 Collector MT Interface

 Overview
 =============================
 This document describes the monitor and test (MT) messages to the collector
 application.
 <BR>
 The MT interface itself isn't described in this document, just the messages
 themselves.
 <BR>
 All of the following messages use [MT_CSF_SYSID](@ref MT_CSF_SYSID) as the
 SubSystem ID, and follows the same byte ordering in normal MT processing.
 In the MT Protocol, the CMD0 field is the combined SREQ/SRSP/AREQ and
 SubSystemID and the CMD1 is the "Command ID" listed in each message.
 The CMD0 fields for this subSystemID are:
  - 0x29 for SREQ messages
  - 0x69 for SRSP messages
  - 0x49 for AREQ messages

 Request and Response Messages
 =============================
 The <b>Start Request Message</b> (SREQ) is defined as:
 - Command ID - [MT_CSF_CMDID_START](@ref MT_CSF_CMDID_START)

 The <b>Start Response Message</b> (SRSP) is defined as:
 - Command ID - [MT_CSF_CMDID_START](@ref MT_CSF_CMDID_START)
 - status - 1 byte - 0 for success, 1 for not allowed (AUTO_START enabled)

 The <b>Permit Join Request Message</b> (SREQ) is defined as:
 - Command ID - [MT_CSF_CMDID_PERMIT_JOIN](@ref MT_CSF_CMDID_PERMIT_JOIN)
 - duration - uint32_t -  0 sets it Off, 0xFFFFFFFF sets it on indefinitely.
 Any other non zero value sets it on for that duration (in milliseconds).

 The <b>Permit Join Response Message</b> (SRSP) is defined as:
 - Command ID - [MT_CSF_CMDID_PERMIT_JOIN](@ref MT_CSF_CMDID_PERMIT_JOIN)
 - status - 1 byte

 The <b>Get Statistics Request Message</b> (SREQ), which requests the
 Collector's data statistics, is defined as:
 - Command ID - [MT_CSF_CMDID_GET_STATS](@ref MT_CSF_CMDID_GET_STATS)

 The <b>Get Statistics Response Message</b> (SRSP) is defined as:
 - Command ID - [MT_CSF_CMDID_GET_STATS](@ref MT_CSF_CMDID_GET_STATS)
 - status - 1 byte
 - trackingRequestAttempts - uint32_t - Total number of tracking request
 messages attempted.
 - trackingRequestSent - uint32_t - Total number of tracking request
 messages sent.
 - trackingResponseReceived - uint32_t - Total number of tracking response
 messages received.
 - configRequestAttempts - uint32_t - Total number of config request
 messages attempted.
 - configRequestSent - uint32_t - Total number of config request
 messages sent.
 - configResponseReceived - uint32_t - Total number of config response
 messages received.
  - sensorMessagesReceived - uint32_t - Total number of sensor
 messages received.
 - channelAccessFailures - uint32_t - Total number of failed messages
 because of channel access failure
 - ackFailures - uint32_t - Total number of failed messages because
 of ACKs not received.
 - otherTxFailures - uint32_t - Total number of failed transmit messages
 that are not channel access failure and not ACK failures.
 - fhNumPASolicitReceived - uint32_t - Total number of PA Solicit messsages
 received.
 - fhNumPASent - uint32_t - Total number of PA messsages
 sent.
 - fhNumPANConfigSolicitsReceived - uint32_t - Total number of PA Config
 Solicits received.
 - fhNumPANConfigSent - uint32_t - Total number of PA Config
 Solicits sent.
 - rxDecryptFailures - uint32_t - Total number of RX Decrypt failures.
 - txEncryptFailures - uint32_t - Total number of TX Encrypt failures.

 The <b>Network Information Request Message</b> (SREQ), which requests the
 Collector's network settings (determined when the coordinator is started),
 is defined as:
 - Command ID - [MT_CSF_CMDID_GET_NETWORK_INFO]
 (@ref MT_CSF_CMDID_GET_NETWORK_INFO)

 The <b>Network Information Response Message</b> (SRSP) is defined as:
 - Command ID - [MT_CSF_CMDID_GET_NETWORK_INFO]
 (@ref MT_CSF_CMDID_GET_NETWORK_INFO)
 - status - 1 byte - 0 - if successful, 1 if not available and the rest of
 the message is not included.
 - numDevices - uint16_t - number of devices in the device list.
 - panID - uint16_t - The 16-bit PAN identifier of the device
 - shortAddress - uint16_t - The 16-bit short address of the device
 - extAddress - 8 bytes - The 64-bit IEEE extended address of the device.
 This element is also used inunsecuring operations on incoming frames.
 - channel - uint8_t - the set channel, not valid for frequency hopping
 - fhEnabled - 1 byte - true if frequency hopping is enabled, false if not.

 The <b>Device Information Request Message</b> (SREQ), which requests the
 stored information of a single device, is defined as:
 - Command ID - [MT_CSF_CMDID_GET_DEVICE_INFO]
 (@ref MT_CSF_CMDID_GET_DEVICE_INFO)
 - deviceIndex - uint16_t - This is the device index (0 to
 ("numDevices"-1) fromNetwork Information Response) in the device list.

 The <b>Device Information Response Message</b> (SRSP) is defined as:
 - Command ID - [MT_CSF_CMDID_GET_DEVICE_INFO]
 (@ref MT_CSF_CMDID_GET_DEVICE_INFO)
 - status - 1 byte - 0 - if successful, 1 if not available and the rest of
 the message is not included.
 - numDevices - uint16_t - total number of devices in the device list.
 - deviceIndex - uint16_t - This is the device index (0 to
 ("numDevices"-1) from Network Information Response) in the device list.
 - panID - uint16_t - The 16-bit PAN identifier of the device
 - shortAddress - uint16_t - The 16-bit short address of the device
 - extAddress - 8 bytes - The 64-bit IEEE extended address of the device.
 - capabilityInfo - 1 byte (bitmask values) - capability information
 - rxFrameCounter - uint32_t - last received frame counter

 The <b>Send Config Request Message</b> (SREQ), which sends a Config Request
 message to a device, is defined as:
 - Command ID - [MT_CSF_CMDID_SEND_CONFIG](@ref MT_CSF_CMDID_SEND_CONFIG)
 - shortAddr - uint16_t - destination short address of the device to send
 the message.
 - frameControl - uint16_t - configure what to the device is to report back.
     - include Temp Sensor (bit 0 - 0x0001)
     - include Light Sensor (bit 1 - 0x0002)
     - include Humidity Sensor (bit 2 - 0x0004)
     - include Message Statistics (bit 3 - 0x0008)
     - include Config Settings (bit 4 - 0x0010)
 - reportingInterval - uint32_t - in millseconds- how often to report, 0
 means to turn off automated reporting, but will force the sensor device
 to send the Sensor Data message once.
 - pollingInterval - uint32_t - in millseconds- how often to the device is
 to poll its parent for data (for sleeping devices only.

 The <b>Send Config Response Message</b> (SRSP) is defined as:
 - Command ID - [MT_CSF_CMDID_SEND_CONFIG](@ref MT_CSF_CMDID_SEND_CONFIG)
 - status - 1 byte - 0 is success message sent, 1 is device not found, 2 means
 the device is in an invalid state to send a message.

 The <b>Send Toggle Request Message</b> (SREQ), which sends a Toggle LED Request
 message to a device, is defined as:
 - Command ID - [MT_CSF_CMDID_SEND_TOGGLE](@ref MT_CSF_CMDID_SEND_TOGGLE)
 - shortAddr - uint16_t - destination short address of the device to send
 the message.

 The <b>Send Toggle Response Message</b> (SRSP) is defined as:
 - Command ID - [MT_CSF_CMDID_SEND_TOGGLE](@ref MT_CSF_CMDID_SEND_TOGGLE)
 - status - 1 byte - 0 is success message sent, 1 is device not found, 2 means
 the device is in an invalid state to send a message.

 Asynchronous Messages
 =============================

 The <b>Network Information Indication Message</b> (AREQ), this message is
 received when a device starts as a coordinator, is defined as:
 - Command ID - [MT_CSF_CMDID_NETWORK_INFO_IND]
 (@ref MT_CSF_CMDID_NETWORK_INFO_IND)
 - panID - uint16_t - The 16-bit PAN identifier of the device
 - shortAddress - uint16_t - The 16-bit short address of the device
 - extAddress - 8 bytes - The 64-bit IEEE extended address of the device.
 This element is also used inunsecuring operations on incoming frames.
 - channel - uint8_t - the set channel, not valid for frequency hopping
 - fhEnabled - 1 byte - true if frequency hopping is enabled, false if not.

 The <b>Device Update Indication Message</b> (AREQ), received when a device 
 joins the coordinator, is defined as:
 - Command ID - [MT_CSF_CMDID_DEVICE_UPDATE_IND]
 (@ref MT_CSF_CMDID_DEVICE_UPDATE_IND)
 - panID - uint16_t - The 16-bit PAN identifier of the device
 - shortAddress - uint16_t - The 16-bit short address of the device
 - extAddress - 8 bytes - The 64-bit IEEE extended address of the device.
 - capabilityInfo - 1 byte (bitmask values) - capability information
 - rxFrameCounter - uint32_t - last received frame counter

 The <b>Device Not Active Indication Message</b> (AREQ), received when a
 device hasn't responded to a tracking message, is defined as:
 - Command ID - [MT_CSF_CMDID_DEVICE_NOT_ACTIVE_IND]
 (@ref MT_CSF_CMDID_DEVICE_NOT_ACTIVE_IND)
 - panID - uint16_t - The 16-bit PAN identifier of the device
 - shortAddress - uint16_t - The 16-bit short address of the device
 - extAddress - 8 bytes - The 64-bit IEEE extended address of the device.
 - tracking - 1 byte - true if becaue it isn't responding to tracking
 request.

 The <b>Config Response Indication Message</b> (AREQ), received when a 
 Config Response message is received, is defined as:
 - Command ID - [MT_CSF_CMDID_DEVICE_CONFIG_UPDATE_IND]
 (@ref MT_CSF_CMDID_DEVICE_CONFIG_UPDATE_IND)
 - shortAddress - uint16_t - The 16-bit short address of the device
 - rssi - int8_t - the received packet's signal strength.
 - status - uint16_t - Config Request status
     - 0 - Success
     - 1 - Message was invalid and ignored
     - 2 - Config message was received but only some frame control fields
     can be sent
 - frameControl - uint16_t - Result frame control, what the device will do.
     - include Temp Sensor (bit 0 - 0x0001)
     - include Light Sensor (bit 1 - 0x0002)
     - include Humidity Sensor (bit 2 - 0x0004)
     - include Message Statistics (bit 3 - 0x0008)
     - include Config Settings (bit 4 - 0x0010)
 - reportingInterval - uint32_t - in millseconds- how often to report, 0
 means to turn off automated reporting, but will force the sensor device
 to send the Sensor Data message once.
 - pollingInterval - uint32_t - in millseconds- how often to the device is
 to poll its parent for data (for sleeping devices only.

 The <b>Sensor Update Indication Message</b> (AREQ), received when a Sensor
 Data message is received, is defined as:
 - Command ID - [MT_CSF_CMDID_SENSOR_UPDATE_IND]
 (@ref MT_CSF_CMDID_SENSOR_UPDATE_IND)
 - shortAddress - uint16_t - The 16-bit short address of the device
 - rssi - int8_t - the received packet's signal strength.
 - extAddr - 8 bytes - device's extended address
 - frameControl - uint16_t - Result frame control, what the device will do.
     - include Temp Sensor (bit 0 - 0x0001)
     - include Light Sensor (bit 1 - 0x0002)
     - include Humidity Sensor (bit 2 - 0x0004)
     - include Message Statistics (bit 3 - 0x0008)
     - include Config Settings (bit 4 - 0x0010)
 - Data Fields - The length of this field is determined by what data fields
 are included.  The order of the data fields are determined by the bit
 position of the Frame Control field (low bit first).  For example, if the
 frame control field has Smsgs_dataFields_tempSensor and
 Smsgs_dataFields_lightSensor set, then the Temp Sensor field is first,
 followed by the light sensor field.
 - The <b>Temp Sensor Field</b> is defined as:
     - ambienceChipTemperature - int16_t - each value represents signed
     integer part of temperature in Deg C (-256 .. +255)
     - objectTemperature - int16_t - each value represents signed
     integer part of temperature in Deg C (-256 .. +255)
 - The <b>Light Sensor Field</b> is defined as:
     - rawSensorData - uint16_t - raw data read out of the OPT2001 light
     sensor.
 - The <b>Humidity Sensor Field</b> is defined as:
     - rawTempSensorData - uint16_t - raw temperature data from the
     Texas Instruments HCD1000 humidity sensor.
     - rawHumiditySensorData - uint16_t - raw humidity data from the
     Texas Instruments HCD1000 humidity sensor.
 - The <b>Message Statistics Field</b> is defined as:
     - joinAttempts - uint16_t - total number of join attempts (associate
     request sent)
     - joinFails - uint16_t - total number of join attempts failed
     - msgsAttempted - uint16_t - total number of sensor data messages
     attempted.
     - msgsSent - uint16_t - total number of sensor data messages successfully
     sent (OTA).
     - trackingRequests - uint16_t - total number of tracking requests received.
     - trackingResponseAttempts - uint16_t - total number of tracking response
     attempted
     - trackingResponseSent - uint16_t - total number of tracking response
     success
     - configRequests - uint16_t - total number of config requests received.
     - configResponseAttempts - uint16_t - total number of config response
     attempted
     - configResponseSent - uint16_t - total number of config response
     success
     - channelAccessFailures - uint16_t - total number of Channel Access
     Failures.  These are indicated in MAC data confirms for MAC data requests.
     - macAckFailures - uint16_t - Total number of MAC ACK failures. These are
     indicated in MAC data confirms for MAC data requests.
     - otherDataRequestFailures - uint16_t - Total number of MAC data request
     failures, other than channel access failure or MAC ACK failures.
     - syncLossIndications - uint16_t - Total number of sync loss failures
     received for sleepy devices.
     - rxDecryptFailues - uint16_t - Total number of RX Decrypt failures.
     - txEncryptFailues - uint16_t - Total number of TX Encrypt failures.
     - resetCount - uint16_t - Total number of resets.
     - lastResetReason - uint16_t - last assert reason for the last reset:
     0 = no reason, 2 = HAL/ICALL, 3 = MAC, 4 = TIRTOS.
 - The <b>Config Settings Field</b> is defined as:
     - reportingInterval - uint32_t - in millseconds, how often to report, 0
     means reporting is off.
     - pollingInterval - uint32_t - in millseconds, if the sensor device is
     a sleep device, this states how often the device polls its parent for
     data. This field is 0 if the device doesn't sleep.

 The <b>State Change Indication Message</b> (AREQ), received when there 
 is a coordinator state change, is defined as:
 - Command ID - [MT_CSF_CMDID_COLLECTOR_STATE_CHANGE_IND]
 (@ref MT_CSF_CMDID_COLLECTOR_STATE_CHANGE_IND)
 - newState - uint8_t - the state values are:
     - 0 - Powered up, not started and waiting for user to start
     - 1 - Starting coordinator, scanning and selecting the best parameters
     - 2 - Powered up, found network information, and restoring device in network
     - 3 - Device is operating as coordinator
     - 4 - Device is restored as coordinator in the network
     - 5 - Joining allowed state has changed to allowed
     - 6 - Joining allowed state has changed to not allowed

 The <b>Device Toggle Indication Message</b> (AREQ), received when a Sensor
 Toggle LED Response message is received, is defined as:
 - Command ID - [MT_CSF_CMDID_DEVICE_TOGGLE_IND]
 (@ref MT_CSF_CMDID_DEVICE_TOGGLE_IND)
 - shortAddress - uint16_t - The 16-bit short address of the device
 - ledState - uint8_t - the state values are:
     - 0 - LED is off
     - 1 - LED is on

 */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Typedefs
 *****************************************************************************/

#define MT_CSF_SYSID   9    /*!< re-using the MT_RPC_SYS_APP SubSystem ID */

/*! Start SREQ/SRSP message ID */
#define MT_CSF_CMDID_START                          0x01
/*! Permit Join SREQ/SRSP message */
#define MT_CSF_CMDID_PERMIT_JOIN                    0x02
/*!  Get Statistics SREQ/SRSP messageID */
#define MT_CSF_CMDID_GET_STATS                      0x03
/*! Get Network Information SREQ/SRSP message ID */
#define MT_CSF_CMDID_GET_NETWORK_INFO               0x04
/*! Get Device Information SREQ/SRSP message ID */
#define MT_CSF_CMDID_GET_DEVICE_INFO                0x05
/*! Send OTA Config SREQ/SRSP message ID */
#define MT_CSF_CMDID_SEND_CONFIG                    0x06
/*! Send Toggle LED SREQ/SRSP message ID */
#define MT_CSF_CMDID_SEND_TOGGLE                    0x07

/*! Network Information Indication AREQ message ID */
#define MT_CSF_CMDID_NETWORK_INFO_IND               0x84
/*! Device Update Indication AREQ message ID */
#define MT_CSF_CMDID_DEVICE_UPDATE_IND              0x85
/*! Device Not Active Indication AREQ message ID */
#define MT_CSF_CMDID_DEVICE_NOT_ACTIVE_IND          0x86
/*! Device Configuration Update Indication AREQ message ID */
#define MT_CSF_CMDID_DEVICE_CONFIG_UPDATE_IND       0x87
/*! Device Sensor Reading Indication AREQ message ID */
#define MT_CSF_CMDID_SENSOR_UPDATE_IND              0x88
/*! Collector State Change Indication AREQ message ID */
#define MT_CSF_CMDID_COLLECTOR_STATE_CHANGE_IND     0x89
/*! Device Toggle indication AREQ message ID */
#define MT_CSF_CMDID_DEVICE_TOGGLE_IND              0x8A

/******************************************************************************
 Function Prototypes
 *****************************************************************************/

/*!
 * @brief   Initialize the MTCSF system for use with NPI system
 *
 * @param   resetReason - saved reset reason
 */
extern void MTCSF_init(uint8_t resetReason, Button_Handle leftBtnHandle);

/*!
 * @brief   Process incoming MT_CSF message.
 *
 * @param   pBuf - pointer to the incoming message
 */
extern uint8_t MTCSF_processMsg(uint8_t *pBuf);

/*!
 * @brief   Display the MT CSF statistics.
 */
extern void MTCSF_displayStatistics(void);

/*!
 * @brief  Generate a Network Update Indication.
 */
extern void MTCSF_networkUpdateIndCB(void);

/*!
 * @brief  Generate a Device Update Indication.
 *
 * @param  pDevInfo - pointer to the device information
 * @param  pCapInfo - capability information of the joining device.
 */
extern void MTCSF_deviceUpdateIndCB(ApiMac_deviceDescriptor_t *pDevInfo,
                                    ApiMac_capabilityInfo_t *pCapInfo);

/*!
 * @brief       Generate the Device Not Active Indication.
 *
 * @param       pDevInfo - pointer to the device information
 * @param       timeout - true if not active because of tracking timeout.
 *              meaning that the device didn't respond to the tracking request
 *              within the timeout period.
 */
extern void MTCSF_deviceNotActiveIndCB(ApiMac_deviceDescriptor_t *pDevInfo,
                                       bool timeout);

/*!
 * @brief       Generate the Config Response Indication.
 *
 * @param       pSrcAddr - short address of the device that sent the message
 * @param       rssi - the received packet's signal strength
 * @param       pMsg - pointer to the Config Response message
 */
extern void MTCSF_configResponseIndCB(ApiMac_sAddr_t *pSrcAddr, int8_t rssi,
                                    Smsgs_configRspMsg_t *pMsg);

/*!
 * @brief       Generate the Sensor Update Indication.
 *
 * @param       pSrcAddr - short address of the device that sent the message
 * @param       rssi - the received packet's signal strength
 * @param       pMsg - pointer to the Sensor Data message
 */
extern void MTCSF_sensorUpdateIndCB(ApiMac_sAddr_t *pSrcAddr, int8_t rssi,
                                    Smsgs_sensorMsg_t *pMsg);

/*!
 * @brief       Generate the State Change Indication.
 *
 * @param       state - new state
 */
extern void MTCSF_stateChangeIndCB(Cllc_states_t state);

/*!
 * @brief       Generate the Device Toggle Indication.
 *
 * @param       shortAddr - short address of the device that sent the message
 * @param       ledState - 0 is off, 1 is on
 */
extern void MTCSF_deviceToggleIndCB(uint16_t shortAddr, bool ledState);


#ifdef __cplusplus
}
#endif

#endif /* MT_CSF_H */

