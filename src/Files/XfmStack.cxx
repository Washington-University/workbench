/*LICENSE_START*/
/*
 *  Copyright (C) 2025  Washington University School of Medicine
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

#include "XfmStack.h"

#include "CaretAssert.h"
#include "CaretException.h"
#include "CaretLogger.h"

#include <vector>

using namespace caret;
using namespace std;

namespace
{
    void sanityCheckAffine(const FloatMatrix& affine)
    {
        if (affine.getNumberOfRows() != 4 || affine.getNumberOfColumns() != 4)
        {
            CaretAssert(false);
            throw CaretException("affines must be 4x4 matrices");//what exception type?
        }
        if (affine[3][0] != 0.0f || affine[3][1] != 0.0f || affine[3][2] != 0.0f || affine[3][3] != 1.0f)
        {
            CaretAssert(false);
            throw CaretException("affine fourth row must be 0 0 0 1");
        }
    }
}

AffineXfm::AffineXfm(const FloatMatrix& xfm)
{
    sanityCheckAffine(xfm);
    m_xfm = xfm;
}

Vector3D AffineXfm::xfmPoint(const Vector3D& coordIn, const int64_t /*frame*/, bool* validCoord) const
{
    if (validCoord != NULL) *validCoord = true;
    return m_xfm.transformPoint(coordIn);
}

AffineSeriesXfm::AffineSeriesXfm(const vector<FloatMatrix>& xfmList)
{
    for (auto matrix : xfmList)
    {
        sanityCheckAffine(matrix);
    }
    m_xfmList = xfmList;
}

Vector3D AffineSeriesXfm::xfmPoint(const Vector3D& coordIn, const int64_t frame, bool* validCoord) const
{
    CaretAssertVectorIndex(m_xfmList, frame);
    if (validCoord != NULL) *validCoord = true;
    return m_xfmList[frame].transformPoint(coordIn);
}

WarpfieldXfm::WarpfieldXfm(const VolumeFile* warp)
{
    if ((warp->getDimensions())[3] != 3)
    {
        throw CaretException("warpfields must have 3 subvolumes");
    }
    m_warp = warp;
}

Vector3D WarpfieldXfm::xfmPoint(const Vector3D& coordIn, const int64_t /*frame*/, bool* validCoord) const
{
    Vector3D offset;
    bool validDisplacement = false;
    offset[0] = m_warp->interpolateValue(coordIn, VolumeFile::TRILINEAR, &validDisplacement, 0);
    if (validDisplacement)
    {
        offset[1] = m_warp->interpolateValue(coordIn, VolumeFile::TRILINEAR, NULL, 1);
        offset[2] = m_warp->interpolateValue(coordIn, VolumeFile::TRILINEAR, NULL, 2);
    }
    if (validCoord != NULL) *validCoord = validDisplacement;
    return offset + coordIn;
}

void XfmStack::push_back(CaretPointer<const XfmBase> nextXfm)
{
    m_xfmStack.push_back(nextXfm);
}

Vector3D XfmStack::xfmPoint(const Vector3D& coordIn, const int64_t frame, bool* validCoord) const
{
    Vector3D ret = coordIn;
    bool thisValid = true, retValid = true;
    for (auto& xfm : m_xfmStack)
    {
        ret = xfm->xfmPoint(ret, frame, &thisValid);
        if (!thisValid)
        {
            retValid = false; //don't break, consistency is better than a tiny optimization when bad things happen
        }
    }
    if (validCoord != NULL)
    {
        *validCoord = retValid;
    } else {
        if (!retValid && !m_haveWarned) //if the calling code doesn't check the validity, warn once per instance
        {
            CaretLogWarning("at least one transformed point leaves the bounds of a warpfield");
            m_haveWarned = true;
        }
    }
    return ret;
}
