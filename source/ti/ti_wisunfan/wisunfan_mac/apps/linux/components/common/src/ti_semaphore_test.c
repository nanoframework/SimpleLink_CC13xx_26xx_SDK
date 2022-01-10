/******************************************************************************
 @file ti_semaphore_test.c

 @brief TIMAC 2.0 API Unit test for the semaphore abstraction

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

#include "ti_semaphore.h"
#include "stream.h"
#include "threads.h"
#include "log.h"
#include "timer.h"
#include "fatal.h"

#include <string.h>
#include <stdlib.h>

/*
 * @var semtest
 * @brief semaphore under test.
 */
intptr_t semtest;

/*
 * @var p_id
 * @brief producer thread id
 */
intptr_t p_id;

/*
 * @var c_id
 * @brief consumer thread id
 */
intptr_t c_id;

/*
 * @brief producer function, produces every 2 seconds
 * @param cookie - not used here
 * @return always 0 (not used)
 */
static intptr_t prod_func(intptr_t cookie)
{
    int x;
    (void)(cookie);

    for(x = 0 ; x < 10 ; x++)
    {
        TIMER_sleep(2000);
        LOG_printf(LOG_ALWAYS, "Produce!\n");
        SEMAPHORE_put(semtest);

        if(!THREAD_isAlive(c_id))
        {
            LOG_printf(LOG_ALWAYS, "(consumer) has exited so will we\n");
            break;
        }
    }
    LOG_printf(LOG_ALWAYS, "Done producing\n");
    return (0);
}

/*
 * @brief_approx
 * @param etime - elapsed time
 * @param mintime - minimal time expected
 * @param maxtime - maximal time expected
 *
 * return (1) if elapsed time is between min and max time
 */
static int _approx(int etime, int mintime, int maxtime)
{
    if((etime >= mintime) && (etime <= maxtime))
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

/*
 * @brief consumer, this consumes what is produced.
 * @param cookie - not used here
 * @return always 0 (not used)
 *
 * The logic in this function is effectivly the test sequence.
 */
static intptr_t cons_func(intptr_t cookie)
{
    int x,r;
    unsigned tstart;
    unsigned later;
    unsigned e;

    (void)(cookie);

    LOG_printf(LOG_ALWAYS, "Waiting...(wait forever test)\n");
    /* test wait for ever */
    tstart = TIMER_getNow();
    SEMAPHORE_waitWithTimeout(semtest , -1);
    later = TIMER_getNow();
    e = (later - tstart);
    if(!_approx(e, 1990, 2050))
    {
        FATAL_printf("timeout seems wrong!\n");
    }

    LOG_printf(LOG_ALWAYS, "Got\n");

    /* test wait timeout */
    /* 12 * 200 = 2.4 seconds, we will succed at 2 seconds */
    /* we should get a timeout at approximatly 4 seconds */
    tstart = TIMER_getNow();
    for(x = 0 ; x < 12 ; x++)
    {
        LOG_printf(LOG_ALWAYS, "Wait 200mSec (wait timeout test), x=%d\n",x);
        r = SEMAPHORE_waitWithTimeout(semtest, 200);
        LOG_printf(LOG_ALWAYS, "Wait = %d\n", r);
        if(r < 0)
        {
            FATAL_printf("wait failed\n");
        }
        if(r > 0)
        {
            LOG_printf(LOG_ALWAYS,"Success (200mSec)\n");
            break;
        }
        else
        {
            LOG_printf(LOG_ALWAYS, "timeout... try again\n");
        }
    }
    if(r != 1)
    {
        FATAL_printf("WRONG we expected to get 1\n");
    }
    later = TIMER_getNow();
    e = (later - tstart);
    if(!_approx(e, 1990, 2020))
    {
        FATAL_printf("timeout seems wrong here, e = %d\n", e);
    }

    LOG_printf(LOG_ALWAYS,
               "Wait 5 seconds, garentee we have 2 in the semaphore\n");
    TIMER_sleep(5000);
    r = SEMAPHORE_inspect(semtest);
    LOG_printf(LOG_ALWAYS, "Waited, result = %d\n", r);
    if(r != 2)
    {
        FATAL_printf("Fail result should be 2\n");
    }

    /* these should go quick */
    tstart = TIMER_getNow();
    for(x = 0 ; x < 3 ; x++)
    {

        LOG_printf(LOG_ALWAYS, "before wait #%d\n",x);;
        r = SEMAPHORE_waitWithTimeout(semtest, 0);
        LOG_printf(LOG_ALWAYS, "wait 0 = result: %d\n", r);
    }
    later = TIMER_getNow();
    e = later - tstart;
    if(!_approx(e, 0, 50))
    {
        FATAL_printf("This should have been done very quickly, e = %d\n", e);
    }

    LOG_printf(LOG_ALWAYS, "Consumer complete\n");
    return (0);
}

int main(int argc, char **argv)
{
    (void)(argc);
    (void)(argv);

    STREAM_init();
    LOG_init("/dev/stdout");

    semtest = SEMAPHORE_create("sem", 0);

    p_id = THREAD_create("producer", prod_func, 0, THREAD_FLAGS_DEFAULT);
    c_id = THREAD_create("consumer", cons_func, 0, THREAD_FLAGS_DEFAULT);
    LOG_printf(LOG_ALWAYS, "Wait test complete\n");
    TIMER_sleep(15 * 1000);
    LOG_printf(LOG_ALWAYS, "Good bye SUCCESS\n");
    if(THREAD_isAlive(p_id) ||
        THREAD_isAlive(c_id))
    {
        FATAL_printf("p & c threads should be dead by now\n");
    }
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

