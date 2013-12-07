#include "Interpolation.h"
#include "IntrinUtil/Calculation.h"
#include <stdio.h>
/* NOT FINISHED YET */

Parabola GenerateQuad(float x0, float y0, float x1, float y1, float x2, float y2)
{
	Parabola ret;
	ret.a = ((y0 - y1) / (x0 - x1) - (y1 - y2) / (x1 - x2)) / (x0 - x2);
	ret.b = (y0 - y1) / (x0 - x1) - ret.a * (x0 + x1);
	ret.c = y0 - ret.b * x0 - ret.a * x0 * x0;
	return ret;
}

//The following functions are adapted from
//http://paulbourke.net/miscellaneous/interpolation/
float LinearInterpolate(float y0, float y1, float Ratio)
{
    return y0 * (1 - Ratio) + y1 * Ratio;
}

float CosineInterpolate(float y0, float y1, float Ratio)
{
    float CosRatio = (1 - Boost_Cos(Ratio * 3.1415926)) / 2;
    return y0 * (1 - CosRatio) + y1 * CosRatio;
}

float CubicInterpolate(float y0, float y1, float y2, float y3, float Ratio)
{
    float a0, a1, a2, a3, CubicRatio;

    CubicRatio = Ratio * Ratio;
    a0 = y3 - y2 - y0 + y1;
    a1 = y0 - y1 - a0;
    a2 = y2 - y0;
    a3 = y1;

    return(a0 * Ratio * CubicRatio + a1 * CubicRatio + a2 * Ratio + a3);
}
