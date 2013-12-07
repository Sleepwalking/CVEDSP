/*
 * TDAnalysis.h
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
#ifndef TDANALYSIS_H
#define TDANALYSIS_H

#ifdef __cplusplus
extern "C"{
#endif

extern float Energy(float* Wave, int Length);
extern float AverageMagnitude(float* Wave, int Length);
extern float ZeroCrossingRate(float* Wave, int Length);
extern float ThresholdCrossingRate(float* Wave, float Threshold, int Length);
extern float CorrelationAt(float* Wave1, float* Wave2, int Length);
extern float AMDFAt(float* Wave1, float* Wave2, int Length);

extern void EnergyCurve(float* Dest, float* Wave, int Resolution, int DestLength);
extern void AmplitudeCurve(float* Dest, float* Wave, int Resolution, int DestLength);
extern void Correlation(float* Dest, float* Wave1, float* Wave2, int CompareLength, int DestLength);
extern void FastCorrelation(float* Dest, float* Wave1, float* Wave2, int Power);
extern void AMDF(float* Dest, float* Wave1, float* Wave2, int CompareLength, int DestLength);

extern void Threshold(float* Dest, float* Wave, float Threshold, int Length);
extern void Limit(float* Dest, float* Wave, float Limit, int Length);

#ifdef __cplusplus
}
#endif

#endif
