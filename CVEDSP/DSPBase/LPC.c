/*
* LPC.c
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

#include "LPC.h"
#include "IntrinUtil/FloatArray.h"
#include "IntrinUtil/Calculation.h"
#include "Spectrum.h"
#include <math.h>
#include <malloc.h>

//Adapted from
//http://www.emptyloop.com/technotes/A%20tutorial%20on%20linear%20prediction%20and%20Levinson-Durbin.pdf
void LPCFromWave(float* Dest, float* Src, int Length, int CoefNum)
{
    int i, j;
    float E, Lambda, Tmp;
    float* R = (float*)malloc(sizeof(float) * CoefNum);
    Boost_FloatSet(R, 0, CoefNum);
    Boost_FloatSet(Dest, 0, CoefNum);

    //Auto-Correlation
    for(i = 0; i < CoefNum; i ++)
    {
        R[i] = 0;
        for(j = 0; j < Length - i; j ++)
            R[i] += Src[j] * Src[j + i];
    }

    //Initial Condition
    Dest[0] = 1;
    E = R[0];

    //Levinson-Durbin Recursion
    for(i = 0; i < CoefNum - 1; i ++)
    {
        Lambda = 0;
        for(j = 0; j <= i; j ++)
            Lambda -= Dest[j] * R[i + 1 - j];
        Lambda /= E;

        for(j = 0; j <= (i + 1) / 2; j ++)
        {
            Tmp = Dest[i + 1 - j] + Lambda * Dest[j];
            Dest[j] += Lambda * Dest[i + 1 - j];
            Dest[i + 1 - j] = Tmp;
        }

        E *= (1.0f - Lambda * Lambda);
    }
    free(R);
}

void SpectralEnvelopeFromLPC(float* Dest, float* Src, int CoefNum, int Power)
{
    int Amount = pow(2, Power);
    float* tmp = (float*)malloc(Amount * 2 * sizeof(float));
    Boost_FloatSet(tmp, 0, Amount);
    Boost_FloatCopy(tmp, Src, CoefNum);
    MagnitudeFromWave(Dest, tmp, Power);
    Boost_FloatRcp(Dest, Dest, Amount);
    free(tmp);
}
