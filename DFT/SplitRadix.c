/*
 * SplitRadix.c
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
#include "SplitRadix.h"
#include "../IntrinUtil/Calculation.h"
#include "DFT.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xmmintrin.h>

#ifdef __AVX__
#include <immintrin.h>
#endif

void SRExpression_Ctor(SRExpression* Expression)
{
	Expression -> Number = 0;
	Expression -> Operator = malloc(4 * 50000);
	Expression -> Operand1 = malloc(4 * 50000);
	Expression -> Operand2 = malloc(4 * 50000);
}
void SRExpression_Dtor(SRExpression* Expression)
{
	free(Expression -> Operator);
	free(Expression -> Operand1);
	free(Expression -> Operand2);
}

void SRExpressionGenerator_L(SRExpression* Expression, int32_t Start, int32_t Amount)
{
	SRExpressionPush(Expression, OP_RADIXL, Start, Amount);
	if(Amount >= 32)
	{
		SRExpressionGenerator_L(Expression, Start, Amount / 2);
		SRExpressionGenerator_L(Expression, Start + Amount / 2, Amount / 4);
		SRExpressionGenerator_L(Expression, Start + Amount / 4 * 3, Amount / 4);
	}else if(Amount == 16)
	{
		SRExpressionGenerator_L(Expression, Start, 8);
		SRExpressionGenerator_Radix4(Expression, Start + 8);
		SRExpressionGenerator_Radix4(Expression, Start + 12);
	}else if(Amount == 8)
	{
		SRExpressionGenerator_Radix4(Expression, Start);
		SRExpressionGenerator_Radix2(Expression, Start + 4);
		SRExpressionGenerator_Radix2(Expression, Start + 6);
	}
}
void SRExpressionGenerator_Radix2(SRExpression* Expression, int32_t Start)
{
	SRExpressionPush(Expression, OP_RADIX2, Start, 2);
}
void SRExpressionGenerator_Radix4(SRExpression* Expression, int32_t Start)
{
	SRExpressionPush(Expression, OP_RADIX4, Start, 4);
}

void SplitRadix_Interpreter(SRExpression* Expression, float Real[], float Imag[])
{
	int32_t Amount = Expression -> Number;
	int32_t i;
	for(i = Amount - 1;i >= 0;i --)
	{
		switch(Expression -> Operator[i])
		{
			case OP_RADIXL:
				SplitRadix_RadixL(Real, Imag, Expression -> Operand1[i], Expression -> Operand2[i]);
				break;
			case OP_RADIX2:
				SplitRadix_Radix2(Real, Imag, Expression -> Operand1[i]);
				break;
			case OP_RADIX4:
				SplitRadix_Radix4(Real, Imag, Expression -> Operand1[i]);
				break;
		}
	}
}
#ifdef __AVX__
//RadixL for AVX
void SplitRadix_RadixL(float Real[], float Imag[], int32_t Start, int32_t Amount)
{
	int32_t i, half, quarter;
	int32_t this, another;
	float R;
	float I;
	float ReFactor_1, ImFactor_1, ReFactor_2, ImFactor_2;
	__m128 mReFactor_1, mImFactor_1, mReFactor_2, mImFactor_2;
	__m128 mR, mI;
	__m256 m256ReFactor_1, m256ImFactor_1, m256ReFactor_2, m256ImFactor_2;
	__m256 m256R, m256I;
	float TPOA;

	half = Amount / 2;
	quarter = Amount / 4;
	TPOA = 3.1415926535 * 2 / Amount;
	
	Start += half;// Start + half + i or Start + half + quarter + i

	for(i = 0;i < quarter;i ++)
	{
		this = Start + i;
		another = this + quarter;

		if(another % 8 == 0 && i < quarter - 7)
		{
			m256ReFactor_1 = _mm256_set_ps(Boost_Cos(TPOA * (i + 7)), Boost_Cos(TPOA * (i + 6)),
			                               Boost_Cos(TPOA * (i + 5)), Boost_Cos(TPOA * (i + 4)),
			                               Boost_Cos(TPOA * (i + 3)), Boost_Cos(TPOA * (i + 2)),
			                               Boost_Cos(TPOA * (i + 1)), Boost_Cos(TPOA * (i + 0)));
			m256ImFactor_1 = _mm256_set_ps(- Boost_Sin(TPOA * (i + 7)), - Boost_Sin(TPOA * (i + 6)),
			                               - Boost_Sin(TPOA * (i + 5)), - Boost_Sin(TPOA * (i + 4)),
			                               - Boost_Sin(TPOA * (i + 3)), - Boost_Sin(TPOA * (i + 2)),
			                               - Boost_Sin(TPOA * (i + 1)), - Boost_Sin(TPOA * (i + 0)));
			TPOA *= 3;
			m256ReFactor_2 = _mm256_set_ps(Boost_Cos(TPOA * (i + 7)), Boost_Cos(TPOA * (i + 6)),
			                               Boost_Cos(TPOA * (i + 5)), Boost_Cos(TPOA * (i + 4)),
			                               Boost_Cos(TPOA * (i + 3)), Boost_Cos(TPOA * (i + 2)),
			                               Boost_Cos(TPOA * (i + 1)), Boost_Cos(TPOA * (i + 0)));
			m256ImFactor_2 = _mm256_set_ps(- Boost_Sin(TPOA * (i + 7)), - Boost_Sin(TPOA * (i + 6)),
			                               - Boost_Sin(TPOA * (i + 5)), - Boost_Sin(TPOA * (i + 4)),
			                               - Boost_Sin(TPOA * (i + 3)), - Boost_Sin(TPOA * (i + 2)),
			                               - Boost_Sin(TPOA * (i + 1)), - Boost_Sin(TPOA * (i + 0)));
			TPOA /= 3;
			
			m256R = _mm256_load_ps(Real + this);
			m256I = _mm256_load_ps(Imag + this);
			_mm256_store_ps(Real + this, _mm256_sub_ps(_mm256_mul_ps(m256R, m256ReFactor_1), _mm256_mul_ps(m256I, m256ImFactor_1)));
			_mm256_store_ps(Imag + this, _mm256_add_ps(_mm256_mul_ps(m256R, m256ImFactor_1), _mm256_mul_ps(m256I, m256ReFactor_1)));

			m256R = _mm256_load_ps(Real + another);
			m256I = _mm256_load_ps(Imag + another);
			_mm256_store_ps(Real + another, _mm256_sub_ps(_mm256_mul_ps(m256R, m256ReFactor_2), _mm256_mul_ps(m256I, m256ImFactor_2)));
			_mm256_store_ps(Imag + another, _mm256_add_ps(_mm256_mul_ps(m256R, m256ImFactor_2), _mm256_mul_ps(m256I, m256ReFactor_2)));

			m256R = _mm256_load_ps(Real + another);
			m256I = _mm256_load_ps(Imag + another);
			_mm256_store_ps(Real + another, _mm256_sub_ps(_mm256_load_ps(Imag + this), m256I));
			_mm256_store_ps(Imag + another, _mm256_sub_ps(m256R, _mm256_load_ps(Real + this)));
			_mm256_store_ps(Real + this, _mm256_add_ps(_mm256_load_ps(Real + this), m256R));
			_mm256_store_ps(Imag + this, _mm256_add_ps(_mm256_load_ps(Imag + this), m256I));

			i += 7;
		}else if(another % 4 == 0 && i < quarter - 3)
		{
			mReFactor_1 = _mm_set_ps(Boost_Cos(TPOA * (i + 3)), Boost_Cos(TPOA * (i + 2)),
			                         Boost_Cos(TPOA * (i + 1)), Boost_Cos(TPOA * (i + 0)));
			mImFactor_1 = _mm_set_ps(- Boost_Sin(TPOA * (i + 3)), - Boost_Sin(TPOA * (i + 2)),
			                         - Boost_Sin(TPOA * (i + 1)), - Boost_Sin(TPOA * (i + 0)));
			TPOA *= 3;
			mReFactor_2 = _mm_set_ps(Boost_Cos(TPOA * (i + 3)), Boost_Cos(TPOA * (i + 2)),
			                         Boost_Cos(TPOA * (i + 1)), Boost_Cos(TPOA * (i + 0)));
			mImFactor_2 = _mm_set_ps(- Boost_Sin(TPOA * (i + 3)), - Boost_Sin(TPOA * (i + 2)),
			                         - Boost_Sin(TPOA * (i + 1)), - Boost_Sin(TPOA * (i + 0)));
			TPOA /= 3;
			
			mR = _mm_load_ps(Real + this);
			mI = _mm_load_ps(Imag + this);
			_mm_store_ps(Real + this, _mm_sub_ps(_mm_mul_ps(mR, mReFactor_1), _mm_mul_ps(mI, mImFactor_1)));
			_mm_store_ps(Imag + this, _mm_add_ps(_mm_mul_ps(mR, mImFactor_1), _mm_mul_ps(mI, mReFactor_1)));

			mR = _mm_load_ps(Real + another);
			mI = _mm_load_ps(Imag + another);
			_mm_store_ps(Real + another, _mm_sub_ps(_mm_mul_ps(mR, mReFactor_2), _mm_mul_ps(mI, mImFactor_2)));
			_mm_store_ps(Imag + another, _mm_add_ps(_mm_mul_ps(mR, mImFactor_2), _mm_mul_ps(mI, mReFactor_2)));

			mR = _mm_load_ps(Real + another);
			mI = _mm_load_ps(Imag + another);
			_mm_store_ps(Real + another, _mm_sub_ps(_mm_load_ps(Imag + this), mI));
			_mm_store_ps(Imag + another, _mm_sub_ps(mR, _mm_load_ps(Real + this)));
			_mm_store_ps(Real + this, _mm_add_ps(_mm_load_ps(Real + this), mR));
			_mm_store_ps(Imag + this, _mm_add_ps(_mm_load_ps(Imag + this), mI));

			i += 3;
		}else
		{
			ReFactor_1 = Boost_Cos(TPOA * i);
			ImFactor_1 = - Boost_Sin(TPOA * i);
			ReFactor_2 = Boost_Cos(TPOA * i * 3);
			ImFactor_2 = - Boost_Sin(TPOA * i * 3);

			R = Real[this];
			I = Imag[this];
			Real[this] = R * ReFactor_1 - I * ImFactor_1;
			Imag[this] = R * ImFactor_1 + I * ReFactor_1;

			R = Real[another];
			I = Imag[another];
			Real[another] = R * ReFactor_2 - I * ImFactor_2;
			Imag[another] = R * ImFactor_2 + I * ReFactor_2;

			R = Real[another];
			I = Imag[another];
			Real[another] = + Imag[this] - I;
			Imag[another] = - Real[this] + R;
			Real[this] += R;
			Imag[this] += I;
		}
	}
	Start -= half;
	for(i = 0;i < half;i += 4)
	{
		this = Start + i;
		another = this + half;
		if(another % 8 == 0 && i < half - 7)
		{
			m256R = _mm256_add_ps(_mm256_load_ps(Real + Start + i), _mm256_load_ps(Real + Start + half + i));
			m256I = _mm256_add_ps(_mm256_load_ps(Imag + Start + i), _mm256_load_ps(Imag + Start + half + i));
			_mm256_store_ps(Real + Start + half + i,
			             _mm256_sub_ps(_mm256_load_ps(Real + Start + i), _mm256_load_ps(Real + Start + half + i)));
			_mm256_store_ps(Imag + Start + half + i,
			             _mm256_sub_ps(_mm256_load_ps(Imag + Start + i), _mm256_load_ps(Imag + Start + half + i)));
			_mm256_store_ps(Real + Start + i, m256R);
			_mm256_store_ps(Imag + Start + i, m256I);
			
			i += 4;
		}else
		{
			mR = _mm_add_ps(_mm_load_ps(Real + Start + i), _mm_load_ps(Real + Start + half + i));
			mI = _mm_add_ps(_mm_load_ps(Imag + Start + i), _mm_load_ps(Imag + Start + half + i));
			_mm_store_ps(Real + Start + half + i,
			             _mm_sub_ps(_mm_load_ps(Real + Start + i), _mm_load_ps(Real + Start + half + i)));
			_mm_store_ps(Imag + Start + half + i,
			             _mm_sub_ps(_mm_load_ps(Imag + Start + i), _mm_load_ps(Imag + Start + half + i)));
			_mm_store_ps(Real + Start + i, mR);
			_mm_store_ps(Imag + Start + i, mI);
		}
	}
}
#else
#ifdef __SSE__
//RadixL for SSE
void SplitRadix_RadixL(float Real[], float Imag[], int32_t Start, int32_t Amount)
{
	int32_t i, half, quarter;
	int32_t this, another;
	float R;
	float I;
	float ReFactor_1, ImFactor_1, ReFactor_2, ImFactor_2;
	__m128 mReFactor_1, mImFactor_1, mReFactor_2, mImFactor_2;
	__m128 mR, mI;
	float TPOA;

	half = Amount / 2;
	quarter = Amount / 4;
	TPOA = 3.1415926535 * 2 / Amount;
	
	Start += half;// Start + half + i or Start + half + quarter + i

	for(i = 0;i < quarter;i ++)
	{
		this = Start + i;
		another = this + quarter;

		if(another % 4 == 0 && i < quarter - 3)
		{
			mReFactor_1 = _mm_set_ps(Boost_Cos(TPOA * (i + 3)), Boost_Cos(TPOA * (i + 2)),
			                         Boost_Cos(TPOA * (i + 1)), Boost_Cos(TPOA * (i + 0)));
			mImFactor_1 = _mm_set_ps(- Boost_Sin(TPOA * (i + 3)), - Boost_Sin(TPOA * (i + 2)),
			                         - Boost_Sin(TPOA * (i + 1)), - Boost_Sin(TPOA * (i + 0)));
			TPOA *= 3;
			mReFactor_2 = _mm_set_ps(Boost_Cos(TPOA * (i + 3)), Boost_Cos(TPOA * (i + 2)),
			                         Boost_Cos(TPOA * (i + 1)), Boost_Cos(TPOA * (i + 0)));
			mImFactor_2 = _mm_set_ps(- Boost_Sin(TPOA * (i + 3)), - Boost_Sin(TPOA * (i + 2)),
			                         - Boost_Sin(TPOA * (i + 1)), - Boost_Sin(TPOA * (i + 0)));
			TPOA /= 3;
			
			mR = _mm_load_ps(Real + this);
			mI = _mm_load_ps(Imag + this);
			_mm_store_ps(Real + this, _mm_sub_ps(_mm_mul_ps(mR, mReFactor_1), _mm_mul_ps(mI, mImFactor_1)));
			_mm_store_ps(Imag + this, _mm_add_ps(_mm_mul_ps(mR, mImFactor_1), _mm_mul_ps(mI, mReFactor_1)));

			mR = _mm_load_ps(Real + another);
			mI = _mm_load_ps(Imag + another);
			_mm_store_ps(Real + another, _mm_sub_ps(_mm_mul_ps(mR, mReFactor_2), _mm_mul_ps(mI, mImFactor_2)));
			_mm_store_ps(Imag + another, _mm_add_ps(_mm_mul_ps(mR, mImFactor_2), _mm_mul_ps(mI, mReFactor_2)));

			mR = _mm_load_ps(Real + another);
			mI = _mm_load_ps(Imag + another);
			_mm_store_ps(Real + another, _mm_sub_ps(_mm_load_ps(Imag + this), mI));
			_mm_store_ps(Imag + another, _mm_sub_ps(mR, _mm_load_ps(Real + this)));
			_mm_store_ps(Real + this, _mm_add_ps(_mm_load_ps(Real + this), mR));
			_mm_store_ps(Imag + this, _mm_add_ps(_mm_load_ps(Imag + this), mI));

			i += 3;
		}else
		{
			ReFactor_1 = Boost_Cos(TPOA * i);
			ImFactor_1 = - Boost_Sin(TPOA * i);
			ReFactor_2 = Boost_Cos(TPOA * i * 3);
			ImFactor_2 = - Boost_Sin(TPOA * i * 3);

			R = Real[this];
			I = Imag[this];
			Real[this] = R * ReFactor_1 - I * ImFactor_1;
			Imag[this] = R * ImFactor_1 + I * ReFactor_1;

			R = Real[another];
			I = Imag[another];
			Real[another] = R * ReFactor_2 - I * ImFactor_2;
			Imag[another] = R * ImFactor_2 + I * ReFactor_2;

			R = Real[another];
			I = Imag[another];
			Real[another] = + Imag[this] - I;
			Imag[another] = - Real[this] + R;
			Real[this] += R;
			Imag[this] += I;
		}
	}
	Start -= half;
	for(i = 0;i < half;i += 4)
	{
		mR = _mm_add_ps(_mm_load_ps(Real + Start + i), _mm_load_ps(Real + Start + half + i));
		mI = _mm_add_ps(_mm_load_ps(Imag + Start + i), _mm_load_ps(Imag + Start + half + i));
		_mm_store_ps(Real + Start + half + i,
		             _mm_sub_ps(_mm_load_ps(Real + Start + i), _mm_load_ps(Real + Start + half + i)));
		_mm_store_ps(Imag + Start + half + i,
		             _mm_sub_ps(_mm_load_ps(Imag + Start + i), _mm_load_ps(Imag + Start + half + i)));
		_mm_store_ps(Real + Start + i, mR);
		_mm_store_ps(Imag + Start + i, mI);
	}
}
#else
//RadixL for X87
void SplitRadix_RadixL(float Real[], float Imag[], int32_t Start, int32_t Amount)
{
	int32_t i, half, quarter;
	float R;
	float I;
	float ReFactor_1, ImFactor_1, ReFactor_2, ImFactor_2;
	float TPOA;

	half = Amount / 2;
	quarter = Amount / 4;
	TPOA = 3.1415926535 * 2 / Amount;
	
	Start += half;// Start + half + i or Start + half + quarter + i
	for(i = 0;i < quarter;i ++)
	{
		ReFactor_1 = Boost_Cos(TPOA * i);
		ImFactor_1 = - Boost_Sin(TPOA * i);
		ReFactor_2 = Boost_Cos(TPOA * i * 3);
		ImFactor_2 = - Boost_Sin(TPOA * i * 3);

		R = Real[Start + i];
		I = Imag[Start + i];
		Real[Start + i] = R * ReFactor_1 - I * ImFactor_1;
		Imag[Start + i] = R * ImFactor_1 + I * ReFactor_1;
		
		R = Real[Start + quarter + i];
		I = Imag[Start + quarter + i];
		Real[Start + quarter + i] = R * ReFactor_2 - I * ImFactor_2;
		Imag[Start + quarter + i] = R * ImFactor_2 + I * ReFactor_2;

		R = Real[Start + quarter + i];
		I = Imag[Start + quarter + i];
		Real[Start + quarter + i] = + Imag[Start + i] - I;
		Imag[Start + quarter + i] = - Real[Start + i] + R;
		Real[Start + i] += R;
		Imag[Start + i] += I;
	}
	Start -= half;
	for(i = 0;i < half;i ++)
	{
		R = Real[Start + i] + Real[Start + half + i];
		I = Imag[Start + i] + Imag[Start + half + i];
		Real[Start + half + i] = Real[Start + i] - Real[Start + half + i];
		Imag[Start + half + i] = Imag[Start + i] - Imag[Start + half + i];
		Real[Start + i] = R;
		Imag[Start + i] = I;
	}
}
#endif
#endif
void SplitRadix_Radix2(float Real[], float Imag[], int32_t Start)
{
	float ReNew, ImNew;
	ReNew = Real[Start] + Real[Start + 1];
	ImNew = Imag[Start] + Imag[Start + 1];
	Real[Start + 1] = Real[Start] - Real[Start + 1];
	Imag[Start + 1] = Imag[Start] - Imag[Start + 1];
	Real[Start] = ReNew;
	Imag[Start] = ImNew;
}
void SplitRadix_Radix4(float Real[], float Imag[], int32_t Start)
{
	float Re_0, Re_1, Re_2, Re_3;
	float Im_0, Im_1, Im_2, Im_3;
	float ReAdd, ImAdd;

	Re_0 = Real[Start];
	Im_0 = Imag[Start];
	Re_1 = Real[Start + 1];
	Im_1 = Imag[Start + 1];
	Re_2 = Real[Start + 2];
	Im_2 = Imag[Start + 2];
	Re_3 = Real[Start + 3];
	Im_3 = Imag[Start + 3];

	Real[Start + 0] = Re_0 + Re_1;
	Imag[Start + 0] = Im_0 + Im_1;
	Real[Start + 1] = Re_0 - Re_1;
	Imag[Start + 1] = Im_0 - Im_1;
	Real[Start + 2] = Re_2 + Re_3;
	Imag[Start + 2] = Im_2 + Im_3;
	Real[Start + 3] = Im_2 - Im_3;//Complex3 *= -i
	Imag[Start + 3] = Re_3 - Re_2;

	ReAdd = Real[Start + 0] + Real[Start + 2];
	ImAdd = Imag[Start + 0] + Imag[Start + 2];
	Real[Start + 2] = Real[Start + 0] - Real[Start + 2];
	Imag[Start + 2] = Imag[Start + 0] - Imag[Start + 2];
	Real[Start + 0] = ReAdd;
	Imag[Start + 0] = ImAdd;
	
	ReAdd = Real[Start + 1] + Real[Start + 3];
	ImAdd = Imag[Start + 1] + Imag[Start + 3];
	Real[Start + 3] = Real[Start + 1] - Real[Start + 3];
	Imag[Start + 3] = Imag[Start + 1] - Imag[Start + 3];
	Real[Start + 1] = ReAdd;
	Imag[Start + 1] = ImAdd;
}
