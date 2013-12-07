#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#ifdef __cplusplus
extern "C"{
#endif

typedef void (*pFunc)(float*, int);

float ProcessorTmp_Re[16384];
float ProcessorTmp_Im[16384];
float ProcessorTmp_Filter[16384];

extern void Process(float* Dest, float* Src, pFunc Processor, int Power, int Length);
extern void Processor_Filter(float* Wave, int Power);

#ifdef __cplusplus
}
#endif

#endif
