
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
    this->volumeFile = NULL;
    this->voxelIJK[0] = -1;
    this->voxelIJK[1] = -1;
    this->voxelIJK[2] = -1;
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
    this->volumeFile = NULL;
    this->voxelIJK[0] = -1;
    this->voxelIJK[1] = -1;
    this->voxelIJK[2] = -1;
}

/**
 * @return The volume file.
 */
const VolumeFile* 
IdentificationItemVoxel::getVolumeFile() const
{
    return this->volumeFile;
}

/**
 * Get the voxel indices.
 * @param voxelIJK
 *    Output containing voxel indices.
 */
void 
IdentificationItemVoxel::getVoxelIJK(int64_t voxelIJK[3]) const
{
    voxelIJK[0] = this->voxelIJK[0];
    voxelIJK[1] = this->voxelIJK[1];
    voxelIJK[2] = this->voxelIJK[2];
}

/**
 * Set the volume file.
 * @param volumeFile
 *    New value for volume file.
 */
void 
IdentificationItemVoxel::setVolumeFile(VolumeFile* volumeFile)
{
    this->volumeFile = volumeFile;
}

/**
 * Set the voxel indices.
 * @param voxelIJK
 *    New value for voxel indices.
 */
void 
IdentificationItemVoxel::setVoxelIJK(const int64_t voxelIJK[3])
{
    this->voxelIJK[0] = voxelIJK[0];
    this->voxelIJK[1] = voxelIJK[1];
    this->voxelIJK[2] = voxelIJK[2];
}

/**
 * @return Is this identified item valid?
 */
bool 
IdentificationItemVoxel::isValid() const
{
    return (this->volumeFile != NULL);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentificationItemVoxel::toString() const
{
    AString text = "IdentificationItemVoxel\n";
    
    text += IdentificationItem::toString() + "\n";
    text += "Volume: " + this->volumeFile->getFileNameNoPath() + "\n";
    text += ("Voxel: " 
             + AString::number(this->voxelIJK[0]) + ", "
             + AString::number(this->voxelIJK[1]) + ", "
             + AString::number(this->voxelIJK[2]) + "\n");
    
    return text;
}




