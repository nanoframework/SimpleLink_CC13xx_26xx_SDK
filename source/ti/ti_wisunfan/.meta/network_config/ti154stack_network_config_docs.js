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
 *  ======== ti154stack_network_config_docs.js ========
 */

"use strict";

// ======== Top Level ========

const panID = {
    description: "Configures personal area network ID",
    longDescription: `
Used to restrict the network to a certain PAN ID. If left as 0xFFFF on the \
border router, the border router starts with a randomly selected PAN ID between 0x0000 and 0xFFFE. If this parameter is set \
to a certain value for the border router, the value should be set to either the \
same value or 0xFFFF for the router application, so that the router joins the \
intended parent.
\n\
__Default__: 0xFFFF
\n\
__Range__: 0x0000 to 0xFFFF
\n\
__Acceptable Values__: Accepts inputs in decimal, and in hexadecimal provided \
that the value is prefaced by '0x'.
`
};

const channels = {
    description: "Configures channels to be scanned",
    longDescription: `
Configures the channel mask used for non-beacon and beacon modes. Each bit \
indicates if the corresponding channel is to be scanned. The first byte \
represents channels 0 to 7, and the last byte represents channels 128 to 135; \
for example, 0x01 0x10 represents the inclusion of channels 0 and 12.
\n\
For more information, refer to the \
[Data-Rate and PHY](/ti154stack/html/ti154stack/\
ti154stack-overview.html#data-rate-and-phy) \
section of the TI15.4-Stack User's Guide.
\n\
__Default__ (Sub-1 GHz): 0 to 4
\n\
__Default__ (2.4 GHz): 11 to 14
\n\
__Range__:
\n\
Frequency Band | 5 kbps | 50 kbps | 100 kbps | 200 kbps | 250 kbps
--- | --- | --- | --- | --- | --- |
433 MHz |  0 to 6 | 0 to 6 | Not Supported | Not Supported | Not Supported
866 MHz | Not Supported | 0 to 18 | 0 to 9 | Not Supported | Not Supported-
868 MHz | 0 to 33 | 0 to 33 | 0 to 16 | Not Supported | Not Supported
915 MHz | 0 to 128 | 0 to 128 | 0 to 63 | Not Supported | Not Supported
2.4 GHz | Not Supported | Not Supported | Not Supported | Not Supported | \
11 to 26
`
};

const fhChannels = {
    description: "Configures channels to be scanned",
    longDescription: `
Represents the list of channels on which the device can hop. In frequency \
hopping, the actual sequence will be based on DH1CF function. When it is set \
to false, the sequence shall be a linear hopping over available channels in \
ascending order and shall be used to change channel during the join phase.
\n\
The first byte represents channels 0 to 7, and the last byte represents \
channels 128 to 135; for example, 0x01 0x10 represents the inclusion of \
channels 0 and 12.
\n\
For more information, refer to the \
[Data-Rate and PHY](/ti154stack/html/ti154stack/\
ti154stack-overview.html#data-rate-and-phy) and \
[Frequency-Hopping Mode](/ti154stack/html/ti154stack/\
frequency-hopping-mode.html#frequency-hopping-mode) \
sections of the TI15.4-Stack User's Guide.
\n\
__Default__: 0 to 4
\n\
__Range__:
\n\
Frequency Band | 5 kbps | 50 kbps | 100 kbps | 200 kbps
--- | --- | --- | --- | --- |
433 MHz |  0 to 6 | 0 to 6 | Not Supported | Not Supported
866 MHz | Not Supported | 0 to 18 | 0 to 9 | Not Supported
868 MHz | 0 to 33 | 0 to 33 | Not Supported | 0 to 16
915 MHz | 0 to 128 | 0 to 128 | Not Supported | 0 to 63
`
};

const fhAsyncChannels = {
    description: "Configures channels to target async frames",
    longDescription: `
Configures the list of channels to target the async frames. It is represented \
as a bit string with LSB representing Ch0; for example, 0x01 0x10 represents \
the inclusion of channels 0 and 12. It must cover all channels that could \
be used by a target device in its hopping sequence. Channels marked beyond \
number of channels supported by PHY Config are excluded by stack. To avoid \
interference on a channel, remove it from async mask and add it to the \
exclude channels for FH Channel Mask (\`CONFIG_FH_CHANNEL_MASK\`).
\n\
__Default__ (Border Router): 0 to 128
\n\
__Default__ (Router): 0 to 55
\n\
__Range__:
\n\
Frequency Band | 5 kbps | 50 kbps | 100 kbps | 200 kbps
--- | --- | --- | --- | --- |
433 MHz |  0 to 6 | 0 to 6 | Not Supported | Not Supported
866 MHz | Not Supported| 0 to 18 | 0 to 9 | Not Supported
868 MHz | 0 to 33 | 0 to 33 | Not Supported | 0 to 16
915 MHz | 0 to 128 | 0 to 128 | Not Supported | 0 to 63
`
};

const maxDevices = {
    description: "Configures the maximum number of children for coordinator",
    longDescription: `
Configures the maximum number of children devices that can be supported by \
the coordinator. It is recommended that this value be set below 150.
\n\
__Default__: 50
\n\
__Range__: 0 to UINT16_MAX
\n\
***Additional Setup Required***:
\n\
\`MAX_DEVICE_TABLE_ENTRIES\` in the project specific .opts file must be \
updated if this parameter is modified from the default setting. For secure \
applications, \`MAX_DEVICE_TABLE_ENTRIES\` must be set equal to or greater \
than the maximum devices selected. For non-secure applications, \
\`MAX_DEVICE_TABLE_ENTRIES\` can be set to 0 to save space.
`
};

const fhNetname = {
    description: "Configures value for FH PIB attribute netname",
    longDescription: `
The network name (NetName) is a unique network identifier that is configured \
by the application using frequency hopping and PAN information base (FH-PIB) \
attributes. Maintenance of the NetName is beyond the scope of TI 15.4-Stack \
and is not used to filter frames; instead, the value is carried in a PAN \
Advertisement frame that can be parsed by a receiving application.
\n\
__Default__: FHTest
\n\
__Acceptable Values__: 32-bit ASCII value
    `
};

// ======== Application ========

const trackingDelayTime = {
    description: "Configures interval in ms between tracking messages",
    longDescription: `
Configures the interval in milliseconds at which the collector will send \
tracking request messges to the devices in the network to confirm that \
they are still active. \
It is recommended to set this value to at least twice \
the beacon interval (based on beacon order and superframe order) in \
BCN Mode, or to at least twice the polling interval in NBCN and \
FH Mode.
\n\
__Default__: 5000
\n\
__Range__: 0 to UINT32_MAX
`
};

const pollingInterval = {
    description: "Configures interval in ms between sensor data polls",
    longDescription: `
Configures the interval in milliseconds for how often a sensor device polls \
its parent for data. If the sensor device is a sleep device, this tells how \
often this device will poll its parent. On the collector, it must be greater \
than or equal to the polling interval set on sensor devices. It is \
recommended that this value be set above 300 ms.
\n\
__Default__: 2000
\n\
__Range__: The polling interval must be within the range of \
the \`MIN_POLLING_INTERVAL\` and \`MAX_POLLING_INTERVAL\` defined in the \
sensor project. By default, that range is from 1,000 to 10,000 ms.
`
};

const reportingInterval = {
    description: "Configures interval in ms between sensor data reports",
    longDescription: `
Configures interval in milliseconds for how often the device will report data.
\n\
On the sensor side, this parameter defines the default sensor reporting \
interval in milliseconds. On the collector side, this parameter defines \
the reporting interval in milliseconds that will be configured in the \
sensor when the sensor joins the network. A reporting interval of 0 means \
to turn off automated reporting, but will force the sensor device to send \
the sensor data message once.
\n\
It is recommended that this value be set above 500 ms.
\n\
__Default__: 3000
\n\
__Range__: The reporting interval must be within the range of \
the \`MIN_REPORTING_INTERVAL\` and \`MAX_REPORTING_INTERVAL\` defined in the \
sensor project. By default, that range is from 1,000 to 360,000 ms.
`
};

const scanBackoffInterval = {
    description: "Configures scan back-off interval in ms",
    longDescription: `
Configures the scan back-off interval in milliseconds.
\n\
__Default__: 5000
\n\
__Range__: 0 to UINT32_MAX
`
};

const orphanBackoffInterval = {
    description: "Configures interval in ms between orphan notifications",
    longDescription: `
Configures the interval in milliseconds for delay between orphan notifications.
\n\
__Default__: 300000
\n\
__Range__: 0 to UINT32_MAX
`
};

const scanDuration = {
    description: "Configures scan duration for scan request",
    longDescription: `
Configures the exponent used in the scan duration calculation.
\n\
__Default__: 5
\n\
__Range__: 0 to 255
`
};

// ======== MAC ========

const ccaThreshold = {
    description: "Configures network ccaThreshold",
    longDescription: `
    Configures the CCA Threshold that you wish to use within the network.
    \n\
    __Default__: -60
    \n\
    __Range__: -60 to -120
`
};

const fhBroadcastInterval = {
    description: `Configures interval in ms between start of two broadcast \
slots`,
    longDescription: `
Configures the interval in milliseconds for broadcast message generation \
in frequency hopping. If set to 0, it shall disable broadcast messages and \
will not cause sleepy devices any additional power overhead.
\n\
It is recommended that this value be set above 200 ms.
\n\
__Default__: 10000
\n\
__Range__: 0 to UINT32_MAX
`
};


const unicastDwellTime = {
    description: `Configures interval in ms between start of two unicast \
slots`,
    longDescription: `
Configures the interval in milliseconds for unicast message generation \
in frequency hopping. If set to 0, it shall disable unicast messages and \
will not cause sleepy devices any additional power overhead.
\n\
It is recommended that this value be set above 200 ms.
\n\
__Default__: 100
\n\
__Range__: 0 to 255
`
};

const fhBroadcastDwellTime = {
    description: `Configures duration in ms of the node’s broadcast slot in \
frequency hopping`,
    longDescription: `
Configures the duration, in milliseconds, of a node's broadcast slot in \
frequency hopping. If set to 0, it shall disable broadcast hopping and \
broadcast message transmissions.
\n\
__Default__: 255
\n\
__Range__: 0 to 255
`
};

exports = {
    panID: panID,
    channels: channels,
    fhChannels: fhChannels,
    fhAsyncChannels: fhAsyncChannels,
    maxDevices: maxDevices,
    fhNetname: fhNetname,
    trackingDelayTime: trackingDelayTime,
    scanBackoffInterval: scanBackoffInterval,
    orphanBackoffInterval: orphanBackoffInterval,
    scanDuration: scanDuration,
    fhBroadcastInterval: fhBroadcastInterval,
    fhBroadcastDwellTime: fhBroadcastDwellTime,
    ccaThreshold: ccaThreshold,
    unicastDwellTime: unicastDwellTime
};
