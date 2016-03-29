
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

#define __VOLUME_MAP_UNDO_COMMAND_DECLARE__
#include "VolumeMapUndoCommand.h"
#undef __VOLUME_MAP_UNDO_COMMAND_DECLARE__

#include "CaretAssert.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::VolumeMapUndoCommand 
 * \brief Command pattern for volume map modifications that undo and redo.
 * \ingroup Files
 */

/**
 * Constructor.
 */
VolumeMapUndoCommand::VolumeMapUndoCommand(VolumeFile* volumeFile,
                                           const int32_t mapIndex)
: CaretUndoCommand(),
m_volumeFile(volumeFile),
m_mapIndex(mapIndex)
{
    CaretAssert(volumeFile);
    CaretAssert((mapIndex >= 0) && (mapIndex < volumeFile->getNumberOfMaps()));
}

/**
 * Destructor.
 */
VolumeMapUndoCommand::~VolumeMapUndoCommand()
{
    for (std::vector<VoxelMemento*>::iterator iter = m_voxelMementos.begin();
         iter != m_voxelMementos.end();
         iter++) {
        delete *iter;
    }
    m_voxelMementos.clear();
}

/**
 * Operation that "redoes" the command.
 *
 * @param errorMessageOut
 *     Output containing error message.
 * @return
 *     True if the command executed successfully, else false.
 */
bool
VolumeMapUndoCommand::redo(AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    for (std::vector<VoxelMemento*>::iterator iter = m_voxelMementos.begin();
         iter != m_voxelMementos.end();
         iter++) {
        const VoxelMemento* voxelMod = *iter;
        m_volumeFile->setValue(voxelMod->m_redoValue,
                               voxelMod->m_ijk,
                               m_mapIndex);
    }
    
    return true;
}

/**
 * Operation that "undoes" the command.
 */
bool
VolumeMapUndoCommand::undo(AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    for (std::vector<VoxelMemento*>::iterator iter = m_voxelMementos.begin();
         iter != m_voxelMementos.end();
         iter++) {
        const VoxelMemento* voxelMod = *iter;
        m_volumeFile->setValue(voxelMod->m_undoValue,
                               voxelMod->m_ijk,
                               m_mapIndex);
    }
    
    return true;
}

/**
 * @return Number of modified voxels.
 */
int32_t
VolumeMapUndoCommand::count() const
{
    return m_voxelMementos.size();
}


/**
 * Add the redo and undo values for a voxel.
 * 
 * @param ijk
 *     The voxel's indices.
 * @param redoValue
 *     Value for redo operation.
 * @param undoValue
 *     Value for undo operation.
 */
void
VolumeMapUndoCommand::addVoxelRedoUndo(const int64_t ijk[3],
                                       const float redoValue,
                                       const float undoValue)
{
    m_voxelMementos.push_back(new VoxelMemento(ijk,
                                               redoValue,
                                               undoValue));
}

/**
 * Add the redo and undo values for a voxel.
 *
 * @param i
 *     The voxel's "i" index.
 * @param j
 *     The voxel's "j" index.
 * @param k
 *     The voxel's "k" index.
 * @param redoValue
 *     Value for redo operation.
 * @param undoValue
 *     Value for undo operation.
 */
void
VolumeMapUndoCommand::addVoxelRedoUndo(const int64_t i,
                                       const int64_t j,
                                       const int64_t k,
                                       const float redoValue,
                                       const float undoValue)
{
    const int64_t ijk[3] = { i, j, k };
    addVoxelRedoUndo(ijk, redoValue, undoValue);
}
/* ------------------------------------------------------------------ */
/**
 * Constructor.
 *
 * @param ijk
 *     The voxel's indices.
 * @param redoValue
 *     Value for redo operation.
 * @param undoValue
 *     Value for undo operation.
 */
VolumeMapUndoCommand::VoxelMemento::VoxelMemento(const int64_t ijk[3],
                                                 const float redoValue,
                                                 const float undoValue)
{
    m_ijk[0] = ijk[0];
    m_ijk[1] = ijk[1];
    m_ijk[2] = ijk[2];
    
    m_redoValue = redoValue;
    m_undoValue = undoValue;
}

VolumeMapUndoCommand::VoxelMemento::~VoxelMemento()
{
    
}



