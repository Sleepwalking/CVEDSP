/*
 * FFTBlock.h
 * Copyright (C) 2013 Sleepwalking
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Sleepwalking'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 
 * CVEDSP IS PROVIDED BY Sleepwalking ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Sleepwalking OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef FFTBLOCK_H
#define FFTBLOCK_H
#include "FFTCommon.h"
/*
#define FFTBlock9() \
    StaticFFTExecTable[5]();\
    SFRe += 256; SFIm += 256;\
    StaticFFTExecTable[4]();\
    SFRe += 128; SFIm += 128;\
    StaticFFTExecTable[4]();\
    SFRe -= 384; SFIm -= 384;\
    SplitRadix_RadixL(SFRe, SFIm, 0, 512);
*/

#define FFTBlock10() \
    StaticFFTExecTable[6]();\
    SFRe += 512; SFIm += 512;\
    StaticFFTExecTable[5]();\
    SFRe += 256; SFIm += 256;\
    StaticFFTExecTable[5]();\
    SFRe -= 768; SFIm -= 768;\
    StaticRadix_10(SFRe, SFIm);
/*
#define FFTBlock11() \
    FFTBlock10();\
    SFRe += 1024; SFIm += 1024;\
    FFTBlock9();\
    SFRe += 512; SFIm += 512;\
    FFTBlock9();\
    SFRe -= 1536; SFIm -= 1536;\
    StaticRadix_11(SFRe, SFIm);
*/
#define FFTBlock11() \
    FFTBlock10();\
    SFRe += 1024; SFIm += 1024;\
    StaticFFTExecTable[6]();\
    SFRe += 512; SFIm += 512;\
    StaticFFTExecTable[6]();\
    SFRe -= 1536; SFIm -= 1536;\
    StaticRadix_11(SFRe, SFIm);

#define FFTBlock12() \
    FFTBlock11();\
    SFRe += 2048; SFIm += 2048;\
    FFTBlock10();\
    SFRe += 1024; SFIm += 1024;\
    FFTBlock10();\
    SFRe -= 3072; SFIm -= 3072;\
    SplitRadix_RadixL(SFRe, SFIm, 0, 4096)

#define FFTBlock13() \
    FFTBlock12();\
    SFRe += 4096; SFIm += 4096;\
    FFTBlock11();\
    SFRe += 2048; SFIm += 2048;\
    FFTBlock11();\
    SFRe -= 6144; SFIm -= 6144;\
    SplitRadix_RadixL(SFRe, SFIm, 0, 8192)

#define FFTBlock14() \
    FFTBlock13();\
    SFRe += 8192; SFIm += 8192;\
    FFTBlock12();\
    SFRe += 4096; SFIm += 4096;\
    FFTBlock12();\
    SFRe -= 12288; SFIm -= 12288;\
    SplitRadix_RadixL(SFRe, SFIm, 0, 16384)

#endif

