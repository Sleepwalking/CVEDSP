/*
 * Radix2.c
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
#include "Radix2.h"
#include "../IntrinUtil/Calculation.h"

#include <stdio.h>
#include <math.h>
#include <xmmintrin.h>

#ifdef __AVX__
#include <immintrin.h>
#endif

#ifndef __AVX__
//Radix2-FFT for CPUs that do not support AVX.
int Radix2_Dynamic(float Real[], float Imag[], int Power)
{
	int Layer;
	int i, j, k, pmul, current, another, currentBase;
	int Amount;
	float ReFactor, ImFactor;
	float Re, Im;
	float TPOA, TPOATP; //TPOA = TwoPiOverAmount, TPOATP = TwoPiOverAmountTimesPmul
	if(Power > 31 || Power < 1)
		return 0;

	__m128 mReFactor, mImFactor, mReal, mImag, m0, m1, m2, m3, mRealC, mImagC;
	
	Amount = pow(2, Power);
	TPOA = 2 * 3.1415926535 / Amount;

	//For each layer in fft.
    for(Layer = 1; Layer <= Power; Layer ++)
	{
		j = pow(2, Layer - 1);
		pmul = pow(2, Power - Layer); //W_Amount ^ (0 to p interval j)
		
        for(i = 0; i < pmul; i ++)
		{
			currentBase = i * 2 * j;
			TPOATP = TPOA * pmul;
			{
				//k = 0
				current = currentBase;
				another = current + j;
				
				Re = Real[current] + Real[another];
				Im = Imag[current] + Imag[another];
				Real[another] = Real[current] - Real[another];
				Imag[another] = Imag[current] - Imag[another];
				
				Real[current] = Re;
				Imag[current] = Im;
			}
			{
                for(k = 1; k < j; k ++)
				{
					current = currentBase + k;
					another = current + j;

					if(current % 4 == 0 && k < j - 3)
					{
						//Aligned

						mReFactor = _mm_set_ps(Boost_Cos(TPOATP * (k + 3)), Boost_Cos(TPOATP * (k + 2)), Boost_Cos(TPOATP * (k + 1)), Boost_Cos(TPOATP * (k + 0)));
						mReal = _mm_load_ps(Real + another);
						mImFactor = _mm_set_ps( - Boost_Sin(TPOATP * (k + 3)), - Boost_Sin(TPOATP * (k + 2)), - Boost_Sin(TPOATP * (k + 1)), - Boost_Sin(TPOATP * (k + 0)));
						mImag = _mm_load_ps(Imag + another);

						m0 = _mm_mul_ps(mReal, mReFactor);
						m1 = _mm_mul_ps(mImag, mImFactor);
						m2 = _mm_mul_ps(mImag, mReFactor);
						m3 = _mm_mul_ps(mReal, mImFactor);

						mReal = _mm_sub_ps(m0, m1);
						mImag = _mm_add_ps(m2, m3);
						
						mRealC = _mm_load_ps(Real + current);
						mImagC = _mm_load_ps(Imag + current);
						
						m0 = mRealC;
						m1 = mImagC;
						
						mRealC = _mm_add_ps(mRealC, mReal);
						mImagC = _mm_add_ps(mImagC, mImag);
						_mm_store_ps(Real + current, mRealC);
						_mm_store_ps(Imag + current, mImagC);

						mReal = _mm_sub_ps(m0, mReal);
						mImag = _mm_sub_ps(m1, mImag);

						_mm_store_ps(Real + another, mReal);
						_mm_store_ps(Imag + another, mImag);
						
						k += 3;
					}else
					{
						ReFactor = Boost_Cos(TPOATP * k);
						ImFactor = - Boost_Sin(TPOATP * k);

						Re = Real[another];
						Real[another] = Real[another] * ReFactor - Imag[another] * ImFactor;
						Imag[another] = Imag[another] * ReFactor + Re * ImFactor;

						Re = Real[current];
						Im = Imag[current];
						Real[current] += Real[another];
						Imag[current] += Imag[another];

						Real[another] = Re - Real[another];
						Imag[another] = Im - Imag[another];
					}
				}
			}
		}
	}
	return 1;
}
#else
//Radix2-FFT for CPUs that support AVX.
int Radix2_Dynamic(float Real[], float Imag[], int Power)
{
	int Layer;
	int i, j, k, pmul, current, another, currentBase;
	int Amount;
	float ReFactor, ImFactor;
	float Re, Im;
	float TPOA, TPOATP; //TPOA = TwoPiOverAmount, TPOATP = TwoPiOverAmountTimesPmul
	if(Power > 31 || Power < 1)
		return 0;

	__m128 mReFactor, mImFactor, mReal, mImag, m0, m1, m2, m3, mRealC, mImagC;
	__m256 mReFactor256, mImFactor256, mReal256, mImag256, m4, m5, m6, m7, mRealC256, mImagC256;
	
	Amount = pow(2, Power);
	TPOA = 2 * 3.1415926535 / Amount;

	//For each layer in fft.
    for(Layer = 1; Layer <= Power; Layer ++)
	{
		j = pow(2, Layer - 1);
		pmul = pow(2, Power - Layer); //W_Amount ^ (0 to p interval j)
		
        for(i = 0; i < pmul; i ++)
		{
			currentBase = i * 2 * j;
			TPOATP = TPOA * pmul;
			{
				//k = 0
				current = currentBase;
				another = current + j;
				
				Re = Real[current] + Real[another];
				Im = Imag[current] + Imag[another];
				Real[another] = Real[current] - Real[another];
				Imag[another] = Imag[current] - Imag[another];
				
				Real[current] = Re;
				Imag[current] = Im;
			}
			{
				for(k = 1;k < j;k ++)
				{
					current = currentBase + k;
					another = current + j;

					if(current % 8 == 0 && k < j - 7)
					{
						//Aligned

						mReFactor256 = _mm256_set_ps(
						                             Boost_Cos(TPOATP * (k + 7)), Boost_Cos(TPOATP * (k + 6)),
						                             Boost_Cos(TPOATP * (k + 5)), Boost_Cos(TPOATP * (k + 4)),
						                             Boost_Cos(TPOATP * (k + 3)), Boost_Cos(TPOATP * (k + 2)),
						                             Boost_Cos(TPOATP * (k + 1)), Boost_Cos(TPOATP * (k + 0)));
						mReal256 = _mm256_load_ps(Real + another);
						mImFactor256 = _mm256_set_ps(
						                             - Boost_Sin(TPOATP * (k + 7)), - Boost_Sin(TPOATP * (k + 6)),
						                             - Boost_Sin(TPOATP * (k + 5)), - Boost_Sin(TPOATP * (k + 4)),
						                             - Boost_Sin(TPOATP * (k + 3)), - Boost_Sin(TPOATP * (k + 2)),
						                             - Boost_Sin(TPOATP * (k + 1)), - Boost_Sin(TPOATP * (k + 0)));
						mImag256 = _mm256_load_ps(Imag + another);

						m4 = _mm256_mul_ps(mReal256, mReFactor256);
						m5 = _mm256_mul_ps(mImag256, mImFactor256);
						m6 = _mm256_mul_ps(mImag256, mReFactor256);
						m7 = _mm256_mul_ps(mReal256, mImFactor256);

						mReal256 = _mm256_sub_ps(m4, m5);
						mImag256 = _mm256_add_ps(m6, m7);
						
						mRealC256 = _mm256_load_ps(Real + current);
						mImagC256 = _mm256_load_ps(Imag + current);
						
						m4 = mRealC256;
						m5 = mImagC256;
						
						mRealC256 = _mm256_add_ps(mRealC256, mReal256);
						mImagC256 = _mm256_add_ps(mImagC256, mImag256);
						_mm256_store_ps(Real + current, mRealC256);
						_mm256_store_ps(Imag + current, mImagC256);

						mReal256 = _mm256_sub_ps(m4, mReal256);
						mImag256 = _mm256_sub_ps(m5, mImag256);

						_mm256_store_ps(Real + another, mReal256);
						_mm256_store_ps(Imag + another, mImag256);
						
						k += 7;
					}else if(current % 4 == 0 && k < j - 3)
					{
						//Aligned

						mReFactor = _mm_set_ps(Boost_Cos(TPOATP * (k + 3)), Boost_Cos(TPOATP * (k + 2)),
						                       Boost_Cos(TPOATP * (k + 1)), Boost_Cos(TPOATP * (k + 0)));
						mReal = _mm_load_ps(Real + another);
						mImFactor = _mm_set_ps( - Boost_Sin(TPOATP * (k + 3)), - Boost_Sin(TPOATP * (k + 2)),
						                        - Boost_Sin(TPOATP * (k + 1)), - Boost_Sin(TPOATP * (k + 0)));
						mImag = _mm_load_ps(Imag + another);

						m0 = _mm_mul_ps(mReal, mReFactor);
						m1 = _mm_mul_ps(mImag, mImFactor);
						m2 = _mm_mul_ps(mImag, mReFactor);
						m3 = _mm_mul_ps(mReal, mImFactor);

						mReal = _mm_sub_ps(m0, m1);
						mImag = _mm_add_ps(m2, m3);
						
						mRealC = _mm_load_ps(Real + current);
						mImagC = _mm_load_ps(Imag + current);
						
						m0 = mRealC;
						m1 = mImagC;
						
						mRealC = _mm_add_ps(mRealC, mReal);
						mImagC = _mm_add_ps(mImagC, mImag);
						_mm_store_ps(Real + current, mRealC);
						_mm_store_ps(Imag + current, mImagC);

						mReal = _mm_sub_ps(m0, mReal);
						mImag = _mm_sub_ps(m1, mImag);

						_mm_store_ps(Real + another, mReal);
						_mm_store_ps(Imag + another, mImag);
						
						k += 3;
					}else
					{
						ReFactor = Boost_Cos(TPOATP * k);
						ImFactor = - Boost_Sin(TPOATP * k);

						Re = Real[another];
						Real[another] = Real[another] * ReFactor - Imag[another] * ImFactor;
						Imag[another] = Imag[another] * ReFactor + Re * ImFactor;

						Re = Real[current];
						Im = Imag[current];
						Real[current] += Real[another];
						Imag[current] += Imag[another];

						Real[another] = Re - Real[another];
						Imag[another] = Im - Imag[another];
					}
				}
			}
		}
	}
	return 1;
}
#endif
