
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __VOLUME_SURFACE_OUTLINE_MODEL_CACHE_KEY_DECLARE__
#include "VolumeSurfaceOutlineModelCacheKey.h"
#undef __VOLUME_SURFACE_OUTLINE_MODEL_CACHE_KEY_DECLARE__

#include <cmath>

#include "CaretAssert.h"
#include "Plane.h"
#include "VolumeMappableInterface.h"

using namespace caret;


    
/**
 * \class caret::VolumeSurfaceOutlineModelCacheKey 
 * \brief Key for a cached volume surface outline model
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param underlayVolume
 *     The underlay volume
 * @param sliceViewPlane
 *     The orthogonal slice view plane
 * @param sliceCoordinate
 *     Coordinate of slice in orthogonal view plane
 */
VolumeSurfaceOutlineModelCacheKey::VolumeSurfaceOutlineModelCacheKey(const VolumeMappableInterface* underlayVolume,
                                                                     const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                     const float sliceCoordinate)
: CaretObject(),
m_mode(Mode::SLICE_VIEW_PLANE),
m_sliceViewPlane(sliceViewPlane)
{
    const float scaleFactor = computeScaleFactor(underlayVolume);
    m_sliceCoordinateScaled = static_cast<int64_t>(sliceCoordinate * scaleFactor);
}

/**
 * Constructor.
 *
 * @param histologySlice
 *     The histology slice
 * @param histologySliceIndex
 *     Index of histology slice
 */
VolumeSurfaceOutlineModelCacheKey::VolumeSurfaceOutlineModelCacheKey(const HistologySlice* histologySlice,
                                                                     const int32_t histologySliceIndex)
: CaretObject(),
m_mode(Mode::HISTOLOGY_SLICE),
m_histologySlice(histologySlice),
m_histologySliceIndex(histologySliceIndex)
{
    
}

/**
 * Constructor.
 *
 * @param underlayVolume
 *     The underlay volume
 * @param sliceViewPlane
 *     The orthogonal slice view plane
 * @param sliceCoordinate
 *     Coordinate of slice in orthogonal view plane
 */
VolumeSurfaceOutlineModelCacheKey::VolumeSurfaceOutlineModelCacheKey(const VolumeMappableInterface* underlayVolume,
                                                                     const Plane& plane)
: m_mode(Mode::PLANE_EQUATION)
{
    CaretAssert(underlayVolume);
    if (underlayVolume != NULL) {
        const float scaleFactor = computeScaleFactor(underlayVolume);
        double a, b, c, d;
        plane.getPlane(a, b, c, d);
        m_planeEquationScaled[0] = scaleFactor * a;
        m_planeEquationScaled[1] = scaleFactor * b;
        m_planeEquationScaled[2] = scaleFactor * c;
        m_planeEquationScaled[3] = scaleFactor * d;
    }
}

/**
 * Compute the scale that is used to scale float values into integers
 *
 * @param underlayVolume
 *     The underlay volume whose voxel sizes are used to determine the scale factor
 * @return The scale factor
 */
float
VolumeSurfaceOutlineModelCacheKey::computeScaleFactor(const VolumeMappableInterface* underlayVolume) const
{
    float scaleFactor(10.0);
    
    if (underlayVolume != NULL) {
        float voxelSizesMM[3];
        underlayVolume->getVoxelSpacing(voxelSizesMM[0],
                                        voxelSizesMM[1],
                                        voxelSizesMM[2]);
        const float voxelSize = std::min(std::min(voxelSizesMM[0], voxelSizesMM[1]),
                                         voxelSizesMM[2]);
        if (voxelSize > 0.0) {
            /*
             * The coordinate for the slice is stored as an integer since
             * since integer comparison is precise oppposed to a float comparison
             * that requires some sort of tolerance.
             *
             * (1.0 / voxelSize) is used so that the scale factor will
             * be larger for small voxels and prevent an outline from
             * being used by adjacent volume slices
             */
            scaleFactor = 10.0 * (1.0 / voxelSize);
        }
    }

    return scaleFactor;
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineModelCacheKey::~VolumeSurfaceOutlineModelCacheKey()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
VolumeSurfaceOutlineModelCacheKey::VolumeSurfaceOutlineModelCacheKey(const VolumeSurfaceOutlineModelCacheKey& obj)
: CaretObject(obj)
{
    this->copyHelperVolumeSurfaceOutlineModelCacheKey(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
VolumeSurfaceOutlineModelCacheKey&
VolumeSurfaceOutlineModelCacheKey::operator=(const VolumeSurfaceOutlineModelCacheKey& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperVolumeSurfaceOutlineModelCacheKey(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
VolumeSurfaceOutlineModelCacheKey::copyHelperVolumeSurfaceOutlineModelCacheKey(const VolumeSurfaceOutlineModelCacheKey& obj)
{
    m_mode = obj.m_mode;
    m_sliceViewPlane = obj.m_sliceViewPlane;
    m_sliceCoordinateScaled = obj.m_sliceCoordinateScaled;
    m_planeEquationScaled   = obj.m_planeEquationScaled;
    m_histologySlice        = obj.m_histologySlice;
    m_histologySliceIndex   = obj.m_histologySliceIndex;
}

/**
 * Less than operator.
 *
 * @param obj
 *    Instance compared to this for equality.
 * @return
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
VolumeSurfaceOutlineModelCacheKey::operator<(const VolumeSurfaceOutlineModelCacheKey& obj) const
{
    if (this == &obj) {
        return false;
    }
    
    if (m_mode == obj.m_mode) {
        switch (m_mode) {
            case Mode::HISTOLOGY_SLICE:
                if (m_histologySlice < obj.m_histologySlice) {
                    return true;
                }
                else if (m_histologySlice > obj.m_histologySlice) {
                    return false;
                }
                
                if (m_histologySliceIndex < obj.m_histologySliceIndex) {
                    return true;
                }
                break;
            case Mode::PLANE_EQUATION:
            {
                for (int32_t i = 0; i < static_cast<int32_t>(m_planeEquationScaled.size()); i++) {
                    if (m_planeEquationScaled[i] < obj.m_planeEquationScaled[i]) {
                        return true;
                    }
                    else if (m_planeEquationScaled[i] > obj.m_planeEquationScaled[i]) {
                        return false;
                    }
                }
            }
                break;
            case Mode::SLICE_VIEW_PLANE:
            {
                if (static_cast<int32_t>(m_sliceViewPlane) < static_cast<int32_t>(obj.m_sliceViewPlane)) {
                    return true;
                }
                else if (static_cast<int32_t>(m_sliceViewPlane) > static_cast<int32_t>(obj.m_sliceViewPlane)) {
                    return false;
                }
                
                if (m_sliceCoordinateScaled < obj.m_sliceCoordinateScaled) {
                    return true;
                }
            }
                break;
        }
    }
    else {
        if (static_cast<int32_t>(m_mode) < static_cast<int32_t>(obj.m_mode)) {
            return true;
        }
    }
    
    
    return false;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeSurfaceOutlineModelCacheKey::toString() const
{
    QString str;
    switch (m_mode) {
        case Mode::HISTOLOGY_SLICE:
            str = ("Histology Slice Ptr = "
                   + QString::number((long long)m_histologySlice)
                   + " Slice Index="
                   + QString::number(m_histologySliceIndex));
            break;
        case Mode::PLANE_EQUATION:
            str = ("Plane Equation = "
                   + AString::fromNumbers(&m_planeEquationScaled[0], m_planeEquationScaled.size(), ","));
            break;
        case Mode::SLICE_VIEW_PLANE:
            str = ("Slice View Plane="
                   + VolumeSliceViewPlaneEnum::toName(m_sliceViewPlane)
                   + ", "
                   + "ScaledCoordinate="
                   + QString::number(m_sliceCoordinateScaled));
            break;
    }
    
    return str;
}

