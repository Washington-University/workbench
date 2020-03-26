/*LICENSE_START*/
/*
 *  Copyright (C) 2020  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include "ColorFunctions.h"

#include <cmath>

using namespace std;
using namespace caret;

namespace
{
    float sRGBinvGamma(const float nonlin)
    {//https://en.wikipedia.org/wiki/SRGB
        if (nonlin <= 0.04045f)
        {
            return 25 * nonlin / 323;
        } else {
            return pow((nonlin + 0.055) / 1.055f, 2.4);
        }
    }

    float Labf(const float t)
    {//https://en.wikipedia.org/wiki/CIELAB_color_space
        if (t > (6.0f / 29.0f) * (6.0f / 29.0f) * (6.0f / 29.0f))//assume this is more likely to be optimized out than pow()
        {
            return pow(t, 1.0f / 3.0f);
        } else {
            return t / (3.0f * (6.0f / 29.0f) * (6.0f / 29.0f)) + 4.0f / 29.0f;
        }
    }
}

void ColorFunctions::sRGBtoCIEXYZ(const float sRGB[3], float xyzOut[3])
{//https://en.wikipedia.org/wiki/SRGB
    float rgblin[3];
    for (int i = 0; i < 3; ++i) rgblin[i] = sRGBinvGamma(sRGB[i]);
    xyzOut[0] = 0.4124f * rgblin[0] + 0.3576f * rgblin[1] + 0.1805f * rgblin[2];
    xyzOut[1] = 0.2126f * rgblin[0] + 0.7152f * rgblin[1] + 0.0722f * rgblin[2];
    xyzOut[2] = 0.0193f * rgblin[0] + 0.1192f * rgblin[1] + 0.9505f * rgblin[2];
}

void ColorFunctions::CIEXYZtoCIELab(const float xyz[3], float LabOut[3])
{//https://en.wikipedia.org/wiki/CIELAB_color_space
    float fy = Labf(xyz[1] / 100.0f);
    LabOut[0] = 116.0f * fy;
    LabOut[1] = 500.0f * (Labf(xyz[0] / 95.0489f) - fy);//let's use D65, i guess?
    LabOut[2] = 200.0f * (fy - Labf(xyz[2] / 108.8840));
}

float ColorFunctions::perceptualDistance(const float srgb1[3], const float srgb2[3])
{
    float xyz1[3], xyz2[3], lab1[3], lab2[3];
    sRGBtoCIEXYZ(srgb1, xyz1);
    sRGBtoCIEXYZ(srgb2, xyz2);
    CIEXYZtoCIELab(xyz1, lab1);
    CIEXYZtoCIELab(xyz2, lab2);
    float accum = 0.0f;
    for (int i = 0; i < 3; ++i)
    {
        float diff = lab1[i] - lab2[i];
        accum += diff * diff;
    }
    return sqrt(accum);
}
