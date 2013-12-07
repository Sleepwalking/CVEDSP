/*
 * Window.c
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
#include "Window.h"
#include "IntrinUtil/Calculation.h"
#include "IntrinUtil/FloatArray.h"

void GenerateBoxCar(float* Dest, int Length)
{
	int i;
	for(i = 0;i < Length;i ++)
		Dest[i] = 1.0f;
}
void GenerateTriang(float* Dest, int Length)
{
	int i;
	float LenInv = 1.0f / (Length - 1);
	for(i = 0;i < Length / 2;i ++)
		Dest[i] = 2.0f * (float)i * LenInv;
	for(;i < Length;i ++)
		Dest[i] = 2.0f - 2.0f * (float)i * LenInv;
}
void GenerateAsymTriang(float* Dest, int Length, int Center)
{
	int i;
	float CenterInv = 1.0f / Center;
	float LenInv = 1.0f / (Length - Center);
	for(i = 0;i < Center;i ++)
		Dest[i] = i * CenterInv;
	for(;i < Length;i ++)
		Dest[i] = 1.0f - (i - Center) * LenInv;
}
void GenerateHanning(float* Dest, int Length)
{
	int i;
	float TPiLenInv = 3.1415926536 * 2 / (Length - 1);
	for(i = 0;i < Length;i ++)
        Dest[i] = 0.5 * (1.0f - cos(TPiLenInv * i));
}
void GenerateHamming(float* Dest, int Length)
{
	int i;
	float TPiLenInv = 3.1415926536 * 2 / (Length - 1);
	for(i = 0;i < Length;i ++)
        Dest[i] = 0.54 - 0.46 * cos(TPiLenInv * i);
}
void GenerateBlackman(float* Dest, int Length)
{
	int i;
	float TPiLenInv = 3.1415926536 * 2 / (Length - 1);
	for(i = 0;i < Length;i ++)
        Dest[i] = 0.42 - 0.5 * cos(TPiLenInv * (float)i) + 0.08 * cos(2 * TPiLenInv * (float)i);
}

inline void ApplyWindow(float* Dest, float* Src, float* Window, int Offset, int Length)
{
	Boost_FloatMulArr(Dest, Src + Offset, Window, Length);
}
