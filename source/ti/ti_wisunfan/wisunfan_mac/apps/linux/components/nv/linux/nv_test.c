/******************************************************************************
 @file nv_test.c

 @brief TIMAC 2.0 API Linux test driver for NV module

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

// main file to test the NV driver, csf functionality, ui interface

#include <stdio.h>
#include "log.h"
#include "fatal.h"

#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

// NV Driver
#include "nvintf.h"
#include "nv_linux.h"
#include "nvocmp.h"

#include "hlos_specific.h"
/*****************************************************************************
 Constants and definitions
******************************************************************************/

/*
 * Application specific log flag constants are defined here.
 * There are none for this app.
 *
 * See "log.h" for details.
 */
const struct ini_flag_name * const log_flag_names[] = {
    log_builtin_flag_names,
    NULL
};

/******************************************************************************
 Variables
******************************************************************************/
/* Non-volatile function pointers */
NVINTF_nvFuncts_t nvFps;

/*
 * @brief Quick test function for the NV module
 *
 * Note: The multi-page NV driver will raise an alert messages stating that
 *       there is "Corrupted or Version/Signature mismatch" on initialization.
 *       This is correct behavior on initialization, since this is the process
 *       of generating the required page header required for the multi-page
 *       NV driver. For more informaition, see nvocmp.c documentation or the
 *       use of the multi-page NV driver in the SimpleLink 13x2/26x2 TI 15.4
 *       Stack example projects.
 *
 */
static void Quick_Test_NV(void)
{
    int testNum = 1;

    /* Load function pointers */
    LOG_printf(LOG_ALWAYS, "%d.Testing API: NVOCMP_loadApiPtrs() \n", testNum++);
    NVOCMP_loadApiPtrs(&nvFps);
    LOG_printf(LOG_ALWAYS, "\n");

    /* Test the NV API Functions */
    LOG_printf(LOG_ALWAYS,"%d.Testing API: nvFps.initNV() \n", testNum++);
    /*! Initialization function */
    nvFps.initNV(NULL);
    LOG_printf(LOG_ALWAYS,"\n");

    /*! Compact NV function */
    LOG_printf(LOG_ALWAYS,"%d.Testing API: nvFps.compactNV() \n", testNum++);
    LOG_printf(LOG_ALWAYS,"Force compaction by sending '0' as input parameter \n");
    nvFps.compactNV(0);
    LOG_printf(LOG_ALWAYS, "\n");

    /*! Create item function */
    LOG_printf(LOG_ALWAYS, "%d.Testing API: nvFps.createItem() \n", testNum++);
    LOG_printf(LOG_ALWAYS, "Create an NV item with id = 0x0001 \n");
    NVINTF_itemID_t id_a = { 0x10, 0x0001, 0x0001 };
    uint32_t bLen_a = 10;
    uint8_t pBuf_a[10] = {1,2,3,4,5,6,7,8,9,10};
    nvFps.createItem(id_a, bLen_a, pBuf_a);
    LOG_printf(LOG_ALWAYS, "\n");

    LOG_printf(LOG_ALWAYS, "Create an NV item with id = 0x0002 \n");
    NVINTF_itemID_t id_b = { 0x10, 0x0002, 0x0002 };
    uint32_t bLen_b = 15;
    uint8_t pBuf_b[15] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    nvFps.createItem(id_b, bLen_b, pBuf_b);
    LOG_printf(LOG_ALWAYS, "\n");

    LOG_printf(LOG_ALWAYS, "Create an NV item with id = 0x0003 \n");
    NVINTF_itemID_t id_c = { 0x10, 0x0003, 0x0003 };
    uint32_t bLen_c = 20;
    uint8_t pBuf_c[20] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    nvFps.createItem(id_c, bLen_c, pBuf_c);
    LOG_printf(LOG_ALWAYS, "\n");

    LOG_printf(LOG_ALWAYS, "%d.Testing API: nvFps.getItemLen() \n", testNum++);
    /*! Get item length function */
    LOG_printf(LOG_ALWAYS, "Reading back id = 0x0003, length should be 20 \n");
    uint32_t readBackItemLen =  nvFps.getItemLen(id_c);
    LOG_printf(LOG_ALWAYS, "Length read back = %d \n", readBackItemLen);
    if(readBackItemLen == 20)
    {
            LOG_printf(LOG_ALWAYS, "Test Sucessfull\n");
    }
    else
    {
        FATAL_printf("Test Unsucessfull\n");
    }
    LOG_printf(LOG_ALWAYS, "\n");

    LOG_printf(LOG_ALWAYS, "%d.Testing API: nvFps.readItem() \n", testNum++);
    LOG_printf(LOG_ALWAYS, "Reading back value of item 3, it should be number from 1 to 20 \n");
    uint8_t data[20];
    /*! Read item function */
    LOG_printf(LOG_ALWAYS, "Reading back \n");
    nvFps.readItem(id_c, 0, (uint16_t)readBackItemLen , data);
    LOG_printf(LOG_ALWAYS, "Data read from the NV file is : ");
#if 0
    for(i = 0; i < readBackItemLen; i++)
    {
        /* display the data */
            LOG_printf(LOG_ALWAYS, "%d ", data[i]);
    }
#endif
    LOG_printf(LOG_ALWAYS,"\n");
    LOG_hexdump(LOG_ALWAYS, 0, data, readBackItemLen);
    LOG_printf(LOG_ALWAYS, "\n");

    /*! Delete NV item function */
    LOG_printf(LOG_ALWAYS, "%d.Testing API: nvFps.deleteItem() \n", testNum++);
    LOG_printf(LOG_ALWAYS, "deleting item 3 \n");
    /*! Delete NV item function */
    nvFps.deleteItem(id_c);
    /* Now try to read the item again */
    uint8_t returnValueDeleteItemReadBack = nvFps.readItem(id_c, 0, (uint16_t)readBackItemLen , data);
    LOG_printf(LOG_ALWAYS, "When trying to read a deleted item we get 0x%x as return value \n", returnValueDeleteItemReadBack);
    LOG_printf(LOG_ALWAYS, "The NV Item has been deleted \n");
    LOG_printf(LOG_ALWAYS, "\n");
    LOG_printf(LOG_ALWAYS, "\nTest Complete\n");
}

/*
 * @brief callback for configuration file parser.
 * @param p - parser information
 * @param handled - flag to indicate the cfg item was handled by this function.
 * @return non-zero means stop processing due to error.
 */
static int ini_callback(struct ini_parser *p, bool *handled)
{
    int r;
    r = LOG_INI_settings(p, handled);
    if((r != 0) || (*handled))
    {
        return (r);
    }

    r = NV_LINUX_INI_settings(p, handled);
    return (r);
}

/*
 * @brief Main test function
 *
 * @param argc - standard argument count
 * @param argv - standard argument vector.
 *
 * @return does nothing, see _Exit_Pause(0)
 */
int main(int argc, char **argv)
{
    (void)argv;
    (void)argc;

    /* Test NV Function */
    LOG_init("/dev/stdout");
    INI_read("nv_test.ini", ini_callback, 0);
    Quick_Test_NV();
    _EXIT_pause(0);
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

