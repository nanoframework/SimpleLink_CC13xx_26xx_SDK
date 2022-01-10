/******************************************************************************
 @file windows_uart.c

 @brief TIMAC 2.0 API Windows specific HLOS implimentation for uarts

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

#include "stream.h"
#include "malloc.h"
#include "fifo.h"
#include "threads.h"
#include "stream_uart.h"
#include "log.h"
#include "void_ptr.h"
#include "unix_fdrw.h"
#include "timer.h"

#define _STREAM_IMPLIMENTOR_ 1
#include "stream_private.h"

#include <stdio.h>
#include <stdbool.h>

/*!
 * \def UF_isSet()
 * @brief test if a uart flag is set or not
 */
#define UF_isSet(pUL, FN) \
    ((pUL)->cfg.open_flags & (STREAM_UART_FLAG_ ## FN))

/*!
 * @var [private] test variable to determine if stream is a uart or not
 */
static const int  uart_test = 'U';

/*!
 * @struct windows_uart
 *
 * Private UART details for a UART on Linux
 */
struct windows_uart {
    /*! Parent owning stream */
    struct io_stream *pParent;
    /*! used to verify pointer, should point to uart_test */
    const int  *test_ptr;

    /*! Windows file Handle */
    HANDLE h;

    /*! uart configuration */
    struct uart_cfg cfg;
    char windows_device_name[100];

    /*! Has the terminal conditions been set? */
    DCB dcb;

    COMMTIMEOUTS ctimeout;

#define LUTS_birth 0 /*! Linux Uart Thread State, not started yet */
#define LUTS_alive 1 /*! Linux Uart Thread State, running */
#define LUTS_exit  2 /*! Linux Uart Thread State, requested to exit */
#define LUTS_dead  3 /*! Linux Uart Thread State, It's Dead Jim.. */
    volatile int    thread_state;
    /*! If this UART is using a fifo this is the fifo handle */
    intptr_t rx_fifo;
    /*! If this uart is using a thread to read, this is the thread handle */
    intptr_t rx_thread;
};

/*!
 * @brief [private] Convert a io_stream to a Linux Uart Pointer
 * @param pIO - the io stream
 * @return linux uart pointer, or NULL if not valid
 */

static struct windows_uart *_uart_pio_to_pWU(struct io_stream *pIO)
{
    struct windows_uart *pWU;

    /* Cast */
    pWU = (struct windows_uart *)(pIO->opaque_ptr);

    /* do we have a pointer */
    if(pWU)
    {
        /* Verify the internal pointer */
        if(pWU->test_ptr != &uart_test)
        {
            pWU = NULL;
        }
    }
    return (pWU);
}

static struct windows_uart *uart_pio_to_pWU(struct io_stream *pIO)
{
    struct windows_uart *pWU;

    pWU = _uart_pio_to_pWU(pIO);
    if(pWU == NULL)
    {
        LOG_printf(LOG_ERROR, "not a uart: %p\n", (void *)(pIO));
    }
    return (pWU);
}

bool STREAM_isUart(intptr_t h)
{
    struct windows_uart *pWU;

    pWU = _uart_pio_to_pWU(STREAM_hToStruct(h));
    if(pWU)
    {
        return (true);
    }
    else
    {
        return (false);
    }
}

/*!
 * @brief Common routine to log a UART error
 * @param pWU - the linux uart
 * @param msg1 - message
 * @param is_syscall_error - true if error relates to GetLastError()
 */
static void _uart_error(struct windows_uart *pWU,
                         const char *msg1, bool is_syscall_error)
{
    LPVOID lpMsgBuf;
    DWORD gle;
    /* mark the error */
    pWU->pParent->is_error = true;
    /* do we need to look up strerror? */

    lpMsgBuf = NULL;
    gle = 0;
    if(is_syscall_error)
    {
        gle = GetLastError();
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            gle,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&lpMsgBuf,
            0, NULL);
    }

    if(is_syscall_error)
    {
        LOG_printf(LOG_ERROR, "%s: %s [0x%08x] %s\n",
            pWU->windows_device_name,
            msg1,
            (unsigned)gle,
            (char *)(lpMsgBuf));
    }
    else
    {
        LOG_printf(LOG_ERROR, "%s: %s\n",
            pWU->windows_device_name,
            msg1);
    }
    if(lpMsgBuf)
    {
        LocalFree(lpMsgBuf);
        lpMsgBuf = NULL;
    }
}

/*!
 * @brief [private] Code to handl the STREAM_WrBytes() for uarts
 * @param pIO - the io stream
 * @param databytes - pointer to data buffer
 * @param nbytes- count of bytes to write
 * @param timeout_mSecs - write timeout
 *
 * @returns negative on error, 0..actual written
 */
static int _uart_wrBytes(struct io_stream *pIO,
                          const void *databytes,
                          size_t nbytes, int timeout_mSecs)
{
    struct unix_fdrw rw;
    struct windows_uart *pWU;

    pWU = uart_pio_to_pWU(pIO);
    if(!pWU)
    {
        return (-1);
    }

    /* setup common IO routine */
    memset((void *)(&rw), 0, sizeof(rw));

    rw.is_connected = true;
    rw.type         = 'u';
    rw.rw           = 'w';
    rw.fd           = (intptr_t)(pWU->h);
    rw.log_prefix   = "uart-wr";
    rw.log_why      = LOG_DBG_UART;
    rw.log_why_raw  = LOG_DBG_UART_RAW;
    rw.c_bytes      = databytes;
    rw.n_todo       = nbytes;
    rw.fifo_handle  = 0; /* we don't (currently) do writes via fifo operations */
    rw.mSecs_timeout = timeout_mSecs;

    /* use the common code */
    return (UNIX_fdRw(&rw));
}

/*!
 * @brief [private] Code to handl the STREAM_RdBytes() for uarts
 * @param pIO - the io stream
 * @param databytes - pointer to data buffer
 * @param nbytes- count of bytes to read
 * @param timeout_mSecs - read timeout
 *
 * @returns negative on error, 0..actual read
 */
static int _uart_rdBytes(struct io_stream *pIO,
                          void *databytes, size_t nbytes, int timeout_mSecs)
{
    struct unix_fdrw rw;
    struct windows_uart *pWU;

    pWU = uart_pio_to_pWU(pIO);
    if(!pWU)
    {
        return (-1);
    }

    /* setup common IO routine */
    memset((void *)(&rw), 0, sizeof(rw));

    rw.is_connected  = true;
    rw.type          = 'u';
    rw.rw            = 'r';
    rw.fd            = (intptr_t)(pWU->h);
    rw.log_prefix    = "uart-rd";
    rw.log_why       = LOG_DBG_UART;
    rw.log_why_raw   = LOG_DBG_UART_RAW;
    rw.v_bytes       = databytes;
    rw.n_todo        = nbytes;
    rw.fifo_handle   = pWU->rx_fifo;
    rw.mSecs_timeout = timeout_mSecs;

    /* use the common code */
    return (UNIX_fdRw(&rw));
}

/*!
 * @brief [private] Poll function for the UART
 * @param pIO - io stream for the uart
 * @param mSec_timeout - timeout period for the poll
 *
 * @returns boolean, true if it is readable within the timeout period
 */
static bool _uart_pollRxAvail(struct io_stream *pIO, int mSec_timeout)
{
    struct unix_fdrw rw;
    struct windows_uart *pWU;
    int r;

    /* retrieve our linux uart */
    pWU = uart_pio_to_pWU(pIO);
    if(pWU == NULL)
    {
        return (false);
    }

    /* if we are using the RX thread.. */
    /* we simply check the fifo. */
    if(pWU->rx_fifo)
    {
        r = FIFO_getItemsAvail(pWU->rx_fifo);
    }
    else
    {
        memset((void *)(&rw), 0, sizeof(rw));
        rw.is_connected = true;
        rw.fd           = (intptr_t)(pWU->h);
        rw.rw           = 'r';
        rw.type         = 'u';
        rw.log_prefix   = "uart-poll";
        rw.log_why      = LOG_DBG_UART;
        rw.log_why_raw  = LOG_DBG_UART_RAW;
        rw.mSecs_timeout = mSec_timeout;
        r = POLL_readable(&rw);
        if((r < 0) || (rw.is_error))
        {
            _uart_error(pWU, "poll",false);
            r = 0;
        }
    }
    if(r)
    {
        return (true);
    }
    else
    {
        return (false);
    }
}

/*!
 * @brief Close/Deallocate the uart information.
 */
static void _uart_closePtr(struct windows_uart *pWU)
{
    int n;

    if(pWU == NULL)
    {
        return;
    }

    /* if we are using a thread, tell it to shut down */
    if(pWU->rx_thread)
    {
        pWU->thread_state = LUTS_exit;
        /* wait for thread to wakeup and acknowledge */
        /* when it does acknowlege, it will exit */
        for(n = 0 ; n < 100 ; n++)
        {
            if(pWU->thread_state != pWU->thread_state)
            {
                /* YEA it has stopped. */
                break;
            }
            else
            {
                /* be power nice. */
                TIMER_sleep(1);
            }
        }
        FIFO_destroy(pWU->rx_fifo);
        /* The thread has exited we don't need to clean up */
        pWU->rx_thread = 0;
    }

    /* do we need to restore the line state? */
    /* not on windows.. */

    /* release our copy of the device name */
    if(pWU->cfg.devname)
    {
        free((void *)(pWU->cfg.devname));
        pWU->cfg.devname = NULL;
    }

    /* Close the handle */
    if(pWU->h != INVALID_HANDLE_VALUE)
    {
        CloseHandle(pWU->h);
        pWU->h = INVALID_HANDLE_VALUE;
    }

    if(pWU->pParent)
    {
        STREAM_destroyPrivate(pWU->pParent);
        pWU->pParent = NULL;
    }
    memset((void *)(pWU), 0, sizeof(*pWU));
    /* And release our uart structure */
    free((void *)(pWU));
}

/*!
 * @brief [private] Close the UART
 * @param pIO - the io stream for the uart
 */
static void _uart_close(struct io_stream *pIO)
{
    struct windows_uart *pWU;

    pWU = uart_pio_to_pWU(pIO);
    if(pWU)
    {
        _uart_closePtr(pWU);
    }
}

/*!
 * @brief This is the rx thread for the uart
 * @brief h - the linux uart in abstract
 * @returns Nothing meaningful
 */
static intptr_t _uart_rx_thread(intptr_t h)
{
    struct windows_uart *pWU;
    struct unix_fdrw  rw;
    int r;

    /* recover the linux uart */
    pWU = (struct windows_uart *)(h);

    /* mark the new thread state */
    pWU->thread_state = LUTS_alive;

    /* we run until we are toll to exit. */
    while(pWU->thread_state == LUTS_alive)
    {
        if(pWU->pParent->is_error)
        {
            break;
        }
        memset((void *)(&rw), 0, sizeof(rw));
        rw.is_connected  = true;
        rw.fd            = (intptr_t)(pWU->h);
        rw.type          = 'u';
        rw.log_prefix    = pWU->windows_device_name;
        rw.log_why       = LOG_DBG_UART;
        rw.log_why_raw   = LOG_DBG_UART_RAW;
        rw.rw            = 'r';
        rw.fifo_handle   = pWU->rx_fifo;

        rw.c_bytes       = NULL;
        rw.v_bytes       = NULL;
        rw.n_done        = 0;
        rw.n_todo        = 0;
        rw.mSecs_timeout = 100;

        r = POLL_readable(&rw);
        if((r < 0) || (rw.is_error))
        {
            _uart_error(pWU, "poll error?",false);
            continue;
        }
        if(!r)
        {
            /* no data.. */
            continue;
        }

        /* do the transfer */
        r = UNIX_fdRw(&rw);
        if((r < 0) || (rw.is_error))
        {
            _uart_error(pWU, "fifo xfer error",false);
        }
    }

    pWU->thread_state = LUTS_dead;
    /* Done */
    return (0);
}

/*!
 * @brief [private] Setup a thread for this uart
 * @param pWU - the linux uart
 */
static void _setup_rx_thread(struct windows_uart *pWU)
{
    /* are we using a thread? */
    if(!UF_isSet(pWU, rd_thread))
    {
        return;
    }

    /* FIFO for *bytes*, depth 4K, with an access mutex. */
    pWU->rx_fifo   = FIFO_create(pWU->windows_device_name,
                                  sizeof(uint8_t), __4K, true);
    if(pWU->rx_fifo == 0)
    {
        _uart_error(pWU, "no fifo?", false);
        return;
    }

    /* We are going to "give birth" our thread.. */
    pWU->thread_state = LUTS_birth;

    /* Create the thread */
    pWU->rx_thread = THREAD_create(pWU->windows_device_name,
                                    _uart_rx_thread, (intptr_t)(pWU),
                                    THREAD_FLAGS_DEFAULT);

    /* Hmm something is wrong */
    if(pWU->rx_thread == 0)
    {
        _uart_error(pWU, "no rx thread?",false);
        return;
    }
}

/*!
 * @brief Flush the uart transmission
 * @param pIO - the uart stream
 * @returns 0 on success, negative on error
 */
static int _uart_flush(struct io_stream *pIO)
{
    int r;
    struct windows_uart *pWU;

    /* recover our pointer */
    pWU    = uart_pio_to_pWU(pIO);
    if(!pWU)
    {
        return (-1);
    }

    /* not supported on windows */
    r = 0;
    return (r);
}

/*!
 * @var STREAM_uart_funcs
 * @brief [private]
 *
 * This is the various functions that provide for the uart
 */
static const struct io_stream_funcs STREAM_uart_funcs = {
    .name          = "uart",
    .close_fn      = _uart_close,
    .wr_fn         = _uart_wrBytes,
    .rd_fn         = _uart_rdBytes,
    .flush_fn      = _uart_flush,
    .poll_fn       = _uart_pollRxAvail
};

/*
 * Create a uart stream
 *
 * Public function defined in stream_uart.h
 */
intptr_t STREAM_createUart(const struct uart_cfg *pCFG)
{
    int r;
    struct windows_uart *pWU;

    pWU = NULL;
    /* minimal sanity */
    if(pCFG->devname == NULL)
    {
        LOG_printf(LOG_ERROR, "uart: no devicename?\n");
        goto fail;
    }

    if(pCFG->baudrate == 0)
    {
        LOG_printf(LOG_ERROR, "uart: bad baudrate?\n");
        goto fail;
    }

    /* get space for our private stuff */
    pWU = (struct windows_uart *)calloc(1, sizeof(*pWU));
    if(!pWU)
    {
        goto fail;
    }

    /* in case this goes bad.. */
    pWU->h = INVALID_HANDLE_VALUE;

    /* mark as a valid structure */
    pWU->test_ptr = &uart_test;

    /* Remember things */
    pWU->pParent = STREAM_createPrivate(&STREAM_uart_funcs, (intptr_t)(pWU));

    /* copy the cfg over */
    pWU->cfg = *pCFG;

    /* convert caller pointer to our pointer */
    pWU->cfg.devname = strdup(pWU->cfg.devname);
    if(pWU->cfg.devname == NULL)
    {
        _uart_error(pWU, "no memory?", false);
        goto fail;
    }

    (void)snprintf(pWU->windows_device_name,
                   sizeof(pWU->windows_device_name),
                   "\\\\.\\%s", pWU->cfg.devname);
    /* the CFG file says:  "COM1" or maybe "COM9" */
    /* windows wants something else... */

    /* get our device. */
    LOG_printf(LOG_DBG_UART, "open(%s) begin\n", pWU->windows_device_name);
    pWU->h = CreateFileA(pWU->windows_device_name,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    LOG_printf(LOG_DBG_UART, "open(%s) result=%p\n",
                pWU->windows_device_name,((void *)(pWU->h)));
    if(pWU->h == INVALID_HANDLE_VALUE)
    {
        _uart_error(pWU,"open",true);
        goto fail;
    }

    if(!GetCommState(pWU->h, &(pWU->dcb)))
    {
        _uart_error(pWU, "GetCommState()", true);
        goto fail;
    }

    /* Map Baudrate */
    r = -1;

#define _b(X)   case X: r = CBR_ ## X; break /* no ending semicolon */
    switch (pWU->cfg.baudrate)
    {
        _b(110);
        _b(300);
        _b(600);
        _b(1200);
        _b(2400);
        _b(4800);
        _b(9600);
        _b(14400);
        _b(19200);
        _b(38400);
        _b(56000);
        _b(57600);
        _b(115200);
        _b(128000);
        _b(256000);
    default:
        r = -1;
        break;
    }
#undef _b
    if(r == -1)
    {
        _uart_error(pWU, "Unsupported baudrate\n", false);
        goto fail;
    }
    pWU->dcb.ByteSize = 8;
    pWU->dcb.BaudRate = r;
    pWU->dcb.fBinary = true;
    pWU->dcb.fParity = false;
    pWU->dcb.fOutX = false;
    pWU->dcb.fOutxCtsFlow = false;
    pWU->dcb.fOutxDsrFlow = false;
    pWU->dcb.fDtrControl = DTR_CONTROL_ENABLE;
    pWU->dcb.fDsrSensitivity = false;
    pWU->dcb.fTXContinueOnXoff = false;
    pWU->dcb.fInX = false;
    pWU->dcb.fErrorChar = false;
    pWU->dcb.fNull = false;
    pWU->dcb.fRtsControl = RTS_CONTROL_ENABLE;
    pWU->dcb.fAbortOnError = false;
    pWU->dcb.Parity = NOPARITY;
    pWU->dcb.StopBits = ONESTOPBIT;

    if(!SetCommState(pWU->h, &(pWU->dcb)))
    {
        _uart_error(pWU, "SetCommState()", true);
        goto fail;
    }

    GetCommTimeouts(pWU->h, &(pWU->ctimeout));
    /* Interval between two bytes.. */
    /* we normally run at 115.2k baud */
    /* this is about 11 bytes per millisecond */
    /* 50 byte delay is about 5 mSec */
    pWU->ctimeout.ReadIntervalTimeout = 5;
    pWU->ctimeout.ReadTotalTimeoutMultiplier = 1;
    pWU->ctimeout.ReadTotalTimeoutConstant = 5;
    pWU->ctimeout.WriteTotalTimeoutConstant = 5;
    pWU->ctimeout.WriteTotalTimeoutMultiplier = 1;

    SetCommTimeouts(pWU->h, &(pWU->ctimeout));

    /* setup the thread if needed */
    _setup_rx_thread(pWU);
    if(pWU->pParent->is_error)
    {
        goto fail;
    }

    r = 0;

    if(r<0)
    {
 fail:
        _uart_closePtr(pWU);
        return (0);
    }
    else
    {
        /* all is well */
        return (STREAM_structToH(pWU->pParent));
    }
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
