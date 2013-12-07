/*
 * Radix2Gen.c
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
#include "Radix2Gen.h"
#include "../IntrinUtil/Calculation.h"

#include <stdio.h>
#include <math.h>
#include <xmmintrin.h>

#ifdef __AVX__
#include <immintrin.h>
#endif

#define diff(a, b) ((a - b) > 0 ? (a - b) : (b - a))
#define IM 0xFFFE0000|
#define RE 0xFFFF0000|

int Radix2_Generator_SSE(int Power)
{	
	int Layer;
	int i, j, k, pmul, current, another;
	int Amount;
	float ReFactor, ImFactor;
	float TPOA, TPOATP;
	if(Power > 31 || Power < 1)
		return 0;

	Amount = pow(2, Power);
	TPOA = 2 * 3.1415926535 / Amount;

	for(Layer = 1;Layer <= Power;Layer ++)
	{
		j = pow(2, Layer - 1);
		pmul = pow(2, Power - Layer);
		
		for(i = 0;i < pmul;i ++)
		{
			TPOATP = TPOA * pmul;
			{
				//k = 0
				ReFactor = 1;
				ImFactor = 0;
				current = i * 2 * j;
				another = current + j;

				printf("\tTR = SFRe[%d] + SFRe[%d];\n", current, another);
				printf("\tTI = SFIm[%d] + SFIm[%d];\n", current, another);
				printf("\tSFRe[%d] = SFRe[%d] - SFRe[%d];\n", another, current, another);
				printf("\tSFIm[%d] = SFIm[%d] - SFIm[%d];\n", another, current, another);
				printf("\tSFRe[%d] = TR; SFIm[%d] = TI;\n", current, current);

			}
			for(k = 1;k < j;k ++)
			{
				current = i * 2 * j + k;
				another = current + j;

				if(current % 4 == 0 && k <= j - 4)
				{
					//Aligned
					printf("\tmRL = _mm_load_ps(SFRe + %d);\n", another);
					printf("\tmIL = _mm_load_ps(SFIm + %d);\n", another);
					
					printf("\tmR = mRL;\n");
					printf("\tmI = mIL;\n");

					printf("\tmRF = _mm_set_ps(%.9f, %.9f, %.9f, %.9f);\n",
					       cos(TPOATP * (k + 3)),
					       cos(TPOATP * (k + 2)),
					       cos(TPOATP * (k + 1)),
					       cos(TPOATP * (k + 0)));
					printf("\tmIF = _mm_set_ps(%.9f, %.9f, %.9f, %.9f);\n",
					       - sin(TPOATP * (k + 3)),
					       - sin(TPOATP * (k + 2)),
					       - sin(TPOATP * (k + 1)),
					       - sin(TPOATP * (k + 0)));

					printf("\tm0 = _mm_mul_ps(mR, mRF);\n");
					printf("\tm1 = _mm_mul_ps(mI, mIF);\n");
					printf("\tm2 = _mm_mul_ps(mI, mRF);\n");
					printf("\tm3 = _mm_mul_ps(mR, mIF);\n");
					
					printf("\tmR = _mm_sub_ps(m0, m1);\n");
					printf("\tmI = _mm_add_ps(m2, m3);\n");

					printf("\tmRC = _mm_load_ps(SFRe + %d);\n", current);
					printf("\tmIC = _mm_load_ps(SFIm + %d);\n", current);
					printf("\tm0 = mRC; m1 = mIC;\n");

					
					printf("\tmRC = _mm_add_ps(mRC, mR);\n");
					printf("\tmIC = _mm_add_ps(mIC, mI);\n");
					printf("\t_mm_store_ps(SFRe + %d, mRC);\n", current);
					printf("\t_mm_store_ps(SFIm + %d, mIC);\n", current);

					printf("\tm0 = _mm_sub_ps(m0, mR);\n");
					printf("\tm1 = _mm_sub_ps(m1, mI);\n");
					
					printf("\t_mm_store_ps(SFRe + %d, m0);\n", another);
					printf("\t_mm_store_ps(SFIm + %d, m1);\n", another);

					k += 3;
				}else
				{
					ReFactor = cos(TPOATP * k);
					ImFactor = - sin(TPOATP * k);

					if(! (diff(ImFactor, 0) < 0.00001 && diff(ReFactor, 1) < 0.00001))
					{
						printf("\tTR = SFRe[%d];\n", another);
						printf("\tSFRe[%d] = ", another);
						if(diff(ReFactor, 0) > 0.00001)
						{
							if(diff(ReFactor, 1) > 0.00001)
							{
								printf("SFRe[%d] * %.9f", another, ReFactor);
							}
							else
								printf("SFRe[%d]", another);
						}
						if(diff(ImFactor, 0) > 0.00001)
						{
							if(diff(ImFactor, 1) > 0.00001)
							{
								if(ImFactor > 0)
								{
									printf(" - ");
									printf("SFIm[%d] * %.9f", another, ImFactor);
								}else
								{
									printf(" + ");
									if(diff(-ImFactor, 1) > 0.00001)
									{
										printf("SFIm[%d] * %.9f", another, - ImFactor);
									}
									else
										printf("SFIm[%d]", another);
								}
							}
							else
							{
								printf(" - ");
								printf("SFIm[%d]", another);
							}
						}
						printf(";\n");


						printf("\tSFIm[%d] = ", another);
						if(diff(ReFactor, 0) > 0.00001)
						{
							if(diff(ReFactor, 1) > 0.00001)
							{
								printf("SFIm[%d] * %.9f", another, ReFactor);
							}
							else
								printf("SFIm[%d]", another);
						}
						if(diff(ImFactor, 1) > 0.00001)
						{
							if(ImFactor != 1)
							{
								if(ImFactor > 0)
								{
									printf(" + ");
									printf("TR * %.9f", ImFactor);
								}else
								{
									printf(" - ");
									if(diff(- ImFactor, 1) < 0.00001)
									{
										printf("TR");
									}
									else
									{
										printf("TR * %.9f", - ImFactor);
									}
								}
							}else
							{
								printf(" + ");
								printf("TR");
							}
						}
						printf(";\n");
					}

					printf("\tTR = SFRe[%d]; TI = SFIm[%d];\n", current, current);
					printf("\tSFRe[%d] += SFRe[%d]; SFIm[%d] += SFIm[%d];\n", current, another, current, another);
					printf("\tSFRe[%d] = TR - SFRe[%d];\n", another, another);
					printf("\tSFIm[%d] = TI - SFIm[%d];\n", another, another);

				}
			}
		}
	}
	return 1;
}

int Radix2_Generator_AVX(int Power)
{	
	int Layer;
	int i, j, k, pmul, current, another;
	int Amount;
	float ReFactor, ImFactor;
	float TPOA, TPOATP;
	if(Power > 31 || Power < 1)
		return 0;

	Amount = pow(2, Power);
	TPOA = 2 * 3.1415926535 / Amount;

	for(Layer = 1;Layer <= Power;Layer ++)
	{
		j = pow(2, Layer - 1);
		pmul = pow(2, Power - Layer);
		
		for(i = 0;i < pmul;i ++)
		{
			TPOATP = TPOA * pmul;
			{
				//k = 0
				ReFactor = 1;
				ImFactor = 0;
				current = i * 2 * j;
				another = current + j;

				printf("\tTR = SFRe[%d] + SFRe[%d];\n", current, another);
				printf("\tTI = SFIm[%d] + SFIm[%d];\n", current, another);
				printf("\tSFRe[%d] = SFRe[%d] - SFRe[%d];\n", another, current, another);
				printf("\tSFIm[%d] = SFIm[%d] - SFIm[%d];\n", another, current, another);
				printf("\tSFRe[%d] = TR; SFIm[%d] = TI;\n", current, current);

			}
			for(k = 1;k < j;k ++)
			{
				current = i * 2 * j + k;
				another = current + j;

				if(current % 8 == 0 && k <= j - 8)
				{
					//Aligned
					printf("\tmRL256 = _mm256_load_ps(SFRe + %d);\n", another);
					printf("\tmIL256 = _mm256_load_ps(SFIm + %d);\n", another);
					
					printf("\tmR256 = mRL256;\n");
					printf("\tmI256 = mIL256;\n");

					printf("\tmRF256 = _mm256_set_ps(%.9f, %.9f, %.9f, %.9f, %.9f, %.9f, %.9f, %.9f);\n",
					       cos(TPOATP * (k + 7)),
					       cos(TPOATP * (k + 6)),
					       cos(TPOATP * (k + 5)),
					       cos(TPOATP * (k + 4)),
					       cos(TPOATP * (k + 3)),
					       cos(TPOATP * (k + 2)),
					       cos(TPOATP * (k + 1)),
					       cos(TPOATP * (k + 0)));
					printf("\tmIF256 = _mm256_set_ps(%.9f, %.9f, %.9f, %.9f, %.9f, %.9f, %.9f, %.9f);\n",
					       - sin(TPOATP * (k + 7)),
					       - sin(TPOATP * (k + 6)),
					       - sin(TPOATP * (k + 5)),
					       - sin(TPOATP * (k + 4)),
					       - sin(TPOATP * (k + 3)),
					       - sin(TPOATP * (k + 2)),
					       - sin(TPOATP * (k + 1)),
					       - sin(TPOATP * (k + 0)));

					printf("\tm4 = _mm256_mul_ps(mR256, mRF256);\n");
					printf("\tm5 = _mm256_mul_ps(mI256, mIF256);\n");
					printf("\tm6 = _mm256_mul_ps(mI256, mRF256);\n");
					printf("\tm7 = _mm256_mul_ps(mR256, mIF256);\n");
					
					printf("\tmR256 = _mm256_sub_ps(m4, m5);\n");
					printf("\tmI256 = _mm256_add_ps(m6, m7);\n");

					printf("\tmRC256 = _mm256_load_ps(SFRe + %d);\n", current);
					printf("\tmIC256 = _mm256_load_ps(SFIm + %d);\n", current);
					printf("\tm4 = mRC256; m5 = mIC256;\n");

					
					printf("\tmRC256 = _mm256_add_ps(mRC256, mR256);\n");
					printf("\tmIC256 = _mm256_add_ps(mIC256, mI256);\n");
					printf("\t_mm256_store_ps(SFRe + %d, mRC256);\n", current);
					printf("\t_mm256_store_ps(SFIm + %d, mIC256);\n", current);

					printf("\tm4 = _mm256_sub_ps(m4, mR256);\n");
					printf("\tm5 = _mm256_sub_ps(m5, mI256);\n");
					
					printf("\t_mm256_store_ps(SFRe + %d, m4);\n", another);
					printf("\t_mm256_store_ps(SFIm + %d, m5);\n", another);

					k += 7;
				}else if(current % 4 == 0 && k <= j - 4)
				{
					//Aligned
					
					printf("\tmRL = _mm_load_ps(SFRe + %d);\n", another);
					printf("\tmIL = _mm_load_ps(SFIm + %d);\n", another);
					
					printf("\tmR = mRL;\n");
					printf("\tmI = mIL;\n");

					printf("\tmRF = _mm_set_ps(%.9f, %.9f, %.9f, %.9f);\n",
					       cos(TPOATP * (k + 3)),
					       cos(TPOATP * (k + 2)),
					       cos(TPOATP * (k + 1)),
					       cos(TPOATP * (k + 0)));
					printf("\tmIF = _mm_set_ps(%.9f, %.9f, %.9f, %.9f);\n",
					       - sin(TPOATP * (k + 3)),
					       - sin(TPOATP * (k + 2)),
					       - sin(TPOATP * (k + 1)),
					       - sin(TPOATP * (k + 0)));

					printf("\tm0 = _mm_mul_ps(mR, mRF);\n");
					printf("\tm1 = _mm_mul_ps(mI, mIF);\n");
					printf("\tm2 = _mm_mul_ps(mI, mRF);\n");
					printf("\tm3 = _mm_mul_ps(mR, mIF);\n");
					
					printf("\tmR = _mm_sub_ps(m0, m1);\n");
					printf("\tmI = _mm_add_ps(m2, m3);\n");

					printf("\tmRC = _mm_load_ps(SFRe + %d);\n", current);
					printf("\tmIC = _mm_load_ps(SFIm + %d);\n", current);
					printf("\tm0 = mRC; m1 = mIC;\n");

					
					printf("\tmRC = _mm_add_ps(mRC, mR);\n");
					printf("\tmIC = _mm_add_ps(mIC, mI);\n");
					printf("\t_mm_store_ps(SFRe + %d, mRC);\n", current);
					printf("\t_mm_store_ps(SFIm + %d, mIC);\n", current);

					printf("\tm0 = _mm_sub_ps(m0, mR);\n");
					printf("\tm1 = _mm_sub_ps(m1, mI);\n");
					
					printf("\t_mm_store_ps(SFRe + %d, m0);\n", another);
					printf("\t_mm_store_ps(SFIm + %d, m1);\n", another);

					k += 3;
				}else
				{
					ReFactor = cos(TPOATP * k);
					ImFactor = - sin(TPOATP * k);

					if(! (diff(ImFactor, 0) < 0.00001 && diff(ReFactor, 1) < 0.00001))
					{
						printf("\tTR = SFRe[%d];\n", another);
						printf("\tSFRe[%d] = ", another);
						if(diff(ReFactor, 0) > 0.00001)
						{
							if(diff(ReFactor, 1) > 0.00001)
							{
								printf("SFRe[%d] * %.9f", another, ReFactor);
							}
							else
								printf("SFRe[%d]", another);
						}
						if(diff(ImFactor, 0) > 0.00001)
						{
							if(diff(ImFactor, 1) > 0.00001)
							{
								if(ImFactor > 0)
								{
									printf(" - ");
									printf("SFIm[%d] * %.9f", another, ImFactor);
								}else
								{
									printf(" + ");
									if(diff(-ImFactor, 1) > 0.00001)
									{
										printf("SFIm[%d] * %.9f", another, - ImFactor);
									}
									else
										printf("SFIm[%d]", another);
								}
							}
							else
							{
								printf(" - ");
								printf("SFIm[%d]", another);
							}
						}
						printf(";\n");


						printf("\tSFIm[%d] = ", another);
						if(diff(ReFactor, 0) > 0.00001)
						{
							if(diff(ReFactor, 1) > 0.00001)
							{
								printf("SFIm[%d] * %.9f", another, ReFactor);
							}
							else
								printf("SFIm[%d]", another);
						}
						if(diff(ImFactor, 1) > 0.00001)
						{
							if(ImFactor != 1)
							{
								if(ImFactor > 0)
								{
									printf(" + ");
									printf("TR * %.9f", ImFactor);
								}else
								{
									printf(" - ");
									if(diff(- ImFactor, 1) < 0.00001)
									{
										printf("TR");
									}
									else
									{
										printf("TR * %.9f", - ImFactor);
									}
								}
							}else
							{
								printf(" + ");
								printf("TR");
							}
						}
						printf(";\n");
					}

					printf("\tTR = SFRe[%d]; TI = SFIm[%d];\n", current, current);
					printf("\tSFRe[%d] += SFRe[%d]; SFIm[%d] += SFIm[%d];\n", current, another, current, another);
					printf("\tSFRe[%d] = TR - SFRe[%d];\n", another, another);
					printf("\tSFIm[%d] = TI - SFIm[%d];\n", another, another);
				}
			}
		}
	}
	return 1;
}
