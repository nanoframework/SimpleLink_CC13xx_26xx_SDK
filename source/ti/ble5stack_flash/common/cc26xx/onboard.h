/******************************************************************************

 @file  onboard.h

 @brief Defines constants and prototypes for Evaluation boards
        This file targets the Texas Instruments CC26xx Device Family.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2006 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef ONBOARD_H
#define ONBOARD_H

#include <driverlib/sys_ctrl.h>
#include "hal_mcu.h"
#include "hal_sleep.h"
#include "osal.h"

/*********************************************************************
 */
// Internal (MCU) RAM addresses
//#define MCU_RAM_BEG 0x1100
//#define MCU_RAM_END 0x20FF
#define MCU_RAM_BEG 0x20000000
#define MCU_RAM_END 0x20004000

#define MCU_RAM_LEN (MCU_RAM_END - MCU_RAM_BEG + 1)

// Internal (MCU) heap size
#if !defined( INT_HEAP_LEN )
  #define INT_HEAP_LEN  4096 //1024  // 1.00K
#endif

// Memory Allocation Heap
#define MAXMEMHEAP INT_HEAP_LEN  // Typically, 0.70-1.50K

/* OSAL timer defines */
#define TICK_TIME   1000   // Timer per tick - in micro-sec
// Timer clock and power-saving definitions
#define TICK_COUNT         1  // TIMAC requires this number to be 1

#ifndef _WIN32
extern void _itoa(uint16 num, uint8 *buf, uint8 radix);
#endif

/* Tx and Rx buffer size defines used by SPIMgr.c */
#define MT_UART_THRESHOLD    5
#define MT_UART_TX_BUFF_MAX  170
#define MT_UART_RX_BUFF_MAX  120
#define MT_UART_IDLE_TIMEOUT 5

/* system restart and boot loader used from MTEL.c */
#define SystemReset()        SysCtrlSystemReset();
#define SystemResetSoft()    Onboard_soft_reset();
#define BootLoader()         // Not yet implemented for MSP430

/* Reset reason for reset indication */
#define ResetReason() (0)

/* port definition stuff used by MT */
#if defined (ZAPP_P1)
  #define ZAPP_PORT HAL_UART_PORT_0 //SERIAL_PORT1
#elif defined (ZAPP_P2)
  #define ZAPP_PORT HAL_UART_PORT_1 //SERIAL_PORT2
#else
  #undef ZAPP_PORT
#endif
#if defined (ZTOOL_P1)
  #define ZTOOL_PORT HAL_UART_PORT_0 //SERIAL_PORT1
#elif defined (ZTOOL_P2)
  #define ZTOOL_PORT HAL_UART_PORT_1 //SERIAL_PORT2
#else
  #undef ZTOOL_PORT
#endif

// Power conservation
#define OSAL_SET_CPU_INTO_SLEEP(timeout) halSleep(timeout);  /* Called from OSAL_PwrMgr */

typedef struct
{
  osal_event_hdr_t hdr;
  uint8             state; // shift
  uint8             keys;  // keys
} keyChange_t;

/*
 * Used to Initialize HAL setting in *_Main
 */
void InitBoard( void );

/* used by MTEL.c */
uint8 OnBoard_SendKeys( uint8 keys, uint8 state );

/*
 * Board specific random number generator
 */
extern uint16 Onboard_rand( void );

/*
 * Get elapsed timer clock counts
 */
extern uint32 TimerElapsed( void );

/*
 * Perform a soft reset - jump to 0x0
 */
extern void Onboard_soft_reset( void );

/*
 * Register for all key events
 */
extern uint8 RegisterForKeys( uint8 task_id );

/*
 * Callback routine to handle keys
 */
extern void OnBoard_KeyCallback ( uint8 keys, uint8 state );

/*********************************************************************
 */

#endif
