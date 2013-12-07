/*
 * SplitRadixGen.c
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
#include "SplitRadixGen.h"
#include "../IntrinUtil/Calculation.h"
#include "DFT.h"
#include "FFTCommon.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xmmintrin.h>

#ifdef __AVX__
#include <immintrin.h>
#endif

void SplitRadix_Generator(SRExpression* Expression, int AVXEnabled)
{
	int32_t Amount = Expression -> Number;
	int32_t i;
	for(i = Amount - 1;i >= 0;i --)
	{
		switch(Expression -> Operator[i])
		{
			case OP_RADIXL:
				if(!AVXEnabled)
					SplitRadix_RadixL_Generator_SSE(Expression -> Operand1[i], Expression -> Operand2[i]);
				else
					SplitRadix_RadixL_Generator_AVX(Expression -> Operand1[i], Expression -> Operand2[i]);					
				break;
			case OP_RADIX2:
				SplitRadix_Radix2_Generator(Expression -> Operand1[i]);
				break;
			case OP_RADIX4:
				SplitRadix_Radix4_Generator(Expression -> Operand1[i]);
				break;
		}
	}
}

void SplitRadix_RadixL_Generator_AVX(int32_t Start, int32_t Amount)
{
        int32_t i, half, quarter;
        int32_t this, another;
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
                        printf("\tm256RF1 = _mm256_set_ps(%f, %f, %f, %f, %f, %f, %f, %f);\n",
                               Boost_Cos(TPOA * (i + 7)), Boost_Cos(TPOA * (i + 6)),
                               Boost_Cos(TPOA * (i + 5)), Boost_Cos(TPOA * (i + 4)),
                               Boost_Cos(TPOA * (i + 3)), Boost_Cos(TPOA * (i + 2)),
                               Boost_Cos(TPOA * (i + 1)), Boost_Cos(TPOA * (i + 0)));
                        printf("\tm256IF1 = _mm256_set_ps(%f, %f, %f, %f, %f, %f, %f, %f);\n",
                               - Boost_Sin(TPOA * (i + 7)), - Boost_Sin(TPOA * (i + 6)),
                               - Boost_Sin(TPOA * (i + 5)), - Boost_Sin(TPOA * (i + 4)),
                               - Boost_Sin(TPOA * (i + 3)), - Boost_Sin(TPOA * (i + 2)),
                               - Boost_Sin(TPOA * (i + 1)), - Boost_Sin(TPOA * (i + 0)));
                        TPOA *= 3;
                        printf("\tm256RF2 = _mm256_set_ps(%f, %f, %f, %f, %f, %f, %f, %f);\n",
                               Boost_Cos(TPOA * (i + 7)), Boost_Cos(TPOA * (i + 6)),
                               Boost_Cos(TPOA * (i + 5)), Boost_Cos(TPOA * (i + 4)),
                               Boost_Cos(TPOA * (i + 3)), Boost_Cos(TPOA * (i + 2)),
                               Boost_Cos(TPOA * (i + 1)), Boost_Cos(TPOA * (i + 0)));
                        printf("\tm256IF2 = _mm256_set_ps(%f, %f, %f, %f, %f, %f, %f, %f);\n",
                               - Boost_Sin(TPOA * (i + 7)), - Boost_Sin(TPOA * (i + 6)),
                               - Boost_Sin(TPOA * (i + 5)), - Boost_Sin(TPOA * (i + 4)),
                               - Boost_Sin(TPOA * (i + 3)), - Boost_Sin(TPOA * (i + 2)),
                               - Boost_Sin(TPOA * (i + 1)), - Boost_Sin(TPOA * (i + 0)));
                        TPOA /= 3;

                        printf("\tm256R = _mm256_load_ps(SFRe + %d);\n", this);
                        printf("\tm256I = _mm256_load_ps(SFIm + %d);\n", this);
                        printf("\t_mm256_store_ps(SFRe + %d, _mm256_sub_ps(_mm256_mul_ps(m256R, m256RF1), _mm256_mul_ps(m256I, m256IF1)));\n", this);
                        printf("\t_mm256_store_ps(SFIm + %d, _mm256_add_ps(_mm256_mul_ps(m256R, m256IF1), _mm256_mul_ps(m256I, m256RF1)));\n", this);

                        printf("\tm256R = _mm256_load_ps(SFRe + %d);\n", another);
                        printf("\tm256I = _mm256_load_ps(SFIm + %d);\n", another);
                        printf("\t_mm256_store_ps(SFRe + %d, _mm256_sub_ps(_mm256_mul_ps(m256R, m256RF2), _mm256_mul_ps(m256I, m256IF2)));\n", another);
                        printf("\t_mm256_store_ps(SFIm + %d, _mm256_add_ps(_mm256_mul_ps(m256R, m256IF2), _mm256_mul_ps(m256I, m256RF2)));\n", another);

                        printf("\tm256R = _mm256_load_ps(SFRe + %d);\n", another);
                        printf("\tm256I = _mm256_load_ps(SFIm + %d);\n", another);

                        printf("\t_mm256_store_ps(SFRe + %d, _mm256_sub_ps(_mm256_load_ps(SFIm + %d), m256I));\n", another, this);
                        printf("\t_mm256_store_ps(SFIm + %d, _mm256_sub_ps(m256R, _mm256_load_ps(SFRe + %d)));\n", another, this);
                        printf("\t_mm256_store_ps(SFRe + %d, _mm256_add_ps(_mm256_load_ps(SFRe + %d), m256R));\n", this, this);
                        printf("\t_mm256_store_ps(SFIm + %d, _mm256_add_ps(_mm256_load_ps(SFIm + %d), m256I));\n", this, this);


                        i += 7;
                }else if(another % 4 == 0 && i < quarter - 3)
                {
                        printf("\tmRF1 = _mm_set_ps(%f, %f, %f, %f);\n",
                               Boost_Cos(TPOA * (i + 3)), Boost_Cos(TPOA * (i + 2)),
                               Boost_Cos(TPOA * (i + 1)), Boost_Cos(TPOA * (i + 0)));
                        printf("\tmIF1 = _mm_set_ps(%f, %f, %f, %f);\n",
                               - Boost_Sin(TPOA * (i + 3)), - Boost_Sin(TPOA * (i + 2)),
                               - Boost_Sin(TPOA * (i + 1)), - Boost_Sin(TPOA * (i + 0)));
                        TPOA *= 3;
                        printf("\tmRF2 = _mm_set_ps(%f, %f, %f, %f);\n",
                               Boost_Cos(TPOA * (i + 3)), Boost_Cos(TPOA * (i + 2)),
                               Boost_Cos(TPOA * (i + 1)), Boost_Cos(TPOA * (i + 0)));
                        printf("\tmIF2 = _mm_set_ps(%f, %f, %f, %f);\n",
                               - Boost_Sin(TPOA * (i + 3)), - Boost_Sin(TPOA * (i + 2)),
                               - Boost_Sin(TPOA * (i + 1)), - Boost_Sin(TPOA * (i + 0)));
                        TPOA /= 3;

                        printf("\tmR = _mm_load_ps(SFRe + %d);\n", this);
                        printf("\tmI = _mm_load_ps(SFIm + %d);\n", this);
                        printf("\t_mm_store_ps(SFRe + %d, _mm_sub_ps(_mm_mul_ps(mR, mRF1), _mm_mul_ps(mI, mIF1)));\n", this);
                        printf("\t_mm_store_ps(SFIm + %d, _mm_add_ps(_mm_mul_ps(mR, mIF1), _mm_mul_ps(mI, mRF1)));\n", this);

                        printf("\tmR = _mm_load_ps(SFRe + %d);\n", another);
                        printf("\tmI = _mm_load_ps(SFIm + %d);\n", another);
                        printf("\t_mm_store_ps(SFRe + %d, _mm_sub_ps(_mm_mul_ps(mR, mRF2), _mm_mul_ps(mI, mIF2)));\n", another);
                        printf("\t_mm_store_ps(SFIm + %d, _mm_add_ps(_mm_mul_ps(mR, mIF2), _mm_mul_ps(mI, mRF2)));\n", another);

                        printf("\tmR = _mm_load_ps(SFRe + %d);\n", another);
                        printf("\tmI = _mm_load_ps(SFIm + %d);\n", another);

                        printf("\t_mm_store_ps(SFRe + %d, _mm_sub_ps(_mm_load_ps(SFIm + %d), mI));\n", another, this);
                        printf("\t_mm_store_ps(SFIm + %d, _mm_sub_ps(mR, _mm_load_ps(SFRe + %d)));\n", another, this);
                        printf("\t_mm_store_ps(SFRe + %d, _mm_add_ps(_mm_load_ps(SFRe + %d), mR));\n", this, this);
                        printf("\t_mm_store_ps(SFIm + %d, _mm_add_ps(_mm_load_ps(SFIm + %d), mI));\n", this, this);

                        i += 3;
                }else
                {
                        printf("\tRF1 = %f;\n", + Boost_Cos(TPOA * i));
                        printf("\tIF1 = %f;\n", - Boost_Sin(TPOA * i));
                        printf("\tRF2 = %f;\n", + Boost_Cos(TPOA * i * 3));
                        printf("\tIF2 = %f;\n", - Boost_Sin(TPOA * i * 3));

                        printf("\tR = SFRe[%d];\n", this);
                        printf("\tI = SFIm[%d];\n", this);
                        printf("\tSFRe[%d] = R * RF1 - I * IF1;\n", this);
                        printf("\tSFIm[%d] = R * IF1 + I * RF1;\n", this);

                        printf("\tR = SFRe[%d];\n", another);
                        printf("\tI = SFIm[%d];\n", another);
                        printf("\tSFRe[%d] = R * RF2 - I * IF2;\n", another);
                        printf("\tSFIm[%d] = R * IF2 + I * RF2;\n", another);

                        printf("\tR = SFRe[%d];\n", another);
                        printf("\tI = SFIm[%d];\n", another);
                        printf("\tSFRe[%d] = SFIm[%d] - I;\n", another, this);
                        printf("\tSFIm[%d] = R - SFRe[%d];\n", another, this);
                        printf("\tSFRe[%d] += R;\n", this);
                        printf("\tSFIm[%d] += I;\n", this);
                }
        }
        Start -= half;
        for(i = 0;i < half;i += 4)
        {
                this = Start + i;
                another = this + half;
                if(another % 8 == 0 && i < half - 7)
                {
                        printf("\tm256R = _mm256_add_ps(_mm256_load_ps(SFRe + %d), _mm256_load_ps(SFRe + %d));\n", (Start + i), (Start + half + i));
                        printf("\tm256I = _mm256_add_ps(_mm256_load_ps(SFIm + %d), _mm256_load_ps(SFIm + %d));\n", (Start + i), (Start + half + i));
                        printf("\t_mm256_store_ps(SFRe + %d, _mm256_sub_ps(_mm256_load_ps(SFRe + %d), _mm256_load_ps(SFRe + %d)));\n", (Start + half + i), (Start + i), (Start + half + i));
                        printf("\t_mm256_store_ps(SFIm + %d, _mm256_sub_ps(_mm256_load_ps(SFIm + %d), _mm256_load_ps(SFIm + %d)));\n", (Start + half + i), (Start + i), (Start + half + i));
                        printf("\t_mm256_store_ps(SFRe + %d, m256R);\n", (Start + i));
                        printf("\t_mm256_store_ps(SFIm + %d, m256I);\n", (Start + i));
                        i += 4;
                }else
                {
                        printf("\tmR = _mm_add_ps(_mm_load_ps(SFRe + %d), _mm_load_ps(SFRe + %d));\n", (Start + i), (Start + half + i));
                        printf("\tmI = _mm_add_ps(_mm_load_ps(SFIm + %d), _mm_load_ps(SFIm + %d));\n", (Start + i), (Start + half + i));
                        printf("\t_mm_store_ps(SFRe + %d, _mm_sub_ps(_mm_load_ps(SFRe + %d), _mm_load_ps(SFRe + %d)));\n", (Start + half + i), (Start + i), (Start + half + i));
                        printf("\t_mm_store_ps(SFIm + %d, _mm_sub_ps(_mm_load_ps(SFIm + %d), _mm_load_ps(SFIm + %d)));\n", (Start + half + i), (Start + i), (Start + half + i));
                        printf("\t_mm_store_ps(SFRe + %d, mR);\n", (Start + i));
                        printf("\t_mm_store_ps(SFIm + %d, mI);\n", (Start + i));
                }
        }
}
void SplitRadix_RadixL_Generator_SSE(int32_t Start, int32_t Amount)
{
	int32_t i, half, quarter;
	int32_t this, another;
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
			printf("\tmRF1 = _mm_set_ps(%f, %f, %f, %f);\n",
			       Boost_Cos(TPOA * (i + 3)), Boost_Cos(TPOA * (i + 2)),
			       Boost_Cos(TPOA * (i + 1)), Boost_Cos(TPOA * (i + 0)));
			printf("\tmIF1 = _mm_set_ps(%f, %f, %f, %f);\n",
			       - Boost_Sin(TPOA * (i + 3)), - Boost_Sin(TPOA * (i + 2)),
			       - Boost_Sin(TPOA * (i + 1)), - Boost_Sin(TPOA * (i + 0)));
			TPOA *= 3;
			printf("\tmRF2 = _mm_set_ps(%f, %f, %f, %f);\n",
			       Boost_Cos(TPOA * (i + 3)), Boost_Cos(TPOA * (i + 2)),
			       Boost_Cos(TPOA * (i + 1)), Boost_Cos(TPOA * (i + 0)));
			printf("\tmIF2 = _mm_set_ps(%f, %f, %f, %f);\n",
			       - Boost_Sin(TPOA * (i + 3)), - Boost_Sin(TPOA * (i + 2)),
			       - Boost_Sin(TPOA * (i + 1)), - Boost_Sin(TPOA * (i + 0)));
			TPOA /= 3;

			printf("\tmR = _mm_load_ps(SFRe + %d);\n", this);
			printf("\tmI = _mm_load_ps(SFIm + %d);\n", this);
			printf("\t_mm_store_ps(SFRe + %d, _mm_sub_ps(_mm_mul_ps(mR, mRF1), _mm_mul_ps(mI, mIF1)));\n", this);
			printf("\t_mm_store_ps(SFIm + %d, _mm_add_ps(_mm_mul_ps(mR, mIF1), _mm_mul_ps(mI, mRF1)));\n", this);

			printf("\tmR = _mm_load_ps(SFRe + %d);\n", another);
			printf("\tmI = _mm_load_ps(SFIm + %d);\n", another);
			printf("\t_mm_store_ps(SFRe + %d, _mm_sub_ps(_mm_mul_ps(mR, mRF2), _mm_mul_ps(mI, mIF2)));\n", another);
			printf("\t_mm_store_ps(SFIm + %d, _mm_add_ps(_mm_mul_ps(mR, mIF2), _mm_mul_ps(mI, mRF2)));\n", another);

			printf("\tmR = _mm_load_ps(SFRe + %d);\n", another);
			printf("\tmI = _mm_load_ps(SFIm + %d);\n", another);

			printf("\t_mm_store_ps(SFRe + %d, _mm_sub_ps(_mm_load_ps(SFIm + %d), mI));\n", another, this);
			printf("\t_mm_store_ps(SFIm + %d, _mm_sub_ps(mR, _mm_load_ps(SFRe + %d)));\n", another, this);
			printf("\t_mm_store_ps(SFRe + %d, _mm_add_ps(_mm_load_ps(SFRe + %d), mR));\n", this, this);
			printf("\t_mm_store_ps(SFIm + %d, _mm_add_ps(_mm_load_ps(SFIm + %d), mI));\n", this, this);

			i += 3;
		}else
		{
			printf("\tRF1 = %f;\n", + Boost_Cos(TPOA * i));
			printf("\tIF1 = %f;\n", - Boost_Sin(TPOA * i));
			printf("\tRF2 = %f;\n", + Boost_Cos(TPOA * i * 3));
			printf("\tIF2 = %f;\n", - Boost_Sin(TPOA * i * 3));

			printf("\tR = SFRe[%d];\n", this);
			printf("\tI = SFIm[%d];\n", this);
			printf("\tSFRe[%d] = R * RF1 - I * IF1;\n", this);
			printf("\tSFIm[%d] = R * IF1 + I * RF1;\n", this);

			printf("\tR = SFRe[%d];\n", another);
			printf("\tI = SFIm[%d];\n", another);
			printf("\tSFRe[%d] = R * RF2 - I * IF2;\n", another);
			printf("\tSFIm[%d] = R * IF2 + I * RF2;\n", another);

			printf("\tR = SFRe[%d];\n", another);
			printf("\tI = SFIm[%d];\n", another);
			printf("\tSFRe[%d] = SFIm[%d] - I;\n", another, this);
			printf("\tSFIm[%d] = R - SFRe[%d];\n", another, this);
			printf("\tSFRe[%d] += R;\n", this);
			printf("\tSFIm[%d] += I;\n", this);
		}
	}
	Start -= half;
	for(i = 0;i < half;i += 4)
	{
		printf("\tmR = _mm_add_ps(_mm_load_ps(SFRe + %d), _mm_load_ps(SFRe + %d));\n", (Start + i), (Start + half + i));
		printf("\tmI = _mm_add_ps(_mm_load_ps(SFIm + %d), _mm_load_ps(SFIm + %d));\n", (Start + i), (Start + half + i));
		printf("\t_mm_store_ps(SFRe + %d, _mm_sub_ps(_mm_load_ps(SFRe + %d), _mm_load_ps(SFRe + %d)));\n", (Start + half + i), (Start + i), (Start + half + i));
		printf("\t_mm_store_ps(SFIm + %d, _mm_sub_ps(_mm_load_ps(SFIm + %d), _mm_load_ps(SFIm + %d)));\n", (Start + half + i), (Start + i), (Start + half + i));
		printf("\t_mm_store_ps(SFRe + %d, mR);\n", (Start + i));
		printf("\t_mm_store_ps(SFIm + %d, mI);\n", (Start + i));
	}
}
void SplitRadix_Radix4_Generator(int32_t Start)
{
	printf("\tRe0 = SFRe[%d]; ", Start);
	printf("Im0 = SFIm[%d];\n", Start);
	printf("\tRe1 = SFRe[%d]; ", (Start + 1));
	printf("Im1 = SFIm[%d];\n", (Start + 1));
	printf("\tRe2 = SFRe[%d]; ", (Start + 2));
	printf("Im2 = SFIm[%d];\n", (Start + 2));
	printf("\tRe3 = SFRe[%d]; ", (Start + 3));
	printf("Im3 = SFIm[%d];\n", (Start + 3));

	printf("\tSFRe[%d] = Re0 + Re1; ", (Start + 0));
	printf("SFIm[%d] = Im0 + Im1;\n", (Start + 0));
	printf("\tSFRe[%d] = Re0 - Re1; ", (Start + 1));
	printf("SFIm[%d] = Im0 - Im1;\n", (Start + 1));
	printf("\tSFRe[%d] = Re2 + Re3; ", (Start + 2));
	printf("SFIm[%d] = Im2 + Im3;\n", (Start + 2));
	printf("\tSFRe[%d] = Im2 - Im3; ", (Start + 3));
	printf("SFIm[%d] = Re3 - Re2;\n", (Start + 3));

	printf("\tRA = SFRe[%d] + SFRe[%d];\n", (Start + 0), (Start + 2));
	printf("\tIA = SFIm[%d] + SFIm[%d];\n", (Start + 0), (Start + 2));
	printf("\tSFRe[%d] = SFRe[%d] - SFRe[%d];\n", (Start + 2), (Start + 0), (Start + 2));
	printf("\tSFIm[%d] = SFIm[%d] - SFIm[%d];\n", (Start + 2), (Start + 0), (Start + 2));
	printf("\tSFRe[%d] = RA;\n", (Start + 0));
	printf("\tSFIm[%d] = IA;\n", (Start + 0));
	
	printf("\tRA = SFRe[%d] + SFRe[%d];\n", (Start + 1), (Start + 3));
	printf("\tIA = SFIm[%d] + SFIm[%d];\n", (Start + 1), (Start + 3));
	printf("\tSFRe[%d] = SFRe[%d] - SFRe[%d];\n", (Start + 3), (Start + 1), (Start + 3));
	printf("\tSFIm[%d] = SFIm[%d] - SFIm[%d];\n", (Start + 3), (Start + 1), (Start + 3));
	printf("\tSFRe[%d] = RA;\n", (Start + 1));
	printf("\tSFIm[%d] = IA;\n", (Start + 1));
}
void SplitRadix_Radix2_Generator(int32_t Start)
{
	printf("\tR = SFRe[%d] + SFRe[%d];\n", Start, (Start + 1));
	printf("\tI = SFIm[%d] + SFIm[%d];\n", Start, (Start + 1));
	printf("\tSFRe[%d] = SFRe[%d] - SFRe[%d];\n", (Start + 1), Start, (Start + 1));
	printf("\tSFIm[%d] = SFIm[%d] - SFIm[%d];\n", (Start + 1), Start, (Start + 1));
	printf("\tSFRe[%d] = R;\n", Start);
	printf("\tSFIm[%d] = I;\n", Start);
}
