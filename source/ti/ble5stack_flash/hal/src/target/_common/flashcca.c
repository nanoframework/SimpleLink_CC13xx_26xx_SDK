/******************************************************************************

 @file  flashcca.c

 @brief Startup code for CC26xx for use with IAR EWARM.
        !

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2013 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_ioc.h>
#include <inc/hw_adi_2_refsys.h>
#include <inc/hw_adi_3_refsys.h>
#include <inc/hw_factory_cfg.h>
#include "hw_osc_dig.h"
#include <inc/hw_aon_wuc.h>
#include <inc/hw_aux_wuc.h>
#include <driverlib/ddi.h>
#include <driverlib/adi.h>
#include <driverlib/ioc.h>

// We need intrinsic functions for IAR (if used in source code)
#ifdef __IAR_SYSTEMS_ICC__
#include <intrinsics.h>
#endif

//*****************************************************************************
//
//! Customer Configuration Area defines
//
//*****************************************************************************

#define CCA_MODE_CONFIG         0xFFFFFFFF
#define CCA_VOLT_LOAD_0         0xFFFFFFFF
#define CCA_VOLT_LOAD_1         0xFFFFFFFF
#define CCA_RTC_OFFSET          0xFFFFFFFF
#define CCA_FREQ_OFFSET         0xFFFFFFFF
#define CCA_IEEE_MAC_ADDR_0     0xFFFFFFFF
#define CCA_IEEE_MAC_ADDR_1     0xFFFFFFFF
#define CCA_IEEE_BLE_ADDR_0     0xFFFFFFFF
#define CCA_IEEE_BLE_ADDR_1     0xFFFFFFFF
#define CCA_BOOTLOADER_CONFIG   0xC5FFFFFF
#define CCA_ERASE_CONFIG        0xFFFFFFFF
#define CCA_BACKDOOR_CONFIG     0xFFFFFFC5
#define CCA_TAPDAP_CONFIG_0     0xFFC5C5C5
#define CCA_TAPDAP_CONFIG_1     0xFFC5C5C5
#define CCA_FLASH_IMAGE_VALID   0x00000000
#define CCA_SECTOR_PROT_0       0xFFFFFFFF
#define CCA_SECTOR_PROT_1       0xFFFFFFFF
#define CCA_SECTOR_PROT_2       0xFFFFFFFF
#define CCA_SECTOR_PROT_3       0xFFFFFFFF

//*****************************************************************************
//
// Customer Configuration Area in Lock Page
//
//*****************************************************************************
typedef struct
{
    unsigned long ulModeConfig;
    unsigned long ulVoltLoad0;
    unsigned long ulVoltLoad1;
    unsigned long ulRtcOffset;
    unsigned long ulFreqOffset;
    unsigned long ulIeeeMacAddr0;
    unsigned long ulIeeeMacAddr1;
    unsigned long ulIeeeBleAddr0;
    unsigned long ulIeeeBleAddr1;
    unsigned long ulBlConfig;
    unsigned long ulEraseConfig;
    unsigned long ulTiBackdoor;
    unsigned long ulTapDap0;
    unsigned long ulTapDap1;
    unsigned long ulImageValid;
    unsigned long ulSectorProt0;
    unsigned long ulSectorProt1;
    unsigned long ulSectorProt2;
    unsigned long ulSectorProt3;} lockPageCCA_t;

#ifdef __IAR_SYSTEMS_ICC__
__root const lockPageCCA_t __cca @ ".flashcca" =
#elif __TI_COMPILER_VERSION__
#pragma DATA_SECTION(__ccfg, ".ccfg")
#pragma RETAIN(__ccfg)
const lockPageCCA_t __ccfg =
#else
const lockPageCCA_t __cca __attribute__((section("flashcca")))=
#endif
{
    CCA_MODE_CONFIG,
    CCA_VOLT_LOAD_0,
    CCA_VOLT_LOAD_1,
    CCA_RTC_OFFSET,
    CCA_FREQ_OFFSET,
    CCA_IEEE_MAC_ADDR_0,
    CCA_IEEE_MAC_ADDR_1,
    CCA_IEEE_BLE_ADDR_0,
    CCA_IEEE_BLE_ADDR_1,
    CCA_BOOTLOADER_CONFIG,
    CCA_ERASE_CONFIG,
    CCA_BACKDOOR_CONFIG,
    CCA_TAPDAP_CONFIG_0,
    CCA_TAPDAP_CONFIG_1,
    CCA_FLASH_IMAGE_VALID,
    CCA_SECTOR_PROT_0,
    CCA_SECTOR_PROT_1,
    CCA_SECTOR_PROT_2,
    CCA_SECTOR_PROT_3,
};
