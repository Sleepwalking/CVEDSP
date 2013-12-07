/*
 * PSOLA.c
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
#include "PSOLA.h"
#include "IntrinUtil/FloatArray.h"
#include "FrameProcessor.h"
#include "DSPBase/Filter.h"
#include "DSPBase/TDEffects.h"
#include "SpeechDetector.h"
#include <stdlib.h>
#include <stdio.h>

float VolumeThreshold = 0.005;
float VoiceThreshold = 0.005;

int32_t PSOLAFrame_SpawnSize = 1024;

_Constructor_ (PSOLAFrame)
{
    Dest -> Data = malloc(sizeof(float) * PSOLAFrame_SpawnSize);
    Dest -> Length = PSOLAFrame_SpawnSize;
}

_Destructor_ (PSOLAFrame)
{
    free(Dest -> Data);
}

void PSOLAFrame_CtorSize(PSOLAFrame *Dest, int32_t Length)
{
    Dest -> Data = malloc(sizeof(float) * Length);
    Dest -> Length = Length;
}

void PSOLAFrame_DtorAll(PSOLAFrame* Dest, int32_t Amount)
{
    int i;
    for(i = 0; i < Amount; i ++)
        PSOLAFrame_Dtor(Dest + i);
}

void PSOLAFrame_Resize(PSOLAFrame* Dest, int32_t Length)
{
    Dest -> Data = (float*)realloc(Dest -> Data, sizeof(float) * Length);
    Dest -> Length = Length;
}

void PSOLAFrame_Get(PSOLAFrame* Dest, float* Src, int32_t Position)
{
    Boost_FloatCopy(Dest -> Data, Src + Position - Dest -> Length / 2, Dest -> Length);
}

void PSOLAFrame_SecureGet(PSOLAFrame* Dest, float* Src, int32_t UBound, int32_t Position)
{
    int Length = Dest -> Length;
    int Offset = 0;
    Boost_FloatSet(Dest -> Data, 0, Length);

    if(Position < Length / 2)
    {
        Offset = Length / 2 - Position;
        Length -= Offset;
    }
    if(Position + (Dest -> Length) / 2 > UBound)
        Length -= Position + (Dest -> Length) / 2 - UBound;

    Boost_FloatCopy(Dest -> Data + Offset, Src + Position - (Dest -> Length / 2) + Offset, Length);
}

void PSOLAFrame_StaticGet(PSOLAFrame* Dest, float* Src, int32_t Position)
{
    Dest -> Data = Src + Position - Dest -> Length / 2;
}

void PSOLAFrame_Mix(float* Dest, PSOLAFrame* Src, int32_t Front, int32_t Position, int32_t Back)
{
    int32_t i;
    int32_t half = Src -> Length / 2;
    float ratio;
    for(i = 0; i < Position - Front; i ++)
    {
        ratio = (float)i / (Position - Front);
        Dest[i + Front] += ratio * Src -> Data[half - (Position - Front) + i];
    }
    for(i = 0; i < Back - Position; i ++)
    {
        ratio = 1.0f - (float)i / (Back - Position);
        Dest[i + Position] += ratio * Src -> Data[half + i];
    }
}

void PSOLAFrame_MixByRatio(float* Dest, PSOLAFrame* Src, int32_t Front, int32_t Position, int32_t Back, float Ratio, int MType)
{
    int32_t i;
    int32_t half = Src -> Length / 2;
    //float ratio_;

    int32_t Index = half - (Position - Front);
    float Dist = Position - Front;
    float Dist2 = Back - Position;
    float* Tmp = (float*)malloc(sizeof(float) * Src -> Length);
    if(MType == MixNormal || MType == MixBack)
    {

        for(i = 0; i < Dist; i ++)
            Tmp[i] = i;
        Boost_FloatMul(Tmp, Tmp, Ratio / Dist, Dist);
        Boost_FloatMulArr(Tmp, Src -> Data + Index, Tmp, Dist);
        Boost_FloatAddArr(Dest + Front, Dest + Front, Tmp, Dist);
        /*
        for(i = 0; i < Dist; i ++)
        {
            ratio_ = (float)i / Dist;
            Dest[i + Front] += Ratio * ratio_ * Src -> Data[Index + i];
        }*/
    }
    else
    {
        Boost_FloatMul(Tmp, Src -> Data + Index, Ratio, Dist);
        Boost_FloatAddArr(Dest + Front, Dest + Front, Tmp, Dist);
    }
    if(MType == MixNormal || MType == MixFront)
    {
        for(i = 0; i < Dist2; i ++)
            Tmp[i] = i;
        Boost_FloatDiv(Tmp, Tmp, Dist2, Dist2);
        Boost_FloatSubBy(Tmp, Tmp, 1, Dist2);
        Boost_FloatMul(Tmp, Tmp, Ratio, Dist2);
        Boost_FloatMulArr(Tmp, Src -> Data + half, Tmp, Dist2);
        Boost_FloatAddArr(Dest + Position, Dest + Position, Tmp, Dist2);
        /*
        for(i = 0; i < Dist2; i ++)
        {
            ratio_ = 1.0f - (float)i / Dist2;
            Dest[i + Position] += Ratio * ratio_ * Src -> Data[half + i];
        }*/
    }
    else
    {
        Boost_FloatMul(Tmp, Src -> Data + half, Ratio, Dist2);
        Boost_FloatAddArr(Dest + Position, Dest + Position, Tmp, Dist2);
    }
    free(Tmp);
}

void PSOLAFrame_MixByRatio_Float(float* Dest, PSOLAFrame* Src, float Front, float Position, float Back, float Ratio, int MType)
{
    int32_t i;
    int32_t half = Src -> Length / 2;
    int32_t Front_Int, Position_Int, Back_Int;
    float ratio_;
    float Total;
    float IntRatio;
    Front_Int = (int)Front;
    Position_Int = (int)Position;
    Back_Int = (int)Back;
    IntRatio = Position - (float)((int)Position);
    if(MType == MixNormal || MType == MixBack)
        for(i = 0; i < Position_Int - Front_Int; i ++)
        {
            ratio_ = (float)i / (Position - Front);
            Total = Ratio * ratio_ * Src -> Data[half - (int)(Position - Front) + i];
            Dest[i + Front_Int + 0] += Total * (1.0f - IntRatio);
            Dest[i + Front_Int + 1] += Total * IntRatio;
        }
    else
        for(i = 0; i < Position_Int - Front_Int; i ++)
        {
            Total = Ratio * Src -> Data[half - (int)(Position - Front) + i];
            Dest[i + Front_Int + 0] += Total * (1.0f - IntRatio);
            Dest[i + Front_Int + 1] += Total * IntRatio;
        }
    if(MType == MixNormal || MType == MixFront)
        for(i = 0; i < Back_Int - Position_Int; i ++)
        {
            ratio_ = 1.0f - (float)i / (Back - Position);
            Total = Ratio * ratio_ * Src -> Data[half + i];
            Dest[i + Position_Int + 0] += Total * (1.0f - IntRatio);
            Dest[i + Position_Int + 1] += Total * IntRatio;
        }
    else
        for(i = 0; i < Back_Int - Position_Int; i ++)
        {
            Total = Ratio * Src -> Data[half + i];
            Dest[i + Position_Int + 0] += Total * (1.0f - IntRatio);
            Dest[i + Position_Int + 1] += Total * IntRatio;
        }
}

void SetExtractorVoiceThreshold(float _Threshold)
{
    VoiceThreshold = _Threshold;
}

void SetExtractorVolumeThreshold(float _Threshold)
{
    VolumeThreshold = _Threshold;
}

void ExtractPulsesByBaseFrequency(int32_t* DestPulses, PulseDescriptor* DestDescriptor, float* Wave, float BaseFrequency, int32_t Length)
{
    float* Averaged = malloc(sizeof(float) * Length);
    int32_t i;
    int32_t BasePeriod = SampleRate / BaseFrequency;
    int32_t VoiceOnsetRecord;
    int32_t SilenceTime;

    SpeechDetectorPreProcess(Averaged, Wave, BaseFrequency, Length);

    DestDescriptor -> Amount = 0;
    DestDescriptor -> VoiceOnsetIndex = - 1;

    VoiceOnsetRecord = DetectVOTWithBaseFreq(Averaged, BaseFrequency, Length) * SampleRate;
    SilenceTime = DetectVocalOnset(Wave, Length) * SampleRate;

    for(i = SilenceTime; i < Length - BasePeriod; i ++)
    {
        if(i < VoiceOnsetRecord)
        {
            //Unvoiced
            //Voice ended.
            if(DestDescriptor -> VoiceOnsetIndex != - 1)
                break;
            i += BasePeriod;
            DestPulses[DestDescriptor -> Amount] = i;
            DestDescriptor -> Amount ++;
            continue;
        }else
        {
            //Voiced
            if(Averaged[i] < 0 && Averaged[i + 1] > 0)
            {
                if(DestDescriptor -> VoiceOnsetIndex == - 1)
                    DestDescriptor -> VoiceOnsetIndex = DestDescriptor -> Amount;
                DestPulses[DestDescriptor -> Amount] = i;
                DestDescriptor -> Amount ++;
            }
        }
    }
    free(Averaged);
}

void PSOLA_Regenesis(float* Dest, PSOLAFrame* Frames, int32_t* NewPulses, int32_t* OriginalPulses, int32_t PulseNum)
{
    int i;
    for(i = 1; i < PulseNum; i ++)
    {
        PSOLAFrame_Mix(Dest, Frames + i,
                       NewPulses[i] - (OriginalPulses[i] - OriginalPulses[i - 1]),
                       NewPulses[i],
                       NewPulses[i] + (OriginalPulses[i + 1] - OriginalPulses[i]));
    }
}
