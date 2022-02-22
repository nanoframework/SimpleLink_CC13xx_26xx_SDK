/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 *  ======== LP_CC1354R10_RGZ_rf_defaults.js ========
 */

"use strict";

// Get common rf settings
const rfCommon = system.getScript("/ti/easylink/rf_config/"
    + "easylink_rf_config_common.js");

/*
 *  ======== Device Specific Proprietary PHY Settings ========
 *
 * These Objects will be used to extend the common proprietary phy settings
 * defined in easylink_rf_config_common.js.
 *
 * These Objects must contain the following elements:
 *      - args: Phy setting arguments to be passed to the radio config module
 *          - freqBand: The frequency band containing the carrierFrequency.
 *                      Valid options: 433, 868, 2440(reserved for future use)
 *          - phyType<band>: Name of the phy found in the radio config module.
 *                           Valid options: phyType868, phyType433,
 *                           phyType2440(reserved for future use)
 *          - carrierFrequency: Frequency setting within the freqBand
 *          - txPower: Transmit power setting, must match a value in radio
 *                     config's txPower drop-down
 *          - codeExportConfig: Exported code names to be passed to
 *                              radioConfig.codeExportConfig
 *              - cmdPropRadioDivSetup: Name of the generated
 *                      rfc_CMD_PROP_RADIO_DIV_SETUP_t command
 */

// Object containing 2GFSK, 50kbps settings for the LP_CC1354R10_RGZ
const devSpecific2Gfsk50KbpsSettings = {
    args: {
        freqBand: "868", // options: 868 or 433
        phyType868: "2gfsk50kbps", // phyType suffix must match freqBand
        carrierFrequency: 868.00,
        txPower: "14",
        codeExportConfig: {
            cmdList_prop: ["cmdPropRadioDivSetup"],
            cmdPropRadioDivSetup: "RF_cmdPropRadioDivSetup_fsk_50kbps"
        }
    }
};

// Object containing SimpleLink Long Range, 5kbps settings for LP_CC1354R10_RGZ
const devSpecificSlLr5KbpsSettings = {
    args: {
        freqBand: "868", // options: 868 or 433
        phyType868: "slr5kbps2gfsk", // phyType suffix must match freqBand
        carrierFrequency: 868.00,
        txPower: "14",
        codeExportConfig: {
            cmdList_prop: ["cmdPropRadioDivSetup"],
            cmdPropRadioDivSetup: "RF_cmdPropRadioDivSetup_sl_lr"
        }
    }
};

// Object containing 2GFSK, 200kbps settings for the LP_CC1354R10_RGZ
const devSpecific2Gfsk200KbpsSettings = {
    args: {
        freqBand: "868",
        phyType868: "2gfsk200kbps154g", // phyType sfx must match freqBand
        carrierFrequency: 868.00,
        txPower: "14",
        codeExportConfig: {
            cmdList_prop: ["cmdPropRadioDivSetup"],
            cmdPropRadioDivSetup: "RF_cmdPropRadioDivSetup_fsk_200kbps"
        }
    }
};

// Object containing 2GFSK, 250kbps settings for the LP_CC1354R10_RGZ
// Left empty because there are no overrides from the common settings
const devSpecific2Gfsk250Kbps24GHzSettings = {
    args: {
        codeExportConfig: {
            cmdList_prop: ["cmdPropRadioDivSetup"],
            cmdPropRadioDivSetup: "RF_cmdPropRadioDivSetup_2_4G_fsk_250kbps"
        }
    }
};

// Object containing 2GFSK, 100kbps settings for the LP_CC1354R10_RGZ
// Left empty because there are no overrides from the common settings
const devSpecific2Gfsk100Kbps24GHzSettings = {
    args: {
        codeExportConfig: {
            cmdList_prop: ["cmdPropRadioDivSetup"],
            cmdPropRadioDivSetup: "RF_cmdPropRadioDivSetup_2_4G_fsk_100kbps"
        }
    }
};

/*
 *  ======== Device Specific BLE PHY Settings ========
 *
 * These Objects will be used to extend the common BLE phy settings defined in
 * easylink_rf_config_common.js.
 *
 * These Objects must contain the following elements:
 *      - args: Phy setting arguments to be passed to the radio config module
 *          - codeExportConfig: Exported code names to be passed to
 *                              radioConfig.codeExportConfig
 *              - cmdBle5RadioSetup: Name of the generated
 *                      rfc_CMD_PROP_RADIO_DIV_SETUP_t command
 */

// Object containing BLE settings for the LP_CC1354R10_RGZ
const devSpecificBleSettings = {
    args: {
        codeExportConfig: {
            cmdList_ble: ["cmdBle5RadioSetup"],
            cmdBle5RadioSetup: "RF_ble_cmdRadioSetup"
        }
    }
};

/*
 *  ======== Arrays Containing all PHY Settings ========
 *
 * These Arrays will pull the common phy settings defined in
 * easylink_rf_config_common.js and merge them with the device specific phy
 * settings defined in this file.
 *
 * Note: The first element in the defaultPropPhyList with easyLinkOption defined
 *       will be the phy that EasyLink_Phy_Custom defaults to when added to a
 *       configuration
 */

// Array containing all the proprietary phy settings for the LP_CC1354R10_RGZ
const defaultPropPhyList = [
    rfCommon.mergeRfSettings(devSpecific2Gfsk50KbpsSettings,
        rfCommon.common2Gfsk50KbpsSettings),
    rfCommon.mergeRfSettings(devSpecificSlLr5KbpsSettings,
        rfCommon.commonSlLr5KbpsSettings),
    rfCommon.mergeRfSettings(devSpecific2Gfsk200KbpsSettings,
        rfCommon.common2Gfsk200KbpsSettings),
    rfCommon.mergeRfSettings(devSpecific2Gfsk100Kbps24GHzSettings,
        rfCommon.common2Gfsk100Kbps24GHzSettings)
];

// Array containing all the BLE phy settings for the LP_CC1354R10_RGZ
const defaultBlePhyList = [
    rfCommon.mergeRfSettings(devSpecificBleSettings, rfCommon.commonBleSettings)
];

// Reserved for future use
const default154PhyList = [];

exports = {
    defaultPropPhyList: defaultPropPhyList,
    defaultBlePhyList: defaultBlePhyList,
    default154PhyList: default154PhyList
};
