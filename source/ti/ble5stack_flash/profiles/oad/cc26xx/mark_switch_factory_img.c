/******************************************************************************

 @file  mark_switch_factory_img.c

 @brief This file marks own image header to switch to factory image.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2017 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include <driverlib/flash.h>
#include <common/cc26xx/oad/oad_image_header.h>
#include "onboard.h"

/*********************************************************************
 * GLOBAL VARIABLES
 */
// The current image's header is initialized in oad_image_header_app.c
extern const imgHdr_t _imgHdr;
/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn     markSwitchFactoryImg
 *
 * @brief   This function invalidate the the CRC status of it's own image
 *          header to mark it to switch to factory image
 *
 * @return  None
 */
void markSwitchFactoryImg(void)
{
    /* Read image header */
    uint32_t imgStartAddr = _imgHdr.imgPayload.startAddr;

    uint8_t crcStat = CRC_INVALID;

    uint8_t state = VIMSModeGet(VIMS_BASE);

    // Check VIMS state
    if (state != VIMS_MODE_DISABLED)
    {
      // Invalidate cache
      VIMSModeSet(VIMS_BASE, VIMS_MODE_DISABLED);

      // Wait for disabling to be complete
      while (VIMSModeGet(VIMS_BASE) != VIMS_MODE_DISABLED);
    }


    FlashProgram(&crcStat, (imgStartAddr + CRC_STAT_OFFSET),
                                     sizeof(_imgHdr.fixedHdr.crcStat));
    SystemReset();

    return;
}


/*********************************************************************
*********************************************************************/
