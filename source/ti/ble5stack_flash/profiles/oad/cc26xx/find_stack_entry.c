/******************************************************************************

 @file  find_stack_entry.c

 @brief This module finds th entry point of stack application from stack image
 header.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2012 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/* -----------------------------------------------------------------------------
 *                                          Includes
 * -----------------------------------------------------------------------------
 */
#include <common/cc26xx/flash_interface/flash_interface.h>
#include <common/cc26xx/bim/bim_util.h>

/* -----------------------------------------------------------------------------
 *                                          Externs
 * -----------------------------------------------------------------------------
 */
extern const imgHdr_t _imgHdr;

/*********************************************************************
 * @fn      findStackBoundaryAddr
 *
 * @brief   This function is used to find the active address of the app/stack
 *          boundary for on-chip OAD applications
 *
 * @return  stackBoundaryAddr - The address of the stack boundary information
 */
uint32_t findStackBoundaryAddr()
{
    imgHdr_t *imgHdr;
    bool search = true;
    bool searchFromUsrApp = true;

    /*
     * To be efficient, start search at flash page starting at flash page before
     * the start of persistent image for on-chip OAD and for offchip from the
     * start of the flash.
     */
    int16_t flashPageNum = (_imgHdr.imgPayload.startAddr > 0)?(FLASH_PAGE(_imgHdr.imgPayload.startAddr) - 1):0;

    if(_imgHdr.fixedHdr.imgType == OAD_IMG_TYPE_PERSISTENT_APP)
    {
        searchFromUsrApp = false;
    }

    /* Search all flash pages for the stack header */
    do
    {
        /* Read image header */
        imgHdr = (imgHdr_t *)FLASH_ADDRESS(flashPageNum, 0);

        /* Check imageID bytes */
        if (imgIDCheck(&imgHdr->fixedHdr) == true)
        {
            /* Check that we found a stack */
            if((imgHdr->fixedHdr.imgType == OAD_IMG_TYPE_STACK) /* matching image type */
               &&
               (imgHdr->fixedHdr.crcStat != CRC_INVALID)) /* Valid CRC */
            {
                return (flashPageNum * INTFLASH_PAGE_SIZE);
            }
        }
        if(!searchFromUsrApp)
        {
            search = (flashPageNum-- >= 0);
        }
        else
        {
            search = (flashPageNum++ < MAX_ONCHIP_FLASH_PAGES);
        }

    }while(search);

    return (0xFFFFFFFF);
}

/*******************************************************************************
*/
