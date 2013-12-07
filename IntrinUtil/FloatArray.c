/*
 * FloatArray.c
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
#include "FloatArray.h"
#include <math.h>
#include <xmmintrin.h>

#define Boost_Float_Fetch2_SSE() \
    xmm0 = _mm_loadu_ps(Src + i);\
    xmm1 = _mm_loadu_ps(Src + i + 4)

#define Boost_Float_Fetch4_SSE() \
    xmm0 = _mm_loadu_ps(Src + i);\
    xmm1 = _mm_loadu_ps(Src + i + 4);\
    xmm2 = _mm_loadu_ps(Src + i + 8);\
    xmm3 = _mm_loadu_ps(Src + i + 12)

#define Boost_Float_Fetch4_1_SSE() \
    xmm1 = _mm_loadu_ps(Src + i);\
    xmm2 = _mm_loadu_ps(Src + i + 4);\
    xmm3 = _mm_loadu_ps(Src + i + 8);\
    xmm4 = _mm_loadu_ps(Src + i + 12)

#define Boost_Float_Fetch8_SSE() \
    xmm0 = _mm_loadu_ps(Src + i);\
    xmm1 = _mm_loadu_ps(Src + i + 4);\
    xmm2 = _mm_loadu_ps(Src + i + 8);\
    xmm3 = _mm_loadu_ps(Src + i + 12);\
    xmm4 = _mm_loadu_ps(Src + i + 16);\
    xmm5 = _mm_loadu_ps(Src + i + 20);\
    xmm6 = _mm_loadu_ps(Src + i + 24);\
    xmm7 = _mm_loadu_ps(Src + i + 28)

#define Boost_Float_Store2_SSE() \
    _mm_storeu_ps(Dest + i, xmm0);\
    _mm_storeu_ps(Dest + i + 4, xmm1)

#define Boost_Float_Store4_SSE() \
    _mm_storeu_ps(Dest + i, xmm0);\
    _mm_storeu_ps(Dest + i + 4, xmm1);\
    _mm_storeu_ps(Dest + i + 8, xmm2);\
    _mm_storeu_ps(Dest + i + 12, xmm3);

#define Boost_Float_Store4_1_SSE() \
    _mm_storeu_ps(Dest + i, xmm1);\
    _mm_storeu_ps(Dest + i + 4, xmm2);\
    _mm_storeu_ps(Dest + i + 8, xmm3);\
    _mm_storeu_ps(Dest + i + 12, xmm4);

#define Boost_Float_Store8_SSE() \
    _mm_storeu_ps(Dest + i, xmm0);\
    _mm_storeu_ps(Dest + i + 4, xmm1);\
    _mm_storeu_ps(Dest + i + 8, xmm2);\
    _mm_storeu_ps(Dest + i + 12, xmm3);\
    _mm_storeu_ps(Dest + i + 16, xmm4);\
    _mm_storeu_ps(Dest + i + 20, xmm5);\
    _mm_storeu_ps(Dest + i + 24, xmm6);\
    _mm_storeu_ps(Dest + i + 28, xmm7);

#ifdef __AVX__
#include <immintrin.h>
#endif

#ifndef __AVX__

//---------------------------SSE-------------------------------
void Boost_FloatCopy(float* Dest, float* Src, int Amount)
{
	int i;
	__m128 xmm0, xmm1;
    for(i = 0; i < Amount - 7; i += 8)
	{
        Boost_Float_Fetch2_SSE();

        Boost_Float_Store2_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Src[i];
}
void Boost_FloatInv(float* Dest, float* Src, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm_setzero_ps();
    for(i = 0; i < Amount - 15; i += 16)
    {
        Boost_Float_Fetch4_1_SSE();
		
		xmm1 = _mm_sub_ps(xmm0, xmm1);
		xmm2 = _mm_sub_ps(xmm0, xmm2);
		xmm3 = _mm_sub_ps(xmm0, xmm3);
		xmm4 = _mm_sub_ps(xmm0, xmm4);
		
        Boost_Float_Store4_1_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = - Src[i];
}
void Boost_FloatSqr(float* Dest, float* Src, int Amount)
{
	int i;
	__m128 xmm0;
    for(i = 0; i < Amount - 3; i += 4)
	{
		xmm0 = _mm_loadu_ps(Src + i);
		xmm0 = _mm_sqrt_ps(xmm0);
		_mm_storeu_ps(Dest + i, xmm0);
	}
    for(; i < Amount; i ++)
		Dest[i] = sqrt(Src[i]);
}
void Boost_FloatRcp(float* Dest, float* Src, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3;
    for(i = 0; i < Amount - 15; i += 16)
	{
        Boost_Float_Fetch4_SSE();
		
		xmm0 = _mm_rcp_ps(xmm0);
		xmm1 = _mm_rcp_ps(xmm1);
		xmm2 = _mm_rcp_ps(xmm2);
        xmm3 = _mm_rcp_ps(xmm3);

        Boost_Float_Store4_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = 1.0f / Src[i];
}
void Boost_FloatRSqr(float* Dest, float* Src, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3;
    for(i = 0; i < Amount - 15; i += 16)
    {
        Boost_Float_Fetch4_SSE();
		
		xmm1 = _mm_rsqrt_ps(xmm0);
		xmm1 = _mm_rsqrt_ps(xmm1);
		xmm2 = _mm_rsqrt_ps(xmm2);
		xmm3 = _mm_rsqrt_ps(xmm3);

        Boost_Float_Store4_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = 1.0f / sqrt(Src[i]);
}
void Boost_FloatAbs(float* Dest, float* Src, int Amount)
{
	int i;
    __m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6;
    xmm6 = _mm_setzero_ps();
    for(i = 0; i < Amount - 7; i += 8)
	{
        Boost_Float_Fetch2_SSE();

        xmm2 = _mm_min_ps(xmm0, xmm6);
        xmm3 = _mm_min_ps(xmm1, xmm6);

        xmm4 = _mm_max_ps(xmm0, xmm6);
        xmm5 = _mm_max_ps(xmm1, xmm6);

		xmm0 = _mm_sub_ps(xmm4, xmm2);
		xmm1 = _mm_sub_ps(xmm5, xmm3);

        Boost_Float_Store2_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Src[i] > 0 ? Src[i] : - Src[i];
}

void Boost_FloatSet(float* Dest, float Data, int Amount)
{
	int i;
	__m128 xmm;
	xmm = _mm_set_ps(Data, Data, Data, Data);
    for(i = 0; i < Amount - 15; i += 16)
	{
		_mm_storeu_ps(Dest + i, xmm);
		_mm_storeu_ps(Dest + i + 4, xmm);
		_mm_storeu_ps(Dest + i + 8, xmm);
		_mm_storeu_ps(Dest + i + 12, xmm);
	}
    for(; i < Amount; i ++)
		Dest[i] = Data;
}
void Boost_FloatMaxVal(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm_set_ps(Data, Data, Data, Data);
    for(i = 0; i < Amount - 15; i += 16)
	{
        Boost_Float_Fetch4_1_SSE();

		xmm1 = _mm_max_ps(xmm0, xmm1);
		xmm2 = _mm_max_ps(xmm0, xmm2);
		xmm3 = _mm_max_ps(xmm0, xmm3);
		xmm4 = _mm_max_ps(xmm0, xmm4);
		
        Boost_Float_Store4_1_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Data < Src[i] ? Src[i] : Data;
}
void Boost_FloatMinVal(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm_set_ps(Data, Data, Data, Data);
    for(i = 0; i < Amount - 15; i += 16)
    {
        Boost_Float_Fetch4_1_SSE();

		xmm1 = _mm_min_ps(xmm0, xmm1);
		xmm2 = _mm_min_ps(xmm0, xmm2);
		xmm3 = _mm_min_ps(xmm0, xmm3);
		xmm4 = _mm_min_ps(xmm0, xmm4);

        Boost_Float_Store4_1_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Data > Src[i] ? Src[i] : Data;
}

void Boost_FloatAdd(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm_set_ps(Data, Data, Data, Data);
    for(i = 0; i < Amount - 15; i += 16)
	{
        Boost_Float_Fetch4_1_SSE();
		
		xmm1 = _mm_add_ps(xmm1, xmm0);
		xmm2 = _mm_add_ps(xmm2, xmm0);
		xmm3 = _mm_add_ps(xmm3, xmm0);
		xmm4 = _mm_add_ps(xmm4, xmm0);

        Boost_Float_Store4_1_SSE();
    }
    for(; i < Amount; i ++)
		Dest[i] = Src[i] + Data;
}
void Boost_FloatSub(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm_set_ps(Data, Data, Data, Data);
    for(i = 0; i < Amount - 15; i += 16)
    {
        Boost_Float_Fetch4_1_SSE();
		
		xmm1 = _mm_sub_ps(xmm1, xmm0);
		xmm2 = _mm_sub_ps(xmm2, xmm0);
		xmm3 = _mm_sub_ps(xmm3, xmm0);
		xmm4 = _mm_sub_ps(xmm4, xmm0);

        Boost_Float_Store4_1_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Src[i] - Data;
}
void Boost_FloatMul(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm_set_ps(Data, Data, Data, Data);
    for(i = 0; i < Amount - 15; i += 16)
    {
        Boost_Float_Fetch4_1_SSE();
		
		xmm1 = _mm_mul_ps(xmm1, xmm0);
		xmm2 = _mm_mul_ps(xmm2, xmm0);
		xmm3 = _mm_mul_ps(xmm3, xmm0);
		xmm4 = _mm_mul_ps(xmm4, xmm0);

        Boost_Float_Store4_1_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Src[i] * Data;
}

void Boost_FloatSubBy(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm_set_ps(Data, Data, Data, Data);
	for(i = 0;i < Amount - 15;i += 16)
    {
        Boost_Float_Fetch4_1_SSE();
		
		xmm1 = _mm_sub_ps(xmm0, xmm1);
		xmm2 = _mm_sub_ps(xmm0, xmm2);
		xmm3 = _mm_sub_ps(xmm0, xmm3);
		xmm4 = _mm_sub_ps(xmm0, xmm4);

        Boost_Float_Store4_1_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Data - Dest[i];
}
void Boost_FloatDivBy(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m128 xmm0, xmm1;
	xmm1 = _mm_set_ps(Data, Data, Data, Data);
    for(i = 0; i < Amount - 3; i += 4)
	{
		xmm0 = _mm_loadu_ps(Src + i);
		xmm0 = _mm_div_ps(xmm1, xmm0);
		_mm_storeu_ps(Dest + i, xmm0);
	}
    for(; i < Amount; i ++)
		Dest[i] = Data / Dest[i];
}

void Boost_FloatAddArr(float* Dest, float* Data1, float* Data2, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
    for(i = 0; i < Amount - 15; i += 16)
    {
        xmm0 = _mm_loadu_ps(Data1 + i);
        xmm1 = _mm_loadu_ps(Data2 + i);
        xmm2 = _mm_loadu_ps(Data1 + i + 4);
        xmm3 = _mm_loadu_ps(Data2 + i + 4);
        xmm4 = _mm_loadu_ps(Data1 + i + 8);
        xmm5 = _mm_loadu_ps(Data2 + i + 8);
        xmm6 = _mm_loadu_ps(Data1 + i + 12);
        xmm7 = _mm_loadu_ps(Data2 + i + 12);
		
		xmm0 = _mm_add_ps(xmm0, xmm1);
		xmm1 = _mm_add_ps(xmm2, xmm3);
		xmm2 = _mm_add_ps(xmm4, xmm5);
		xmm3 = _mm_add_ps(xmm6, xmm7);

        Boost_Float_Store4_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Data1[i] + Data2[i];
}
void Boost_FloatSubArr(float* Dest, float* Data1, float* Data2, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
    for(i = 0; i < Amount - 15; i += 16)
	{
		xmm0 = _mm_loadu_ps(Data1 + i);
		xmm1 = _mm_loadu_ps(Data2 + i);
		xmm2 = _mm_loadu_ps(Data1 + i + 4);
		xmm3 = _mm_loadu_ps(Data2 + i + 4);
		xmm4 = _mm_loadu_ps(Data1 + i + 8);
		xmm5 = _mm_loadu_ps(Data2 + i + 8);
		xmm6 = _mm_loadu_ps(Data1 + i + 12);
		xmm7 = _mm_loadu_ps(Data2 + i + 12);
		
		xmm0 = _mm_sub_ps(xmm0, xmm1);
		xmm1 = _mm_sub_ps(xmm2, xmm3);
		xmm2 = _mm_sub_ps(xmm4, xmm5);
		xmm3 = _mm_sub_ps(xmm6, xmm7);

        Boost_Float_Store4_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Data1[i] - Data2[i];
}
void Boost_FloatMulArr(float* Dest, float* Data1, float* Data2, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
    for(i = 0; i < Amount - 15; i += 16)
	{
		xmm0 = _mm_loadu_ps(Data1 + i);
		xmm1 = _mm_loadu_ps(Data2 + i);
		xmm2 = _mm_loadu_ps(Data1 + i + 4);
		xmm3 = _mm_loadu_ps(Data2 + i + 4);
		xmm4 = _mm_loadu_ps(Data1 + i + 8);
		xmm5 = _mm_loadu_ps(Data2 + i + 8);
		xmm6 = _mm_loadu_ps(Data1 + i + 12);
		xmm7 = _mm_loadu_ps(Data2 + i + 12);
		
		xmm0 = _mm_mul_ps(xmm0, xmm1);
		xmm1 = _mm_mul_ps(xmm2, xmm3);
		xmm2 = _mm_mul_ps(xmm4, xmm5);
		xmm3 = _mm_mul_ps(xmm6, xmm7);

        Boost_Float_Store4_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Data1[i] * Data2[i];
}
void Boost_FloatDivArr(float* Dest, float* Data1, float* Data2, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
    for(i = 0; i < Amount - 15; i += 16)
	{
		xmm0 = _mm_loadu_ps(Data1 + i);
		xmm1 = _mm_loadu_ps(Data2 + i);
		xmm2 = _mm_loadu_ps(Data1 + i + 4);
		xmm3 = _mm_loadu_ps(Data2 + i + 4);
		xmm4 = _mm_loadu_ps(Data1 + i + 8);
		xmm5 = _mm_loadu_ps(Data2 + i + 8);
		xmm6 = _mm_loadu_ps(Data1 + i + 12);
		xmm7 = _mm_loadu_ps(Data2 + i + 12);
		
		xmm0 = _mm_div_ps(xmm0, xmm1);
		xmm1 = _mm_div_ps(xmm2, xmm3);
		xmm2 = _mm_div_ps(xmm4, xmm5);
		xmm3 = _mm_div_ps(xmm6, xmm7);

        Boost_Float_Store4_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Data1[i] / Data2[i];
}

void Boost_FloatMaxArr(float* Dest, float* Data1, float* Data2, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
    for(i = 0; i < Amount - 15; i += 16)
	{
		xmm0 = _mm_loadu_ps(Data1 + i);
		xmm1 = _mm_loadu_ps(Data2 + i);
		xmm2 = _mm_loadu_ps(Data1 + i + 4);
		xmm3 = _mm_loadu_ps(Data2 + i + 4);
		xmm4 = _mm_loadu_ps(Data1 + i + 8);
		xmm5 = _mm_loadu_ps(Data2 + i + 8);
		xmm6 = _mm_loadu_ps(Data1 + i + 12);
		xmm7 = _mm_loadu_ps(Data2 + i + 12);
		
		xmm0 = _mm_max_ps(xmm0, xmm1);
		xmm1 = _mm_max_ps(xmm2, xmm3);
		xmm2 = _mm_max_ps(xmm4, xmm5);
		xmm3 = _mm_max_ps(xmm6, xmm7);

        Boost_Float_Store4_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Data1[i] > Data2[i] ? Data1[i] : Data2[i];
}
void Boost_FloatMinArr(float* Dest, float* Data1, float* Data2, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
    for(i = 0; i < Amount - 15; i += 16)
	{
		xmm0 = _mm_loadu_ps(Data1 + i);
		xmm1 = _mm_loadu_ps(Data2 + i);
		xmm2 = _mm_loadu_ps(Data1 + i + 4);
		xmm3 = _mm_loadu_ps(Data2 + i + 4);
		xmm4 = _mm_loadu_ps(Data1 + i + 8);
		xmm5 = _mm_loadu_ps(Data2 + i + 8);
		xmm6 = _mm_loadu_ps(Data1 + i + 12);
		xmm7 = _mm_loadu_ps(Data2 + i + 12);
		
		xmm0 = _mm_min_ps(xmm0, xmm1);
		xmm1 = _mm_min_ps(xmm2, xmm3);
		xmm2 = _mm_min_ps(xmm4, xmm5);
		xmm3 = _mm_min_ps(xmm6, xmm7);

        Boost_Float_Store4_SSE();
	}
    for(; i < Amount; i ++)
		Dest[i] = Data1[i] < Data2[i] ? Data1[i] : Data2[i];
}

void Boost_FloatComplexMulArr(float* DestRe, float* DestIm,
                              float* Re1, float* Im1,
                              float* Re2, float* Im2, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	for(i = 0;i < Amount - 3;i += 4)
	{
		xmm0 = _mm_loadu_ps(Re1 + i);
		xmm1 = _mm_loadu_ps(Im1 + i);
		xmm2 = _mm_loadu_ps(Re2 + i);
		xmm3 = _mm_loadu_ps(Im2 + i);

		xmm4 = _mm_mul_ps(xmm0, xmm2);// Re1 * Re2
		xmm5 = _mm_mul_ps(xmm1, xmm3);// Im1 * Im2
		xmm4 = _mm_sub_ps(xmm4, xmm5);// Re1 * Re2 - Im1 * Im2 => Re

		xmm6 = _mm_mul_ps(xmm0, xmm3);// Re1 * Im2
		xmm7 = _mm_mul_ps(xmm1, xmm2);// Im1 * Re2
		xmm6 = _mm_add_ps(xmm6, xmm7);// Re1 * Im2 + Im1 * Re2 => Im

		_mm_storeu_ps(DestRe + i, xmm4);
		_mm_storeu_ps(DestIm + i, xmm6);
	}
	for(;i < Amount;i ++)
	{
		DestRe[i] = Re1[i] * Re2[i] - Im1[i] * Im2[i];
		DestIm[i] = Re1[i] * Im2[i] + Im1[i] * Re2[i];
	}	
}

float Boost_FloatSum(float* Src, int Amount)
{
	int i;
	float sum;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	xmm0 = _mm_setzero_ps();
	xmm1 = _mm_setzero_ps();
	xmm2 = _mm_setzero_ps();
	xmm3 = _mm_setzero_ps();
	for(i = 0;i < Amount - 15;i += 16)
	{
		xmm4 = _mm_loadu_ps(Src + i);
		xmm5 = _mm_loadu_ps(Src + i + 4);
		xmm6 = _mm_loadu_ps(Src + i + 8);
		xmm7 = _mm_loadu_ps(Src + i + 12);
		
		xmm0 = _mm_add_ps(xmm0, xmm4);
		xmm1 = _mm_add_ps(xmm1, xmm5);
		xmm2 = _mm_add_ps(xmm2, xmm6);
		xmm3 = _mm_add_ps(xmm3, xmm7);
	}
	xmm0 = _mm_add_ps(xmm0, xmm1);
	xmm2 = _mm_add_ps(xmm2, xmm3);
	xmm0 = _mm_add_ps(xmm0, xmm2);
	sum = xmm0[0] + xmm0[1] + xmm0[2] + xmm0[3];
	for(;i < Amount;i ++)
		sum += Src[i];
	return sum;
}

//---------------------------AVX-------------------------------
#else

void Boost_FloatCopy(float* Dest, float* Src, int Amount)
{
	int i;
	__m256 xmm0, xmm1;
	for(i = 0;i < Amount - 15;i += 16)
	{
		xmm0 = _mm256_loadu_ps(Src + i);
		xmm1 = _mm256_loadu_ps(Src + i + 8);
		
		_mm256_storeu_ps(Dest + i, xmm0);
		_mm256_storeu_ps(Dest + i + 8, xmm1);
	}
	for(;i < Amount;i ++)
		Dest[i] = Src[i];
}
void Boost_FloatInv(float* Dest, float* Src, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm256_setzero_ps();
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm1 = _mm256_loadu_ps(Src + i);
		xmm2 = _mm256_loadu_ps(Src + i + 8);
		xmm3 = _mm256_loadu_ps(Src + i + 16);
		xmm4 = _mm256_loadu_ps(Src + i + 24);
		
		xmm1 = _mm256_sub_ps(xmm0, xmm1);
		xmm2 = _mm256_sub_ps(xmm0, xmm2);
		xmm3 = _mm256_sub_ps(xmm0, xmm3);
		xmm4 = _mm256_sub_ps(xmm0, xmm4);
		
		_mm256_storeu_ps(Dest + i, xmm1);
		_mm256_storeu_ps(Dest + i + 8, xmm2);
		_mm256_storeu_ps(Dest + i + 16, xmm3);
		_mm256_storeu_ps(Dest + i + 24, xmm4);
	}
	for(;i < Amount;i ++)
		Dest[i] = - Src[i];
}
void Boost_FloatSqr(float* Dest, float* Src, int Amount)
{
	int i;
	__m256 xmm0;
	for(i = 0;i < Amount - 7;i += 8)
	{
		xmm0 = _mm256_loadu_ps(Src + i);
		xmm0 = _mm256_sqrt_ps(xmm0);
		_mm256_storeu_ps(Dest + i, xmm0);
	}
	for(;i < Amount;i ++)
		Dest[i] = sqrt(Src[i]);
}
void Boost_FloatRcp(float* Dest, float* Src, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3;
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm0 = _mm256_loadu_ps(Src + i);
		xmm1 = _mm256_loadu_ps(Src + i + 8);
		xmm2 = _mm256_loadu_ps(Src + i + 16);
		xmm3 = _mm256_loadu_ps(Src + i + 24);
		
		xmm0 = _mm256_rcp_ps(xmm0);
		xmm1 = _mm256_rcp_ps(xmm1);
		xmm2 = _mm256_rcp_ps(xmm2);
		xmm3 = _mm256_rcp_ps(xmm3);
		
		_mm256_storeu_ps(Dest + i, xmm0);
		_mm256_storeu_ps(Dest + i + 8, xmm1);
		_mm256_storeu_ps(Dest + i + 16, xmm2);
		_mm256_storeu_ps(Dest + i + 24, xmm3);
	}
	for(;i < Amount;i ++)
		Dest[i] = 1.0f / Src[i];
}
void Boost_FloatRSqr(float* Dest, float* Src, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3;
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm0 = _mm256_loadu_ps(Src + i);
		xmm1 = _mm256_loadu_ps(Src + i + 8);
		xmm2 = _mm256_loadu_ps(Src + i + 16);
		xmm3 = _mm256_loadu_ps(Src + i + 24);
		
		xmm1 = _mm256_rsqrt_ps(xmm0);
		xmm1 = _mm256_rsqrt_ps(xmm1);
		xmm2 = _mm256_rsqrt_ps(xmm2);
		xmm3 = _mm256_rsqrt_ps(xmm3);
		
		_mm256_storeu_ps(Dest + i, xmm0);
		_mm256_storeu_ps(Dest + i + 8, xmm1);
		_mm256_storeu_ps(Dest + i + 16, xmm2);
		_mm256_storeu_ps(Dest + i + 24, xmm3);
	}
	for(;i < Amount;i ++)
		Dest[i] = 1.0f / sqrt(Src[i]);
}
void Boost_FloatAbs(float* Dest, float* Src, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm7;
	xmm7 = _mm256_setzero_ps();
	for(i = 0;i < Amount - 15;i += 16)
	{
		xmm0 = _mm256_loadu_ps(Src + i);
		xmm1 = _mm256_loadu_ps(Src + i + 8);

		xmm2 = _mm256_min_ps(xmm0, xmm7);
		xmm3 = _mm256_min_ps(xmm1, xmm7);

		xmm4 = _mm256_max_ps(xmm0, xmm7);
		xmm5 = _mm256_max_ps(xmm1, xmm7);

		xmm0 = _mm256_sub_ps(xmm4, xmm2);
		xmm1 = _mm256_sub_ps(xmm5, xmm3);
		
		_mm256_storeu_ps(Dest + i, xmm0);
		_mm256_storeu_ps(Dest + i + 8, xmm1);
	}
	for(;i < Amount;i ++)
		Dest[i] = Src[i] > 0 ? Src[i] : - Src[i];
}

void Boost_FloatSet(float* Dest, float Data, int Amount)
{
	int i;
	__m256 xmm;
	xmm = _mm256_set_ps(Data, Data, Data, Data,
	                    Data, Data, Data, Data);
	for(i = 0;i < Amount - 31;i += 32)
	{
		_mm256_storeu_ps(Dest + i, xmm);
		_mm256_storeu_ps(Dest + i + 8, xmm);
		_mm256_storeu_ps(Dest + i + 16, xmm);
		_mm256_storeu_ps(Dest + i + 24, xmm);
	}
	for(;i < Amount;i ++)
		Dest[i] = Data;
}
void Boost_FloatMaxVal(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm256_set_ps(Data, Data, Data, Data,
	                     Data, Data, Data, Data);
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm1 = _mm256_loadu_ps(Src + i);
		xmm2 = _mm256_loadu_ps(Src + i + 8);
		xmm3 = _mm256_loadu_ps(Src + i + 16);
		xmm4 = _mm256_loadu_ps(Src + i + 24);

		xmm1 = _mm256_max_ps(xmm0, xmm1);
		xmm2 = _mm256_max_ps(xmm0, xmm2);
		xmm3 = _mm256_max_ps(xmm0, xmm3);
		xmm4 = _mm256_max_ps(xmm0, xmm4);
		
		_mm256_storeu_ps(Dest + i, xmm1);
		_mm256_storeu_ps(Dest + i + 8, xmm2);
		_mm256_storeu_ps(Dest + i + 16, xmm3);
		_mm256_storeu_ps(Dest + i + 24, xmm4);
	}
	for(;i < Amount;i ++)
		Dest[i] = Data < Src[i] ? Src[i] : Data;
}
void Boost_FloatMinVal(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm256_set_ps(Data, Data, Data, Data,
	                     Data, Data, Data, Data);
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm1 = _mm256_loadu_ps(Src + i);
		xmm2 = _mm256_loadu_ps(Src + i + 8);
		xmm3 = _mm256_loadu_ps(Src + i + 16);
		xmm4 = _mm256_loadu_ps(Src + i + 24);

		xmm1 = _mm256_min_ps(xmm0, xmm1);
		xmm2 = _mm256_min_ps(xmm0, xmm2);
		xmm3 = _mm256_min_ps(xmm0, xmm3);
		xmm4 = _mm256_min_ps(xmm0, xmm4);
		
		_mm256_storeu_ps(Dest + i, xmm1);
		_mm256_storeu_ps(Dest + i + 8, xmm2);
		_mm256_storeu_ps(Dest + i + 16, xmm3);
		_mm256_storeu_ps(Dest + i + 24, xmm4);
	}
	for(;i < Amount;i ++)
		Dest[i] = Data > Src[i] ? Src[i] : Data;
}

void Boost_FloatAdd(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm256_set_ps(Data, Data, Data, Data,
	                     Data, Data, Data, Data);
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm1 = _mm256_loadu_ps(Src + i);
		xmm2 = _mm256_loadu_ps(Src + i + 8);
		xmm3 = _mm256_loadu_ps(Src + i + 16);
		xmm4 = _mm256_loadu_ps(Src + i + 24);
		
		xmm1 = _mm256_add_ps(xmm1, xmm0);
		xmm2 = _mm256_add_ps(xmm2, xmm0);
		xmm3 = _mm256_add_ps(xmm3, xmm0);
		xmm4 = _mm256_add_ps(xmm4, xmm0);
		
		_mm256_storeu_ps(Dest + i, xmm1);
		_mm256_storeu_ps(Dest + i + 8, xmm2);
		_mm256_storeu_ps(Dest + i + 16, xmm3);
		_mm256_storeu_ps(Dest + i + 24, xmm4);
	}
	for(;i < Amount;i ++)
		Dest[i] = Src[i] + Data;
}
void Boost_FloatSub(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm256_set_ps(Data, Data, Data, Data,
	                     Data, Data, Data, Data);
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm1 = _mm256_loadu_ps(Src + i);
		xmm2 = _mm256_loadu_ps(Src + i + 8);
		xmm3 = _mm256_loadu_ps(Src + i + 16);
		xmm4 = _mm256_loadu_ps(Src + i + 24);
		
		xmm1 = _mm256_sub_ps(xmm1, xmm0);
		xmm2 = _mm256_sub_ps(xmm2, xmm0);
		xmm3 = _mm256_sub_ps(xmm3, xmm0);
		xmm4 = _mm256_sub_ps(xmm4, xmm0);
		
		_mm256_storeu_ps(Dest + i, xmm1);
		_mm256_storeu_ps(Dest + i + 8, xmm2);
		_mm256_storeu_ps(Dest + i + 16, xmm3);
		_mm256_storeu_ps(Dest + i + 24, xmm4);
	}
	for(;i < Amount;i ++)
		Dest[i] = Src[i] - Data;
}
void Boost_FloatMul(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm256_set_ps(Data, Data, Data, Data,
	                     Data, Data, Data, Data);
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm1 = _mm256_loadu_ps(Src + i);
		xmm2 = _mm256_loadu_ps(Src + i + 8);
		xmm3 = _mm256_loadu_ps(Src + i + 16);
		xmm4 = _mm256_loadu_ps(Src + i + 24);
		
		xmm1 = _mm256_mul_ps(xmm1, xmm0);
		xmm2 = _mm256_mul_ps(xmm2, xmm0);
		xmm3 = _mm256_mul_ps(xmm3, xmm0);
		xmm4 = _mm256_mul_ps(xmm4, xmm0);
		
		_mm256_storeu_ps(Dest + i, xmm1);
		_mm256_storeu_ps(Dest + i + 8, xmm2);
		_mm256_storeu_ps(Dest + i + 16, xmm3);
		_mm256_storeu_ps(Dest + i + 24, xmm4);
	}
	for(;i < Amount;i ++)
		Dest[i] = Src[i] * Data;
}

void Boost_FloatSubBy(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm256_set_ps(Data, Data, Data, Data,
	                     Data, Data, Data, Data);
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm1 = _mm256_loadu_ps(Src + i);
		xmm2 = _mm256_loadu_ps(Src + i + 8);
		xmm3 = _mm256_loadu_ps(Src + i + 16);
		xmm4 = _mm256_loadu_ps(Src + i + 24);
		
		xmm1 = _mm256_sub_ps(xmm0, xmm1);
		xmm2 = _mm256_sub_ps(xmm0, xmm2);
		xmm3 = _mm256_sub_ps(xmm0, xmm3);
		xmm4 = _mm256_sub_ps(xmm0, xmm4);
		
		_mm256_storeu_ps(Dest + i, xmm1);
		_mm256_storeu_ps(Dest + i + 8, xmm2);
		_mm256_storeu_ps(Dest + i + 16, xmm3);
		_mm256_storeu_ps(Dest + i + 24, xmm4);
	}
	for(;i < Amount;i ++)
		Dest[i] = Data - Dest[i];
}
void Boost_FloatDivBy(float* Dest, float* Src, float Data, int Amount)
{
	int i;
	__m256 xmm0, xmm1;
	xmm1 = _mm256_set_ps(Data, Data, Data, Data,
	                     Data, Data, Data, Data);
	for(i = 0;i < Amount - 7;i += 8)
	{
		xmm0 = _mm256_loadu_ps(Src + i);
		xmm0 = _mm256_div_ps(xmm1, xmm0);
		_mm256_storeu_ps(Dest + i, xmm0);
	}
	for(;i < Amount;i ++)
		Dest[i] = Data / Dest[i];
}

void Boost_FloatAddArr(float* Dest, float* Data1, float* Data2, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm0 = _mm256_loadu_ps(Data1 + i);
		xmm1 = _mm256_loadu_ps(Data2 + i);
		xmm2 = _mm256_loadu_ps(Data1 + i + 8);
		xmm3 = _mm256_loadu_ps(Data2 + i + 8);
		xmm4 = _mm256_loadu_ps(Data1 + i + 16);
		xmm5 = _mm256_loadu_ps(Data2 + i + 16);
		xmm6 = _mm256_loadu_ps(Data1 + i + 24);
		xmm7 = _mm256_loadu_ps(Data2 + i + 24);
		
		xmm0 = _mm256_add_ps(xmm0, xmm1);
		xmm1 = _mm256_add_ps(xmm2, xmm3);
		xmm2 = _mm256_add_ps(xmm4, xmm5);
		xmm3 = _mm256_add_ps(xmm6, xmm7);
		
		_mm256_storeu_ps(Dest + i, xmm0);
		_mm256_storeu_ps(Dest + i + 8, xmm1);
		_mm256_storeu_ps(Dest + i + 16, xmm2);
		_mm256_storeu_ps(Dest + i + 24, xmm3);
	}
	for(;i < Amount;i ++)
		Dest[i] = Data1[i] + Data2[i];
}
void Boost_FloatSubArr(float* Dest, float* Data1, float* Data2, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm0 = _mm256_loadu_ps(Data1 + i);
		xmm1 = _mm256_loadu_ps(Data2 + i);
		xmm2 = _mm256_loadu_ps(Data1 + i + 8);
		xmm3 = _mm256_loadu_ps(Data2 + i + 8);
		xmm4 = _mm256_loadu_ps(Data1 + i + 16);
		xmm5 = _mm256_loadu_ps(Data2 + i + 16);
		xmm6 = _mm256_loadu_ps(Data1 + i + 24);
		xmm7 = _mm256_loadu_ps(Data2 + i + 24);
		
		xmm0 = _mm256_sub_ps(xmm0, xmm1);
		xmm1 = _mm256_sub_ps(xmm2, xmm3);
		xmm2 = _mm256_sub_ps(xmm4, xmm5);
		xmm3 = _mm256_sub_ps(xmm6, xmm7);
		
		_mm256_storeu_ps(Dest + i, xmm0);
		_mm256_storeu_ps(Dest + i + 8, xmm1);
		_mm256_storeu_ps(Dest + i + 16, xmm2);
		_mm256_storeu_ps(Dest + i + 24, xmm3);
	}
	for(;i < Amount;i ++)
		Dest[i] = Data1[i] - Data2[i];
}
void Boost_FloatMulArr(float* Dest, float* Data1, float* Data2, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm0 = _mm256_loadu_ps(Data1 + i);
		xmm1 = _mm256_loadu_ps(Data2 + i);
		xmm2 = _mm256_loadu_ps(Data1 + i + 8);
		xmm3 = _mm256_loadu_ps(Data2 + i + 8);
		xmm4 = _mm256_loadu_ps(Data1 + i + 16);
		xmm5 = _mm256_loadu_ps(Data2 + i + 16);
		xmm6 = _mm256_loadu_ps(Data1 + i + 24);
		xmm7 = _mm256_loadu_ps(Data2 + i + 24);
		
		xmm0 = _mm256_mul_ps(xmm0, xmm1);
		xmm1 = _mm256_mul_ps(xmm2, xmm3);
		xmm2 = _mm256_mul_ps(xmm4, xmm5);
		xmm3 = _mm256_mul_ps(xmm6, xmm7);
		
		_mm256_storeu_ps(Dest + i, xmm0);
		_mm256_storeu_ps(Dest + i + 8, xmm1);
		_mm256_storeu_ps(Dest + i + 16, xmm2);
		_mm256_storeu_ps(Dest + i + 24, xmm3);
	}
	for(;i < Amount;i ++)
		Dest[i] = Data1[i] * Data2[i];
}
void Boost_FloatDivArr(float* Dest, float* Data1, float* Data2, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm0 = _mm256_loadu_ps(Data1 + i);
		xmm1 = _mm256_loadu_ps(Data2 + i);
		xmm2 = _mm256_loadu_ps(Data1 + i + 8);
		xmm3 = _mm256_loadu_ps(Data2 + i + 8);
		xmm4 = _mm256_loadu_ps(Data1 + i + 16);
		xmm5 = _mm256_loadu_ps(Data2 + i + 16);
		xmm6 = _mm256_loadu_ps(Data1 + i + 24);
		xmm7 = _mm256_loadu_ps(Data2 + i + 24);
		
		xmm0 = _mm256_div_ps(xmm0, xmm1);
		xmm1 = _mm256_div_ps(xmm2, xmm3);
		xmm2 = _mm256_div_ps(xmm4, xmm5);
		xmm3 = _mm256_div_ps(xmm6, xmm7);
		
		_mm256_storeu_ps(Dest + i, xmm0);
		_mm256_storeu_ps(Dest + i + 8, xmm1);
		_mm256_storeu_ps(Dest + i + 16, xmm2);
		_mm256_storeu_ps(Dest + i + 24, xmm3);
	}
	for(;i < Amount;i ++)
		Dest[i] = Data1[i] / Data2[i];
}

void Boost_FloatMaxArr(float* Dest, float* Data1, float* Data2, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm0 = _mm256_loadu_ps(Data1 + i);
		xmm1 = _mm256_loadu_ps(Data2 + i);
		xmm2 = _mm256_loadu_ps(Data1 + i + 8);
		xmm3 = _mm256_loadu_ps(Data2 + i + 8);
		xmm4 = _mm256_loadu_ps(Data1 + i + 16);
		xmm5 = _mm256_loadu_ps(Data2 + i + 16);
		xmm6 = _mm256_loadu_ps(Data1 + i + 24);
		xmm7 = _mm256_loadu_ps(Data2 + i + 24);
		
		xmm0 = _mm256_max_ps(xmm0, xmm1);
		xmm1 = _mm256_max_ps(xmm2, xmm3);
		xmm2 = _mm256_max_ps(xmm4, xmm5);
		xmm3 = _mm256_max_ps(xmm6, xmm7);
		
		_mm256_storeu_ps(Dest + i, xmm0);
		_mm256_storeu_ps(Dest + i + 8, xmm1);
		_mm256_storeu_ps(Dest + i + 16, xmm2);
		_mm256_storeu_ps(Dest + i + 24, xmm3);
	}
	for(;i < Amount;i ++)
		Dest[i] = Data1[i] > Data2[i] ? Data1[i] : Data2[i];
}
void Boost_FloatMinArr(float* Dest, float* Data1, float* Data2, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm0 = _mm256_loadu_ps(Data1 + i);
		xmm1 = _mm256_loadu_ps(Data2 + i);
		xmm2 = _mm256_loadu_ps(Data1 + i + 8);
		xmm3 = _mm256_loadu_ps(Data2 + i + 8);
		xmm4 = _mm256_loadu_ps(Data1 + i + 16);
		xmm5 = _mm256_loadu_ps(Data2 + i + 16);
		xmm6 = _mm256_loadu_ps(Data1 + i + 24);
		xmm7 = _mm256_loadu_ps(Data2 + i + 24);
		
		xmm0 = _mm256_min_ps(xmm0, xmm1);
		xmm1 = _mm256_min_ps(xmm2, xmm3);
		xmm2 = _mm256_min_ps(xmm4, xmm5);
		xmm3 = _mm256_min_ps(xmm6, xmm7);
		
		_mm256_storeu_ps(Dest + i, xmm0);
		_mm256_storeu_ps(Dest + i + 8, xmm1);
		_mm256_storeu_ps(Dest + i + 16, xmm2);
		_mm256_storeu_ps(Dest + i + 24, xmm3);
	}
	for(;i < Amount;i ++)
		Dest[i] = Data1[i] < Data2[i] ? Data1[i] : Data2[i];
}

void Boost_FloatComplexMulArr(float* DestRe, float* DestIm,
                              float* Re1, float* Im1,
                              float* Re2, float* Im2, int Amount)
{
	int i;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	for(i = 0;i < Amount - 7;i += 8)
	{
		xmm0 = _mm256_loadu_ps(Re1 + i);
		xmm1 = _mm256_loadu_ps(Im1 + i);
		xmm2 = _mm256_loadu_ps(Re2 + i);
		xmm3 = _mm256_loadu_ps(Im2 + i);

		xmm4 = _mm256_mul_ps(xmm0, xmm2);// Re1 * Re2
		xmm5 = _mm256_mul_ps(xmm1, xmm3);// Im1 * Im2
		xmm4 = _mm256_sub_ps(xmm4, xmm5);// Re1 * Re2 - Im1 * Im2 => Re

		xmm6 = _mm256_mul_ps(xmm0, xmm3);// Re1 * Im2
		xmm7 = _mm256_mul_ps(xmm1, xmm2);// Im1 * Re2
		xmm6 = _mm256_add_ps(xmm6, xmm7);// Re1 * Im2 + Im1 * Re2 => Im

		_mm256_storeu_ps(DestRe + i, xmm4);
		_mm256_storeu_ps(DestIm + i, xmm6);
	}
	for(;i < Amount;i ++)
	{
		DestRe[i] = Re1[i] * Re2[i] - Im1[i] * Im2[i];
		DestIm[i] = Re1[i] * Im2[i] + Im1[i] * Re2[i];
	}	
}

float Boost_FloatSum(float* Src, int Amount)
{
	int i;
	float sum;
	__m256 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	xmm0 = _mm256_setzero_ps();
	xmm1 = _mm256_setzero_ps();
	xmm2 = _mm256_setzero_ps();
	xmm3 = _mm256_setzero_ps();
	for(i = 0;i < Amount - 31;i += 32)
	{
		xmm4 = _mm256_loadu_ps(Src + i);
		xmm5 = _mm256_loadu_ps(Src + i + 8);
		xmm6 = _mm256_loadu_ps(Src + i + 16);
		xmm7 = _mm256_loadu_ps(Src + i + 24);
		
		xmm0 = _mm256_add_ps(xmm0, xmm4);
		xmm1 = _mm256_add_ps(xmm1, xmm5);
		xmm2 = _mm256_add_ps(xmm2, xmm6);
		xmm3 = _mm256_add_ps(xmm3, xmm7);
	}
	xmm0 = _mm256_add_ps(xmm0, xmm1);
	xmm2 = _mm256_add_ps(xmm2, xmm3);
	xmm0 = _mm256_add_ps(xmm0, xmm2);
	sum = xmm0[0] + xmm0[1] + xmm0[2] + xmm0[3] + xmm0[4] + xmm0[5] + xmm0[6] + xmm0[7];
	for(;i < Amount;i ++)
		sum += Src[i];
	return sum;
}
#endif

void Boost_FloatDiv(float* Dest, float* Src, float Data, int Amount)
{
	Boost_FloatMul(Dest, Src, 1.0f / Data, Amount);
}

float Boost_FloatAvg(float* Src, int Amount)
{
	return Boost_FloatSum(Src, Amount) / Amount;
}

float Boost_FloatMax(float* Src, int From, int To)
{
    int i;
    float Max = - 999;
    for(i = From; i <= To; i ++)
        if(Src[i] > Max)
            Max = Src[i];
    return Max;
}

float Boost_FloatMin(float* Src, int From, int To)
{
    int i;
    float Min = 999;
    for(i = From; i <= To; i ++)
        if(Src[i] < Min)
            Min = Src[i];
    return Min;
}

#if UINTPTR_MAX == 0xffffffff
void Boost_IntCFloat(float* Dest, int32_t* Src, int Amount)
{
	int i;
	__m64 a, b;
	__m128 xmm0;
	for(i = 0;i < Amount - 3;i += 4)
	{
		a = _mm_set_pi32(Src[i + 1], Src[i]);
		b = _mm_set_pi32(Src[i + 3], Src[i + 2]);
		xmm0 = _mm_cvtpi32x2_ps(a, b);
		_mm_storeu_ps(Dest + i, xmm0);
	}
	for(;i < Amount;i ++)
		Dest[i] = (float)Src[i];
}
#endif
