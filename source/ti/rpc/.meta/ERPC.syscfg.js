/*
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ERPC.syscfg.js ========
 *  EmbeddedRPC configuration support
 */

"use strict";

/*
 *  ======== moduleDescription ========
 */
let moduleDescription =
    "Configure EmbeddedRPC instances. Each instance corresponds to one IDL " +
    "description, and creates one pair of Client Manager and RPC Server " +
    "objects. The configuration generates setup code which must be invoked " +
    "at run-time. Inspect the generated file `ti_rpc_erpc.h` to see the names " +
    "of the generated functions.";

/*
 *  ======== config ========
 *  Define config params for the module instance
 */
let config = [
    {
        name: "role",
        displayName: "RPC Role",
        default: "client",
        options: [
            { name: "client", displayName: "Client" },
            { name: "server", displayName: "Server"  }
        ]
    },
    {
        name: "namespace",
        displayName: "Namespace",
        default: ""
    },
    {
        name: "idl",
        displayName: "IDL Name",
        default: "IDL"
    },
    {
        name: "hasCbk",
        displayName: "Enable callback support",
        default: false
    },
    {
        name: "interfaceArgs",
        default: "",
        hidden: true
    },
    {
        name: "transport",
        displayName: "Transport",
        default: "uart",
        onChange: transport_onChange,
        getDisabledOptions: transport_disabledOptions,
        options: [
//          { name: "pcl",      displayName: "PCL"  },
            { name: "serial",   displayName: "Serial" },
            { name: "uart",     displayName: "UART" }
        ]
    },
    {
        name: "sdev",
        displayName: "Device Pathname",
        default: "",
        hidden: true
    }
];

/*
 *  ======== transport_onChange ========
 */
function transport_onChange(inst, ui)
{
    if (inst.transport == "serial") {
        ui.sdev.hidden = false
    }
    else {
        ui.sdev.hidden = true
    }
}

/*
 *  ======== transport_disabledOptions ========
 */
function transport_disabledOptions(inst)
{
    let rtos = system.modules["/ti/utils/RTOS"];
    let dopts = new Array();

    /* this function is irrelevant if rtos not loaded */
    if (rtos == undefined) {
        return (dopts);
    }

    if (rtos.$static.name == "Linux") {
        dopts.push({
            name: "uart",
            reason: "Not available for current RTOS selection"
        });
    }
    else {
        dopts.push({
            name: "serial",
            reason: "Not available for current RTOS selection"
        });
    }

    return (dopts);
}

/*
 *  ======== moduleInstances ========
 *  Express dependencies for private instances of other modules
 *
 *  This function is invoked on any configuration change to the given
 *  instance.
 */
function moduleInstances(inst)
{
    let modules = new Array();
    let name = "";

    if (inst.interfaceArgs != "") {
        let args = JSON.parse(inst.interfaceArgs);

        for (let i = 0; i < args.length; i++) {
            modules.push({
                moduleName: "/ti/rpc/Interface",
                name: "interface_" + (i+1),
                displayName: "Interface: " + args[i].ifn,
                args: args[i],
                collapsed: true
            });
        }

    }
    else {
        modules.push({
            moduleName: "/ti/rpc/Interface",
            name: "interfaces",
            displayName: "Interface",
            useArray: true,
            collapsed: true
        });
    }

    if (inst.transport == "pcl") {
        if (inst.role == "client") {
            /* client api send queue */
            name = [inst.idl, inst.role, "api_sendq"].join("_");
            modules.push({
                moduleName: "/ti/pcl/MsgQue",
                name: "api_sendq",
                displayName: "MsgQue: API send",
                args: {
                    $name: "CONFIG_MsgQue_" + name,
                    role: "owner",
                    mode: "writer"
                }
            });

            /* client api reply queue */
            name = [inst.idl, inst.role, "api_rplyq"].join("_");
            modules.push({
                moduleName: "/ti/pcl/MsgQue",
                name: "api_rplyq",
                displayName: "MsgQue: API reply",
                args: {
                    $name: "CONFIG_MsgQue_" + name,
                    role: "owner",
                    mode: "reader"
                }
            });

            if (inst.hasCbk) {
                /* client cbk receive queue */
                name = [inst.idl, inst.role, "cbk_recvq"].join("_");
                modules.push({
                    moduleName: "/ti/pcl/MsgQue",
                    name: "cbk_recvq",
                    displayName: "MsgQue: Callback receive",
                    args: {
                        $name: "CONFIG_MsgQue_" + name,
                        role: "owner",
                        mode: "reader"
                    }
                });

                /* client cbk return queue */
                name = [inst.idl, inst.role, "cbk_retnq"].join("_");
                modules.push({
                    moduleName: "/ti/pcl/MsgQue",
                    name: "cbk_retnq",
                    displayName: "MsgQue: Callback return",
                    args: {
                        $name: "CONFIG_MsgQue_" + name,
                        role: "owner",
                        mode: "writer"
                    }
                });
            }
        }
        else {
            /* server api receive queue */
            name = [inst.idl, inst.role, "api_recvq"].join("_");
            modules.push({
                moduleName: "/ti/pcl/MsgQue",
                name: "api_recvq",
                displayName: "MsgQue: API receive",
                args: {
                    $name: "CONFIG_MsgQue_" + name,
                    role: "user",
                    mode: "reader"
                }
            });

            /* server api return queue */
            name = [inst.idl, inst.role, "api_retnq"].join("_");
            modules.push({
                moduleName: "/ti/pcl/MsgQue",
                name: "api_retnq",
                displayName: "MsgQue: API return",
                args: {
                    $name: "CONFIG_MsgQue_" + name,
                    role: "user",
                    mode: "writer"
                }
            });

            if (inst.hasCbk) {
                /* server cbk send queue */
                name = [inst.idl, inst.role, "cbk_sendq"].join("_");
                modules.push({
                    moduleName: "/ti/pcl/MsgQue",
                    name: "cbk_sendq",
                    displayName: "MsgQue: Callback send",
                    args: {
                        $name: "CONFIG_MsgQue_" + name,
                        role: "user"
                    }
                });

                /* server cbk reply queue */
                name = [inst.idl, inst.role, "cbk_rplyq"].join("_");
                modules.push({
                    moduleName: "/ti/pcl/MsgQue",
                    name: "cbk_rplyq",
                    displayName: "MsgQue: Callback reply",
                    args: {
                        $name: "CONFIG_MsgQue_" + name,
                        role: "user",
                        mode: "reader"
                    }
                });
            }
        }
    }
    else if (inst.transport == "uart") {
        /*  When running on Linux, UART driver is not available. Until
         *  SysConfig supports platform specific default values, make
         *  sure that uart is available before expressing a requirement
         *  for it. Otherwise, on Linux this default will cause SysConfig
         *  to crash. PMUX-2407
         */
        try {
            let uart = system.getScript("/ti/drivers/UART.syscfg.js");
            if (uart != undefined) {
                modules.push({
                    name: "uart",
                    displayName: "UART transport",
                    moduleName: "/ti/drivers/UART"
                });
            }
        }
        catch (e) {
            /* use validation to raise conflict */
        }
    }
    else if (inst.transport == "serial") {
    }
    else {
        throw Error("unsupported transport selected");
    }

    return (modules);
}

/*
 *  ======== moduleValidate ========
 */
function moduleValidate(inst, vo)
{
    let rtos = system.modules["/ti/utils/RTOS"];

    if (rtos.$static.name == "Linux") {
        if (inst.transport != "serial") {
            vo["transport"].errors.push("Invalid transport for current RTOS selection");
        }
        if (inst.sdev == "") {
            vo["sdev"].errors.push("Invalid device pathname");
        }
    }
    else {
        if (inst.transport != "uart") {
            vo["transport"].errors.push("Invalid transport for current RTOS selection");
        }
        /*  If uart instance is missing, then assume Linux distribution and raise
         *  conflict. This can be re-worked once SysConfig supports configurable
         *  defaults. PMUX-2407
         */
        else if ((inst.transport == "uart") && (inst.uart == undefined)) {
            vo["transport"].errors.push("Transport not supported. Check RTOS selection.");
        }
    }
}

/*
 *  ======== moduleStatic_instances ========
 *  Express dependencies for private instances of other modules
 */
function moduleStatic_instances(moduleStatic)
{
    let modules = new Array();

    if (moduleStatic.Bench_BasicCodecFactory_create_t0) {
        modules.push({
            name: "CONFIG_ERPC_BasicCodecFactory_create_t0",
            moduleName: "/ti/utils/runtime/Bench",
            args: { $name: "CONFIG_ERPC_BasicCodecFactory_create_t0" }
        });
    }

    if (moduleStatic.Bench_BasicCodecFactory_create_t1) {
        modules.push({
            name: "CONFIG_ERPC_BasicCodecFactory_create_t1",
            moduleName: "/ti/utils/runtime/Bench",
            args: { $name: "CONFIG_ERPC_BasicCodecFactory_create_t1" }
        });
    }

    if (moduleStatic.Bench_BasicCodecFactory_dispose_t0) {
        modules.push({
            name: "CONFIG_ERPC_BasicCodecFactory_dispose_t0",
            moduleName: "/ti/utils/runtime/Bench",
            args: { $name: "CONFIG_ERPC_BasicCodecFactory_dispose_t0" }
        });
    }

    if (moduleStatic.Bench_BasicCodecFactory_dispose_t1) {
        modules.push({
            name: "CONFIG_ERPC_BasicCodecFactory_dispose_t1",
            moduleName: "/ti/utils/runtime/Bench",
            args: { $name: "CONFIG_ERPC_BasicCodecFactory_dispose_t1" }
        });
    }

    if (moduleStatic.Bench_ClientManager_createRequest_t0) {
        modules.push({
            name: "CONFIG_ERPC_ClientManager_createRequest_t0",
            moduleName: "/ti/utils/runtime/Bench",
            args: { $name: "CONFIG_ERPC_ClientManager_createRequest_t0" }
        });
    }

    if (moduleStatic.Bench_ClientManager_createRequest_t1) {
        modules.push({
            name: "CONFIG_ERPC_ClientManager_createRequest_t1",
            moduleName: "/ti/utils/runtime/Bench",
            args: { $name: "CONFIG_ERPC_ClientManager_createRequest_t1" }
        });
    }

    if (moduleStatic.Bench_ClientManager_performRequest_t0) {
        modules.push({
            name: "CONFIG_ERPC_ClientManager_performRequest_t0",
            moduleName: "/ti/utils/runtime/Bench",
            args: { $name: "CONFIG_ERPC_ClientManager_performRequest_t0" }
        });
    }

    if (moduleStatic.Bench_Codec_ctor_t0) {
        modules.push({
            name: "CONFIG_ERPC_Codec_ctor_t0",
            moduleName: "/ti/utils/runtime/Bench",
            args: { $name: "CONFIG_ERPC_Codec_ctor_t0" }
        });
    }

    if (moduleStatic.Bench_Codec_ctor_t1) {
        modules.push({
            name: "CONFIG_ERPC_Codec_ctor_t1",
            moduleName: "/ti/utils/runtime/Bench",
            args: { $name: "CONFIG_ERPC_Codec_ctor_t1" }
        });
    }

    if (moduleStatic.Bench_MsgQueTransport_recv_t0) {
        modules.push({
            name: "CONFIG_ERPC_MsgQueTransport_recv_t0",
            moduleName: "/ti/utils/runtime/Bench",
            args: { $name: "CONFIG_ERPC_MsgQueTransport_recv_t0" }
        });
    }

    if (moduleStatic.Bench_MsgQueTransport_send_t0) {
        modules.push({
            name: "CONFIG_ERPC_MsgQueTransport_send_t0",
            moduleName: "/ti/utils/runtime/Bench",
            args: { $name: "CONFIG_ERPC_MsgQueTransport_send_t0" }
        });
    }

    if (moduleStatic.Bench_MsgQueTransport_send_t1) {
        modules.push({
            name: "CONFIG_ERPC_MsgQueTransport_send_t1",
            moduleName: "/ti/utils/runtime/Bench",
            args: { $name: "CONFIG_ERPC_MsgQueTransport_send_t1" }
        });
    }

    return (modules);
}

/*
 *  ======== moduleStatic_modules ========
 *  Express dependencies for other modules
 */
function moduleStatic_modules(moduleStatic)
{
    let modules = new Array();

    modules.push({
        displayName: "RTOS",
        moduleName: "/ti/utils/RTOS"
    });

    modules.push({
        displayName: "GetLibs",
        moduleName: "/ti/rpc/GetLibs"
    });

    return (modules);
}

/*
 *  ======== moduleStatic_config ========
 *  Define config params for the module static instance
 */
let moduleStatic_config = [
    {   /* Bench */
        name: "Bench_BasicCodecFactory_create_t0",
        default: false
    },
    {   /* Bench */
        name: "Bench_BasicCodecFactory_create_t1",
        default: false
    },
    {   /* Bench */
        name: "Bench_BasicCodecFactory_dispose_t0",
        default: false
    },
    {   /* Bench */
        name: "Bench_BasicCodecFactory_dispose_t1",
        default: false
    },
    {   /* Bench */
        name: "Bench_ClientManager_createRequest_t0",
        default: false
    },
    {   /* Bench */
        name: "Bench_ClientManager_createRequest_t1",
        default: false
    },
    {   /* Bench */
        name: "Bench_ClientManager_performRequest_t0",
        default: false
    },
    {   /* Bench */
        name: "Bench_Codec_ctor_t0",
        default: false
    },
    {   /* Bench */
        name: "Bench_Codec_ctor_t1",
        default: false
    },
    {   /* Bench */
        name: "Bench_MsgQueTransport_recv_t0",
        default: false
    },
    {   /* Bench */
        name: "Bench_MsgQueTransport_send_t0",
        default: false
    },
    {   /* Bench */
        name: "Bench_MsgQueTransport_send_t1",
        default: false
    }
];

/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "EmbeddedRPC",
    description: "Embedded Remote Procedure Call configuration",
    config: config,
    defaultInstanceName: "ti_rpc_ERPC_",
    longDescription: moduleDescription,
    moduleInstances: moduleInstances,
    validate: moduleValidate,
    moduleStatic: {
        moduleInstances: moduleStatic_instances,
        modules: moduleStatic_modules,
        config: moduleStatic_config
    },

    templates: {
        "/ti/rpc/Config.c.xdt": "/ti/rpc/ERPC.Config.c.xdt",
        "/ti/rpc/ERPC.cpp.xdt": "/ti/rpc/ERPC.cpp.xdt",
        "/ti/rpc/ERPC.h.xdt": "/ti/rpc/ERPC.h.xdt",
        "/ti/rpc/Makefile.erpc.xdt": "/ti/rpc/Makefile.erpc.xdt",
        "/ti/rpc/erpc.js.xdt": "/ti/rpc/erpc.js.xdt",
        "/ti/rpc/rpc.gv.xdt": "/ti/rpc/rpc.gv.xdt",
        "/ti/rpc/model.gv.xdt": "/ti/rpc/model.gv.xdt"
    }
};

/* export the module */
exports = base;
