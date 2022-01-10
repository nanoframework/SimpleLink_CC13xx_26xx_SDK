/******************************************************************************

 @file  hal_flash_wrapper.c

 @brief This file implements the hal_flash interface for the flash driver.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2005 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "hal_board.h"
#include "hal_types.h"
#include <driverlib/flash.h>
#include <inc/hw_memmap.h>

uint8* HalFlashGetAddress( uint8 pg, uint16 offset );
/**************************************************************************************************
 * @fn          HalFlashRead
 *
 * @brief       This function reads 'cnt' bytes from the internal flash.
 *
 * input parameters
 *
 * @param       pg - Valid HAL flash page number (ie < 128).
 * @param       offset - Valid offset into the page (so < HAL_NV_PAGE_SIZE and byte-aligned is ok).
 * @param       buf - Valid buffer space at least as big as the 'cnt' parameter.
 * @param       cnt - Valid number of bytes to read: a read cannot cross into the next 32KB bank.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void HalFlashRead(uint8 pg, uint16 offset, uint8 *buf, uint16 cnt)
{
  halIntState_t cs;

  // Calculate the offset into the containing flash bank as it gets mapped into XDATA.
  uint8 *ptr = HalFlashGetAddress(pg, offset);

  // Enter Critical Section.
  HAL_ENTER_CRITICAL_SECTION(cs);

  // Read data.
  while (cnt--)
  {
    *buf++ = *ptr++;
  }

  // Exit Critical Section.
  HAL_EXIT_CRITICAL_SECTION(cs);
}

/**************************************************************************************************
 * @fn          HalFlashWrite
 *
 * @brief       This function reads 'cnt' bytes from the internal flash.
 *
 * input parameters
 *
 * @param       addr - Valid HAL flash write address: actual addr / 4 and quad-aligned.
 * @param       buf - Valid buffer space at least as big as the 'cnt' parameter.
 * @param       cnt - Valid number of bytes to write: a write cannot cross into the next 32KB bank.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void HalFlashWrite(uint32 addr, uint8 *buf, uint16 cnt)
{
  FlashProgram( buf, addr, cnt );
}

/**************************************************************************************************
 * @fn          HalFlashErase
 *
 * @brief       This function erases 'cnt' pages of the internal flash.
 *
 * input parameters
 *
 * @param       pg - Valid HAL flash page number (ie < 128) to erase.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void HalFlashErase(uint8 pg)
{
  FlashSectorErase( (uint32)HalFlashGetAddress(pg, 0));
}

/**************************************************************************************************
 * @fn          HalFlashGetAddress
 *
 * @brief       This function maps a page and offset to the flash address
 *
 * input parameters
 *
 * @param       pg - Valid HAL flash page number (ie < 128).
 *
 * @param       offset - Valid HAL flash offset (ie < 4096).
 *
 * output parameters
 *
 * None.
 *
 * @return      flashAddr - the flash address to map to.
 **************************************************************************************************
 */
uint8* HalFlashGetAddress( uint8 pg, uint16 offset )
{
#ifndef FEATURE_OAD
  // Calculate the offset into the containing flash bank as it gets mapped into XDATA.
  uint8 *flashAddr = (uint8 *)(offset + HAL_NV_START_ADDR) + ((pg % HAL_NV_PAGE_BEG )* HAL_FLASH_PAGE_SIZE);

  return flashAddr;
#else //FEATURE_OAD
  // The actual address is a 4-KiloByte multiple of the page number plus offset in bytes.
  return (uint8*)((pg << 12) + offset);
#endif //FEATURE_OAD
}


/**************************************************************************************************
*/
