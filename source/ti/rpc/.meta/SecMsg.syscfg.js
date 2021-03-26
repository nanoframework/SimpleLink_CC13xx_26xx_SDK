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
 */

/*
 *  ======== SecMsg.syscfg.js ========
 *  SecMsg configuration support
 */

"use strict";

/*
 *  ======== pinmuxRequirements ========
 */
function pinmuxRequirements(inst)
{
    let modules = new Array();
    return (modules);
}

/*
 *  ======== config ========
 *  Define the config params of the module instance
 */
let config = [
    {
        name: "msgSize",
        displayName: "Message size (bytes)",
        default: 64
    },
    {
        name: "msgCount",
        displayName: "Number of messages in the pool",
        default: 4,
        options: [              /* must be power-of-2 */
            { name: 4 },
            { name: 8 },
            { name: 16 }
        ]
    }
];

/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "SecMsg",
    description: "SecMsg module",
    config: config,
    defaultInstanceName: "Config_SecMsg",
    pinmuxRequirements: pinmuxRequirements,

    templates: {
        "/ti/rpc/Config.c.xdt": "/ti/rpc/SecMsg.Config.c.xdt",
        "/ti/rpc/Config.h.xdt": "/ti/rpc/SecMsg.Config.h.xdt"
    }
};

/* export the module */
exports = base;