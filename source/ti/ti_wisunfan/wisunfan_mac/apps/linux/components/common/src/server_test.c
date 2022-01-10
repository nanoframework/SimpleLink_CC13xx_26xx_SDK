/******************************************************************************
 @file server_test.c

 @brief TIMAC 2.0 API Unit test for the socket server code

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
#include "stream.h"
#include "stream_socket.h"
#include "log.h"
#include "fatal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* make the char printable for debug purposes */
static int _safe_ascii(int c)
{
    if((c >= 0x20) && (c <= 0x7e))
    {
        return (c);
    }
    else
    {
        return ('.');
    }
}

int main(int argc, char **argv)
{
    intptr_t h_listen;
    intptr_t h_accept;
    struct socket_cfg cfg;
    int r;
    int c;

    (void)(argc);
    (void)(argv);
    h_accept = 0;
    h_listen = 0;

    STREAM_init();
    LOG_init("/dev/stderr");
    SOCKET_init();
    log_cfg.log_flags = LOG_EVERYTHING;

    /* setup our socket configuration */
    memset((void *)(&cfg), 0, sizeof(cfg));
    cfg.ascp = 's';
    cfg.host = NULL;
    /* we listen here on localhost */
    cfg.service = "9999";
    cfg.server_backlog = 1;
    cfg.device_binding = NULL;
    cfg.inet_4or6 = 4;

    /* standard BSD type create, listen, accept */
    h_listen = SOCKET_SERVER_create(&cfg);
    if(h_listen == 0)
    {
        FATAL_printf("Cannot init server\n");
    }

    r = SOCKET_SERVER_listen(h_listen);
    if(r != 0)
    {
        FATAL_printf("Cannot make socket listen\n");
    }

 accept_agian:
    if(h_accept)
    {
        SOCKET_destroy(h_accept);
        h_accept = 0;
    }

    for(;;)
    {
        /* we poll here for connections */
        LOG_printf(LOG_ALWAYS, "Waiting for new connection\n");
        r = SOCKET_SERVER_accept(&h_accept, h_listen, 5000);
        if(r == 0)
        {
            continue;
        }
        LOG_printf(LOG_ALWAYS, "We have a connection\n");
        break;
    }

    /* if the client sends "+++" in a row we disconnect */
    /* and wait for another connection */
    int pluscount = 0;
    for(;;)
    {
        if(!STREAM_SOCKET_isConnected(h_accept))
        {
            LOG_printf(LOG_ALWAYS, "disconnect!\n");
            STREAM_close(h_accept);
            goto accept_agian;
        }

        c = STREAM_fgetc(h_accept);
        if(c == EOF)
        {
            LOG_printf(LOG_ALWAYS, "\n RX - EOF - stopping\n");
            STREAM_close(h_accept);
            goto accept_agian;
        }

        /* did remote side tell us to hangup? */
        if(c == '+')
        {
            pluscount++;
            LOG_printf(LOG_ALWAYS, "\nPLUS COUNT = %d\n", pluscount);
            if(pluscount == 3)
            {
                STREAM_close(h_accept);
                goto accept_agian;
            }
        }

        /* Reply to the user what they typed */
        STREAM_printf(h_accept,
                       "You typed: 0x%02x (%c)\n", c, _safe_ascii(c));
        /* and print it locally so we can see activity */
        LOG_printf(LOG_ALWAYS, "RX 0x%02x (%c)\n", c, _safe_ascii(c));
    }

    /* will never happen */
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

