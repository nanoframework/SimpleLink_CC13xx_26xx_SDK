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

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>

#include <arm_hal_interrupt.h>

void platform_critical_init(void)
{
}

void platform_enter_critical(void)
{
    /* Enter critical section */
    //Task_disable();
    Hwi_disable();
}

void platform_exit_critical(void)
{
    /* Exit critical section */
    Hwi_enable();
    //Task_enable();
}
