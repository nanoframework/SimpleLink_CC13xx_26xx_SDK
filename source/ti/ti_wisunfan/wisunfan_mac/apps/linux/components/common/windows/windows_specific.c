/******************************************************************************
 @file windows_specific.c

 @brief TIMAC 2.0 API Windows specific HLOS implimentation functions

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
#include <Windows.h>
#include "winsock2.h"
#include "ws2tcpip.h"
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <malloc.h>
#include "stream.h"
#include "timer.h"
#include "stream_socket.h"
#include "log.h"
#include "unix_fdrw.h"
#include <time.h>
#include "hlos_specific.h"
#include "fatal.h"
#include "debug_helpers.h"

#include <sys/types.h> /* for stat */
#include <sys/stat.h>  /* for stat */

#include <conio.h>

/* tell the linker to link with winsock */
#pragma comment(lib, "Ws2_32.lib")

#undef gai_strerror /* we want only the ascii version here */
#define gai_strerror gai_strerrorA

static intptr_t atomic_global = 0;

static const int is_atomic = 'A';
static const int is_sem = 's';

struct p_atomic {
    const int *check_ptr;
    HANDLE s;
    DWORD  owner;
};

struct p_sem {
    const int *check_ptr;
    HANDLE s;
};

static struct p_atomic *h2pA(intptr_t h)
{
    struct p_atomic *pA;

    pA = (struct p_atomic *)h;
    if(pA)
    {
        if(pA->check_ptr != &is_atomic)
        {
            pA = NULL;
            /* DO NOT use log code here. */
            /* why? Because we might be in the log code */
            /* and then things get really messy */
        }
    }
    return (pA);
}

static struct p_sem *h2pS(intptr_t h)
{
    struct p_sem *pS;

    pS = (struct p_sem *)h;
    if(pS)
    {
        if(pS->check_ptr != &is_sem)
        {
            pS = NULL;
            /* DO NOT use log code here. */
            /* why? Because we might be in the log code */
            /* and then things get really messy */
        }
    }
    return (pS);
}

static void MSVC_NO_RETURN _atomic_fatal(const char *msg) GCC_NO_RETURN
{
    fprintf(stderr, "%s\n", msg);
    _FATAL_exit(1);
}

void _FATAL_exit(int code)
{
    if(IsDebuggerPresent())
    {
        DebugBreak();
    }
    _EXIT_pause(code);
}

static void _atomic_init(void)
{
    if(atomic_global == 0)
    {
        atomic_global = _ATOMIC_local_create();
    }
}

void _ATOMIC_global_lock(void)
{
    _atomic_init();
    _ATOMIC_local_lock(atomic_global, -1);
}

void _ATOMIC_global_unlock(void)
{
    _atomic_init();
    _ATOMIC_local_unlock(atomic_global);
}

intptr_t _ATOMIC_local_create(void)
{
    struct p_atomic *pA;

    pA = calloc(1, sizeof(*pA));
    if(pA == NULL)
    {
        _atomic_fatal("no memory for atomic\n");
    }
    pA->check_ptr = &is_atomic;
    pA->owner = 0;
    pA->s = CreateMutex(NULL, FALSE, NULL);
    if(pA->s == NULL)
    {
        _atomic_fatal("cannot create atomic\n");
    }
    return ((intptr_t)(pA));
}

void _ATOMIC_local_destroy(intptr_t h)
{
    struct p_atomic *pA;

    pA = h2pA(h);
    if(pA)
    {
        CloseHandle(pA->s);
        memset((void *)(pA), 0, sizeof(*pA));
    }
}

void _ATOMIC_local_unlock(intptr_t h)
{
    struct p_atomic *pA;

    pA = h2pA(h);
    if(pA)
    {
        ReleaseMutex(pA->s);
        pA->owner = 0;
    }

}

int _ATOMIC_local_lock(intptr_t h, int timeout_mSecs)
{
    DWORD dw_retval;
    DWORD dw_timeout;
    struct p_atomic *pA;

    pA = h2pA(h);
    if(pA == NULL)
    {
        return (-1);
    }

    /* Microsoft wait for object has the *SAME* */
    /* symantics as we do, with the exception */
    /* the timeout parameter is a DWORD... */
    /* so we adjust ... */
    if(timeout_mSecs < 0)
    {
        dw_timeout = INFINITE;
    }
    else
    {
        dw_timeout = timeout_mSecs;
    }

    dw_retval = WaitForSingleObject(pA->s, dw_timeout);

    if(dw_retval == WAIT_OBJECT_0)
    {
        /* great success */
        pA->owner = GetCurrentThreadId();
        return (0);
    }
    else
    {
        /* failed */
        return (-1);
    }
}

intptr_t _ATOMIC_sem_create(void)
{
    struct p_sem *pS;

    pS = calloc(1, sizeof(*pS));
    if(pS == NULL)
    {
        _atomic_fatal("no mem for sem\n");
    }
    pS->check_ptr = &is_sem;
    pS->s = CreateSemaphore(NULL, 0, 1000000, NULL);
    if(pS->s == NULL)
    {
        _atomic_fatal("cannot create sem\n");
    }
    return ((intptr_t)(pS));
}

void _ATOMIC_sem_destroy(intptr_t h)
{
    struct p_sem *pS;

    pS = h2pS(h);
    if(pS)
    {
        if(pS->s)
        {
            CloseHandle(pS->s);
        }
        memset((void *)(pS), 0, sizeof(*pS));
        free((void*)(pS));
    }
}

void _ATOMIC_sem_put(intptr_t h)
{
    struct p_sem *pS;

    pS = h2pS(h);
    if(pS == NULL)
    {
        _atomic_fatal("not a semaphore\n");
        return;
    }

    ReleaseSemaphore(pS->s, 1, NULL);
}

int _ATOMIC_sem_get(intptr_t h, int timeout_mSecs)
{
    struct p_sem *pS;
    DWORD dw;
    DWORD dwtimeout;

    pS = h2pS(h);
    if(pS == NULL)
    {
        _atomic_fatal("not a semaphore\n");
    }

    if(timeout_mSecs < 0)
    {
        dwtimeout = INFINITE;
    }
    else
    {
        dwtimeout = (DWORD)(timeout_mSecs);
    }
    dw = WaitForSingleObject(pS->s, dwtimeout);
    if(dw == WAIT_OBJECT_0)
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

int _ATOMIC_sem_cnt(intptr_t h)
{
    struct p_sem *pS;
    DWORD dw;
    LONG count;

    pS = h2pS(h);
    if(pS == NULL)
    {
        _atomic_fatal("not a semaphore\n");
    }

    /* try to take the semaphore */
    count = 0;
    dw = WaitForSingleObject(pS->s, 0);
    if(dw != WAIT_OBJECT_0)
    {
        /* the count is zero, because we could not acquire */
        count = 0;
    }
    else
    {
        /* the count is at least 1, and we took it. */
        /* so go put it back. */
        ReleaseSemaphore(pS->s, 1, &count);
        /* the lpPreviousCount is the OLD value */
        /* which we need to add our 1 back into */
        count++;
    }
    return ((int)(count));
}

uint64_t _TIMER_getAbsNow(void)
{
    ULONGLONG ull;
    ull = GetTickCount64();
    return (uint64_t)(ull);
}

void _TIMER_sleep(uint32_t mSecs)
{
    uint64_t tend;
    int64_t delta;

    tend = _TIMER_getAbsNow();
    tend += mSecs;

    for(;;)
    {
        delta = (int64_t)(tend - _TIMER_getAbsNow());
        if(delta <= 0)
        {
            break;
        }
        Sleep((DWORD)(delta));
    }
}

static const int thread_check = 'T';

struct win_thread {
    const int *check_ptr;
    HANDLE hThread;
    intptr_t cookie;
    DWORD thread_id;
    intptr_t(*thread_func)(intptr_t cookie);
    struct win_thread *pNext;
};

static struct win_thread *all_win_threads;

static DWORD WINAPI win_thread_wrapper(LPVOID p)
{
    struct win_thread *pT;
    pT = (struct win_thread *)p;

    (*(pT->thread_func))(pT->cookie);
    /* Do not reference pT because */
    /* it may have been destroyed... */
    pT = NULL;
    return (0);
}

/* From */
/* ------------------------------------------------------- */
/* https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx */
/* ------------------------------------------------------- */

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; /* Must be 0x1000. */
    LPCSTR szName; /* Pointer to name (in user addr space). */
    DWORD dwThreadID; /* Thread ID (-1=caller thread). */
    DWORD dwFlags; /* Reserved for future use, must be zero. */
} THREADNAME_INFO;
#pragma pack(pop)

static void SetThreadName(DWORD dwThreadID, const char* threadName)
{
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
    __try {
        #define MS_VC_EXCEPTION 0x406D1388
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
#pragma warning(pop)
}

intptr_t _THREAD_create(const char *dbg_name,
                        intptr_t(*thread_func)(intptr_t threadparam),
                        intptr_t cookie)
{
    struct win_thread *pT;

    pT = calloc(1, sizeof(*pT));
    if(pT == NULL)
    {
        _atomic_fatal("no memory for thread\n");
    }
    pT->check_ptr = &thread_check;
    pT->cookie = cookie;
    pT->thread_func = thread_func;
    _ATOMIC_global_lock();
    pT->pNext = all_win_threads;
    all_win_threads = pT;
    pT->hThread = CreateThread(NULL,
                                0,
                                win_thread_wrapper,
                                (void *)(pT), 0, &pT->thread_id);
    if(pT->hThread)
    {
        SetThreadName(GetThreadId(pT->hThread), dbg_name);
    }
    _ATOMIC_global_unlock();

    return ((intptr_t)(pT));
}

intptr_t _THREAD_self(void)
{
    struct win_thread *pT;
    DWORD whoami;

    whoami = GetCurrentThreadId();
    _ATOMIC_global_lock();

    pT = all_win_threads;
    while(pT)
    {
        if(whoami == pT->thread_id)
        {
            break;
        }
        pT = pT->pNext;
    }
    _ATOMIC_global_unlock();
    return ((intptr_t)(pT));
}

void _THREAD_exit(void)
{
    struct win_thread *pT;

    pT = (struct win_thread *)_THREAD_self();
    pT->hThread = INVALID_HANDLE_VALUE;
    ExitThread(0);
}

void _THREAD_destroy(intptr_t t)
{
    struct win_thread *pT;
    struct win_thread **ppT;

    pT = (struct win_thread *)(t);
    ppT = &(all_win_threads);

    _ATOMIC_global_lock();
    while(*ppT)
    {
        if(pT == *ppT)
        {
            break;
        }
        ppT = &((*ppT) ->pNext);
    }
    /* remove from list */
    if(ppT)
    {
        /* found so de-link */
        *ppT = pT->pNext;
    }
    pT->pNext = NULL;
    _ATOMIC_global_unlock();
    if(pT->hThread != INVALID_HANDLE_VALUE)
    {
/* Yes we know we TerminateThread() can't clean up! */
#pragma warning(push)
#pragma warning(disable: 6258)
        TerminateThread(pT->hThread, 0);
#pragma warning(pop)
        pT->hThread = INVALID_HANDLE_VALUE;
    }
    memset((void *)pT, 0, sizeof(*pT));

    free((void *)(pT));
}

/*!
 * @brief windows specific poll comport for readability.
 */
static int poll_uart_readable(struct unix_fdrw *pRW)
{
    COMSTAT cs;
    timertoken_t tstart;
    bool b;

    if(pRW->mSecs_timeout > 0)
    {
        tstart = TIMER_timeoutStart();
    }
    else
    {
        tstart = 0;
    }

    for(;;)
    {
        /* we don't clear anything... */
        /* we are calling it to get the un-read byte count in the uart. */
        b = ClearCommError((HANDLE)(pRW->fd), 0, &cs);
        if(!b)
        {
            pRW->is_error = true;
            LOG_printf(LOG_ERROR, "Comport has died\n");
            return (1);
        }
        if(cs.cbInQue)
        {
            return (1);
        }
        /* immediate poll */
        if(pRW->mSecs_timeout == 0)
        {
            return (0);
        }
        /* wait forever.. */
        if(pRW->mSecs_timeout < 0)
        {
            /* don't beat on this, be nice.. */
            /* need a better solution here */
            /* but this is good enough for now. */
            /* this is about 50 character times @ 115.2Kbaud */
            TIMER_sleep(5);
            continue;
        }
        TIMER_sleep(5);
        if(TIMER_timeoutIsExpired(tstart, pRW->mSecs_timeout))
        {
            return (0);
        }
    }
}

/*!
 * @brief [private] common polling function.
 * @param fd - file descriptor
 * @param mSecs - postive how long to wait, 0 do not wait, negative forever
 *
 * @return postive action is possible, 0 not possible, negative error/timeout
 */
static int poll_socket(struct unix_fdrw *pRW)
{
    fd_set err_set;
    fd_set rw_set;
    int r;
    struct timeval tv;
    struct timeval *pTV;

    /* valid FD? */
    if(pRW->fd <= 0)
    {
        LOG_printf(LOG_ERROR, "fd: %d is not valid\n", (int)(pRW->fd));
        return (-1);
    }

    FD_ZERO(&rw_set);
    FD_ZERO(&err_set);

    FD_SET((SOCKET)(pRW->fd), &rw_set);
    FD_SET((SOCKET)(pRW->fd), &err_set);

    memset((void *)(&tv), 0, sizeof(tv));
    pTV = &tv;
    if(pRW->mSecs_timeout < 0)
    {
        pTV = NULL;
    }
    else
    {
        r = pRW->mSecs_timeout % 1000;
        tv.tv_usec = r * 1000;
        r = pRW->mSecs_timeout / 1000;
        tv.tv_sec = r;
    }

    if(pRW->rw == 'r')
    {
        r = select(1, &rw_set, NULL, &err_set, pTV);
    }
    else
    {
        r = select(1, NULL, &rw_set, &err_set, pTV);
    }
    if((r < 0) || FD_ISSET((SOCKET)(pRW->fd), &err_set))
    {
        pRW->is_error = true;
        if(pRW->type == 's')
        {
            LOG_printf(LOG_ERROR, "%s: Socket error: %sn",
                pRW->log_prefix, gai_strerror(WSAGetLastError()));
        }
        else
        {
            LOG_printf(LOG_ERROR, "%s: error...??\n",
                pRW->log_prefix);
        }
        return (-1);
    }
    r = FD_ISSET((SOCKET)(pRW->fd), &rw_set);
    return (r);
}

int POLL_readable(struct unix_fdrw *pRW)
{
    int r;

    r = -1;
    if(pRW->rw != 'r')
    {
        BUG_HERE("not reading?\n");
    }
    switch (pRW->type)
    {
    default:
        BUG_HERE("unspported device type\n");
        break;
    case 'f':
        /* we can always read */
        r = 1;
        break;
    case 'u':
        r = poll_uart_readable(pRW);
        break;
    case 's':
        r = poll_socket(pRW);
        break;
    }
    return (r);
}

int POLL_writable(struct unix_fdrw *pRW)
{
    int r;

    r = -1;
    if(pRW->rw != 'w')
    {
        BUG_HERE("not writing?\n");
    }
    switch (pRW->type)
    {
    default:
        BUG_HERE("unspported device type\n");
        break;
    case 'f':
        /* we can always write */
        r = 1;
        break;
    case 's':
        r = poll_socket(pRW);
        break;
    case 'u':
        /* uart - always writable */
        r = 1;
        break;
    }
    return (r);
}

void UNIX_fdRdThis(struct unix_fdrw *pRW)
{
    bool b;
    int e;
    DWORD a;
    struct unix_fdrw tmp;

    pRW->this_actual = 0;

    if(pRW->type == 's')
    {
        tmp = *pRW;
        tmp.mSecs_timeout = 0;
        b = POLL_readable(&tmp);
        pRW->this_actual = recv(pRW->fd,
                                pRW->this_v_buf, (int)(pRW->this_len), 0);
        if(pRW->this_actual < 0)
        {
            /* Most likely we have a disconnect. */
            e = WSAGetLastError();
            LOG_printf(LOG_ERROR, "%s: Socket error (%d) %s\n",
                       pRW->log_prefix, e, gai_strerrorA(e));
            pRW->is_error = true;
            pRW->is_connected = false;
        }
        if(b && (pRW->this_actual == 0))
        {
            /* then we have a disconnect.. */
            pRW->is_connected = false;
            pRW->is_error = true;
        }
        return;
    }

    b = ReadFile((HANDLE)(pRW->fd), pRW->this_v_buf,
                 (int)(pRW->this_len), &a, NULL);
    if(b)
    {
        pRW->this_actual = (int)(a);
    }
    else
    {
        pRW->is_error = true;
        pRW->this_actual = 0;
    }
}

void UNIX_fdWrThis(struct unix_fdrw *pRW)
{
    bool b;
    DWORD a;
    DWORD64 tstart;
    DWORD64 tend;

    pRW->this_actual = 0;

    if(pRW->type =='s')
    {
        pRW->this_actual = send(pRW->fd,
                                pRW->this_c_buf, (int)(pRW->this_len), 0);
        return;
    }
    tstart = GetTickCount64();
    b = WriteFile((HANDLE)(pRW->fd), pRW->this_c_buf,
                  (DWORD)(pRW->this_len), &a, NULL);
    tend = GetTickCount64();
    /*LOG_printf(LOG_ALWAYS, "tx start: %u, tend=%u, delta=%u\n", */
    /*          (unsigned)tstart, (unsigned)tend, (unsigned)(tend - tstart)); */
    if(b)
    {
        pRW->this_actual = (int)(a);
    }
    else
    {
        pRW->this_actual = 0;
        pRW->is_error = true;
    }
}

WSADATA wsa_details;

void SOCKET_init(void)
{
    int e;
    e = WSAStartup(MAKEWORD(2, 2), &wsa_details);
    if(e != 0)
    {
        fprintf(stderr,
                 "Cannot initialize windows sockes: %s\n", gai_strerror(e));
        FATAL_printf("Cannot initialize windows sockes: %s\n", gai_strerror(e));
    }
}

void free_const(const void *vp)
{
    free((void *)vp);
}

bool STREAM_FS_fileExists(const char *filename)
{
    struct _stat64 sbuf;
    int r;
    /* determine if the file exists ... */
    r = _stat64(filename, &sbuf);
    if(r < 0)
    {
        return (false);
    }

    if(!(sbuf.st_mode & _S_IFREG))
    {
        /* not a file */
        return (false);
    }
    /* could be a device. */
    /* we see char device.. */
    /* and fifo .. */
    /* but no block */
    if(sbuf.st_mode & (_S_IFCHR | _S_IFIFO))
    {
        return (false);
    }
    else
    {
        return (true);
    }
}

int64_t STREAM_FS_getSize(const char *filename)
{
    int64_t r;
    struct _stat64 sbuf;

    /* determine if the file exists ... */
    r = _stat64(filename, &sbuf);
    if(r < 0)
    {
        return (-1);
    }

    if(!(sbuf.st_mode & _S_IFREG))
    {
        /* not a file */
        return (-1);
    }
    /* could be a device. */
    /* we see char device.. */
    /* and fifo .. */
    /* but no block */
    if(sbuf.st_mode & (_S_IFCHR | _S_IFIFO))
    {
        return (-1);
    }

    return ((uint64_t)sbuf.st_size);
}

void _EXIT_pause(int code)
{
    fprintf(stdout, "\n");
    fprintf(stdout,"===========================\n");
    fprintf(stdout, "Press any key to continue..\n");
    fprintf(stdout, "===========================\n");

    (void)getch();
    exit(code);
}

static int decode_key2(int key1, int key2)
{
    int r;
    int x;
    /* See remarks here: */
    /* https://msdn.microsoft.com/en-us/library/078sfkak.aspx */

    struct { int key1; int key2; int r; } lut[] = {

        {.key1 = 0xe0,.key2 = 0x48, .r = DEBUG_KEY_u_arrow }, /* independent */
        {.key1 = 0xe0,.key2 = 0x50, .r = DEBUG_KEY_d_arrow },
        {.key1 = 0xe0,.key2 = 0x4b, .r = DEBUG_KEY_l_arrow },
        {.key1 = 0xe0,.key2 = 0x4d, .r = DEBUG_KEY_r_arrow },

        { .key1 = 0x00,.key2 = 0x48,.r = DEBUG_KEY_u_arrow }, /* keypad */
        { .key1 = 0x00,.key2 = 0x50,.r = DEBUG_KEY_d_arrow },
        { .key1 = 0x00,.key2 = 0x4b,.r = DEBUG_KEY_l_arrow },
        { .key1 = 0x00,.key2 = 0x4d,.r = DEBUG_KEY_r_arrow },

        {.key1 = 0xe0,.key2 = 0x8d, .r = DEBUG_KEY_cu_arrow },
        {.key1 = 0xe0,.key2 = 0x91, .r = DEBUG_KEY_cd_arrow },
        {.key1 = 0xe0,.key2 = 0x73, .r = DEBUG_KEY_cl_arrow },
        {.key1 = 0xe0,.key2 = 0x74, .r = DEBUG_KEY_cr_arrow },

#if 0 /* windows: SHIFT and PLAIN arrow keys are the same. */
        {.key1 = 0xe0,.key2 = 0x48, .r = DEBUG_KEY_su_arrow },
        {.key1 = 0xe0,.key2 = 0x50, .r = DEBUG_KEY_sd_arrow },
        {.key1 = 0xe0,.key2 = 0x4b, .r = DEBUG_KEY_sl_arrow },
        {.key1 = 0xe0,.key2 = 0x4d, .r = DEBUG_KEY_sr_arrow },
#endif

        {.key1 = 0x00,.key2 = 0x47, .r = DEBUG_KEY_home }, /* keypad */
        { .key1 = 0x00,.key2 = 0x4f,.r = DEBUG_KEY_end },
        { .key1 = 0x00,.key2 = 0x49,.r = DEBUG_KEY_pgup },
        { .key1 = 0x00,.key2 = 0x51,.r = DEBUG_KEY_pgdn },

        { .key1 = 0xe0,.key2 = 0x47,.r = DEBUG_KEY_home },  /* independent key */
        { .key1 = 0xe0,.key2 = 0x4f,.r = DEBUG_KEY_end },
        { .key1 = 0xe0,.key2 = 0x49, .r = DEBUG_KEY_pgup },
        { .key1 = 0xe0,.key2 = 0x51, .r = DEBUG_KEY_pgdn },

#if 0
        /* These overlap with other keys, see examples: */
        {.key1 = 0xe0,.key2 = 0x77, .r = DEBUG_KEY_chome },
        {.key1 = 0xe0,.key2 = 0x75, .r = DEBUG_KEY_cend },
        {.key1 = 0xe0,.key2 = 0x86, .r = DEBUG_KEY_cpgup },
        /* same as control page up so we disable this group */
        {.key1 = 0xe0,.key2 = 0x76, .r = DEBUG_KEY_cpgdn },
#endif

        {.key1 = 0x00,.key2 = 0x3b, .r = DEBUG_KEY_f1  },
        {.key1 = 0x00,.key2 = 0x3c, .r = DEBUG_KEY_f2 },
        {.key1 = 0x00,.key2 = 0x3d, .r = DEBUG_KEY_f3 },
        {.key1 = 0x00,.key2 = 0x3e, .r = DEBUG_KEY_f4 },
        {.key1 = 0x00,.key2 = 0x3f, .r = DEBUG_KEY_f5 },
        {.key1 = 0x00,.key2 = 0x40, .r = DEBUG_KEY_f6 },
        {.key1 = 0x00,.key2 = 0x41, .r = DEBUG_KEY_f7 },
        {.key1 = 0x00,.key2 = 0x42, .r = DEBUG_KEY_f8 },
        {.key1 = 0x00,.key2 = 0x43, .r = DEBUG_KEY_f9 },
        {.key1 = 0x00,.key2 = 0x44, .r = DEBUG_KEY_f10 },
        {.key1 = 0xe0,.key2 = 0x85, .r = DEBUG_KEY_f11 },
        {.key1 = 0xe0,.key2 = 0x86, .r = DEBUG_KEY_f12 },

        {.key1 = 0x00,.key2 = 0x54, .r = DEBUG_KEY_sf1 },
        {.key1 = 0x00,.key2 = 0x55, .r = DEBUG_KEY_sf2 },
        {.key1 = 0x00,.key2 = 0x56, .r = DEBUG_KEY_sf3 },
        {.key1 = 0x00,.key2 = 0x57, .r = DEBUG_KEY_sf4 },
        {.key1 = 0x00,.key2 = 0x58, .r = DEBUG_KEY_sf5 },
        {.key1 = 0x00,.key2 = 0x59, .r = DEBUG_KEY_sf6 },
        {.key1 = 0x00,.key2 = 0x5a, .r = DEBUG_KEY_sf7 },
        {.key1 = 0x00,.key2 = 0x5b, .r = DEBUG_KEY_sf8 },
        {.key1 = 0x00,.key2 = 0x5c, .r = DEBUG_KEY_sf9 },
        {.key1 = 0x00,.key2 = 0x5d, .r = DEBUG_KEY_sf10 },
        {.key1 = 0xe0,.key2 = 0x87, .r = DEBUG_KEY_sf11 },
        {.key1 = 0xe0,.key2 = 0x88, .r = DEBUG_KEY_sf12 },

        { .key1 = 0x00,.key2 = 0x5e,.r = DEBUG_KEY_cf1 },
        { .key1 = 0x00,.key2 = 0x5f,.r = DEBUG_KEY_cf2 },
        { .key1 = 0x00,.key2 = 0x60,.r = DEBUG_KEY_cf3 },
        { .key1 = 0x00,.key2 = 0x61,.r = DEBUG_KEY_cf4 },
        { .key1 = 0x00,.key2 = 0x62,.r = DEBUG_KEY_cf5 },
        { .key1 = 0x00,.key2 = 0x63,.r = DEBUG_KEY_cf6 },
        { .key1 = 0x00,.key2 = 0x64,.r = DEBUG_KEY_cf7 },
        { .key1 = 0x00,.key2 = 0x65,.r = DEBUG_KEY_cf8 },
        { .key1 = 0x00,.key2 = 0x66,.r = DEBUG_KEY_cf9 },
        { .key1 = 0x00,.key2 = 0x67,.r = DEBUG_KEY_cf10 },
        { .key1 = 0xe0,.key2 = 0x89,.r = DEBUG_KEY_cf11 },
        { .key1 = 0xe0,.key2 = 0x8a,.r = DEBUG_KEY_cf12 },

        {.key1 = 0x00,.key2 = 0x68, .r = DEBUG_KEY_af1 },
        {.key1 = 0x00,.key2 = 0x69, .r = DEBUG_KEY_af2 },
        {.key1 = 0x00,.key2 = 0x6a, .r = DEBUG_KEY_af3 },
        {.key1 = 0x00,.key2 = 0x6b, .r = DEBUG_KEY_af4 },
        {.key1 = 0x00,.key2 = 0x6c, .r = DEBUG_KEY_af5 },
        {.key1 = 0x00,.key2 = 0x6d, .r = DEBUG_KEY_af6 },
        {.key1 = 0x00,.key2 = 0x6e, .r = DEBUG_KEY_af7 },
        {.key1 = 0x00,.key2 = 0x6f, .r = DEBUG_KEY_af8 },
        {.key1 = 0x00,.key2 = 0x70, .r = DEBUG_KEY_af9 },
        {.key1 = 0x00,.key2 = 0x71, .r = DEBUG_KEY_af10 },
        {.key1 = 0xe0,.key2 = 0x8b, .r = DEBUG_KEY_af11 },
        {.key1 = 0xe0,.key2 = 0x8c, .r = DEBUG_KEY_af12 },

        { .key1 = 0xe0,.key2 = 0x53,.r = 0x7f }, /* Delete (standalone) */
        { .key1 = 0x0,.key2 = 0x53,.r = 0x7f }, /* Delete (keypad) */
        {.key1 = -1,.key2 = -1, .r = -1 },
    };

    /* go through the lookup table. */
    r = EOF;
    for(x = 0; lut[x].key1 != -1; x++)
    {
        if(lut[x].key1 == key1)
        {
            if(lut[x].key2 == key2)
            {
                r = lut[x].r;
                break;
            }
        }
    }
    /* BELOW Is very helpful if you need to add to the keys above. */
    /*printf("KEY1 = 0x%02x, KEY2 = 0x%02x, maps: 0x%04x\n", key1, key2, (unsigned)r); */

    return (r);
}

void _DEBUG_beep(void)
{
    DEBUG_printf("%c", 0x07);
}

int _DEBUG_getkey(void)
{
    int r;
    if(!_kbhit())
    {
        return (EOF);
    }

    r = _getch();
    if(r == 0)
    {
        r = _getch();
        return (decode_key2(0,r));
    }
    if(r == 0xe0)
    {
        r = _getch();
        return (decode_key2(0xe0,r));
    }
    return (r);
}

void _DEBUG_echo_off(void)
{
    /* not required, _getch() does not echo */
}

void _DEBUG_echo_on(void)
{
    /* not required, _getch() does not echo */
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
