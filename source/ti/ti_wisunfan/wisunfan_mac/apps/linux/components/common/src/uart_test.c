/******************************************************************************
 @file uart_test.c

 @brief TIMAC 2.0 API Unit test for the UART abstraction

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

#include "stream.h"
#include "stream_uart.h"
#include "timer.h"
#include "log.h"
#include "fatal.h"
#include "rand_data.h"
#include "threads.h"

#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>

static const char *tx_uart_name;
static const char *rx_uart_name;
static const char *test_name;
/* note: tx is dominate.
 *    in the single uart case (ie: loop back)
 *    tx is real, rx just points to tx
 */
static intptr_t tx_stream;
static intptr_t rx_stream;

uint8_t rx_buf[1024];
int     rx_idx;
int     rx_dump_idx;

/*#define BAUDRATE  115200 */
#define BAUDRATE  115200

static void test_success(void)
{
    LOG_printf(LOG_ALWAYS,"SUCCESS\n");
    exit(0);
}

static void test_failure(void)
{
    FATAL_printf("FAILURE\n");
}

static void test_verify(void)
{
    FATAL_printf("You must visually (with your eyeballs) verify this test\n");
}

static void rx_reset(void)
{
    rx_idx = 0;
    rx_dump_idx = 0;
}

static void rx_buf_check(const char *expected, size_t n)
{
    if(0 == memcmp((const void *)(expected), (const void *)(rx_buf), n))
    {
        return;
    }
    LOG_printf(LOG_ERROR,"DATA MISMATCH! Got: %d bytes\n", rx_idx);
    LOG_hexdump(LOG_ERROR, 0, (const void *)(rx_buf), rx_idx);
    LOG_printf(LOG_ERROR,"Wanted: %d\n",(int)n);
    LOG_hexdump(LOG_ERROR, 0, (const void *)expected,n);
    exit(1);
}

/* is this string the testname? */
static int is_test(const char *name)
{
    return (0 == strcmp(test_name, name));
}

/* read bytes from the uart and stuf them into the buffer. */
static int rd_bytes(int expected, int timeout_mSecs)
{
    int n;
    int b4;
    if(expected <= 0)
    {
        /* fill the entire size */
        expected = sizeof(rx_buf) - rx_idx;
        /* and make it a reasonable timeout */
        timeout_mSecs = 5;
    }
    b4 = rx_idx;
    n = STREAM_rdBytes(rx_stream, rx_buf+rx_idx, expected, timeout_mSecs);
    if(n > 0)
    {
        rx_idx += n;
        LOG_printf(LOG_ALWAYS,
                   "rd: old idx = %d, got %d bytes, new ids = %d\n",
                   b4,n, rx_idx);
    }
    return (n);
}

/* hexdump the rx buffer in 16 byte chunks... (during a test) */
/* or all of it (normall at the end of a test) */
static void rx_dump(int all)
{
    int n;

    if(all)
    {
        n = rx_idx - rx_dump_idx;
    }
    else
    {
        /* only print full rows */
        n = rx_idx & (~0x0f);
    }
    if(n)
    {
        LOG_hexdump(LOG_ALWAYS, rx_dump_idx, rx_buf + rx_dump_idx, n);
        rx_dump_idx += n;
    }
}

/* wait for (n) expected bytes or a timeout. */
static void rx_wait(int nexpect, int msecs)
{
    timertoken_t t;
    int r;

    if(nexpect < 0)
    {
        r = rd_bytes(-nexpect,msecs);
    }
    else
    {
        r = rd_bytes(+nexpect,msecs);
    }
    if(r < 0)
    {
        LOG_printf(LOG_ERROR,"something is wrong\n");
        return;
    }
    if((msecs != 0) && (r == 0))
    {
        LOG_printf(LOG_ERROR,"timeout\n");
        return;
    }

    LOG_printf(LOG_ALWAYS, "rx %d bytes\n", rx_idx);
    rx_dump(1);

    /* did we ask for an exact amount? */
    if(r == (-nexpect))
    {
        /* good then return */
        return;
    }

    /* we should not be expecting more */
    /* so wait to see if we got more. */


    /* we should not get more ... */
    /* so wait and make sure we are not getting more */
    t = TIMER_timeoutStart();
    while(!TIMER_timeoutIsExpired(t, msecs))
    {

        r = STREAM_rxAvail(rx_stream, -1);
        if(!r)
        {
            TIMER_sleep(5);
            continue;
        }

        /* read everything we can. */
        r = rd_bytes(0,0);
        if(r < 0)
        {
            /* some error occured */
            break;
        }
        LOG_printf(LOG_ERROR,"rx extra bytes: %d\n", r);
        rx_dump(1);
    }
}

static intptr_t open_this_uart(int flags, const char *name)
{
    intptr_t h;
    struct uart_cfg cfg;

    memset(&cfg, 0, sizeof(cfg));

    cfg.devname      = name;
    cfg.baudrate     = BAUDRATE;
    cfg.open_flags   = flags;

    h = STREAM_createUart(&cfg);
    if(h == 0)
    {
        FATAL_perror(cfg.devname);
    }
    return (h);
}

static void open_both_uarts(int flags)
{
    tx_stream = open_this_uart(flags, tx_uart_name);
    if(0 == strcmp(tx_uart_name, rx_uart_name))
    {
        rx_stream = tx_stream;
    }
    else
    {
        rx_stream = open_this_uart(flags,rx_uart_name);
    }
}

static void rxdump_test(int flags)
{
    int n,tick;
    uint8_t buf[100];
    tx_stream = open_this_uart(flags, tx_uart_name );
    tick = 0;
    for(;;){
        n = STREAM_rdBytes( tx_stream, (void *)buf, sizeof(buf), 1000);
        if(n > 0){
            LOG_printf( LOG_ALWAYS,"\nRx: %d\n", n);
            LOG_hexdump(LOG_ALWAYS,0,buf,n);
            tick = 0;
            continue;
        }
        if( n < 0 ){
            LOG_printf( LOG_ALWAYS, "ERROR: %d\n", n );
            break;
        }

        if( tick > 60 ){
            printf("\n");
            tick = 0;
        }
        printf(".");
        tick++;
        fflush(stdout);
    }
}

static void echo_test(int flags)
{
    const char *cp;

    LOG_printf(LOG_ALWAYS,"rx & tx should be looped back for this test\n");
    open_both_uarts(flags);
    rx_reset();
    cp = "hello duane is here\n";
    STREAM_wrBytes(tx_stream, (const void *)cp , strlen(cp), 10);

    rx_wait(strlen(cp), 100);

    rx_buf_check(cp, strlen(cp));

    test_success();
}

static void echoextra_test(int flags)
{
    const char *cp;

    LOG_printf(LOG_ALWAYS,"rx & tx should be looped back for this test\n");
    open_both_uarts(flags);
    rx_reset();
    /* we write 22 */
    cp = "hello duane xisx here\n";
    STREAM_wrBytes(tx_stream, cp, strlen(cp), 10);

    /* but want excatly 20 do not pull the extras */
    rx_wait(-(strlen(cp)-2), 100);

    /* then exactly 2 more. */
    rx_wait(2, 100);

    rx_buf_check(cp,strlen(cp));

    test_success();
}

static void allbytes_test(int flags)
{
    int tx;
    int rx;

    open_both_uarts(flags);

    LOG_printf(LOG_ALWAYS,
                "Writing all bytes (0x00 to 0xff) through serial port\n");
    /* we use an FTDI chip for most of our debug cables */
    /* these have a 64byte buffer... */
    /* so we trail our reading a little bit */
#define TX_SIZE 256

    /* simple test 1 */
    for(tx = 0 ; tx < 256 ; tx++)
    {
        STREAM_fputc(tx, tx_stream);
        rx = STREAM_fgetc(rx_stream);
        if(tx == rx)
        {
            if((tx % 16) == 0)
            {
                LOG_printf(LOG_ALWAYS, "Byte %d ok\n", tx);
            }
        }
        else
        {
            LOG_printf(LOG_ALWAYS, "BAD: got %d, want: %d <---- bad\n", rx,tx);
        }
    }
    LOG_printf(LOG_ALWAYS, "Test 1 complete\n");

    for(tx = 0 ; tx < 64 ; tx++)
    {
        STREAM_fputc(tx, tx_stream);
    }
    TIMER_sleep(500);

    for(tx = 0 ; tx < 64 ; tx++)
    {
        rx = STREAM_fgetc(rx_stream);
        if(tx == rx)
        {
            if((tx % 16) == 15)
            {
                LOG_printf(LOG_ALWAYS, "Byte %d ok\n", tx);
            }
        }
        else
        {
            LOG_printf(LOG_ALWAYS, "BAD: got %d, want: %d <---- bad\n", rx,tx);
        }
    }

    test_success();
}

int main(int argc, char **argv)
{
    switch(argc)
    {
    default:
        fprintf(stderr,
                 "usage: %s TESTNAME /dev/txname [/dev/rxname2]", argv[0]);
        fprintf(stderr,
                 "Note: if only 1 uart is used, only specify one name\n");
        exit(1);
    case 3:
        test_name = argv[1];
        tx_uart_name = argv[2];
        rx_uart_name = argv[2];
        break;
    case 4:
        test_name = argv[1];
        tx_uart_name = argv[2];
        rx_uart_name = argv[3];
        break;
    }

    LOG_init("/dev/stdout");
    log_cfg.log_flags |= (LOG_ERROR | LOG_WARN | LOG_DBG_FIFO | LOG_DBG_UART);

    if(is_test("rxdump"))
    {
        rxdump_test(STREAM_UART_FLAG_default);
    }

    if(is_test("echo"))
    {
        echo_test(STREAM_UART_FLAG_default);
    }
    if(is_test("echo-thread"))
    {
        echo_test(STREAM_UART_FLAG_rd_thread);
    }
    if(is_test("echoextra"))
    {
        echoextra_test(STREAM_UART_FLAG_default);
    }
    if(is_test("echoextra-thread"))
    {
        echoextra_test(STREAM_UART_FLAG_rd_thread);
    }

    if(is_test("allbytes"))
    {
        allbytes_test(STREAM_UART_FLAG_default);
    }

    if(tx_uart_name == NULL)
    {
        /* these are here to 'use them' */
        /* otherwise we get warning the */
        /* function is not used. */
        test_success();
        test_failure();
        test_verify();
    }

    FATAL_printf("Unknown test: %s\n", test_name);
    return (1);
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
