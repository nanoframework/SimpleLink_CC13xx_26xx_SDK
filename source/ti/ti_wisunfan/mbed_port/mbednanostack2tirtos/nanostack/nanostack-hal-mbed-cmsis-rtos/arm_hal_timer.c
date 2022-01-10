/*
 * Copyright (c) 2016-2018, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "stdint.h"
#include "stdbool.h"

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>

/* convert slots to ticks */
#define US_PER_SLOT 50
#define SLOTS_TO_TICKS(slots)    (((slots * US_PER_SLOT + Clock_tickPeriod/2) / Clock_tickPeriod))
#define TICKS_TO_SLOTS(ticks)    (((ticks * Clock_tickPeriod + US_PER_SLOT/2) / US_PER_SLOT))

static Clock_Struct hal_timer_struct;
static Clock_Handle hal_timer;

static void (*arm_hal_callback)(void);

static uint32_t clockStartTime;

extern void ns_put_char_blocking(const char ch);

#define MBED_CONF_NANOSTACK_HAL_CRITICAL_SECTION_USABLE_FROM_INTERRUPT

#ifndef MBED_CONF_NANOSTACK_HAL_CRITICAL_SECTION_USABLE_FROM_INTERRUPT

static uint8_t timer_thread_stk[256];

static Task_Handle timer_thread_id;
static Task_Struct timer_thread_struct;

static Semaphore_Struct timer_thread_sem_struct;
static Semaphore_Handle timer_thread_sem_handle;

static void timer_loop_thread(void *arg)
{
    (void)arg;

    while(1)
    {
        Semaphore_pend(timer_thread_sem_handle, BIOS_WAIT_FOREVER);
        arm_hal_callback();
    }
}

#endif

/*
 *  ======== clk0Fxn =======
 */
static void clk0Fxn(UArg arg0)
{
    static bool first = false;

    if(first)
    {
        first = false;
        ns_put_char_blocking('s');
        ns_put_char_blocking('\n');
    }

    if(arm_hal_callback)
    {
#ifdef MBED_CONF_NANOSTACK_HAL_CRITICAL_SECTION_USABLE_FROM_INTERRUPT
        arm_hal_callback();
#else
        /* Release sem so task can call the callback in a task context */
        Semaphore_post(timer_thread_sem_handle);
#endif
    }
}

// Called once at boot
void platform_timer_enable(void)
{
    static bool initialized = FALSE;

    if(!initialized)
    {
        /* Construct BIOS Objects */
        Clock_Params clkParams;

#ifndef MBED_CONF_NANOSTACK_HAL_CRITICAL_SECTION_USABLE_FROM_INTERRUPT
        /* Create a task for running timer interrupts */
        Task_Params taskParams;
        Semaphore_Params semParams;

        /* Construct a Semaphore object to be use as a flag, initial count 0 */
        Semaphore_Params_init(&semParams);
        Semaphore_construct(&timer_thread_sem_struct, 0, &semParams);
        timer_thread_sem_handle = Semaphore_handle(&timer_thread_sem_struct);

        /* Construct ns event thread */
        Task_Params_init(&taskParams);
        taskParams.stackSize = sizeof(timer_thread_stk);
        taskParams.stack = (uint8_t*) &timer_thread_stk;
        taskParams.priority = 1;
        Task_construct(&timer_thread_struct, (Task_FuncPtr)timer_loop_thread, &taskParams, NULL);
#endif //MBED_CONF_NANOSTACK_HAL_CRITICAL_SECTION_USABLE_FROM_INTERRUPT

        Clock_Params_init(&clkParams);
        clkParams.period = 0;
        clkParams.startFlag = false;

        /* Construct a periodic Clock Instance */
        Clock_construct(&hal_timer_struct, (Clock_FuncPtr)clk0Fxn,
                        0, &clkParams);

        hal_timer = Clock_handle(&hal_timer_struct);

        initialized = true;
    }
}

// Actually cancels a timer, not the opposite of enable
void platform_timer_disable(void)
{
    Clock_stop(hal_timer);
}

#include "ns_trace.h"
#define TRACE_GROUP "arm_hal_timer"

// Not called while running, fortunately
void platform_timer_set_cb(void (*new_fp)(void))
{
    arm_hal_callback = new_fp;
}

// This is called from inside platform_enter_critical - IRQs can't happen
void platform_timer_start(uint16_t slots)
{
    uint32_t ticks = SLOTS_TO_TICKS( ((uint32_t) slots) );

    //ticks = (slots * tickPeriod) / 5;
    //tr_info("timer_start ticks: %d - %d", slots, ticks);
    Clock_stop(hal_timer);
    Clock_setTimeout(hal_timer, ticks);
    Clock_start(hal_timer);

    clockStartTime = Clock_getTicks();
}

// This is called from inside platform_enter_critical - IRQs can't happen
uint16_t platform_timer_get_remaining_slots(void)
{
    uint32_t elapsedTicks = 0;
    uint32_t timeoutTicks = 0;
    uint32_t currentTicks = Clock_getTicks();

    if(Clock_isActive(hal_timer))
    {
        if(currentTicks > clockStartTime)
        {
            elapsedTicks = currentTicks - clockStartTime;
        }
        else
        {
            elapsedTicks = currentTicks + (UINT32_MAX - clockStartTime);
        }

        timeoutTicks = Clock_getTimeout(hal_timer);
    }

    return TICKS_TO_SLOTS(timeoutTicks - elapsedTicks);
}

