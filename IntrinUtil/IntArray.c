/*
 * IntArray.c
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
#include "IntArray.h"

#ifdef __SSE2__

void Boost_IntAdd(int32_t* Dest, int32_t Data, int Amount)
{
	int i;
	__m128i xmm1;
	xmm1 = _mm_set_epi32(Data, Data, Data, Data);
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)(Dest + i), _mm_add_epi32(xmm1, _mm_loadu_si128((__m128i*)(Dest + i))));
	for(;i < Amount;i ++)
		Dest[i] += Data;
}
void Boost_IntSub(int32_t* Dest, int32_t Data, int Amount)
{
	int i;
	__m128i xmm1;
	xmm1 = _mm_set_epi32(Data, Data, Data, Data);
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)(Dest + i), _mm_sub_epi32(_mm_loadu_si128((__m128i*)(Dest + i)), xmm1));
	for(;i < Amount;i ++)
		Dest[i] -= Data;
}
void Boost_IntSubBy(int32_t* Dest, int32_t Data, int Amount)
{
	int i;
	__m128i xmm1;
	xmm1 = _mm_set_epi32(Data, Data, Data, Data);
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)(Dest + i), _mm_sub_epi32(xmm1, _mm_loadu_si128((__m128i*)(Dest + i))));
	for(;i < Amount;i ++)
		Dest[i] = Dest[i] - Data;
}
void Boost_IntAddArr(int32_t* Dest, int32_t* Data1, int32_t* Data2, int Amount)
{
	int i;
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)(Dest + i),
		                 _mm_add_epi32(_mm_loadu_si128((__m128i*)(Data1 + i)),
		                               _mm_loadu_si128((__m128i*)(Data2 + i))));
	for(;i < Amount;i ++)
		Dest[i] = Data1[i] + Data2[i];
}
void Boost_IntSubArr(int32_t* Dest, int32_t* Data1, int32_t* Data2, int Amount)
{
	int i;
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)(Dest + i),
		                 _mm_sub_epi32(_mm_loadu_si128((__m128i*)(Data1 + i)),
		                               _mm_loadu_si128((__m128i*)(Data2 + i))));
	for(;i < Amount;i ++)
		Dest[i] = Data1[i] - Data2[i];
}

void Boost_ByteAdd(char* Dest, char Data, int Amount)
{
	int i;
	__m128i xmm1;
	xmm1 = _mm_set_epi8(Data, Data, Data, Data,
	                    Data, Data, Data, Data,
	                    Data, Data, Data, Data,
	                    Data, Data, Data, Data);
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_add_epi8(xmm1, _mm_loadu_si128((__m128i*)((int32_t*)Dest + i))));
	for(;i < Amount;i ++)
	{
		*((char*)((int32_t*)Dest + i)) += Data;
		*((char*)((int32_t*)Dest + i) + 1) += Data;
		*((char*)((int32_t*)Dest + i) + 2) += Data;
		*((char*)((int32_t*)Dest + i) + 3) += Data;
	}
}
void Boost_ByteSub(char* Dest, char Data, int Amount)
{
	int i;
	__m128i xmm1;
	xmm1 = _mm_set_epi8(Data, Data, Data, Data,
	                    Data, Data, Data, Data,
	                    Data, Data, Data, Data,
	                    Data, Data, Data, Data);
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_sub_epi8(_mm_loadu_si128((__m128i*)((int32_t*)Dest + i)), xmm1));
	for(;i < Amount;i ++)
	{
		*((char*)((int32_t*)Dest + i)) -= Data;
		*((char*)((int32_t*)Dest + i) + 1) -= Data;
		*((char*)((int32_t*)Dest + i) + 2) -= Data;
		*((char*)((int32_t*)Dest + i) + 3) -= Data;
	}
}
void Boost_ByteSubBy(char* Dest, char Data, int Amount)
{
	int i;
	__m128i xmm1;
	xmm1 = _mm_set_epi8(Data, Data, Data, Data,
	                    Data, Data, Data, Data,
	                    Data, Data, Data, Data,
	                    Data, Data, Data, Data);
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_sub_epi8(xmm1, _mm_loadu_si128((__m128i*)((int32_t*)Dest + i))));
	for(;i < Amount;i ++)
	{
		*((char*)((int32_t*)Dest + i)) = *((char*)((int32_t*)Dest + i)) - Data;
		*((char*)((int32_t*)Dest + i) + 1) = *((char*)((int32_t*)Dest + i) + 1) - Data;
		*((char*)((int32_t*)Dest + i) + 2) = *((char*)((int32_t*)Dest + i) + 2) - Data;
		*((char*)((int32_t*)Dest + i) + 3) = *((char*)((int32_t*)Dest + i) + 3) - Data;
	}
}
void Boost_UByteAvgArr(char* Dest, char* Data1, char* Data2, int Amount)
{
	int i;
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_avg_epu8(_mm_loadu_si128((__m128i*)((int32_t*)Data1 + i)),
		                              _mm_loadu_si128((__m128i*)((int32_t*)Data2 + i))));
	for(;i < Amount;i ++)
	{
		*((char*)((int32_t*)Dest + i)) = (*((char*)((int32_t*)Data1 + i)) + *((char*)((int32_t*)Data2 + i))) >> 1;
		*((char*)((int32_t*)Dest + i) + 1) = (*((char*)((int32_t*)Data1 + i) + 1) + *((char*)((int32_t*)Data2 + i) + 1)) >> 1;
		*((char*)((int32_t*)Dest + i) + 2) = (*((char*)((int32_t*)Data1 + i) + 2) + *((char*)((int32_t*)Data2 + i) + 2)) >> 1;
		*((char*)((int32_t*)Dest + i) + 3) = (*((char*)((int32_t*)Data1 + i) + 3) + *((char*)((int32_t*)Data2 + i) + 3)) >> 1;
	}
}
void Boost_ByteAddArr(char* Dest, char* Data1, char* Data2, int Amount)
{
	int i;
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_add_epi8(_mm_loadu_si128((__m128i*)((int32_t*)Data1 + i)),
		                              _mm_loadu_si128((__m128i*)((int32_t*)Data2 + i))));
	for(;i < Amount;i ++)
	{
		*((char*)((int32_t*)Dest + i)) = *((char*)((int32_t*)Data1 + i)) + *((char*)((int32_t*)Data2 + i));
		*((char*)((int32_t*)Dest + i) + 1) = *((char*)((int32_t*)Data1 + i) + 1) + *((char*)((int32_t*)Data2 + i) + 1);
		*((char*)((int32_t*)Dest + i) + 2) = *((char*)((int32_t*)Data1 + i) + 2) + *((char*)((int32_t*)Data2 + i) + 2);
		*((char*)((int32_t*)Dest + i) + 3) = *((char*)((int32_t*)Data1 + i) + 3) + *((char*)((int32_t*)Data2 + i) + 3);
	}
}
void Boost_ByteSubArr(char* Dest, char* Data1, char* Data2, int Amount)
{
	int i;
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_sub_epi8(_mm_loadu_si128((__m128i*)((int32_t*)Data1 + i)),
		                              _mm_loadu_si128((__m128i*)((int32_t*)Data2 + i))));
	for(;i < Amount;i ++)
	{
		*((char*)((int32_t*)Dest + i)) = *((char*)((int32_t*)Data1 + i)) - *((char*)((int32_t*)Data2 + i));
		*((char*)((int32_t*)Dest + i) + 1) = *((char*)((int32_t*)Data1 + i) + 1) - *((char*)((int32_t*)Data2 + i) + 1);
		*((char*)((int32_t*)Dest + i) + 2) = *((char*)((int32_t*)Data1 + i) + 2) - *((char*)((int32_t*)Data2 + i) + 2);
		*((char*)((int32_t*)Dest + i) + 3) = *((char*)((int32_t*)Data1 + i) + 3) - *((char*)((int32_t*)Data2 + i) + 3);
	}
}

void Boost_ShortAdd(int16_t* Dest, int16_t Data, int Amount)
{
	int i;
	__m128i xmm1;
	xmm1 = _mm_set_epi16(Data, Data, Data, Data,
	                     Data, Data, Data, Data);
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_add_epi16(xmm1, _mm_loadu_si128((__m128i*)((int32_t*)Dest + i))));
	for(;i < Amount;i ++)
	{
		*((int16_t*)((int32_t*)Dest + i)) += Data;
		*((int16_t*)((int32_t*)Dest + i) + 1) += Data;
	}
}
void Boost_ShortSub(int16_t* Dest, int16_t Data, int Amount)
{
	int i;
	__m128i xmm1;
	xmm1 = _mm_set_epi16(Data, Data, Data, Data,
	                     Data, Data, Data, Data);
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_sub_epi16(_mm_loadu_si128((__m128i*)((int32_t*)Dest + i)), xmm1));
	for(;i < Amount;i ++)
	{
		*((int16_t*)((int32_t*)Dest + i)) -= Data;
		*((int16_t*)((int32_t*)Dest + i) + 1) -= Data;
	}
}
void Boost_ShortSubBy(int16_t* Dest, int16_t Data, int Amount)
{
	int i;
	__m128i xmm1;
	xmm1 = _mm_set_epi16(Data, Data, Data, Data,
	                     Data, Data, Data, Data);
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_sub_epi16(xmm1, _mm_loadu_si128((__m128i*)((int32_t*)Dest + i))));
	for(;i < Amount;i ++)
	{
		*((int16_t*)((int32_t*)Dest + i)) = *((int16_t*)((int32_t*)Dest + i)) - Data;
		*((int16_t*)((int32_t*)Dest + i) + 1) = *((int16_t*)((int32_t*)Dest + i) + 1) - Data;
	}
}
void Boost_UShortAvg(int16_t* Dest, int16_t Data, int Amount)
{
	int i;
	__m128i xmm1;
	xmm1 = _mm_set_epi16(Data, Data, Data, Data,
	                     Data, Data, Data, Data);
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_avg_epu16(xmm1, _mm_loadu_si128((__m128i*)((int32_t*)Dest + i))));
	for(;i < Amount;i ++)
	{
		*((int16_t*)((int32_t*)Dest + i)) = *((int16_t*)((int32_t*)Dest + i)) - Data;
		*((int16_t*)((int32_t*)Dest + i) + 1) = *((int16_t*)((int32_t*)Dest + i) + 1) - Data;
	}
}
void Boost_ShortAddArr(int16_t* Dest, int16_t* Data1, int16_t* Data2, int Amount)
{
	int i;
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_add_epi16(_mm_loadu_si128((__m128i*)((int32_t*)Data1 + i)),
		                              _mm_loadu_si128((__m128i*)((int32_t*)Data2 + i))));
	for(;i < Amount;i ++)
	{
		*((int16_t*)((int32_t*)Dest + i)) = *((int16_t*)((int32_t*)Data1 + i)) + *((int16_t*)((int32_t*)Data2 + i));
		*((int16_t*)((int32_t*)Dest + i) + 1) = *((int16_t*)((int32_t*)Data1 + i) + 1) + *((int16_t*)((int32_t*)Data2 + i) + 1);
	}
}
void Boost_ShortSubArr(int16_t* Dest, int16_t* Data1, int16_t* Data2, int Amount)
{
	int i;
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_sub_epi16(_mm_loadu_si128((__m128i*)((int32_t*)Data1 + i)),
		                              _mm_loadu_si128((__m128i*)((int32_t*)Data2 + i))));
	for(;i < Amount;i ++)
	{
		*((int16_t*)((int32_t*)Dest + i)) = *((int16_t*)((int32_t*)Data1 + i)) - *((int16_t*)((int32_t*)Data2 + i));
		*((int16_t*)((int32_t*)Dest + i) + 1) = *((int16_t*)((int32_t*)Data1 + i) + 1) - *((int16_t*)((int32_t*)Data2 + i) + 1);
	}
}
void Boost_UShortAvgArr(int16_t* Dest, int16_t* Data1, int16_t* Data2, int Amount)
{
	int i;
	for(i = 0;i < Amount - 3;i += 4)
		_mm_storeu_si128((__m128i*)((int32_t*)Dest + i),
		                 _mm_avg_epu16(_mm_loadu_si128((__m128i*)((int32_t*)Data1 + i)),
		                               _mm_loadu_si128((__m128i*)((int32_t*)Data2 + i))));
	for(;i < Amount;i ++)
	{
		*((int16_t*)((int32_t*)Dest + i)) = (*((int16_t*)((int32_t*)Data1 + i)) + *((int16_t*)((int32_t*)Data2 + i))) >> 1;
		*((int16_t*)((int32_t*)Dest + i) + 1) = (*((int16_t*)((int32_t*)Data1 + i) + 1) + *((int16_t*)((int32_t*)Data2 + i) + 1)) >> 1;
	}
}
#endif
