/******************************************************************************
*  Filename:       rf_patch_cpe_multi_protocol.h
*  Revised:        $Date: 2019-04-29 17:29:16 +0200 (ma, 29 apr 2019) $
*  Revision:       $Revision: 19024 $
*
*  Description: RF core patch for multi-protocol support (all available API command sets) in CC13x2 and CC26x2
*
*  Copyright (c) 2015-2019, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/
#ifndef _RF_PATCH_CPE_MULTI_PROTOCOL_H
#define _RF_PATCH_CPE_MULTI_PROTOCOL_H

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>

#ifndef CPE_PATCH_TYPE
#define CPE_PATCH_TYPE static const uint32_t
#endif

#ifndef SYS_PATCH_TYPE
#define SYS_PATCH_TYPE static const uint32_t
#endif

#ifndef PATCH_FUN_SPEC
#define PATCH_FUN_SPEC static inline
#endif

#ifndef _APPLY_PATCH_TAB
#define _APPLY_PATCH_TAB
#endif


CPE_PATCH_TYPE patchImageMultiProtocol[] = {
   0x21004079,
   0x210040e3,
   0x210040a5,
   0x21004185,
   0x210041c1,
   0x21004109,
   0x21004115,
   0x21004121,
   0x2100424d,
   0x21004155,
   0x2100416d,
   0x79654c07,
   0xf809f000,
   0x40697961,
   0xd5030749,
   0x4a042101,
   0x60110389,
   0xb570bd70,
   0x47084902,
   0x21000380,
   0x40041108,
   0x0000592d,
   0xf819f000,
   0x296cb2e1,
   0x2804d00b,
   0x2806d001,
   0x490ed107,
   0x07c97809,
   0x7821d103,
   0xd4000709,
   0x490b2002,
   0x210c780a,
   0xd0024211,
   0x22804909,
   0xb003600a,
   0xb5f0bdf0,
   0x4907b083,
   0x48044708,
   0x22407801,
   0x70014391,
   0x47004804,
   0x210000c8,
   0x21000133,
   0xe000e200,
   0x00031641,
   0x00031b23,
   0x4700b570,
   0xf882f000,
   0x47004800,
   0x00007f57,
   0xf882f000,
   0x47004800,
   0x0000881b,
   0x0a9b9b03,
   0x2b834d08,
   0x4c08d10a,
   0x069b8923,
   0xb407d506,
   0xf805f000,
   0xd0002800,
   0xbc073542,
   0xb5704728,
   0x47004802,
   0x00020b1f,
   0x21000160,
   0x000209eb,
   0x781a4b09,
   0x43a22408,
   0xd0002800,
   0x701a4322,
   0x47104a00,
   0x00008e83,
   0x78084903,
   0xd4010700,
   0x47004802,
   0x00004770,
   0x21000380,
   0x00007e5f,
   0x490cb510,
   0x4a0c4788,
   0x5e512106,
   0xd0072900,
   0xd0052902,
   0xd0032909,
   0xd0012910,
   0xd1072911,
   0x43c92177,
   0xdd014288,
   0xdd012800,
   0x43c0207f,
   0x0000bd10,
   0x000065a9,
   0x21000380,
   0x4810b510,
   0x481088c1,
   0xd0182905,
   0x68214c0f,
   0x0052084a,
   0x6ba26022,
   0x00520852,
   0x602163a2,
   0xff90f7ff,
   0x07006ba0,
   0x2001d408,
   0x60606020,
   0x1c402000,
   0xdbfc280c,
   0x62202014,
   0xf7ffbd10,
   0xbd10ff81,
   0x21000380,
   0x00005b3f,
   0x40046000,
   0x490c6b80,
   0x0f000700,
   0x47707148,
   0x490a4a09,
   0x79502318,
   0x7e4956cb,
   0x428118c0,
   0x4608dd01,
   0x280fe002,
   0x200fdd00,
   0x090989d1,
   0x43010109,
   0x477081d1,
   0x210002e0,
   0x21000088,
   0x47702000,
};
#define _NWORD_PATCHIMAGE_MULTI_PROTOCOL 129

#define _NWORD_PATCHCPEHD_MULTI_PROTOCOL 0

#define _NWORD_PATCHSYS_MULTI_PROTOCOL 0



#ifndef _MULTI_PROTOCOL_SYSRAM_START
#define _MULTI_PROTOCOL_SYSRAM_START 0x20000000
#endif

#ifndef _MULTI_PROTOCOL_CPERAM_START
#define _MULTI_PROTOCOL_CPERAM_START 0x21000000
#endif

#define _MULTI_PROTOCOL_SYS_PATCH_FIXED_ADDR 0x20000000

#define _MULTI_PROTOCOL_PATCH_VEC_ADDR_OFFSET 0x03D0
#define _MULTI_PROTOCOL_PATCH_TAB_OFFSET 0x03D4
#define _MULTI_PROTOCOL_IRQPATCH_OFFSET 0x0480
#define _MULTI_PROTOCOL_PATCH_VEC_OFFSET 0x404C

#define _MULTI_PROTOCOL_PATCH_CPEHD_OFFSET 0x04E0

#ifndef _MULTI_PROTOCOL_NO_PROG_STATE_VAR
static uint8_t bMultiProtocolPatchEntered = 0;
#endif

PATCH_FUN_SPEC void enterMultiProtocolCpePatch(void)
{
#if (_NWORD_PATCHIMAGE_MULTI_PROTOCOL > 0)
   uint32_t *pPatchVec = (uint32_t *) (_MULTI_PROTOCOL_CPERAM_START + _MULTI_PROTOCOL_PATCH_VEC_OFFSET);

   memcpy(pPatchVec, patchImageMultiProtocol, sizeof(patchImageMultiProtocol));
#endif
}

PATCH_FUN_SPEC void enterMultiProtocolCpeHdPatch(void)
{
#if (_NWORD_PATCHCPEHD_MULTI_PROTOCOL > 0)
   uint32_t *pPatchCpeHd = (uint32_t *) (_MULTI_PROTOCOL_CPERAM_START + _MULTI_PROTOCOL_PATCH_CPEHD_OFFSET);

   memcpy(pPatchCpeHd, patchCpeHd, sizeof(patchCpeHd));
#endif
}

PATCH_FUN_SPEC void enterMultiProtocolSysPatch(void)
{
}

PATCH_FUN_SPEC void configureMultiProtocolPatch(void)
{
   uint8_t *pPatchTab = (uint8_t *) (_MULTI_PROTOCOL_CPERAM_START + _MULTI_PROTOCOL_PATCH_TAB_OFFSET);


   pPatchTab[76] = 0;
   pPatchTab[62] = 1;
   pPatchTab[64] = 2;
   pPatchTab[91] = 3;
   pPatchTab[79] = 4;
   pPatchTab[140] = 5;
   pPatchTab[150] = 6;
   pPatchTab[13] = 7;
   pPatchTab[31] = 8;
   pPatchTab[152] = 9;
   pPatchTab[151] = 10;
}

PATCH_FUN_SPEC void applyMultiProtocolPatch(void)
{
#ifdef _MULTI_PROTOCOL_NO_PROG_STATE_VAR
   enterMultiProtocolSysPatch();
   enterMultiProtocolCpePatch();
#else
   if (!bMultiProtocolPatchEntered)
   {
      enterMultiProtocolSysPatch();
      enterMultiProtocolCpePatch();
      bMultiProtocolPatchEntered = 1;
   }
#endif
   enterMultiProtocolCpeHdPatch();
   configureMultiProtocolPatch();
}

PATCH_FUN_SPEC void refreshMultiProtocolPatch(void)
{
   enterMultiProtocolCpeHdPatch();
   configureMultiProtocolPatch();
}

#ifndef _MULTI_PROTOCOL_NO_PROG_STATE_VAR
PATCH_FUN_SPEC void cleanMultiProtocolPatch(void)
{
   bMultiProtocolPatchEntered = 0;
}
#endif

PATCH_FUN_SPEC void rf_patch_cpe_multi_protocol(void)
{
   applyMultiProtocolPatch();
}


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  _RF_PATCH_CPE_MULTI_PROTOCOL_H

