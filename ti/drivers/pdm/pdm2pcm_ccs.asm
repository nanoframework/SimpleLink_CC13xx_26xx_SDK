;
;  Copyright (c) 2018, Texas Instruments Incorporated
;  All rights reserved.
;
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions
;  are met:
;
;  *  Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
;
;  *  Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
;
;  *  Neither the name of Texas Instruments Incorporated nor the names of
;     its contributors may be used to endorse or promote products derived
;     from this software without specific prior written permission.
;
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
;  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
;  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
;  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
;  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
;  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
;  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
;  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
;  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;

;
;  ======== pdm2pcm_cc26x0.sem3 ========
;  This file assumes TI Assembly Language Syntax.
;

  ; Determine whether input bitstream has big endian byte order (natural) or
  ; 16b little endian byte order (bytes #1, #0, #3, #2, #5, ... in sequence in memory)
;BITSTREAM_BE .set 1
BITSTREAM_LE16 .set 1

    .sect ".text:pdm2pcm16k"
    .thumb
    .global pdm2pcm16k

; bool pdm2pcm16k(const void* pIn, uint32_t* pState, const int32_t* pBqCoeffs, int16_t* pOut)
;
; Implements a balanced quality/performance PDM to PCM conversion from a 1.024 MHz PDM bitstream from
; a 4th order binary delta-sigma modulator to 16 kHz PCM output. Produces 32 output samples at a time and
; consumes 2048b or 256B of bitstream input.
; Params:
;   [in]    pIn          - Pointer to input bitstream (choose correct endianess for byte ordering!!!)
;   [inOut] pState       - Pointer to state struct (zero-initialize before use)
;   [in]    pBqCoeffs    - Pointer to biquad coefficients (see below for detail)
;   [out]   pOut         - Pointer to where samples are output
;
; Returns:
;   Boolean value: true means at least one output value saturated
;
; The signal processing algorithm performed is:
; - Decimation by 32 with a 4th order CIC decimator (from 1.024 MHz to 32 kHz). This decimator
;   has a fixed gain of +12 dB (i.e. a signal that is -12 dBFs will saturate output). If this
;   gain is not desired it can be adjusted by changing the gain coefficient of the first biquad
;   section in the following filter accordingly.
;     2048 samples PDM input (256B) -> 64 intermediate sample (32b) output [in place]
; - Intermediate signal is run through a N section biquad filter that should implement
;   a DC notch, a halfband filter and, ideally, passband correction for CIC attenuation
;   A fifth order filter structure that fulfills these requirements is given in the example
;   below.
;     64 intermediate sample (32b) input -> 64 intermediate sample (32b) outpur [in place]
; - A 1st order decimate by two CIC (i.e. mean of two and two samples) downsamples from 32 kHz
;   to 16 kHz
;     64 intermediate sample (32b) input -> 32 intermediate sample (32b) output [in place]
; - An optional N section user-defined biquad filter, that can be used to perform, for example,
;   EQ compensation
;     32 intermediate sample (32b) input -> 32 intermediate sample (32b) output [in place]
; - Saturation and output of 16b 2s complement PCM samples
;     32 intermediate sample (32b) input -> 32 output samples (16b) output
;
; The pBqCoeffs contains one record per biquad filter coefficient. The first filter is terminated by a
; single zero value and so is the second (two consecutive zero values at the end skips the second filter
; altogether. Each records has the following format:
; struct {
;     uint32_t g;      ; value of 0 here indicates end of IIR filter
;     int32_t  b1;
;     int32_t  b2;
;     int32_t  a1;
;     int32_t  a2;
; }
; Each second order section is assumed to have normalized both numerator and denominator,
; i.e b0 and a0 are 1. The gain g is applied to the input. If unormalized, g can be b0/a0
; and each coefficient in the numerator must then be divided by b0 and each coefficient
; in the denominator divided by a0.
; After the last section a dummy section with gain 0 indicates the end of the IIR filter.
; All coefficients must be in the SI.FFFF_FFFF_FF format, i.e 21 sign bits, one integer bit
; and ten fractional bits. Thus the range of the coefficients are [-2, 2) and the resolution
; is 2^-10
;
; All filter coefficients (including gain) can be changed dynamically if they reside in RAM.
; If, for any stage, the gain coefficient g >= 0x20000000 then it is treated as a pointer to a
; gain coefficent in RAM rather than as an actual gain coefficient. This allows the whole coefficient
; structure to reside in flash except for a few gain coefficients that need to be changed dynamically.
;
; The following filter coefficient structure is the default filter and shows where
; to insert the optional second filter. It has five filter stages and a reasonable tradeoff between
; runtime and frequency response.
;
; int32_t aBqCoeffs[] = {
;    166,     0, -1024, -1356,   342,     // DC-notch, halfband LP filter
;    200,   789,   934,  -994,   508,
;    538,   381,   944,  -519,   722,
;    732,   124,   987,  -386,   886,
;    763,    11,  1014,  -386,   886,
;    // Terminate first filter
;    0,
;    // Optional second filter of any order N. For example:
;    //1147, -1516,   522, -1699,   708, // +5dB, F0=500 Hz peak filter
;    // Terminate second filter (and end pdm2pcm function)
;    0
; };
;
; The following filter coefficients yield a flatter frequency response in the passband
; at the cost of an extra filter stage and thus more computational requirements.
; int32_t aBqCoeffs[] = {
;    // First filter H(z)=H1(z)*H2(z)*H3(z)*H4(z)*H5(z)*H6(z) [any order M supported]
;       345,   121,  1024,  -209,   919,  // H1(z): g, b1, b2, a1, a2
;       407,   765,  1024,  -376,   470,  // H2(z): g, b1, b2, a1, a2
;       392,     0, -1024, -1265,   245,  // H3(z): g, b1, b2, a1, a2
;       450,  1345,  1024,  -495,   269,  // H4(z): g, b1, b2, a1, a2
;       507,   400,  1024,  -296,   690,  // H5(z): g, b1, b2, a1, a2
;       554,   209,  1024,  -240,   870,  // H6(z): g, b1, b2, a1, a2
;    // Terminate first filter
;    0,
;    // Optional second filter of any order N. For example:
;    //1147, -1516,   522, -1699,   708, // +5dB, F0=500 Hz peak filter
;    // Terminate second filter (and end pdm2pcm function)
;    0
; };
;
; The size of the state struct pointed to by pState is 4*(6+2N) bytes, where N is the
; total number of biquads in pNqCoeffs.
pdm2pcm16k:
    .asmfunc
    ; Save some arguments, clobbered registers and return address
    PUSH    {R0-R11, LR}

    ; ### FIRST STAGE (1.021 MHz --> 32 kHz) ###
    ; **** 4th order decimate-by-32 CIC ****
    ; Register usage is:
    ; R0:      pIn
    ; R1:      pState / byte read
    ; R2:      temp0
    ; R3:      temp1
    ; R4:      Acc0 (stored in state)
    ; R5:      Acc1 (stored in state)
    ; R6:      Acc2 (stored in state)
    ; R7:      Acc3
    ; R8:      Diff2 (stored in state)
    ; R9:      Diff1 (stored in state)
    ; R10:     Diff0 (stored in state)
    ; R11:     pOut
    ; R12:     nOutSamples
    ; LR/R14:  pLut

    MOV     R11, R0                     ; R11: pOut is copy of pIn [perform in place]
    MOV     R12, #32*2                  ; R12: nOutSamples
    LDR     R14, cicOrd4Bits8Pad0LutLoc16

    ; Get accumulator/differentiator state
    LDM     R1, {R4-R6, R8-R10}

    ; Preload first data byte
    .if $$defined("BITSTREAM_BE")
        LDRB    R1, [R0], #1            ; Read in 8b of bitstream
    .elseif $$defined("BITSTREAM_LE16")
        LDRB    R1, [R0, #1]            ; Read in 8b of bitstream
    .else
        .emsg "Undefined bitstream input byte ordering"
    .endif

p2p16k_cic_SAMPLELOOP:
    ; To save two state variables and one differentiation we reset the innermost
    ; accumulator at the start of each decimation period
    MOV     R7, #0
p2p16k_cic_BYTELOOP:           ; Byte loop is 2*20 cycles
    .loop 32/8/2
        ; Update accumulator values for 8 clocks
        ; (disregarding input bits which we can, thanks to superposition, handle below)
        ADD     R7, R7, R4, LSL #7
        SUB     R7, R7, R4, LSL #3          ; Acc3 += 120*Acc0
        ADD     R7, R7, R5, LSL #5
        ADD     R7, R7, R5, LSL #2          ; Acc3 += 36*Acc1
        ADD     R7, R7, R6, LSL #3          ; Acc3 += 8*Acc2
        ADD     R6, R6, R4, LSL #5
        ADD     R6, R6, R4, LSL #2          ; Acc2 += 36*Acc0
        ADD     R6, R6, R5, LSL #3          ; Acc2 += 8*Acc1
        ADD     R5, R5, R4, LSL #3          ; Acc1 += 8*Acc0

        ; Read in 8b of bitstream input, use LUT, and add effect to each accumulator
        LDR     R2, [R14, R1, LSL #2]       ; R2: LUT entry for 8b of bitstream
        .if $$defined("BITSTREAM_BE")
            LDRB    R1, [R0], #1            ; Read in 8b of bitstream
        .elseif $$defined("BITSTREAM_LE16")
           LDRB    R1, [R0], #2             ; Read in 8b of bitstream
        .endif
        UBFX    R3, R2, #0, #4
        ADD     R4, R3                      ; Acc0 += LUT0
        UBFX    R3, R2, #4, #6
        ADD     R5, R3                      ; Acc1 += LUT1
        UBFX    R3, R2, #10, #7
        ADD     R6, R3                      ; Acc2 += LUT2
        UBFX    R3, R2, #17, #9
        ADD     R7, R3                      ; Acc3 += LUT3

        ; Update accumulator values for 8 clocks
        ; (disregarding input bits which we can, thanks to superposition, handle below)
        ADD     R7, R7, R4, LSL #7
        SUB     R7, R7, R4, LSL #3          ; Acc3 += 120*Acc0
        ADD     R7, R7, R5, LSL #5
        ADD     R7, R7, R5, LSL #2          ; Acc3 += 36*Acc1
        ADD     R7, R7, R6, LSL #3          ; Acc3 += 8*Acc2
        ADD     R6, R6, R4, LSL #5
        ADD     R6, R6, R4, LSL #2          ; Acc2 += 36*Acc0
        ADD     R6, R6, R5, LSL #3          ; Acc2 += 8*Acc1
        ADD     R5, R5, R4, LSL #3          ; Acc1 += 8*Acc0

        ; Read in 8b of bitstream input, use LUT, and add effect to each accumulator
        LDR     R2, [R14, R1, LSL #2]       ; R2: LUT entry for 8b of bitstream
        .if $$defined("BITSTREAM_BE")
            LDRB    R1, [R0], #1            ; Read in 8b of bitstream
        .elseif $$defined("BITSTREAM_LE16")
            LDRB    R1, [R0, #1]            ; Read in 8b of bitstream
        .endif
        UBFX    R3, R2, #0, #4
        ADD     R4, R3                      ; Acc0 += LUT0
        UBFX    R3, R2, #4, #6
        ADD     R5, R3                      ; Acc1 += LUT1
        UBFX    R3, R2, #10, #7
        ADD     R6, R3                      ; Acc2 += LUT2
        UBFX    R3, R2, #17, #9
        ADD     R7, R3                      ; Acc3 += LUT3
    .endloop

    ; End of decimation period, perform differentiation, output sample and see if we do more
    SUB     R3, R7, R8                      ; R3: Diff2out = Diff3out - DIff2 = Acc3 - Diff2
    MOV     R8, R7                          ; Diff2' = Diff3out = Acc3
    SUB     R2, R3, R9                      ; R2: Diff1out = Diff2out - Diff1
    MOV     R9, R3                          ; Diff1' = Diff2out
    SUB     R3, R2, R10                     ; R3: Diff0out = Diff1out - Diff0
    MOV     R10, R2                         ; Diff0' = Diff1out
    SUBS    R12, R12, #1                    ; nOutSamples--
    SUB     R3, R3, #0x80000;               ; Convert to 2s complement and SI3.F17 format (with 12 dB gain)
    STR     R3, [R11], #4
    BNE     p2p16k_cic_SAMPLELOOP

p2p16k_cic_EXIT:
    ; Get pointers to: R0: pIn/intermediateBuf, R1: pState, R2: pBqCoeffs
    POP     {R0-R2}
    ; Store accumulator/differentiator state
    STM     R1!, {R4-R6, R8-R10}
    ; p2pBiQuad expects intermediate buffer pointer on top of stack
    PUSH    {R0}
    B       p2pBiQuad
    .endasmfunc

cicOrd4Bits8Pad0LutLoc16:
    .word cicOrd4Bits8Pad0Lut

    .sect ".text:pdm2pcm8k"
    .thumb
    .global pdm2pcm8k

; bool pdm2pcm8k(const void* pIn, uint32_t* pState, const int32_t* pBqCoeffs, int16_t* pOut)
;
; Implements a balanced quality/performance PDM to PCM conversion from a 1.024 MHz PDM bitstream from
; a 4th order binary delta-sigma modulator to 8 kHz PCM output. Produces 32 output samples at a time and
; consumes 4096b or 512B of bitstream input.
; Params:
;   [in]    pIn          - Pointer to input bitstream (choose correct endianess for byte ordering!!!)
;   [inOut] pState       - Pointer to state struct (zero-initialize before use)
;   [in]    pBqCoeffs    - Pointer to biquad coefficients (see below for detail)
;   [out]   pOut         - Pointer to where samples are output
;
; Returns:
;   Boolean value: true means at least one output value saturated
;
; The signal processing algorithm performed is:
; - Decimation by 64 with a 4th order CIC decimator (from 1.024 MHz to 16 kHz). This decimator
;   has a fixed gain of +12 dB (i.e. a signal that is -12 dBFs will saturate output). If this
;   gain is not desired it can be adjusted by changing the gain coefficient of the first biquad
;   section in the following filter accordingly.
;     4096 samples PDM input (512B) -> 64 intermediate sample (32b) output [in place]
; - Intermediate signal is run through a N section biquad filter that should implement
;   a DC notch, a halfband filter and, ideally, passband correction for CIC attenuation
;   A fifth order filter structure that fulfills these requirements is given in the example
;   below.
;     64 intermediate sample (32b) input -> 64 intermediate sample (32b) outpur [in place]
; - A 1st order decimate by two CIC (i.e. mean of two and two samples) downsamples from 32 kHz
;   to 16 kHz
;     64 intermediate sample (32b) input -> 32 intermediate sample (32b) output [in place]
; - An optional N section user-defined biquad filter, that can be used to perform, for example,
;   EQ compensation
;     32 intermediate sample (32b) input -> 32 intermediate sample (32b) output [in place]
; - Saturation and output of 16b 2s complement PCM samples
;     32 intermediate sample (32b) input -> 32 output samples (16b) output
;
; The pBqCoeffs contains one record per biquad filter coefficient. The first filter is terminated by a
; single zero value and so is the second (two consecutive zero values at the end skips the second filter
; altogether. Each records has the following format:
; struct {
;     uint32_t g;      ; value of 0 here indicates end of IIR filter
;     int32_t  b1;
;     int32_t  b2;
;     int32_t  a1;
;     int32_t  a2;
; }
; Each second order section is assumed to have normalized both numerator and denominator,
; i.e b0 and a0 are 1. The gain g is applied to the input. If unormalized, g can be b0/a0
; and each coefficient in the numerator must then be divided by b0 and each coefficient
; in the denominator divided by a0.
; After the last section a dummy section with gain 0 indicates the end of the IIR filter.
; All coefficients must be in the SI.FFFF_FFFF_FF format, i.e 21 sign bits, one integer bit
; and ten fractional bits. Thus the range of the coefficients are [-2, 2) and the resolution
; is 2^-10
;
; All filter coefficients (including gain) can be changed dynamically if they reside in RAM.
; If, for any stage, the gain coefficient g >= 0x20000000 then it is treated as a pointer to a
; gain coefficent in RAM rather than as an actual gain coefficient. This allows the whole coefficient
; structure to reside in flash except for a few gain coefficients that need to be changed dynamically.
;
; The following filter coefficient structure is the default filter and shows where
; to insert the optional second filter. It has five filter stages and a reasonable tradeoff between
; runtime and frequency response.
;
; int32_t aBqCoeffs[] = {
;    166,     0, -1024, -1356,   342,     // DC-notch, halfband LP filter
;    200,   789,   934,  -994,   508,
;    538,   381,   944,  -519,   722,
;    732,   124,   987,  -386,   886,
;    763,    11,  1014,  -386,   886,
;    // Terminate first filter
;    0,
;    // Optional second filter of any order N. For example:
;    //1147, -1516,   522, -1699,   708, // +5dB, F0=500 Hz peak filter
;    // Terminate second filter (and end pdm2pcm function)
;    0
; };
;
; The following filter coefficients yield a flatter frequency response in the passband
; at the cost of an extra filter stage and thus more computational requirements.
; int32_t aBqCoeffs[] = {
;    // First filter H(z)=H1(z)*H2(z)*H3(z)*H4(z)*H5(z)*H6(z) [any order M supported]
;       345,   121,  1024,  -209,   919,  // H1(z): g, b1, b2, a1, a2
;       407,   765,  1024,  -376,   470,  // H2(z): g, b1, b2, a1, a2
;       392,     0, -1024, -1265,   245,  // H3(z): g, b1, b2, a1, a2
;       450,  1345,  1024,  -495,   269,  // H4(z): g, b1, b2, a1, a2
;       507,   400,  1024,  -296,   690,  // H5(z): g, b1, b2, a1, a2
;       554,   209,  1024,  -240,   870,  // H6(z): g, b1, b2, a1, a2
;    // Terminate first filter
;    0,
;    // Optional second filter of any order N. For example:
;    //1147, -1516,   522, -1699,   708, // +5dB, F0=500 Hz peak filter
;    // Terminate second filter (and end pdm2pcm function)
;    0
; };
;
; The size of the state struct pointed to by pState is 4*(6+2N) bytes, where N is the
; total number of biquads in pNqCoeffs.
pdm2pcm8k:
    .asmfunc
    ; Save some arguments, clobbered registers and return address
    PUSH    {R0-R11, LR}

    ; ### FIRST STAGE (1.021 MHz --> 32 kHz) ###
    ; **** 4th order decimate-by-32 CIC ****
    ; Register usage is:
    ; R0:      pIn
    ; R1:      pState / byte read
    ; R2:      temp0
    ; R3:      temp1
    ; R4:      Acc0 (stored in state)
    ; R5:      Acc1 (stored in state)
    ; R6:      Acc2 (stored in state)
    ; R7:      Acc3
    ; R8:      Diff2 (stored in state)
    ; R9:      Diff1 (stored in state)
    ; R10:     Diff0 (stored in state)
    ; R11:     pOut
    ; R12:     nOutSamples
    ; LR/R14:  pLut

    MOV     R11, R0                     ; R11: pOut is copy of pIn [perform in place]
    MOV     R12, #32*2                  ; R12: nOutSamples
    LDR     R14, cicOrd4Bits8Pad0LutLoc8

    ; Get accumulator/differentiator state
    LDM     R1, {R4-R6, R8-R10}

    ; Preload first data byte
    .if $$defined("BITSTREAM_BE")
        LDRB    R1, [R0], #1            ; Read in 8b of bitstream
    .elseif $$defined("BITSTREAM_LE16")
        LDRB    R1, [R0, #1]            ; Read in 8b of bitstream
    .else
        .emsg "Undefined bitstream input byte ordering"
    .endif

p2p8k_cic_SAMPLELOOP:
    ; To save two state variables and one differentiation we reset the innermost
    ; accumulator at the start of each decimation period
    MOV     R7, #0
p2p8k_cic_BYTELOOP:           ; Byte loop is 2*20 cycles
    .loop 64/8/2
        ; Update accumulator values for 8 clocks
        ; (disregarding input bits which we can, thanks to superposition, handle below)
        ADD     R7, R7, R4, LSL #7
        SUB     R7, R7, R4, LSL #3          ; Acc3 += 120*Acc0
        ADD     R7, R7, R5, LSL #5
        ADD     R7, R7, R5, LSL #2          ; Acc3 += 36*Acc1
        ADD     R7, R7, R6, LSL #3          ; Acc3 += 8*Acc2
        ADD     R6, R6, R4, LSL #5
        ADD     R6, R6, R4, LSL #2          ; Acc2 += 36*Acc0
        ADD     R6, R6, R5, LSL #3          ; Acc2 += 8*Acc1
        ADD     R5, R5, R4, LSL #3          ; Acc1 += 8*Acc0

        ; Read in 8b of bitstream input, use LUT, and add effect to each accumulator
        LDR     R2, [R14, R1, LSL #2]       ; R2: LUT entry for 8b of bitstream
        .if $$defined("BITSTREAM_BE")
            LDRB    R1, [R0], #1            ; Read in 8b of bitstream
        .elseif $$defined("BITSTREAM_LE16")
           LDRB    R1, [R0], #2             ; Read in 8b of bitstream
        .endif
        UBFX    R3, R2, #0, #4
        ADD     R4, R3                      ; Acc0 += LUT0
        UBFX    R3, R2, #4, #6
        ADD     R5, R3                      ; Acc1 += LUT1
        UBFX    R3, R2, #10, #7
        ADD     R6, R3                      ; Acc2 += LUT2
        UBFX    R3, R2, #17, #9
        ADD     R7, R3                      ; Acc3 += LUT3

        ; Update accumulator values for 8 clocks
        ; (disregarding input bits which we can, thanks to superposition, handle below)
        ADD     R7, R7, R4, LSL #7
        SUB     R7, R7, R4, LSL #3          ; Acc3 += 120*Acc0
        ADD     R7, R7, R5, LSL #5
        ADD     R7, R7, R5, LSL #2          ; Acc3 += 36*Acc1
        ADD     R7, R7, R6, LSL #3          ; Acc3 += 8*Acc2
        ADD     R6, R6, R4, LSL #5
        ADD     R6, R6, R4, LSL #2          ; Acc2 += 36*Acc0
        ADD     R6, R6, R5, LSL #3          ; Acc2 += 8*Acc1
        ADD     R5, R5, R4, LSL #3          ; Acc1 += 8*Acc0

        ; Read in 8b of bitstream input, use LUT, and add effect to each accumulator
        LDR     R2, [R14, R1, LSL #2]       ; R2: LUT entry for 8b of bitstream
        .if $$defined("BITSTREAM_BE")
            LDRB    R1, [R0], #1            ; Read in 8b of bitstream
        .elseif $$defined("BITSTREAM_LE16")
            LDRB    R1, [R0, #1]            ; Read in 8b of bitstream
        .endif
        UBFX    R3, R2, #0, #4
        ADD     R4, R3                      ; Acc0 += LUT0
        UBFX    R3, R2, #4, #6
        ADD     R5, R3                      ; Acc1 += LUT1
        UBFX    R3, R2, #10, #7
        ADD     R6, R3                      ; Acc2 += LUT2
        UBFX    R3, R2, #17, #9
        ADD     R7, R3                      ; Acc3 += LUT3
    .endloop

    ; End of decimation period, perform differentiation, output sample and see if we do more
    SUB     R3, R7, R8                      ; R3: Diff2out = Diff3out - DIff2 = Acc3 - Diff2
    MOV     R8, R7                          ; Diff2' = Diff3out = Acc3
    SUB     R2, R3, R9                      ; R2: Diff1out = Diff2out - Diff1
    MOV     R9, R3                          ; Diff1' = Diff2out
    SUB     R3, R2, R10                     ; R3: Diff0out = Diff1out - Diff0
    MOV     R10, R2                         ; Diff0' = Diff1out
    SUBS    R12, R12, #1                    ; nOutSamples--
    ASR     R3, #4                          ; Scale to 12 dB gain
    SUB     R3, R3, #0x80000                ; Convert to 2s complement and SI3.F17 format (with 12 dB gain)
    STR     R3, [R11], #4
    BNE     p2p8k_cic_SAMPLELOOP

p2p8k_cic_EXIT:
    ; Get pointers to: R0: pIn/intermediateBuf, R1: pState, R2: pBqCoeffs
    POP     {R0-R2}
    ; Store accumulator/differentiator state
    STM     R1!, {R4-R6, R8-R10}
    ; p2pBiQuad expects intermediate buffer pointer on top of stack
    PUSH    {R0}
    B       p2pBiQuad
    .endasmfunc

cicOrd4Bits8Pad0LutLoc8:
    .word cicOrd4Bits8Pad0Lut

    .sect ".text:pdm2pcm_2nd_stage"
    .thumb
    ; ### SECOND STAGE (@Fsout*2) ###
    ; **** A N-stage (typically 5) biquad IIR filter that is used to ****
    ; **** remove out-of band signals and noise before the final     ****
    ; **** stage of decimation. Performed at 32 kHz.                 ****
    ; Register usage is:
    ; R0:      pInOut                      [clobbered]
    ; R1:      pState                      [in/out]
    ; R2:      pBqCoeffs                   [in/out]
    ; R3:      temp0 / input value         [clobbered]
    ; R4:      temp1                       [clobbered]
    ; R5:      StateNm1 (stored in state)  [clobbered]
    ; R6:      StateNm2 (stored in state)  [clobbered]
    ; R7:      g (read from pBqCoeffs)     [clobbered]
    ; R8:      b1 (read from pBqCoeffs)    [clobbered]
    ; R9:      b2 (read from pBqCoeffs)    [clobbered]
    ; R10:     a1 (read from pBqCoeffs)    [clobbered]
    ; R11:     a2 (read from pBqCoeffs)    [clobbered]
    ; R12:     nOutSamples                 [clobbered]
    ; LR/R14:  Y (output value)            [clobbered]
p2pBiQuad:
    .asmfunc
    ; Expects SP to point to the intermediate buffer (i.e. be on top of stack)
p2pBiQuadCascade1_SECTION_LOOP:
    ; Load state and coefficients for this second-order section
    LDM     R1, {R5,R6}
    LDM     R2!, {R7-R11}
    ; Check for end of IIR filter
    CBNZ    R7, p2pBiQuadCascade1_MORE_SECTIONS
    SUB     R2, R2, #4*4            ; pBqCoeffs adjusted for (possible) next section
    B       p2pBiQuadCascade1_EXIT

p2pBiQuadCascade1_MORE_SECTIONS:
    ; Check whether gain coefficient is a coefficient or pointer to coefficient in RAM
    TST     R7, #0xE1000000
    BEQ     p2pBiQuadCascade1_GAIN_LOADED
    LDR     R7, [R7]

p2pBiQuadCascade1_GAIN_LOADED:
    ; Reset in/out pointer to start of intermediate buffer for each section
    LDR     R0, [SP]                  ; R0: intermediate buffer
    MOV     R12, #64/8                ; R12: loop counter

p2pBiQuadCascade1_SAMPLE_LOOP:
    SUBS    R12, R12, #1
    .loop 8
      LDR     R3, [R0]                ; R3: X
      MUL     R3, R3, R7
      ASR     R3, R3, #10             ; R3: g*X
      ADD     R14, R5, R3             ; R14: g*X+S[-1] = Y
      MUL     R4, R3, R8              ; R4: g*b1*X (<<10)
      MUL     R5, R14, R10            ; R5: a1*Y (<<10)
      SUB     R5, R6, R5, ASR #10
      ADD     R5, R5, R4, ASR #10     ; R5: S[-2] + g*b1*X - a1*Y = S[0] (S[-1] next iteration)
      MUL     R4, R3, R9              ; R4: g*b2*X (<<10)
      MUL     R3, R14, R11            ; R3: a2*Y (<<10)
      ASR     R4, R4, #10
      SUB     R6, R4, R3, ASR #10     ; R6: g*b2*X - a2*Y = S[-1] (S[-2] next iteration)
      STR     R14, [R0], #4           ; Store Y
    .endloop

    BNE     p2pBiQuadCascade1_SAMPLE_LOOP

    ; Store state for this second-order section
    STM     R1!, {R5,R6}
    B       p2pBiQuadCascade1_SECTION_LOOP

p2pBiQuadCascade1_EXIT:


    ; ### THIRD STAGE (Fsout*2 -> Fsout) ###
    ; **** 1st order decimate-by-2 CIC to go to final sample rate    ****
    ; Register usage is:
    ; R0:      pIn
    ; R3:      pOut
    ; R4:      temp0
    ; R5:      temp1
    ; R6:      nOutSamples
    LDR     R0, [SP]                  ; R0: intermediate buffer
    MOV     R3, R0
    MOVS    R6, #32
p2p16kbq_cic2_SAMPLE_LOOP:
    SUBS    R6, R6, #1
    LDM     R0!, {R4-R5}
    ADD     R4, R4, R5
    ASR     R4, R4, #1
    STR     R4, [R3], #4
    BNE     p2p16kbq_cic2_SAMPLE_LOOP



    ; ### FOURTH STAGE (@Fsout) ###
    ; **** A user-defined N-stage biquad IIR filter that can be      ****
    ; **** skipped (first gain coefficient is 0) or used to implement****
    ; **** for example EQ                                            ****
    ; **** Nth order biquad IIR filter ****
    ; Register usage is:
    ; R0:      pInOut                      [clobbered]
    ; R1:      pState                      [in/out]
    ; R2:      pBqCoeffs                   [in/out]
    ; R3:      temp0 / input value         [clobbered]
    ; R4:      temp1                       [clobbered]
    ; R5:      StateNm1 (stored in state)  [clobbered]
    ; R6:      StateNm2 (stored in state)  [clobbered]
    ; R7:      g (read from pBqCoeffs)     [clobbered]
    ; R8:      b1 (read from pBqCoeffs)    [clobbered]
    ; R9:      b2 (read from pBqCoeffs)    [clobbered]
    ; R10:     a1 (read from pBqCoeffs)    [clobbered]
    ; R11:     a2 (read from pBqCoeffs)    [clobbered]
    ; R12:     nOutSamples                 [clobbered]
    ; LR/R14:  Y (output value)            [clobbered]
p2pBiQuadCascade2_SECTION_LOOP:
    ; Load state and coefficients for this second-order section
    LDM     R1, {R5,R6}
    LDM     R2!, {R7-R11}
    ; Check for end of IIR filter
    CBNZ    R7, p2pBiQuadCascade2_MORE_SECTIONS
    B       p2pBiQuadCascade2_EXIT

p2pBiQuadCascade2_MORE_SECTIONS:
    ; Check whether gain coefficient is a coefficient or pointer to coefficient in RAM
    TST     R7, #0xE1000000
    BEQ     p2pBiQuadCascade2_GAIN_LOADED
    LDR     R7, [R7]

p2pBiQuadCascade2_GAIN_LOADED:    ; Reset in/out pointer to start of intermediate buffer for each section
    LDR     R0, [SP]                  ; R0: intermediate buffer
    MOV     R12, #32/8                ; R12: loop counter

p2pBiQuadCascade2_SAMPLE_LOOP:
    SUBS    R12, R12, #1
    .loop 8
      LDR     R3, [R0]                ; R3: X
      MUL     R3, R3, R7
      ASR     R3, R3, #10             ; R3: g*X
      ADD     R14, R5, R3             ; R14: g*X+S[-1] = Y
      MUL     R4, R3, R8              ; R4: g*b1*X (<<10)
      MUL     R5, R14, R10            ; R5: a1*Y (<<10)
      SUB     R5, R6, R5, ASR #10
      ADD     R5, R5, R4, ASR #10     ; R5: S[-2] + g*b1*X - a1*Y = S[0] (S[-1] next iteration)
      MUL     R4, R3, R9              ; R4: g*b2*X (<<10)
      MUL     R3, R14, R11            ; R3: a2*Y (<<10)
      ASR     R4, R4, #10
      SUB     R6, R4, R3, ASR #10     ; R6: g*b2*X - a2*Y = S[-1] (S[-2] next iteration)
      STR     R14, [R0], #4           ; Store Y
    .endloop

    BNE     p2pBiQuadCascade2_SAMPLE_LOOP

    ; Store state for this second-order section
    STM     R1!, {R5,R6}
    B       p2pBiQuadCascade2_SECTION_LOOP

p2pBiQuadCascade2_EXIT:



    ; ### FIFTH STAGE (@Fsout) ###
    ; **** Convert samples to S.F15 format, saturate and output ****
    ; Register usage is:
    ; R0:      pIn
    ; R1:      pOut
    ; R2-R5:   temp
    ; R6:      nOutSamples
    MOV     R0, #0
    MSR     APSR_nzcvq, R0          ; Clear Q flag in APSR.
    POP     {R0,R1}                 ; R0: intermediate buffer, R1: pOut
    MOVS    R6, #32/4
p2pfs_copy_SAMPLE_LOOP:
    LDM     R0!, {R2-R5}
    SSAT    R2, #16, R2, ASR #3      ; Output format S.F15 (saturated)
    SSAT    R3, #16, R3, ASR #3      ; Output format S.F15 (saturated)
    SSAT    R4, #16, R4, ASR #3      ; Output format S.F15 (saturated)
    SSAT    R5, #16, R5, ASR #3      ; Output format S.F15 (saturated)
    SUBS    R6, R6, #1
    STRH    R2, [R1], #2
    STRH    R3, [R1], #2
    STRH    R4, [R1], #2
    STRH    R5, [R1], #2
    BNE     p2pfs_copy_SAMPLE_LOOP

    ; Extract Q flag (saturation) and return to caller
    MRS     R0, APSR
    UBFX    R0, R0, #27, #1

p2pfs_RETURN:
    ; Restore clobbered registers and return (R0-R3 already popped)
    POP     {R4-R11, PC}

;   LTORG compiler directive does not exist for the TI compiler. The TI compiler places the immediates below in the flash addresses following the code by default, since they are in the same section.

    .endasmfunc


    .sect ".text:pdm2pcm_lut"

; LUT for order 4 CIC decimator. The LUT index corresponds to 8 bits of
; input, MSB to LSB (followed by 0 zero bits). Each LUT entry contains a
; packed word that contains the increment value to apply to each accumulator
; for the given input word:
;    [ 3: 0] 4 bit increment value for ACC0 (max val 8)
;    [ 9: 4] 6 bit increment value for ACC1 (max val 36)
;    [16:10] 7 bit increment value for ACC2 (max val 120)
;    [25:17] 9 bit increment value for ACC3 (max val 330)
; In addition each accumulator needs to be incremented based on the other
; accumulator values so the whole operation becomes:
;    ACC3 +=   120*ACC0 +    36*ACC1 +     8*ACC2 +       LUT[x][25:17]
;    ACC2 +=    36*ACC0 +     8*ACC1 +       LUT[x][16:10]
;    ACC1 +=     8*ACC0 +       LUT[x][ 9: 4]
;    ACC0 +=       LUT[x][ 3: 0]

cicOrd4Bits8Pad0Lut:
    ;These values are const and go in flash.
    .align 4
    .word 0x00000000, 0x00020411, 0x00080C21, 0x000A1032, 0x00141831, 0x00161C42, 0x001C2452, 0x001E2863 ; [   0]
    .word 0x00282841, 0x002A2C52, 0x00303462, 0x00323873, 0x003C4072, 0x003E4483, 0x00444C93, 0x004650A4 ; [   8]
    .word 0x00463C51, 0x00484062, 0x004E4872, 0x00504C83, 0x005A5482, 0x005C5893, 0x006260A3, 0x006464B4 ; [  16]
    .word 0x006E6492, 0x007068A3, 0x007670B3, 0x007874C4, 0x00827CC3, 0x008480D4, 0x008A88E4, 0x008C8CF5 ; [  24]
    .word 0x00705461, 0x00725872, 0x00786082, 0x007A6493, 0x00846C92, 0x008670A3, 0x008C78B3, 0x008E7CC4 ; [  32]
    .word 0x00987CA2, 0x009A80B3, 0x00A088C3, 0x00A28CD4, 0x00AC94D3, 0x00AE98E4, 0x00B4A0F4, 0x00B6A505 ; [  40]
    .word 0x00B690B2, 0x00B894C3, 0x00BE9CD3, 0x00C0A0E4, 0x00CAA8E3, 0x00CCACF4, 0x00D2B504, 0x00D4B915 ; [  48]
    .word 0x00DEB8F3, 0x00E0BD04, 0x00E6C514, 0x00E8C925, 0x00F2D124, 0x00F4D535, 0x00FADD45, 0x00FCE156 ; [  56]
    .word 0x00A87071, 0x00AA7482, 0x00B07C92, 0x00B280A3, 0x00BC88A2, 0x00BE8CB3, 0x00C494C3, 0x00C698D4 ; [  64]
    .word 0x00D098B2, 0x00D29CC3, 0x00D8A4D3, 0x00DAA8E4, 0x00E4B0E3, 0x00E6B4F4, 0x00ECBD04, 0x00EEC115 ; [  72]
    .word 0x00EEACC2, 0x00F0B0D3, 0x00F6B8E3, 0x00F8BCF4, 0x0102C4F3, 0x0104C904, 0x010AD114, 0x010CD525 ; [  80]
    .word 0x0116D503, 0x0118D914, 0x011EE124, 0x0120E535, 0x012AED34, 0x012CF145, 0x0132F955, 0x0134FD66 ; [  88]
    .word 0x0118C4D2, 0x011AC8E3, 0x0120D0F3, 0x0122D504, 0x012CDD03, 0x012EE114, 0x0134E924, 0x0136ED35 ; [  96]
    .word 0x0140ED13, 0x0142F124, 0x0148F934, 0x014AFD45, 0x01550544, 0x01570955, 0x015D1165, 0x015F1576 ; [ 104]
    .word 0x015F0123, 0x01610534, 0x01670D44, 0x01691155, 0x01731954, 0x01751D65, 0x017B2575, 0x017D2986 ; [ 112]
    .word 0x01872964, 0x01892D75, 0x018F3585, 0x01913996, 0x019B4195, 0x019D45A6, 0x01A34DB6, 0x01A551C7 ; [ 120]
    .word 0x00F09081, 0x00F29492, 0x00F89CA2, 0x00FAA0B3, 0x0104A8B2, 0x0106ACC3, 0x010CB4D3, 0x010EB8E4 ; [ 128]
    .word 0x0118B8C2, 0x011ABCD3, 0x0120C4E3, 0x0122C8F4, 0x012CD0F3, 0x012ED504, 0x0134DD14, 0x0136E125 ; [ 136]
    .word 0x0136CCD2, 0x0138D0E3, 0x013ED8F3, 0x0140DD04, 0x014AE503, 0x014CE914, 0x0152F124, 0x0154F535 ; [ 144]
    .word 0x015EF513, 0x0160F924, 0x01670134, 0x01690545, 0x01730D44, 0x01751155, 0x017B1965, 0x017D1D76 ; [ 152]
    .word 0x0160E4E2, 0x0162E8F3, 0x0168F103, 0x016AF514, 0x0174FD13, 0x01770124, 0x017D0934, 0x017F0D45 ; [ 160]
    .word 0x01890D23, 0x018B1134, 0x01911944, 0x01931D55, 0x019D2554, 0x019F2965, 0x01A53175, 0x01A73586 ; [ 168]
    .word 0x01A72133, 0x01A92544, 0x01AF2D54, 0x01B13165, 0x01BB3964, 0x01BD3D75, 0x01C34585, 0x01C54996 ; [ 176]
    .word 0x01CF4974, 0x01D14D85, 0x01D75595, 0x01D959A6, 0x01E361A5, 0x01E565B6, 0x01EB6DC6, 0x01ED71D7 ; [ 184]
    .word 0x019900F2, 0x019B0503, 0x01A10D13, 0x01A31124, 0x01AD1923, 0x01AF1D34, 0x01B52544, 0x01B72955 ; [ 192]
    .word 0x01C12933, 0x01C32D44, 0x01C93554, 0x01CB3965, 0x01D54164, 0x01D74575, 0x01DD4D85, 0x01DF5196 ; [ 200]
    .word 0x01DF3D43, 0x01E14154, 0x01E74964, 0x01E94D75, 0x01F35574, 0x01F55985, 0x01FB6195, 0x01FD65A6 ; [ 208]
    .word 0x02076584, 0x02096995, 0x020F71A5, 0x021175B6, 0x021B7DB5, 0x021D81C6, 0x022389D6, 0x02258DE7 ; [ 216]
    .word 0x02095553, 0x020B5964, 0x02116174, 0x02136585, 0x021D6D84, 0x021F7195, 0x022579A5, 0x02277DB6 ; [ 224]
    .word 0x02317D94, 0x023381A5, 0x023989B5, 0x023B8DC6, 0x024595C5, 0x024799D6, 0x024DA1E6, 0x024FA5F7 ; [ 232]
    .word 0x024F91A4, 0x025195B5, 0x02579DC5, 0x0259A1D6, 0x0263A9D5, 0x0265ADE6, 0x026BB5F6, 0x026DBA07 ; [ 240]
    .word 0x0277B9E5, 0x0279BDF6, 0x027FC606, 0x0281CA17, 0x028BD216, 0x028DD627, 0x0293DE37, 0x0295E248 ; [ 248]


.end
