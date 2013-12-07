/*
 * TDEffects.c
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
#include "TDEffects.h"
#include "IntrinUtil/FloatArray.h"
#include "IntrinUtil/Calculation.h"
#include <xmmintrin.h>
#ifdef __AVX__
#include <immintrin.h>
#endif

void MovingAverage(float* Dest, float* Src, float Strength, int AvgLength, int Length)
{
	int i, half;
	float Sum = 0;
	float AvgFactor = 1.0f / ((float)AvgLength + 1.0f);
	half = AvgLength / 2;
	for(i = 0;i < Length;i ++)
	{
		if(i >= half)
			Sum -= Src[i - half];
		if(i < Length - half)
			Sum += Src[i + half];
		Dest[i] = Src[i] * (1.0f - Strength) + Sum * AvgFactor * Strength;
	}
}
void MinimizeOffset(float* Dest, float* Src, int AvgLength, int Length)
{
	MovingAverage(Dest, Src, 1, AvgLength, Length);
	Boost_FloatSubArr(Dest, Src, Dest, Length);
}
#ifndef __AVX__
//SSE
void Differentiate(float* Dest, float* Src, float Length)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3;
	for(i = 0;i < Length - 4;i += 4)
	{
		xmm0 = _mm_loadu_ps(Src + i + 1);
		xmm1 = _mm_loadu_ps(Src + i);
		xmm2 = _mm_loadu_ps(Src + i + 5);
		xmm3 = _mm_loadu_ps(Src + i + 4);

		xmm0 = _mm_sub_ps(xmm0, xmm1);
		xmm2 = _mm_sub_ps(xmm2, xmm3);

		_mm_storeu_ps(Dest + i, xmm0);
		_mm_storeu_ps(Dest + i + 4, xmm2);
	}
	for(; i < Length - 1;i ++)
		Dest[i] = Src[i + 1] - Src[i];
}
#else
//AVX
void Differentiate(float* Dest, float* Src, float Length)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3;
	for(i = 0;i < Length - 8;i += 8)
	{
		xmm0 = _mm256_loadu_ps(Src + i + 1);
		xmm1 = _mm256_loadu_ps(Src + i);
		xmm2 = _mm256_loadu_ps(Src + i + 9);
		xmm3 = _mm256_loadu_ps(Src + i + 8);

		xmm0 = _mm256_sub_ps(xmm0, xmm1);
		xmm2 = _mm256_sub_ps(xmm2, xmm3);

		_mm256_storeu_ps(Dest + i, xmm0);
		_mm256_storeu_ps(Dest + i + 8, xmm2);
	}
	for(; i < Length - 1;i ++)
		Dest[i] = Src[i + 1] - Src[i];
}
#endif
void Integrate(float* Dest, float* Src, float Length)
{
	int i;
	Dest[0] = 0;
	for(i = 0;i < Length -1;i ++)
		Dest[i + 1] = Dest[i] + Src[i];
}
