/*
 * Copyright (c) 2018-2019, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== GPIO.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common     = system.getScript("/ti/drivers/Common.js");
let logError   = Common.logError;
let logWarning = Common.logWarning;

/* compute /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "GPIO");

let intPriority = Common.newIntPri()[0];
intPriority.name = "interruptPriority";
intPriority.displayName = "Interrupt Priority";

/* generic configuration parameters for GPIO instances */
let config = [
    {
        name: "mode",
        displayName: "Mode",
        description: "Select the GPIO mode",
        longDescription: "The mode configuration parameter is used to"
            + " determine the initial state of GPIO, eliminating the need to"
            + " configure the GPIO pin at runtime prior to using it."
            + "\n[More ...](/tidrivers/syscfg/html/ConfigDoc.html"
            + "#ti_drivers_GPIO_mode \"Full descriptions of all GPIO modes\""
            + ")",

        default: "Input",
        options: [
            {
                name: "Input",
                description: "This GPIO is always used for input"
            },
            {
                name: "Output" ,
                description: "This GPIO is always used for output"
            },
            {
                name: "Dynamic",
                description: "This GPIO is dynamically reconfigured at"
                  + " runtime using the GPIO APIs",
                longDescription: "This mode indicates that the"
                  + " application will dynamically configure the GPIO at"
                  + " runtime using the GPIO APIs. As a result, it's assumed"
                  + " that this GPIO requires an entry in the table of GPIO"
                  + " callback functions; see [Optimize Callback Table Size]"
                  + "(/tidrivers/syscfg/html/ConfigDoc.html#"
                  + "ti_drivers_GPIO_optimizeCallbackTableSize)."
            }
        ],
        onChange: updateConfigs
    },
    {
        name: "nullEntry",
        description: "For internal use only.",
        hidden: true,
        default: false,
        onChange: updateNullEntry
    },
    {
        name: "outputType",
        displayName: "Output Type",
        hidden: true,
        default: "Standard",
        options: [
            { name: "Standard" },
            { name: "Open Drain" }
        ]
    },
    {
        name: "outputStrength",
        displayName: "Output Strength",
        hidden: true,
        default: "Medium",
        options: [
            { name: "High" },
            { name: "Medium" },
            { name: "Low" }
        ]
    },
    {
        name: "initialOutputState",
        displayName: "Initial Output State",
        hidden: true,
        default: "Low",
        options: [
            { name: "High" },
            { name: "Low" }
        ]
    },
    {
        name: "pull",
        displayName: "Pull",
        description: "Specifies the internal pull-up or pull-down resistor"
            + " configuration of this GPIO pin.",
        hidden: false,
        default: "None",
        options: [
            { name: "None" },
            { name: "Pull Up" },
            { name: "Pull Down" }
        ]
    },
    {
        name: "interruptTrigger",
        displayName: "Interrupt Trigger",
        description: "Specifies when or if interrupts are triggered",
        hidden: false,
        default: "None",
        options: [
            { name: "None" },
            { name: "High" },
            { name: "Low" },
            { name: "Falling Edge" },
            { name: "Rising Edge" },
            { name: "Both Edges" }
        ]
    },
    {
        name: "callbackFunction",
        hidden: false,
        displayName: "Callback Function",
        description: "The name of the callback function called when this GPIO pin triggers an interrupt, or 'NULL' if it's specified at runtime",
        longDescription: `
If you need to set the callback at runtime, set this configuration parameter
to 'NULL' and call [\`GPIO_setCallback()\`](/tidrivers/doxygen/html/_g_p_i_o_8h.html#a24c401f32e65f60f11a1594fdafb9d2a) with the name of the function you
want to be triggered.

If this config is not set, GPIO assumes that no callback will ever be set for
this GPIO and, as a result, no space will be allocated to save the address of
the callback; thus, it's important to never use \`GPIO_setCallback()\` on a
GPIO for which this config is empty.

[More ...](/tidrivers/syscfg/html/ConfigDoc.html#ti_drivers_GPIO_callbackFunction "Function's type signature and an example")
`,
        documentation: `
This function is of type [\`GPIO_CallbackFxn\`](/tidrivers/doxygen/html/_g_p_i_o_8h.html#a46b0c9afbe998c88539abc92082a1173),
it's called in the context of a hardware ISR, and it's passed
a single parameter: the index of the GPIO that triggered the interrupt.

Example: [Creating an input callback](/tidrivers/doxygen/html/_g_p_i_o_8h.html#ti_drivers_GPIO_Example_callback "C/C++ source").
`,

        placeholder: "<no callback is needed>",
        default: "",

        onChange: updateConfigs
    },
    {
        name: "comment",
        displayName: "Comment",
        hidden: true,
        default: "",
        description: "User specified comment to add the generated files."
    }
];

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    if (inst.nullEntry == true) {
        return ([]);
    }

    let gpio = {
        name: "gpioPin",
        displayName: "GPIO Pin",
        interfaceName: "GPIO",
        signalTypes: ["DIN", "DOUT"]
    };

    return ([gpio]);
}

/*
 *  ========= filterHardware ========
 */
function filterHardware(component)
{
    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["DIN", "DOUT"])) {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== sort ========
 *  Reorder GPIO instances to minimize space required for callbacks
 *
 *  @param instances - GPIO.$instances array
 *  @returns array of instances such that all GPIOs with interrupts
 *   appear ahead of GPIOs that can't trigger an interrupt.
 */
function sort(instances, addCallback)
{
    if (instances.length == 0
        || instances[0].$module.$static.enableSort == false) {
        return (instances);
    }

    let callbackInstances = [];
    let otherInstances = [];

    for (let i = 0; i < instances.length; i++) {
        let inst = instances[i];
        if (addCallback[inst.$name] == true) {
            callbackInstances[callbackInstances.length] = inst;
        } else {
            otherInstances[otherInstances.length] = inst;
        }
    }

    return (callbackInstances.concat(otherInstances));
}

/*
 *  ======== updateConfigs ========
 *  Adjust UI properties of configs based on current config settings
 */
function updateConfigs(inst, ui)
{
    switch (inst.mode) {
        case "Output":
            {
                ui.callbackFunction.hidden = true;
                ui.outputType.hidden = false;
                ui.initialOutputState.hidden = false;
                if (inst.outputType == "Standard") {
                    ui.pull.hidden = true;
                    ui.outputStrength.hidden = false;
                } else {
                    ui.pull.hidden = false;
                    ui.outputStrength.hidden = true;
                }
                ui.pull.hidden = false;
                ui.interruptTrigger.hidden = true;
                inst.interruptTrigger = "None";
                inst.callbackFunction = "";
                break;
            }
        case "Input":
            {
                ui.outputType.hidden = true;
                ui.outputStrength.hidden = true;
                ui.initialOutputState.hidden = true;
                ui.pull.hidden = false;
                ui.interruptTrigger.hidden = false;
                ui.callbackFunction.hidden = false;
                break;
            }
        case "Dynamic":
            {
                ui.outputType.hidden = true;
                ui.outputStrength.hidden = true;
                ui.initialOutputState.hidden = true;
                ui.pull.hidden = true;
                ui.callbackFunction.hidden = false;
                ui.interruptTrigger.hidden = true;
                break;
            }
    }
}

/*
 *  ======== updateNullEntry ========
 */
function updateNullEntry(inst, ui)
{
    if (inst.nullEntry == true) {
        inst.interruptTrigger = "None";
        inst.callbackFunction = "";
    }
}

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  @param inst       - GPIO instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if (inst.mode === "Output") {
        /* output GPIOs don't trigger interrupts */
        if (inst.interruptTrigger !== "None") {
            logError(validation, inst, "interruptTrigger",
                "Output mode GPIO resources can't trigger interrupts");
        }

        /* specified callbackFunction is never called for output only GPIOs */
        if (inst.callbackFunction.length > 0) {
            let message = "callback '" + inst.callbackFunction
                + "' set, but won't be triggered by this output-only GPIO";

            logError(validation, inst, "callbackFunction", message);
        }
    }

    /* callbackFunction must be valid C identifier */
    if (!Common.isCName(inst.callbackFunction)) {
        let message = "'" + inst.callbackFunction
            + "' is not a valid a C identifier";
        logError(validation, inst, "callbackFunction", message);
    }

    /* TODO: if not defined should we plug-in NOP and remark? */
    if ((inst.interruptTrigger !== "None")
        && (inst.callbackFunction === "")
        && (inst.mode !== "Dynamic")) {
        logWarning(validation, inst, "callbackFunction",
            "Callback must not be NULL or the 'mode' must be set to 'Dynamic'"
                + " (not " + inst.mode + ")");
    }

    if (inst.callbackFunction !== "NULL"
        && (inst.callbackFunction.toLowerCase() === "null")) {
        logWarning(validation, inst, "callbackFunction",
            "Did you mean \"NULL\"?");
    }
}

/*
 *  ======== getPinNum ========
 *  Translate GPIO name ("Px.y") into corresponding device pin number.
 */
function getPinNum(gpioName)
{
    for (let x in system.deviceData.devicePins) {
        if (system.deviceData.devicePins[x].description.match(gpioName)) {
            return (String(system.deviceData.devicePins[x].ball));
        }
    }

    return ("");
}

/*
 *  ======== getFullPinName ========
 *  Translate GPIO name ("Px.y") into corresponding description.
 */
function getFullPinName(gpioName)
{
    for (let x in system.deviceData.devicePins) {
        if (system.deviceData.devicePins[x].description.match(gpioName)) {
            return (String(system.deviceData.devicePins[x].description));
        }
    }

    return ("");
}

/*
 *  ======== getPinName ========
 *  Translate device pin number into GPIO name ("Px.y")
 */
function getPinName(pinNum)
{
    if (isNaN(pinNum)) {
        throw Error("'" + pinNum + "' is not a pin number");
    }
    for (let x in system.deviceData.devicePins) {
        if (system.deviceData.devicePins[x].ball == pinNum) {
            let desc = String(system.deviceData.devicePins[x].description);
            return (desc.substr(0, desc.indexOf("G")-1));
        }
    }

    return ("");
}

/*
 *  ======== onHardwareChanged ========
 */
function onHardwareChanged(inst, ui)
{
    let compatible = false;

    if (inst.$hardware) {
        let key = Object.keys(inst.$hardware.signals)[0];
        let signal = inst.$hardware.signals[key];
        let type = signal.type;

        if (Common.typeMatches(type, ["DOUT"])) {
            if (Common.typeMatches(type, ["DIN"])) {
                inst.mode = "Dynamic";
            }
            else {
                inst.mode = "Output";
                Common.setDefaults(inst, signal, "DOUT");
            }
            compatible = true;
        }
        else if (Common.typeMatches(type, ["DIN"])) {
            inst.mode = "Input";
            inst.callbackFunction = "NULL";
            Common.setDefaults(inst, signal, "DIN");
            compatible = true;
        }
    }

    if (compatible) {
        updateConfigs(inst, ui);
    }
}

/*
 *  ======== getAttrs ========
 *  Return a symbolic GPIO bit mask corresponding to inst's configs
 */
function getAttrs(inst)
{
    if (inst.mode == "Dynamic" || inst.nullEntry == true) {
        return ("GPIO_DO_NOT_CONFIG");
    }

    if (inst.mode == "Output") {
        let output = "";
        if (inst.outputType == "Standard") {
            output += "GPIO_CFG_OUT_STD";
            switch (inst.outputStrength) {
                case "High": {
                    output += " | GPIO_CFG_OUT_STR_HIGH";
                    break;
                }
                case "Medium": {
                    output += " | GPIO_CFG_OUT_STR_MED";
                    break;
                }
                case "Low":
                default: {
                    output += " | GPIO_CFG_OUT_STR_LOW";
                    break;
                }
            }
        }
        else {
            switch (inst.pull) {
                case "Pull Up": {
                    output = "GPIO_CFG_OUT_OD_PU";
                    break;
                }
                case "Pull Down": {
                    output = "GPIO_CFG_OUT_OD_PD";
                    break;
                }
                case "None":
                default: {
                    output = "GPIO_CFG_OUT_OD_NOPULL";
                    break;
                }
            }
        }
        if (inst.initialOutputState == "High") {
            output += " | GPIO_CFG_OUT_HIGH";
        }
        else {
            output += " | GPIO_CFG_OUT_LOW";
        }
        return (output);
    }
    else {
        let input = "";
        switch (inst.pull) {
            case "Pull Up": {
                input = "GPIO_CFG_IN_PU";
                break;
            }
            case "Pull Down": {
                input = "GPIO_CFG_IN_PD";
                break;
            }
            case "None":
            default: {
                input = "GPIO_CFG_IN_NOPULL";
                break;
            }
        }

        if (inst.interruptTrigger != "None") {
            switch (inst.interruptTrigger) {
                case "Falling Edge": {
                    input += " | GPIO_CFG_IN_INT_FALLING";
                    break;
                }
                case "Rising Edge": {
                    input += " | GPIO_CFG_IN_INT_RISING";
                    break;
                }
                case "High": {
                    input += " | GPIO_CFG_IN_INT_HIGH";
                    break;
                }
                case "Low": {
                    input += " | GPIO_CFG_IN_INT_LOW";
                    break;
                }
                case "Both Edges": {
                    input += " | GPIO_CFG_IN_INT_BOTH_EDGES";
                    break;
                }
            }
        }
        else {
            input += " | GPIO_CFG_IN_INT_NONE";
        }
        return (input);
    }
}

/*
 *  ======== collectCallbacks ========
 */
function collectCallbacks(instances, externs, addCallback)
{
    let callbacks = {};
    let callbackCount = 0;
    let sortDisabled = system.getScript("/ti/drivers/GPIO").$static.enableSort == false;

    for (let i = 0; i < instances.length; i++) {
        let inst = instances[i];

        if (sortDisabled ||
            (inst.callbackFunction != "") ||
            (inst.interruptTrigger != "None") ||
            (inst.mode === "Dynamic") ||
            (inst.nullEntry == true)) {
                addCallback[inst.$name] = true;
                callbackCount++;
        }
        else {
            addCallback[inst.$name] = false;
        }

        if (inst.callbackFunction != "" && inst.callbackFunction !== "NULL") {
            callbacks[inst.callbackFunction] = 1;
        }
    }

    for (let name in callbacks) {
        externs.push("extern void " + name + "(uint_least8_t index);");
    }

    return (callbackCount);
}

/*
 *  ======== addComment ========
 *
 *  %p = Px.y extracted from $solution.peripheralPinName
 *  %P = entire pin name from $solution.devicePinName
 *  %i = GPIO config array index
 *  %c = C Name
 *  %n = $name
 *  %l = comment text is inserted a line before the config line.
 *  %tnum = comment is placed at character position - num.
 */
function addComment(line, inst, index, pin)
{
    let padding = Array(80).join(' ');
    let comment = inst.comment;
    if (inst.comment === "") {
        if (inst.$hardware && inst.$hardware.displayName) {
            comment = "    /* " + inst.$name + " : " + inst.$hardware.displayName + " */\n";
        }
        else {
            comment = "    /* " + inst.$name + " */\n";
        }
        return (comment + line);
    }
    if (comment.match("%t")) {
        let tab = comment.substring(comment.indexOf("%t") + 2);
        let ts = parseInt(tab);
        comment = comment.replace("%t"+ts, "");
        comment = padding.substring(0, ts - line.length) + comment;
    }
    if (comment.match("%c")) {
        comment = comment.replace("%c",  inst.$name);
    }
    if (comment.match("%n")) {
        comment = comment.replace("%n",  inst.$name);
    }
    if (comment.match("%i")) {
        comment = comment.replace("%i", index);
    }
    if (comment.match("%p")) {
        if (inst.nullEntry == true) {
            comment = comment.replace("%p", "Empty Pin");
        }
        else {
            let pName = pin.$solution.peripheralPinName;
            comment = comment.replace("%p", pName);
        }
    }
    if (comment.match("%P")) {
        if (inst.nullEntry == true) {
            comment = comment.replace("%P", "Empty Pin");
        }
        else {
            comment = comment.replace("%P",  pin.$solution.devicePinName);
        }
    }
    if (comment.match("%l")) {
        comment = comment.replace("%l", "");
        line = comment + "\n" + line;
        return (line);
    }
    return (line + comment);
}

/*
 *  ======== base ========
 *  Define the base/common GPIO property and method exports
 */
let base = {
    displayName: "GPIO",
    description: "General Purpose I/O Driver",

    longDescription: `
The [__GPIO driver__][1] allows you to manage General Purpose I/O
resources via simple and portable APIs. GPIO pin behavior is
configured statically, but can also be [reconfigured at runtime][2].

* [Usage Synopsis][3]
* [Examples][4]
* [Configuration Options][5]

[1]: /tidrivers/doxygen/html/_g_p_i_o_8h.html#details "C API reference"
[2]: /tidrivers/doxygen/html/_g_p_i_o_8h.html#ti_drivers_GPIO_Example_reconfigure "Example: Reconfiguring a GPIO pin"
[3]: /tidrivers/doxygen/html/_g_p_i_o_8h.html#ti_drivers_GPIO_Synopsis "Basic C usage summary"
[4]: /tidrivers/doxygen/html/_g_p_i_o_8h.html#ti_drivers_GPIO_Examples "C usage examples"
[5]: /tidrivers/syscfg/html/ConfigDoc.html#GPIO_Configuration_Options "Configuration options reference"
`,

    pinmuxRequirements: pinmuxRequirements,
    validate: validate,

    defaultInstanceName: "Board_GPIO",
    config: Common.addNameConfig(config, "/ti/drivers/GPIO", "Board_GPIO"),

    moduleStatic: {
        name: "gpioGlobal",
        displayName: "GPIO Global",
        config: [
            intPriority,
            {
                name: "optimizeCallbackTableSize",
                displayName: "Optimize Callback Table Size",

                description: "Enabling this option removes unnecessary"
                    + " entries in the GPIO callback table."
                    + " Before enabling this option, carefully review the"
                    + " detailed help for runtime benefits AND caveats.",

                longDescription: "This option causes the GPIO driver tables"
                    + " to be sorted so that all GPIOs that trigger interrupts"
                    + " appear first in the GPIO table. This allows the table"
                    + " of GPIO callbacks, which is also indexed by the GPIO"
                    + " index, to be substantially shorter then the table of"
                    + " all GPIOs."
                    + "\n\n"
                    + "However, this also means that calls to"
                    + " `GPIO_setCallback()` must **never** be made to GPIOs"
                    + " whose callbacks are configured as the empty string"
                    + " (which is shown as '&lt;no callback is"
                    + " needed&gt;' in the GUI).",

                hidden: false,
                default: false
            }
        ],
        getPinNum: getPinNum,
        getPinName: getPinName,
        getFullPinName: getFullPinName,
        getAttrs: getAttrs,
        collectCallbacks: collectCallbacks,
        addComment: addComment,
        modules: Common.autoForceModules(["Board", "Power"])
    },

    /* common sort for GPIO tables to minimize GPIO ISR table size */
    sort: sort,
    filterHardware: filterHardware,
    onHardwareChanged: onHardwareChanged
};

/* extend our common exports to include the family-specific content */
let deviceGPIO = system.getScript("/ti/drivers/gpio/GPIO" + family);
exports = deviceGPIO.extend(base);
