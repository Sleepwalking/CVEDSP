/*
 * LinearWarping.c
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
#include "LinearWarping.h"
#include <stdlib.h>
#include <stdio.h>
#include "IntrinUtil/FloatArray.h"

_Constructor_ (LinearWarping)
{
    ArrayType_Init(WarpPair, Dest -> WarpList);
    Dest -> UWarp = 22050;
}
_Destructor_ (LinearWarping)
{
    ArrayType_Dtor(WarpPair, Dest -> WarpList);
}

void LinearWarping_AddControlPoint(LinearWarping* Dest, WarpPair* Src)
{
    int i;
    for(i = 0; i <= Dest -> WarpList_Index; i ++)
        if(Src -> To <= Dest -> WarpList[i].To)
            break;
    //Insert at index
    ArrayType_Insert(WarpPair, Dest -> WarpList, i, *Src);
}

void LinearWarping_AddControlFreq(LinearWarping* Dest, float From, float To)
{
    int i;
    for(i = 0; i <= Dest -> WarpList_Index; i ++)
        if(To <= Dest -> WarpList[i].To)
            break;
    //Insert at index
    ArrayType_InsertNull(WarpPair, Dest -> WarpList, i);
    Dest -> WarpList[i].From = From;
    Dest -> WarpList[i].To   = To;
}

#define SrcList Src -> WarpList
#define SrcIndex Src -> WarpList_Index
void LinearWarping_Bake(float* Dest, LinearWarping* Src, int DestLen)
{
	int i, j;
	int LIndex, UIndex;
	float Ratio;
	float MagRatio;

    MagRatio = ((float)DestLen) / (Src -> UWarp);
    SrcList[SrcIndex].From = Src -> UWarp;
    SrcList[SrcIndex].To   = Src -> UWarp;
    for(i = 0; i <= SrcIndex; i ++)
	{
        LIndex = (SrcList[i + 0].To) * DestLen / (Src -> UWarp);
        UIndex = (SrcList[i + 1].To) * DestLen / (Src -> UWarp);
        for(j = LIndex; j < UIndex; j ++)
		{
			Ratio = ((float)(j - LIndex)) / (UIndex - LIndex);
            Dest[j] = (SrcList[i + 0].From * (1.0f - Ratio) +
                       SrcList[i + 1].From * Ratio) * MagRatio;
		}
	}
    SrcIndex --;
}

void LinearWarping_Print(LinearWarping* Src)
{
	int i;
    for(i = 0; i <= SrcIndex; i ++)
	{
        printf("%f -> %f\n", SrcList[i].From, SrcList[i].To);
	}
}
#undef SrcList
#undef SrcIndex
