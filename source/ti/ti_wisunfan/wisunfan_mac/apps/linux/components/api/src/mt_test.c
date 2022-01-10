/******************************************************************************
 @file mt_test.c

 @brief TIMAC 2.0 API TIMAC 2.0 Unit test for MT_MSG (Linux Implimentation)

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
#include "mt_msg.h"
#include "fatal.h"
#include "ini_file.h"       /* this reads our ini file */
#include "log.h"            /* our logging scheme */
#include "timer.h"
#include "stream.h"
#include "stream_socket.h"
#include "stream_uart.h"    /* and a uart. */
#include "debug_helpers.h"
#include "rand_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct uart_cfg my_uart_cfg = {
    .devname = NULL,
    .baudrate = 115200,
    .open_flags = 0
};

struct socket_cfg my_socket_cfg = {
    .inet_4or6 = 4,
    .ascp = 'c',
    .host = "localhost",
    .service = "1310",
    .server_backlog = 1,
    .device_binding = NULL
};

struct mt_msg_interface test_interface = {
    .dbg_name = "mt-iface",
    .hndl = 0, /* done later */
    .u_cfg = NULL,
    .s_cfg = NULL,
    .rx_thread = 0, /* done later */
    .tx_frag_size = 0, /* use the default */
    .retry_max = 0,
    .frag_timeout_mSecs = 2000,
    .intersymbol_timeout_mSecs = 500,
    .srsp_timeout_mSecs = 2000,
    .intermsg_timeout_mSecs = 60 * 1000,

    .is_dead = false,

    .stack_id = 0,
    .len_2bytes = false,
};

/* we have our own debug flags that can be set in the INI file */
const struct ini_flag_name * const log_flag_names[] = {
    NULL
};


struct mt_custom_cmd {
    const char *name;
    int cmd0;
    int cmd1;
    int len;
    uint8_t databuffer[4096];
    struct mt_custom_cmd *pNext;
};

#define MAX_CUSTOM_CMDS  30
static struct debug_menu_item custom_cmds[MAX_CUSTOM_CMDS];
static void do_custom_cmd(const struct debug_menu_item *p, intptr_t cookie);
static int n_custom_cmds = 0;

static int finish_cmd(struct ini_parser *pINI, struct mt_custom_cmd *pC)
{
    if (pC->name == NULL) {
        INI_syntaxError(pINI, "missing name\n");
        return -1;
    }
    if (pC->cmd0 == -1) {
        INI_syntaxError(pINI, "missing cmd0\n");
        return -1;
    }
    if (pC->cmd1 == -1) {
        INI_syntaxError(pINI, "missing cmd1\n");
        return -1;
    }
    // all is well
    n_custom_cmds++;
    if (n_custom_cmds >= MAX_CUSTOM_CMDS) {
        INI_syntaxError(pINI, "too many custom cmds\n");
        return -1;
    }
    return 0;
}


static int my_CUSTOM_CMD_ini(struct ini_parser *pINI, bool *handled)
{
    struct ini_numlist bytelist;
    struct mt_custom_cmd *pC;

    pC = (struct mt_custom_cmd *)(custom_cmds[n_custom_cmds].cookie);

    if (!INI_itemMatches(pINI, "custom-cmd", NULL)) {
        if (pC) {
            return finish_cmd(pINI, pC);
        }
        return 0;
    }

    if (pINI->item_value == NULL) {
        if (pC) {
            return finish_cmd(pINI, pC);
        }
        return 0;
    }

    if (pC == NULL) {
        pC = (struct mt_custom_cmd *)calloc(1, sizeof(struct mt_custom_cmd));
        if (pC == NULL) {
            FATAL_printf("Dead, no memory\n");
        }
        custom_cmds[n_custom_cmds].cookie = (intptr_t)(pC);
        custom_cmds[n_custom_cmds].handler = do_custom_cmd;
        pC->name = NULL;
        pC->cmd0 = -1;
        pC->cmd1 = -1;
        pC->len = 0;
    }

    if (pINI->item_value == NULL) {
        // at the section line
        return 0;
    }

    if (INI_itemMatches(pINI, NULL, "name")) {
        if (pC->name) {
            INI_syntaxError(pINI, "already has a name\n");
        }
        pC->name = strdup(pINI->item_value);
        custom_cmds[n_custom_cmds].txt = pC->name;
        if (pC->name == NULL) {
            FATAL_printf("no memory?\n");
        }
        *handled = true;
        return 0;
    }

    if (INI_itemMatches(pINI, NULL, "cmd0")) {
        if (pC->cmd0 >= 0) {
            INI_syntaxError(pINI, "already has cmd0\n");
        }
        pC->cmd0 = INI_valueAsInt(pINI);
        if ((pC->cmd0 < 0) && (pC->cmd0 > 255)) {
            INI_syntaxError(pINI, "invalid cmd0 range\n");
        }
        *handled = true;
        return 0;
    }


    if (INI_itemMatches(pINI, NULL, "cmd1")) {
        if (pC->cmd1 >= 0) {
            INI_syntaxError(pINI, "already has cmd1\n");
        }
        pC->cmd1 = INI_valueAsInt(pINI);
        if ((pC->cmd1 < 0) && (pC->cmd1 > 255)) {
            INI_syntaxError(pINI, "invalid cmd1 range\n");
        }
        *handled = true;
        return 0;
    }

    if (!INI_itemMatches(pINI, NULL, "bytes")) {
        INI_syntaxError(pINI, "unknown keyword\n");
        return -1;
    }

    INI_valueAsNumberList_init(&bytelist, pINI);
    while (EOF != INI_valueAsNumberList_next(&bytelist)) {
        if ((bytelist.value < 0) || (bytelist.value > 255)) {
            INI_syntaxError(pINI, "invalid byte value\n");
            return -1;
        }
        pC->databuffer[pC->len] = (uint8_t)(bytelist.value);
        pC->len += 1;
        if (pC->len > sizeof(pC->databuffer)) {
            INI_syntaxError(pINI, "too many bytes in transfer buffer\n");
            return -1;
        }

    }
    *handled = true;
    return 0;
}

static void do_custom_cmd(const struct debug_menu_item *pMI, intptr_t cookie)
{
    struct mt_custom_cmd *pC;
    struct mt_msg *pMsg;
    int r;

    (void)(cookie);
    pC = (struct mt_custom_cmd *)(pMI->cookie);

    pMsg = MT_MSG_alloc(pC->len, pC->cmd0, pC->cmd1);
    pMsg->pLogPrefix = pMI->txt;
    MT_MSG_setDestIface(pMsg, &(test_interface));
    if (pC->len) {
        MT_MSG_wrBuf_DBG(pMsg, (const void *)(pC->databuffer), (size_t)(pC->len), "payload");
    }

    LOG_printf(LOG_ALWAYS, "Transmit Custom Cmd: %s\n", pC->name);
    r = MT_MSG_txrx(pMsg);
    LOG_printf(LOG_ALWAYS, "result=%d\n", r);
    if (r == 2)
    {
        LOG_printf(LOG_ALWAYS, "success\n");
    }
    else {
        LOG_printf(LOG_ERROR, "FAILURE\n");
    }
}


static void mt_init_test(const struct debug_menu_item *pMI, intptr_t cookie)
{
    struct mt_msg *pMsg;
    int r;

    (void)(pMI);
    (void)(cookie);

    /* Try some commands */
    /* 22/02 = MAC_INIT() */
    pMsg = MT_MSG_alloc(0, 0x22, 0x02);
    MT_MSG_setDestIface(pMsg, &test_interface);

    LOG_printf(LOG_ALWAYS, "Transmit MAC_INIT\n");
    r = MT_MSG_txrx(pMsg);
    LOG_printf(LOG_ALWAYS, "result=%d\n", r);
    if (r != 2)
    {
        LOG_printf(LOG_ERROR, "MT_MSG_INIT fails r=%d\n", r);
        goto done;
    }

    r = MT_MSG_rdU8(pMsg->pSrsp);
    MT_MSG_parseComplete(pMsg->pSrsp);
    if (pMsg->pSrsp->is_error)
    {
        FATAL_printf("Parse error\n");
    }
    LOG_printf(LOG_ALWAYS, "MAC_INIT status = 0x%02x\n", r);
done:
    MT_MSG_free(pMsg);
}

static void mt_purge_test(const struct debug_menu_item *pMI, intptr_t cookie)
{
    struct mt_msg *pMsg;
    struct mt_msg *pWORK;
    int r;
    int x;
    int y;

    (void)(pMI);
    (void)(cookie);

    LOG_printf(LOG_ALWAYS, "send purge request to an invalid handle (testing the areq flow)\n");
    pMsg = MT_MSG_alloc(1, 0x22, 0x0e);
    MT_MSG_setDestIface(pMsg, &test_interface);

    /* we generally do not have any data items pending */
    /* so we are expecting an invalid handle reply. */
    MT_MSG_wrU8(pMsg, 0x42);

    r = MT_MSG_txrx(pMsg);
    if (r != 2)
    {
        FATAL_printf("expected 2 transfers\n");
    }
    pWORK = pMsg->pSrsp;

    r = MT_MSG_rdU8(pWORK);
    MT_MSG_parseComplete(pMsg->pSrsp);
    if (pWORK->is_error)
    {
        FATAL_printf("parse error\n");
    }
    LOG_printf(LOG_ALWAYS, "purge status: 0x%02x\n", r);

    MT_MSG_free(pMsg);

    pMsg = MT_MSG_LIST_remove(&test_interface, &(test_interface.rx_list), 1000);
    if (pMsg == NULL)
    {
        FATAL_printf("No PURGE CNF response\n");
    }

    if ((pMsg->cmd0 == 0x42) && (pMsg->cmd1 == 0x90) && (pMsg->expected_len == 2))
    {
        /* all is well (below) */
    }
    else
    {
        FATAL_printf("not purge_cnf reply\n");
    }
    x = MT_MSG_rdU8(pMsg);
    y = MT_MSG_rdU8(pMsg);
    MT_MSG_parseComplete(pMsg);
    if (pMsg->is_error)
    {
        FATAL_printf("parse error\n");
    }
    LOG_printf(LOG_ALWAYS, "purge status: 0x%02x, handle 0x%02x\n", x, y);

    MT_MSG_free(pMsg);
}

static uint8_t loopback_tx_buffer[__4K];
static int loopback_n_times = 10;
static int loopback_rand_max = 1024;
static int loopback_repeat = 0;
static int loopback_length = 100;
static int loopback_rate = 10 * 1000;
static int loopback_seed = 0x12345678;
struct rand_data_pair loopback_generator;

static void mt_loopback_test2(const struct debug_menu_item *pMI, intptr_t cookie)
{
    int x;
    int l;
    int r;

    (void)(cookie);
    (void)(pMI);
    while (1)
    {
        DEBUG_printf("\n");
        DEBUG_get_integer2("N-Times: ", &loopback_n_times, loopback_n_times);
        DEBUG_get_integer2("RandSizeMax: ", &loopback_rand_max, loopback_rand_max);
        DEBUG_get_integer2("seed", &loopback_seed, loopback_seed);

        if ((loopback_n_times < 0) || (loopback_n_times < 1))
        {
            DEBUG_printf("invalid\n");
            continue;
        }
        else
        {
            break;
        }
    }

    for (x = 0; x < loopback_n_times; x++)
    {
        l = rand() + 1;
        l = l % loopback_rand_max;

        LOG_printf(LOG_ALWAYS, "==================================\n");
        LOG_printf(LOG_ALWAYS, "[ LOOP BACK REPEAT %d of %d\n", x, loopback_n_times);
        LOG_printf(LOG_ALWAYS, "==================================\n");
        /* create our data */
        RAND_DATA_initPair(&loopback_generator, (uint32_t)(loopback_seed));
        RAND_DATA_generateBuf(&loopback_generator.tx, loopback_tx_buffer, sizeof(loopback_tx_buffer));
        /* next time use a new seed */
        loopback_seed = (int)(loopback_generator.tx.next);;

        /* send the loopback message :-) */
        r = MT_MSG_loopback(&test_interface, 0, loopback_rate, loopback_length, loopback_tx_buffer);
        if (r == 2)
        {
            LOG_printf(LOG_ALWAYS, "LOOPBACK Success\n");
        }
        else
        {
            LOG_printf(LOG_ALWAYS, "LOOPBACK Failure\n");
        }
    }
}

static void mt_loopback_test(const struct debug_menu_item *pMI, intptr_t cookie)
{
    (void)(pMI);
    (void)(cookie);
    int r;
    /* make these persistant */

    while (1)
    {
        DEBUG_printf("\n");

        DEBUG_get_integer2("repeat", &loopback_repeat, loopback_repeat);
        if ((loopback_repeat < 0) || (loopback_repeat > 254))
        {
            DEBUG_printf("Loop back is out of range [0..254]\n");
            continue;
        }

        DEBUG_get_integer2("length", &loopback_length, loopback_length);
        if ((loopback_length < 0) || (loopback_length > 4090))
        {
            DEBUG_printf("Loop back is out of range [0..4090]\n");
            continue;
        }

        DEBUG_get_integer2("rate", &loopback_rate, loopback_rate);
        if ((loopback_rate < 0) || (loopback_rate > 2000000))
        {
            DEBUG_printf("Loop back is out of range [0..200000 (200 seconds)]\n");
            continue;
        }

        DEBUG_get_integer2("seed", &loopback_seed, loopback_seed);
        break;
    }

    /* create our data */
    RAND_DATA_initPair(&loopback_generator, (uint32_t)(loopback_seed));
    RAND_DATA_generateBuf(&loopback_generator.tx,
        loopback_tx_buffer,
        sizeof(loopback_tx_buffer));
    /* next time use a new seed */
    loopback_seed = (int)(loopback_generator.tx.next);;

    /* send the loopback message :-) */
    r = MT_MSG_loopback(&test_interface,
        loopback_repeat,
        loopback_rate,
        loopback_length,
        loopback_tx_buffer);
    if (r == 2)
    {
        LOG_printf(LOG_ALWAYS, "LOOPBACK Success\n");
    }
    else
    {
        LOG_printf(LOG_ALWAYS, "LOOPBACK Failure\n");
    }

}

static void mt_version_test(const struct debug_menu_item *pMI,
    intptr_t extra_cookie)
{
    (void)(pMI);
    (void)extra_cookie;
    int r;
    struct mt_version_info vi;

    r = MT_MSG_getVersion(&test_interface, &vi);
    if (r == 2)
    {
        LOG_printf(LOG_ALWAYS, "Transport: %d\n", vi.transport);
        LOG_printf(LOG_ALWAYS, "  Product: %d\n", vi.product);
        LOG_printf(LOG_ALWAYS, "    Major: %d\n", vi.major);
        LOG_printf(LOG_ALWAYS, "    Minor: %d\n", vi.minor);
        LOG_printf(LOG_ALWAYS, "    Maint: %d\n", vi.maint);
    }
    else
    {
        LOG_printf(LOG_ERROR, "r=%d, did not get version\n", r);
    }
}

static void DEBUG_submenu_wrap(const struct debug_menu_item *pMI, intptr_t extra_cookie)
{
    if (custom_cmds[0].txt == NULL) {
        DEBUG_printf("\n\n There are no custom commands\n\n");
        return;
    }
    DEBUG_submenu(pMI, extra_cookie);
}


const struct debug_menu_item my_debug_menu[] = {

    {.txt = "init_test",.handler = mt_init_test },
    {.txt = "version",.handler = mt_version_test },
    {.txt = "purge_test",.handler = mt_purge_test },
    {.txt = "loopback-test",.handler = mt_loopback_test },
    {.txt = "repeat-loop",.handler = mt_loopback_test2 },
    {.txt = "custom-cmds",.handler = DEBUG_submenu_wrap,.cookie = (intptr_t)(&custom_cmds) },

    /* terminate */
    {.txt = NULL,.handler = NULL }
};

static void test_main(void)
{
    int r;

    MT_MSG_init();

    r = MT_MSG_interfaceCreate(&test_interface);
    if (r != 0)
    {
        FATAL_printf("Cannot create interface\n");
    }

    for (;;)
    {
        DEBUG_menu(my_debug_menu, 0);
    }
}

static int my_UART_INI_settings(struct ini_parser *pINI, bool *handled)
{
    int r;

    r = 0;
    if (INI_itemMatches(pINI, "uart", NULL))
    {
        r = UART_INI_settingsOne(pINI, handled, &my_uart_cfg);
    }
    return (r);
}

static int my_SOCKET_INI_settings(struct ini_parser *pINI, bool *handled)
{
    int r;
    r = 0;
    if (INI_itemMatches(pINI, "socket", NULL))
    {
        r = SOCKET_INI_settingsOne(pINI, handled, &my_socket_cfg);
    }
    return (r);
}

static int my_MT_MSG_INI_settings(struct ini_parser *pINI, bool *handled)
{
    int r;

    r = 0;
    if (INI_itemMatches(pINI, "test-interface", NULL))
    {
        r = MT_MSG_INI_settings(pINI, handled, &test_interface);
    }
    return (r);
}

static int my_ini_settings(struct ini_parser *pINI, bool *handled)
{
    bool b;

    if (INI_itemMatches(pINI, "interface", "socket"))
    {
        *handled = true;
        b = INI_valueAsBool(pINI);
        if (b)
        {
            test_interface.u_cfg = NULL;
            test_interface.s_cfg = &my_socket_cfg;
        }
    }

    if (INI_itemMatches(pINI, "interface", "uart"))
    {
        *handled = true;
        b = INI_valueAsBool(pINI);
        if (b)
        {
            test_interface.u_cfg = &my_uart_cfg;
            test_interface.s_cfg = NULL;
        }
    }
    return (0);
}

static ini_rd_callback * const ini_cb_table[] = {
    LOG_INI_settings,
    my_UART_INI_settings,
    my_CUSTOM_CMD_ini,
    my_SOCKET_INI_settings,
    my_MT_MSG_INI_settings,
    my_ini_settings,
    /*    MT_INI_Settings, */
    /* Terminate list */
    NULL
};

static int cfg_callback(struct ini_parser *pINI, bool *handled)
{
    int x;
    int r;

    for (x = 0; ini_cb_table[x]; x++)
    {
        r = (*(ini_cb_table[x]))(pINI, handled);
        if (*handled)
        {
            return (r);
        }
    }

    /* let the system handle it */
    return (0);
}

static void do_strdup(const char ** cpp)
{
    if (*cpp == NULL)
    {
        return;
    }

    *cpp = strdup(*cpp);
}

static void fixup_cfg(void)
{
    /* the "INI" parser code assumes
     * that strings can be free()ed if they exist.
     * We want some reasonable defaults...
     * and those are hard coded (fixed) strings.
     * =====
     * Rule #1 - Thou shall not 'free()' a constant string.
     * So ... we have to strdup these ...
     * =====
     */
    do_strdup((const char **)&(my_uart_cfg.devname));

    do_strdup((const char **)&(my_socket_cfg.device_binding));
    do_strdup((const char **)&(my_socket_cfg.host));
    do_strdup((const char **)&(my_socket_cfg.service));
}

int main(int argc, char **argv)
{
    int r;
    const char *cfg;
    const char *cfg2;

    fixup_cfg();

    memset(&(custom_cmds[0]), 0, sizeof(custom_cmds));

    /* Basic initialization */
    STREAM_init();
    SOCKET_init();
    LOG_init("/dev/stderr");
    /* we want these logs to begin with */
    log_cfg.log_flags = LOG_FATAL | LOG_WARN | LOG_ERROR;
    TIMER_init();

    cfg = NULL;
    cfg = "mt_test.ini";
    cfg2 = "..\\mt_test.ini"; /* visual studio in debugger */

    switch (argc)
    {
    case 1:
        if (STREAM_FS_fileExists(cfg))
        {
            /* we are good */
        }
        else
        {
            if (STREAM_FS_fileExists(cfg2))
            {
                cfg = cfg2;
            }
            else
            {
                FATAL_printf("CFG file: %s and/or %s does not exist\n",
                    cfg,
                    cfg2);
            }
        }
        break;
    case 2:
        cfg = argv[1];
        break;
    default:
        cfg = NULL;
        break;
    }

    if (cfg == NULL)
    {
        FATAL_printf("usage: %s CONFIGFILE\n", argv[0]);
    }

    /* Rad our configuration file */
    r = INI_read(cfg, cfg_callback, 0);
    if (r != 0)
    {
        fprintf(stderr, "error parsing cfg file\n");
        exit(1);
    }

    test_main();

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
