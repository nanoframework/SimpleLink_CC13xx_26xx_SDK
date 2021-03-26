/******************************************************************************

 @file  cc1352p1lp_board.c

 @brief This file is a simple gateway to include the appropriate Board.c
        file which is located in the following directories relative to this file:
        - CC1352P1_LAUNCHXL

        The project should set the include path to Board.h to point to
        the Board.h in this directory.
        This Board.h file will then define a symbol which is used in this
        file to include the appropriate Board.c file which is found in
        the same directory as Board.h
        This way the project can look the same (and only include this Board.c)
        file, when changing EM user only needs to update include path in
        the project options.
        Alternatively, the device specific board  files can just be included
        directly in the project.

 Group: WCS, LPC, BTS
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2017-2021, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*
 *   The location of this Board.h file depends on your project include path.
 *   Set it correctly to point to your CC1352P1_LAUNCHXL
 */
#if defined(CC13X2P1_LAUNCHXL)
    #include "../../boards/CC1352P1_LAUNCHXL/Board.h"
    #include "../../boards/CC1352P1_LAUNCHXL/CC1352P1_LAUNCHXL.c"
    #include "../../boards/CC1352P1_LAUNCHXL/CC1352P1_LAUNCHXL_fxns.c"
#else
    #error "Must define 'CC13X2P1_LAUNCHXL'. Please set include path to point \
    to appropriate device."
#endif



