#ifndef INTERPOLATION_H
#define INTERPOLATION_H
/* NOT FINISHED YET */

#ifdef __cplusplus
extern "C"{
#endif

typedef struct
{
	float a;
	float b;
	float c;
} Parabola;

extern Parabola GenerateQuad(float x0, float y0, float x1, float y1, float x2, float y2);
extern float LinearInterpolate(float y0, float y1, float Ratio);
extern float CosineInterpolate(float y0, float y1, float Ratio);
extern float CubicInterpolate(float y0, float y1, float y2, float y3, float Ratio);

#ifdef __cplusplus
}
#endif

#endif
