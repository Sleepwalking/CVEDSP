/*
 * FECSOLA.h
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
#ifndef FECSOLA_H
#define FECSOLA_H
#include "PSOLA.h"
#include "DSPBase/ControlPointFilter.h"
//Formant Envelope Coefficient Shift and OverLap Add

#include "RUtil/RUtil.h"

#ifdef __cplusplus
extern "C"{
#endif

AutoClass
{
	int Size;
	
	PSOLAFrame LLimit;
	PSOLAFrame ULimit;
	
	PSOLAFrame F0;
	PSOLAFrame F1;
	PSOLAFrame F2;
	PSOLAFrame F3;

	float SF0; //Frequency of Formant 1
	float SF1; //Frequency of Formant 2
	float SF2; //Frequency of Formant 3
	float SF3; //Frequency of Formant 4
	
} EndClass(FECSOLAFilter);

typedef struct FECSOLAState
{
	float F0; //Frequency of Formant 1
	float F1; //Frequency of Formant 2
	float F2; //Frequency of Formant 3
	float F3; //Frequency of Formant 4

	float L0; //Width of Formant 1
	float L1; //Width of Formant 2
	float L2; //Width of Formant 3
	float L3; //Width of Formant 4

	float S0; //Strength of Formant 1
	float S1; //Strength of Formant 2
	float S2; //Strength of Formant 3
	float S3; //Strength of Formant 4
} FECSOLAState;

#define FECSOLAState_CreateFormant(FState, _F0, _F1, _F2, _F3, _L0, _L1, _L2, _L3, _S0, _S1, _S2, _S3) \
	(FState).F0 = (_F0); (FState).F1 = (_F1); (FState).F2 = (_F2); (FState).F3 = (_F3);\
	(FState).L0 = (_L0); (FState).L1 = (_L1); (FState).L2 = (_L2); (FState).L3 = (_L3);\
    (FState).S0 = (_S0); (FState).S1 = (_S1); (FState).S2 = (_S2); (FState).S3 = (_S3)

extern void FECSOLAFilter_CtorSize(FECSOLAFilter* Dest, int Size);

#define FECSOLAFilter_GetFromCPF FECSOLAFilter_GetFromFormantEnvelope
extern void FECSOLAFilter_GetFromFormantEnvelope(FECSOLAFilter* Dest, CPF* FormantEnvelope, FECSOLAState* State);
extern void FECSOLAFilter_Bake(float* Dest, FECSOLAFilter* Filter, FECSOLAState* State, int Length);

extern void FECSOLAState_Transition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio);
extern inline void FECSOLAState_FTransition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio);
extern inline void FECSOLAState_LTransition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio);
extern inline void FECSOLAState_STransition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio);

#ifdef __cplusplus
}
#endif

#endif
