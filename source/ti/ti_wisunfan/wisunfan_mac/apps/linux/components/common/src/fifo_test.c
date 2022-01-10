/******************************************************************************
 @file fifo_test.c

 @brief TIMAC 2.0 API Unit test for generic fifo implimentation

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

#include "fifo.h"
#include "log.h"
#include "fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint16_t data_t;

static void failure(void)
{
    FATAL_printf("FAIL\n");
}

static void success(void)
{
    printf("Success\n");
    exit(0);
}

static void must_be(int a, int b)
{
    if(a != b)
    {
        failure();
    }
}

static intptr_t basics(unsigned itemsize, unsigned depth, int usemutex)
{
    intptr_t h;
    h = FIFO_create("tstfifo", itemsize, depth, usemutex);
    if(h == 0)
    {
        failure();
    }
    must_be(depth, FIFO_getSpaceAvail(h));
    must_be(0    , FIFO_getItemsAvail(h));
    return (h);
}

static void fifo_test1(void)
{
    intptr_t h;
#define DEPTH 10
    data_t wr_data[ DEPTH ];
    data_t rd_data[ DEPTH ];
    int x,y;

    h = basics(sizeof(data_t), DEPTH, 0);

    /* quick add & remove */
    /* add/remove many times .. ending in the middle somewhere */
    for(x = 0 ; x < (DEPTH + DEPTH/2) ; x++)
    {
        /* we cheat: This test onl works little endian */
        /* because the data is in the low 2 bytes */
        wr_data[0] = (data_t)x;
        must_be(1, FIFO_insert(h, (void *)(&wr_data[0]), 1));
        must_be(1      , FIFO_getItemsAvail(h));
        must_be(DEPTH-1, FIFO_getSpaceAvail(h));
        y = 0;
        must_be(1, FIFO_remove(h, (void *)(&rd_data[0]), 1));
        must_be(rd_data[0], wr_data[0]);
        must_be(0    , FIFO_getItemsAvail(h));
        must_be(DEPTH, FIFO_getSpaceAvail(h));
    }

    rd_data[0] = 0;
    rd_data[1] = 0x1234;
    rd_data[2] = 0;

    /* should not be able to remove too man */
    must_be(0, FIFO_remove(h, (void *)(&rd_data[1]),   1));
    must_be(0, FIFO_remove(h, (void *)(&rd_data[1]), 100));
    /* nothing should be written here */
    must_be(rd_data[0], 0);
    must_be(rd_data[1], 0x1234);
    must_be(rd_data[2], 0);

    /* fill completely, then remove completely */
    for(x = 0 ; x < DEPTH ; x++)
    {
        wr_data[x] = ((data_t)(0x1234 + x));
    }
    must_be(DEPTH, FIFO_insert(h, (void *)&wr_data[0], DEPTH));
    must_be(DEPTH, FIFO_getItemsAvail(h));
    must_be(    0, FIFO_getSpaceAvail(h));

    /* read all out */
    memset((void *)(&rd_data[0]), 0, sizeof(rd_data));
    must_be(DEPTH, FIFO_remove(h, (void *)&rd_data[0], DEPTH));
    must_be(0, memcmp(
                       (void *)(&rd_data[0]),
                       (void *)(&wr_data[0]),
                       DEPTH * sizeof(rd_data[0])));

    /* cycle around inserting more then 1 */
    for(x = 0 ; x < DEPTH ; x++)
    {
        for(y = 0 ; y < 5 ; y++)
        {
            wr_data[y] = ((data_t)((x * 256) + y));
        }
        /* we only write 3 */
        must_be(3, FIFO_insert(h, (void *)(&wr_data[0]), 3));
        must_be(3,       FIFO_getItemsAvail(h)) ;
        must_be(DEPTH-3, FIFO_getSpaceAvail(h)) ;

        /* but we try to remove 5 */
        memset((void *)(&rd_data[0]), 0, sizeof(rd_data));
        must_be(3, FIFO_remove(h, (void *)(&rd_data[0]), 5));
        /* first 3 match */
        must_be(0,
                 memcmp((void *)(&wr_data[0]),
                         (void *)(&rd_data[0]),
                         3 * sizeof(wr_data[0])));
        /* and these should still be zero */
        for(y = 3 ; y < 5 ; y++)
        {
            must_be(0, rd_data[y]);
        }
    }

    success();
}

int main(int argc, char **argv)
{
    (void)(argc);
    (void)(argv);
    LOG_init("/dev/stdout");
    /* log_cfg.log_flags = LOG_EVERYTHING; */
    fifo_test1();
    /* todo: Add more later */

    success();
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
