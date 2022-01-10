/******************************************************************************

 @file  oad_image_header.c

 @brief OAD image header definition file.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2014 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/*******************************************************************************
 * INCLUDES
 */

#include <stddef.h>
#include <common/cc26xx/oad/oad_image_header.h>

/*******************************************************************************
 * EXTERNS
 */

/*******************************************************************************
 * PROTOTYPES
 */

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

#define VECTOR_TB_SIZE       0x40 //!< Interrupt vector table entry size */
#ifndef STACK_LIBRARY
  #define BOUNDARY_SEG_LEN   0x18 //!< Length of the boundary segment */
#endif

#define SOFTWARE_VER            {'0', '0', '0', '1'}

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */
/*
 * NV Page Setting:
 * This define is used ensure the stack is built with a compatible NV setting
 * Note: this restriction does not apply to the stack library configuration
 * for off-chip OAD
 */

extern const uint32_t  RAM_END;

#if defined HAL_IMAGE_A
extern const uint8_t  ENTRY_END;
extern const uint8_t  ENTRY_START;
#endif

#ifdef __IAR_SYSTEMS_ICC__
#pragma section = "ROSTK"
#pragma section = "RWDATA"
#pragma section = "ENTRY_FLASH"
#pragma section = ".intvec"
#endif

#ifndef __IAR_SYSTEMS_ICC__
/* This symbol is create by the linker file */
extern uint8_t ramStartHere;
extern uint8_t prgEntryAddr;
extern uint8_t ramStartHere;
extern uint8_t flashEndAddr;
extern uint32_t heapEnd;
extern uint32_t FLASH_END;
#endif /* ! __IAR_SYSTEMS_ICC__ */

#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION(_imgHdr, ".image_header")
#pragma RETAIN(_imgHdr)
const imgHdr_t _imgHdr =
{
  {
    .imgID = OAD_IMG_ID_VAL,
    .crc32 = DEFAULT_CRC,
    .bimVer = BIM_VER,
    .metaVer = META_VER,                   //!< Metadata version */
    .techType = OAD_WIRELESS_TECH_BLE,     //!< Wireless protocol type BLE/TI-MAC/ZIGBEE etc. */
    .imgCpStat = DEFAULT_STATE,            //!< Image copy status bytes */
    .crcStat = DEFAULT_STATE,              //!< CRC status */
    .imgNo = 0x1,                          //!< Image number of 'image type' */
    .imgVld = 0xFFFFFFFF,                  //!< In indicates if the current image in valid 0xff - valid, 0x00 invalid image */
    .len = INVALID_LEN,                     //!< Image length in bytes. */
    .softVer = SOFTWARE_VER,               //!< Software version of the image */
    .hdrLen = offsetof(imgHdr_t, fixedHdr.rfu) + sizeof(((imgHdr_t){0}).fixedHdr.rfu),   //!< Total length of the image header */
    .rfu = 0xFFFF,                         //!< reserved bytes */
    .prgEntry = (uint32_t)&prgEntryAddr,
    .imgEndAddr = (uint32_t)&flashEndAddr,
#if (!defined(STACK_LIBRARY) && (defined(SPLIT_APP_STACK_IMAGE)))
    .imgType = OAD_IMG_TYPE_APP,
#else
  #if defined HAL_IMAGE_A
    .imgType =  OAD_IMG_TYPE_PERSISTENT_APP,
  #else
    .imgType = OAD_IMG_TYPE_APPSTACKLIB,
  #endif
#endif
  },

#if (defined(SECURITY))
  {
    .segTypeSecure = IMG_SECURITY_SEG_ID,
    .wirelessTech = OAD_WIRELESS_TECH_BLE,
    .verifStat = DEFAULT_STATE,
    .secSegLen = 0x55,
    .secVer = SECURITY_VER,                     //!< Image payload and length */
    .secTimestamp = 0x0,                         //!< Security timestamp */
    .secSignerInfo = 0x0,
  },
#endif

#if (!defined(STACK_LIBRARY) && (defined(SPLIT_APP_STACK_IMAGE)))
  .segTypeBd = IMG_BOUNDARY_SEG_ID,
  .wirelessTech1 = OAD_WIRELESS_TECH_BLE,
  .rfu = DEFAULT_STATE,
  .boundarySegLen = BOUNDARY_SEG_LEN,
  .ram0StartAddr = (uint32_t)&ramStartHere,  //!< RAM entry start address */

  #if defined HAL_IMAGE_A                    //! Persistent image */
    .imgType =  OAD_IMG_TYPE_PERSISTENT_APP, //!< Persistent image Type */
    .stackStartAddr = INVALID_ADDR,          //!< Stack start address */
    .stackEntryAddr = INVALID_ADDR,
  #else /* User application image */
    .imgType =  OAD_IMG_TYPE_APP,            //!< Application image Type */
    .stackEntryAddr = ICALL_STACK0_ADDR,
    .stackStartAddr = ICALL_STACK0_START,
  #endif /* defined HAL_IMAGE_A */
    .imgType = OAD_IMG_TYPE_APP,
#endif /* STACK_LIBRARY */

  // Image payload segment initialization
   {
     .segTypeImg = IMG_PAYLOAD_SEG_ID,
     .wirelessTech = OAD_WIRELESS_TECH_BLE,
     .rfu = DEFAULT_STATE,
     .startAddr = (uint32_t)&(_imgHdr.fixedHdr.imgID),
   }
 };
#elif  defined(__IAR_SYSTEMS_ICC__)
#pragma location=".img_hdr"
const imgHdr_t _imgHdr @ ".img_hdr" =
{
  {
    .imgID = OAD_IMG_ID_VAL,
    .crc32 = DEFAULT_CRC,
    .bimVer = BIM_VER,
    .metaVer = META_VER,                   //!< Metadata version */
    .techType = OAD_WIRELESS_TECH_BLE,     //!< Wireless protocol type BLE/TI-MAC/ZIGBEE etc. */
    .imgCpStat = DEFAULT_STATE,            //!< Image copy status bytes */
    .crcStat = DEFAULT_STATE,              //!< CRC status */
    .imgNo = 0x1,                          //!< Image number of 'image type' */
    .imgVld = 0xFFFFFFFF,                  //!< In indicates if the current image in valid 0xff - valid, 0x00 invalid image */
    .len = INVALID_LEN,                    //!< Image length in bytes. */
    .softVer = SOFTWARE_VER,               //!< Software version of the image */
    .hdrLen = offsetof(imgHdr_t, fixedHdr.rfu) + sizeof(((imgHdr_t){0}).fixedHdr.rfu),   //!< Total length of the image header */
    .rfu = 0xFFFF,                         //!< reserved bytes */
    .prgEntry = (uint32_t)(__section_begin(".intvec")), //!< Program entry address */
    .imgEndAddr = (uint32_t)(__section_end("ROSTK")),
#if (!defined(STACK_LIBRARY) && (defined(SPLIT_APP_STACK_IMAGE)))
    .imgType = OAD_IMG_TYPE_APP,
#else
  #if defined HAL_IMAGE_A
    .imgType =  OAD_IMG_TYPE_PERSISTENT_APP,
  #else
    .imgType = OAD_IMG_TYPE_APPSTACKLIB,
  #endif
#endif
  },

#if defined(SECURITY)
  {
    .segTypeSecure = IMG_SECURITY_SEG_ID,
    .wirelessTech = OAD_WIRELESS_TECH_BLE,
    .verifStat = DEFAULT_STATE,
    .secSegLen = 0x55,
    .secVer = SECURITY_VER,                                       //!< Image payload and length */
    .secTimestamp = 0x0,                              //!< Security timestamp */
    .secSignerInfo = 0x0,
  },
#endif /* if defined(SECURITY) */

#if (!defined(STACK_LIBRARY) && (defined(SPLIT_APP_STACK_IMAGE)))
  {
    .segTypeBd = IMG_BOUNDARY_SEG_ID,
    .wirelessTech = OAD_WIRELESS_TECH_BLE,
    .rfu = DEFAULT_STATE,
    .boundarySegLen = BOUNDARY_SEG_LEN,
  #if defined HAL_IMAGE_A                      //!< Persistent image */
    .imgType =  OAD_IMG_TYPE_PERSISTENT_APP,   //!< Persistent image Type */
    .stackStartAddr = INVALID_ADDR,            //!< Stack start adddress */
    .stackEntryAddr = INVALID_ADDR,
    .prgEntry = (uint32_t)&ENTRY_START,
  #else /* User application image */
    .imgType =  OAD_IMG_TYPE_APP,              //!< Application image Type */
    .stackEntryAddr = ICALL_STACK0_ADDR,
    .stackStartAddr = ICALL_STACK0_START,
  #endif /* defined HAL_IMAGE_A */

  .ram0StartAddr = (uint32)(__section_begin("RWDATA")),  //!< RAM entry start address */
  .imgType = OAD_IMG_TYPE_APP,
  },

#endif /* (!defined(STACK_LIBRARY) && (defined(SPLIT_APP_STACK_IMAGE))) */
  // Image payload segment initialization
  {
    .segTypeImg = IMG_PAYLOAD_SEG_ID,
    .wirelessTech = OAD_WIRELESS_TECH_BLE,
    .rfu = DEFAULT_STATE,                                         //!< Image payload and length */
    .startAddr = (uint32_t)&(_imgHdr.fixedHdr.imgID),
  }
 };
#elif defined(__clang__)
const imgHdr_t _imgHdr __attribute__((section( ".image_header"))) __attribute__((used)) =
{
  {
    .imgID = OAD_IMG_ID_VAL,
    .crc32 = DEFAULT_CRC,
    .bimVer = BIM_VER,
    .metaVer = META_VER,                   //!< Metadata version */
    .techType = OAD_WIRELESS_TECH_BLE,     //!< Wireless protocol type BLE/TI-MAC/ZIGBEE etc. */
    .imgCpStat = DEFAULT_STATE,            //!< Image copy status bytes */
    .crcStat = DEFAULT_STATE,              //!< CRC status */
    .imgNo = 0x1,                          //!< Image number of 'image type' */
    .imgVld = 0xFFFFFFFF,                  //!< In indicates if the current image in valid 0xff - valid, 0x00 invalid image */
    .len = INVALID_LEN,                     //!< Image length in bytes. */
    .softVer = SOFTWARE_VER,               //!< Software version of the image */
    .hdrLen = offsetof(imgHdr_t, fixedHdr.rfu) + sizeof(((imgHdr_t){0}).fixedHdr.rfu),   //!< Total length of the image header */
    .rfu = 0xFFFF,                         //!< reserved bytes */
    .prgEntry = (uint32_t)&prgEntryAddr,
    .imgEndAddr = (uint32_t)&flashEndAddr,
#if (!defined(STACK_LIBRARY) && (defined(SPLIT_APP_STACK_IMAGE)))
    .imgType = OAD_IMG_TYPE_APP,
#else
  #if defined HAL_IMAGE_A
    .imgType =  OAD_IMG_TYPE_PERSISTENT_APP,
  #else
    .imgType = OAD_IMG_TYPE_APPSTACKLIB,
  #endif
#endif
  },

#if (defined(SECURITY))
  {
    .segTypeSecure = IMG_SECURITY_SEG_ID,
    .wirelessTech = OAD_WIRELESS_TECH_BLE,
    .verifStat = DEFAULT_STATE,
    .secSegLen = 0x55,
    .secVer = SECURITY_VER,                     //!< Image payload and length */
    .secTimestamp = 0x0,                         //!< Security timestamp */
    .secSignerInfo = {0x0},
  },
#endif

#if (!defined(STACK_LIBRARY) && (defined(SPLIT_APP_STACK_IMAGE)))
  .segTypeBd = IMG_BOUNDARY_SEG_ID,
  .wirelessTech1 = OAD_WIRELESS_TECH_BLE,
  .rfu = DEFAULT_STATE,
  .boundarySegLen = BOUNDARY_SEG_LEN,
  .ram0StartAddr = (uint32_t)&ramStartHere,  //!< RAM entry start address */

  #if defined HAL_IMAGE_A                    //! Persistent image */
    .imgType =  OAD_IMG_TYPE_PERSISTENT_APP, //!< Persistent image Type */
    .stackStartAddr = INVALID_ADDR,          //!< Stack start address */
    .stackEntryAddr = INVALID_ADDR,
  #else /* User application image */
    .imgType =  OAD_IMG_TYPE_APP,            //!< Application image Type */
    .stackEntryAddr = ICALL_STACK0_ADDR,
    .stackStartAddr = ICALL_STACK0_START,
  #endif /* defined HAL_IMAGE_A */
    .imgType = OAD_IMG_TYPE_APP,
#endif /* STACK_LIBRARY */

  // Image payload segment initialization
   {
     .segTypeImg = IMG_PAYLOAD_SEG_ID,
     .wirelessTech = OAD_WIRELESS_TECH_BLE,
     .rfu = DEFAULT_STATE,
     .startAddr = (uint32_t)&(_imgHdr.fixedHdr.imgID),
   }
 };
 #endif /*  defined(__clang__) */
