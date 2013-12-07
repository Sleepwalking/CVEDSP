/*
 * TDAnalysis.c
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
#include "TDAnalysis.h"
#include "IntrinUtil/FloatArray.h"
#include "IntrinUtil/Calculation.h"
#include "DFT/FFT.h"
#include <malloc.h>
#include <math.h>

float Energy(float* Wave, int Length)
{
	float* Tmp = malloc(Length * sizeof(float));
	float ret;
	Boost_FloatMulArr(Tmp, Wave, Wave, Length);
	ret = Boost_FloatSum(Tmp, Length);
	free(Tmp);
	return ret;
}
float AverageMagnitude(float* Wave, int Length)
{
	float* Tmp = malloc(Length * sizeof(float));
	float ret;
	Boost_FloatAbs(Tmp, Wave, Length);
	ret = Boost_FloatSum(Tmp, Length);
	free(Tmp);
	return ret / Length;
}
float ZeroCrossingRate(float* Wave, int Length)
{
	int i, count = 0;
	for(i = 1;i < Length;i += 4)
	{
		if((Wave[i] > 0 && Wave[i - 1] < 0) ||
		   (Wave[i] < 0 && Wave[i - 1] > 0))
		count ++;
		if((Wave[i + 1] > 0 && Wave[i] < 0) ||
		   (Wave[i + 1] < 0 && Wave[i] > 0))
		count ++;
		if((Wave[i + 2] > 0 && Wave[i + 2] < 0) ||
		   (Wave[i + 2] < 0 && Wave[i + 2] > 0))
		count ++;
		if((Wave[i + 3] > 0 && Wave[i + 3] < 0) ||
		   (Wave[i + 3] < 0 && Wave[i + 3] > 0))
		count ++;
	}
	return (float)count / Length;
}
float ThresholdCrossingRate(float* Wave, float Threshold, int Length)
{
	int i, count = 0;
	for(i = 1;i < Length;i += 4)
	{
		if((Wave[i] > Threshold && Wave[i - 1] < Threshold) ||
		   (Wave[i] < Threshold && Wave[i - 1] > Threshold))
		count ++;
		if((Wave[i + 1] > Threshold && Wave[i] < Threshold) ||
		   (Wave[i + 1] < Threshold && Wave[i] > Threshold))
		count ++;
		if((Wave[i + 2] > Threshold && Wave[i + 2] < Threshold) ||
		   (Wave[i + 2] < Threshold && Wave[i + 2] > Threshold))
		count ++;
		if((Wave[i + 3] > Threshold && Wave[i + 3] < Threshold) ||
		   (Wave[i + 3] < Threshold && Wave[i + 3] > Threshold))
		count ++;
	}
	return (float)count / Length;
}
float CorrelationAt(float* Wave1, float* Wave2, int Length)
{
	float* Tmp = malloc(Length * sizeof(float));
	float ret;
	Boost_FloatMulArr(Tmp, Wave1, Wave2, Length);
	ret = Boost_FloatSum(Tmp, Length);
	free(Tmp);
    return ret / Length;
}
float AMDFAt(float* Wave1, float* Wave2, int Length)
{
	float* Tmp = malloc(Length * sizeof(float));
	float ret;
	Boost_FloatSubArr(Tmp, Wave1, Wave2, Length);
	Boost_FloatAbs(Tmp, Tmp, Length);
	ret = Boost_FloatSum(Tmp, Length);
	free(Tmp);
	return ret / Length;	
}

void EnergyCurve(float* Dest, float* Wave, int Resolution, int DestLength)
{
	int i, half;
	float* Tmp = malloc(DestLength * sizeof(float));
	Boost_FloatMulArr(Tmp, Wave, Wave, DestLength);
	half = Resolution / 2;
	for(i = 0;i < DestLength;i ++)
	{
		Dest[i] = Boost_FloatSum(Tmp + (i > half ? i - half : 0),
		                         i + half > DestLength ? DestLength - i : half);
	}
	free(Tmp);
}
void AmplitudeCurve(float* Dest, float* Wave, int Resolution, int DestLength)
{
	int i, half;
	float* Tmp = malloc(DestLength * sizeof(float));
	Boost_FloatAbs(Tmp, Wave, DestLength);
	half = Resolution / 2;
	for(i = 0;i < DestLength;i ++)
	{
		Dest[i] = Boost_FloatSum(Tmp + (i > half ? i - half : 0),
		                         i + half > DestLength ? DestLength - i : half);
	}
	free(Tmp);
}
void Correlation(float* Dest, float* Wave1, float* Wave2, int CompareLength, int DestLength)
{
	float* Tmp = malloc(CompareLength * sizeof(float));
	int i;
	for(i = 0;i < DestLength;i ++)
	{
		Boost_FloatMulArr(Tmp, Wave1 + i, Wave2, CompareLength);
		Dest[i] = Boost_FloatSum(Tmp, CompareLength);
	}
	Boost_FloatDiv(Dest, Dest, CompareLength, DestLength);
	free(Tmp);
}
void FastCorrelation(float* Dest, float* Wave1, float* Wave2, int Power)
{
	int Length = pow(2, Power);
	float* Re1 = malloc(Length * sizeof(float));
	float* Im1 = malloc(Length * sizeof(float));
	float* Re2 = malloc(Length * sizeof(float));
	float* Im2 = malloc(Length * sizeof(float));
	Boost_FloatSet(Re1, 0, Length);
	Boost_FloatSet(Re2, 0, Length);
	Boost_FloatSet(Im1, 0, Length);
	Boost_FloatSet(Im2, 0, Length);

	FFT(Wave1, Re1, Re1, Im1, Power);
	FFT(Wave2, Re2, Re2, Im2, Power);
	Boost_FloatComplexMulArr(Re1, Im1, Re1, Im1, Re2, Im2, Length);
	IFFT(Re1, Im1, Dest, Im2, Power);
	
	Boost_FloatDiv(Dest, Dest, Length ,Length);
	
	free(Re1);
	free(Im1);
	free(Re2);
	free(Im2);
}
void AMDF(float* Dest, float* Wave1, float* Wave2, int CompareLength, int DestLength)
{
	float* Tmp = malloc(CompareLength * sizeof(float));
	int i;
	for(i = 0;i < DestLength;i ++)
	{
		Boost_FloatSubArr(Tmp, Wave1 + i, Wave2, CompareLength);
		Boost_FloatAbs(Tmp, Tmp, CompareLength);
		Dest[i] = Boost_FloatSum(Tmp, CompareLength);
	}
	Boost_FloatDiv(Dest, Dest, CompareLength, DestLength);
	free(Tmp);
}

void Threshold(float* Dest, float* Wave, float Threshold, int Length)
{
	float* Tmp = malloc(Length * sizeof(float));
	float* Tmp2 = malloc(Length * sizeof(float));
    Boost_FloatMaxVal(Tmp, Wave, Threshold, Length);
	Boost_FloatSub(Tmp, Tmp, Threshold, Length);
    Boost_FloatMinVal(Tmp2, Wave, - Threshold, Length);
	Boost_FloatAdd(Tmp2, Tmp2, Threshold, Length);
	Boost_FloatAddArr(Dest, Tmp, Tmp2, Length);
	free(Tmp);
	free(Tmp2);
}
void Limit(float* Dest, float* Wave, float Limit, int Length)
{
    Boost_FloatMinVal(Dest, Wave, Limit, Length);
    Boost_FloatMaxVal(Dest, Dest, - Limit, Length);
}
