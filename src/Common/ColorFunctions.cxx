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

    struct Vector2D
    {
        float m_vec[2];
        Vector2D() { m_vec[0] = 0.0f; m_vec[1] = 0.0f; }
        Vector2D(const float* rhs)
        {
            m_vec[0] = rhs[0];
            m_vec[1] = rhs[1];
        }
        Vector2D(const float x, const float y)
        {
            m_vec[0] = x;
            m_vec[1] = y;
        }
        float length() const
        {
            return sqrt(m_vec[0] * m_vec[0] + m_vec[1] * m_vec[1]);
        }
        float dot(const Vector2D& rhs) const
        {
            return m_vec[0] * rhs.m_vec[0] + m_vec[1] * rhs.m_vec[1];
        }
        Vector2D operator*(const float& rhs) const
        {
            Vector2D ret = *this;
            ret.m_vec[0] *= rhs;
            ret.m_vec[1] *= rhs;
            return ret;
        }
        Vector2D operator/(const float& rhs) const
        {
            Vector2D ret = *this;
            ret.m_vec[0] /= rhs;
            ret.m_vec[1] /= rhs;
            return ret;
        }
        Vector2D operator+(const Vector2D& rhs) const
        {
            Vector2D ret = *this;
            ret.m_vec[0] += rhs.m_vec[0];
            ret.m_vec[1] += rhs.m_vec[1];
            return ret;
        }
        Vector2D operator-(const Vector2D& rhs) const
        {
            Vector2D ret = *this;
            ret.m_vec[0] -= rhs.m_vec[0];
            ret.m_vec[1] -= rhs.m_vec[1];
            return ret;
        }
        Vector2D normal() const
        {
            return (*this) / length();
        }
        Vector2D perpRot() const
        {
            return Vector2D(m_vec[1], -m_vec[0]);
        }
    };

    //w3c uses CIE Lu'v' (NOT u*v*) to approximate atypical color vision ambiguity lines, so use that plus geometry
    //and assume Lab space of the modified colors is still near-linear for atypical
    //https://www.w3.org/Graphics/atypical-color-response
    //protanope lines look wrong, and it doesn't cover change in lightness, find a different approach

    //does NOT do anomalous red darkness in protanopia
    void confusionTransform(const float tritanLuv[3], float deficientLuv[3], const float confPoint[2], const float resolvStart[2], const float resolvEnd[2])
    {
        deficientLuv[0] = tritanLuv[0];
        Vector2D tritanuv(tritanLuv[1], tritanLuv[2]);
        Vector2D toconf(confPoint[0] - tritanLuv[1], confPoint[1] - tritanLuv[2]);
        Vector2D tostart(resolvStart[0] - tritanLuv[1], resolvStart[1] - tritanLuv[2]);
        Vector2D resolvdir(resolvEnd[0] - resolvStart[0], resolvEnd[1] - resolvStart[1]);
        Vector2D resolvPerpHat = resolvdir.perpRot().normal();
        float distClosest = tostart.dot(resolvPerpHat);
        float perpDistConf = toconf.dot(resolvPerpHat);
        Vector2D result = tritanuv + toconf * distClosest / perpDistConf;
        deficientLuv[1] = result.m_vec[0];
        deficientLuv[2] = result.m_vec[1];
    }

    //for now, leave color deficient transforms private, in case we want to change approaches
    void deuteranopeXYZtransform(const float tritanXYZ[3], float deuterXYZOut[3])
    {//https://www.w3.org/Graphics/atypical-color-response
        float tritanLuv[3], deuterLuv[3];//u'v', not u*v*
        ColorFunctions::CIEXYZtoCIELuvPrime(tritanXYZ, tritanLuv);
        float confusionuv[2] = {-4.75f, 1.31f};
        float ambiguvstart[2] = {0.26f, 0.02f};//uv line to move all ambiguous colors to, eyeballed
        float ambiguvend[2] = {0.25f, 0.56f};
        confusionTransform(tritanLuv, deuterLuv, confusionuv, ambiguvstart, ambiguvend);
        ColorFunctions::CIELuvPrimetoCIEXYZ(deuterLuv, deuterXYZOut);
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
    float fy = Labf(xyz[1] / 1.0f);//our Yn = 1, not 100
    LabOut[0] = 116.0f * fy - 16.0f;
    LabOut[1] = 500.0f * (Labf(xyz[0] / 0.950489f) - fy);//let's use D65, i guess?
    LabOut[2] = 200.0f * (fy - Labf(xyz[2] / 1.088840f));//and output Lab in the range of hundreds to make it harder to confuse with sRGB and XYZ?
}

void ColorFunctions::CIEXYZtoCIELuvPrime(const float xyz[3], float LuvOut[3])
{//https://en.wikipedia.org/wiki/CIELUV
    if (xyz[1] <= (6.0f / 29.0f) * (6.0f / 29.0f) * (6.0f / 29.0f))
    {
        LuvOut[0] = (29.0f / 3.0f) * (29.0f / 3.0f) * (29.0f / 3.0f) * xyz[1];//Yn = 1
    } else {
        LuvOut[0] = 116.0f * pow(xyz[1], 1.0f / 3.0f) - 16;
    }
    float uvdenom = 1.0f / (xyz[0] + 15 * xyz[1] + 3 * xyz[2]);
    LuvOut[1] = 4 * xyz[0] * uvdenom;
    LuvOut[2] = 9 * xyz[1] * uvdenom;//yes, Z is only in the denominator
}

void ColorFunctions::CIELuvPrimetoCIEXYZ (const float Luv[3], float xyzOut[3])
{
    if (Luv[0] <= 8)
    {
        xyzOut[1] = Luv[0] * (3.0f / 29.0f) * (3.0f / 29.0f) * (3.0f / 29.0f);//Yn = 1
    } else {
        xyzOut[1] = pow((Luv[0] + 16) / 116, 3);
    }
    xyzOut[0] = xyzOut[1] * 9 * Luv[1] / (4 * Luv[2]);
    xyzOut[2] = xyzOut[1] * (12 - 3 * Luv[1] - 20 * Luv[2]) / (4 * Luv[2]);
}

float ColorFunctions::perceptualDistanceSRGB(const float srgb1[3], const float srgb2[3])
{
    float xyz1[3], xyz2[3], lab1[3], lab2[3];
    sRGBtoCIEXYZ(srgb1, xyz1);
    sRGBtoCIEXYZ(srgb2, xyz2);
    CIEXYZtoCIELab(xyz1, lab1);
    CIEXYZtoCIELab(xyz2, lab2);
    float accum = 0.0f;//TODO: CIE DE 2000? https://en.wikipedia.org/wiki/CIEDE2000
    for (int i = 0; i < 3; ++i)
    {
        float diff = lab1[i] - lab2[i];
        accum += diff * diff;
    }
    return sqrt(accum);
}
