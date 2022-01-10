/******************************************************************************

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2017 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/**
 *  @addtogroup Micro_BLE_Stack
 *  @{
 *  @defgroup PORT RTOS Interface
 *  @{
 *  @file   port.h
 *  @brief  Micro BLE RTOS Interface API
 *
 *  This file contains interfacing between Micro BLE Stack and the RTOS.
 */

#ifndef PORT_H
#define PORT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdbool.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/** @defgroup PORT_Constants RTOS Interface Constants
 * @{
 */

/* For POSIX only:
 * The message size is tailored to ubleEvtMsg_t plus length in uble.h
 */
#define MQ_DEF_MSGSIZE    16  //!< Maximum POSIX message size
#define MQ_DEF_MAXMSG     12  //!< Maximum number of messages in POSIX

/** @} End PORT_Constants */

/*********************************************************************
 * TYPEDEFS
 */

/** @defgroup Port_Timers RTOS Timer Interfaces
 * @{
 */

/** @defgroup Port_Timer_Callback Port RTOS Timer Callback
 * @{
 */

/**
 * @brief Function to process timer expiration.
 */
typedef void (*port_timerCB_t)(uint32_t param);

/** @} End Port_Timer_Callback */

/// @brief Opaque strut for timer object
struct port_timerObject_s;

/** @} End Port_Timers */

/** @defgroup Port_Queues RTOS Queue Interfaces
 * @{
 */

/// @brief Port RTOS queue element for TIRTOS
typedef struct port_queueElem_tirtos_s
{
  void *next;  //!< next pointer
  void *prev;  //!< previous pointer
} port_queueElem_tirtos_t;

/// @brief Port RTOS queue element for POSIX
typedef struct port_queueElem_posix_s
{
  void *pElem;  //!< pointer to the queue element
  uint16_t size;  //!< size of the queue element
} port_queueElem_posix_t;

/// @brief Union of port RTOS queue element for TIRTOS/POSIX
typedef union port_queueElem_s
{
  port_queueElem_tirtos_t tirtos_queueElem;  //!< TIRTOS queue element
  port_queueElem_posix_t posix_queueElem; //!< POISX queue element
} port_queueElem_t;

/// @brief Opaque strut for queue object
struct port_queueObject_s;

/** @} End Port_Queues */

/** @defgroup Port_Key RTOS Critical Section Key
 * @{
 */

/** @brief RTOS Critical Section Key type */
typedef uint32_t port_key_t;

/** @} End Port_Key */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/**
 * @brief This function creates a timer instance and return the handle of this
 *        timer instance.
 *
 * @param port_TimerCB - timer callback called upon timer expiration.
 * @param port_TimerEvent - timer callback parameter.
 *
 * @return pointer to port_timerObject_s
 */
extern struct port_timerObject_s *port_timerCreate( port_timerCB_t port_TimerCB,
                                                    uint16_t port_TimerEvent );

/**
 * @brief This function starts a timer previously created.
 *
 * @param handle - pointer to the timer object previously created.
 * @param timeout - timeout in SYSTICK previously created.
 *
 * @return none
 */
extern void port_timerStart( struct port_timerObject_s *handle,
                             uint32_t timeout );

/**
 * @brief This function stops a timer previously created.
 *
 * @param handle - pointer to the timer instance previously created.
 *
 * @return none
 */
extern void port_timerStop( struct port_timerObject_s *handle );

/**
 * @brief This function creates a queue instance and return the handle of this
 *        queue instance.
 *
 * @param portQueueName - null terminated string. Used in POSIX only.
 *        Ignored in TIRTOS.
 *
 * @return pointer to port_queueObject_s.
 */
extern struct port_queueObject_s *port_queueCreate( const char *portQueueName );

/**
 * @brief This function removes the element from the front of queue.
 *
 * @param handle - pointer to the queue instance previously created.
 * @param port_ppQueueElement - address of pointer to the retrieved queue element.
 *
 * @return - none.
 */
extern void port_queueGet( struct port_queueObject_s *handle,
                           port_queueElem_t **port_ppQueueElement );

/**
 * @brief This function puts the queue element at the front of queue.
 *
 * @param handle - pointer to the queue instance previously created.
 * @param port_queueElement - pointer to the queue element.
 * @param size - the size of this element. This is ignored in TIRTOS.
 *
 * @return none
 */
extern void port_queuePut( struct port_queueObject_s *handle,
                           port_queueElem_t *port_queueElement,
                           uint16_t size );

/**
 * @brief This function checks if the queue is empty.
 *
 * @param handle - pointer to the queue instance previously created.
 *
 * @return true if queue is empty.
 */
bool port_queueEmpty( struct port_queueObject_s *handle );

/**
 * @brief This function enters the critical section by disabling HWI.
 *
 * @return current key to be saved
 */
extern port_key_t port_enterCS_HW( void );

/**
 * @brief This function exits the critical section by restoring HWI.
 *
 * @param key - restore the current status.
 *
 * @return - None.
 */
extern void port_exitCS_HW( port_key_t key );

/**
 * @brief This function enters the critical section by disabling SWI.
 *        Note that this function will disable master interrupt in POSIX.
 *
 * @return current key to be saved
 */
extern port_key_t port_enterCS_SW( void );

/**
 * @brief This function exits the critical section by restoring SWI.
 *        Note that this function will restore master interrupt in POSIX.
 *
 * @param key - restore the current status.
 *
 * @return - None.
 */
extern void port_exitCS_SW( port_key_t key );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* PORT_H */

/** @} End PORT */

/** @} End Rtos_Port */
