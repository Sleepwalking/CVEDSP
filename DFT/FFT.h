/*
 * FFT.h
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
#ifndef FFT_H
#define FFT_H

#include "StaticRev.h"
#include "SplitRadix.h"

#ifdef __cplusplus
extern "C"{
#endif

extern int ReverseArrange_Complex(float NewReal[], float NewImaginary[], float Real[], float Imaginary[], int Power);
extern int ReverseArrange_Static(float NewArray[], float Array[], int Power);
extern int ReverseArrange_Auto(float NewArray[], float Array[], int Power);
extern int ReverseArrange_AutoComplex(float NewReal[], float NewImaginary[], float Real[], float Imaginary[], int Power);
extern int ReverseArrange_Generator(int Power);

extern void FFT_Init() __attribute__ ((constructor));
extern void FFT_Exit() __attribute__ ((destructor));
extern int FFT_Use(int Power);

extern int FFT(float FReal[], float FImaginary[], float TReal[], float TImaginary[], int Power);
extern int RFFT(float FReal[], float FImaginary[], float TReal[], int Power);
extern int IFFT(float TReal[], float TImaginary[], float FReal[], float FImaginary[],int Power);
extern int RIFFT(float TReal[], float FReal[], float FImaginary[], int Power);
extern void Reflect(float DestReal[], float DestImag[], float Real[], float Imag[], int Power);
extern void ComplexCopy(float DestReal[], float DestImag[], float Real[], float Imag[], int Amount);

#ifdef __cplusplus
}
#endif

#endif
