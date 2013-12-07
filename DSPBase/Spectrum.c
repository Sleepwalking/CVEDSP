/*
 * FreqSpectrum.c
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
#include "Spectrum.h"
#include "ControlPointFilter.h"
#include "Filter.h"
#include "Algorithm/Formant.h"
#include "IntrinUtil/FloatArray.h"
#include "IntrinUtil/Calculation.h"
#include "DFT/FFT.h"
#include <malloc.h>
#include <math.h>

float TempSpace[65536 * 2];

int CepstrumFromMagnitude(float* DestRe, float* DestIm, float* Magnitude, int Power)
{
	int i;
	int Amount = pow(2, Power);
	if(Amount > 65536)
		return 0;

    TempSpace[0] = Boost_Ln(Magnitude[0]);
    for(i = 1; i < Amount / 2; i ++)
	{
        TempSpace[i] = Boost_Ln(Magnitude[i]);
		TempSpace[Amount - i] = TempSpace[i];
	}
	
	Boost_FloatSet(TempSpace + 65536, 0, Amount);
	IFFT(DestRe, DestIm, TempSpace, TempSpace + 65536, Power);
	return 1;
}

void MagnitudeFromComplex(float* Dest, float* FRe, float* FIm, int Amount)
{
	Boost_FloatMulArr(TempSpace, FRe, FRe, Amount);
	Boost_FloatMulArr(Dest, FIm, FIm, Amount);
	Boost_FloatAddArr(Dest, Dest, TempSpace, Amount);
	Boost_FloatSqr(Dest, Dest, Amount);
}
void PhaseFromComplex(float* Dest, float* FRe, float* FIm, int Amount)
{
	int i;
    for(i = 0; i < Amount; i += 2)
	{
		Dest[i] = atan2(FIm[i], FRe[i]);
		Dest[i + 1] = atan2(FIm[i + 1], FRe[i + 1]);
	}
    for(; i < Amount; i ++)
		Dest[i] = atan2(FIm[i], FRe[i]);
}

void MagnitudeFromWave(float* Dest, float* Wave, int Power)
{
    int Amount = pow(2, Power);
    RFFT(Dest, TempSpace + 65536, Wave, Power);
	MagnitudeFromComplex(Dest, Dest, TempSpace + 65536, Amount);
}
void PhaseFromWave(float* Dest, float* Wave, int Power)
{
    int Amount = pow(2, Power);
    RFFT(Dest, TempSpace + 65536, Wave, Power);
	PhaseFromComplex(Dest, Dest, TempSpace + 65536, Amount);
}

void SpectralEnvelopeFromMagnitude(float* Dest, float* Magnitude, float BaseFreq, int Amount)
{
    CPF EnvCPF;
    CPF_Ctor(& EnvCPF);
    EnvCPF.UFreq = SampleRate / 2;
    CPF_AddControlFreq(& EnvCPF, 0, 0);
    ExtractFormantCPF(& EnvCPF, Magnitude, BaseFreq, Amount);
    CPF_Bake(Dest, & EnvCPF, Amount / 2);
    CPF_Dtor(& EnvCPF);
}
