/******************************************************************************

 @file  ll_wl.h

 @brief This file contains the data structures and APIs for handling
        Bluetooth Low Energy White List structures using the CC26xx
        RF Core Firmware Specification.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: TI_TEXT 2009 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR.
 *
 */

#ifndef WL_H
#define WL_H

/*******************************************************************************
 * INCLUDES
 */

#include "rf_hal.h"
#include "bcomdef.h"
#include "ll.h"
#include "ll_config.h"

/*******************************************************************************
 * MACROS
 */

// Advertising White List
// Note: Assumes wlEntryFlags = white list entry's flags.

#define CLR_WL_ENTRY( wlEntryFlags )                                           \
  (wlEntryFlags) = 0;

#define SET_WL_ENTRY_FREE( wlEntryFlags )                                      \
  (wlEntryFlags) &= ~BV(0)

#define SET_WL_ENTRY_BUSY( wlEntryFlags )                                      \
  (wlEntryFlags) |= BV(0)

#define IS_WL_ENTRY_FREE( wlEntryFlags )                                       \
  ((wlEntryFlags) & BV(0)) == 0

#define IS_WL_ENTRY_BUSY( wlEntryFlags )                                       \
  ((wlEntryFlags) & BV(0)) == 1

#define SET_WL_ENTRY_PUBLIC( wlEntryFlags )                                    \
  (wlEntryFlags) &= ~BV(1)

#define SET_WL_ENTRY_RANDOM( wlEntryFlags )                                    \
  (wlEntryFlags) |= BV(1)

#define GET_WL_ENTRY_ADDR_TYPE( wlEntryFlags )                                 \
  (((wlEntryFlags) & BV(1)) >> 1)

#define CLR_WL_ENTRY_IGNORE( wlEntryFlags )                                    \
  (wlEntryFlags) &= ~BV(2)

#define SET_WL_ENTRY_IGNORE( wlEntryFlags )                                    \
  (wlEntryFlags) |= BV(2)

#define SET_WL_ENTRY_PRIV_IGNORE( wlEntryFlags )                               \
  (wlEntryFlags) |= BV(4)

#define CLR_WL_ENTRY_PRIV_IGNORE( wlEntryFlags )                               \
  (wlEntryFlags) &= ~BV(4)

#define GET_WL_TABLE_POINTER( pWlEntry )                                       \
    ((wlTable_t *)((uint8 *)(pWlEntry) - sizeof(wlTable_t)))


/*******************************************************************************
 * CONSTANTS
 */

// API
#define BLE_IGNORE_WL_ENTRY            0
#define BLE_USE_WL_ENTRY               1

// Miscellaneous
#define BLE_BDADDR_SIZE                6
#define BLE_MAX_NUM_WL_ENTRIES         (wlSize)  // at 8 bytes per WL entry
#define BLE_NO_WL_MATCH_FOUND          0xFF

// White List Size
#define BLE_NUM_WL_ENTRIES             ((BLE_MAX_NUM_WL_ENTRIES) + (2 * (BLE_RESOLVING_LIST_SIZE)) + 1)

/*******************************************************************************
 * TYPEDEFS
 */

// BLE White List Flags
// | 7..5 |        4       |  3  |        2         |      1       |      0       |
// |  N/A | Privacy Ignore | N/A | WL Entry Ignored | Address Type | Entry In Use |
//
typedef uint8 wlFlgs_t;

// White List Entry
// Note: The layout of this structure can not be changed.
PACKED_TYPEDEF_STRUCT
{
  uint8    numEntries;                 // W:  number of white list entries
  wlFlgs_t wlFlags;                    // W:  white list flags (RW for bit 2)
  uint8    devAddr[BLE_BDADDR_SIZE];   // W:  BLE address
} wlEntry_t;

// White List Entry Table
// Note: The layout of this structure can be changed as long as pWlEntries is
//       aligned on a word boundary.
PACKED_TYPEDEF_STRUCT
{
  uint8     numWlEntries;
  uint8     numBusyWlEntries;
  uint16    reserve;
  wlEntry_t *pWlEntries;
} wlTable_t;

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

extern wlTable_t *wlTable;
extern wlTable_t *wlTableScan;

/*******************************************************************************
 * GLOBAL ROUTINES
 */

extern void       WL_Init( wlTable_t * );

extern llStatus_t WL_Clear( wlTable_t * );

extern void       WL_ClearEntry( wlEntry_t * );

extern uint8      WL_GetSize( wlTable_t * );

extern uint8      WL_GetNumFreeEntries(  wlTable_t * );

extern uint8      WL_FindEntry( wlTable_t *, uint8 *, uint8 );

extern llStatus_t WL_AddEntry( wlTable_t *, uint8 *, uint8, uint8 );

extern llStatus_t WL_RemoveEntry( wlTable_t *, uint8 *, uint8 );

extern llStatus_t WL_SetWlIgnore( wlTable_t *, uint8 *, uint8 );

extern llStatus_t WL_ClearIgnoreList( wlTable_t * );

extern wlEntry_t *WL_Alloc( uint8 );

extern void       WL_Free( wlEntry_t * );

extern wlEntry_t *WL_Copy( wlEntry_t *, wlEntry_t * );

/*******************************************************************************
 */

#endif /* WL_H */
