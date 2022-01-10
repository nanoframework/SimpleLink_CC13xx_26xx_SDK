/******************************************************************************

 @file  rom.h

 @brief This file contains redirection to ROM initialization and jump tables
        for each specific ROM image

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: TISD 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef ROM_H
#define ROM_H

/*******************************************************************************
 * INCLUDES
 */

#if defined( ROM_BUILD ) || defined ( COMMON_ROM_BUILD )
// If ROM_BUILD or COMMON_ROM_BUILD are defined then all MAP_ prefixed
// functions must be indirectly to any function address through a jump
// table. These indirect mappings are dependent on which ROM image is
// being built and are found in the following headers:
#ifdef R2
  #include "r2/rom_flash_jt.h"
#elif AGAMA_R1
  #include "agama_r1/rom_flash_jt.h"
#else // Default to R1
  #include "r1/r2r_flash_jt.h"
  #include "r1/r2f_flash_jt.h"
  #include "r1/r2r_common_flash_jt.h"
#endif

#else
// If this is not a ROM_BUILD then all MAP_ prefixed functions can be
// directly mapped and linked to the given function's address instead
// of a jump table entry. These mappings are not dependent on the ROM
// image and can be found in the following header:
#include "map_direct.h"
#endif // ROM_BUILD || COMMON_ROM_BUILD

/*******************************************************************************
 * EXTERNS
 */

// ROM's C Runtime initialization
extern void ROM_Init( void );
extern void FPB_Init( void );
extern void CommonROM_Init( void );

#endif // ROM_H
