/*
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 * EVEN IF ADVISED OF THE  POSSIBILITY OF SUCH DAMAGE.
 */

#include "my_example_app.h"
#include "threads.h"
#include "timer.h"
#include "stream.h"
#include "stream_socket.h"
#include "stream_uart.h"
#include "fatal.h"
#include "log.h"

#include "stream.h"

intptr_t hndl_uart;
intptr_t hndl_server;
intptr_t hndl_accepted;
intptr_t uart_thread_id;
intptr_t server_thread_id;

/* we have our own debug flags that can be set in the INI file */
const struct ini_flag_name log_app_flag_names[] = {
    /* terminate list */
    { .name = NULL                          }
};

const struct ini_flag_name * const log_flag_names[]= {
    log_builtin_flag_names,
    NULL
};

static int _safe_ascii(int c)
{
    /* make this char ok to print */
    if((c < 0x20) || (c >= 0x7f)){
        c = '.';
    }
    return c;
}

static intptr_t uart_thread_func(intptr_t cookie)
{
    (void)(cookie); /* not used */
    int c;

    hndl_uart = STREAM_createUart(&(ALL_INI_UARTS[0]));
    if(hndl_uart == 0){
        FATAL_printf("Cannot open uart\n");
    }

    for(;;){

        c = STREAM_fgetc(hndl_uart);
        LOG_printf(LOG_ALWAYS, "UART: rx 0x%02x (%c)\n",  c, _safe_ascii(c));

        STREAM_printf(hndl_uart, "UART: you typed: 0x%02x (%c)\n", c, _safe_ascii(c));
    }
    /* not reachable */
    return 0;
}

static intptr_t sock_thread_func(intptr_t cookie)
{
    (void)(cookie); /* not used */
    int r;
    int c;

    hndl_server = SOCKET_SERVER_create(&ALL_INI_SOCKETS[0]);
    if(hndl_server == 0){
        FATAL_printf("Cannot create server socket\n");
    }

    r = SOCKET_SERVER_listen(hndl_server);
    if(r != 0){
        FATAL_printf("Cannot make server listen\n");
    }

 accept_loop:
    for(;;){
        r = SOCKET_SERVER_accept(&hndl_accepted, hndl_server, 1000);
        if(r == 1){
            goto connect_loop;
        }
        if(r == 0){
            LOG_printf(LOG_ALWAYS, "no connection yet\n");
            continue;
        }
        if(r < 0){
            FATAL_printf("Cannot accept?\n");
        }
    }

 connect_loop:
    for(;;){
        if(STREAM_isError(hndl_accepted)){
            LOG_printf(LOG_ALWAYS, "client disconnected\n");
            SOCKET_destroy(hndl_accepted);
            goto accept_loop;
        }

        c = STREAM_fgetc(hndl_accepted);
        if(c == -1){
            printf("EOF EOF\n");
        }
        LOG_printf(LOG_ALWAYS, "SOCKET: rx 0x%02x (%c)\n",  c, _safe_ascii(c));

        STREAM_printf(hndl_accepted, "SOCKET: you typed: 0x%02x (%c)\n", c, _safe_ascii(c));
    }
    /* not reachable */
    return 0;
}

void APP_Main(void)
{

    uart_thread_id   = THREAD_create("uart-thread", uart_thread_func, 0, THREAD_FLAGS_DEFAULT);
    server_thread_id = THREAD_create("sock-thread", sock_thread_func, 0, THREAD_FLAGS_DEFAULT);

    for(;;){
        TIMER_sleep(1000);
        /* we have nothing to do */
    }
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
