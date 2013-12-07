/*
 * Filter.c
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
#include <math.h>
#include "Filter.h"
#include "DFT/FFT.h"
#include "IntrinUtil/FloatArray.h"

int SampleRate;

void SetSampleRate(int _SampleRate)
{
	SampleRate = _SampleRate;
}

void GenerateLowPass(float*Dest, float Frequency, int Length)
{
	int i;
	int pos = Frequency * Length * 2 / SampleRate;
    for(i = 0; i < pos; i ++)
		Dest[i] = 1;
	/*for(;i < Length - pos;i ++)
		Dest[i] = 0;
	for(;i < Length;i ++)
		Dest[i] = 1;*/
    for(; i < Length; i ++)
		Dest[i] = 0;
}
void GenerateHighPass(float*Dest, float Frequency, int Length)
{
	int i;
	int pos = Frequency * Length * 2 / SampleRate;
    for(i = 0; i < pos; i ++)
		Dest[i] = 0;
	/*for(;i < Length - pos;i ++)
		Dest[i] = 1;
	for(;i < Length;i ++)
		Dest[i] = 0;*/
    for(; i < Length; i ++)
		Dest[i] = 1;
}
void GenerateBandStop(float*Dest, float LFrequency, float HFrequency, int Length)
{
	int i;
	int pos1 = LFrequency * Length * 2 / SampleRate;
	int pos2 = HFrequency * Length * 2 / SampleRate;
    for(i = 0; i < pos1; i ++)
		Dest[i] = 1;
    for(; i < pos2; i ++)
		Dest[i] = 0;
	/*for(;i < Length - pos2;i ++)
		Dest[i] = 1;
	for(;i < Length - pos1;i ++)
		Dest[i] = 0;
	for(;i < Length;i ++)
		Dest[i] = 1;*/
    for(; i < Length; i ++)
		Dest[i] = 1;
}
void GenerateBandPass(float*Dest, float LFrequency, float HFrequency, int Length)
{
	int i;
	int pos1 = LFrequency * Length * 2 / SampleRate;
	int pos2 = HFrequency * Length * 2 / SampleRate;
    for(i = 0; i < pos1; i ++)
		Dest[i] = 0;
    for(; i < pos2; i ++)
		Dest[i] = 1;
	/*for(;i < Length - pos2;i ++)
		Dest[i] = 0;
	for(;i < Length - pos1;i ++)
		Dest[i] = 1;
	for(;i < Length;i ++)
		Dest[i] = 0;*/
    for(; i < Length; i ++)
		Dest[i] = 0;
}
inline void ApplyFilter(float* DestRe, float* DestIm,
                        float* SrcRe, float* SrcIm,
                        float* Filter, int Power)
{
	int Amount = pow(2, Power);
	Boost_FloatMulArr(DestRe, SrcRe, Filter, Amount / 2);
	Boost_FloatMulArr(DestIm, SrcIm, Filter, Amount / 2);
	Reflect(DestRe, DestIm, DestRe, DestIm, Power);
}
