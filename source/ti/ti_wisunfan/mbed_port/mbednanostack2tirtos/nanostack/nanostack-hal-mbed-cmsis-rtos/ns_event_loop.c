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
#include "mbed_config_app.h"
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include "eventOS_scheduler.h"

#include "ns_event_loop_mutex.h"
#include "ns_event_loop.h"

#include "ns_trace.h"

/*static*/ Semaphore_Struct event_thread_sem_struct;
/*static*/ Semaphore_Handle event_thread_sem_handle;
/*static*/ Task_Handle event_thread_id;
/*static*/ Task_Struct event_thread_struct;
static uint64_t event_thread_stk[MBED_CONF_NANOSTACK_HAL_EVENT_LOOP_THREAD_STACK_SIZE / 8];

static void event_loop_thread(void *arg);

extern void eventOS_dispatch_timac_event(void);

void eventOS_scheduler_signal(void)
{
    //tr_debug("signal %p", (void*)event_thread_id);
    Semaphore_post(event_thread_sem_handle);
    //tr_debug("signalled %p", (void*)event_thread_id);
}

void eventOS_scheduler_idle(void)
{
    //tr_debug("idle");
    eventOS_scheduler_mutex_release();

    Semaphore_pend(event_thread_sem_handle, BIOS_WAIT_FOREVER);

    eventOS_scheduler_mutex_wait();
    eventOS_dispatch_timac_event();
}

static void event_loop_thread(void *arg)
{
    (void)arg;
    eventOS_scheduler_mutex_wait();
    eventOS_scheduler_run(); //Does not return
}

// This is used to initialize the lock used by event loop even
// if it is not ran in a separate thread.
void ns_event_loop_init(void)
{
    ns_event_loop_mutex_init();
}

void ns_event_loop_thread_create(void)
{
    Task_Params taskParams;
    Semaphore_Params semParams;

    /* Construct a Semaphore object to be use as a flag, initial count 0 */
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&event_thread_sem_struct, 0, &semParams);
    event_thread_sem_handle = Semaphore_handle(&event_thread_sem_struct);

    /* Construct ns event thread */
    Task_Params_init(&taskParams);
    taskParams.stackSize = sizeof(event_thread_stk);
    taskParams.stack = (uint8_t*) &event_thread_stk;
    taskParams.priority = 2;
    Task_construct(&event_thread_struct, (Task_FuncPtr)event_loop_thread, &taskParams, NULL);

    event_thread_id = Task_handle(&event_thread_struct);
}

void ns_event_loop_thread_start(void)
{
}
