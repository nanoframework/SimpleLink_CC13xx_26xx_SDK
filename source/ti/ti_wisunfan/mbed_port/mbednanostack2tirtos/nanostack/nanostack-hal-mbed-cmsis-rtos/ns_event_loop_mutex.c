/*
 * Copyright (c) 2018, Arm Limited and affiliates.
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

#include "ns_event_loop_mutex.h"
#include "stdint.h"

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

static Semaphore_Struct ns_event_mutex_struct;
static Semaphore_Handle ns_event_mutex_handle;
static Task_Handle event_mutex_owner_id;
static uint32_t owner_count = 0;

void eventOS_scheduler_mutex_wait(void)
{
    Semaphore_pend(ns_event_mutex_handle, BIOS_WAIT_FOREVER);

    if (0 == owner_count) {
        /* store mutex owner task handle */
        event_mutex_owner_id = Task_self();
    }
    owner_count++;
}

void eventOS_scheduler_mutex_release(void)
{
    owner_count--;
    if (0 == owner_count) {
        event_mutex_owner_id = NULL;
    }

    Semaphore_post(ns_event_mutex_handle);
}

uint8_t eventOS_scheduler_mutex_is_owner(void)
{
    return Task_self() == event_mutex_owner_id ? 1 : 0;
}

void ns_event_loop_mutex_init(void)
{
    Semaphore_Params semParams;

    /* Construct a Semaphore object to be use as a resource lock, initial count 1 */
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&ns_event_mutex_struct, 1, &semParams);

    /* Obtain instance handle */
    ns_event_mutex_handle = Semaphore_handle(&ns_event_mutex_struct);
}
