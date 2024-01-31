
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __VOXEL_COLOR_UPDATE_DECLARE__
#include "VoxelColorUpdate.h"
#undef __VOXEL_COLOR_UPDATE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::VoxelColorUpdate 
 * \brief Contains information for updating colors of voxels
 * \ingroup Common
 */

/**
 * Constructor.
 */
VoxelColorUpdate::VoxelColorUpdate()
: CaretObject()
{
    m_rgba.fill(0);
}

/**
 * Destructor.
 */
VoxelColorUpdate::~VoxelColorUpdate()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
VoxelColorUpdate::VoxelColorUpdate(const VoxelColorUpdate& obj)
: CaretObject(obj)
{
    this->copyHelperVoxelColorUpdate(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
VoxelColorUpdate&
VoxelColorUpdate::operator=(const VoxelColorUpdate& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperVoxelColorUpdate(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
VoxelColorUpdate::copyHelperVoxelColorUpdate(const VoxelColorUpdate& obj)
{
    m_mapIndex  = obj.m_mapIndex;
    m_rgba      = obj.m_rgba;
    m_rgbaValid = obj.m_rgbaValid;
    m_voxels    = obj.m_voxels;
}

/**
 * Add a voxel
 * @param voxelIJK
 *    The voxel indices
 */
void
VoxelColorUpdate::addVoxel(const VoxelIJK& voxelIJK)
{
    m_voxels.push_back(voxelIJK);
}

/**
 * Add a voxel
 * @param i
 *    Index 'i' of the voxel
 * @param j
 *    Index 'j' of the voxel
 * @param k
 *    Index 'k' of the voxel
 */
void
VoxelColorUpdate::addVoxel(const int64_t i,
                           const int64_t j,
                           const int64_t k)
{
    m_voxels.emplace_back(i, j, k);
}

/**
 * Add a voxels
 * @param voxelsIJK
 *    The voxel indices
 */
void
VoxelColorUpdate::addVoxels(const std::vector<VoxelIJK>& voxelsIJK)
{
    m_voxels.insert(m_voxels.end(),
                    voxelsIJK.begin(),
                    voxelsIJK.end());
}


/**
 * Clear the instance
 */
void
VoxelColorUpdate::clear()
{
    m_mapIndex = -1;
    m_rgba.fill(0);
    m_rgbaValid = false;
    m_voxels.clear();
}

/**
 * @return True if this instance is valid
 */
bool
VoxelColorUpdate::isValid() const
{
    if (m_rgbaValid
        && (m_mapIndex >= 0)
        && ( ! m_voxels.empty())) {
        return true;
    }
    return false;
}

/**
 * @return Index of the map
 */
int32_t
VoxelColorUpdate::getMapIndex() const
{
    return m_mapIndex;
}

/**
 * Set the map index
 * @param mapIndex
 *    Index of the map
 */
void
VoxelColorUpdate::setMapIndex(const int32_t mapIndex)
{
    m_mapIndex = mapIndex;
}

/**
 * @return the RGBA color components
 */
const std::array<uint8_t, 4>&
VoxelColorUpdate::getRGBA() const
{
    return m_rgba;
}

/**
 * @return The number of voxels
 */
int32_t
VoxelColorUpdate::getNumberOfVoxels() const
{
    return m_voxels.size();
}

/**
 * @return The voxel at the given index
 * @param index
 *    Index of the voxel
 */
const VoxelIJK&
VoxelColorUpdate::getVoxel(const int32_t index) const
{
    CaretAssertVectorIndex(m_voxels, index);
    return m_voxels[index];
}

/**
 * Set the RGBA color components.
 * @param rgba
 *    The color components.
 */
void
VoxelColorUpdate::setRGBA(const std::array<uint8_t, 4>& rgba)
{
    m_rgba = rgba;
    m_rgbaValid = true;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VoxelColorUpdate::toString() const
{
    return "VoxelColorUpdate";
}

