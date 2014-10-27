
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

#define __VOLUME_FILE_MEMENTO_DECLARE__
#include "VolumeFileMemento.h"
#undef __VOLUME_FILE_MEMENTO_DECLARE__

#include "CaretAssert.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::VolumeFileMemento 
 * \brief Restores a volume file map to a previous - "undo"
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param volumeFile
 *     Volume file whose state is saved.
 * @param mapIndex
 *     Index of the map in the volume file.
 */
VolumeFileMemento::VolumeFileMemento(VolumeFile* volumeFile,
                                     const int32_t mapIndex)
: CaretObject(),
m_volumeFile(volumeFile),
m_mapIndex(mapIndex)
{
    CaretAssert(m_volumeFile);
    CaretAssert((m_mapIndex >= 0) && (m_mapIndex < m_volumeFile->getNumberOfMaps()));
}

/**
 * Destructor.
 */
VolumeFileMemento::~VolumeFileMemento()
{
    for (std::vector<VoxelModification*>::iterator iter = m_voxelModifications.begin();
         iter != m_voxelModifications.end();
         iter++) {
        delete *iter;
    }
    m_voxelModifications.clear();
}

/**
 * Add a voxel's previous state.
 *
 * @param ijk
 *     Indices of the voxel.
 * @param value
 *     Value of the voxel.
 */
void
VolumeFileMemento::addVoxel(const int64_t ijk[3],
                            const float value)
{
    m_voxelModifications.push_back(new VoxelModification(ijk,
                                                         value));
}

/**
 * Undo the modification that was performed on the volume file.
 */
void
VolumeFileMemento::undo()
{
    for (std::vector<VoxelModification*>::iterator iter = m_voxelModifications.begin();
         iter != m_voxelModifications.end();
         iter++) {
        const VoxelModification* voxelMod = *iter;
        m_volumeFile->setValue(voxelMod->m_value,
                               voxelMod->m_ijk,
                               m_mapIndex);
    }
}

/**
 * @return Is this emtpy (no voxels to restore)?
 */
bool
VolumeFileMemento::isEmpty() const
{
    return m_voxelModifications.empty();
}

/**
 * @return the number of voxel modifications.
 */
int64_t
VolumeFileMemento::getNumberOfModifiedVoxels() const
{
    return m_voxelModifications.size();
}

/**
 * Get the voxel modification at the given index.
 *
 * @param index
 *     Index of the voxel modification.
 * @return
 *     Voxel modification at the given index.
 */
const VolumeFileMemento::VoxelModification*
VolumeFileMemento::getModifiedVoxel(const int64_t index) const
{
    CaretAssertVectorIndex(m_voxelModifications, index);
    return m_voxelModifications[index];
}

/* ===================================================================== */
/**
 * Constructor for a modified voxel.
 *
 * @param ijk
 *     Indices of the voxel.
 * @param value
 *     Value of the voxel.
 */
VolumeFileMemento::VoxelModification::VoxelModification(const int64_t ijk[3],
                                                        const float value)
{
    m_ijk[0] = ijk[0];
    m_ijk[1] = ijk[1];
    m_ijk[2] = ijk[2];
    
    m_value = value;
}

VolumeFileMemento::VoxelModification::~VoxelModification()
{
    
}


