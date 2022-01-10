/******************************************************************************

 @file  npi_util.c

 @brief NPI Utilities

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include "inc/npi_util.h"

// -----------------------------------------------------------------------------
// defines
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// typedefs
// -----------------------------------------------------------------------------

// RTOS queue for profile/app messages.
typedef struct _queueRec_
{
  Queue_Elem _elem;          // queue element
  uint8_t *pData;            // pointer to app data
} queueRec_t;

/// -----------------------------------------------------------------------------
// globals
/// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PUBLIC FUNCTIONS
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//! \brief    Memory allocation for NPI
//!
//! \param    size - number of bytes to be allocated.
//!
//! \return   pointer to the allocated memory.
// -----------------------------------------------------------------------------
uint8_t *NPIUtil_malloc(uint16_t size)
{
    uint8_t *pMsg;
#ifdef USE_ICALL
    pMsg = ICall_malloc(size);
#else
    volatile uint32_t keyHwi;
    keyHwi = Hwi_disable();
    pMsg = malloc(size);
    Hwi_restore(keyHwi);
#endif
    return pMsg;
}

// -----------------------------------------------------------------------------
//! \brief    Memory free for NPI
//!
//! \param    pMsg - pointer to the message to be freed.
//!
//! \return   None
// -----------------------------------------------------------------------------
void NPIUtil_free(uint8_t *pMsg)
{
#ifdef USE_ICALL
    ICall_free(pMsg);
#else
    volatile uint32_t keyHwi;
    keyHwi = Hwi_disable();
    free(pMsg);
    Hwi_restore(keyHwi);
#endif
}

// -----------------------------------------------------------------------------
//! \brief      Critical section entrance. Disables Tasks and HWI
//!
//! \return     _npiCSKey_t   CS Key used to later exit CS
// -----------------------------------------------------------------------------
_npiCSKey_t NPIUtil_EnterCS(void)
{
    _npiCSKey_t key;
    key.taskkey = (uint_least16_t) Task_disable();
    key.hwikey = (uint_least16_t) Hwi_disable();
    return key;
}

// -----------------------------------------------------------------------------
//! \brief      Critical section exit. Enables Tasks and HWI
//!
//! \param    key   key obtained with corresponding call to EnterCS()
//!
//! \return   void
// -----------------------------------------------------------------------------
void NPIUtil_ExitCS(_npiCSKey_t key)
{
    Hwi_restore((UInt) key.hwikey);
    Task_restore((UInt) key.taskkey);
}


// -----------------------------------------------------------------------------
//! \brief   Initialize an RTOS queue to hold messages to be processed.
//!
//! \param   pQueue - pointer to queue instance structure.
//!
//! \return  A queue handle.
// -----------------------------------------------------------------------------
Queue_Handle NPIUtil_constructQueue(Queue_Struct *pQueue)
{
  // Construct a Queue instance.
  Queue_construct(pQueue, NULL);

  return Queue_handle(pQueue);
}

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
uint8_t NPIUtil_enqueueMsg(Queue_Handle msgQueue,
#ifdef ICALL_EVENTS
                           Event_Handle event,
                           uint32_t eventFlags,
#else //!ICALL_EVENTS
                           Semaphore_Handle sem,
#endif //ICALL_EVENTS
                           uint8_t *pMsg)
{
  queueRec_t *pRec;

  // Allocated space for queue node.
  pRec = (queueRec_t *)NPIUtil_malloc(sizeof(queueRec_t));
  if (pRec)
  {
    pRec->pData = pMsg;

    Queue_put(msgQueue, &pRec->_elem);

    // Wake up the application thread event handler.
#ifdef ICALL_EVENTS
    if (event)
    {
      Event_post(event, eventFlags);
    }
#else //!ICALL_EVENTS
    if (sem)
    {
      Semaphore_post(sem);
    }
#endif //ICALL_EVENTS

    return TRUE;
  }

  // Free the message.
  NPIUtil_free((uint8_t *)pMsg);

  return FALSE;
}

// -----------------------------------------------------------------------------
//! \brief   Dequeues the message from the RTOS queue.
//!
//! \param   msgQueue - queue handle.
//!
//! \return  pointer to dequeued message, NULL otherwise.
// -----------------------------------------------------------------------------
uint8_t *NPIUtil_dequeueMsg(Queue_Handle msgQueue)
{
  queueRec_t *pRec = Queue_get(msgQueue);
  if (pRec != (queueRec_t *)msgQueue)
  {
    // Queue not empty
    uint8_t *pData = pRec->pData;

    // Free the queue node
    // Note:  this does not free space allocated by data within the node.
    NPIUtil_free((uint8_t *)pRec);

    return pData;
  }

  return NULL;
}
