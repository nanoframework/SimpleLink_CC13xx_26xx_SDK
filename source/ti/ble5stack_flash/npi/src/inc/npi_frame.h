/******************************************************************************

 @file  npi_frame.h

 @brief This file contains the Network Processor Interface (NPI) data frame
        specific functions definitions.

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
#ifndef NPIFRAME_H
#define NPIFRAME_H

#ifdef __cplusplus
extern "C"
{
#endif

// ****************************************************************************
// includes
// ****************************************************************************
#include <inc/hw_types.h>
#include "npi_data.h"

// ****************************************************************************
// defines
// ****************************************************************************

// ****************************************************************************
// typedefs
// ****************************************************************************

//! \brief typedef for call back function to return a complete NPI message.
//!        The npiFrame module encapsulates the collecting/parsing of the
//!        complete message and returns via this callback the received message.
//!        NOTE: the message buffer does NOT include the framing elements
//!        (i.e. Start of Frame, FCS/CRC or similar).
typedef void (*npiIncomingFrameCBack_t)( uint16_t frameSize, uint8_t *pFrame,
                                         NPIMSG_Type msgType );


//*****************************************************************************
// globals
//*****************************************************************************

//*****************************************************************************
// function prototypes
//*****************************************************************************
// ----------------------------------------------------------------------------
//! \brief      Initialize Frame module with NPI callbacks.
//!
//! \param[in]  incomingFrameCB   Call back for complete inbound (from host)
//!                               messages
//!
//! \return     void
// ----------------------------------------------------------------------------
extern void NPIFrame_initialize(npiIncomingFrameCBack_t incomingFrameCB);


// ----------------------------------------------------------------------------
//! \brief      Bundles message into Transport Layer frame and NPIMSG_msg_t
//!             container.  A transport layer specific version of this function
//!             must be implemented.
//!
//! \param[in]  pData     Pointer to message buffer.
//!
//! \return     void
// ----------------------------------------------------------------------------
extern NPIMSG_msg_t * NPIFrame_frameMsg(uint8_t *pIncomingMsg);

// ----------------------------------------------------------------------------
//! \brief      Collects serial message buffer.  Called based on events
//!             received from the transport layer.  When an entire message has
//!             been successfully received, it is passed back to NPI task via
//!             the callback function above: npiIncomingFrameCBack_t.
//!
//! \return     void
// -----------------------------------------------------------------------------
extern void NPIFrame_collectFrameData(void);

#ifdef __cplusplus
}
#endif

#endif /* NPIFRAME_H */
