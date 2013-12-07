#include "Tester.h"
#include <malloc.h>
#include <stdio.h>

//Tester should only be used for algorism testing, and should not be contained in any formal projects.

void** GCList;
int GCNum;
short int* WaveBuff;

void __attribute__ ((constructor)) TesterInit()
{
	WaveBuff = (short int*)malloc(44100 * 2 * 10);
	GCNum = 0;
	GCList = (void**)malloc(sizeof(void*) * 65536);
}
void __attribute__ ((destructor)) TesterExit()
{
	for(;GCNum > 0;GCNum --)
		free(GCList[GCNum]);
	free(WaveBuff);
	free(GCList);
}

void* GCMalloc(int Size)
{
	GCList[GCNum ++] = malloc(Size);
	return GCList[GCNum - 1];
}
/*
int LoadWave(float* Dest, const char* Path)
{
	wave_reader_error waveerr;
	wave_reader* WaveReader = wave_reader_open(Path, &waveerr);
	int WaveSize = wave_reader_get_num_samples(WaveReader);
	int j;
	wave_reader_get_samples(WaveReader, WaveSize, WaveBuff);
	
	for(j = 0;j < WaveSize;j ++)
	{
		Dest[j] = (float)WaveBuff[j] / 32767;
	}
	
	wave_reader_close(WaveReader);
	return WaveSize;
}
int WriteWave(const char* Path, float* Src, int Length)
{
	wave_writer_format format;
	format.num_channels = 1;
	format.sample_rate = 44100;
	format.sample_bits = 16;
	wave_writer_error wavewerr;
	int i;
	
    wave_writer* writer = wave_writer_open(Path, &format, &wavewerr);
	
	for(i = 0;i < Length;i ++)
		WaveBuff[i] = 32767 * Src[i];

    wave_writer_put_samples(writer, Length, WaveBuff);
    wave_writer_close(writer, &wavewerr);
	return 1;
}
*/
