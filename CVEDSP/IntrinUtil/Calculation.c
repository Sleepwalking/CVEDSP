/*
 * Calculation.c
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
#include "Calculation.h"
#include <xmmintrin.h>
#include <math.h>
#include <stdio.h>

float* SinTable = 0;
float* CosTable = 0;
float* LnTable = 0;

void Trig_Init()
{
	int i;
	float factor = 3.1415926535 / TRIG_TABLE_SIZE;
	if(SinTable == 0)
	{
		SinTable = _mm_malloc(TRIG_TABLE_SIZE * 4, 16);
		CosTable = _mm_malloc(TRIG_TABLE_SIZE * 4, 16);

		for(i = 0;i < TRIG_TABLE_SIZE;i ++)
		{
			SinTable[i] = sin(i * factor);
			CosTable[i] = cos(i * factor);
		}
	}
}
void Trig_Exit()
{
	if(SinTable != 0)
	{
		_mm_free(SinTable);
		_mm_free(CosTable);
	}
}
void Ln_Init()
{
	int i;
	float factor = LN_TABLE_MAXIMUM / LN_TABLE_SIZE;
	if(LnTable == 0)
	{
		LnTable = _mm_malloc(LN_TABLE_SIZE * 4, 16);
		LnTable[0] = 0;
		for(i = 1;i < LN_TABLE_SIZE;i ++)
			LnTable[i] = log(i * factor);
	}
}
void Ln_Exit()
{
	if(LnTable != 0)
	{
		_mm_free(LnTable);
	}	
}

float Boost_Sin(float x)
{
    if(x < 0)
        x = 3.1415926535 - x;
    x -= ((int)(x * (1.0f / 6.283185307))) * 6.283185307;
    if(x < 3.1415926535)
        return SinTable[(int)(x * TRIG_TABLE_MAGNITUDE)];
    x -= 3.1415926535;
    return - SinTable[(int)(x * TRIG_TABLE_MAGNITUDE)];
}
float Boost_Cos(float x)
{
    if(x < 0)
        x = - x;
    x -= ((int)(x * (1.0f / 6.283185307))) * 6.283185307;
    if(x < 3.1415926535)
        return CosTable[(int)(x * TRIG_TABLE_MAGNITUDE)];
    x -= 3.1415926535;
    return - CosTable[(int)(x * TRIG_TABLE_MAGNITUDE)];
}
float Boost_Tan(float x)
{
    return Boost_Div(Boost_Sin(x), Boost_Cos(x));
}
float Boost_Ln(float x)
{
    float val;
    int index;
    if(x < 0)
        return 0;
    if(x < LN_TABLE_MINIMUM || x > LN_TABLE_MAXIMUM)
        return log(x);
    val = x * LN_TABLE_SIZE;
    index = (int)(val);
    return
        LnTable[index] * (1 - val + index) +
        LnTable[index + 1 >= LN_TABLE_SIZE ? LN_TABLE_SIZE - 1 : index + 1] * (val - index);
}

float FSin(float x)
{
    float ret;
    asm(
    "fld %1;"
    "fsin;"
    "fstp %0;"
    :"=m"(ret)
    :"m"(x)
    :);
    return ret;
}
float FCos(float x)
{
    float ret;
    asm(
    "fld %1;"
    "fcos;"
    "fstp %0;"
    :"=m"(ret)
    :"m"(x)
    :);
    return ret;
}

float Boost_Sqr(float x)
{
    return _mm_sqrt_ss(_mm_set_ss(x))[0];
}
float Boost_RSqr(float x)
{
    return _mm_rsqrt_ss(_mm_set_ss(x))[0];
}
float Boost_Rcp(float x)
{
    return _mm_rcp_ss(_mm_set_ss(x))[0];
}
float Boost_Div(float x, float y)
{
    return _mm_div_ss(_mm_set_ss(x), _mm_set_ss(y))[0];
}
