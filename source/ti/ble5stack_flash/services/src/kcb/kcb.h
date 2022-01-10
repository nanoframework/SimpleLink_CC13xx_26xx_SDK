/******************************************************************************

 @file  kcb.h

 @brief This file contains the Key Callback module definitions

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2012 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef KCB_H
#define KCB_H

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"

/******************************************************************************
 * CONSTANTS
 */

// number of keys supported
#define KCB_ONE_KEYS_SUPPORTED     1

// Generic RC Key IDs
typedef enum
{
  RC_SW_PWR       = 0x00,
  RC_SW_TV        = 0x01,
  RC_SW_STB       = 0x02,
  RC_SW_VOL_UP    = 0x03,
  RC_SW_MUTE      = 0x04,
  RC_SW_VOL_DN    = 0x05,
  RC_SW_MENU      = 0x06,
  RC_SW_HOME      = 0x07,
  RC_SW_BACK      = 0x08,
  RC_SW_ENTER     = 0x09,
  RC_SW_VOICE     = 0x0A,
  RC_SW_PAIR      = 0x0B,
  RC_SW_INFINITY  = 0x0C,
  RC_SW_UP_ARW    = 0x0D,
  RC_SW_LF_ARW    = 0x0E,
  RC_SW_RT_ARW    = 0x0F,
  RC_SW_DN_ARW    = 0x10,
  RC_SW_DIG_0     = 0x11,
  RC_SW_DIG_1     = 0x12,
  RC_SW_DIG_2     = 0x13,
  RC_SW_DIG_3     = 0x14,
  RC_SW_DIG_4     = 0x15,
  RC_SW_DIG_5     = 0x16,
  RC_SW_DIG_6     = 0x17,
  RC_SW_DIG_7     = 0x18,
  RC_SW_DIG_8     = 0x19,
  RC_SW_DIG_9     = 0x1A,
  RC_SW_AV        = 0x1B,
  RC_SW_RECORD    = 0x1C,
  RC_SW_PREV      = 0x1D,
  RC_SW_NEXT      = 0x1E,
  RC_SW_PLAY      = 0x1F,
  RC_SW_NA        = 0xFF
} rcKeyId_t;

/******************************************************************************
 * MACROS
 */

/******************************************************************************
 * TYPEDEFS
 */
typedef enum
{
  RC_KEY_STATE_NOT_PRESSED,
  RC_KEY_STATE_PRESSED,
  RC_KEY_STATE_PRESSED_NOT_YET_REPEATED,
  RC_KEY_STATE_REPEATED
} rcKeyState_t;

typedef void (*KcbEvtCBack_t) (rcKeyId_t harKeyId,
                               rcKeyState_t harCurrentKeyState);

/******************************************************************************
 * GLOBALS
 */

/******************************************************************************
 * FUNCTIONS
 */

/*
 * Initialize the Key Callback module
 */
void kcb_Init(KcbEvtCBack_t processKeyCB, uint8_t numKeys);

/*
 * Processes a list of keys that are pressed
 */
void kcb_ProcessKey(uint8 keyId);

/******************************************************************************
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* KCB_H */
