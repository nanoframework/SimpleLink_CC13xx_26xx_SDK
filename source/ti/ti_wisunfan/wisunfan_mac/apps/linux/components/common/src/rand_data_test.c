/******************************************************************************
 @file rand_data_test.c

 @brief TIMAC 2.0 - Unit test for random data generator

 To be clear: this is not a cryptographic random solution
 this is just to simplify creating random test data to
 test a communications channel nothing more.

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

#include "compiler.h"
#include "rand_data.h"
#include "stream.h"
#include "log.h"
#include "fatal.h"

#include "stdlib.h"

uint8_t buf[ 64 ];

int main(int argc, char **argv)
{
    struct rand_data_pair dut;
    int r;
    int who;

    (void)argc;
    (void)argv;
    STREAM_init();
    LOG_init("/dev/stdout");

    /* Inialize our device under test.. */
    RAND_DATA_initPair(&dut, 0x12345678);

    /* create & print our data */
    RAND_DATA_txGenerate(&dut, buf, sizeof(buf));
    LOG_printf(LOG_ALWAYS, "Simulated perfect TX Data is:\n");
    LOG_hexdump(LOG_ALWAYS, 0, (const void *)(&buf[0]), sizeof(buf));

    /* Did we get what we wanted? */
    r = RAND_DATA_rxVerify(&dut, buf, sizeof(buf));
    if(r != 0)
    {
        FATAL_printf("test failure\n");
    }
    LOG_printf(LOG_ALWAYS,"Test(good) passes\n");

    /* we force an error */
    RAND_DATA_initPair(&dut, 0xdeadbeef);

    RAND_DATA_txGenerate(&dut, buf, sizeof(buf));
    LOG_printf(LOG_ALWAYS, "Simulated (non-modified) TX Data is:\n");
    LOG_hexdump(LOG_ALWAYS, 0, (const void *)(&buf[0]), sizeof(buf));

    /* Modify some value in the stream */
    who = 42; /* answer to life, the universe, etc... */
    buf[ who ] ^= who;
    if(who >= sizeof(buf))
    {
        FATAL_printf("error simulation must be within the buffer!\n");
    }
    LOG_printf(LOG_ALWAYS, "After modifying byte %d (0x%02x)\n", who, who);
    LOG_hexdump(LOG_ALWAYS, 0, (const void *)(&buf[0]), sizeof(buf));

    /* make sure it fails where we expect it to fail */
    r = RAND_DATA_rxVerify(&dut, buf, sizeof(buf));
    r = -r;
    r--;
    if(r != 42)
    {
        FATAL_printf("forced error test fails\n");
    }
    LOG_printf(LOG_ALWAYS, "Test (forced error) passes\n");

    LOG_printf(LOG_ALWAYS, "Test Success\n");
    exit(0);
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

