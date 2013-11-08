/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include "VolumeSpace.h"

#include "CaretAssert.h"
#include "CaretException.h"
#include "FloatMatrix.h"

#include <cmath>

using namespace std;
using namespace caret;

VolumeSpace::VolumeSpace()
{
    m_dims[0] = 0;
    m_dims[1] = 0;
    m_dims[2] = 0;
    m_sform = FloatMatrix::identity(4).getMatrix();
    computeInverse();
}

VolumeSpace::VolumeSpace(const int64_t dims[3], const vector<vector<float> >& sform)
{
    setSpace(dims, sform);
}

VolumeSpace::VolumeSpace(const int64_t dims[3], const float sform[16])
{
    setSpace(dims, sform);
}

void VolumeSpace::setSpace(const int64_t dims[3], const vector<vector<float> >& sform)
{
    if (sform.size() < 2 || sform.size() > 4)
    {
        CaretAssert(false);
        throw CaretException("VolumeSpace initialized with wrong size sform");
    }
    for (int i = 0; i < (int)sform.size(); ++i)
    {
        if (sform[i].size() != 4)
        {
            CaretAssert(false);
            throw CaretException("VolumeSpace initialized with wrong size sform");
        }
    }
    m_dims[0] = dims[0];
    m_dims[1] = dims[1];
    m_dims[2] = dims[2];
    m_sform = sform;
    m_sform.resize(4);//make sure its 4x4
    m_sform[3].resize(4);
    m_sform[3][0] = 0.0f;//force the fourth row to be correct
    m_sform[3][1] = 0.0f;
    m_sform[3][2] = 0.0f;
    m_sform[3][3] = 1.0f;
    computeInverse();
}

void VolumeSpace::setSpace(const int64_t dims[3], const float sform[12])
{
    m_sform = FloatMatrix::identity(4).getMatrix();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            m_sform[i][j] = sform[i * 4 + j];
        }
    }
    m_dims[0] = dims[0];
    m_dims[1] = dims[1];
    m_dims[2] = dims[2];
    computeInverse();
}

void VolumeSpace::computeInverse()
{
    m_inverse = FloatMatrix(m_sform).inverse().getMatrix();
}

void VolumeSpace::spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float& indexOut1, float& indexOut2, float& indexOut3) const
{
    indexOut1 = coordIn1 * m_inverse[0][0] + coordIn2 * m_inverse[0][1] + coordIn3 * m_inverse[0][2] + m_inverse[0][3];
    indexOut2 = coordIn1 * m_inverse[1][0] + coordIn2 * m_inverse[1][1] + coordIn3 * m_inverse[1][2] + m_inverse[1][3];
    indexOut3 = coordIn1 * m_inverse[2][0] + coordIn2 * m_inverse[2][1] + coordIn3 * m_inverse[2][2] + m_inverse[2][3];
}

void VolumeSpace::enclosingVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3) const
{
    float tempInd1, tempInd2, tempInd3;
    spaceToIndex(coordIn1, coordIn2, coordIn3, tempInd1, tempInd2, tempInd3);
    indexOut1 = (int64_t)floor(0.5f + tempInd1);
    indexOut2 = (int64_t)floor(0.5f + tempInd2);
    indexOut3 = (int64_t)floor(0.5f + tempInd3);
}

bool VolumeSpace::matchesVolumeSpace(const VolumeSpace& right) const
{
    for (int i = 0; i < 3; ++i)
    {
        if (m_dims[i] != right.m_dims[i])
        {
            return false;
        }
    }
    const float TOLER_RATIO = 0.999f;//ratio a spacing element can mismatch by
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float leftelem = m_sform[i][j];
            float rightelem = right.m_sform[i][j];
            if ((leftelem != rightelem) && (leftelem == 0.0f || rightelem == 0.0f || (leftelem / rightelem < TOLER_RATIO || rightelem / leftelem < TOLER_RATIO)))
            {
                return false;
            }
        }
    }
    return true;
}

void VolumeSpace::getSpacingVectors(Vector3D& iStep, Vector3D& jStep, Vector3D& kStep, Vector3D& origin) const
{
    FloatMatrix(m_sform).getAffineVectors(iStep, jStep, kStep, origin);
}

void VolumeSpace::getOrientAndSpacingForPlumb(OrientTypes* orientOut, float* spacingOut, float* originOut) const
{
    CaretAssert(isPlumb());
    if (!isPlumb())
    {
        throw CaretException("orientation and spacing asked for on non-plumb volume space");//this will fail MISERABLY on non-plumb volumes, so throw otherwise
    }
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (m_sform[i][j] != 0.0f)
            {
                spacingOut[j] = m_sform[i][j];
                originOut[j] = m_sform[i][3];
                bool negative;
                if (m_sform[i][j] > 0.0f)
                {
                    negative = true;
                } else {
                    negative = false;
                }
                switch (i)
                {
                case 0:
                    //left/right
                    orientOut[j] = (negative ? RIGHT_TO_LEFT : LEFT_TO_RIGHT);
                    break;
                case 1:
                    //forward/back
                    orientOut[j] = (negative ? ANTERIOR_TO_POSTERIOR : POSTERIOR_TO_ANTERIOR);
                    break;
                case 2:
                    //up/down
                    orientOut[j] = (negative ? SUPERIOR_TO_INFERIOR : INFERIOR_TO_SUPERIOR);
                    break;
                default:
                    //will never get called
                    break;
                };
            }
        }
    }
}

void VolumeSpace::getOrientation(VolumeSpace::OrientTypes orientOut[3]) const
{
    Vector3D ivec, jvec, kvec, origin;
    getSpacingVectors(ivec, jvec, kvec, origin);
    int next = 1, bestarray[3] = {0, 0, 0};
    float bestVal = -1.0f;//make sure at least the first test trips true, if there is a zero spacing vector it will default to report LPI
    for (int first = 0; first < 3; ++first)//brute force search for best fit - only 6 to try
    {
        int third = 3 - first - next;
        float testVal = abs(ivec[first] * jvec[next] * kvec[third]);
        if (testVal > bestVal)
        {
            bestVal = testVal;
            bestarray[0] = first;
            bestarray[1] = next;
        }
        testVal = abs(ivec[first] * jvec[third] * kvec[next]);
        if (testVal > bestVal)
        {
            bestVal = testVal;
            bestarray[0] = first;
            bestarray[1] = third;
        }
        next = 0;
    }
    bestarray[2] = 3 - bestarray[0] - bestarray[1];
    Vector3D spaceHats[3];//to translate into enums without casting
    spaceHats[0] = ivec;
    spaceHats[1] = jvec;
    spaceHats[2] = kvec;
    for (int i = 0; i < 3; ++i)
    {
        bool neg = (spaceHats[i][bestarray[i]] < 0.0f);
        switch (bestarray[i])
        {
            case 0:
                if (neg)
                {
                    orientOut[i] = RIGHT_TO_LEFT;
                } else {
                    orientOut[i] = LEFT_TO_RIGHT;
                }
                break;
            case 1:
                if (neg)
                {
                    orientOut[i] = ANTERIOR_TO_POSTERIOR;
                } else {
                    orientOut[i] = POSTERIOR_TO_ANTERIOR;
                }
                break;
            case 2:
                if (neg)
                {
                    orientOut[i] = SUPERIOR_TO_INFERIOR;
                } else {
                    orientOut[i] = INFERIOR_TO_SUPERIOR;
                }
                break;
            default:
                CaretAssert(0);
        }
    }
}

bool VolumeSpace::isPlumb() const
{
    char axisUsed = 0;
    char indexUsed = 0;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (m_sform[i][j] != 0.0f)
            {
                if (axisUsed & (1<<i))
                {
                    return false;
                }
                if (indexUsed & (1<<j))
                {
                    return false;
                }
                axisUsed |= (1<<i);
                indexUsed |= (1<<j);
            }
        }
    }
    return true;
}
