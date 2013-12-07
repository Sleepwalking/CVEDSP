/*
 * Calculation.h
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
#ifndef CALCULATION_H
#define CALCULATION_H

#include <math.h>

#ifdef __cplusplus
extern "C"{
#endif

#define TRIG_TABLE_SIZE 62831
#define TRIG_TABLE_MAGNITUDE 20000
#define LN_TABLE_SIZE 10000
#define LN_TABLE_MAXIMUM 1.0f
#define LN_TABLE_MINIMUM 0.00001

extern float* SinTable;
extern float* CosTable;
extern float* LnTable;

extern void Trig_Init() __attribute__ ((constructor));
extern void Trig_Exit() __attribute__ ((destructor));

extern void Ln_Init() __attribute__ ((constructor));
extern void Ln_Exit() __attribute__ ((destructor));

extern float Boost_Sin(float x);
extern float Boost_Cos(float x);
extern float Boost_Tan(float x);
extern float Boost_Ln(float x);

extern float FSin(float x);
extern float FCos(float x);

extern float Boost_Sqr(float x);
extern float Boost_RSqr(float x);
extern float Boost_Rcp(float x);
extern float Boost_Div(float x, float y);

#ifdef __cplusplus
}
#endif

#endif
