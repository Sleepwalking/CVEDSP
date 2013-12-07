#ifndef PSOLA_H
#define PSOLA_H
#include <stdint.h>
#include "../RUtil/RUtil.h"

#ifdef __cplusplus
extern "C"{
#endif

enum PSOLAMixType
{
	MixFront = 0,
	MixNormal = 1,
	MixBack = 2
};

AutoClass
{
	int Length;
	float* Data;
} EndClass(PSOLAFrame);

typedef struct
{
	int Amount;
	int VoiceOnsetIndex;
} PulseDescriptor;

#define PSOLAFrame_StaticCtor(Dest, _Length) (Dest) -> Length = (_Length)
#define PSOLAFrame_StaticDtor(Dest) do{}while(0)
extern void PSOLAFrame_CtorSize(PSOLAFrame* Dest, int32_t Length);
extern void PSOLAFrame_DtorAll(PSOLAFrame* Dest, int32_t Amount);

extern void PSOLAFrame_Resize(PSOLAFrame* Dest, int32_t Length);

extern void PSOLAFrame_Get(PSOLAFrame* Dest, float* Src, int32_t Position);
extern void PSOLAFrame_SecureGet(PSOLAFrame* Dest, float* Src, int32_t UBound, int32_t Position);
extern void PSOLAFrame_StaticGet(PSOLAFrame* Dest, float* Src, int32_t Position);
extern void PSOLAFrame_Mix(float* Dest, PSOLAFrame* Src, int32_t Front, int32_t Position, int32_t Back);
extern void PSOLAFrame_MixByRatio(float* Dest, PSOLAFrame* Src, int32_t Front, int32_t Position, int32_t Back, float Ratio, int MType);
extern void PSOLAFrame_MixByRatio_Float(float* Dest, PSOLAFrame* Src, float Front, float Position, float Back, float Ratio, int MType);

extern void SetExtractorVoiceThreshold(float _Threshold);
extern void SetExtractorVolumeThreshold(float _Threshold);
extern void ExtractPulsesByBaseFrequency(int32_t* DestPulses, PulseDescriptor* DestDescriptor, float* Wave, float BaseFrequency, int32_t Length);

extern void PSOLA_Regenesis(float* Dest, PSOLAFrame* Frames, int32_t* NewPulses, int32_t* OriginalPulses, int32_t PulseNum);

#ifdef __cplusplus
}
#endif

#endif
