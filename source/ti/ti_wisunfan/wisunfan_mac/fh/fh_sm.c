/******************************************************************************

 @file fh_sm.c

 @brief TIMAC 2.0 FH finite state machine API

 Group: WCS LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2021, Texas Instruments Incorporated
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

/******************************************************************************
 Includes
 *****************************************************************************/

#ifndef LIBRARY
#include "ti_wisunfan_features.h"
#endif

#include "mac_pib.h"
#include "fh_api.h"
#include "fh_util.h"
#include "fh_mgr.h"
#include "fh_data.h"
#include "fh_sm.h"

#ifdef FEATURE_FREQ_HOP_MODE

/******************************************************************************
  ROM jump table will support
  1. TIMAC ROM image build
  2. TIMAC stack build without ROM image
  3. TIMAC stack with calling ROM image
 *****************************************************************************/
#include "rom_jt_154.h"


/**************************************************************************************************
 * @fn          FSM_transaction_debug
 *
 * @brief       This function tracks the state machine transaction and record
 * 			    the current state and events
 *
 * input parameters
 *
 * @param		currState - current state of FH state machine
 * @param		event - FH event
 * @param		pdata - input data, it can be casted to FH handle
 *
 * output parameters
 *
 * None
 *
 * @return      the status of transaction function. It is always FHAPI_STATUS_SUCCESS
 **************************************************************************************************
 */
FHAPI_status FSM_transaction_debug(uint16_t currState, uint16_t event, void* pdata)
{
    FHFSM_debugBuffer_t *pDbg;
    uint16_t idx,temp;

    /* avoid compiler warning */
    (void) pdata;

    pDbg = &FHFSM_Dbg;

    /* get index */
    idx = pDbg->idx;

    /* save the current state and event */
    temp = event & 0xFF;
    temp |= (currState & 0xFF) << 8 ;
    pDbg->state_evt[ idx++ ] = temp;

    /* handle buffer wrap around */
    if (idx >= FHFSM_MAX_DEBUG_EVENTS)
        idx = 0;

    pDbg->idx = idx;

    return FHAPI_STATUS_SUCCESS;
}

/******************************************************************************
* FUNCTION NAME: FSM_event
*
* DESCRIPTION:   This function perform event transition in the matrix
*
* Return Value:
*
* Input Parameters:  fsm_p - pointer to the state machine structure
*                    event - event causing transition
*                    data_p - data for activation function
*
* Output Parameters: update current state after action taken
* Functions Called:
*
* RLiang:
* we may need to add the mutex code when update FSM current state
******************************************************************************/
FHAPI_status FHSM_event(FSM_stateMachine_t *fsm_p,
        uint16_t         event,
        void             *data_p)
{
    FHAPI_status status = FHAPI_STATUS_SUCCESS;
    uint32_t     idx;

    /* boundary check */
    if ((fsm_p->currentState >= fsm_p->maxStates) || (event >= fsm_p->maxEvents))
    {
        return(FHAPI_STATUS_ERR);
    }

    /* Disable interrupts to enter critical section*/
    /* update FHSM Dbg History only on state transitions */
    if(fsm_p->currentState != fsm_p->previousState)
    {
        if (fsm_p->transitionFunc_p)
        {   /* user provide transition function */
            (fsm_p->transitionFunc_p)(fsm_p->currentState, event, data_p);
        }
    }

    fsm_p->previousState = fsm_p->currentState;

    /* Index to cell */
    idx = (fsm_p->currentState * fsm_p->maxEvents) + event;

    /* update current state */
    fsm_p->currentState = fsm_p->stateEventMatrix[idx].nextState;

    /* Restore interrupts to exit critical section  */

    /* activate transition function */
    (*fsm_p->stateEventMatrix[idx].actionFunc_p)(data_p);
    return(status);
}

#endif
