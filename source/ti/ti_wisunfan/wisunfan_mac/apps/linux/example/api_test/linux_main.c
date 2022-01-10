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

#include "ini_file.h"       /* this reads our ini file */
#include "log.h"            /* our logging scheme */
#include "timer.h"
#include "stream.h"
#include "stream_socket.h"  /* we use a socket in our app */
#include "stream_uart.h"    /* and a uart. */

#include <stdio.h>
#include <stdlib.h>

const char *nv_filename = NULL;

static int NV_INI_settings(struct ini_parser *pINI, bool *handled)
{
    if(!INI_itemMatches(pINI, "nvram", "filename"))
    {
        return 0;
    }

    *handled = true;

    nv_filename = INI_itemValue_strdup(pINI);
    if(nv_filename)
    {
        LOG_printf(LOG_ALWAYS, "NV Filename: %s\n", nv_filename);
    }
    return 0;
}

static ini_rd_callback * const ini_cb_table[] = {
    LOG_INI_settings,
    UART_INI_settingsNth,
    SOCKET_INI_settingsNth,
    NV_INI_settings,
    /* Terminate list */
    NULL
};

static int cfg_callback(struct ini_parser *pINI, bool *handled)
{
    int x;
    int r;

    for(x = 0 ; ini_cb_table[x] ; x++)
    {
        r = (*(ini_cb_table[x]))(pINI, handled);
        if(*handled)
        {
            return r;
        }
    }
    /* let the system handle it */
    return 0;
}

int main(int argc, char **argv)
{
    int r;

    if(argc != 2)
    {
        fprintf(stderr, "usage: %s CONFIGFILE\n", argv[0]);
        exit(1);
    }

    /* Basic initialization */
    STREAM_init();
    TIMER_init();
    LOG_init("/dev/stderr");
    /* we want these logs to begin with */
    log_cfg.log_flags = LOG_FATAL | LOG_WARN | LOG_ERROR;

    /* Rad our configuration file */
    r = INI_read(argv[1], cfg_callback, 0);
    if(r != 0)
    {
        fprintf(stderr, "error parsing cfg file\n");
        exit(1);
    }

    APP_Main();

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
