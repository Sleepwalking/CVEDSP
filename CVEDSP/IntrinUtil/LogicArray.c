/*
 * LogicArray.c
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
#include "LogicArray.h"
#include <math.h>
#include <xmmintrin.h>

void Boost_LogicAnd(void* Dest, void* Src, int32_t Mask, int Amount)
{
	int i;
	float Data = *(float*)&Mask;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm_set_ps(Data, Data, Data, Data);
	for(i = 0;i < Amount - 15;i += 16)
	{
		xmm1 = _mm_loadu_ps((float*)((int32_t*)Src + i));
		xmm2 = _mm_loadu_ps((float*)((int32_t*)Src + i + 4));
		xmm3 = _mm_loadu_ps((float*)((int32_t*)Src + i + 8));
		xmm4 = _mm_loadu_ps((float*)((int32_t*)Src + i + 12));
		
		xmm1 = _mm_and_ps(xmm0, xmm1);
		xmm2 = _mm_and_ps(xmm0, xmm2);
		xmm3 = _mm_and_ps(xmm0, xmm3);
        xmm4 = _mm_and_ps(xmm0, xmm4);
		
		_mm_storeu_ps((float*)((int32_t*)Dest + i), xmm1);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 4), xmm2);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 8), xmm3);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 12), xmm4);
	}
	for(;i < Amount;i ++)
		*((int32_t*)Dest + i) = *((int32_t*)Src + i) & Mask;
}
void Boost_LogicAndNot(void* Dest, void* Src, int Mask, int Amount)
{
	int i;
	float Data = *(float*)&Mask;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm_set_ps(Data, Data, Data, Data);
	for(i = 0;i < Amount - 15;i += 16)
	{
		xmm1 = _mm_loadu_ps((float*)((int32_t*)Src + i));
		xmm2 = _mm_loadu_ps((float*)((int32_t*)Src + i + 4));
		xmm3 = _mm_loadu_ps((float*)((int32_t*)Src + i + 8));
		xmm4 = _mm_loadu_ps((float*)((int32_t*)Src + i + 12));
		
		xmm1 = _mm_andnot_ps(xmm0, xmm1);
		xmm2 = _mm_andnot_ps(xmm0, xmm2);
		xmm3 = _mm_andnot_ps(xmm0, xmm3);
		xmm4 = _mm_andnot_ps(xmm0, xmm4);
		
		_mm_storeu_ps((float*)((int32_t*)Dest + i), xmm1);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 4), xmm2);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 8), xmm3);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 12), xmm4);
	}
	for(;i < Amount;i ++)
		*((int32_t*)Dest + i) = ~ *((int32_t*)Src + i) & Mask;
}
void Boost_LogicOr(void* Dest, void* Src, int Mask, int Amount)
{
	int i;
	float Data = *(float*)&Mask;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm_set_ps(Data, Data, Data, Data);
	for(i = 0;i < Amount - 15;i += 16)
	{
		xmm1 = _mm_loadu_ps((float*)((int32_t*)Src + i));
		xmm2 = _mm_loadu_ps((float*)((int32_t*)Src + i + 4));
		xmm3 = _mm_loadu_ps((float*)((int32_t*)Src + i + 8));
		xmm4 = _mm_loadu_ps((float*)((int32_t*)Src + i + 12));
		
		xmm1 = _mm_or_ps(xmm0, xmm1);
		xmm2 = _mm_or_ps(xmm0, xmm2);
		xmm3 = _mm_or_ps(xmm0, xmm3);
		xmm4 = _mm_or_ps(xmm0, xmm4);
		
		_mm_storeu_ps((float*)((int32_t*)Dest + i), xmm1);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 4), xmm2);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 8), xmm3);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 12), xmm4);
	}
	for(;i < Amount;i ++)
		*((int32_t*)Dest + i) = *((int32_t*)Src + i) | Mask;
}
void Boost_LogicXor(void* Dest, void* Src, int Mask, int Amount)
{
	int i;
	float Data = *(float*)&Mask;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4;
	xmm0 = _mm_set_ps(Data, Data, Data, Data);
	for(i = 0;i < Amount - 15;i += 16)
	{
		xmm1 = _mm_loadu_ps((float*)((int32_t*)Src + i));
		xmm2 = _mm_loadu_ps((float*)((int32_t*)Src + i + 4));
		xmm3 = _mm_loadu_ps((float*)((int32_t*)Src + i + 8));
		xmm4 = _mm_loadu_ps((float*)((int32_t*)Src + i + 12));
		
		xmm1 = _mm_xor_ps(xmm0, xmm1);
		xmm2 = _mm_xor_ps(xmm0, xmm2);
		xmm3 = _mm_xor_ps(xmm0, xmm3);
		xmm4 = _mm_xor_ps(xmm0, xmm4);
		
		_mm_storeu_ps((float*)((int32_t*)Dest + i), xmm1);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 4), xmm2);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 8), xmm3);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 12), xmm4);
	}
	for(;i < Amount;i ++)
		*((int32_t*)Dest + i) ^= Mask;
}

void Boost_LogicAndArr(void* Dest, void* Data1, void* Data2, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	for(i = 0;i < Amount - 15;i += 16)
	{
		xmm0 = _mm_loadu_ps((float*)((int32_t*)Data1 + i));
		xmm1 = _mm_loadu_ps((float*)((int32_t*)Data2 + i));
		xmm2 = _mm_loadu_ps((float*)((int32_t*)Data1 + i + 4));
		xmm3 = _mm_loadu_ps((float*)((int32_t*)Data2 + i + 4));
		xmm4 = _mm_loadu_ps((float*)((int32_t*)Data1 + i + 8));
		xmm5 = _mm_loadu_ps((float*)((int32_t*)Data2 + i + 8));
		xmm6 = _mm_loadu_ps((float*)((int32_t*)Data1 + i + 12));
		xmm7 = _mm_loadu_ps((float*)((int32_t*)Data2 + i + 12));

		xmm0 = _mm_and_ps(xmm0, xmm1);
		xmm2 = _mm_and_ps(xmm2, xmm3);
		xmm4 = _mm_and_ps(xmm4, xmm5);
		xmm6 = _mm_and_ps(xmm6, xmm7);
		
		_mm_storeu_ps((float*)((int32_t*)Dest + i), xmm0);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 4), xmm2);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 8), xmm4);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 12), xmm6);
	}
	for(;i < Amount;i ++)
		((int32_t*)Dest)[i] = ((int32_t*)Data1)[i] & ((int32_t*)Data2)[i];
}
void Boost_LogicAndNotArr(void* Dest, void* Data2, void* Data1, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	for(i = 0;i < Amount - 15;i += 16)
	{
		xmm0 = _mm_loadu_ps((float*)((int32_t*)Data1 + i));
		xmm1 = _mm_loadu_ps((float*)((int32_t*)Data2 + i));
		xmm2 = _mm_loadu_ps((float*)((int32_t*)Data1 + i + 4));
		xmm3 = _mm_loadu_ps((float*)((int32_t*)Data2 + i + 4));
		xmm4 = _mm_loadu_ps((float*)((int32_t*)Data1 + i + 8));
		xmm5 = _mm_loadu_ps((float*)((int32_t*)Data2 + i + 8));
		xmm6 = _mm_loadu_ps((float*)((int32_t*)Data1 + i + 12));
		xmm7 = _mm_loadu_ps((float*)((int32_t*)Data2 + i + 12));

		xmm0 = _mm_andnot_ps(xmm0, xmm1);
		xmm2 = _mm_andnot_ps(xmm2, xmm3);
		xmm4 = _mm_andnot_ps(xmm4, xmm5);
		xmm6 = _mm_andnot_ps(xmm6, xmm7);
		
		_mm_storeu_ps((float*)((int32_t*)Dest + i), xmm0);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 4), xmm2);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 8), xmm4);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 12), xmm6);
	}
	for(;i < Amount;i ++)
		((int32_t*)Dest)[i] = ~ ((int32_t*)Data1)[i] & ((int32_t*)Data2)[i];
}
void Boost_LogicOrArr(void* Dest, void* Data1, void* Data2, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	for(i = 0;i < Amount - 15;i += 16)
	{
		xmm0 = _mm_loadu_ps((float*)((int32_t*)Data1 + i));
		xmm1 = _mm_loadu_ps((float*)((int32_t*)Data2 + i));
		xmm2 = _mm_loadu_ps((float*)((int32_t*)Data1 + i + 4));
		xmm3 = _mm_loadu_ps((float*)((int32_t*)Data2 + i + 4));
		xmm4 = _mm_loadu_ps((float*)((int32_t*)Data1 + i + 8));
		xmm5 = _mm_loadu_ps((float*)((int32_t*)Data2 + i + 8));
		xmm6 = _mm_loadu_ps((float*)((int32_t*)Data1 + i + 12));
		xmm7 = _mm_loadu_ps((float*)((int32_t*)Data2 + i + 12));

		xmm0 = _mm_or_ps(xmm0, xmm1);
		xmm2 = _mm_or_ps(xmm2, xmm3);
		xmm4 = _mm_or_ps(xmm4, xmm5);
		xmm6 = _mm_or_ps(xmm6, xmm7);
		
		_mm_storeu_ps((float*)((int32_t*)Dest + i), xmm0);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 4), xmm2);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 8), xmm4);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 12), xmm6);
	}
	for(;i < Amount;i ++)
		((int32_t*)Dest)[i] = ((int32_t*)Data1)[i] | ((int32_t*)Data2)[i];
}
void Boost_LogicXorArr(void* Dest, void* Data1, void* Data2, int Amount)
{
	int i;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	for(i = 0;i < Amount - 15;i += 16)
	{
		xmm0 = _mm_loadu_ps((float*)((int32_t*)Data1 + i));
		xmm1 = _mm_loadu_ps((float*)((int32_t*)Data2 + i));
		xmm2 = _mm_loadu_ps((float*)((int32_t*)Data1 + i + 4));
		xmm3 = _mm_loadu_ps((float*)((int32_t*)Data2 + i + 4));
		xmm4 = _mm_loadu_ps((float*)((int32_t*)Data1 + i + 8));
		xmm5 = _mm_loadu_ps((float*)((int32_t*)Data2 + i + 8));
		xmm6 = _mm_loadu_ps((float*)((int32_t*)Data1 + i + 12));
		xmm7 = _mm_loadu_ps((float*)((int32_t*)Data2 + i + 12));

		xmm0 = _mm_xor_ps(xmm0, xmm1);
		xmm2 = _mm_xor_ps(xmm2, xmm3);
		xmm4 = _mm_xor_ps(xmm4, xmm5);
		xmm6 = _mm_xor_ps(xmm6, xmm7);
		
		_mm_storeu_ps((float*)((int32_t*)Dest + i), xmm0);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 4), xmm2);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 8), xmm4);
		_mm_storeu_ps((float*)((int32_t*)Dest + i + 12), xmm6);
	}
	for(;i < Amount;i ++)
		((int32_t*)Dest)[i] = ((int32_t*)Data1)[i] ^ ((int32_t*)Data2)[i];
}
