/******************************************************************************

 @file gateway.js

 @brief local gateway

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
var AppClient = require("./appClient/appclient.js");
var Webserver = require("./webserver/webserver.js");

/* Toggle debug print statements */
const PRINT_DEBUG = false;


/*!
 * @brief      Constructor for local gateway
 *
 * @param      none
 *
 * @retun      none
 */
function Gateway() {
	var applclient = new AppClient();
	var webserver = new Webserver();

	/* rcvd send config req */
	webserver.on('sendConfig', function (data) {
		/* send config request */
		applclient.appC_sendConfig(data);
	});

	/* rcvd send toggle req */
	webserver.on('sendToggle', function (data) {
		/* send toggle request */
		applclient.appC_sendToggle(data);
	});

	/* rcvd getDevArray Req */
	webserver.on('getDevArrayReq', function (data) {
		/* process the request */
		applclient.appC_getDeviceArray();
	});

	/* rcvd getNwkInfoReq */
	webserver.on('getNwkInfoReq', function (data) {
		/* process the request */
		applclient.appC_getNwkInfo();
	});

	webserver.on('setJoinPermitReq', function (data) {
		/* process the request */
		applclient.appC_setPermitJoin(data);
	});

	/* send message to web-client */
	applclient.on('permitJoinCnf', function (data) {
		if(PRINT_DEBUG) console.log("gateway to ws: permitJoinCnf");
		webserver.webserverSendToClient('permitJoinCnf', data);
	});

	/* send connected device info update to web-client */
	applclient.on('connDevInfoUpdate', function (data) {
		if(PRINT_DEBUG) console.log("gateway to ws: connDevInfoUpdate");
		webserver.webserverSendToClient('connDevInfoUpdate', data);
	});

	/* Send Network Information Message to to web-client*/
	applclient.on('nwkInfo', function () {
		if(PRINT_DEBUG) console.log("gateway to ws: nwkInfo");
		webserver.webserverSendToClient('nwkInfo',applclient.nwkInfo);
	});

	/* send device array to web-client */
	applclient.on('getdevArrayRsp', function (data) {
		if(PRINT_DEBUG) console.log("gateway to ws: getdevArrayRsp");
		webserver.webserverSendToClient('getdevArrayRsp', data);
 });

}



/* create gateway */
var gateway = new Gateway();
