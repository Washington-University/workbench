
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
#include "VolumeMappableInterface.h"

using namespace caret;


    
/**
 * \class caret::VolumeSurfaceOutlineModelCacheKey 
 * \brief Key for a cached volume surface outline model
 * \ingroup Brain
 */

/**
 * Constructor.
 */
VolumeSurfaceOutlineModelCacheKey::VolumeSurfaceOutlineModelCacheKey(const VolumeMappableInterface* underlayVolume,
                                                                     const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                                     const float sliceCoordinate)
: CaretObject(),
m_slicePlane(slicePlane),
m_sliceCoordinateScaled(static_cast<int32_t>(std::round(sliceCoordinate * 10.0)))
{
    CaretAssert(underlayVolume);
    if (underlayVolume != NULL) {
        float voxelSizesMM[3];
        underlayVolume->getVoxelSpacing(voxelSizesMM[0],
                                        voxelSizesMM[1],
                                        voxelSizesMM[2]);
        
        float voxelSize(0.0);
        switch (slicePlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                voxelSize = voxelSizesMM[2];
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                voxelSize = voxelSizesMM[2];
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                voxelSize = voxelSizesMM[1];
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                voxelSize = voxelSizesMM[0];
                break;
        }
        
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
            const float scaleFactor = 10.0 * (1.0 / voxelSize);
            m_sliceCoordinateScaled = static_cast<int32_t>(std::round(sliceCoordinate * scaleFactor));
        }
    }
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
    m_slicePlane = obj.m_slicePlane;
    m_sliceCoordinateScaled = obj.m_sliceCoordinateScaled;
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
VolumeSurfaceOutlineModelCacheKey::operator==(const VolumeSurfaceOutlineModelCacheKey& obj) const
{
    if (this == &obj) {
        return true;    
    }

    /* perform equality testing HERE and return true if equal ! */
    if ((m_slicePlane == obj.m_slicePlane)
        && (m_sliceCoordinateScaled == obj.m_sliceCoordinateScaled)) {
        return true;
    }
    
    return false;    
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
    
    if (static_cast<int32_t>(m_slicePlane) < static_cast<int32_t>(obj.m_slicePlane)) {
        return true;
    }
    else if (static_cast<int32_t>(m_slicePlane) > static_cast<int32_t>(obj.m_slicePlane)) {
        return false;
    }
    
    if (m_sliceCoordinateScaled < obj.m_sliceCoordinateScaled) {
        return true;
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
    QString str("Plane="
                + VolumeSliceViewPlaneEnum::toName(m_slicePlane)
                + ", "
                + "ScaledCoordinate="
                + QString::number(m_sliceCoordinateScaled));
    return str;
}

