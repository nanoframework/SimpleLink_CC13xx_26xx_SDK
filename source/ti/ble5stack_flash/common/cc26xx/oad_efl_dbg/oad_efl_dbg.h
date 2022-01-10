/******************************************************************************

 @file  oad_efl_dbg.h

 @brief This module provides functionality for debugging external flash via UART
        for off-chip OAD applications.

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2017 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

#ifndef OAD_EFL_DBG_H
#define OAD_EFL_DBG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      OadEflDbg_displayMetadata
 *
 * @brief   Display external flash metadata
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
extern bool OadEflDbg_displayMetadata(uint8_t index);

/*********************************************************************
 * @fn      OadEflDbg_displayMetaList
 *
 * @brief   Displays external flash metadata list
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
extern bool OadEflDbg_displayMetaList(uint8_t index);

/*********************************************************************
 * @fn      OadEflDbg_eraseAll
 *
 * @brief   Erase all images and their meta pages
 *
 * @param   index - menu item index
 *
 * @return  None
 */
extern bool  OadEflDbg_eraseAll(uint8_t index);

/*********************************************************************
 * @fn      OadEflDbg_eraseSingle
 *
 * @brief   Erase selected meta pages
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
extern bool  OadEflDbg_eraseImg(uint8_t index);

/*********************************************************************
 * @fn      OadEflDbg_createFactoryImg
 *
 * @brief   Copies on-chip flash image to create factory image
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
extern bool OadEflDbg_createFactoryImg(uint8_t index);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* OAD_EFL_DBG_H */
