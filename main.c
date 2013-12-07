/*
 * main.c
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "Plot.h"
#include "DFT/FFT.h"
#include "DFT/DFT.h"
#include "DFT/Radix2.h"
#include "IntrinUtil/FloatArray.h"
#include "IntrinUtil/Calculation.h"
#include "IntrinUtil/Memory.h"
#include "DSPBase/Spectrum.h"
#include "DSPBase/Filter.h"
#include "DSPBase/LinearWarping.h"
#include "DSPBase/ControlPointFilter.h"
#include "DSPBase/Window.h"
#include "DSPBase/TDEffects.h"
#include "Algorithm/PSOLA.h"
#include "Algorithm/Formant.h"
#include "Algorithm/FECSOLA.h"
#include "Algorithm/BaseFreq.h"
#include "FrameProcessor.h"
#include "Interpolation.h"

#include "DFT/SplitRadixGen.h"
#include "DFT/SplitRadix.h"
#include "DFT/Radix2Gen.h"

#include "Tester.h"

#include <math.h>

int m = 0;
int main(int argc,char **argv)
{
    int i, j;
/*
    int t0, t1, t2, t3, t4, t5, t6;
	SRExpression OPExp;
	SRExpressionCtor(&OPExp);
	SRExpressionGenerator_L(&OPExp, 0, 16);

	SplitRadix_Generator(&OPExp, 0);
	
	SRExpressionDtor(&OPExp);
	//Radix2_Generator_AVX(4);
	*/
    /*
	float* Wave1 = GCMalloc(_Size_Wave);
	float* Wave3= GCMalloc(_Size_Wave);
	float* Wave2 = GCMalloc(_Size_Wave);
	float* WaveOut = GCMalloc(_Size_Wave);

	//for(i = 0;i < 100000;i ++)
	//FFT(Wave1, Wave3, Wave1, Wave3, 11);
	
	
	int WaveSize1;
	int WaveSize2;
	float Hamming[1024];
	GenerateHamming(Hamming, 1024);
//----------------------------
	t0 = clock();

	CPF Formant;
	CPF Formant2;
	CPF_Ctor(&Formant, 1000);
	CPF_Ctor(&Formant2, 1000);
	Formant.UFreq = 22050;
	Formant2.UFreq = 22050;

//----------------------------
	t1 = clock();
	
	GNUPlot_Open();
	GNUPlot_SetTitle("Modified Formant Envelope Coefficient");

	char F1Name[1000];
	char F2Name[1000];
	strcpy(F1Name,argv[1]);
	strcpy(F2Name,argv[2]);
	
	SetSampleRate(44100);
	
	WaveSize1 = LoadWave(Wave1, F1Name);
	WaveSize2 = LoadWave(Wave3, F2Name);

	for(i = 0;i < WaveSize2;i ++)
	{
		float Target = i * 1.5;
		float Exceed = Target - (int)Target;
		Wave2[i] = Wave3[(int)Target] * (1.0f - Exceed) + Wave3[(int)Target + 1] * Exceed;
	}
	WaveSize2 /= 3;

//----------------------------
	t2 = clock();
	
	float BF1 = GetBaseFrequencyFromWave(Wave1 + WaveSize1 / 2, 100, 1500, 13);
	float BF2 = GetBaseFrequencyFromWave(Wave2 + WaveSize2 / 2, 100, 1500, 13);
	printf("%f Hz & %f Hz\n", BF1, BF2);

//----------------------------
	t3 = clock();
	
	int Pulses1[3500];
	PulseDescriptor PD1;
	int Pulses2[3500];
	PulseDescriptor PD2;
	ExtractPulsesByBaseFrequency(Pulses1, &PD1, Wave1, BF1, WaveSize1);
	ExtractPulsesByBaseFrequency(Pulses2, &PD2, Wave2, BF2, WaveSize2);

	PD1.Amount *= 1;
	PD2.Amount *= 1;
	
//----------------------------
	t4 = clock();
	
	printf("Amount1 = %d, VOI1 = %d.\n", PD1.Amount, PD1.VoiceOnsetIndex);
	printf("Amount2 = %d, VOI2 = %d.\n", PD2.Amount, PD2.VoiceOnsetIndex);

	PSOLAFrame* PFrames1= GCMalloc(sizeof(PSOLAFrame) * PD1.Amount);
	PSOLAFrame* PFrames2= GCMalloc(sizeof(PSOLAFrame) * PD2.Amount);
	
	float Spec[1024];
	float Spec2[1024];

	FECSOLAFilter FormantFilter;
	FECSOLAState Formant_a;
	FECSOLAState Formant_i;
	FECSOLAState Formant_now;
	FECSOLAFilter_Ctor(&FormantFilter, 2048);

	CreateFormant(Formant_a,
	              BF1, 1300, 1700, 4000,
	              600, 600, 600, 600,
	              1, 1, 1, 1);
	
	CreateFormant(Formant_i,
	              BF1, BF1 + 50, 3900, 4000,
	              600, 600, 600, 600,
	              1, 0.7, 0.4, 1);
	
	for(i = 0;i < PD1.Amount;i ++)
	{
		PSOLAFrame_Ctor(&PFrames1[i], 1024);
		PSOLAFrame_Get(&PFrames1[i], Wave1, Pulses1[i]);
	}
	for(i = 0;i < PD2.Amount;i ++)
	{
		PSOLAFrame_Ctor(&PFrames2[i], 1024);
		PSOLAFrame_Get(&PFrames2[i], Wave2, Pulses2[i]);
		Formant_a.F0 = BF1;
		GetMagnitudeFromWave(Spec, PFrames1[i].Data, Hamming, 10);

		ExtractFormantCPF(& Formant, Spec, BF1, 1024);
		CPF_Bake(Spec, & Formant, 512);

		FECSOLAFilter_GetFromFormantEnvelope(& FormantFilter, & Formant, & Formant_a);
		Formant_a.F0 = BF2;

		Boost_FloatSet(ProcessorTmp_Filter, 0, 1024);
		
		Formant.ControlPointQ = 0;
		GetMagnitudeFromWave(Spec2, PFrames2[i].Data, Hamming, 10);
		//GNUPlot_PlotFloat(Spec2, 500);
		//WaitForDraw(20000);
		ExtractFormantCPF(& Formant, Spec2, BF2, 1024);
		CPF_Bake(Spec2, & Formant, 512);

		//GNUPlot_PlotFloat(Spec2, 500);
		//WaitForDraw(20000);
		
		Boost_FloatAdd(Spec2, Spec2, 0.0001, 512);
		Boost_FloatRcp(ProcessorTmp_Filter, Spec2, 512);
		Processor_Filter(PFrames2[i].Data, 10);
		
		Boost_FloatSet(ProcessorTmp_Filter, 0, 512);
		FECSOLAFilter_Bake(ProcessorTmp_Filter, & FormantFilter, & Formant_a, 1024);
		Boost_FloatCopy(ProcessorTmp_Filter + 139, Spec + 139, 512 - 139);
		
		GetMagnitudeFromWave(Spec, PFrames1[i].Data, Hamming, 10);
		//GNUPlot_PlotFloat(Spec, 500);
		//WaitForDraw(20000);
		//GNUPlot_PlotFloat(ProcessorTmp_Filter, 100);
		//WaitForDraw(20000);
		Processor_Filter(PFrames2[i].Data, 10);

		Boost_FloatDivArr(PFrames2[i].Data, PFrames2[i].Data, Hamming, 1024);		

		Formant.ControlPointQ = 0;
	}
	
	
		if(i == PD1.Amount / 2)
		{
			GetMagnitudeFromWave(Spec2, PFrames1[i].Data, Hamming, 10);
			for(j = 0;j < 512;j ++)
			{
				Spec[j / 2] = Spec2[j];
			}
			ExtractFormantCPF(& Formant, Spec, BF1, 1024);
			FECSOLAFilter_GetFromFormantEnvelope(& FormantFilter, & Formant, & Formant_a);
			printf("Envelope Gotten.\n");
		}

	FECSOLAFilter_Dtor(&FormantFilter);
//----------------------------
	t5 = clock();
	
	Boost_FloatSet(WaveOut, 0, WaveSize1 + WaveSize2);

	int PulsesMod[3500];
	for(i = 0;i < 3500;i ++)
		PulsesMod[i] = (float)Pulses2[i] / 1;
	PSOLA_Regenesis(WaveOut, PFrames2, PulsesMod, Pulses2, PD2.Amount);

//----------------------------
	t6 = clock();

	PSOLAFrame_DtorAll(PFrames1, PD1.Amount);
	PSOLAFrame_DtorAll(PFrames2, PD2.Amount);
	
	WriteWave("/tmp/output.wav", WaveOut, WaveSize1 + WaveSize2);

	CPF_Dtor(&Formant);
	CPF_Dtor(&Formant2);
	
	GNUPlot_Close();

	printf("Memory allocated: %d\n", t0);
	printf("CPF generated: %d\n", t1);
	printf("Wave loaded: %d\n", t2);
	printf("Base frequency gotten: %d\n", t3);
	printf("Pulses extracted: %d\n", t4);
	printf("PSOLA frames filtered: %d\n", t5);
	printf("Mixed: %d\n", t6);
    */
	return 0;
}

