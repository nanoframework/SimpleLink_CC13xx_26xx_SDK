/******************************************************************************

 @file  hal_rtc_wrapper.h

 @brief This file contains the HAL RTC Wrapper types, constants,
        API's etc.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2013 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef HAL_RTC_WRAPPER_H
#define HAL_RTC_WRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */


/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

typedef void (*rtcIntCBack_t)( void );

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * Functions
 */

extern void   halInitRTC( void );

extern void   halSetRtcTimerEvent(uint32 rtcChan, uint32 compVal  );

extern void   halClearRtcTimerEvent( uint32 rtcChan );

extern uint32 halReadRtcTimer( void );

extern void   halSetRtcIntFlag( uint8 rtcChan );

extern uint8  halGetRtcIntFlag( uint8 rtcChan );

extern void   halClrRtcIntFlag( uint8 rtcChan );

extern uint32 halRtcTimerDelta( uint32 time1, uint32 time2 );

extern uint32 halRtcTimerCompare( uint32 time1, uint32 time2 );

extern void   halRtcRegIntCBack( uint8 rtcChan, rtcIntCBack_t rtcCback );

extern void   RTC_ISR( void );

#ifdef __cplusplus
}
#endif

#endif /* HAL_RTC_WRAPPER_H */
