#!/bin/bash
#############################################################
# @file run_gateway.sh
#
# @brief TIMAC 2.0 run_gateway.sh, used by run_demo.sh to launch gateway
#
# Group: WCS LPC
# Target Device: cc13xx_cc26xx
#
#############################################################
# 
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

#############################################################
# 
# 
#############################################################

nodejs=`which nodejs`
arch=`uname -m`

if [ x"$nodejs"x != "xx" ]
then
    # great, we have this
    :
else
    if [ x"${arch}"x == x"armv7l"x ]
    then
	# This name should be changed...
	# however it has not been done yet
	# SEE:
	#  https://lists.debian.org/debian-devel-announce/2012/07/msg00002.html
	nodejs=`which node`
    fi
fi

if [ x"$nodejs"x == xx ]
then
	echo "Cannot find node-js appplication, is it installed?"
	exit 1
fi

if [ ! -f $nodejs ]
then
	echo "Cannot find node-js application, is it installed?"
	exit 1
fi

if [ ! -x $nodejs ]
then
    echo "Cannot find node-js application, is it installed?"
    exit 1
fi

PID=`pidof $nodejs`

if [ "x${PID}x" != "xx" ]
then
    kill -9 ${PID}
fi


$nodejs ./gateway.js &
PID=$!
# Wait a couple seconds for it to get started
# or ... for it to exit
sleep 2

if ps -p $PID > /dev/null
then
    echo "Gateway is running as Process id: ${PID}"
    exit 0
else
    echo "Cannot start gateway application"
    exit 1
fi
