#ifndef TESTER_H
#define TESTER_H

#ifdef __cplusplus
extern "C"{
#endif

#define _Size_Wave (44100 * 10 * sizeof(float))
#define CreateFloatArray(Name, Size) float* (Name) = GCMalloc(Size)

//Memory allocated with GCMalloc are automatically collected when program is terminated,
//and should not be freed manually.
extern void* GCMalloc(int Size);

extern int LoadWave(float* Dest, const char* Path);
extern int WriteWave(const char* Path, float* Src, int Length);

#ifdef __cplusplus
}
#endif

#endif
