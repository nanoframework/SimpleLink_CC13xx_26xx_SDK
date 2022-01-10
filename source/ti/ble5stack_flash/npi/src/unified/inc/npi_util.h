/******************************************************************************

 @file  npi_util.h

 @brief NPI Utilities

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
#ifndef NPI_UTIL_H
#define NPI_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

// ****************************************************************************
// includes
// ****************************************************************************

#ifdef USE_ICALL
#include "icall.h"
#else
#include <stdlib.h>
#include "hal_assert.h"
#endif //USE_ICALL

#include <ti/sysbios/knl/Queue.h>
#ifdef ICALL_EVENTS
#include <ti/sysbios/knl/Event.h>
#else //!ICALL_EVENTS
#include <ti/sysbios/knl/Semaphore.h>
#endif //ICALL_EVENTS

// ****************************************************************************
// defines
// ****************************************************************************

// ****************************************************************************
// typedefs
// ****************************************************************************

//! \brief Keys used to enter and exit critical sections
typedef struct _npiCSKey_t
{
    uint_least16_t hwikey;
    uint_least16_t taskkey;
} _npiCSKey_t;

//*****************************************************************************
// globals
//*****************************************************************************

//*****************************************************************************
// function prototypes
//*****************************************************************************

// -----------------------------------------------------------------------------
//! \brief    Memory allocation for NPI
//!
//! \param    size - number of bytes to be allocated.
//!
//! \return   pointer to the allocated memeory.
// -----------------------------------------------------------------------------
extern uint8_t *NPIUtil_malloc(uint16_t size);

// -----------------------------------------------------------------------------
//! \brief    Memory free for NPI
//!
//! \param    pMsg - pointer to the message to be freed.
//!
//! \return   None
// -----------------------------------------------------------------------------
extern void NPIUtil_free(uint8_t *pMsg);

// -----------------------------------------------------------------------------
//! \brief      Critical section entrance. Disables Tasks and HWI
//!
//! \return     _npiCSKey_t 	CS Key used to later exit CS
// -----------------------------------------------------------------------------
extern _npiCSKey_t NPIUtil_EnterCS(void);

// -----------------------------------------------------------------------------
//! \brief      Critical section exit. Enables Tasks and HWI
//!
//! \param		key 	key obtained with corresponding call to EnterCS()
//!
//! \return     void
// -----------------------------------------------------------------------------
extern void NPIUtil_ExitCS(_npiCSKey_t key);

// -----------------------------------------------------------------------------
//! \brief   Initialize an RTOS queue to hold messages to be processed.
//!
//! \param   pQueue - pointer to queue instance structure.
//!
//! \return  A queue handle.
// -----------------------------------------------------------------------------
extern Queue_Handle NPIUtil_constructQueue(Queue_Struct *pQueue);

// -----------------------------------------------------------------------------
//! \brief   Creates a queue node and puts the node in RTOS queue.
//!
//! \param   msgQueue - queue handle.
//! \param   event - thread's event processing synchronization object that
//!                  queue is associated with.
//! \param   eventFlag - events to signal with synchronization object associated
//!                      with this pMsg.
//! \param   sem - thread's event processing semaphore that queue is
//!                associated with.
//! \param   pMsg - pointer to message to be queued
//!
//! \return  TRUE if message was queued, FALSE otherwise.
// -----------------------------------------------------------------------------
extern uint8_t NPIUtil_enqueueMsg(Queue_Handle msgQueue,
#ifdef ICALL_EVENTS
                                  Event_Handle event,
                                  uint32_t eventFlags,
#else //!ICALL_EVENTS
                                  Semaphore_Handle sem,
#endif //ICALL_EVENTS
                                  uint8_t *pMsg);

// -----------------------------------------------------------------------------
//! \brief   Dequeues the message from the RTOS queue.
//!
//! \param   msgQueue - queue handle.
//!
//! \return  pointer to dequeued message, NULL otherwise.
// -----------------------------------------------------------------------------
extern uint8_t * NPIUtil_dequeueMsg(Queue_Handle msgQueue);

#ifdef __cplusplus
}
#endif

#endif /* NPI_UTIL_H */
