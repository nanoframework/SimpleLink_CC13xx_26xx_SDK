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
 *  ======== easylink_common.js ========
 */

"use strict";

// Max and min C type values
const maxUint32t = 0xFFFFFFFF;
const maxUint32tRatUsTime = Math.floor(maxUint32t / 4);
const maxUint32tRatMsTime = Math.floor(maxUint32t / 4000);
const maxInt8t = 127;
const minInt8t = -128;

// Dictionary mapping a device name to default LaunchPad; used to discover the
// appropriate RF settings when a device is being used without a LaunchPad
const deviceToBoard = {
    CC1352R: "CC1352R1_LAUNCHXL",
    CC1352P: "CC1352P1_LAUNCHXL",
    CC1312: "CC1312R1_LAUNCHXL",
    CC2640: "CC2640R2_LAUNCHXL",
    CC2650: "CC2650_LAUNCHXL",
    CC1350: "CC1350_LAUNCHXL",
    CC1310: "CC1310_LAUNCHXL"
};

/*
 * ======== isValidAddress ========
 * Checks that the address is hex format
 *
 * @param addr    - The address to be validated in string format
 * @returns bool     - True or false depending on if the string format is valid
 */
function isValidAddress(addr)
{
    let isValid = false;

    // Check if the address is in hex format
    if(addr.substring(0, 2) === "0x")
    {
        // Parse the address
        const parsedNum = Number(addr);

        // Check if the parsed number is valid
        if(!isNaN(parsedNum))
        {
            isValid = true;
        }
    }

    return(isValid);
}


/*!
 *  ======== underscoreToCamelCase ========
 *  Convert a string separated by underscores to a camelCase representation
 *
 *  @param underscoreCase  - a string separated by underscores with no spaces
 *
 *  @returns String - the corresponding camelCase representation
 */
function underscoreToCamelCase(underscoreCase)
{
    let camelCase = null;

    // Check the string is not empty and contains no spaces before parsing
    if(underscoreCase.length > 0 && !underscoreCase.includes(" "))
    {
        const splitString = underscoreCase.split("_"); // Split on underscore
        let i = 0;
        for(i = 0; i < splitString.length; i++)
        {
            // Convert first letter of each entry to upper case
            splitString[i] = splitString[i].charAt(0).toUpperCase()
                + splitString[i].slice(1).toLowerCase();
        }

        camelCase = splitString.join("");
    }

    return(camelCase);
}

/*!
 *  ======== device2DeviceFamily ========
 *  Map a pinmux deviceID to a TI-driver device family string
 *
 *  @param deviceId  - a pinmux deviceId (system.deviceData)
 *
 *  @returns String - the corresponding "DeviceFamily_xxxx" string
 *                    used by driverlib header files.
 */
function device2DeviceFamily(deviceId)
{
    let driverString = null;

    /* Determine libraries required by device name. */
    if(deviceId.match(/CC13.2/))
    {
        driverString = "DeviceFamily_CC13X2";
    }
    else if(deviceId.match(/CC13.0/))
    {
        driverString = "DeviceFamily_CC13X0";
    }
    else if(deviceId.match(/CC26.0R2/))
    {
        driverString = "DeviceFamily_CC26X0R2";
    }
    else if(deviceId.match(/CC26.2/))
    {
        driverString = "DeviceFamily_CC26X2";
    }
    else if(deviceId.match(/CC26.0/))
    {
        driverString = "DeviceFamily_CC26X0";
    }
    else if(deviceId.match(/CC3220/))
    {
        driverString = "DeviceFamily_CC3220";
    }
    else if(deviceId.match(/MSP432E.*/))
    {
        driverString = "DeviceFamily_MSP432E401Y";
    }
    else if(deviceId.match(/MSP432P4.1.I/)
            || deviceId.match(/MSP432P4111/))
    {
        driverString = "DeviceFamily_MSP432P4x1xI";
    }
    else if(deviceId.match(/MSP432P4.1.T/))
    {
        driverString = "DeviceFamily_MSP432P4x1xT";
    }
    else if(deviceId.match(/MSP432P401/))
    {
        driverString = "DeviceFamily_MSP432P401x";
    }
    else
    {
        driverString = "";
    }

    return(driverString);
}

/*!
 *  ======== isCName ========
 *  Determine if specified id is either empty or a valid C identifier
 *
 *  @param id  - String that may/may not be a valid C identifier
 *
 *  @returns true if id is a valid C identifier OR is the empty
 *           string; otherwise false.
 */
function isCName(id)
{
    if((id !== null && id.match(/^[a-zA-Z_][0-9a-zA-Z_]*$/) !== null)
            || id === "") /* '' is a special value that means "default" */
    {
        return true;
    }
    return false;
}

/*!
 *  ======== getBoardOrLaunchPadName ========
 *  Get the name of the board (or device)
 *
 *  @param convertToBoard - Boolean. When true, return the associated LaunchPad
 *                          name if a device is being used without a LaunchPad
 *
 *  @returns String - Name of the board with prefix /ti/boards and
 *                    suffix .syscfg.json stripped off.  If no board
 *                    was specified, the device name is returned.
 */
function getBoardOrLaunchPadName(convertToBoard)
{
    let name = system.deviceData.deviceId;

    if(system.deviceData.board != null)
    {
        name = system.deviceData.board.source;

        /* Strip off everything up to and including the last '/' */
        name = name.replace(/.*\//, "");

        /* Strip off everything after and including the first '.' */
        name = name.replace(/\..*/, "");
    }

    // Check if this is a standalone device without a LaunchPad
    if(convertToBoard && !name.includes("LAUNCHXL"))
    {
        // Find the LaunchPad name in deviceToBoard dictionary
        let key = null;
        for(key in deviceToBoard)
        {
            if(name.includes(key))
            {
                name = deviceToBoard[key];
                break;
            }
        }
    }

    return(name);
}

/*
 * ======== stringToArray ========
 * Convert a comma separated string to an array
 *
 * @param string       - The string to be converted
 * @returns Array      - An array containing the values separated from string
 *                       without whitespace characters
 */
function stringToArray(string)
{
    let splitString = [];

    // Ignore spaces in user input
    const stringNoSpaces = string.replace(/\s/g, "");

    // Check if the string is empty before parsing
    if(stringNoSpaces.length > 0)
    {
        splitString = stringNoSpaces.split(","); // Split based on comma
    }

    return(splitString);
}

/*
 * ======== addPropPhy ========
 * Adds an instance of a proprietary phy in the "Custom" stack module. Used in
 * RF Driver .syscfg files
 *
 * @param stackInst - Object. Instance of the "Custom" stack
 * @param phy       - Object. The phy containing args and codeExportConfig
 * @param isCustom  - Boolean. Denotes whether the phy added should be custom
 *
 * @returns - Object. The instance of the newly added phy
 */
function addPropPhy(stackInst, phy, isCustom)
{
    // Get the frequency band and phy type of the phy
    const freqBand = phy.args.freqBand;
    const phyType = phy.args["phyType" + freqBand];

    const blackList = ["codeExportConfig", "freqBand", "phyType" + freqBand];

    const phyName = isCustom ? "custom" + freqBand : phyType;

    // Add the instance of the new PHY
    const selectedPhyOptions = stackInst["prop" + freqBand[0]].slice(0);
    selectedPhyOptions.push(phyName);
    stackInst["prop" + freqBand[0]] = selectedPhyOptions;

    // Get the instance of the newly added PHY
    const phyInst = stackInst["radioConfig" + phyName];

    // Set RF settings of the phy instance to match RF Driver project defaults
    let key = null;
    for(key in phy.args)
    {
        if(!blackList.includes(key) || (isCustom && key.includes("phyType")))
        {
            phyInst[key] = phy.args[key];
        }
    }

    // Set code export settings of phy inst to match RF Driver project defaults
    for(key in phy.args.codeExportConfig)
    {
        if(Object.prototype.hasOwnProperty.call(phy.args.codeExportConfig, key))
        {
            phyInst.codeExportConfig[key] = phy.args.codeExportConfig[key];
        }
    }

    // If Custom, remove rx adv command and replace with a standard rx command
    if(isCustom)
    {
        phyInst.codeExportConfig.symGenMethod = "Legacy";

        const exportedCommands = phyInst.codeExportConfig.cmdList_prop.slice(0);
        const index = exportedCommands.indexOf("cmdPropRxAdv");

        if(index > -1)
        {
            exportedCommands.splice(index, 1, "cmdPropRx");
        }
        else if(!exportedCommands.includes("cmdPropRx"))
        {
            exportedCommands.push("cmdPropRx");
        }

        phyInst.codeExportConfig.cmdList_prop = exportedCommands;
    }

    return(phyInst);
}

/*
 * ======== addBlePhy ========
 * Adds an instance of a ble phy in the "Custom" stack module. Used in
 * RF Driver .syscfg files
 *
 * @param stackInst - Object. Instance of the "Custom" stack
 * @param phy       - Object. The phy containing args and codeExportConfig
 *
 * @returns - Object. The instance of the newly added phy
 */
function addBlePhy(stackInst, phy)
{
    // Add the instance of the new PHY
    const selectedPhyOptions = stackInst.ble.slice(0);
    selectedPhyOptions.push(phy.args.phyType);
    stackInst.ble = selectedPhyOptions;

    const blackList = ["codeExportConfig", "phyType"];

    // Get the instance of the newly added PHY
    const phyInst = stackInst["radioConfig" + phy.args.phyType];

    // Set RF settings of the phy instance to match RF Driver project defaults
    let key = null;
    for(key in phy.args)
    {
        if(!blackList.includes(key))
        {
            phyInst[key] = phy.args[key];
        }
    }

    // Set code export settings of phy inst to match RF Driver project defaults
    for(key in phy.args.codeExportConfig)
    {
        if(Object.prototype.hasOwnProperty.call(phy.args.codeExportConfig, key))
        {
            phyInst.codeExportConfig[key] = phy.args.codeExportConfig[key];
        }
    }

    return(phyInst);
}

exports = {
    isValidAddress: isValidAddress,
    device2DeviceFamily: device2DeviceFamily,
    isCName: isCName,
    getBoardOrLaunchPadName: getBoardOrLaunchPadName,
    underscoreToCamelCase: underscoreToCamelCase,
    stringToArray: stringToArray,
    addPropPhy: addPropPhy,
    addBlePhy: addBlePhy,
    maxUint32t: maxUint32t,
    maxUint32tRatUsTime: maxUint32tRatUsTime,
    maxUint32tRatMsTime: maxUint32tRatMsTime,
    maxInt8t: maxInt8t,
    minInt8t: minInt8t
};
