#include "FrameProcessor.h"
#include "DSPBase/Window.h"
#include "IntrinUtil/FloatArray.h"
#include "DFT/FFT.h"
#include "DSPBase/Filter.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void Process(float* Dest, float* Src, pFunc Processor, int Power, int Length)
{
	int FrameNum;
	int Frame;
	int Half;
	int i;
	float* HWindow;
	float* TWindow;
	float* Wave;
	float* WaveNext;
	float* Tmp;

	Frame = pow(2, Power);
	Half = Frame / 2;
	FrameNum = Length / Half;
	HWindow = malloc(Frame * 4);
	TWindow = malloc(Frame * 4);
	Wave = malloc(Frame * 4);
	WaveNext = malloc(Frame * 4);
	GenerateHamming(HWindow, Frame);
	GenerateTriang(TWindow, Frame);
	Boost_FloatDivArr(TWindow, TWindow, HWindow, Frame);

	Boost_FloatCopy(WaveNext, Src, Frame);
	Boost_FloatSet(Dest, 0, Half);
	for(i = 0;i < FrameNum - 1;i ++)
	{
		Tmp = Wave;
		Wave = WaveNext;
		WaveNext = Tmp;
		if(i < FrameNum - 1)
			Boost_FloatCopy(WaveNext, Src + i * Half + Half, Frame);
		Boost_FloatMulArr(Wave, Wave, HWindow, Frame);
		Processor(Wave, Power);
		Boost_FloatMulArr(Wave, Wave, TWindow, Frame);
		Boost_FloatSet(Dest + i * Half + Half, 0, Half);
        Boost_FloatAddArr(Dest + i * Half, Dest + i * Half, Wave, Frame);
	}

	free(Wave);
	free(WaveNext);
	free(HWindow);
	free(TWindow);
}

void Processor_Filter(float* Wave, int Power)
{
	RFFT(ProcessorTmp_Re, ProcessorTmp_Im, Wave,  Power);
	ApplyFilter(ProcessorTmp_Re, ProcessorTmp_Im, ProcessorTmp_Re, ProcessorTmp_Im, ProcessorTmp_Filter, Power);
	RIFFT(Wave, ProcessorTmp_Re, ProcessorTmp_Im, Power);
}
