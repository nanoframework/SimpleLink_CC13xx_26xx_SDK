/******************************************************************************
 @file hexline_test.c

 @brief TIMAC 2.0 API Unit test for hexline debug dump code

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
#include "hexline.h"
#include <stdio.h>

const uint8_t txt1[] = "<--Duane-->";

const uint8_t txt2[] = "<--R\xc8\xc1ll\xdd L\xd6\xd1 String Duane Is Here -->";

int main(int argc, char **argv)
{
    struct hexline h;

    (void)(argc);
    (void)(argv);

    printf("SHORT starting at 0 offset\n");
    HEXLINE_init(&h, 0, txt1, sizeof(txt1));
    while(h.ndone < h.nbytes)
    {
        HEXLINE_format(&h);
        printf("%s\n", h.buf);
    }

    printf("SHORT starting at non-0 offset\n");
    HEXLINE_init(&h, 13, txt1, sizeof(txt1));
    while(h.ndone < h.nbytes)
    {
        HEXLINE_format(&h);
        printf("%s\n", h.buf);
    }

    printf("Long, with non-printables at 0\n");
    HEXLINE_init(&h, 0, txt2, sizeof(txt2));
    while(h.ndone < h.nbytes)
    {
        HEXLINE_format(&h);
        printf("%s\n", h.buf);
    }

    printf("Long, with non-printables at non-zero\n");
    HEXLINE_init(&h, 13, txt2, sizeof(txt2));
    while(h.ndone < h.nbytes)
    {
        HEXLINE_format(&h);
        printf("%s\n", h.buf);
    }
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
