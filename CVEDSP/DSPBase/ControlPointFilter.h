/*
 * ControlPointFilter.h
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
#ifndef CONTROLPOINTFILTER_H
#define CONTROLPOINTFILTER_H

#include "../RUtil/RUtil.h"

#ifdef __cplusplus
extern "C"{
#endif

AutoClass
{
	float Freq;
	float Magnitude;
} EndClass(FreqPoint);

AutoClass
{
    ArrayType_Define(FreqPoint, ControlPointList);
    int UFreq;
} EndClass(CPF);

extern void CPF_AddControlPoint(CPF* Dest, FreqPoint* FPoint);
extern void CPF_AddControlFreq(CPF* Dest, float Freq, float Magnitude);

extern void CPF_Bake(float* Dest, CPF* Src, int DestLen);

extern void CPF_Print(CPF* Src);
extern void CPF_PrintBake(float* Dest, int Length);

#ifdef __cplusplus
}
#endif

#endif
