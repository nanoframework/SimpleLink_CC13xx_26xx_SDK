
/* main file to test the NV driver, csf functionality, ui interface */
#include "compiler.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
/* #include <unistd.h> */

/* #include <pthread.h> */
/* #include <semaphore.h> */
#include <time.h>
#include <stdbool.h>

/* NV Driver */
#include "log.h"
#include "nvintf.h"
#include "nv_linux.h"
#include "csf.h"
#include "csf_linux.h"
#include "debug_helpers.h"
#include "smsgs.h"
#include "llc.h"
#include "cllc.h"

#include "collector.h"
/******************************************************************************
 Constants and definitions
*******************************************************************************/

/* Set Debug Verbosity */
int debug_Verbosity = 5; /* Default */

/******************************************************************************
 Variables
******************************************************************************/
/* Non-volatile function pointers */
NVINTF_nvFuncts_t nvFps;
void getDeviceParam(uint16_t *shortAddressM, uint8_t *extAddressM);

/*util functions */
void print_CsfNetworkInformation(Llc_netInfo_t *pInfo);
void print_ApiMacDeviceDescriptor(ApiMac_deviceDescriptor_t *devInfo);

/* Test Function - not part of the production release */
/*void Test_CSF(void); */
void Test_CSF(int x);

void Test_Csf_Init(void);
void Test_Csf_networkUpdate(void);
void Test_Csf_deviceUpdate(void);
void Test_Csf_deviceNotActiveUpdate(void);
void Test_Csf_deviceConfigUpdate(void);
void Test_Csf_deviceSensorDataUpdate(void);
void Test_Csf_stateChangeUpdate(void);
void Test_Csf_getNetworkInformation(void);

void Test_Csf_networkUpdatefh(void);
void Test_Csf_deviceNotActiveManual(void);

/* allows to enter the short address, ext address, cap info to add the device */
void Test_Csf_addDeviceManually(void);
void Test_Csf_sendDataToShortAddr(void);

void Test_Csf_getDeviceArray(void);

void Test_Collector(void);

void Test_CSF(int x)
{
    DEBUG_printf("=======================================================\n");
    DEBUG_printf("**************  Testing the CSF Module  ***************\n");
    DEBUG_printf("=======================================================\n");

    for(;;)
    {
        DEBUG_printf("  This is a test file, following API's can be tested \n");
        DEBUG_printf("  1. Csf_Init(); \n");
        DEBUG_printf("  2. Csf_networkUpdate(); \n");
        DEBUG_printf("  3. Csf_deviceUpdate(); \n");
        DEBUG_printf("  4. Csf_deviceConfigUpdate(); \n");
        DEBUG_printf("  5. Csf_deviceNotActiveUpdate(); \n");
        DEBUG_printf("  6. Csf_deviceSensorDataUpdate(); \n");
        DEBUG_printf("  7. Csf_stateChangeUpdate(); \n");
        DEBUG_printf("  8. Add custom device -- Csf_deviceUpdate(); \n");
        DEBUG_printf("  9. Send data to a given short address \n");
        DEBUG_printf(" 10. Get connected device array \n");
        DEBUG_printf(" 11. Send FH newtork update Csf_networkUpdate(); \n");
        DEBUG_printf(" 12. Send device not active for specific device Csf_deviceNotActiveUpdate(); \n");
        DEBUG_printf("Enter test: ");
        DEBUG_get_integer(&x);
        if(x == 1)
        {
            DEBUG_printf("Testing API: Csf_Init \n");
            Test_Csf_Init();
        }
        else if(x == 2)
        {
            DEBUG_printf("Testing API: Csf_networkUpdate \n");
            Test_Csf_networkUpdate();
        }
        else if(x == 3)
        {
            DEBUG_printf("Testing API: Csf_deviceUpdate \n");
            Test_Csf_deviceUpdate();

        }
        else if(x == 4)
        {
            DEBUG_printf("Testing API: Csf_deviceUpdate \n");
            Test_Csf_deviceConfigUpdate();
        }

        else if(x == 5)
        {
            DEBUG_printf("Testing API: Csf_deviceUpdate \n");
            Test_Csf_deviceNotActiveUpdate();

        }

        else if(x == 6)
        {
            DEBUG_printf("Testing API: Csf_deviceUpdate \n");
            Test_Csf_deviceSensorDataUpdate();

        }

        else if(x == 7)
        {
            DEBUG_printf("Testing API: Csf_deviceUpdate \n");
            Test_Csf_stateChangeUpdate();

        }

        else if(x == 8)
        {
            DEBUG_printf("Add custom device -- via API: Csf_deviceUpdate \n");
            Test_Csf_addDeviceManually();

        }

        else if(x == 9)
        {
            DEBUG_printf("Add custom device -- via API: Csf_deviceUpdate \n");
            Test_Csf_sendDataToShortAddr();

        }

        else if(x == 10)  /* a */
        {
            DEBUG_printf("Add custom device -- via API: Csf_deviceUpdate \n");
            Test_Csf_getDeviceArray();

        }
        else if(x == 11) /*b */
        {
            DEBUG_printf("Testing API: Csf_networkUpdate fh config\n");
            Test_Csf_networkUpdatefh();
        }

        else if(x == 12) /*c */
        {
            DEBUG_printf("Testing API: Csf_networkUpdate fh config\n");
            Test_Csf_deviceNotActiveManual();
        }
    }
}

/* Util Functions */
#if 0
void print_CsfNetworkInformation(Csf_networkInformation_t *pInfo)
{
    int i=0;
    DEBUG_printf("  >> Network Information is displayed below: \n");
    DEBUG_printf("  >> Frequency hopping is %d \n", pInfo->fh);
    if(pInfo->fh == false)
    {
        print_ApiMacDeviceDescriptor(&(pInfo->info.netInfo.deviceInfo));
        DEBUG_printf("  >> Channel is %d \n", pInfo->info.netInfo.channel);
    }
    else if(pInfo->fh == true)
    {
        /* Print ApiMac_deviceDescriptor_t */
        print_ApiMacDeviceDescriptor(&(pInfo->info.fhNetInfo.devInfo));
        /* Print Llc_deviceInfoFh_t */
        /* Print Broadcast Interval Settings */
        DEBUG_printf("  >> Broadcast Interval Settings: Dwell Time (ms) = %d \n", pInfo->info.fhNetInfo.fhInfo.bcIntervals.dwell);
        DEBUG_printf("  >> Broadcast Interval Settings: interval (ms) = %d \n", pInfo->info.fhNetInfo.fhInfo.bcIntervals.interval);
        /* Print Broadcast Number of channels used */
        DEBUG_printf("  >> Broadcast Number of channels used = %d \n", pInfo->info.fhNetInfo.fhInfo.bcNumChans);
        /* Print Broadcast channels used */
        DEBUG_printf("  >> Broadcast channels used = ");
        for(i = 0; i<pInfo->info.fhNetInfo.fhInfo.bcNumChans; i++)
        {
            DEBUG_printf("%d ", pInfo->info.fhNetInfo.fhInfo.pBcChans[i]);
        }
        DEBUG_printf("\n");
        /* Print Unicast Interval Settings */
        DEBUG_printf("  >> Unicast Interval Settings: Dwell Time (ms) = %d \n", pInfo->info.fhNetInfo.fhInfo.unicastIntervals.dwell);
        DEBUG_printf("  >> Unicast Interval Settings: interval (ms) = %d \n", pInfo->info.fhNetInfo.fhInfo.unicastIntervals.interval);
        /* Print Unicast number of channels used */
        DEBUG_printf("  >> Unicast number of channels used = %d \n", pInfo->info.fhNetInfo.fhInfo.unicastNumChans);
        /* Print Unicast channels used */
        DEBUG_printf("  >> Unicast channels used = ");
        for(i = 0; i<pInfo->info.fhNetInfo.fhInfo.unicastNumChans; i++)
        {
            DEBUG_printf("%d ", pInfo->info.fhNetInfo.fhInfo.pUnicastChans[i]);
        }
        DEBUG_printf("\n");
    }
    else
    {
        DEBUG_printf("  >> Illegal value of fh in Csf_networkInfomration_t struct = %d\n", pInfo->fh);
    }
}
#endif

void print_ApiMacDeviceDescriptor(ApiMac_deviceDescriptor_t *devInfo)
{
    int i = 0;
    DEBUG_printf("  >> Device Descriptor Info: \n");
    DEBUG_printf("  >> PanId = 0x%X \n", devInfo->panID);
    DEBUG_printf("  >> ShortAddress = 0x%X \n", devInfo->shortAddress);
    DEBUG_printf("  >> Extended PanId = ");
    for(i = 0 ; i<8; i++)
    {
        DEBUG_printf("0x%X ", devInfo->extAddress[i]);
    }
    DEBUG_printf("\n");
}

/* Test Functions */
void Test_Csf_Init(void)
{
    /* Test API

       API: extern void Csf_init(void *sem);

    */
    DEBUG_printf("  >> Start testing the API : Csf_init \n");
    DEBUG_printf("  ----------------------------------------------------\n");
    Csf_init(NULL);
    DEBUG_printf("  >> End testing the API : Csf_init \n");
    DEBUG_printf("=======================================================\n");
}

void Test_Csf_getNetworkInformation(void)
{

    /* Test API

       extern bool Csf_getNetworkInformation(Llc_netInfo *pInfo);

    */
    DEBUG_printf("  >> Start testing the API : Csf_getNetworkInformation \n");
    DEBUG_printf("  ----------------------------------------------------\n");
    Llc_netInfo_t pInfo;
    Csf_getNetworkInformation(&pInfo);
    DEBUG_printf("  >> The returned value from the Csf module are \n");
    /* Util function */
    /* print_CsfNetworkInformation(&pInfo); */
    DEBUG_printf("  >> End testing the API : Csf_getNetworkInformation \n");
    DEBUG_printf("=======================================================\n");

}

void Test_Csf_networkUpdate(void)
{
    /* Test API

       API: extern void Csf_networkUpdate(bool restored, bool fh, void *pNetworkInfo);

    */
    DEBUG_printf(" >> Start testing the API : Csf_networkUpdate \n");
    DEBUG_printf("  ----------------------------------------------------\n");
#if 1
    bool restored = true;
    Llc_netInfo_t pNetworkInfo;
    /* populate the test network information data */
    pNetworkInfo.fh = false;
    pNetworkInfo.channel = 0x14;
    pNetworkInfo.devInfo.panID = 0xAAAA;
    pNetworkInfo.devInfo.shortAddress = 0x1212;
    uint8_t a[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAA, 0xBB};
    memcpy(pNetworkInfo.devInfo.extAddress, a, 8);
    DEBUG_printf("calling the csf module now \n");
    /* Send the message */
    Csf_networkUpdate(restored,  &pNetworkInfo);
#endif
#if 0  /* Frequency Hopping network config. */
    /* Now send the information with frequency hopping parameters */
    bool restoredFh = true;
    bool fhFh = true;
    Csf_networkInformation_t pNetworkInfoFh;

    pNetworkInfoFh.fh = true;
    pNetworkInfoFh.info.fhNetInfo.devInfo.panID = 0xBBBB;
    pNetworkInfoFh.info.fhNetInfo.devInfo.shortAddress = 0x1212;
    uint8_t extAddr[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAA, 0xBB};
    memcpy(pNetworkInfoFh.info.fhNetInfo.devInfo.extAddress, extAddr, 8);
    /* set the broadcast dwell settings */
    pNetworkInfoFh.info.fhNetInfo.fhInfo.bcIntervals.dwell = 0x12;
    pNetworkInfoFh.info.fhNetInfo.fhInfo.bcIntervals.interval = 0x13;
    /* set the broadcast number of channels */
    pNetworkInfoFh.info.fhNetInfo.fhInfo.bcNumChans = 0x1;
    uint8_t a[1] = {9};
    pNetworkInfoFh.info.fhNetInfo.fhInfo.pBcChans = malloc(1);
    if(pNetworkInfoFh.info.fhNetInfo.fhInfo.pBcChans != NULL)
    {
        memcpy(&(pNetworkInfoFh.info.fhNetInfo.fhInfo.pBcChans[0]), a, 1);
    }
    /* set the unicast dwell settings */
    pNetworkInfoFh.info.fhNetInfo.fhInfo.unicastIntervals.dwell = 0x15;
    pNetworkInfoFh.info.fhNetInfo.fhInfo.unicastIntervals.interval = 0x16;
    /* set the unicast number of channels */
    pNetworkInfoFh.info.fhNetInfo.fhInfo.unicastNumChans = 0x1;
    uint8_t b[1] = {9};
    pNetworkInfoFh.info.fhNetInfo.fhInfo.pUnicastChans = malloc(1);
    if(pNetworkInfoFh.info.fhNetInfo.fhInfo.pUnicastChans != NULL)
    {
        memcpy(&(pNetworkInfoFh.info.fhNetInfo.fhInfo.pUnicastChans[0]), b, 1);
    }
    DEBUG_printf("  >> Network info used for test is displayed below \n");
    print_CsfNetworkInformation(&pNetworkInfoFh);
    /* Send the message */
    Csf_networkUpdate(restoredFh, fhFh, &pNetworkInfoFh);

    if(pNetworkInfoFh.info.fhNetInfo.fhInfo.pUnicastChans != NULL)
    {
        free(pNetworkInfoFh.info.fhNetInfo.fhInfo.pUnicastChans);
    }
    if(pNetworkInfoFh.info.fhNetInfo.fhInfo.pBcChans != NULL)
    {
        free(pNetworkInfoFh.info.fhNetInfo.fhInfo.pBcChans);
    }

#endif

    DEBUG_printf(" >> End testing the API : Csf_networkUpdate \n");
    DEBUG_printf("=======================================================\n");

}

void Test_Csf_deviceUpdate(void)
{
    /* Test API

       API:
       ApiMac_assocStatus_t Csf_deviceUpdate(ApiMac_deviceDescriptor_t *pDevInfo,
       ApiMac_capabilityInfo_t capInfo)

       Objective:
       Call this API and check if the new device information is being sent correctly
       to the client

       Procedure:
       Start the server,
       Connect the client,
       send the test message

    */
    DEBUG_printf("  >> Start testing the API : Csf_deviceUpdate \n");
    DEBUG_printf("  ----------------------------------------------------\n");
    ApiMac_deviceDescriptor_t pDevInfo;
    ApiMac_capabilityInfo_t capInfo;
    /* set up device descriptor */
    pDevInfo.panID = 0xAAAA;
    pDevInfo.shortAddress = 0x1212;
    uint8_t a[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAA, 0xBB};
    memcpy(pDevInfo.extAddress, a, 8);
    /* set up cap info */
    capInfo.panCoord = true;
    capInfo.ffd = true;
    capInfo.mainsPower = true;
    capInfo.rxOnWhenIdle = true;
    capInfo.security = true;
    capInfo.allocAddr = true;
    /* send the message */
    Csf_deviceUpdate(&pDevInfo, &capInfo);
    DEBUG_printf("  >> End testing the API: Csf_deviceUpdate \n");
    DEBUG_printf("=======================================================\n");

}

void Test_Csf_deviceNotActiveUpdate(void)
{
    /* Test API

       API: void Csf_deviceNotActiveUpdate(ApiMac_deviceDescriptor_t *pDevInfo,
       bool timeout);

    */

    DEBUG_printf("  >> Start testing the API : Csf_deviceNotActiveUpdate \n");
    DEBUG_printf("  ----------------------------------------------------\n");
    ApiMac_deviceDescriptor_t pDevInfo;
    bool timeout = true;
    /* set up device descriptor */
    pDevInfo.panID = 0xAAAA;
    pDevInfo.shortAddress = 0x1212;
    uint8_t a[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAA, 0xBB};
    memcpy(pDevInfo.extAddress, a, 8);
    /* send the message */
    Csf_deviceNotActiveUpdate(&pDevInfo, timeout);
    DEBUG_printf("  >> End testing the API: Csf_deviceNotActiveUpdate \n");
    DEBUG_printf("=======================================================\n");

}

void Test_Csf_deviceConfigUpdate(void)
{
    /* Test API

       API: extern void Csf_deviceConfigUpdate(ApiMac_sAddr_t *pSrcAddr, int8_t rssi,
       Smsgs_configRspMsg_t *pMsg);

    */
    DEBUG_printf("  >> Start testing the API : Csf_deviceConfigUpdate \n");
    DEBUG_printf("  ----------------------------------------------------\n");
    ApiMac_sAddr_t pSrcAddr;
    int8_t rssi = -40;
    Smsgs_configRspMsg_t pMsg;
    pSrcAddr.addr.shortAddr = 0x1212;
    pSrcAddr.addrMode = ApiMac_addrType_short;
    pMsg.cmdId = Smsgs_cmdIds_configRsp;
    pMsg.status = Smsgs_statusValues_success;
    pMsg.frameControl = (Smsgs_dataFields_tempSensor|Smsgs_dataFields_lightSensor|Smsgs_dataFields_humiditySensor|Smsgs_dataFields_msgStats|Smsgs_dataFields_configSettings);
    pMsg.reportingInterval = 15;
    pMsg.pollingInterval = 15;

    Csf_deviceConfigUpdate(&pSrcAddr,rssi, &pMsg);
    DEBUG_printf("  >> End testing the API: Csf_deviceConfigUpdate \n");
    DEBUG_printf("=======================================================\n");

}

void Test_Csf_deviceSensorDataUpdate(void)
{
    /* Test API

       API: extern void Csf_deviceSensorDataUpdate(ApiMac_sAddr_t *pSrcAddr, int8_t rssi,
       Smsgs_sensorMsg_t *pMsg);

    */
    DEBUG_printf("  >> Start testing the API : Csf_deviceSensorDataUpdate \n");
    DEBUG_printf("  ----------------------------------------------------\n");
    ApiMac_sAddr_t pSrcAddr;
    int8_t rssi = -40;
    Smsgs_sensorMsg_t pMsg;
    pSrcAddr.addr.shortAddr = 0x1212;
    pSrcAddr.addrMode = ApiMac_addrType_short;
    /* Message Data */
    pMsg.cmdId = Smsgs_cmdIds_sensorData;
    pMsg.frameControl = (Smsgs_dataFields_tempSensor|Smsgs_dataFields_lightSensor|Smsgs_dataFields_humiditySensor);/*|Smsgs_dataFields_msgStats|Smsgs_dataFields_configSettings); */
    /* Temperature Sensor Data */
    pMsg.tempSensor.ambienceTemp = 0x1234 ;
    pMsg.tempSensor.objectTemp = 0x4567;
    /* Light Sensor Data */
    pMsg.lightSensor.rawData = 0x9999;
    /* Humidity Sensor Data */
    pMsg.humiditySensor.temp = 0x4567;;
    pMsg.humiditySensor.humidity = 0xABCD;
    /* Network Statistics */
    pMsg.msgStats.joinAttempts = 0x01;
    pMsg.msgStats.joinFails = 0x00;
    pMsg.msgStats.msgsAttempted = 0x100;
    pMsg.msgStats.msgsSent = 0x100;
    pMsg.msgStats.trackingRequests = 0x50;
    pMsg.msgStats.trackingResponseAttempts = 0x50;
    pMsg.msgStats.trackingResponseSent = 0x50;
    pMsg.msgStats.channelAccessFailures = 0x00;
    pMsg.msgStats.macAckFailures = 0x00;
    pMsg.msgStats.otherDataRequestFailures = 0x00;
    pMsg.msgStats.syncLossIndications = 0x00;
    /* Configuration Settings Data */
    pMsg.configSettings.reportingInterval = 10000;
    pMsg.configSettings.pollingInterval = 10000;
    /* Send the message :) */
    printf("send data to csf \n");
    Csf_deviceSensorDataUpdate(&pSrcAddr, rssi, &pMsg);
    DEBUG_printf("  >> End testing the API: Csf_deviceSensorDataUpdate \n");
    DEBUG_printf("=======================================================\n");

}

void Test_Csf_stateChangeUpdate(void)
{
    /* Test API

       API: extern void extern void Csf_stateChangeUpdate(Cllc_states state);

    */
    DEBUG_printf("  >> Start testing the API : Csf_stateChangeUpdate \n");
    DEBUG_printf("  ----------------------------------------------------\n");
    /* send a test state as started. */
    Csf_stateChangeUpdate(Cllc_states_started);
    DEBUG_printf("  >> End testing the API: Csf_stateChangeUpdate \n");
    DEBUG_printf("=======================================================\n");

}

#define inputSize 20
void Test_Csf_addDeviceManually(void)
{

    uint16_t shortAddressM;
    uint8_t extAddressM[8];

    /* note there are no checks if the user is providing the values correctly */

    DEBUG_printf("  >> Start testing the API : Csf_deviceUpdate \n");
    DEBUG_printf("  >> This test will allow to enter short addr, ext addr \n");
    DEBUG_printf("  ----------------------------------------------------\n");
    /* Wait for the user input */

    getDeviceParam(&shortAddressM, extAddressM);

    ApiMac_deviceDescriptor_t pDevInfo;
    ApiMac_capabilityInfo_t capInfo;
    /* set up device descriptor */
    pDevInfo.panID = 0xAAAA;
    pDevInfo.shortAddress = shortAddressM;
    /*uint8_t a[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAA, 0xBB}; */
    memcpy(pDevInfo.extAddress, extAddressM, 8);
    /* set up cap info */
    capInfo.panCoord = true;
    capInfo.ffd = true;
    capInfo.mainsPower = true;
    capInfo.rxOnWhenIdle = true;
    capInfo.security = true;
    capInfo.allocAddr = true;
    /* send the message */
    Csf_deviceUpdate(&pDevInfo, &capInfo);
    DEBUG_printf("  >> End testing the API: Csf_deviceUpdate \n");
    DEBUG_printf("=======================================================\n");

}

void getDeviceParam(uint16_t *shortAddressM, uint8_t *extAddressM)
{

    int x;
    uint64_t v64;

    DEBUG_printf("Enter Short Address: ");
    DEBUG_get_integer(&x);
    *shortAddressM = (uint16_t)x;

    DEBUG_printf("Enter EXT Address as one hex number: ");
    DEBUG_get_uint64(&v64);

    for(x = 0; x < 8; x++)
    {
        extAddressM[x] = ((v64 >> (64 - 8 - (8 * x)) & 0x0ff));
    }
}

void Test_Csf_sendDataToShortAddr(void)
{
    uint16_t shortAddressM;
    uint8_t extAddressM[8];

    /* Test API

       API: extern void Csf_deviceSensorDataUpdate(ApiMac_sAddr_t *pSrcAddr, int8_t rssi,
       Smsgs_sensorMsg_t *pMsg);

    */
    DEBUG_printf("  >> Start testing the API : Csf_deviceSensorDataUpdate \n");
    DEBUG_printf("  ----------------------------------------------------\n");
    getDeviceParam(&shortAddressM, extAddressM);
    ApiMac_sAddr_t pSrcAddr;
    int8_t rssi = -40;
    Smsgs_sensorMsg_t pMsg;
    pSrcAddr.addr.shortAddr = shortAddressM;
    pSrcAddr.addrMode = ApiMac_addrType_short;
    /* Message Data */
    pMsg.cmdId = Smsgs_cmdIds_sensorData;
    pMsg.frameControl = (Smsgs_dataFields_tempSensor|Smsgs_dataFields_lightSensor|Smsgs_dataFields_humiditySensor);/*|Smsgs_dataFields_msgStats|Smsgs_dataFields_configSettings); */
    /* Temperature Sensor Data */
    pMsg.tempSensor.ambienceTemp = 0x1234 ;
    pMsg.tempSensor.objectTemp = 0x4567;
    /* Light Sensor Data */
    pMsg.lightSensor.rawData = 0x9999;
    /* Humidity Sensor Data */
    pMsg.humiditySensor.temp = 0x4567;;
    pMsg.humiditySensor.humidity = 0xABCD;
    /* Network Statistics */
    pMsg.msgStats.joinAttempts = 0x01;
    pMsg.msgStats.joinFails = 0x00;
    pMsg.msgStats.msgsAttempted = 0x100;
    pMsg.msgStats.msgsSent = 0x100;
    pMsg.msgStats.trackingRequests = 0x50;
    pMsg.msgStats.trackingResponseAttempts = 0x50;
    pMsg.msgStats.trackingResponseSent = 0x50;
    pMsg.msgStats.channelAccessFailures = 0x00;
    pMsg.msgStats.macAckFailures = 0x00;
    pMsg.msgStats.otherDataRequestFailures = 0x00;
    pMsg.msgStats.syncLossIndications = 0x00;
    /* Configuration Settings Data */
    pMsg.configSettings.reportingInterval = 10000;
    pMsg.configSettings.pollingInterval = 10000;
    /* Send the message :) */
    Csf_deviceSensorDataUpdate(&pSrcAddr, rssi, &pMsg);
    DEBUG_printf("  >> End testing the API: Csf_deviceSensorDataUpdate \n");
    DEBUG_printf("=======================================================\n");

}

void Test_Csf_getDeviceArray(void)
{
    DEBUG_printf("  >> Start testing the API : Csf_getDeviceList \n");
    DEBUG_printf("  ----------------------------------------------------\n");
    int i;
    Csf_deviceInformation_t **devList = NULL;
    int numDev;

    numDev = Csf_getDeviceInformationList(devList);

    DEBUG_printf("number of connected device is %d \n", numDev);
    if(devList != NULL)
    {
        DEBUG_printf("printing dev info \n");
        /* print connected device short address */
        for(i = 0; i< numDev; i++)
        {
            DEBUG_printf("short addr of %d device is 0x%X \n", i, devList[i]->devInfo.shortAddress);
            Csf_freeDeviceInformationList(numDev, devList[i]);
        }

    }
    DEBUG_printf("  >> End testing the API: Csf_getDeviceList \n");
    DEBUG_printf("=======================================================\n");
}

void Test_Csf_networkUpdatefh(void)
{
    LOG_printf(LOG_ALWAYS," No supported (needs to be re-written)\n");
#if 0
    /* Test API

       API: extern void Csf_networkUpdate(bool restored, bool fh, void *pNetworkInfo);

    */
    DEBUG_printf(" >> Start testing the API : Csf_networkUpdate \n");
    DEBUG_printf("  ----------------------------------------------------\n");
#if 0
    bool restored = true;
    bool fh = false;
    Csf_networkInformation_t pNetworkInfo;
    /* populate the test network information data */
    pNetworkInfo.fh = false;
    pNetworkInfo.info.netInfo.deviceInfo.panID = 0xAAAA;
    pNetworkInfo.info.netInfo.deviceInfo.shortAddress = 0x1212;
    uint8_t a[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAA, 0xBB};
    memcpy(pNetworkInfo.info.netInfo.deviceInfo.extAddress, a, 8);
    pNetworkInfo.info.netInfo.channel = 0x14;
    /* Send the message */
    Csf_networkUpdate(restored, fh, &pNetworkInfo);
#endif
#if 1  /* Frequency Hopping network config. */
    /* Now send the information with frequency hopping parameters */
    bool restoredFh = true;
    bool fhFh = true;
    Csf_networkInformation_t pNetworkInfoFh;

    pNetworkInfoFh.fh = true;
    pNetworkInfoFh.info.fhNetInfo.devInfo.panID = 0xBBBB;
    pNetworkInfoFh.info.fhNetInfo.devInfo.shortAddress = 0x1212;
    uint8_t extAddr[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAA, 0xBB};
    memcpy(pNetworkInfoFh.info.fhNetInfo.devInfo.extAddress, extAddr, 8);
    /* set the broadcast dwell settings */
    pNetworkInfoFh.info.fhNetInfo.fhInfo.bcIntervals.dwell = 0x12;
    pNetworkInfoFh.info.fhNetInfo.fhInfo.bcIntervals.interval = 0x13;
    /* set the broadcast number of channels */
    pNetworkInfoFh.info.fhNetInfo.fhInfo.bcNumChans = 0x1;
    uint8_t a[1] = {9};
    pNetworkInfoFh.info.fhNetInfo.fhInfo.pBcChans = malloc(1);
    if(pNetworkInfoFh.info.fhNetInfo.fhInfo.pBcChans != NULL)
    {
        memcpy(&(pNetworkInfoFh.info.fhNetInfo.fhInfo.pBcChans[0]), a, 1);
    }
    /* set the unicast dwell settings */
    pNetworkInfoFh.info.fhNetInfo.fhInfo.unicastIntervals.dwell = 0x15;
    pNetworkInfoFh.info.fhNetInfo.fhInfo.unicastIntervals.interval = 0x16;
    /* set the unicast number of channels */
    pNetworkInfoFh.info.fhNetInfo.fhInfo.unicastNumChans = 0x1;
    uint8_t b[1] = {9};
    pNetworkInfoFh.info.fhNetInfo.fhInfo.pUnicastChans = malloc(1);
    if(pNetworkInfoFh.info.fhNetInfo.fhInfo.pUnicastChans != NULL)
    {
        memcpy(&(pNetworkInfoFh.info.fhNetInfo.fhInfo.pUnicastChans[0]), b, 1);
    }
    DEBUG_printf("  >> Network info used for test is displayed below \n");
    print_CsfNetworkInformation(&pNetworkInfoFh);
    /* Send the message */
    Csf_networkUpdate(restoredFh, &pNetworkInfoFh);

    if(pNetworkInfoFh.info.fhNetInfo.fhInfo.pUnicastChans != NULL)
    {
        free(pNetworkInfoFh.info.fhNetInfo.fhInfo.pUnicastChans);
    }
    if(pNetworkInfoFh.info.fhNetInfo.fhInfo.pBcChans != NULL)
    {
        free(pNetworkInfoFh.info.fhNetInfo.fhInfo.pBcChans);
    }

#endif

    DEBUG_printf(" >> End testing the API : Csf_networkUpdate \n");
    DEBUG_printf("=======================================================\n");
#endif
}

void Test_Csf_deviceNotActiveManual(void)
{
    /* Test API

       API: void Csf_deviceNotActiveUpdate(ApiMac_deviceDescriptor_t *pDevInfo,
       bool timeout);

    */

    uint16_t shortAddressM;
    uint8_t extAddressM[8];

    DEBUG_printf("  >> Start testing the API : Csf_deviceNotActiveUpdate \n");
    DEBUG_printf("  ----------------------------------------------------\n");
    getDeviceParam(&shortAddressM, extAddressM);
    ApiMac_deviceDescriptor_t pDevInfo;
    bool timeout = true;
    /* set up device descriptor */
    pDevInfo.panID = 0xAAAA;
    pDevInfo.shortAddress = shortAddressM;
    /* uint8_t a[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAA, 0xBB}; */
    memcpy(pDevInfo.extAddress, extAddressM, 8);
    /* send the message */
    Csf_deviceNotActiveUpdate(&pDevInfo, timeout);
    DEBUG_printf("  >> End testing the API: Csf_deviceNotActiveUpdate \n");
    DEBUG_printf("=======================================================\n");

}

/*
 *  ========================================
 *  Texas Instruments Micro Controller Style
 *  ========================================
 *  Local Variables:
 *  mode: c
 *  c-file-style: "bsd"
 *  tab-width: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 *  End:
 *  vim:set  filetype=c tabstop=4 shiftwidth=4 expandtab=true
 */

