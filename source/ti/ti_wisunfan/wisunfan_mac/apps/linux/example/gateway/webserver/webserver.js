/******************************************************************************

 @file webserver.js

 @brief webserver implementation

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

var express = require('express');
var events = require("events");
var socket = require("socket.io");
var http = require("http");


/* Webserver Instance */
var webserverInstance;

/*!
 * @brief      Constructor for web-server
 *
 * @param      none
 *
 * @retun      none
 */
function Webserver() {

    /* There should be only one app client */
	if (typeof webserverInstance !== "undefined") {
		return webserverInstance;
	}

	/* Set up to emit events */
	events.EventEmitter.call(this);
	webserverInstance = this;

	/* Set up webserver */
	var	app = express();
	var	server = http.createServer(app);
	webserverInstance.io = socket.listen(server);
	server.listen( 1310, '0.0.0.0');
	var path = require('path');
	app.use(express.static(path.join(__dirname, '..'+'/public')));
	app.get('/', function(req, res){
		res.sendFile(__dirname + '/collectorApp.html');
	});

    /* Handle socket events */
    webserverInstance.io.sockets.on('connection', function (socket) {

        socket.on('setJoinPermit', function (data) {
            webserverInstance.emit('setJoinPermitReq', data);
        });

        socket.on('getDevArrayReq', function (data) {
            webserverInstance.emit('getDevArrayReq', data);
        });

        socket.on('getNwkInfoReq', function (data) {
            webserverInstance.emit('getNwkInfoReq', data);
        });

        socket.on('sendConfig', function (data) {
            webserverInstance.emit('sendConfig', data);
        });

        socket.on('sendToggle', function (data) {
            webserverInstance.emit('sendToggle', data);
        });

    });

	/**********************************************************************
	 Public method to send Update Messages to the client
    ***********************************************************************/
	webserverInstance.webserverSendToClient = function(msgType, data){
		webserverInstance.io.sockets.emit(msgType, data);
		};

    }

Webserver.prototype.__proto__ = events.EventEmitter.prototype;

module.exports = Webserver;
