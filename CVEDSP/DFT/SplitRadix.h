/*
 * SplitRadix.h
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
#ifndef SPLITRADIX_H
#define SPLITRADIX_H
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

enum SREOperator
{
	OP_RADIX2,
	OP_RADIX4,
    OP_RADIXL
};

typedef struct SRExpression
{
	int32_t Number;
	int32_t* Operator;
	int32_t* Operand1;
	int32_t* Operand2;
} SRExpression;

extern void SRExpression_Ctor(SRExpression* Expression);
extern void SRExpression_Dtor(SRExpression* Expression);
extern inline void SRExpressionPush(SRExpression* Expression, int32_t Operator, int32_t Operand1, int32_t Operand2)
{
    Expression -> Operator[Expression -> Number] = Operator;
    Expression -> Operand1[Expression -> Number] = Operand1;
    Expression -> Operand2[Expression -> Number] = Operand2;
    Expression -> Number ++;
}
extern void SRExpressionGenerator_L(SRExpression* Expression, int32_t Start, int32_t Amount);
extern void SRExpressionGenerator_Radix2(SRExpression* Expression, int32_t Start);
extern void SRExpressionGenerator_Radix4(SRExpression* Expression, int32_t Start);

extern void SplitRadix_Interpreter(SRExpression* Expression, float Real[], float Imag[]);

extern void SplitRadix_RadixL(float Real[], float Imag[], int32_t Start, int32_t Amount);
extern void SplitRadix_Radix2(float Real[], float Imag[], int32_t Start);
extern void SplitRadix_Radix4(float Real[], float Imag[], int32_t Start);

#ifdef __cplusplus
}
#endif

#endif
