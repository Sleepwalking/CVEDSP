/*
 * DFT.c
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
#include "DFT.h"
#include "../IntrinUtil/Calculation.h"
#include <stdio.h>

/* Only used for testing & verifying. EXTREMELY SLOW.*/

void DFT(float Real[], float OutputRe[], float OutputIm[], int Amount)
{
	int k, n;
	float SumRe, SumIm;
	float TAOP = 2 * 3.1415926535 / Amount;
	float TAOPTK;
	for(k = 0;k < Amount;k ++)
	{
		SumRe = 0;
		SumIm = 0;
		TAOPTK = TAOP * k;
		for(n = 0;n < Amount;n ++)
		{
			SumRe += Real[n] * Boost_Cos(TAOPTK * n);
			SumIm -= Real[n] * Boost_Sin(TAOPTK * n);
		}
		OutputRe[k] = SumRe;
		OutputIm[k] = SumIm;
	}
}
void DFTImag(float Real[], float Imag[], float OutputRe[], float OutputIm[], int Amount)
{
	int k, n;
	float SumRe, SumIm;
	float ReFactor, ImFactor;
	float TAOP = 2 * 3.1415926535 / Amount;
	float TAOPTK;
	for(k = 0;k < Amount;k ++)
	{
		SumRe = 0;
		SumIm = 0;
		TAOPTK = TAOP * k;
		for(n = 0;n < Amount;n ++)
		{
			ReFactor = Boost_Cos(TAOPTK * n);
			ImFactor = - Boost_Sin(TAOPTK * n);
			SumRe += Real[n] * ReFactor - Imag[n] * ImFactor;
			SumIm += Real[n] * ImFactor + Imag[n] * ReFactor;
		}
		OutputRe[k] = SumRe;
		OutputIm[k] = SumIm;
	}
}
void IDFT(float InputRe[], float InputIm[], float OutputRe[], int Amount)
{
	int k, n;
	float Sum;
	float TAOP = 2 * 3.1415926535 / Amount;
	float TAOPTK;
	for(k = 0;k < Amount;k ++)
	{
		Sum = 0;
		TAOPTK = TAOP * k;
		for(n = 0;n < Amount;n ++)
		{
			Sum += 1.0f / Amount * InputRe[n] * Boost_Cos(TAOPTK * n);
			Sum -= 1.0f / Amount * InputIm[n] * Boost_Sin(TAOPTK * n);
		}
		//printf("%f\n", Sum);
		OutputRe[k] = Sum;
	}
}
