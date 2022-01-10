/******************************************************************************

 @file ti_154stack_config.h

 @brief TI-15.4 Stack configuration parameters for Sensor applications

 Group: WCS LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2021, Texas Instruments Incorporated
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
#ifndef TI_154STACK_CONFIG_H
#define TI_154STACK_CONFIG_H

/******************************************************************************
 Includes
 *****************************************************************************/
#include "api_mac.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/
/* config parameters */
/*! Security Enable - set to true to turn on security */
#define CONFIG_SECURE                true
/*! PAN ID */
#define CONFIG_PAN_ID                0xFFFF
/*! FH disabled as default */
#define CONFIG_FH_ENABLE             false

/*!
 Beacon order, value of 15 indicates non beacon mode,
 8 is a good value for beacon mode
*/
#define CONFIG_MAC_BEACON_ORDER          15
/*!
 Superframe order, value of 15 indicates non beacon mode,
 8 is a good value for beacon mode
*/
#define CONFIG_MAC_SUPERFRAME_ORDER      15
/*! Maximum number of message failure, to indicate sync loss */
#define CONFIG_MAX_DATA_FAILURES     5
/* Interval for scan backoff */
#define CONFIG_SCAN_BACKOFF_INTERVAL  5000
/* Interval for delay between orphan notifications */
#define CONFIG_ORPHAN_BACKOFF_INTERVAL 300000

/*! Setting for Phy ID */
#define CONFIG_PHY_ID                (APIMAC_250KBPS_IEEE_PHY_0)

/*! Setting for channel page */
#define CONFIG_CHANNEL_PAGE          (APIMAC_CHANNEL_PAGE_NONE)

/*! scan duration */
#define CONFIG_SCAN_DURATION         5

/*!
 Coordinator Short Address When Operating with FH Enabled.
 */
#define FH_COORD_SHORT_ADDR 0xAABB
/*!
 Range Extender Mode setting.
 The following modes are available.
 APIMAC_NO_EXTENDER - does not have PA/LNA
 APIMAC_HIGH_GAIN_MODE - high gain mode
 To enable CC1190, use
 #define CONFIG_RANGE_EXT_MODE       APIMAC_HIGH_GAIN_MODE
*/
#define CONFIG_RANGE_EXT_MODE       APIMAC_NO_EXTENDER

/*! Setting Default Key*/
#define KEY_TABLE_DEFAULT_KEY {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,\
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

/*!
 Channel mask used when CONFIG_FH_ENABLE is false.
 Each bit indicates if the corresponding channel is to be scanned
 First byte represents channels 0 to 7 and the last byte represents
 channels 128 to 135.
 For byte zero in the bit mask, LSB representing Ch0.
 For byte 1, LSB represents Ch8 and so on.
 e.g., 0x01 0x10 represents Ch0 and Ch12 are included.
 The default of 0x0F represents channels 0-3 are selected.
 APIMAC_50KBPS_915MHZ_PHY_1 (50kbps/2-FSK/915MHz band) has channels 0 - 128.
 APIMAC_50KBPS_868MHZ_PHY_3 (50kbps/2-FSK/863MHz band) has channels 0 - 33.
 APIMAC_50KBPS_433MHZ_PHY_128 (50kbps/2-FSK/433MHz band) has channels 0 - 6.
 IEEE 802.15.4 has channels 11 - 26.
*/
#define CONFIG_CHANNEL_MASK           { 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00, 0x00, 0x00 }
/*!
 Channel mask used when CONFIG_FH_ENABLE is true.
 Represents the list of channels on which the device can hop.
 When CONFIG_RX_ON_IDLE is true, the actual sequence will
 be based on DH1CF function. When it is set to false, the sequence
 shall be a linear hopping over available channels in ascending order and
 shall be used to change channel during the join phase.
 It is represented as a bit string with LSB representing Ch0.
 e.g., 0x01 0x10 represents Ch0 and Ch12 are included.
 */
#define CONFIG_FH_CHANNEL_MASK        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                                        0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00, 0x00, 0x00,}
/* FH related config variables */
/*!
 List of channels to target the Async frames
 It is represented as a bit string with LSB representing Ch0
 e.g., 0x01 0x10 represents Ch0 and Ch12 are included
 It should cover all channels that could be used by a target device in its
 hopping sequence. Channels marked beyond number of channels supported by
 PHY Config will be excluded by stack. To avoid interference on a channel,
 it should be removed from Async Mask and added to exclude channels
 (CONFIG_CHANNEL_MASK).
 */
#define FH_ASYNC_CHANNEL_MASK         { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                                        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                                        0xFF, 0xFF, 0xFF, 0xFF, 0xFF }

/*! Rx on when idle, false for sleepy device, true for non sleepy device */
#define CONFIG_RX_ON_IDLE          false

/*! MAC Parameter */
/*! Min BE - Minimum Backoff Exponent */
#define CONFIG_MIN_BE   3
/*! Max BE - Maximum Backoff Exponent */
#define CONFIG_MAX_BE   5

/*! Polling interval in milliseconds */
#define CONFIG_POLLING_INTERVAL      500

/*! Default Reporting Interval - in milliseconds */
#define CONFIG_REPORTING_INTERVAL  2000

/*! If enabled, the periodic sensor message shall be sent as a fixed size
 * packet of specified size. If set to 0, the periodic sensor message shall be
 * of type sensor data specified in smsgs.h
 */
#define SENSOR_TEST_RAMP_DATA_SIZE   0

/*! value for ApiMac_FHAttribute_netName */
#define CONFIG_FH_NETNAME            {"FHTest"}
/*!
 Value for Transmit Power in dBm
 For US and ETSI band, Default value is 10, allowed values are
 -10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 and 14dBm.
 For China band, allowed values are 6, 10, 13, 14 and 15dBm.
 For CC1190, allowed values are between 18, 23, 25, 26 and 27dBm.
 When the nodes in the network are close to each other
 lowering this value will help reduce saturation */

#define CONFIG_TRANSMIT_POWER        0

/*!
* Enable this mode for certfication.
* For FH certification, CONFIG_FH_ENABLE should
* also be enabled
*/
#define CERTIFICATION_TEST_MODE     false

#ifdef POWER_MEAS
/*!
 Power profile to be used when Power MEAS is enabled.
 Profile 1 - POLL_ACK - Polling Only
 Profile 2 - DATA_ACK - 20 byte application data + ACK from sensor to collector
 Profile 3 - POLL_DATA - Poll + received Data from collector
 Profile 4 - SLEEP - No Poll or Data. In Beacon mode, beacon RX would occur
 */
#define POWER_TEST_PROFILE  DATA_ACK
#endif

/* Check if stack level security is enabled if application security is enabled */
#if CONFIG_SECURE
#if !defined(FEATURE_MAC_SECURITY)
#error "Define FEATURE_MAC_SECURITY or FEATURE_ALL_MODES in ti_154stack_features.h to \
        be able to use security at application level"
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* TI_154STACK_CONFIG_H */

