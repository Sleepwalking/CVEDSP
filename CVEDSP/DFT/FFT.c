/*
 * FFT.c
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
#include "FFT.h"
#include "DFT.h"
#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>

#include "StaticFFT_9.h"
#include "StaticFFT_8.h"
#include "StaticFFT_7.h"
#include "StaticFFT_6.h"
#include "StaticFFT_5.h"
#include "StaticFFT_Small.h"

#include "StaticRadix.h"


#include "StaticRev.h"
#include "SplitRadix.h"
#include "FFTBlock.h"
#include "../IntrinUtil/Memory.h"
#include "../IntrinUtil/FloatArray.h"
#include "../IntrinUtil/Calculation.h"

#include "FFTCommon.h"

typedef void (*rFunc)(float*, float*);
typedef void (*vFunc)();

SRExpression _SRExpFFT[32];
int _SRExpState[32];

float* _FRe;
float* _FIm;
float* _Empty;

rFunc StaticReverseArrangeTable[] =
{
    StaticReverseArrange_3,
    StaticReverseArrange_4,
    StaticReverseArrange_5,
    StaticReverseArrange_6,
    StaticReverseArrange_7,
    StaticReverseArrange_8,
    StaticReverseArrange_9,
    StaticReverseArrange_10,
    StaticReverseArrange_11
};
vFunc StaticFFTExecTable[] =
{
    StaticFFT_3,
    StaticFFT_4,
    StaticFFT_5,
    StaticFFT_6,
    StaticFFT_7,
    StaticFFT_8,
    StaticFFT_9
};

int ReverseArrange_Complex(float NewReal[], float NewImaginary[], float Real[], float Imaginary[], int Power)
{
    if(! ReverseArrange_Static(NewReal, Real, Power))
        return 0;
    if(! ReverseArrange_Static(NewImaginary, Imaginary, Power))
        return 0;
    return 1;
}
int ReverseArrange_Static(float NewArray[], float Array[], int Power)
{
    if(Power > 2 && Power < 12)
    {
        StaticReverseArrangeTable[Power - 3](Array, NewArray);
        return 1;
    }
    return 0;
}
int ReverseArrange_AutoComplex(float NewReal[], float NewImaginary[], float Real[], float Imaginary[], int Power)
{
    int Amount, i, j;
    int tmp = 0x00000000;
    if(Power > 31 || Power < 1)
        return 0;
    if(Power < 12)
        return ReverseArrange_Complex(NewReal, NewImaginary, Real, Imaginary, Power);

    Amount = pow(2, Power);
    NewReal[0] = Real[0];
    NewImaginary[0] = Imaginary[0];
    for(i = 0; i < Amount - 1; i ++)
    {
        j = Power - 1;
        while((tmp & (1 << j)) != 0)
        {
            tmp &= ~ (1 << j);
            j --;
        }
        tmp |= (1 << j);
        //printf("%d <- %d\n", i + 1, tmp);
        NewReal[i + 1] = Real[tmp];
        NewImaginary[i + 1] = Imaginary[tmp];
    }
    return 1;
}
int ReverseArrange_Auto(float NewArray[], float Array[], int Power)
{
    int Amount, i, j;
    int tmp = 0x00000000;
    if(Power > 31 || Power < 1)
        return 0;
    
    Amount = pow(2, Power);
    NewArray[0] = Array[0];
    for(i = 0; i < Amount - 1; i ++)
    {
        j = Power - 1;
        while((tmp & (1 << j)) != 0)
        {
            tmp &= ~ (1 << j);
            j --;
        }
        tmp |= (1 << j);
        NewArray[i + 1] = Array[tmp];
    }
    return 1;
}
int ReverseArrange_Generator(int Power)
{
    int Amount, i, j;
    int tmp = 0x00000000;
    if(Power > 31 || Power < 1)
        return 0;
    Amount = pow(2, Power);

    printf("\tO[%d] = N[%d];\n", 0, tmp);
    for(i = 0; i < Amount - 1; i ++)
    {
        j = Power - 1;
        while((tmp & (1 << j)) != 0)
        {
            tmp &= ~ (1 << j);
            j --;
        }
        tmp |= (1 << j);
        printf("\tO[%d] = N[%d];\n", i + 1, tmp);
    }
    return 1;
}

void FFT_Init()
{
    int i;
    for(i = 0; i < 32; i ++)
        _SRExpState[i] = 0;

    _FRe = memalign(32, 2048 * 40);
    _FIm = memalign(32, 2048 * 40);
    _Empty = memalign(32, 2048 * 40);

    SFRe = _FRe;
    SFIm = _FIm;
}
void FFT_Exit()
{
    int i;
    for(i = 0; i < 32; i ++)
        if(_SRExpState[i])
            SRExpression_Dtor(_SRExpFFT +i);

    free(_FRe);
    free(_FIm);
    free(_Empty);
}
int FFT_Use(int Power)
{
    if(Power < 2 || Power > 31)
        return 0;
    if(!_SRExpState[Power])
    {
        _SRExpState[Power] = 1;
        SRExpression_Ctor(_SRExpFFT + Power);
        SRExpressionGenerator_L(_SRExpFFT + Power, 0, pow(2, Power));        
    }
    return 1;
}

int FFTBase(float FReal[], float FImaginary[], float TReal[], float TImaginary[], int Power, int RealType)
{
    int Amount;
    if(Power < 2)
        return 0;
    Amount = pow(2, Power);
    if(RealType)
        TImaginary = _Empty;
    if(Power < 10 && Power > 2)
    {
        ReverseArrange_Complex(_FRe, _FIm, TReal, TImaginary, Power);
        StaticFFTExecTable[Power - 3]();
        Boost_FloatCopy(FReal, _FRe, Amount);
        Boost_FloatCopy(FImaginary, _FIm, Amount);
    }else
    {
        if(Power < 12)
            ReverseArrange_Complex(_FRe, _FIm, TReal, TImaginary, Power);
        else
            ReverseArrange_AutoComplex(_FRe, _FIm, TReal, TImaginary, Power);
        switch(Power)
        {
            case 10:
                FFTBlock10();
                break;
            case 11:
                FFTBlock11();
                break;
            case 12:
                FFTBlock12();
                break;
            case 13:
                FFTBlock13();
                break;
            case 14:
                FFTBlock14();
                break;
            default:
                FFT_Use(Power);
                SplitRadix_Interpreter(_SRExpFFT + Power, _FRe, _FIm);
        }
        Boost_FloatCopy(FReal, _FRe, Amount);
        Boost_FloatCopy(FImaginary, _FIm, Amount);
    }
    return 1;
}

int FFT(float FReal[], float FImaginary[], float TReal[], float TImaginary[], int Power)
{
    return FFTBase(FReal, FImaginary, TReal, TImaginary, Power, 0);
}
int RFFT(float FReal[], float FImaginary[], float TReal[], int Power)
{
    return FFTBase(FReal, FImaginary, TReal, NULL, Power, 1);
}
int IFFTBase(float TReal[], float TImaginary[], float FReal[], float FImaginary[], int Power, int RealType)
{
    int Amount;
    if(Power < 2)
        return 0;
    Amount = pow(2, Power);
    
    if(Power < 10 && Power > 2)
    {
        FFT_Use(Power);
        ReverseArrange_Complex(_FRe, _FIm, FReal, FImaginary, Power);
        Boost_FloatInv(_FIm, _FIm, Amount);
        StaticFFTExecTable[Power - 3]();
        Boost_FloatDiv(_FRe, _FRe, Amount, Amount);
        if(!RealType)
            Boost_FloatDiv(_FIm, _FIm, - Amount, Amount);
        Boost_FloatCopy(TReal, _FRe, Amount);
        if(!RealType)
            Boost_FloatCopy(TImaginary, _FIm, Amount);
    }else
    {
        ReverseArrange_AutoComplex(_FRe, _FIm, FReal, FImaginary, Power);
        Boost_FloatInv(_FIm, _FIm, Amount);
        switch(Power)
        {
            case 10:
                FFTBlock10();
                break;
            case 11:
                FFTBlock11();
                break;
            case 12:
                FFTBlock12();
                break;
            case 13:
                FFTBlock13();
                break;
            case 14:
                FFTBlock14();
                break;
            default:
                FFT_Use(Power);
                SplitRadix_Interpreter(_SRExpFFT + Power, _FRe, _FIm);
        }
        Boost_FloatDiv(_FRe, _FRe, Amount, Amount);
        if(!RealType)
            Boost_FloatDiv(_FIm, _FIm, - Amount, Amount);
        Boost_FloatCopy(TReal, _FRe, Amount);
        if(!RealType)
            Boost_FloatCopy(TImaginary, _FIm, Amount);
    }
    return 1;
}
int IFFT(float TReal[], float TImaginary[], float FReal[], float FImaginary[], int Power)
{
    return IFFTBase(TReal, TImaginary, FReal, FImaginary, Power, 0);
}
int RIFFT(float TReal[], float FReal[], float FImaginary[], int Power)
{
    return IFFTBase(TReal, NULL, FReal, FImaginary, Power, 1);
}

void Reflect(float DestReal[], float DestImag[], float Real[], float Imag[], int Power)
{
    int k;
    int N = pow(2, Power);
    int j = N / 2;
    DestReal[0] = Real[0];
    DestImag[0] = Imag[0];
    DestReal[j] = 0;
    DestImag[j] = 0;
    if(DestReal != Real)
        Boost_FloatCopy(DestReal, Real, j);
    if(DestImag != Imag)
        Boost_FloatCopy(DestImag, Imag, j);
    for(k = 1; k < j; k ++)
    {
        DestReal[N - k] = Real[k];
        DestImag[N - k] = Imag[k];
    }
    Boost_FloatInv(DestImag + N - j + 1, DestImag + N - j + 1, j - 1);
}
void ComplexCopy(float DestReal[], float DestImag[], float Real[], float Imag[], int Amount)
{
    Boost_FloatCopy(DestReal, Real, Amount);
    Boost_FloatCopy(DestImag, Imag, Amount);
}
