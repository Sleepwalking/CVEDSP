/*
 * FDAnalysis.h
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
#ifndef BASEFREQ_H
#define BASEFREQ_H

#ifdef __cplusplus
extern "C"{
#endif

typedef struct
{
    int Forward;
    int Backward;
    float AvgFactor;
    float MinFactor;
    int Start;
} BaseFreqAnalyzerParameters;

extern BaseFreqAnalyzerParameters BPara;

extern void BFAnalyzer_Init() __attribute__ ((constructor));
extern void BFAnalyzer_SetParameters(BaseFreqAnalyzerParameters Src);

extern float GetBaseFrequencyFromSpectrum(float* Spectrum, float LFreq, float UFreq, int Amount);
extern float GetBaseFrequencyFromSpectrum2(float* Spectrum, float LFreq, float UFreq, int Amount);

extern float GetBaseFrequencyFromCepstrum(float* Cepstrum, float LFreq, float UFreq, int Amount);
extern int GetBasePeriodFromCepstrum(float* Cepstrum, float LPeriod, float UPeriod, int Amount);

extern float GetBaseFrequencyFromWave(float* Wave, float LFreq, float UFreq, int Power);

#ifdef __cplusplus
}
#endif

#endif
