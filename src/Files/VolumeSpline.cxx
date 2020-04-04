/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include "CaretOMP.h"
#include "CubicSpline.h"
#include "MathFunctions.h"
#include "VolumeSpline.h"

#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;
using namespace caret;

VolumeSpline::VolumeSpline()
{
    m_ignoredNonNumeric = false;
    m_dims[0] = 0;
    m_dims[1] = 0;
    m_dims[2] = 0;
}

VolumeSpline::VolumeSpline(const float* frame, const int64_t framedims[3])
{
    m_ignoredNonNumeric = false;
    m_dims[0] = framedims[0];
    m_dims[1] = framedims[1];
    m_dims[2] = framedims[2];
    m_deconv = CaretArray<float>(m_dims[0] * m_dims[1] * m_dims[2]);
    CaretArray<float> scratchArray(m_dims[0] * max(m_dims[1], m_dims[2])), deconvScratch(max(m_dims[0], max(m_dims[1], m_dims[2])));//allocate as much as we will need, even if we don't use it all yet
    predeconvolve(deconvScratch, m_dims[0]);
    for (int k = 0; k < m_dims[2]; ++k)
    {
        int64_t index = m_dims[0] * m_dims[1] * k;
        int64_t index2 = 0;
        for (int j = 0; j < m_dims[1]; ++j)
        {
            for (int i = 0; i < m_dims[0]; ++i)
            {
                float tempf = frame[index];
                if (MathFunctions::isNumeric(tempf))
                {
                    scratchArray[index2] = tempf;
                } else {
                    scratchArray[index2] = 0.0f;
                    m_ignoredNonNumeric = true;
                }
                ++index;
                ++index2;
            }
        }
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int j = 0; j < m_dims[1]; ++j)
        {
            int64_t privIndex = j * m_dims[0];
            deconvolve(scratchArray.getArray() + privIndex, deconvScratch, m_dims[0]);
        }
        index = m_dims[0] * m_dims[1] * k;
        index2 = 0;
        for (int j = 0; j < m_dims[1]; ++j)
        {
            for (int i = 0; i < m_dims[0]; ++i)
            {
                m_deconv[index] = scratchArray[index2];
                ++index;
                ++index2;
            }
        }
    }
    predeconvolve(deconvScratch, m_dims[1]);
    for (int k = 0; k < m_dims[2]; ++k)
    {
        int64_t indexbase = k * m_dims[1] * m_dims[0];
        int64_t index = indexbase;
        for (int j = 0; j < m_dims[1]; ++j)
        {
            int64_t index2 = j;
            for (int i = 0; i < m_dims[0]; ++i)
            {
                scratchArray[index2] = m_deconv[index];//read linearly from frame while writing transposed should be slightly faster, because cache can stay dirty?
                index2 += m_dims[1];
                ++index;
            }
        }
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int i = 0; i < m_dims[0]; ++i)
        {
            int64_t privindex = i * m_dims[1];
            deconvolve(scratchArray.getArray() + privindex, deconvScratch, m_dims[1]);
        }
        index = 0;
        for (int i = 0; i < m_dims[0]; ++i)
        {
            int64_t index2 = i + indexbase;
            for (int j = 0; j < m_dims[1]; ++j)
            {
                m_deconv[index2] = scratchArray[index];//even though scratch should be cached now, if writing to frame collides, reading linearly should give better behavior
                ++index;
                index2 += m_dims[0];
            }
        }
    }
    predeconvolve(deconvScratch, m_dims[2]);
    for (int j = 0; j < m_dims[1]; ++j)//finally, use a similar strategy to do linear reads instead of widely interleaved reads for k-rows
    {
        int64_t indexbase = j * m_dims[0];
        int64_t increment = m_dims[1] * m_dims[0];
        for (int k = 0; k < m_dims[2]; ++k)
        {
            int64_t index = indexbase + k * increment;
            int64_t index2 = k;
            for (int i = 0; i < m_dims[0]; ++i)
            {
                scratchArray[index2] = m_deconv[index];
                index2 += m_dims[2];
                ++index;
            }
        }
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int i = 0; i < m_dims[0]; ++i)
        {
            int64_t privindex = i * m_dims[2];
            deconvolve(scratchArray.getArray() + privindex, deconvScratch, m_dims[2]);
        }
        for (int i = 0; i < m_dims[0]; ++i)
        {
            int64_t index = indexbase + i;
            int64_t index2 = i * m_dims[2];
            for (int k = 0; k < m_dims[2]; ++k)
            {
                m_deconv[index] = scratchArray[index2];
                index += increment;
                ++index2;
            }
        }
    }
}

namespace
{
    int64_t sampleParams(const float indexf, const int64_t dim, float& fpartOut, bool& lowEdgeOut, bool& highEdgeOut)
    {
        int64_t ret;
        float ipartf = 0.0f;
        fpartOut = modf(indexf, &ipartf);//even -0.99f should give -0 for ipart, and sample() will only allow -0.01f, so don't need to max(..., 0)
        ret = int64_t(ipartf);//extrapolate for beyond the voxel center without attempting invalid access
        if (ret > dim - 2)
        {
            fpartOut += ret - (dim - 2);
            ret = dim - 2;
        }
        lowEdgeOut = (ret < 1);//extrapolation is very limited, see the range test in sample()
        highEdgeOut = (ret >= dim - 2);//to extrapolate by half a voxel, should decide what the second-outside voxel's value should be (repeat or reflect)
        return ret;//otherwise, it will trend toward zero
    }
}

float VolumeSpline::sample(const float& ifloat, const float& jfloat, const float& kfloat)
{
    if (m_dims[0] < 2 || m_dims[1] < 2 || m_dims[2] < 2 ||
        ifloat < -0.01f || jfloat < -0.01f || kfloat < -0.01f ||
        ifloat > m_dims[0] - 0.99f || jfloat > m_dims[1] - 0.99f || kfloat > m_dims[2] - 0.99f) return 0.0f;//allow slight rounding error beyond voxel center
    const int64_t zstep = m_dims[0] * m_dims[1];
    float fparti, fpartj, fpartk;
    bool lowedgei, highedgei, lowedgej, highedgej, lowedgek, highedgek;
    int64_t lowi = sampleParams(ifloat, m_dims[0], fparti, lowedgei, highedgei);
    int64_t lowj = sampleParams(jfloat, m_dims[1], fpartj, lowedgej, highedgej);
    int64_t lowk = sampleParams(kfloat, m_dims[2], fpartk, lowedgek, highedgek);
    CubicSpline ispline = CubicSpline::bspline(fparti, lowedgei, highedgei);
    CubicSpline jspline = CubicSpline::bspline(fpartj, lowedgej, highedgej);
    CubicSpline kspline = CubicSpline::bspline(fpartk, lowedgek, highedgek);
    float jtemp[4], ktemp[4];//the weights of the splines are zero and never touched for off-the edge values, but zero the data anyway
    jtemp[0] = 0.0f;
    jtemp[3] = 0.0f;
    ktemp[0] = 0.0f;
    ktemp[3] = 0.0f;
    if (lowedgei || lowedgej || lowedgek || highedgei || highedgej || highedgek)
    {//there is an edge nearby, use the generic version with more conditionals
        int jstart = lowedgej ? 1 : 0;
        int kstart = lowedgek ? 1 : 0;
        int jend = highedgej ? 3 : 4;
        int kend = highedgek ? 3 : 4;
        for (int k = kstart; k < kend; ++k)
        {
            int64_t indexk = (k + lowk - 1) * zstep;
            for (int j = jstart; j < jend; ++j)
            {
                int64_t indexj = indexk + (j + lowj - 1) * m_dims[0] + lowi - 1;
                if (lowedgei)//have to do these tests for the simple reason that otherwise we might access off the end of the array in two of the 8 corners
                {
                    if (highedgei)
                    {
                        jtemp[j] = ispline.evalBothEdge(m_deconv[indexj + 1], m_deconv[indexj + 2]);
                    } else {
                        jtemp[j] = ispline.evalLowEdge(m_deconv[indexj + 1], m_deconv[indexj + 2], m_deconv[indexj + 3]);
                    }
                } else {
                    if (highedgei)
                    {
                        jtemp[j] = ispline.evalHighEdge(m_deconv[indexj], m_deconv[indexj + 1], m_deconv[indexj + 2]);
                    } else {
                        jtemp[j] = ispline.evaluate(m_deconv[indexj], m_deconv[indexj + 1], m_deconv[indexj + 2], m_deconv[indexj + 3]);
                    }
                }
            }
            ktemp[k] = jspline.evaluate(jtemp[0], jtemp[1], jtemp[2], jtemp[3]);
        }
        return kspline.evaluate(ktemp[0], ktemp[1], ktemp[2], ktemp[3]);
    } else {//we are clear of all edges, we can use fewer conditionals
        int64_t indexbase = lowi - 1 + m_dims[0] * (lowj - 1 + m_dims[1] * (lowk - 1));
        const float* basePtr = m_deconv.getArray() + indexbase;
        int64_t indexk = 0;
        for (int k = 0; k < 4; ++k)
        {
            int64_t indexj = indexk;
            for (int j = 0; j < 4; ++j)
            {
                jtemp[j] = ispline.evaluate(basePtr[indexj], basePtr[indexj + 1], basePtr[indexj + 2], basePtr[indexj + 3]);
                indexj += m_dims[0];
            }
            ktemp[k] = jspline.evaluate(jtemp[0], jtemp[1], jtemp[2], jtemp[3]);
            indexk += zstep;
        }
        return kspline.evaluate(ktemp[0], ktemp[1], ktemp[2], ktemp[3]);
    }
}

void VolumeSpline::deconvolve(float* data, const float* backsubs, const int64_t& length)
{
    if (length < 1) return;
    const float A = 1.0f / 6.0f, B = 2.0f / 3.0f;//the values of a bspline at center and +/-1
    //forward pass simulating gaussian elimination on matrix of bspline kernels and data
    data[0] /= B + A;//repeat final value for data outside the bounding box, to prevent bright edges
    for (int i = 1; i < length - 1; ++i)//the first and last rows are handled slightly differently
    {
        data[i] = (data[i] - A * data[i - 1]) / (B - A * backsubs[i - 1]);
    }
    data[length - 1] = (data[length - 1] - A * data[length - 2]) / (B + A - A * backsubs[length - 2]);//repeat final value for data outside the bounding box, to prevent bright edges
    //back substitution, making it gauss-jordan
    for (int i = length - 2; i >= 0; --i)//the last row doesn't need back-substitution
    {
        data[i] -= backsubs[i] * data[i + 1];
    }
}

void VolumeSpline::predeconvolve(float* backsubs, const int64_t& length)
{
    if (length < 1) return;
    const float A = 1.0f / 6.0f, B = 2.0f / 3.0f;
    backsubs[0] = A / (B + A);//repeat final value for data outside the bounding box, to prevent bright edges
    for (int i = 1; i < length; ++i)
    {
        backsubs[i] = A / (B - A * backsubs[i - 1]);
    }
}
