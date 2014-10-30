
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

#define __VOLUME_FILE_EDITOR_DELEGATE_DECLARE__
#include "VolumeFileEditorDelegate.h"
#undef __VOLUME_FILE_EDITOR_DELEGATE_DECLARE__

#include "CaretAssert.h"
#include "CaretPointer.h"
#include "CaretUndoStack.h"
#include "VolumeFile.h"
#include "VolumeMapUndoCommand.h"
#include "VoxelIJK.h"

using namespace caret;



/**
 * \class caret::VolumeFileEditorDelegate
 * \brief Delegate for performing editing operations on a volume file's voxels.
 * \ingroup Files
 *
 * Perform interactive editing operations in a GUI on a volume file 
 * including the ability to undo, redo, and reset the editing operations.
 */

/**
 * Constructor.
 */
VolumeFileEditorDelegate::VolumeFileEditorDelegate(VolumeFile* volumeFile)
: CaretObject(),
m_volumeFile(volumeFile)
{
    CaretAssert(volumeFile);

    m_volumeDimensions[0] = 0;
    m_volumeDimensions[1] = 0;
    m_volumeDimensions[2] = 0;
    
    updateIfVolumeFileChangedNumberOfMaps();
}

/**
 * Destructor.
 */
VolumeFileEditorDelegate::~VolumeFileEditorDelegate()
{
    clear();
}

/**
 * Clear the instance.
 */
void
VolumeFileEditorDelegate::clear()
{
    /*
     * The undo stacks are only created when needed so some
     * entries may be NULL
     */
    for (std::vector<CaretUndoStack*>::iterator mapIter = m_volumeMapUndoStacks.begin();
         mapIter != m_volumeMapUndoStacks.end();
         mapIter++) {
        CaretUndoStack* undoStack = *mapIter;
        delete undoStack;
    }
    
    m_volumeMapUndoStacks.clear();
    
    m_volumeDimensions[0] = 0;
    m_volumeDimensions[1] = 0;
    m_volumeDimensions[2] = 0;
    m_volumeMapEditingLocked.clear();
}

/**
 * Perform an editing operation.
 *
 * @param mapIndex
 *     Index of map (brick) within the volume that is being edited.
 * @param mode
 *     The editing mode.
 * @param slicePlane
 *     The selected slice plane.
 * @param voxelIJK
 *     Indices of voxel selected by the user.
 * @param brushSize
 *     Size of brush used by some operations.
 * @param voxelValueOn
 *     Value that is assigned by a "turn on" operation.
 * @param voxelValueOff
 *     Value that is assigned by a "turn off" operation.
 * @param errorMessageOut
 *     Will contain error information.
 * @return
 *     True if successful, else false and errorMessageOut will be set.
 */
bool
VolumeFileEditorDelegate::performEditingOperation(const int64_t mapIndex,
                                                  const VolumeEditingModeEnum::Enum mode,
                                                  const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                  const int64_t voxelIJK[3],
                                                  const int64_t brushSize[3],
                                                  const float voxelValueOn,
                                                  const float voxelValueOff,
                                                  AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    CaretAssert(m_volumeFile);
    int64_t dimTime = 0;
    int64_t dimNumMaps = 0;
    m_volumeFile->getDimensions(m_volumeDimensions[0],
                                m_volumeDimensions[1],
                                m_volumeDimensions[2],
                                dimTime,
                                dimNumMaps);
    
    if ((mapIndex < 0)
        || (mapIndex >= dimNumMaps)) {
        errorMessageOut = ("Invalid map index="
                           + AString::number(mapIndex)
                           + ", number of maps="
                           + AString::number(dimNumMaps));
        return false;
    }
    
    if (isLocked(mapIndex)) {
        errorMessageOut = "Volume is locked to prevent editing";
        return false;
    }
    
    
    int64_t iHalf = brushSize[0] / 2;
    int64_t jHalf = brushSize[1] / 2;
    int64_t kHalf = brushSize[2] / 2;
    
    int64_t ijkMin[3] = {
        voxelIJK[0] - iHalf,
        voxelIJK[1] - jHalf,
        voxelIJK[2] - kHalf
    };
    clampVoxelIndices(ijkMin);
    
    int64_t ijkMax[3] = {
        voxelIJK[0] + iHalf,
        voxelIJK[1] + jHalf,
        voxelIJK[2] + kHalf
    };
    clampVoxelIndices(ijkMax);
    
    bool result = false;
    
    switch (mode) {
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_ON:
            result = performTurnOnOrOff(VolumeEditingModeEnum::VOLUME_EDITING_MODE_ON,
                                        mapIndex,
                                   slicePlane,
                                   ijkMin,
                                   ijkMax,
                                   voxelValueOn,
                                        voxelValueOff,
                                   errorMessageOut);
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_OFF:
            result = performTurnOnOrOff(VolumeEditingModeEnum::VOLUME_EDITING_MODE_OFF,
                                        mapIndex,
                                        slicePlane,
                                        ijkMin,
                                        ijkMax,
                                        voxelValueOn,
                                        voxelValueOff,
                                        errorMessageOut);
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_DILATE:
            result = performDilateOrErode(VolumeEditingModeEnum::VOLUME_EDITING_MODE_DILATE,
                                   mapIndex,
                                   slicePlane,
                                          ijkMin,
                                          ijkMax,
                                          voxelValueOn,
                                          voxelValueOff,
                                   errorMessageOut);
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_ERODE:
            result = performDilateOrErode(VolumeEditingModeEnum::VOLUME_EDITING_MODE_ERODE,
                                          mapIndex,
                                          slicePlane,
                                          ijkMin,
                                          ijkMax,
                                          voxelValueOn,
                                          voxelValueOff,
                                          errorMessageOut);
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_2D:
            result = performFloodFill2D(mapIndex,
                                        slicePlane,
                                        voxelIJK,
                                        brushSize,
                                        voxelValueOn,
                                        voxelValueOff,
                                        errorMessageOut);
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_3D:
            result = performFloodFill3D(mapIndex,
                                        slicePlane,
                                        voxelIJK,
                                        brushSize,
                                        voxelValueOn,
                                        voxelValueOff,
                                        errorMessageOut);
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_2D:
            result = performRemoveConnected2D(mapIndex,
                                              slicePlane,
                                              voxelIJK,
                                              brushSize,
                                              voxelValueOn,
                                              voxelValueOff,
                                              errorMessageOut);
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_3D:
            result = performRemoveConnected3D(mapIndex,
                                              slicePlane,
                                              voxelIJK,
                                              brushSize,
                                              voxelValueOn,
                                              voxelValueOff,
                                              errorMessageOut);
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_RETAIN_CONNECTED_3D:
            result = performRetainConnected3D(mapIndex,
                                              slicePlane,
                                              voxelIJK,
                                              brushSize,
                                              voxelValueOn,
                                              voxelValueOff,
                                              errorMessageOut);
            break;
    }
    
    return result;
}

/**
 * @return Is the volume file locked (does not allow editing)?
 *
 * @param mapIndex
 *    Index of map that is tested for locked.
 */
bool
VolumeFileEditorDelegate::isLocked(const int64_t mapIndex) const
{
    CaretAssertVectorIndex(m_volumeMapEditingLocked, mapIndex);
    return m_volumeMapEditingLocked[mapIndex];
}

/**
 * Set the volume file's lock status (does not allow editing).
 * 
 * If the locked status transitions to 'locked',
 * then CLEAR THE UNDO STACK since by locking,
 * the user is satisfied with changes made to the volume.
 *
 *
 * @param mapIndex
 *    Index of map that has lock status set.
 * @param locked
 *     New locked status.
 */
void
VolumeFileEditorDelegate::setLocked(const int64_t mapIndex,
                                    const bool locked)
{
    CaretAssertVectorIndex(m_volumeMapEditingLocked, mapIndex);
    if (locked != m_volumeMapEditingLocked[mapIndex]) {
        m_volumeMapEditingLocked[mapIndex] = locked;
        
        if (m_volumeMapEditingLocked[mapIndex]) {
            CaretAssertVectorIndex(m_volumeMapUndoStacks, mapIndex);
            m_volumeMapUndoStacks[mapIndex]->clear();
        }
    }
}


/**
 * Adjust the voxel indices so that they are within the volume.
 *
 * @param ijk
 *     Voxel indices that are adjusted to be in the range
 *     0 to dimension minus one.
 */
void
VolumeFileEditorDelegate::clampVoxelIndices(int64_t ijk[3]) const
{
    for (int32_t i = 0; i < 3; i++) {
        ijk[i] = clampDimensionIndex(m_volumeDimensions[i], ijk[i]);
    }
}

/**
 * Clamp a voxel dimensions index (zero to dim-1)
 *
 * @param maxDim
 *    Maximum dimension value.
 * @param dimIndex
 *    Value that is clamped.
 */
int64_t
VolumeFileEditorDelegate::clampDimensionIndex(const int64_t maxDim,
                                         int64_t dimIndex) const
{
    if (dimIndex < 0) {
        dimIndex = 0;
    }
    else if (dimIndex >= maxDim) {
        dimIndex = maxDim - 1;
    }
    
    return dimIndex;
}

/**
 * Clamp voxel indices.
 *
 * @param i
 *    Index for dimension 0.
 * @param j
 *    Index for dimension 1.
 * @param k
 *    Index for dimension 2.
 */
void
VolumeFileEditorDelegate::clampVoxelIndices(int64_t& i, int64_t& j, int64_t& k) const
{
    i = clampDimensionIndex(m_volumeDimensions[0], i);
    j = clampDimensionIndex(m_volumeDimensions[1], j);
    k = clampDimensionIndex(m_volumeDimensions[2], k);
}


/**
 * Add to the modified undo stacks for the given map.
 *
 * @param mapIndex
 *     Index of the map that was modified.
 * @param modifiedVoxels
 *     Voxels that were modified.
 */
void
VolumeFileEditorDelegate::addToMapUndoStacks(const int32_t mapIndex,
                                             VolumeMapUndoCommand* modifiedVoxels)
{
    if (modifiedVoxels->count() <= 0) {
        delete modifiedVoxels;
        return;
    }
    
    CaretAssertVectorIndex(m_volumeMapUndoStacks, mapIndex);
    m_volumeMapUndoStacks[mapIndex]->push(modifiedVoxels);
}

/**
 * Update in case number of maps in volume file has changed.
 */
void
VolumeFileEditorDelegate::updateIfVolumeFileChangedNumberOfMaps()
{
    const int32_t oldNumMaps = static_cast<int32_t>(m_volumeMapUndoStacks.size());
    const int32_t numMaps = m_volumeFile->getNumberOfMaps();
    const int32_t numMapsToAdd = numMaps - oldNumMaps;
    
    if (numMapsToAdd > 0) {
        for (int32_t i = 0; i < numMapsToAdd; i++) {
            m_volumeMapUndoStacks.push_back(new CaretUndoStack());
            m_volumeMapEditingLocked.push_back(true);
        }
    }
    
    CaretAssert(static_cast<int32_t>(m_volumeMapUndoStacks.size()) == numMaps);
    CaretAssert(static_cast<int32_t>(m_volumeMapEditingLocked.size()) == numMaps);
}

/**
 * Undo the last voxel editing operation for the given map index.
 *
 * @param mapIndex
 *     Index of map that has last voxel operation 'undone'.
 */
void
VolumeFileEditorDelegate::undo(const int64_t mapIndex)
{
    CaretAssertVectorIndex(m_volumeMapUndoStacks, mapIndex);
    m_volumeMapUndoStacks[mapIndex]->undo();
}

/**
 * Reset all voxel editing since last lock.
 *
 * Note that when the lock status transitions to lock, 
 * the undo stack is cleared.
 *
 * @param mapIndex
 *     Index of map that has last voxel operation 'undone'.
 */
void
VolumeFileEditorDelegate::reset(const int64_t mapIndex)
{
    CaretAssertVectorIndex(m_volumeMapUndoStacks, mapIndex);
    m_volumeMapUndoStacks[mapIndex]->undoAll();
}


/**
 * Redo the last voxel editing operation for the given map index.
 *
 * @param mapIndex
 *     Index of map that has last voxel operation 'redone'.
 */
void
VolumeFileEditorDelegate::redo(const int64_t mapIndex)
{
    CaretAssertVectorIndex(m_volumeMapUndoStacks, mapIndex);
    m_volumeMapUndoStacks[mapIndex]->redo();
}


/**
 * Perform an editing operation that turns on voxels.
 *
 * @param mode
 *     The editing mode.
 * @param mapIndex
 *     Index of map (brick) within the volume that is being edited.
 * @param slicePlane
 *     The selected slice plane.
 * @param minIJK
 *     Inclusive minimum voxel indices that are affected by this operation.
 * @param maxIJK
 *     Inclusive maximum voxel indices that are affected by this operation.
 * @param voxelValueOn
 *     Value that is assigned by a "turn on" operation.
 * @param voxelValueOff
 *     Value that is assigned by a "turn off" operation.
 * @param errorMessageOut
 *     Will contain error information.
 * @return
 *     True if there was an error, else false.
 */
bool
VolumeFileEditorDelegate::performTurnOnOrOff(const VolumeEditingModeEnum::Enum mode,
                                             const int64_t mapIndex,
                                        const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                        const int64_t minIJK[3],
                                        const int64_t maxIJK[3],
                                             const float voxelValueOn,
                                             const float voxelValueOff,
                                        AString& errorMessageOut)
{
    float redoVoxelValue = 0.0;
    switch (mode){
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_OFF:
            redoVoxelValue = voxelValueOff;
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_ON:
            redoVoxelValue = voxelValueOn;
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_DILATE:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_ERODE:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_2D:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_3D:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_2D:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_3D:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_RETAIN_CONNECTED_3D:
            CaretAssert(0);
            errorMessageOut = "Program error in performTurnOnOrOff but mode not valid.";
            return false;
            break;
    }
    
    CaretPointer<VolumeMapUndoCommand> modifiedVoxels;
    modifiedVoxels.grabNew(new VolumeMapUndoCommand(m_volumeFile,
                                                 mapIndex));
    for (int64_t i = minIJK[0]; i <= maxIJK[0]; i++) {
        for (int64_t j = minIJK[1]; j <= maxIJK[1]; j++) {
            for (int64_t k = minIJK[2]; k <= maxIJK[2]; k++) {
                const int64_t ijk[3] = { i, j, k };
                modifiedVoxels->addVoxelRedoUndo(ijk,
                                                 redoVoxelValue,
                                                 m_volumeFile->getValue(ijk, mapIndex));
                m_volumeFile->setValue(redoVoxelValue,
                                       ijk,
                                       mapIndex);
            }
        }
    }

    addToMapUndoStacks(mapIndex,
                       modifiedVoxels.releasePointer());
    
    return true;
}

/**
 * Perform an editing operation that dilates voxels
 * connected to the selected voxel.
 *
 * @param mode
 *     The editing mode.
 * @param mapIndex
 *     Index of map (brick) within the volume that is being edited.
 * @param slicePlane
 *     The selected slice plane.
 * @param minIJK
 *     Inclusive minimum voxel indices that are affected by this operation.
 * @param maxIJK
 *     Inclusive maximum voxel indices that are affected by this operation.
 * @param voxelValueOn
 *     Value that is assigned by a "turn on" operation.
 * @param voxelValueOff
 *     Value that is assigned by a "turn off" operation.
 * @param errorMessageOut
 *     Will contain error information.
 * @return
 *     True if there was an error, else false.
 */
bool
VolumeFileEditorDelegate::performDilateOrErode(const VolumeEditingModeEnum::Enum mode,
                                        const int64_t mapIndex,
                                        const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                               const int64_t minIJK[3],
                                               const int64_t maxIJK[3],
                                               const float voxelValueOn,
                                               const float voxelValueOff,
                                        AString& errorMessageOut)
{
    bool dilateFlag = false;
    
    switch (mode){
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_DILATE:
            dilateFlag = true;
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_ERODE:
            dilateFlag = false;
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_2D:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_3D:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_OFF:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_ON:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_2D:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_3D:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_RETAIN_CONNECTED_3D:
            CaretAssert(0);
            errorMessageOut = "Program error in dilate/erode but mode not valid.";
            return false;
            break;
    }
    
    const int64_t STRUCTURE_ELEMENT_SIZE = 1;
    
    CaretPointer<VolumeMapUndoCommand> modifiedVoxels;
    modifiedVoxels.grabNew(new VolumeMapUndoCommand(m_volumeFile,
                                                    mapIndex));
    
    /*
     * Check each voxel in the desired region
     */
    for (int64_t i = minIJK[0]; i <= maxIJK[0]; i++) {
        for (int64_t j = minIJK[1]; j <= maxIJK[1]; j++) {
            for (int64_t k = minIJK[2]; k <= maxIJK[2]; k++) {
                
                const int64_t ijk[3] = { i, j, k };
                
                /*
                 * Get the value of the voxel
                 */
                float value = m_volumeFile->getValue(ijk, mapIndex);
                
                bool voxelMatches = false;
                
                /*
                 * If eroding, look for "NOT OFF" voxels
                 */
                if (! dilateFlag) {
                    if (value != voxelValueOff) {
                        voxelMatches = true;
                    }
                }
                
                /*
                 * If dilating, look for "OFF" voxels
                 */
                if (dilateFlag) {
                    if (value == voxelValueOff) {
                        voxelMatches = true;
                    }
                }
                
                /*
                 * Should we continue processing this voxel
                 */
                if (voxelMatches) {
                    /*
                     * Create Structuring Element based upon the axis
                     */
                    int64_t iMin = ijk[0];
                    int64_t iMax = ijk[0];
                    int64_t jMin = ijk[1];
                    int64_t jMax = ijk[1];
                    int64_t kMin = ijk[2];
                    int64_t kMax = ijk[2];
                    switch (slicePlane) {
                        case VolumeSliceViewPlaneEnum::ALL:
                            CaretAssert(0);
                            errorMessageOut = "Program Error: Cannot edit an ALL slice";
                            return false;
                            break;
                        case VolumeSliceViewPlaneEnum::AXIAL:
                            iMin -= STRUCTURE_ELEMENT_SIZE;
                            iMax += STRUCTURE_ELEMENT_SIZE;
                            jMin -= STRUCTURE_ELEMENT_SIZE;
                            jMax += STRUCTURE_ELEMENT_SIZE;
//                            if (brushSize[2] > 1) {
//                                kMin -= STRUCTURE_ELEMENT_SIZE;
//                                kMax += STRUCTURE_ELEMENT_SIZE;
//                            }
                            break;
                        case VolumeSliceViewPlaneEnum::CORONAL:
                            iMin -= STRUCTURE_ELEMENT_SIZE;
                            iMax += STRUCTURE_ELEMENT_SIZE;
//                            if (brushSize[1] > 1) {
//                                jMin -= STRUCTURE_ELEMENT_SIZE;
//                                jMax += STRUCTURE_ELEMENT_SIZE;
//                            }
                            kMin -= STRUCTURE_ELEMENT_SIZE;
                            kMax += STRUCTURE_ELEMENT_SIZE;
                            break;
                        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//                            if (brushSize[0] > 1) {
//                                iMin -= STRUCTURE_ELEMENT_SIZE;
//                                iMax += STRUCTURE_ELEMENT_SIZE;
//                            }
                            jMin -= STRUCTURE_ELEMENT_SIZE;
                            jMax += STRUCTURE_ELEMENT_SIZE;
                            kMin -= STRUCTURE_ELEMENT_SIZE;
                            kMax += STRUCTURE_ELEMENT_SIZE;
                            break;
                    }
                    
                    clampVoxelIndices(iMin, jMin, kMin);
                    clampVoxelIndices(iMax, jMax, kMax);
                    
                    /*
                     * Check all voxels "under" the structuring element
                     */
                    bool foundVoxelFlag = false;
                    for (int64_t ii = iMin; ii <= iMax; ii++) {
                        for (int64_t jj = jMin; jj <= jMax; jj++) {
                            for (int64_t kk = kMin; kk <= kMax; kk++) {
                                /*
                                 * Ignore the voxel under the center of the
                                 * structuring element
                                 */
                                if ((ii != i) || (jj != j) || (kk != k)) {
                                    /*
                                     * Make sure voxel is valid since structuring element
                                     * may exceed bounds of the volume
                                     */
                                    const int64_t iijjkk[3] = { ii, jj, kk };
                                    float value = m_volumeFile->getValue(iijjkk);
                                    
                                    /*
                                     * If dilating, look for voxels that are "NOT OFF"
                                     * under the structuring element
                                     */
                                    if (dilateFlag) {
                                        if (value != voxelValueOff) {
                                            foundVoxelFlag = true;
                                            break;
                                        }
                                    }
                                    
                                    /*
                                     * If eroding look for voxels that are "OFF"
                                     * under the structuring element
                                     */
                                    if ( ! dilateFlag) {
                                        if (value == voxelValueOff) {
                                            foundVoxelFlag = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (foundVoxelFlag) {
                                break;
                            }
                        }
                        if (foundVoxelFlag) {
                            break;
                        }
                    }
                    
                    if (foundVoxelFlag) {
                        /*
                         * For now, just note which voxels need to be set since
                         * we do not want to modify the volume until after all voxels
                         * under structuring element have been checked.
                         */
                        if (dilateFlag) {
                            modifiedVoxels->addVoxelRedoUndo(ijk,
                                                             voxelValueOn,
                                                             m_volumeFile->getValue(ijk,
                                                                                    mapIndex));
                        }
                        else {
                            modifiedVoxels->addVoxelRedoUndo(ijk,
                                                             voxelValueOff,
                                                             m_volumeFile->getValue(ijk,
                                                                                    mapIndex));
                        }
                    }
                }
            }
        }
    }
    
    /*
     * Calling 'redo' will apply the changes to the volume file.
     */
    modifiedVoxels->redo();
    
    addToMapUndoStacks(mapIndex,
                       modifiedVoxels.releasePointer());
    
    return true;
}

/**
 * Perform an editing operation that flood fills the region
 * containing the selected voxel in the selected slice.
 *
 * @param mapIndex
 *     Index of map (brick) within the volume that is being edited.
 * @param slicePlane
 *     The selected slice plane.
 * @param voxelIJK
 *     Indices of voxel selected by the user.
 * @param brushSize
 *     Size of brush used by some operations.
 * @param voxelValueOn
 *     Value that is assigned by a "turn on" operation.
 * @param voxelValueOff
 *     Value that is assigned by a "turn off" operation.
 * @param errorMessageOut
 *     Will contain error information.
 * @return
 *     True if there was an error, else false.
 */
bool
VolumeFileEditorDelegate::performFloodFill2D(const int64_t mapIndex,
                                             const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                             const int64_t voxelIJK[3],
                                             const int64_t brushSize[3],
                                             const float voxelValueOn,
                                             const float voxelValueOff,
                                             AString& errorMessageOut)
{
    return performFloodFillAndRemoveConnected(VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_2D,
                                              mapIndex,
                                              slicePlane,
                                              voxelIJK,
                                              brushSize,
                                              voxelValueOn,
                                              voxelValueOff,
                                              errorMessageOut);
}

/**
 * Perform an editing operation that flood fills the region
 * containing the selected voxel in all dimensions.
 *
 * @param mapIndex
 *     Index of map (brick) within the volume that is being edited.
 * @param slicePlane
 *     The selected slice plane.
 * @param voxelIJK
 *     Indices of voxel selected by the user.
 * @param brushSize
 *     Size of brush used by some operations.
 * @param voxelValueOn
 *     Value that is assigned by a "turn on" operation.
 * @param voxelValueOff
 *     Value that is assigned by a "turn off" operation.
 * @param errorMessageOut
 *     Will contain error information.
 * @return
 *     True if there was an error, else false.
 */
bool
VolumeFileEditorDelegate::performFloodFill3D(const int64_t mapIndex,
                                             const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                             const int64_t voxelIJK[3],
                                             const int64_t brushSize[3],
                                             const float voxelValueOn,
                                             const float voxelValueOff,
                                             AString& errorMessageOut)
{
    return performFloodFillAndRemoveConnected(VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_3D,
                                              mapIndex,
                                              slicePlane,
                                              voxelIJK,
                                              brushSize,
                                              voxelValueOn,
                                              voxelValueOff,
                                              errorMessageOut);
}

/**
 * Perform an editing operation that removes all voxels connected
 * to the selected voxel in the slice.
 *
 * @param mapIndex
 *     Index of map (brick) within the volume that is being edited.
 * @param slicePlane
 *     The selected slice plane.
 * @param voxelIJK
 *     Indices of voxel selected by the user.
 * @param brushSize
 *     Size of brush used by some operations.
 * @param voxelValueOn
 *     Value that is assigned by a "turn on" operation.
 * @param voxelValueOff
 *     Value that is assigned by a "turn off" operation.
 * @param errorMessageOut
 *     Will contain error information.
 * @return
 *     True if there was an error, else false.
 */
bool
VolumeFileEditorDelegate::performRemoveConnected2D(const int64_t mapIndex,
                                                   const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                   const int64_t voxelIJK[3],
                                                   const int64_t brushSize[3],
                                                   const float voxelValueOn,
                                                   const float voxelValueOff,
                                                   AString& errorMessageOut)
{
    return performFloodFillAndRemoveConnected(VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_2D,
                                              mapIndex,
                                              slicePlane,
                                              voxelIJK,
                                              brushSize,
                                              voxelValueOn,
                                              voxelValueOff,
                                              errorMessageOut);
}

/**
 * Perform an editing operation that removes all voxels connected
 * to the selected voxel in all dimensions.
 *
 * @param mapIndex
 *     Index of map (brick) within the volume that is being edited.
 * @param slicePlane
 *     The selected slice plane.
 * @param voxelIJK
 *     Indices of voxel selected by the user.
 * @param brushSize
 *     Size of brush used by some operations.
 * @param voxelValueOn
 *     Value that is assigned by a "turn on" operation.
 * @param voxelValueOff
 *     Value that is assigned by a "turn off" operation.
 * @param errorMessageOut
 *     Will contain error information.
 * @return
 *     True if there was an error, else false.
 */
bool
VolumeFileEditorDelegate::performRemoveConnected3D(const int64_t mapIndex,
                                                   const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                   const int64_t voxelIJK[3],
                                                   const int64_t brushSize[3],
                                                   const float voxelValueOn,
                                                   const float voxelValueOff,
                                                   AString& errorMessageOut)
{
    return performFloodFillAndRemoveConnected(VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_3D,
                                              mapIndex,
                                              slicePlane,
                                              voxelIJK,
                                              brushSize,
                                              voxelValueOn,
                                              voxelValueOff,
                                              errorMessageOut);
}

/**
 * Perform an editing operation that rmeoves all voxels that
 * are not connected to the selected voxel.
 *
 * @param mapIndex
 *     Index of map (brick) within the volume that is being edited.
 * @param slicePlane
 *     The selected slice plane.
 * @param voxelIJK
 *     Indices of voxel selected by the user.
 * @param brushSize
 *     Size of brush used by some operations.
 * @param voxelValueOn
 *     Value that is assigned by a "turn on" operation.
 * @param voxelValueOff
 *     Value that is assigned by a "turn off" operation.
 * @param errorMessageOut
 *     Will contain error information.
 * @return
 *     True if there was an error, else false.
 */
bool
VolumeFileEditorDelegate::performRetainConnected3D(const int64_t mapIndex,
                                                   const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                   const int64_t voxelIJK[3],
                                                   const int64_t brushSize[3],
                                                   const float voxelValueOn,
                                                   const float voxelValueOff,
                                                   AString& errorMessageOut)
{
    errorMessageOut = "Retain connected 3D not yet implemented.";
    return false;
}

/**
 * Perform an editing operation that rmeoves all voxels that
 * are not connected to the selected voxel.
 *
 * @param mode
 *     The editing mode.
 * @param mapIndex
 *     Index of map (brick) within the volume that is being edited.
 * @param slicePlane
 *     The selected slice plane.
 * @param voxelIJK
 *     Indices of voxel selected by the user.
 * @param brushSize
 *     Size of brush used by some operations.
 * @param voxelValueOn
 *     Value that is assigned by a "turn on" operation.
 * @param voxelValueOff
 *     Value that is assigned by a "turn off" operation.
 * @param errorMessageOut
 *     Will contain error information.
 * @return
 *     True if there was an error, else false.
 */
bool
VolumeFileEditorDelegate::performFloodFillAndRemoveConnected(const VolumeEditingModeEnum::Enum mode,
                                                             const int64_t mapIndex,
                                                             const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                             const int64_t voxelIJK[3],
                                                             const int64_t brushSize[3],
                                                             const float voxelValueOn,
                                                             const float voxelValueOff,
                                                             AString& errorMessageOut)
{
    bool fillingFlag = false;
    bool threeDimensionalFlag = false;
    switch (mode){
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_DILATE:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_ERODE:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_OFF:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_ON:
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_RETAIN_CONNECTED_3D:
            CaretAssert(0);
            errorMessageOut = "Program error in performFloodFillAndRemoveConnected but mode not valid.";
            return false;
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_2D:
            fillingFlag = true;
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_3D:
            fillingFlag = true;
            threeDimensionalFlag = true;
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_2D:
            break;
        case VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_3D:
            threeDimensionalFlag = true;
            break;
    }
    
    CaretPointer<VolumeMapUndoCommand> modifiedVoxels;
    modifiedVoxels.grabNew(new VolumeMapUndoCommand(m_volumeFile,
                                                    mapIndex));
    
    /*
     * Initialize to the staring voxel
     */
    std::stack<VoxelIJK> st;
    st.push(VoxelIJK(voxelIJK));
    
    /*
     * While there are voxels to process
     */
    while (st.empty() == false) {
        /*
         * Get the next voxel to process
         */
        const VoxelIJK v = st.top();
        st.pop();
        int64_t i = v.m_ijk[0];
        int64_t j = v.m_ijk[1];
        int64_t k = v.m_ijk[2];
        
        /*
         * If the voxel has valid indices
         */
        if ((i >= 0) && (i < m_volumeDimensions[0]) &&
            (j >= 0) && (j < m_volumeDimensions[1]) &&
            (k >= 0) && (k < m_volumeDimensions[2])) {
            const int64_t ijk[3] = { i, j, k };
            float currentValue = m_volumeFile->getValue(ijk, mapIndex);
            
            /*
             * See if voxel has proper value for operation
             */
            bool matchingVoxel = false;
            if (fillingFlag) {
                matchingVoxel = (currentValue == 0.0);
            }
            else {
                matchingVoxel = (currentValue != 0.0);
            }
            
            /*
             * If the voxel should be modified
             */
            if (matchingVoxel) {
                /*
                 * Update the voxels value
                 */
                modifiedVoxels->addVoxelRedoUndo(ijk,
                                                 voxelValueOff,
                                                 m_volumeFile->getValue(ijk, mapIndex));
                m_volumeFile->setValue(voxelValueOff,
                                       ijk,
                                       mapIndex);
                
                /*
                 * Determine neighboring voxels
                 */
                int64_t iDelta = 0;
                int64_t jDelta = 0;
                int64_t kDelta = 0;
                switch (slicePlane) {
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        if (threeDimensionalFlag) {
                            iDelta = 1;
                        }
                        else {
                            iDelta = 0;
                        }
                        jDelta = 1;
                        kDelta = 1;
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        iDelta = 1;
                        if (threeDimensionalFlag) {
                            jDelta = 1;
                        }
                        else {
                            jDelta = 0;
                        }
                        kDelta = 1;
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        iDelta = 1;
                        jDelta = 1;
                        if (threeDimensionalFlag) {
                            kDelta = 1;
                        }
                        else {
                            kDelta = 0;
                        }
                        break;
                    case VolumeSliceViewPlaneEnum::ALL:
                        break;
                }
                
                /*
                 * Add neighboring voxels for search
                 */
                if (iDelta != 0) {
                    st.push(VoxelIJK(i - iDelta, j, k));
                    st.push(VoxelIJK(i + iDelta, j, k));
                }
                if (jDelta != 0) {
                    st.push(VoxelIJK(i, j - jDelta, k));
                    st.push(VoxelIJK(i, j + jDelta, k));
                }
                if (kDelta != 0) {
                    st.push(VoxelIJK(i, j, k - kDelta));
                    st.push(VoxelIJK(i, j, k + kDelta));
                }
            }
        }
    }
    
    addToMapUndoStacks(mapIndex,
                       modifiedVoxels.releasePointer());
    
    return true;
}



