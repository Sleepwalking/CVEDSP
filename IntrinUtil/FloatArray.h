/*
 * FloatArray.h
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
#ifndef FLOATARRAY_H
#define FLOATARRAY_H
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

extern void Boost_FloatCopy(float* Dest, float* Src, int Amount);

extern void Boost_FloatInv(float* Dest, float* Src, int Amount);
extern void Boost_FloatSqr(float* Dest, float* Src, int Amount);
extern void Boost_FloatRcp(float* Dest, float* Src, int Amount);
extern void Boost_FloatRSqr(float* Dest, float* Src, int Amount);
extern void Boost_FloatAbs(float* Dest, float* Src, int Amount);

extern void Boost_FloatSet(float* Dest, float Data, int Amount);
extern void Boost_FloatMaxVal(float* Dest, float* Src, float Data, int Amount);
extern void Boost_FloatMinVal(float* Dest, float* Src, float Data, int Amount);

extern void Boost_FloatAdd(float* Dest, float* Src, float Data, int Amount);
extern void Boost_FloatSub(float* Dest, float* Src, float Data, int Amount);
extern void Boost_FloatMul(float* Dest, float* Src, float Data, int Amount);
extern void Boost_FloatDiv(float* Dest, float* Src, float Data, int Amount);

extern void Boost_FloatSubBy(float* Dest, float* Src, float Data, int Amount);
extern void Boost_FloatDivBy(float* Dest, float* Src, float Data, int Amount);

extern void Boost_FloatAddArr(float* Dest, float* Data1, float* Data2, int Amount);
extern void Boost_FloatSubArr(float* Dest, float* Data1, float* Data2, int Amount);
extern void Boost_FloatMulArr(float* Dest, float* Data1, float* Data2, int Amount);
extern void Boost_FloatDivArr(float* Dest, float* Data1, float* Data2, int Amount);

extern void Boost_FloatMaxArr(float* Dest, float* Data1, float* Data2, int Amount);
extern void Boost_FloatMinArr(float* Dest, float* Data1, float* Data2, int Amount);

extern void Boost_FloatComplexMulArr(float* DestRe, float* DestIm,
                                     float* Re1, float* Im1,
                                     float* Re2, float* Im2, int Amount);

extern float Boost_FloatSum(float* Src, int Amount);
extern float Boost_FloatAvg(float* Src, int Amount);
extern float Boost_FloatMax(float* Src, int From, int To);
extern float Boost_FloatMin(float* Src, int From, int To);

//Does not support 64-bit environment.
#if UINTPTR_MAX == 0xffffffff
	extern void Boost_IntCFloat(float* Dest, int32_t* Src, int Amount);
#endif

#ifdef __cplusplus
}
#endif

#endif
