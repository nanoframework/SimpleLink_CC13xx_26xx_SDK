#!/bin/bash
#############################################################
# @file run_collector.sh
#
# @brief TIMAC 2.0 run_collector.sh, used by run_demo.sh to launch collector
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

# Because this is a "quick demo" we hard code
# the device name in this check. For a production
# application, a better check is suggested.
# 
if [ ! -c /dev/ttyACM0 ]
then
    echo ""
    echo "The Launchpad (/dev/ttyACM0) does not seem to be present"
    echo ""
    exit 1
fi

# This test is simple... 
arch=`uname -m`

if [ "x${arch}x" == 'xx86_64x' ]
then
    exe=host_collector
fi


if [ "x${arch}x" == 'xarmv7lx' ]
then
    # ---------
    # This script has no way to determine how you built
    # the application.
    # --------
    # This script assumes that you have built the "collector"
    # application via the a cross compiler method and not
    # built natively on the BBB
    # ---------
    # If you build natively on the BBB, then the BBB
    # is actually the HOST... and thus the application is
    # "host_collector"
    exe=bbb_collector
fi

if [ "x${exe}x" == "xx" ]
then
    echo "Cannot find Collector App exe: $exe"
    exit 1
fi

if [ ! -x $exe ]
then
    echo "Cannot find EXE $exe"
    exit 1
fi

PID=`pidof $exe`

if [ "x${PID}x" != "xx" ]
then
    kill -9 ${PID}
fi

# by default, the application uses the name: "collector.cfg" as the configuration file
# or you can pass the name of the configuration file on the command line
./$exe collector.cfg &
PID=$!
# Wait 3 seconds for it to get started ...
sleep 3
if ps -p $PID > /dev/null
then
    echo "Collector Running as Process id: ${PID}"
    exit 0
else
    echo "Error starting collector application"
    exit 1
fi





