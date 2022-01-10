/******************************************************************************
 @file client_test.c

 @brief TIMAC 2.0 API Unit test for the client socket abstraction

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

/*!
 * OVERVIEW
 * ========
 * This is by no means a complete API test, its purpose is to show
 * the intent of the the abstraction, and to perform some basic checks
 * against the abstraction.
 */

#include "stream.h"
#include "stream_socket.h"
#include "log.h"
#include "fatal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    char buf[1000];
    char *cp;
    intptr_t h_client;
    struct socket_cfg cfg;
    int r;

    STREAM_init();
    SOCKET_init();
    LOG_init("/dev/stderr");
    log_cfg.log_flags = LOG_EVERYTHING;

    /*========================================
     * There are many examples of making HTTP requests via telnet
     * What the client socket is, is exactly something like telnet
     *
     * So to test this, we simply connect to a well known web server
     * and try to do an HTTP get request.
     *
     * For more details, see this google search
     * ---------------------------------------
     * https://www.google.com/search?q=http+request+using+telnet
     *
     *========================================
     *
     * Note: Internally at TI, we are behind a non-transparent proxy server
     * So - internally at TI, this specific address will fail.
     *
     * You, our customer might also be behind an internal firewall or proxy
     *
     * This code could (internal to TI) connect to the TI proxy server,
     * and negotiate that is beyond the scope of a simple telnet client test
     * however that is not really needed for test purposes
     *
     * For  test purposes, there is an easy solution :-)
     * Externally (ie: at home) I do not have proxy server, it works.
     * (or perhaps my ISP has a transparent proxy/cache, many ISPs do this)
     *
     * At home this works and I get an HTTP Redirect to "https://www.yahoo.com"
     */

    memset((void *)(&cfg), 0, sizeof(cfg));
    cfg.ascp = 'c';

    /* let user specify server name from command line */
    if(argc == 2)
    {
        cfg.host = argv[1];
    }
    else
    {
        cfg.host = "www.yahoo.com";
    }

    cfg.service = "80"; /* http */
    cfg.server_backlog = 0;
    cfg.device_binding = NULL;
    cfg.inet_4or6 = 4;
    cfg.connect_timeout_mSecs = 3000;

    h_client = SOCKET_CLIENT_create(&cfg);
    if(h_client == 0)
    {
        FATAL_printf("Cannot init client\n");
    }

    r = SOCKET_CLIENT_connect(h_client);
    if(r != 0)
    {
        FATAL_printf("Cannot make socket connect\n");
    }

    /* Send our http request */
    STREAM_printf(h_client,
                   "GET / HTTP/1.1\r\n"
                   "Host: %s\r\n"
                   "\r\n", cfg.host);

    /* read until remote end closes */
    for(;;)
    {
        cp = STREAM_fgets(buf, sizeof(buf), h_client);
        if(cp)
        {
            cp = strchr(buf, '\r');
            if(cp) *cp = 0;
            cp = strchr(buf, '\n');
            if(cp) *cp = 0;

            LOG_printf(LOG_ALWAYS, "rx: %s\n", buf);
        }
        else
        {
            break;
        }
    }

    STREAM_close(h_client);
    SOCKET_destroy(h_client);

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
