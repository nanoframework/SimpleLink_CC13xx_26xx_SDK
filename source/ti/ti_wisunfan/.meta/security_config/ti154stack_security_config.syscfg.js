/*
 * Copyright (c) 2017-2018 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ti154stack_security_config.syscfg.js ========
 */

"use strict";

// Get common utility functions
const Common = system.getScript("/ti/ti_wisunfan/ti154stack_common.js");

// Get security setting descriptions
const Docs = system.getScript("/ti/ti_wisunfan/security_config/"
    + "ti154stack_security_config_docs.js");

// Configurables for the static 15.4 security settings group
const config = {
    displayName: "Security",
    description: "Configure settings for network security",
    config: [
        {
            name: "secureLevel",
            displayName: "Security Level",
            default: "macSecure",
            options: [
                {
                    name: "macSecureDisabled",
                    displayName: "Disabled"
                },
                {
                    name: "macSecure",
                    displayName: "Enabled"
                }
            ],
            description: Docs.secureLevel.description,
            longDescription: Docs.secureLevel.longDescription,
            onChange: onSecureLevelChange
        },
        {
            name: "keyTableDefaultKey",
            displayName: "Pre-Shared Network Key",
            default: "123456789ABCDEF00000000000000000",
            description: Docs.keyTableDefaultKey.description,
            longDescription: Docs.keyTableDefaultKey.longDescription
        }
    ]
};

/*
 *******************************************************************************
 Security Group Config Functions
 *******************************************************************************
 */

/*
 * ======== onSecureLevelChange ========
 * On change function for secureLevel config
 * Updates visibility of dependent configs based on security level selected
 *
 * @param inst - 15.4 instance
 * @param ui   - user interface object
 */
function onSecureLevelChange(inst, ui)
{
    // Set visibility of dependent configs
    setSecurityConfigHiddenState(inst, ui, "keyTableDefaultKey");
}

/*
 * ======== setSecureLevelReadOnlyState ========
 * Sets the read only status of secure level config. If config is read only, a
 * read only reason is displayed
 *
 * @param ui       - user interface object
 * @param readOnly - true if config must be set to read only
 */
function setSecureLevelReadOnlyState(ui, readOnly)
{
    // Set read only state of config
    ui.secureLevel.readOnly = (readOnly) ? Docs.secureLevel.readOnly : false;
}

/*
 * ======== getSecurityConfigHiddenState ========
 * Get the expected visibility of the selected config
 *
 * @param inst    - module instance containing the config that changed
 * @param cfgName - name of config
 * @returns bool  - true if hidden, false if visible
 */
function getSecurityConfigHiddenState(inst, cfgName)
{
    let isVisible = true;

    switch(cfgName)
    {
        case "keyTableDefaultKey":
        {
            isVisible = (inst.secureLevel !== "macSecureDisabled");
            break;
        }
        case "secureLevel":
        default:
        {
            isVisible = true;
            break;
        }
    }

    // Return whether config is hidden
    return(!isVisible);
}

/*
 * ======== setSecurityConfigHiddenState ========
 * Sets the visibility of the selected config
 *
 * @param inst    - module instance containing the config that changed
 * @param ui      - user interface object
 * @param cfgName - name of config
 */
function setSecurityConfigHiddenState(inst, ui, cfgName)
{
    // Set visibility of config
    ui[cfgName].hidden = getSecurityConfigHiddenState(inst, cfgName);
    if(ui[cfgName].hidden)
    {
        // get a list of all nested and unnested configs
        const configToReset = Common.findConfig(config.config, cfgName);
        // restore the default value for the hidden parameter.
        Common.restoreDefaultValue(inst, configToReset, cfgName);
    }
}

/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - settings instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    const isSecurityDisabled = inst.secureLevel === "macSecure";

    // Linked Library Message - add info messages regarding linked library
    if(isSecurityDisabled)
    {
        validation.logInfo("Ensure that the maclib_nosecure library is linked",
            inst, "secureLevel");
    }
    else
    {
        validation.logInfo("Ensure that the maclib_secure library is linked",
            inst, "secureLevel");
    }

    // Network Security Key - verify 32 hexadecimal digits entered
    const actualNumDigits = inst.keyTableDefaultKey.length;

    // Verify 32 digits entered if security is enabled
    if(!isSecurityDisabled)
    {
        if(actualNumDigits !== 32)
        {
            validation.logError("Must be 32 hex digits. Currently "
                + actualNumDigits + " digits", inst, "keyTableDefaultKey");
        }

        // Verify all digits are hexadecimal
        const hexOnly = _.every(inst.keyTableDefaultKey,
            (digit) => /^[a-fA-F0-9]$/.test(digit));
        if(!hexOnly)
        {
            validation.logError("Must be hex digits only", inst,
                "keyTableDefaultKey");
        }
    }

}

/*
 *******************************************************************************
 Module Dependencies and Exports
 *******************************************************************************
 */

/*
 *  ======== moduleInstances ========
 *  Determines what modules are added as non-static sub-modules
 *
 *  @param inst  - Module instance containing the config that changed
 *  @returns     - Array containing dependency modules
 */
function moduleInstances(inst)
{
    const dependencyModule = [];

    if(inst.secureLevel !== "macSecureDisabled")
    {
        dependencyModule.push({
            name: "aesccmModule",
            displayName: "AESCCM",
            description: "Settings for MAC security",
            moduleName: "/ti/drivers/AESCCM",
            hidden: true
        });

        dependencyModule.push({
            name: "aesecbModule",
            displayName: "AESECB",
            description: "Settings for MAC security",
            moduleName: "/ti/drivers/AESECB",
            hidden: true
        });

        dependencyModule.push({
            name: "aescbcModule",
            displayName: "AESCBC",
            description: "Settings for MAC security",
            moduleName: "/ti/drivers/AESCBC",
            hidden: true
        });


    }

    return(dependencyModule);
}

// Exports to the top level 15.4 module
exports = {
    config: config,
    validate: validate,
    moduleInstances: moduleInstances,
    setSecureLevelReadOnlyState: setSecureLevelReadOnlyState,
    setSecurityConfigHiddenState: setSecurityConfigHiddenState,
    getSecurityConfigHiddenState: getSecurityConfigHiddenState
};
