/******************************************************************************

 @file  npi_data.h

 @brief NPI Data structures

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
#ifndef NPIDATA_H
#define NPIDATA_H

#ifdef __cplusplus
extern "C"
{
#endif

// ****************************************************************************
// includes
// ****************************************************************************

#include <stdint.h>

// ****************************************************************************
// defines
// ****************************************************************************

// ****************************************************************************
// typedefs
// ****************************************************************************

//! \brief NPI Command IDs.
//!
typedef enum
{
    NPIMSG_Type_ASYNC       = 0x00,
    NPIMSG_Type_SYNCREQ     = 0x01,
    NPIMSG_Type_SYNCRSP     = 0x02
} NPIMSG_Type;


//! \brief Structure to send an NPI message to the stack thread.
//!
typedef struct _npimsg_t
{
    // Msg Type
    uint_least8_t msgType;

    // message buffer size
    uint16_t pBufSize;

    // message buffer
    uint8_t *pBuf;
} NPIMSG_msg_t;


//*****************************************************************************
// globals
//*****************************************************************************

//*****************************************************************************
// function prototypes
//*****************************************************************************

#ifdef __cplusplus
}
#endif

#endif /* NPIDATA_H */
