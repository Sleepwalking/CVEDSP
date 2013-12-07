#ifndef SPEECHDETECTOR_H
#define SPEECHDETECTOR_H

#ifdef __cplusplus
extern "C"{
#endif

typedef struct
{
    float VolumeThreshold;
    float VoiceThreshold;

    float PreProcessLowFreq;
    float PreProcessHighFreqFactor;
    float PreProcessMovingAverageRatio;

    float Delay;
} SpeechDetectorParameters;

extern SpeechDetectorParameters SPara;

extern void SDetector_Init() __attribute__ ((constructor));
extern void SDetector_SetParameters(SpeechDetectorParameters Src);

extern void SpeechDetectorPreProcess(float* Dest, float* Src, float BaseFreq, int Length);

extern float DetectVocalOnset(float* Src, int Length);
extern float DetectVOTWithBaseFreq(float* Src, float BaseFreq, int Length);

#ifdef __cplusplus
}
#endif

#endif
