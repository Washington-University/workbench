#ifndef __XFM_STACK_H__
#define __XFM_STACK_H__

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

#include "CaretPointer.h"
#include "FloatMatrix.h"
#include "Vector3D.h"
#include "VolumeFile.h"

namespace caret {

    struct XfmBase
    {
        virtual Vector3D xfmPoint(const Vector3D& coordIn, const int64_t frame, bool* validCoord = NULL) const = 0;
        virtual ~XfmBase() {};
    };

    class AffineXfm : public XfmBase
    {
        FloatMatrix m_xfm;
    public:
        AffineXfm(const FloatMatrix& xfm);
        Vector3D xfmPoint(const Vector3D& coordIn, const int64_t frame, bool* validCoord = NULL) const;
    };

    class AffineSeriesXfm : public XfmBase
    {
        std::vector<FloatMatrix> m_xfmList;
    public:
        AffineSeriesXfm(const std::vector<FloatMatrix>& xfmList);
        Vector3D xfmPoint(const Vector3D& coordIn, const int64_t frame, bool* validCoord = NULL) const;
    };

    class WarpfieldXfm : public XfmBase
    {
        const VolumeFile* m_warp;//NOTE: the warp is by reference
    public:
        WarpfieldXfm(const VolumeFile* warp);
        Vector3D xfmPoint(const Vector3D& coordIn, const int64_t frame, bool* validCoord = NULL) const;
    };

    class XfmStack : public XfmBase //allow stacking of transform stacks, because why not
    {
        std::vector<CaretPointer<const XfmBase> > m_xfmStack;
        mutable bool m_haveWarned { false };
    public:
        Vector3D xfmPoint(const Vector3D& coordIn, const int64_t frame, bool* validCoord = NULL) const;
        void push_back(CaretPointer<const XfmBase> nextXfm);
    };

}

#endif //__XFM_STACK_H__
