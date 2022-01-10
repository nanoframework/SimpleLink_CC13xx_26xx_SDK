/******************************************************************************

 @file  rom.c

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

#ifdef R2
  #include "r2/rom_init.c"
  #include "r2/common_rom_init.c"
#elif AGAMA_R1
  #include "agama_r1/rom_init.c"
  #include "agama_r1/common_rom_init.c"
#else // Default to R1
  #include "r1/rom_init.c"
  #include "r1/common_rom_init.c"
#endif
