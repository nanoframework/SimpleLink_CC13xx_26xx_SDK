/******************************************************************************
 @file mutex_test.c

 @brief TIMAC 2.0 API Unit test for mutex abstraction

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
#include "mutex.h"
#include "stream.h"
#include "fifo.h"
#include "threads.h"
#include "log.h"
#include "fatal.h"
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> /* for time(), to init our srand() */

intptr_t f;
intptr_t m;
intptr_t p0;
intptr_t p1;
intptr_t d;

/* KEY HERE is both strings are the same length */
/* string(0) contains captial O (captial-letter-oh) and not z. */
/* string(1) contains z and not (capital-O */

#define TOTAL_BYTES (2 * NTIMES * LINE_LEN)
static const char line_1[] = "ONE this is ONE foobar ONE\n";
static const char line_0[] = "ZERO line 000 dog cat ZERO\n";
#define LINE_LEN sizeof(line_0)
#define NTIMES   20
static char rx_buf[ LINE_LEN + 1 ];

static void failure(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

/* this function pumps buts into the fifo.
 * While doing so, it randomly sleeps
 *
 * The goal of the randomness is to expose problems
 *---
 * There are two different thread pumping
 * two differnet strings.. because we lock/unlock
 * at the start/end of the strings
 *  The strings will be inserted correctly :-)
*/

/* for debug purposes, extract R bytes from the string */
/* and return a nice debug printable string */
static const char *_x(const char *cp,int r)
{
    static char buf[30];
    int c;
    char *wr;
    wr = buf;

    *wr = 0;
    c = 0;
    while(r > 0)
    {
        r--;
        c = *cp++;
        if(c == 0)
        {
            break;
        }
        if(c == '\n')
        {
            break;
        }
        *wr++ = (char)c;
        *wr   = 0;
    }
    if(c == '\n')
    {
        strcpy(wr, "\\n");
    }
    return (buf);
}

/* our pump thread, there are two threads, same code
 * just using a different parameter in the 'cookie'
 * each pumps a different string into the fifo */
static intptr_t pump_func(intptr_t cookie)
{
    const char *cp;
    int r;
    int n;

    for(n = 0 ; n < NTIMES ; n++)
    {
        if(cookie)
        {
            cp = line_1;
        }
        else
        {
            cp = line_0;
        }

        r = MUTEX_lock(m, -1);
        if(r != 0)
        {
            failure("Cannot lock mutex\n");
        }
        while(*cp)
        {
            r = (rand() & 7) + 1;
            LOG_printf(LOG_DBG_MUTEX, "%s: wr: %d -> %s\n",
                        THREAD_selfName(), r, _x(cp,r));
            while((r > 0) && (*cp !=0))
            {
                FIFO_fputc(*cp, f);

                r--;
                cp++;
            }
            r = (rand() & 7) + 1;
            /* sleep that many random millisecons */
            TIMER_sleep(r);

        }
        r = MUTEX_unLock(m);
        if(r != 0)
        {
            failure("Cannot un-lock mutex\n");
        }
        /* force/simulate random thread switches */
        /* and high priority context switches */
        r = ((rand() & 1) ? 100 : 0);
        /* by sleeping */
        if(r)
        {
            TIMER_sleep(r);
        }
    }
    return (0xdeadbeef);
}

/* this is our *SINGLE* drain thread
 * It reads a string and prints it
 * it also knows what is going in
 * so that it can verify if the data is corrupted */
static intptr_t drain_func(intptr_t cookie)
{
    int x;
    int nlines;
    int c;
    (void)(cookie); /* not used */

    /* we only do this NTIMES.. */
    for(nlines = 0 ; nlines < (NTIMES * 2) ; nlines++)
    {
        x = 0;
        do
        {
            do
            {
                c = FIFO_fgetc(f);
                if(c == EOF)
                {
                    TIMER_sleep(10);
                }
            }
            while(c == EOF)
                ;

            LOG_printf(LOG_ALWAYS,"fifo rx: %c\n",
                       (c=='\n') ? 'X' : c);
            rx_buf[x+0] = (char)c;
            rx_buf[x+1] = 0;
            x++;
            /* stop on a new line */
        }
        while(c != '\n')
            ;

        /* which string did we get? */
        if(0 == strcmp(rx_buf, line_1))
        {
            LOG_printf(LOG_ALWAYS,"%2d: LINE 1: %s", nlines, rx_buf);
            continue;
        }
        if(0 == strcmp(rx_buf, line_0))
        {
            LOG_printf(LOG_ALWAYS, "%2d: LINE 0: %s", nlines, rx_buf);
            continue;
        }

        /* WRONG! */
        FATAL_printf("FAILURE: rx: %s\n", rx_buf);

        exit(1);
    }
    return (0xdeadbeef);

}

/* main test function */
int main(int argc, char **argv)
{
    time_t t;

    (void)argc;
    (void)argv;

    /* comment out one of these two */
    /* fixed value */
    t = 1450071291;
    /* random value */
    time(&t);

    STREAM_init();

    LOG_init("/dev/stdout");
    /*log_cfg.log_flags = -1; */
    srand((unsigned int)(t));
    LOG_printf(LOG_ALWAYS,"SRAND: %u\n", (unsigned int)(t));
    /*log_cfg.log_flags |= (LOG_DBG_MUTEX  | LOG_DBG_THREAD | LOG_DBG_FIFO); */

    /* create our test subjects */
    f = FIFO_create("fifo", sizeof(char), 100, 1);
    m = MUTEX_create("mutex");
    p0 = THREAD_create("pump0", pump_func, 0, THREAD_FLAGS_DEFAULT);
    p1 = THREAD_create("pump1", pump_func, 1, THREAD_FLAGS_DEFAULT);
    d = THREAD_create("drain", drain_func, 0, THREAD_FLAGS_DEFAULT);

    /* things are running.. */

    int n;
    for(;;)
    {
        /* wait till all are complete */
        intptr_t v;
        n = 0;
        v = THREAD_getExitValue(p0);
        if(v == 0xdeadbeef)
        {
            n++;
        }

        v = THREAD_getExitValue(p1);
        if(v == 0xdeadbeef)
        {
            n++;
        }

        v = THREAD_getExitValue(d);
        if(v == 0xdeadbeef)
        {
            n++;
        }

        if(n == 3)
        {
            /* success :-) */
            break;
        }
        TIMER_sleep(1000);
    }
    LOG_printf(LOG_ALWAYS,"Test success\n");
    return (0);
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
