
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

#define __SELECTION_ITEM_VOXEL_DECLARE__
#include "SelectionItemVoxel.h"
#undef __SELECTION_ITEM_VOXEL_DECLARE__

#include "CaretAssert.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class SelectionItemVoxel
 * \brief Selected voxel.
 *
 * Information about an selected voxel.
 */


/**
 * Constructor.
 */
SelectionItemVoxel::SelectionItemVoxel()
: SelectionItem(SelectionItemDataTypeEnum::VOXEL)
{
    /*
     * Note: reset() is virtual so cannot call from constructor.
     */
    resetPrivate();
}

/**
 * Constructor for child classes.
 *
 * @param itemDataType
 *     The selection item data type for child class.
 */
SelectionItemVoxel::SelectionItemVoxel(const SelectionItemDataTypeEnum::Enum itemDataType)
: SelectionItem(itemDataType)
{
    /*
     * Note: reset() is virtual so cannot call from constructor.
     */
    resetPrivate();
}


/**
 * Destructor.
 */
SelectionItemVoxel::~SelectionItemVoxel()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemVoxel::SelectionItemVoxel(const SelectionItemVoxel& obj)
: SelectionItem(obj)
{
    copyHelperSelectionItemVoxel(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to m_.
 * @return
 *    Reference to m_ object.
 */
SelectionItemVoxel&
SelectionItemVoxel::operator=(const SelectionItemVoxel& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        copyHelperSelectionItemVoxel(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of m_ type.
 * @param ff
 *    Object that is copied.
 */
void
SelectionItemVoxel::copyHelperSelectionItemVoxel(const SelectionItemVoxel& idItem)
{
    m_volumeFile  = idItem.m_volumeFile;
    m_voxelIJK    = idItem.m_voxelIJK;
    m_voxelXYZ    = idItem.m_voxelXYZ;
    m_plane       = idItem.m_plane;
    m_voxelSizeMillimeters = idItem.m_voxelSizeMillimeters;
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemVoxel::reset()
{
    SelectionItem::reset();
    
    resetPrivate();
}

/**
 * Reset this selection item.
 */
void
SelectionItemVoxel::resetPrivate()
{
    m_volumeFile = NULL;
    m_voxelIJK[0] = -1;
    m_voxelIJK[1] = -1;
    m_voxelIJK[2] = -1;
    m_voxelXYZ.set(0.0, 0.0, 0.0);
    m_plane       = Plane();
    m_voxelSizeMillimeters = 1.0;
}


/**
 * @return The volume file.
 */
const VolumeMappableInterface* 
SelectionItemVoxel::getVolumeFile() const
{
    return m_volumeFile;
}

/**
 * Get the voxel indices.
 * @param voxelIJK
 *    Output containing voxel indices.
 */
VoxelIJK
SelectionItemVoxel::getVoxelIJK() const
{
    return m_voxelIJK;
}

/**
 * @return The voxel XYZ coordinate
 */
Vector3D
SelectionItemVoxel::getVoxelXYZ() const
{
    return m_voxelXYZ;
}

/**
 * @return The plane
 */
Plane
SelectionItemVoxel::getPlane() const
{
    return m_plane;
}

/**
 * @return The voxel size in millimeters (greatest spacing)
 */
float
SelectionItemVoxel::getVoxelSizeMillimeters() const
{
    return m_voxelSizeMillimeters;
}

/**
 * Set the volume file.
 *
 * @param brain
 *    Brain containing the volume.
 * @param volumeFile
 *    New value for volume file.
 * @param voxelIJK
 *    New value for voxel indices.
 * @param voxelXYZ
 *    XYZ coordinate of voxel
 * @param sliceNormalVector
 *    Normal vector of slice
 * @param screenDepth
 *    The screen depth.
 */
void 
SelectionItemVoxel::setVoxelIdentification(Brain* brain,
                                           VolumeMappableInterface* volumeFile,
                                           const VoxelIJK& voxelIJK,
                                           const Vector3D& voxelXYZ,
                                           const Plane& plane,
                                           const double screenDepth)
{
    setBrain(brain);
    m_volumeFile  = volumeFile;
    m_voxelSizeMillimeters = m_volumeFile->getMaximumVoxelSpacing();
//    if (m_volumeFile != NULL) {
//        float sp, sc, sa;
//        m_volumeFile->getVoxelSpacingPCA(sp, sc, sa);
//        m_voxelSizeMillimeters = std::max(sp, std::max(sc, sa));
//    }
//    else {
//        m_voxelSizeMillimeters = 1.0;
//    }
    m_voxelIJK    = voxelIJK;
    m_voxelXYZ    = voxelXYZ;
    setModelXYZ(voxelXYZ);
    m_plane       = plane;
    setScreenDepth(screenDepth);
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemVoxel::isValid() const
{
    return (m_volumeFile != NULL);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SelectionItemVoxel::toString() const
{
    AString text = SelectionItem::toString();
    AString name = "INVALID";
    if (m_volumeFile != NULL) {
        CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(m_volumeFile);
        if (cmdf != NULL) {
            name = cmdf->getFileNameNoPath();
        }
    }
    
    text += ("Volume: " + name);
    text += ("Voxel: "
             + AString::fromNumbers(m_voxelIJK.m_ijk, 3) + "\n");
    
    return text;
}




