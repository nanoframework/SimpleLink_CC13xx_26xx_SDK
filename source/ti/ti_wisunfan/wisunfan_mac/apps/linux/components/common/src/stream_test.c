/******************************************************************************
 @file stream_test.c

 @brief TIMAC 2.0 API Unit test for the stream module

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

#include <stdio.h>
#include <stdlib.h>

#include "stream.h"

int main(int argc, char **argv)
{
    intptr_t f;
    (void)argc;
    (void)argv;

    STREAM_init();

    STREAM_printf(STREAM_stdout, "hello world\n");

    STREAM_printf(STREAM_stdout, "%d and %d make %d\n", 1, 1, 2);

    f = STREAM_createWrFile("test.txt");
    STREAM_printf(f, "hello world\n");

    STREAM_printf(f, "%d and %d make %d\n", 1, 1, 2);
    STREAM_fputs("Duane is here\n", f);
    STREAM_fputs("With CRLF\r\n", f);
    STREAM_fputs("Mac Newline\r", f);
    STREAM_fputs("blah blah no newline at end", f);

    STREAM_close(f);

    {
        char buf[100];
        char *cp;
        int c;
        f = STREAM_createRdFile("test.txt");

        while(STREAM_fgets(buf, sizeof(buf), f))
        {
            printf("|");
            cp = buf-1;
            for(;;)
            {
                cp++;
                c = *cp;
                if(c == 0)
                {
                    break;
                }
                if(c == '\n')
                {
                    printf("\\n");
                    continue;
                }
                if(c == '\r')
                {
                    printf("\\r");
                    continue;
                }
                printf("%c", c);
            }
            printf("|\n");
        }
        STREAM_close(f);
    }
    printf("Success\n");
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
