/*
 * Formant.c
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
#include "Formant.h"
#include "DSPBase/Filter.h"
#include "DSPBase/LPC.h"
#include "DSPBase/Spectrum.h"
#include "DFT/FFT.h"
#include "DSPBase/Window.h"
#include "IntrinUtil/FloatArray.h"
#include "IntrinUtil/Calculation.h"
#include <stdio.h>

#define NVERBOSE

typedef struct
{
    float Freq;
    float Weight;
} WeightedFreq;

float WeightIntegration(float* Src, int From, int To)
{
    int i;
    float Sum = 0;
    for(i = From; i < To; i ++)
        Sum += Src[i];
    return Sum;
}

WeightedFreq WeightedFreqIntegration(float* Src, int From, int To)
{
    int i;
    WeightedFreq Ret;
    Ret.Freq = 0;
    Ret.Weight = 0;
    for(i = From; i < To; i ++)
    {
        Ret.Weight += Src[i];
        Ret.Freq += Src[i] * i;
    }
    Ret.Freq /= Ret.Weight;
    Ret.Weight /= To - From;
    return Ret;
}

void ExtractFormantCPF(CPF* Dest, float* Spectrum, float BaseFrequency, int Amount)
{
    int LIndex, UIndex;
    //Avoid small interval.
    if(BaseFrequency < 262)
        BaseFrequency = 262;
	float Freq = BaseFrequency;
	float HalfFreq = BaseFrequency * 0.5;
	float Max;
    float UBound = SampleRate / 2 - BaseFrequency;
    for(; Freq < UBound; Freq += BaseFrequency)
    {
        LIndex = (Freq - HalfFreq) * Amount / SampleRate;
        UIndex = (Freq + HalfFreq) * Amount / SampleRate;
        Max = Boost_FloatMax(Spectrum, LIndex, UIndex);

        ArrayType_PushNull(FreqPoint, Dest -> ControlPointList);
        Dest -> ControlPointList[Dest -> ControlPointList_Index].Freq      = Freq;
        Dest -> ControlPointList[Dest -> ControlPointList_Index].Magnitude = Max;
	}
}

FormantAnalyzerParameters APara;

/* Original Condition
APara.F1Min = 400;
APara.F1Max = 2000;
APara.F2Min = 1000;
APara.F2Max = 4000;
APara.F3Min = 3000;
APara.F3Max = 5000;

APara.PreEmphasisStrength = 1;

APara.FPeakDistanceWidth = 140;
APara.FPeakThreshold = 0.3;
APara.FPeakWeightWidth = 220;

APara.F2SearchRatioThreshold = 1.05;
APara.F2SearchWidth = 350;
APara.F2SearchWeightThreshold = 0.05;
APara.F2SearchWeightThresholdRatio = 0.25;
APara.F2SearchF3MinWeight = 0.03;

APara.F1F2WeightDeviationFactor = 20;
APara.F1F2WeightMinDeviation = 1750;

APara.HighFreqAdditionalRange = 500;
APara.HighLowFreqSearchInterval = 100;
APara.HighLowFreqSearchThreshold = 0.5;
APara.F2Interval = 200;

APara.F3PosibilityFactor = 2;
APara.LowSideF1LowFactor = 1;
APara.LowSideF1HighFactor = 1;
APara.LowSideF2LowFactor = 0.3333;
APara.LowSideF2HighFactor = 1.333;
APara.HighSideF2LowFactor = 0;
APara.HighSideF2HighFactor = 0.6666;
APara.HighSideF3LowFactor = 0.3333;
APara.HighSideF3HighFactor = 1;*/

void Analyzer_Init()
{
/*
    APara.F1Min = 400;
    APara.F1Max = 2000;
    APara.F2Min = 1000;
    APara.F2Max = 4000;
    APara.F3Min = 3000;
    APara.F3Max = 5000;

    APara.PreEmphasisStrength = 1;

    APara.FPeakDistanceWidth = 140;
    APara.FPeakThreshold = 0.3;
    APara.FPeakWeightWidth = 220;

    APara.F2SearchRatioThreshold = 1.05;
    APara.F2SearchWidth = 350;
    APara.F2SearchWeightThreshold = 0.05;
    APara.F2SearchWeightThresholdRatio = 0.25;
    APara.F2SearchF3MinWeight = 0.03;

    APara.F1F2WeightDeviationFactor = 20;
    APara.F1F2WeightMinDeviation = 1750;

    APara.HighFreqAdditionalRange = 500;
    APara.HighLowFreqSearchInterval = 100;
    APara.HighLowFreqSearchThreshold = 0.5;
    APara.F2Interval = 200;

    APara.F3PosibilityFactor = 2;
    APara.LowSideF1LowFactor = 1;
    APara.LowSideF1HighFactor = 1;
    APara.LowSideF2LowFactor = 0.3333;
    APara.LowSideF2HighFactor = 1.333;
    APara.HighSideF2LowFactor = 0;
    APara.HighSideF2HighFactor = 0.6666;
    APara.HighSideF3LowFactor = 0.3333;
    APara.HighSideF3HighFactor = 1;
*/
    APara.F1Min = 265.621155;
    APara.F1Max = 2243.843018;
    APara.F2Min = 738.711975;
    APara.F2Max = 4037.571045;
    APara.F3Min = 3337.207275;
    APara.F3Max = 4920.357910;

    APara.PreEmphasisStrength = 0.203728;
    APara.FPeakDistanceWidth = 124.339043;
    APara.FPeakThreshold = 0.451212;
    APara.FPeakWeightWidth = 231.016525;

    APara.F2SearchRatioThreshold = 0.160711;
    APara.F2SearchWidth = 492.520782;
    APara.F2SearchWeightThreshold = 0.041972;
    APara.F2SearchWeightThresholdRatio = 0.230699;
    APara.F2SearchF3MinWeight = 0.022611;

    APara.F1F2WeightDeviationFactor = 21.734838;
    APara.F1F2WeightMinDeviation = 1907.316162;

    APara.HighFreqAdditionalRange = 345.591095;
    APara.HighLowFreqSearchInterval = 89.311554;
    APara.HighLowFreqSearchThreshold = 0.516974;
    APara.F2Interval = 203.535492;

    APara.F3PosibilityFactor = 2.250160;
    APara.LowSideF1LowFactor = 1.064202;
    APara.LowSideF1HighFactor = 0.723672;
    APara.LowSideF2LowFactor = 0.469137;
    APara.LowSideF2HighFactor = 1.387626;
    APara.HighSideF2LowFactor = -0.058236;
    APara.HighSideF2HighFactor = 0.447298;
    APara.HighSideF3LowFactor = 0.557424;
    APara.HighSideF3HighFactor = 1.070801;

}
void Analyzer_SetParameters(FormantAnalyzerParameters Src)
{
    APara = Src;
}

#define FreqToIndex(x) ((int)(x * Amount / SampleRate))
#define IndexToFreq(x) ((float)x * SampleRate / Amount)
#define FloatMalloc(Size) (float*)malloc(sizeof(float) * Size)

void NormalizeSpectrum(float* Dest, int MaxIndex)
{
    int i;
    float Max, Min;
    Max = - 999; Min = 999;
    for(i = 0; i < MaxIndex; i ++)
    {
        Dest[i] = Boost_Ln(Dest[i]);
        if(Dest[i] > Max)
            Max = Dest[i];
        if(Dest[i] < Min)
            Min = Dest[i];
    }
    Boost_FloatSub(Dest, Dest, Min, MaxIndex);
    Boost_FloatDiv(Dest, Dest, Max - Min, MaxIndex);
}

int FormantEnvelopeFromComplex(float* Dest, float* SrcRe, float* SrcIm, float BaseFreq, float MaxFreq, int LPCLength, int Power)
{
    int i;
    int Amount = pow(2, Power);
    float* Spectrum, * LPCSpectrum, * Window, * LPCCoef;
    CPF EnvCPF;

    CPF_Ctor(& EnvCPF);
    EnvCPF.UFreq = SampleRate / 2;
    CPF_AddControlFreq(& EnvCPF, 0, 0);

    LPCSpectrum = FloatMalloc(Amount);
    Spectrum = FloatMalloc(Amount);
    Window = FloatMalloc(Amount);
    LPCCoef = FloatMalloc(LPCLength);

    int MaxIndex = FreqToIndex(MaxFreq);
    for(i = 0; i < MaxIndex; i ++)
    {
        SrcRe[i] *= 1.0f + (float)i / MaxIndex * APara.PreEmphasisStrength;
        SrcIm[i] *= 1.0f + (float)i / MaxIndex * APara.PreEmphasisStrength;
    }
    RIFFT(Window, SrcRe, SrcIm, Power);

    //LPC Spectrum
    LPCFromWave(LPCCoef, Window, Amount, LPCLength);
    SpectralEnvelopeFromLPC(LPCSpectrum, LPCCoef, LPCLength, Power);

    //FFT Spectrum Envelope
    MagnitudeFromComplex(Spectrum, SrcRe, SrcIm, Amount);
    ExtractFormantCPF(& EnvCPF, Spectrum, BaseFreq, Amount);
    CPF_Bake(Spectrum, & EnvCPF, Amount / 2);
    Boost_FloatAdd(Spectrum, Spectrum, 0.1, Amount);
    Boost_FloatMulArr(Dest, LPCSpectrum, Spectrum, MaxIndex);

    NormalizeSpectrum(Dest, MaxIndex);

    if(Boost_FloatSum(Dest, MaxIndex) < 25)
    {
        //Spectrum is too weak.
        Boost_FloatCopy(Dest, Spectrum, MaxIndex);
        NormalizeSpectrum(Dest, MaxIndex);
    }

    CPF_Dtor(& EnvCPF);
    free(LPCCoef);
    free(LPCSpectrum);
    free(Spectrum);
    free(Window);

    return MaxIndex;
}

int FormantEnvelopeFromWave(float* Dest, float* Src, float BaseFreq, float MaxFreq, int LPCLength, int Power)
{
    int Amount = pow(2, Power);
    int Ret;
    float* FFTRe = FloatMalloc(Amount);
    float* FFTIm = FloatMalloc(Amount);
    RFFT(FFTRe, FFTIm, Src, Power);
    Ret = FormantEnvelopeFromComplex(Dest, FFTRe, FFTIm, BaseFreq, MaxFreq, LPCLength, Power);
    free(FFTRe);
    free(FFTIm);
    return Ret;
}
#undef FreqToIndex
#undef IndexToFreq

#define FreqToIndex(x) ((int)((x) * Length / SampleRate))
#define IndexToFreq(x) ((float)(x) * SampleRate / Length)
#define min(a, b) ((a) > (b) ? (b) : (a))
#define max(a, b) ((a) > (b) ? (a) : (b))
FormantDescriptor AnalyzeFormantFromEnvelope(float* Spectrum, int Length)
{
    int i, j;
    WeightedFreq FPeak;
    WeightedFreq F1, F2, F3;

    float AverageDistance = 0; j = 0;
    int FPeakIndex = FreqToIndex(APara.FPeakDistanceWidth);
    for(i = 0; i < FreqToIndex(APara.F1Max); i ++)
        if(Spectrum[i] > Spectrum[i + 1] && Spectrum[i] > Spectrum[i - 1])
        {
            AverageDistance += min(Spectrum[i] - Spectrum[i + FPeakIndex],
                                   Spectrum[i] - Spectrum[i - FPeakIndex]);
            j ++;
        }
    AverageDistance /= j;

    //------------------
    //Obtain FPeak
    for(i = FreqToIndex(APara.F1Max); i > 0; i --)
    {
        if(Spectrum[i] > Spectrum[i + 1] && Spectrum[i] > Spectrum[i - 1])
        {
            float Distance;
            Distance = min(Spectrum[i] - Spectrum[i + FPeakIndex],
                           Spectrum[i] - Spectrum[i - FPeakIndex]);
            if(Distance > AverageDistance * APara.FPeakThreshold)
                break;
        }
    }
    int FPeakWeightWidthIndex = FreqToIndex(APara.FPeakWeightWidth);
    FPeak.Freq = i;
    FPeak.Weight = WeightIntegration(Spectrum, i - FPeakWeightWidthIndex, i + FPeakWeightWidthIndex) / FPeakWeightWidthIndex / 2;
#ifdef VERBOSE
    printf("Peak: %f\n", IndexToFreq(FPeak.Freq));
    printf("Weight: %f\n", FPeak.Weight);
#endif
    float HighFreqWeight = Boost_FloatMax(Spectrum, FreqToIndex(APara.F3Min), FreqToIndex(APara.F3Max));
#ifdef VERBOSE
    printf("HighFreqWeight: %f\n", HighFreqWeight);
#endif

    //------------------
    //Search for Mid-Freq F2.
    int FoundF2 = 0;
    float MaxRatio = APara.F2SearchRatioThreshold;
    F2.Freq = 0; F2.Weight = 0;
    for(i = FreqToIndex(APara.F1Max); i < FreqToIndex(APara.F3Min); i ++)
        if(Spectrum[i] > Spectrum[i + 1] && Spectrum[i] > Spectrum[i - 1])
        {
            WeightedFreq TmpF2;
            float ForwardMin, BackwardMin, Min;
            ForwardMin = Boost_FloatMin(Spectrum, i, i + FreqToIndex(APara.F2SearchWidth));
            BackwardMin = Boost_FloatMin(Spectrum, i - FreqToIndex(APara.F2SearchWidth), i);
            Min = max(ForwardMin, BackwardMin);
            TmpF2.Freq = i;
            TmpF2.Weight = Spectrum[i];

            //printf("Ratio: %f\n", (TmpF2.Weight - Min) / HighFreqWeight);
            //TmpF2.Weight = WeightIntegration(Spectrum, i - FPeakWeightWidthIndex, i + FPeakWeightWidthIndex) / FPeakWeightWidthIndex / 2;
            /* F2 shoud be higher than nearer values */
            /* F2 shoudn't be too low unless F3 is also very low */
            if((TmpF2.Weight - Min) / HighFreqWeight > MaxRatio
                    && (!(TmpF2.Weight < APara.F2SearchWeightThreshold && HighFreqWeight > APara.F2SearchF3MinWeight))
                    && (!(TmpF2.Weight < HighFreqWeight * APara.F2SearchWeightThresholdRatio && HighFreqWeight > APara.F2SearchF3MinWeight)))
            {
                MaxRatio = (TmpF2.Weight - Min) / HighFreqWeight;
                //printf("MaxRatio: %f\n", MaxRatio);
                F2 = TmpF2;
                FoundF2 = 1;
            }
        }

    //Omit peaks close to F1.
    if(FoundF2)
    {
#ifdef VERBOSE
        printf("Mid F2: %f\n", IndexToFreq(F2.Freq));
        printf("Weight: %f\n", F2.Weight);
        printf("Ratio: %f\n", MaxRatio);
#endif
        float FreqDeviation = F2.Freq - FPeak.Freq;
        float WeightDeviation = (F2.Weight - FPeak.Weight) * APara.F1F2WeightDeviationFactor;
        float Distance = Boost_Sqr(FreqDeviation * FreqDeviation + WeightDeviation * WeightDeviation);
        float Min = Boost_FloatMin(Spectrum, FPeak.Freq, F2.Freq);
        Distance *= (F2.Weight - Min) * APara.F1F2WeightDeviationFactor;
        if(Distance < FreqToIndex(APara.F1F2WeightMinDeviation))
            FoundF2 = 0;
    }

    //------------------
    //Obtain F1\F3 Range
    float LowFreqAvg = WeightIntegration(Spectrum, 0, FreqToIndex(APara.F1Max)) / FreqToIndex(APara.F1Max);
    float HighFreqAvg = WeightIntegration(Spectrum, FreqToIndex(APara.F3Min - APara.HighFreqAdditionalRange), FreqToIndex(APara.F3Max)) / FreqToIndex(APara.F3Max - APara.F3Min + APara.HighFreqAdditionalRange);

    int F1HighIndex, F3LowIndex;
    int F1RangeWidth, F3RangeWidth;
    float RangeAvg, LastAvg;
    LastAvg = WeightIntegration(Spectrum, 0,  FreqToIndex(APara.HighLowFreqSearchInterval)) / FreqToIndex(APara.HighLowFreqSearchInterval);
    for(i = 1; i < FreqToIndex(APara.F1Max + 1000); i += FreqToIndex(APara.HighLowFreqSearchInterval))
    {
        RangeAvg = WeightIntegration(Spectrum, i, i + FreqToIndex(APara.HighLowFreqSearchInterval)) / FreqToIndex(APara.HighLowFreqSearchInterval);
        if(RangeAvg < LastAvg && RangeAvg < LowFreqAvg * APara.HighLowFreqSearchThreshold)
            break;
        LastAvg = RangeAvg;
    }
    F1HighIndex = i;

    int F3MinIndex = FreqToIndex(APara.F3Min - APara.HighFreqAdditionalRange);
    if(FoundF2)
        F3MinIndex = F2.Freq + FreqToIndex(APara.F2Interval);
    LastAvg = WeightIntegration(Spectrum, F3MinIndex - FreqToIndex(APara.HighLowFreqSearchInterval), F3MinIndex) / FreqToIndex(APara.HighLowFreqSearchInterval);
    for(i = F3MinIndex; i < FreqToIndex(APara.F3Max); i += FreqToIndex(APara.HighLowFreqSearchInterval))
    {
        RangeAvg = WeightIntegration(Spectrum, i, i + FreqToIndex(APara.HighLowFreqSearchInterval)) / FreqToIndex(APara.HighLowFreqSearchInterval);
        if(RangeAvg > LastAvg && RangeAvg > HighFreqAvg * APara.HighLowFreqSearchThreshold)
            break;
        LastAvg = RangeAvg;
    }
    F3LowIndex = i;
    if(i >= FreqToIndex(APara.F3Max) - 1)
    {
        //No Decrease
        F3LowIndex = FreqToIndex(APara.F3Min);
    }
    F1RangeWidth = F1HighIndex; F3RangeWidth = FreqToIndex(APara.F3Max) - F3LowIndex;
#ifdef VERBOSE
    printf("F1HighIndex: %f\n", IndexToFreq(F1HighIndex));
    printf("F1Avg: %f\n", LowFreqAvg);
    printf("F3LowIndex: %f\n", IndexToFreq(F3LowIndex));
    printf("F3Avg: %f\n", HighFreqAvg);
#endif

    //Obtain F1\F3 Intensity
    WeightedFreq F1Intensity, F3Intensity;
    F1Intensity = WeightedFreqIntegration(Spectrum, 0, F1HighIndex);
    F3Intensity = WeightedFreqIntegration(Spectrum, F3LowIndex, FreqToIndex(APara.F3Max));

    //------------------
    //Obtain F1\F3 if found F2
    if(FoundF2)
    {
        F1 = WeightedFreqIntegration(Spectrum, 0, FreqToIndex(APara.F1Max));
        F3 = WeightedFreqIntegration(Spectrum, FreqToIndex(APara.F3Min), FreqToIndex(APara.F3Max));
    }else
    {
        //F2 is either in low freq or high freq
        float F1Posibility, F3Posibility;
        F1Posibility = F1Intensity.Weight * F1RangeWidth * F1RangeWidth;
        F3Posibility = F3Intensity.Weight * F3RangeWidth * F3RangeWidth * APara.F3PosibilityFactor;
#ifdef VERBOSE
        printf("F1Pos: %f\n", F1Posibility);
        printf("F3Pos: %f\n", F3Posibility);
#endif

        if(F1Posibility > F3Posibility)
        {
            //F2 is on low freq side
            F1 = WeightedFreqIntegration(Spectrum, FreqToIndex(APara.F1Min * APara.LowSideF1LowFactor), F1HighIndex * APara.LowSideF1HighFactor);
            F2 = WeightedFreqIntegration(Spectrum, F1HighIndex * APara.LowSideF2LowFactor, F1HighIndex * APara.LowSideF2HighFactor);
            F3 = F3Intensity;
        }else
        {
            //F2 is on high freq side
            F1 = F1Intensity;
            F2 = WeightedFreqIntegration(Spectrum, F3LowIndex + F3RangeWidth * APara.HighSideF2LowFactor, F3LowIndex + F3RangeWidth * APara.HighSideF2HighFactor);
            F3 = WeightedFreqIntegration(Spectrum, F3LowIndex + F3RangeWidth * APara.HighSideF3LowFactor, FreqToIndex(APara.F3Max * APara.HighSideF3HighFactor));
        }
    }
#ifdef VERBOSE
    printf("F1: %f\n", IndexToFreq(F1.Freq));
    printf("F2: %f\n", IndexToFreq(F2.Freq));
    printf("F3: %f\n", IndexToFreq(F3.Freq));
#endif

    FormantDescriptor Ret;
    Ret.F1 = IndexToFreq(F1.Freq);
    Ret.F2 = IndexToFreq(F2.Freq);
    Ret.F3 = IndexToFreq(F3.Freq);
    return Ret;
}
#undef FreqToIndex
#undef IndexToFreq
