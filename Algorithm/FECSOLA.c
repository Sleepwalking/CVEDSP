/*
 * FECSOLA.c
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
#include "FECSOLA.h"
#include "DSPBase/Filter.h"
#include "IntrinUtil/FloatArray.h"

float AntiAliasSpec[16384];

#define SFreq(x) ((x) * (Filter -> Size) / SampleRate)
#define min(a, b) ((a) > (b) ? (b) : (a))
#define max(a, b) ((a) > (b) ? (a) : (b))

void FECSOLAFilter_CtorSize(FECSOLAFilter* Dest, int Size)
{
    Dest -> Size = Size;

    PSOLAFrame_CtorSize(& Dest -> LLimit, Size);
    PSOLAFrame_CtorSize(& Dest -> ULimit, Size);
	
    PSOLAFrame_CtorSize(& Dest -> F0, Size);
    PSOLAFrame_CtorSize(& Dest -> F1, Size);
    PSOLAFrame_CtorSize(& Dest -> F2, Size);
    PSOLAFrame_CtorSize(& Dest -> F3, Size);
}

_Constructor_ (FECSOLAFilter)
{
    FECSOLAFilter_CtorSize(Dest, 2048);
}

_Destructor_ (FECSOLAFilter)
{
	PSOLAFrame_Dtor(& Dest -> LLimit);
	PSOLAFrame_Dtor(& Dest -> ULimit);
	
	PSOLAFrame_Dtor(& Dest -> F0);
	PSOLAFrame_Dtor(& Dest -> F1);
	PSOLAFrame_Dtor(& Dest -> F2);
	PSOLAFrame_Dtor(& Dest -> F3);
}

void FECSOLAFilter_GetFromFormantEnvelope(FECSOLAFilter* Dest, CPF* FormantEnvelope, FECSOLAState* State)
{
    CPF_Bake(AntiAliasSpec, FormantEnvelope, Dest -> Size / 2);

	PSOLAFrame_SecureGet(& Dest -> LLimit, AntiAliasSpec, Dest -> Size, 0);
    PSOLAFrame_SecureGet(& Dest -> ULimit, AntiAliasSpec, Dest -> Size, 6000 * Dest -> Size / SampleRate);
	
	PSOLAFrame_SecureGet(& Dest -> F0, AntiAliasSpec, Dest -> Size, State -> F0 * Dest -> Size / SampleRate);
	PSOLAFrame_SecureGet(& Dest -> F1, AntiAliasSpec, Dest -> Size, State -> F1 * Dest -> Size / SampleRate);
	PSOLAFrame_SecureGet(& Dest -> F2, AntiAliasSpec, Dest -> Size, State -> F2 * Dest -> Size / SampleRate);
	PSOLAFrame_SecureGet(& Dest -> F3, AntiAliasSpec, Dest -> Size, State -> F3 * Dest -> Size / SampleRate);

	Boost_FloatDiv(Dest -> F0.Data, Dest -> F0.Data, State -> S0, Dest -> Size);
	Boost_FloatDiv(Dest -> F1.Data, Dest -> F1.Data, State -> S1, Dest -> Size);
	Boost_FloatDiv(Dest -> F2.Data, Dest -> F2.Data, State -> S2, Dest -> Size);
	Boost_FloatDiv(Dest -> F3.Data, Dest -> F3.Data, State -> S3, Dest -> Size);

	Dest -> SF0 = State -> F0;
	Dest -> SF1 = State -> F1;
	Dest -> SF2 = State -> F2;
	Dest -> SF3 = State -> F3;
}

void FECSOLAFilter_Bake(float* Dest, FECSOLAFilter* Filter, FECSOLAState* State, int Length)
{
    int i, j;
    Boost_FloatSet(AntiAliasSpec, 0, Filter -> Size);

    PSOLAFrame_MixByRatio      (AntiAliasSpec, & Filter -> ULimit,
	                            SFreq(State -> F3),
                                SFreq(6000),
                                SFreq(6000),
	                            1,
                                MixBack);
    PSOLAFrame_MixByRatio      (AntiAliasSpec, & Filter -> F0,
	                            0,
	                            SFreq(State -> F0),
                                SFreq(State -> F0 + Filter -> SF1 - Filter -> SF0),
	                            State -> S0,
                                MixFront);
    PSOLAFrame_MixByRatio      (AntiAliasSpec, & Filter -> F1,
                                SFreq(max(0, max(State -> F0 - 100, State -> F1 + Filter -> SF0 - Filter -> SF1))),
	                            SFreq(State -> F1),
	                            SFreq(State -> F1 + Filter -> SF2 - Filter -> SF1),
	                            State -> S1,
                                MixNormal);
    PSOLAFrame_MixByRatio      (AntiAliasSpec, & Filter -> F2,
                                SFreq(max(0, max(State -> F0 - 100, State -> F2 + Filter -> SF1 - Filter -> SF2))),
	                            SFreq(State -> F2),
	                            SFreq(State -> F2 + Filter -> SF3 - Filter -> SF2),
	                            State -> S2,
	                            MixNormal);
    PSOLAFrame_MixByRatio      (AntiAliasSpec, & Filter -> F3,
                                SFreq(max(0, max(State -> F0 - 100, State -> F3 + Filter -> SF2 - Filter -> SF3))),
	                            SFreq(State -> F3),
                                SFreq(6000),
	                            State -> S3,
                                MixNormal);
	Boost_FloatSet(Dest, 0, Length);
    int SFactor = (float)Filter -> Size / Length;

    //Optimized
    switch(SFactor)
    {
    case 1:
        Boost_FloatCopy(Dest, AntiAliasSpec, Length);
        break;
    case 2:
        for(i = 0; i < Length; i ++)
        {
            Dest[i] = AntiAliasSpec[i * SFactor];
            Dest[i] += AntiAliasSpec[i * SFactor + 1];
            Dest[i] /= 2;
        }
        break;
    case 3:
        for(i = 0; i < Length; i ++)
        {
            Dest[i] = AntiAliasSpec[i * SFactor];
            Dest[i] += AntiAliasSpec[i * SFactor + 1];
            Dest[i] += AntiAliasSpec[i * SFactor + 2];
            Dest[i] /= 3;
        }
        break;
    case 4:
        for(i = 0; i < Length; i ++)
        {
            Dest[i] = AntiAliasSpec[i * SFactor];
            Dest[i] += AntiAliasSpec[i * SFactor + 1];
            Dest[i] += AntiAliasSpec[i * SFactor + 2];
            Dest[i] += AntiAliasSpec[i * SFactor + 3];
            Dest[i] /= 4;
        }
        break;
    default:
        for(i = 0; i < Length; i ++)
        {
            for(j = i * SFactor; j < i * SFactor + SFactor; j ++)
            {
                Dest[i] += AntiAliasSpec[j];
            }
            Dest[i] /= SFactor;
        }
    }

}

void FECSOLAState_Transition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio)
{
	FECSOLAState_FTransition(Dest, From, To, Ratio);
	FECSOLAState_LTransition(Dest, From, To, Ratio);
	FECSOLAState_STransition(Dest, From, To, Ratio);
}

#define Transition(Property) Dest -> Property = (1.0f - Ratio) * From -> Property + Ratio * To -> Property
inline void FECSOLAState_FTransition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio)
{
    Transition(F0);
    Transition(F1);
    Transition(F2);
    Transition(F3);
}
inline void FECSOLAState_LTransition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio)
{
    Transition(L0);
    Transition(L1);
    Transition(L2);
    Transition(L3);
}
inline void FECSOLAState_STransition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio)
{
    Transition(S0);
    Transition(S1);
    Transition(S2);
    Transition(S3);
}
