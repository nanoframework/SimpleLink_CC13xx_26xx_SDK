/******************************************************************************
 @file timer_cb_test.c

 @brief TIMAC 2.0 API Unit test for callback timers

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

#include "timer.h"
#include "log.h"
#include "fatal.h"

/*
 * @struct test2
 * @brief  This contains tests details for the 'test2' portion of the api test.
 *
 * The general idea of the test is this:
 *   - Start a bunch of timers at diffrent intervals.
 *   - After a varied number of 'expires' - cancel the timer
 *   - When all timers are expired, the test is complete.
 */
struct test2_cfg {
    /*! dbg name */
    const char *dbg_name;
    /*! timer handle */
    intptr_t h;
    /*! how many times has this timer expired? */
    int cnt;
    /*! period of this timer */
    unsigned period;
    /*! n times before this timer should be canceled */
    int n_occur;
    /*! When did timer last expire? */
    unsigned last_time;
};

struct test2_cfg test2_cfg[] = {
    /* Test runs for about 10 seconds.. */
    /* hence the occur = 10000msecs / period, rounded down */
    { .dbg_name = "t1", .h = 0, .cnt = 0, .period = 100, .n_occur = (10000/100), .last_time = 0 },
    { .dbg_name = "t2", .h = 0, .cnt = 0, .period =  75, .n_occur = (10000/ 75), .last_time = 0 },
    { .dbg_name = "t3", .h = 0, .cnt = 0, .period = 125, .n_occur = (10000/125), .last_time = 0 },
    { .dbg_name = "t4", .h = 0, .cnt = 0, .period =  90, .n_occur = (10000/ 90), .last_time = 0 },
    /* terminate the list */
    { .dbg_name = NULL }
};

#define _between(v, l, h) (((v) >= (l)) && ((v) <= (h)))

static void timer2_cb(intptr_t h, intptr_t cookie)
{
    unsigned tnow;
    unsigned elapsed;

    struct test2_cfg *t2;

    /* recover test details */
    t2 = (struct test2_cfg *)(cookie);

    /* get elapsed time */
    tnow = TIMER_getNow();
    elapsed = tnow - t2->last_time;

    /* bump occurance */
    t2->cnt += 1;

    /* print diagnostic */
    LOG_printf(LOG_ALWAYS,
                "Timer(%s) last=%3u, cnt=%3d, period=%3d, elapsed=%3d\n",
                t2->dbg_name, t2->last_time, t2->cnt, t2->period, elapsed);

    /* remember last time */
    t2->last_time = tnow;

    /* is elasped time reasonable? */
    if(!_between(elapsed, t2->period - 1, t2->period + 120))
    {
        LOG_printf(LOG_ALWAYS,
                    "NOTE: This failure might be a 'linux/windows-thing', "
                    "read the comments here\n");
        LOG_printf(LOG_ALWAYS, "Bottom line: Try the test again, if it passes "
                    "you have a false-failure\n");
        /*
         * What happened?
         *
         * We tell LINUX to sleep for (X) milliseconds .. however,
         * linux instead sleeps for (X) plus _random_amount_
         *
         * The above checks for a random amount of less then 15 mSecs
         * If linux is stupid.. and is doing something, it might
         * actually have slept 15 milliseconds more ...
         *
         * There is nothing we can do about this...
         * it's the way linux works.
         */

        FATAL_printf("Timer: %s, bad elapsed time\n", t2->dbg_name);
    }

    if(t2->cnt >= t2->n_occur)
    {
        LOG_printf(LOG_ALWAYS, "Timer(%s) Done\n", t2->dbg_name);
        TIMER_CB_destroy(h);
        /* we are going to *PURPOSELY* not zero the handle */
    }
}

/*
 * @brief Create N timers and wait for them to self expire
 *
 * The general idea of the test is this:
 *   - Start a bunch of timers at diffrent intervals(see array above)
 *   - After a varied number of 'expires' - timers will self-cancel
 *   - When all timers are expired, the test is complete.
 */
static void test2(void)
{
    int x;
    int done;

    for(x = 0 ; test2_cfg[x].dbg_name ; x++)
    {
        test2_cfg[x].last_time = TIMER_getNow();
        test2_cfg[x].h = TIMER_CB_create(test2_cfg[x].dbg_name,
                                   timer2_cb,
                                   (intptr_t)(&test2_cfg[x]),
                                   test2_cfg[x].period,
                                   true);
    }

    do
    {
        done = true;
        TIMER_sleep(100);
        for(x = 0 ; test2_cfg[x].dbg_name ; x++)
        {
            /*
             * HERE this test will do something normally invalid.
             * Recall that this is *TEST* code, not production code.
             *
             * In this test case, the timer will self destruct...  we
             * leave the handle around (which is really a pointer) The
             * validation check will inspect the memory that *WAS* the
             * timer (and is now 'free()ed' memory)
             *
             * This could cause a certain type of access memory
             * after it was free()ed error to occur.
             *
             * However, the IsValid should return false
             *
             * BECAUSE:
             *  * We have not done more memory allocation
             *    Thus the memory is "still there"
             *
             *  * The IsValid process inspects a pointer
             *    to see if it still points at the check value.
             *
             *  * The destruction processed zeroed memory
             *
             *  * Thus the pointer check will fail... and thus
             *    The timer will be invalid!
             */
            if(TIMER_CB_isValid(test2_cfg[x].h))
            {
                done = false;
            }
            else
            {
                /* we'll zap it here, now that we know it is done */
                test2_cfg[x].h = 0;
            }
        }
        LOG_printf(LOG_ALWAYS, "Timecheck done=%d\n", (int)(done));
    }
    while(!done)
        ;
}

int main(int argc, char **argv)
{
    (void)(argc);
    (void)(argv);
    LOG_init("/dev/stdout");

    test2();
    LOG_printf(LOG_ALWAYS, "Test success\n");
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
