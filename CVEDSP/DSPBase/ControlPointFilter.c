/*
 * ControlPointFilter.c
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
#include "ControlPointFilter.h"
#include <stdlib.h>
#include <stdio.h>
#include "IntrinUtil/FloatArray.h"
#include "Interpolation.h"
#include "Filter.h"

_Constructor_ (CPF)
{
    ArrayType_Init(FreqPoint, Dest -> ControlPointList);
    Dest -> UFreq = SampleRate / 2;
}

_Destructor_ (CPF)
{
    ArrayType_Dtor(FreqPoint, Dest -> ControlPointList);
}

void CPF_AddControlPoint(CPF* Dest, FreqPoint* FPoint)
{
    int i;
    for(i = 0; i <= Dest -> ControlPointList_Index; i ++)
        if(FPoint -> Freq <= Dest -> ControlPointList[i].Freq)
            break;
	//Insert at index
    ArrayType_Insert(FreqPoint, Dest -> ControlPointList, i, *FPoint);
}

void CPF_AddControlFreq(CPF* Dest, float Freq, float Magnitude)
{
    int i;
    for(i = 0; i <= Dest -> ControlPointList_Index; i ++)
		if(Freq <= Dest -> ControlPointList[i].Freq)
			break;
    //Insert at index
    ArrayType_InsertNull(FreqPoint, Dest -> ControlPointList, i);
    Dest -> ControlPointList[i].Freq      = Freq;
    Dest -> ControlPointList[i].Magnitude = Magnitude;
}

#define SrcList Src -> ControlPointList
#define SrcIndex Src -> ControlPointList_Index
void CPF_Bake(float* Dest, CPF* Src, int DestLen)
{
	int i, j;
    int LIndex, UIndex;
    float Factor;
    float M0, M1;
    UIndex = 0; LIndex = 0;

    ArrayType_PushNull(FreqPoint, Src -> ControlPointList);
    SrcList[SrcIndex].Freq      = Src -> UFreq;
    SrcList[SrcIndex].Magnitude = 0;
    ArrayType_PushNull(FreqPoint, Src -> ControlPointList);
    SrcList[SrcIndex].Freq      = Src -> UFreq;
    SrcList[SrcIndex].Magnitude = 0;

    Factor = (float)DestLen / (Src -> UFreq);

    for(i = 0; i < SrcIndex; i ++)
    {
        LIndex = (SrcList[i + 0].Freq) * Factor;
        UIndex = (SrcList[i + 1].Freq) * Factor;
        M0 = SrcList[i + 0].Magnitude;
        M1 = SrcList[i + 1].Magnitude;

        float Count = M0;
        float Increment = (M1 - M0) / (UIndex - LIndex);
        for(j = LIndex; j < UIndex - 1; j += 2)
        {
            Dest[j] = Count;
            Count += Increment;
            Dest[j + 1] = Count;
            Count += Increment;
        }
        if(j < UIndex)
            Dest[j] = Count;
            Count += Increment;
    }
    SrcIndex -= 2;
}

void CPF_Print(CPF* Src)
{
	int i;
    for(i = 0; i <= SrcIndex; i ++)
	{
        printf("%f : %f\n", SrcList[i].Freq, SrcList[i].Magnitude);
	}
}
#undef SrcIndex
#undef SrcList
