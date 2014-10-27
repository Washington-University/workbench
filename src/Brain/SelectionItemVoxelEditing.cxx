
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __SELECTION_ITEM_VOXEL_EDITING_DECLARE__
#include "SelectionItemVoxelEditing.h"
#undef __SELECTION_ITEM_VOXEL_EDITING_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemVoxelEditing 
 * \brief Information about selected voxel used in volume editing.
 * \ingroup Brain
 *
 * Reports selection of any voxel, even those that are
 * not displayed.  This allows the user to select "off" voxels
 * so that they can be turned on when editing a volume.
 */

/**
 * Constructor.
 */
SelectionItemVoxelEditing::SelectionItemVoxelEditing()
: SelectionItemVoxel()
{
    m_volumeFileForEditing = NULL;
}

/**
 * Destructor.
 */
SelectionItemVoxelEditing::~SelectionItemVoxelEditing()
{
}

/**
 * @return Volume file that is being edited.
 */
const VolumeFile*
SelectionItemVoxelEditing::getVolumeFileForEditing() const
{
    return m_volumeFileForEditing;
}

/**
 * Set the volume file that is being edited.
 *
 * @param volumeFile
 *     Volume file that is being edited.
 */
void
SelectionItemVoxelEditing::setVolumeFileForEditing(VolumeFile* volumeFile)
{
    m_volumeFileForEditing = volumeFile;
}

/**
 * Reset this selection item.
 */
void
SelectionItemVoxelEditing::reset()
{
    SelectionItemVoxel::reset();
    m_volumeFileForEditing = NULL;
}


