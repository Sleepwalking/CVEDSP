/*
 * Formant.h
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
#ifndef FORMANT_H
#define FORMANT_H
#include "../DSPBase/ControlPointFilter.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct
{
    float F1;
    float F2;
    float F3;
} FormantDescriptor;

typedef struct
{
    float F1Min;
    float F1Max;
    float F2Min;
    float F2Max;
    float F3Min;
    float F3Max;

    float PreEmphasisStrength;

    float FPeakDistanceWidth;
    float FPeakThreshold;
    float FPeakWeightWidth;

    float F2SearchRatioThreshold;
    float F2SearchWidth;
    float F2SearchWeightThreshold;
    float F2SearchWeightThresholdRatio;
    float F2SearchF3MinWeight;

    float F1F2WeightDeviationFactor;
    float F1F2WeightMinDeviation;

    float HighFreqAdditionalRange;
    float HighLowFreqSearchInterval;
    float HighLowFreqSearchThreshold;
    float F2Interval;

    float F3PosibilityFactor;
    float LowSideF1LowFactor;
    float LowSideF1HighFactor;
    float LowSideF2LowFactor;
    float LowSideF2HighFactor;
    float HighSideF2LowFactor;
    float HighSideF2HighFactor;
    float HighSideF3LowFactor;
    float HighSideF3HighFactor;
} FormantAnalyzerParameters;

extern FormantAnalyzerParameters APara;

extern void NormalizeSpectrum(float* Dest, int MaxIndex);

extern void ExtractFormantCPF(CPF* Dest, float* Spectrum, float BaseFrequency, int Amount);

extern int FormantEnvelopeFromComplex(float* Dest, float* SrcRe, float* SrcIm, float BaseFreq, float MaxFreq, int LPCLength, int Power);
extern int FormantEnvelopeFromWave(float* Dest, float* Src, float BaseFreq, float MaxFreq, int LPCLength, int Power);

extern void Analyzer_Init() __attribute__ ((constructor));
extern void Analyzer_SetParameters(FormantAnalyzerParameters Src);
extern FormantDescriptor AnalyzeFormantFromEnvelope(float* Src, int Length);

#ifdef __cplusplus
}
#endif

#endif
