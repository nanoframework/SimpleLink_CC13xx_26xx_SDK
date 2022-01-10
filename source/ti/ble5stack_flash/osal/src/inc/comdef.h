/******************************************************************************

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2004 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/**
 *  @file       comdef.h
 *  @brief      Common Defines
 */

#ifndef COMDEF_H
#define COMDEF_H

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * INCLUDES
 */

/* HAL */
#include "hal_types.h"
#include "hal_defs.h"

/// @cond NODOC

/*********************************************************************
 * Lint Keywords
 */
#define VOID (void)

#define NULL_OK
#define INP
#define OUTP
#define UNUSED
#define ONLY
#define READONLY
#define SHARED
#define KEEP
#define RELAX

/*********************************************************************
 * CONSTANTS
 */

#ifndef false
  #define false 0
#endif

#ifndef true
  #define true 1
#endif

#ifndef CONST
  #define CONST const
#endif

#ifndef GENERIC
  #define GENERIC
#endif

/// @endcond // NODOC

/*** Generic Status Return Values ***/
#define SUCCESS                   0x00 //!< SUCCESS
#define FAILURE                   0x01 //!< Failure
#define INVALIDPARAMETER          0x02 //!< Invalid Parameter
#define INVALID_TASK              0x03 //!< Invalid Task
#define MSG_BUFFER_NOT_AVAIL      0x04 //!< No HCI Buffer is Available
#define INVALID_MSG_POINTER       0x05 //!< Invalid Message Pointer
#define INVALID_EVENT_ID          0x06 //!< Invalid Event ID
#define INVALID_INTERRUPT_ID      0x07 //!< Invalid Interupt ID
#define NO_TIMER_AVAIL            0x08 //!< No Timer Available
#define NV_ITEM_UNINIT            0x09 //!< NV Item Uninitialized
#define NV_OPER_FAILED            0x0A //!< NV Operation Failed
#define INVALID_MEM_SIZE          0x0B //!< Invalid Memory Size
#define NV_BAD_ITEM_LEN           0x0C //!< NV Bad Item Length

/*********************************************************************
 * TYPEDEFS
 */

/// @cond NODOC

// Generic Status return
typedef uint8 Status_t;

// Data types
typedef int32   int24;
typedef uint32  uint24;

/// @endcond // NODOC

/*********************************************************************
 * Global System Events
 */

#define SYS_EVENT_MSG               0x8000  //!< A message is waiting event

/*********************************************************************
 * Global Generic System Messages
 */

#define KEY_CHANGE                0xC0    //!< Key Events

// OSAL System Message IDs/Events Reserved for applications (user applications)
// 0xE0 - 0xFC

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* COMDEF_H */
