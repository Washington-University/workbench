
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __IDENTIFICATION_ITEM_VOXEL_DECLARE__
#include "IdentificationItemVoxel.h"
#undef __IDENTIFICATION_ITEM_VOXEL_DECLARE__

#include "CaretAssert.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class IdentificationItemVoxel
 * \brief Identified voxel.
 *
 * Information about an identified voxel.
 */


/**
 * Constructor.
 */
IdentificationItemVoxel::IdentificationItemVoxel()
: IdentificationItem(IdentificationItemDataTypeEnum::VOXEL)
{
    this->volumeFiles.clear();
    this->voxelIJK.clear();
    this->depth.clear();
}

/**
 * Destructor.
 */
IdentificationItemVoxel::~IdentificationItemVoxel()
{
    
}

/**
 * Reset this selection item. 
 */
void 
IdentificationItemVoxel::reset()
{
    IdentificationItem::reset();
    this->volumeFiles.clear();
    this->voxelIJK.clear();
    this->depth.clear();
}

/**
 * @return  The number of identified voxels.
 */
int32_t 
IdentificationItemVoxel::getNumberOfIdentifiedVoxels() const
{
    return this->volumeFiles.size();
}

/**
 * Get the volume at the given index.
 * @param  indx
 *    index of volume.
 * @return
 *    Volume file at index.
 */
const VolumeFile* 
IdentificationItemVoxel::getVolumeFile(const int indx) const
{
    CaretAssertVectorIndex(this->volumeFiles, indx);
    return this->volumeFiles[indx];
}

/**
 * Get the voxel at the given index.
 * @param indx
 *    Index of voxel.
 * @param voxelIJK
 *    Output containing voxel indices.
 */
void 
IdentificationItemVoxel::getVoxelIJK(const int32_t indx,
                                     int64_t voxelIJK[3]) const
{
    const int32_t indx3 = indx * 3;
    CaretAssertVectorIndex(this->voxelIJK, indx3);
    voxelIJK[0] = this->voxelIJK[indx3];
    voxelIJK[1] = this->voxelIJK[indx3+1];
    voxelIJK[2] = this->voxelIJK[indx3+2];
}

/**
 * Get the depth of the identified voxel.
 * @param indx
 *    Index of voxel.
 * @return
 *    Depth of voxel.
 */
float 
IdentificationItemVoxel::getDepth(const int indx) const
{
    CaretAssertVectorIndex(this->depth, indx);
    return this->depth[indx];
}

/**
 * Add an identified voxel.
 * @param volumeFile
 *    Volume file on which identification took place.
 * @param voxelIJK
 *    Indices of the voxel.
 */
void 
IdentificationItemVoxel::addVoxel(VolumeFile* volumeFile, 
                                  const int64_t voxelIJK[3],
                                  const float depth)
{
    this->volumeFiles.push_back(volumeFile);
    this->voxelIJK.push_back(voxelIJK[0]);
    this->voxelIJK.push_back(voxelIJK[1]);
    this->voxelIJK.push_back(voxelIJK[2]);
    this->depth.push_back(depth);
}

/**
 * @return Is this identified item valid?
 */
bool 
IdentificationItemVoxel::isValid() const
{
    return (this->getNumberOfIdentifiedVoxels() > 0);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentificationItemVoxel::toString() const
{
    AString text = "IdentificationItemVoxel";
    
    const int32_t numVolumes = static_cast<int32_t>(this->volumeFiles.size());
    text += IdentificationItem::toString() + "\n";
    for (int32_t i = 0; i < numVolumes; i++) {
        text += "Volume: " + this->volumeFiles[i]->getFileNameNoPath() + "\n";
        const int32_t i3 = i * 3;
        text += ("Voxel: " 
                 + AString::number(this->voxelIJK[i3]) + ", "
                 + AString::number(this->voxelIJK[i3+1]) + ", "
                 + AString::number(this->voxelIJK[i3+2]) + "\n");
    }
    
    return text;
}




