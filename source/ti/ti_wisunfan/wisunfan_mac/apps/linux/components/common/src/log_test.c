/******************************************************************************
 @file log_test.c

 @brief TIMAC 2.0 API Unit test for the log code

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

#include "log.h"
#include "stream.h"

#include <stdio.h>
#include <stdlib.h>
#include "bitsnbits.h"
#include "timer.h"

int main(int argc, char **argv)
{
    int x;
    (void)(argc);
    (void)(argv);
    STREAM_init();
    LOG_init("foo.log");

    LOG_printf(LOG_ALWAYS, "Hi %s\n", "mom");

    /* try turning log flags on and off */
    log_cfg.log_flags = 0;
    LOG_printf(_bit3, "Should not print\n");
    log_cfg.log_flags = _bit3;
    LOG_printf(_bit3, "Should YES print\n");

    /* Try some hex dumps */
    LOG_hexdump(LOG_ALWAYS, 0x12345678,
                 "this is my big long test string foo bar",  39);
    LOG_hexdump(LOG_ALWAYS, 0x0123456789abcdef,
                 "tiny\xffstring", 11);
    LOG_hexdump(LOG_ALWAYS, 0xfedcba9876543210,
                 "this is the really long string with a big address The End->",
                 59);

    /* see if our log timestamp is delayed due to sleep */
    for(x = 0; x < 10; x++)
    {
        TIMER_sleep(100);
        LOG_printf(LOG_ALWAYS, "This should be 100msecs later\n");
    }
    LOG_close();
    /* great succes */
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
