/******************************************************************************

 @file cllc_linux.c

 @brief Linux Specific Coordinator Logical Link Controller

 Group: CMCU LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2019-2021, Texas Instruments Incorporated
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

/******************************************************************************
 Includes
 *****************************************************************************/
#include <string.h>
#include "cllc.h"
#include "cllc_linux.h"

/******************************************************************************
 Public Functions
 *****************************************************************************/
/*!
 Linux specific init

 Public function defined in cllc_linux.h
 */
void CLLC_LINUX_init(uint8_t *chanMask, uint16_t *shortAddr, uint32_t *fhPAtrickleTime, uint32_t *fhPCtrickleTime)
{
    if(CONFIG_FH_ENABLE)
    {
        memcpy( chanMask, linux_FH_ASYNC_CHANNEL_MASK, APIMAC_154G_CHANNEL_BITMAP_SIZ);
    }
    else
    {
        memcpy( chanMask, linux_CONFIG_CHANNEL_MASK, APIMAC_154G_CHANNEL_BITMAP_SIZ);
    }
    *shortAddr = CONFIG_COORD_SHORT_ADDR;

    if(!linux_CERTIFICATION_TEST_MODE)
    {
        linux_CONFIG_TRICKLE_MIN_CLK_DURATION = linux_CONFIG_TRICKLE_MIN_CLK_DURATION;
        linux_CONFIG_TRICKLE_MAX_CLK_DURATION = linux_CONFIG_TRICKLE_MAX_CLK_DURATION;
    }
    else if((linux_CONFIG_PHY_ID >= APIMAC_MRFSK_STD_PHY_ID_BEGIN) && (CONFIG_PHY_ID <= APIMAC_MRFSK_GENERIC_PHY_ID_BEGIN))
    {
        linux_CONFIG_TRICKLE_MIN_CLK_DURATION = 6000;
        linux_CONFIG_TRICKLE_MAX_CLK_DURATION = 6000;
    }
    else
    {
        linux_CONFIG_TRICKLE_MIN_CLK_DURATION = 30000;
        linux_CONFIG_TRICKLE_MAX_CLK_DURATION = 30000;
    }

    *fhPAtrickleTime = linux_CONFIG_TRICKLE_MIN_CLK_DURATION;
    *fhPCtrickleTime = linux_CONFIG_TRICKLE_MAX_CLK_DURATION;
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
