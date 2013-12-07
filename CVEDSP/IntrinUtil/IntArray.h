/*
 * IntArray.h
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
#ifndef INTARRAY_H
#define INTARRAY_H
#ifdef __SSE2__
#include <emmintrin.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

//Requires SSE2 support.

extern void Boost_IntAdd(int32_t* Dest, int32_t Data, int Amount);
extern void Boost_IntSub(int32_t* Dest, int32_t Data, int Amount);
extern void Boost_IntSubBy(int32_t* Dest, int32_t Data, int Amount);
extern void Boost_IntAddArr(int32_t* Dest, int32_t* Data1, int* Data2, int Amount);
extern void Boost_IntSubArr(int32_t* Dest, int32_t* Data1, int* Data2, int Amount);

extern void Boost_ByteAdd(char* Dest, char Data, int DWAmount);
extern void Boost_ByteSub(char* Dest, char Data, int DWAmount);
extern void Boost_ByteSubBy(char* Dest, char Data, int DWAmount);
extern void Boost_UByteAvgArr(char* Dest, char* Data1, char* Data2, int DWAmount);
extern void Boost_ByteAddArr(char* Dest, char* Data1, char* Data2, int DWAmount);
extern void Boost_ByteSubArr(char* Dest, char* Data1, char* Data2, int DWAmount);

extern void Boost_ShortAdd(int16_t* Dest, int16_t Data, int DWAmount);
extern void Boost_ShortSub(int16_t* Dest, int16_t Data, int DWAmount);
extern void Boost_ShortSubBy(int16_t* Dest, int16_t Data, int DWAmount);
extern void Boost_UShortAvgArr(int16_t* Dest, int16_t* Data1, int16_t* Data2, int DWAmount);
extern void Boost_ShortAddArr(int16_t* Dest, int16_t* Data1, int16_t* Data2, int DWAmount);
extern void Boost_ShortSubArr(int16_t* Dest, int16_t* Data1, int16_t* Data2, int DWAmount);

#ifdef __cplusplus
}
#endif

#endif
#endif
