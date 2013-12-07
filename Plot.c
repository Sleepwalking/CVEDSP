/*
 * Plot.c
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
#include "Plot.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

FILE* GNUPlot;
FILE* Output;

void GNUPlot_Open()
{
    GNUPlot = popen("gnuplot","w");
	fprintf(GNUPlot, "set mouse ruler at {0, 0}\n");
}
void GNUPlot_Close()
{
	fflush(GNUPlot);
	fclose(GNUPlot);
}

void GNUPlot_SetTitle(const char* Title)
{
	fprintf(GNUPlot, "set title \"%s\"\n", Title);
}
void GNUPlot_SetTitleAndNumber(const char* Title, int Number)
{
	fprintf(GNUPlot, "set title \"%s %d\"\n", Title, Number);
}

void WaitForDraw(int Time)
{
    usleep(Time);
//	clock_t now = clock();
//	while(clock() - now < Time);
}

void GNUPlot_PlotFloat(float* Array, int Amount)
{
	int i;
	Output = fopen("/tmp/plot.txt", "ws");
	for(i = 0;i < Amount;i ++)
	{
		fprintf(Output, "%f\n", Array[i]);
	}
	fflush(Output);
	fclose(Output);
    fprintf(GNUPlot, "plot [0:%d][0:7] \"/tmp/plot.txt\" with line\n", Amount);
	fflush(GNUPlot);
}
