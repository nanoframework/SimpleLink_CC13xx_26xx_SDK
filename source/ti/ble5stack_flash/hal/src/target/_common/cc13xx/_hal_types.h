/******************************************************************************

 @file  _hal_types.h

 @brief Describe the purpose and contents of the file.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2006 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef _HAL_TYPES_H
#define _HAL_TYPES_H

#include <stdint.h>

/* ------------------------------------------------------------------------------------------------
 *                                               Types
 * ------------------------------------------------------------------------------------------------
 */
typedef signed   char   int8;
typedef unsigned char   uint8;

typedef signed   short  int16;
typedef unsigned short  uint16;

typedef signed   long   int32;
typedef unsigned long   uint32;

//typedef unsigned char   bool;

typedef uint32          halDataAlign_t;
#define bool            _Bool

/* ------------------------------------------------------------------------------------------------
 *                                        Compiler Macros
 * ------------------------------------------------------------------------------------------------
 */
/* ----------- IAR Compiler ----------- */
#ifdef __IAR_SYSTEMS_ICC__
#define ASM_NOP    asm("NOP")

/* ----------- KEIL Compiler ----------- */
#elif defined __KEIL__
#define ASM_NOP   __nop()

/* ----------- CCS Compiler ----------- */
#elif defined __TI_COMPILER_VERSION || defined __TI_COMPILER_VERSION__
#define ASM_NOP    asm(" NOP")

/* ----------- GNU Compiler ----------- */
#elif defined __GNUC__
#define ASM_NOP __asm__ __volatile__ ("nop")

/* ---------- MSVC compiler ---------- */
#elif _MSC_VER
#define ASM_NOP __asm NOP

/* ----------- Unrecognized Compiler ----------- */
#else
#error "ERROR: Unknown compiler."
#endif


/* ------------------------------------------------------------------------------------------------
 *                                        Standard Defines
 * ------------------------------------------------------------------------------------------------
 */
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0L
#endif


/* ------------------------------------------------------------------------------------------------
 *                                       Memory Attributes
 * ------------------------------------------------------------------------------------------------
 */

#if defined (__IAR_SYSTEMS_ICC__)
#define XDATA
#define CODE
#define DATA_ALIGN(x)               _Pragma data_alignment=(x)
#define PACKED                      __packed
#define PACKED_STRUCT               PACKED struct
#define PACKED_TYPEDEF_STRUCT       PACKED typedef struct
#define PACKED_TYPEDEF_CONST_STRUCT PACKED typedef const struct
#define PACKED_TYPEDEF_UNION        PACKED typedef union

#elif defined __TI_COMPILER_VERSION || defined __TI_COMPILER_VERSION__
#define XDATA
#define CODE
#define DATA
#define NEARFUNC
#define PACKED                      __attribute__((__packed__))
#define PACKED_STRUCT               struct PACKED
#define PACKED_TYPEDEF_STRUCT       typedef struct PACKED
#define PACKED_TYPEDEF_CONST_STRUCT typedef const struct PACKED
#define PACKED_TYPEDEF_UNION        typedef union PACKED

#elif defined (__GNUC__)
#define PACKED __attribute__((__packed__))
#endif

/**************************************************************************************************
 */
#endif
