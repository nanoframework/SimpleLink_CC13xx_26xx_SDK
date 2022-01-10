/******************************************************************************

 @file  mac_ie_build.c

 @brief This file implements building of MAC header and payload IEs.

 Group: WCS, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2021, Texas Instruments Incorporated
 All rights reserved.

 IMPORTANT: Your use of this Software is limited to those specific rights
 granted under the terms of a software license agreement between the user
 who downloaded the software, his/her employer (which must be your employer)
 and Texas Instruments Incorporated (the "License"). You may not use this
 Software unless you agree to abide by the terms of the License. The License
 limits your use, and you acknowledge, that the Software may not be modified,
 copied or distributed unless embedded on a Texas Instruments microcontroller
 or used solely and exclusively in conjunction with a Texas Instruments radio
 frequency transceiver, which is integrated into your product. Other than for
 the foregoing purpose, you may not use, reproduce, copy, prepare derivative
 works of, modify, distribute, perform, display or sell this Software and/or
 its documentation for any purpose.

 YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
 PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
 NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
 TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
 NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
 LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
 INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
 OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
 OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

 Should you have any questions regarding your right to use this Software,
 contact Texas Instruments Incorporated at www.TI.com.

 ******************************************************************************
 
 
 *****************************************************************************/

/*-------------------------------------------------------------------------------------------------
*           Include
*--------------------------------------------------------------------------------------------------
*/

#ifndef LIBRARY
#include "ti_wisunfan_features.h"
#endif

#include "mac_ie_build.h"
#include "mac_ie_parse.h"

#include "mac_mgmt.h"
#include "mac_spec.h"
#include "mac_pib.h"

#include "rom_jt_154.h"
/*-------------------------------------------------------------------------------------------------
*           Macros
*--------------------------------------------------------------------------------------------------
*/

/*-------------------------------------------------------------------------------------------------
 *           Functions
 *--------------------------------------------------------------------------------------------------
 */

#if !defined(TIMAC_ROM_PATCH)
/**
 * @brief This function builds and inserts header ies in the
 *        transmit buffer.
 *
 * @param pBuf           pointer to the structure containing the
 *                       transmit message buffer.
 * @param pHeaderIes     pointer to the list of header ie's that
 *                       needs to be built and inserted in the
 *                       transmit buffer.
 *
 * @return uint8         returns MAC_SUCCESS if successful.
 *                       MAC_NO_DATA if it fails to build the
 *                       requested IE.
 */
MAC_INTERNAL_API uint8
macBuildHeaderIes(macTx_t* pBuf, headerIeInfo_t *pHeaderIes )
{
  uint8 i;
  uint8 status = MAC_SUCCESS;

  MAC_PARAM_STATUS(pHeaderIes, status);
  MAC_PARAM_STATUS(pBuf, status);

  if ( MAC_SUCCESS != status )
  {
    return status;
  }

  /**
   * Start backward, with the header termination IE.
   */
  *(--pBuf->msdu.p) = MAC_HEADER_IE_TERM_SECOND_BYTE;

  if ( pBuf->internal.ptrPayloadIE )
  {
    *(--pBuf->msdu.p) = MAC_HEADER_IE_TERM_FIRST_BYTE_PAYIE;
  }
  else
  {
    *(--pBuf->msdu.p) = MAC_HEADER_IE_TERM_FIRST_BYTE_NO_PAYIE;
  }


  pBuf->msdu.len += 2;
  pBuf->internal.headerIesLen = 2;

  for (i = 0; i < MAC_HEADER_IE_MAX; i++ )
  {
    if ( MAC_HEADER_IE_TERM == pHeaderIes[i].elementId )
    {
      break;
    }

    /**
     * For now we have only WiSUN header IE. As and when we have
     * more IE's, build support to insert it, here.
     */

    if ( (MAC_HEADER_IE_WISUN == pHeaderIes[i].elementId) && (pHeaderIes[i].subIdBitMap) )
    {
      uint16 ieLen;

      /* Get the WiSUN Sub IE's len */
      ieLen = MAC_FH_GET_IE_LEN_FN( pHeaderIes[i].subIdBitMap );

      pBuf->msdu.p = pBuf->msdu.p - ieLen;

      //ToDo: Check to make sure we do not cross the buffer boundary.

      /* Insert the WiSUN Sub IE's */
      if ( pHeaderIes[i].subIdBitMap && MAC_FH_RSL_IE )
      {
          FHIE_ieInfo_t rslVal;

          //rslVal.rslIe.rsl = 0x99; /* for debug */
          rslVal.rslIe.rsl = rxFhRsl;
          if ( 0 == MAC_FH_GENERATE_IE_FN(pBuf->msdu.p, pHeaderIes[i].subIdBitMap, pBuf, &rslVal))
          {
            status = MAC_NO_DATA;
            return status;
          }
      }
      else
      {
          if ( 0 == MAC_FH_GENERATE_IE_FN(pBuf->msdu.p, pHeaderIes[i].subIdBitMap, pBuf, NULL))
          {
            status = MAC_NO_DATA;
            return status;
          }
      }
      pBuf->msdu.len += ieLen;
      pBuf->internal.headerIesLen += ieLen;
    }
  }

  pBuf->internal.ptrHeaderIE = pBuf->msdu.p;
  return status;
}
#endif

/**
 * @brief Builds the MLME payload co-existence sub IE.
 *
 * @param pBuf      pointer to the structure containing the transmitpBuf
 *                  message buffer.
 * @param ieIdOnly  if TRUE no IE content will be populated and the length
 *                  field will be set to zero.
 *
 * @return uint16 returns the length of the IE if successful,
 *         otherwise zero.
 */
uint16 macBuildPayloadCoexistIe( macTx_t* pBuf, bool ieIdOnly )
{
  if ( FALSE == ieIdOnly )
  {
    uint8 finalCapSlot;

    if ( (MAC_PAYLOAD_MLME_SUB_IE_COEXIST_LEN + MAC_PAYLOAD_SUB_IE_HEADER_LEN +
          2 * MAC_PAYLOAD_IE_HEADER_LEN + pBuf->msdu.len) >
         pBuf->internal.totalLen )
    {
      return 0;
    }

    /**
     * +-------+-------+-------+-------+-------+-------+-------+
     * |Bits0-3| 4-7   | 8-11  | 12-15 | 16-19 | 20-23 | 24-39 |
     * +-------+-------+-------+-------+-------+-------+-------+
     * |Beacon | Super | Final |E.Becon| Offset| CAP   |NBPAN  |
     * | Order | Order | C.slot| Order |T. slot| Backof|E. ordr|
     * +-------+-------+-------+-------+-------+-------+-------+
     */
    /* Working backwards */
    *(--pBuf->msdu.p) = HI_UINT16(pMacPib->eBeaconOrderNBPAN);
    *(--pBuf->msdu.p) = LO_UINT16(pMacPib->eBeaconOrderNBPAN);

    //ToDo: The Cap Backoff offset needs to be filled in by low level MAC,
    //      May need the location of the field.
    *(--pBuf->msdu.p) = 0;

    /* time offset */
    *(pBuf->msdu.p) = (pMacPib->offsetTimeSlot << MAC_COEXIST_IE_OFFSET_TIME_SLOT_POS);

    /* get final cap slot */
    finalCapSlot = MAC_A_NUM_SUPERFRAME_SLOTS - 1;

    *(--pBuf->msdu.p) = (finalCapSlot << MAC_COEXIST_IE_FINAL_CAP_SLOT_POS) |
             (pMacPib->eBeaconOrder << MAC_COEXIST_IE_ENHANCED_BEACON_ORDER_POS);

    *(--pBuf->msdu.p) = (pMacPib->beaconOrder << MAC_COEXIST_IE_BEACON_ORDER_POS) |
              (pMacPib->superframeOrder << MAC_COEXIST_IE_SUPERFRAME_ORDER_POS);

    *(--pBuf->msdu.p) = MAC_PAYLOAD_MLME_SUB_IE_COEXIST;

    *(--pBuf->msdu.p) = MAC_PAYLOAD_MLME_SUB_IE_COEXIST_LEN;

    return (MAC_PAYLOAD_MLME_SUB_IE_COEXIST_LEN +
            MAC_PAYLOAD_SUB_IE_HEADER_LEN);
  }
  else
  {
    if ( ( MAC_PAYLOAD_SUB_IE_HEADER_LEN + 2 * MAC_PAYLOAD_IE_HEADER_LEN + pBuf->msdu.len) >
         pBuf->internal.totalLen )
    {
      return 0;
    }

    *(--pBuf->msdu.p) = MAC_PAYLOAD_MLME_SUB_IE_COEXIST;

    /* zero length for ieIdOnly */
    *(--pBuf->msdu.p) = 0;

    return (MAC_PAYLOAD_SUB_IE_HEADER_LEN);
  }
}

/**
 * @brief This function builds and inserts payload ies in the
 *        transmit buffer.
 *
 * @param pBuf           pointer to the structure containing the
 *                       transmit message buffer.
 * @param pPayloadIeInfo pointer to the list of payload ie's
 *                       that needs to be built and inserted in
 *                       the transmit buffer.
 * @param ieIdOnly       if TRUE no IE content will be populated and the length
 *                       field will be set to zero.
 *
 * @return uint8         returns MAC_SUCCESS if successful.
 *                       MAC_NO_RESOURCES if it fails to build the
 *                       requested IE.
 */
MAC_INTERNAL_API uint8
macBuildPayloadIes( macTx_t* pBuf, payloadIeInfo_t *pPayloadIeInfo, bool includeTermIe, bool ieIdOnly )
{
  uint8 status = MAC_SUCCESS;
  uint8 i, j;

  MAC_PARAM_STATUS( (NULL != pBuf), status );
  MAC_PARAM_STATUS( (NULL != pPayloadIeInfo), status );

  if ( MAC_SUCCESS != status )
  {
    return status;
  }

  if (includeTermIe)
  {
    /**
     * Fill the Payload termination IE.
     */
    *(--pBuf->msdu.p) = MAC_PAYLOAD_IE_TERM_SECOND_BYTE_PAYIE;
    *(--pBuf->msdu.p) = MAC_PAYLOAD_IE_TERM_FIRST_BYTE;

    pBuf->msdu.len += MAC_PAYLOAD_IE_HEADER_LEN;
  }

  for ( i = 0;
        (i < MAC_PAYLOAD_IE_MAX) && (MAC_PAYLOAD_IE_TERM_GID != (pPayloadIeInfo[i].gId & 0x0F));
        i++ )
  {
    if ( MAC_PAYLOAD_IE_MLME_GID == pPayloadIeInfo[i].gId )
    {
      uint16 mlmeContentLen = 0;
      /* Check if MLME has sub id's*/
      for (j = 0; j < MAC_PAYLOAD_SUB_IE_MAX; j++)
      {
        if ( MAC_PAYLOAD_SUB_IE_TERM == pPayloadIeInfo[i].subId.id[j])
        {
          break;
        }

        switch (pPayloadIeInfo[i].subId.id[j] )
        {
          case MAC_PAYLOAD_MLME_SUB_IE_COEXIST:
          {
            uint16 len = MAP_macBuildPayloadCoexistIe( pBuf, ieIdOnly );
            if (0 == len)
            {
              status = MAC_NO_RESOURCES;
              return status;
            }
            mlmeContentLen += len;
          }
          break;
        }
      }

      /* Fill the MLME header */
      *(--pBuf->msdu.p) = (HI_UINT16(mlmeContentLen) & 0x03) |
                          (((uint8)MAC_PAYLOAD_IE_MLME_GID << MAC_PAYLOAD_GID_POS) & 0x78) |
                          (((uint8)MAC_PAYLOAD_IE_TYPE_VAL << MAC_PAYLOAD_TYPE_POS) & 0x80) ;
      *(--pBuf->msdu.p) = LO_UINT16(mlmeContentLen);

      pBuf->msdu.len += mlmeContentLen + MAC_PAYLOAD_IE_HEADER_LEN;
    }
    else if ( MAC_PAYLOAD_IE_WISUN_GID == pPayloadIeInfo[i].gId )
    {
      int len = 0;
      if (MAC_FH_ENABLED)
      {
        len = MAC_FH_GET_IE_LEN_FN( pPayloadIeInfo[i].subId.subIdBMap);
        if (len &&
            ((len + pBuf->msdu.len + MAC_PAYLOAD_IE_HEADER_LEN) <= pBuf->internal.totalLen) )
        {
          uint8* ptr = pBuf->msdu.p - len;

          if ( 0 == MAC_FH_GENERATE_IE_FN(ptr, pPayloadIeInfo[i].subId.subIdBMap, pBuf, NULL) )
          {
            status = MAC_NO_RESOURCES;
            return status;
          }

          pBuf->msdu.p -= len;
          pBuf->msdu.len += len;
        }
        else
        {
          status = MAC_NO_RESOURCES;
          return status;
        }
      }
    }
  }

  pBuf->internal.ptrPayloadIE = pBuf->msdu.p;
  return status;
}

