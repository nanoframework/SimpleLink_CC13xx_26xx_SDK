/******************************************************************************

 @file nwkinfo.js

 @brief network information specific functions

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

var util = require('util');
var fs = require("fs");

/* Collector states */
const cllc_states = Object.freeze({
    /* Powered up, not started and waiting for user to start */
    initWaiting: 0,
    /* Starting coordinator, scanning and selecting the best parameters */
    startingCoordinator: 1,
    /* Powered up, found network information, and restoring device in network */
    initRestoringCoordinator: 2,
    /* Device is operating as coordinator */
    started: 3,
    /* Device is restored as coordinator in the network */
    restored: 4,
    /* Joining allowed state has changed to allowed */
    joiningAllowed: 5,
    /* Joining allowed state has changed to not allowed */
    joiningNotAllowed: 6
});

/* Network Modes */
const nwkModes = Object.freeze({
    BEACON_ENABLED: 1,
    NON_BEACON: 2,
    FREQUENCY_HOPPING: 3
});


/*!
 * @brief      Constructor for network Information object
 *
 * @param      networkInfo - object with network information
 *
 * @retun      network information object
 */
function NwkInfo(networkInfo) {
    var nwkInfo = this;
    return createNwkInfoObj(networkInfo);
    function createNwkInfoObj(networkInfo) {
        /* set network mode */

        nwkInfo.fh = networkInfo.fh;
        /* set network channel information */
        nwkInfo.channel = networkInfo.channel;
        /* set the PAN Coordinator device informatoin */
        nwkInfo.panCoord = {
            panId: networkInfo.devInfo.panID,
            shortAddress: networkInfo.devInfo.shortAddress,
            extAddress: networkInfo.devInfo.extAddress
        };
        /* set the security information */
        nwkInfo.securityEnabled = networkInfo.securityEnabled;
        /* set network mode */
        if (networkInfo.networkMode == nwkModes.BEACON_ENABLED) {
            nwkInfo.networkMode = "Beacon Enabled"
        }
        else if (networkInfo.networkMode == nwkModes.NON_BEACON) {
            nwkInfo.networkMode = "Non Beacon";
        }
        else if (networkInfo.networkMode == nwkModes.FREQUENCY_HOPPING) {
            nwkInfo.networkMode = "Freq Hopping";
        }
        else {
            nwkInfo.networkMode = "Unknown Mode"
        }
        /* set network state */
        switch (networkInfo.state) {
            case cllc_states.initWaiting:
                /* Application is waiting for user input
                to start the application */
                nwkInfo.state = "waiting";
                break;
            case cllc_states.startingCoordinator:
                /* Application is working to start the network */
                nwkInfo.state = "starting";
                break;
            case cllc_states.initRestoringCoordinator:
                /* Application is working to restore the network
                from previously stored informatoin */
                nwkInfo.state = "restoring";
                break;
            case cllc_states.started:
            case cllc_states.restored:
                /* Network is started */
                nwkInfo.state = "started";
                break;
            case cllc_states.joiningAllowed:
                /* Network is open for new devices to join */
                nwkInfo.state = "open";
                break;
            case cllc_states.joiningNotAllowed:
                /* Network is closed for new devices to join */
                nwkInfo.state = "close";
                break;
            default:
                /* Should not get here */
                console.log("ERROR: rcvd illegal coord state(NwkInfo)");
                nwkInfo.state = "unknown";
                break;
        }
        return nwkInfo;
    }
}

/* update network information */
NwkInfo.prototype.updateNwkInfo = function (networkInfo) {
    var self = this;
    self.fh = networkInfo.fh;
    /* set network channel information */
    self.channel = networkInfo.channel;
    /* set the PAN Coordinator device informatoin */
    self.panCoord = {
        panId: networkInfo.devInfo.panID,
        shortAddress: networkInfo.devInfo.shortAddress,
        extAddress: networkInfo.devInfo.extAddress
    };
    self.securityEnabled = networkInfo.securityEnabled;
    self.networkMode = networkInfo.networkMode;
    /* set network state */
    switch (networkInfo.state) {
        case cllc_states.initWaiting:
            /* Application is waiting for user input
            to start the application */
            self.state = "waiting";
            break;
        case cllc_states.startingCoordinator:
            /* Application is working to start the network */
            self.state = "starting";
            break;
        case cllc_states.initRestoringCoordinator:
            /* Application is working to restore the network
            from previously stored informatoin */
            self.state = "restoring";
            break;
        case cllc_states.started:
        case cllc_states.restored:
            /* Network is started */
            self.state = "started";
            break;
        case cllc_states.joiningAllowed:
            /* Network is open for new devices to join */
            self.state = "open";
            break;
        case cllc_states.joiningNotAllowed:
            /* Network is closed for new devices to join */
            self.state = "close";
            break;
        default:
            /* Should not get here */
            console.log("ERROR: rcvd illegal coord state(networkInfo)");
            self.state = "unknown";
            break;
    }
}

/* update network state */
NwkInfo.prototype.updateNwkState = function (nState) {
    var self = this;
    /* set network state */
    switch (nState.state) {
        case cllc_states.initWaiting:
            /* Application is waiting for user input
            to start the application */
            self.state = "waiting";
            break;
        case cllc_states.startingCoordinator:
            /* Application is working to start the network */
            self.state = "starting";
            break;
        case cllc_states.initRestoringCoordinator:
            /* Application is working to restore the network
            from previously stored informatoin */
            self.state = "restoring";
            break;
        case cllc_states.started:
        case cllc_states.restored:
            /* Network is started */
            self.state = "started";
            break;
        case cllc_states.joiningAllowed:
            /* Network is open for new devices to join */
            self.state = "open";
            break;
        case cllc_states.joiningNotAllowed:
            /* Network is closed for new devices to join */
            self.state = "close";
            break;
        default:
            /* Should not get here */
            console.log("ERROR: rcvd illegal coord state(nState)");
            self.state = "unknown";
            break;
    }
}

module.exports = NwkInfo;
