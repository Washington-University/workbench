
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

#define __CONNECTIVITY_DATA_LOADED_DECLARE__
#include "ConnectivityDataLoaded.h"
#undef __CONNECTIVITY_DATA_LOADED_DECLARE__

#include "CaretAssert.h"
#include "SceneClassAssistant.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "ScenePrimitiveArray.h"
#include "SurfaceFile.h"

using namespace caret;



/**
 * \class caret::ConnectivityDataLoaded
 * \brief Maintains information on loaded brainordinate data.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
ConnectivityDataLoaded::ConnectivityDataLoaded()
: CaretObject()
{
    reset();
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add("m_rowIndex",
                          &m_rowIndex);
    m_sceneAssistant->add("m_columnIndex",
                          &m_columnIndex);
    m_sceneAssistant->add("m_surfaceNumberOfNodes",
                          &m_surfaceNumberOfNodes);
    m_sceneAssistant->add<StructureEnum, StructureEnum::Enum>("m_surfaceStructure",
                                                              &m_surfaceStructure);
    m_sceneAssistant->addArray("m_volumeDimensionsIJK",
                               m_volumeDimensionsIJK,
                               3,
                               -1);
    m_sceneAssistant->addArray("m_volumeXYZ",
                               m_volumeXYZ,
                               3,
                               0.0);
    reset();
}

/**
 * Destructor.
 */
ConnectivityDataLoaded::~ConnectivityDataLoaded()
{
    reset();
    
    delete m_sceneAssistant;
}

/**
 * Reset the data.
 */
void
ConnectivityDataLoaded::reset()
{
    m_mode = MODE_NONE;
    
    m_rowIndex    = -1;
    m_columnIndex = -1;
    
    m_surfaceNodeIndices.clear();
    m_surfaceNumberOfNodes = 0;
    m_surfaceStructure = StructureEnum::INVALID;
    
    m_volumeDimensionsIJK[0] = -1;
    m_volumeDimensionsIJK[1] = -1;
    m_volumeDimensionsIJK[2] = -1;
    
    m_volumeXYZ[0] = 0.0;
    m_volumeXYZ[1] = 0.0;
    m_volumeXYZ[2] = 0.0;
    
    m_voxelIndices.clear();
}

/**
 * @return The mode.
 */
ConnectivityDataLoaded::Mode
ConnectivityDataLoaded::getMode() const
{
    return m_mode;
}

/**
 * Get the row that were loaded.
 *
 * @param rowIndex
 *    Row that was loaded (may be -1 if none).
 * @param columnIndex
 *    Column that was loaded (may be -1 if none).
 */
void
ConnectivityDataLoaded::getRowColumnLoading(int64_t& rowIndex,
                                            int64_t& columnIndex) const
{
    rowIndex = m_rowIndex;
    columnIndex = m_columnIndex;
}

/**
 * Set the row that were loaded.
 *
 * @param rowIndex
 *    Row that was loaded (may be -1 if none).
 * @param columnIndex
 *    Column that was loaded (may be -1 if none)
 */
void
ConnectivityDataLoaded::setRowColumnLoading(const int64_t rowIndex,
                                            const int64_t columnIndex)
{
    reset();
    
    if (rowIndex >= 0) {
        m_mode = MODE_ROW;
        m_rowIndex = rowIndex;
    }
    else if (columnIndex >= 0) {
        m_mode = MODE_COLUMN;
        m_columnIndex = columnIndex;
    }
    else {
        CaretAssertMessage(0, "One or row index or column index should be negative indicating that dimension was not loaded.");
    }
}

/**
 * Get the surface loading information (MODE_SURFACE_NODE)
 * One of rowIndex or columnIndex will be negative.
 *
 * @param structure
 *    The surface structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param surfaceNodeIndex
 *    Index of the surface node.
 * @param rowIndex
 *    Index of row corresponding to the surface node (may be -1 if none).
 * @param columnIndex
 *    Index of row corresponding to the surface node (may be -1 if none).
 */
void
ConnectivityDataLoaded::getSurfaceNodeLoading(StructureEnum::Enum& structure,
                                              int32_t& surfaceNumberOfNodes,
                                              int32_t& surfaceNodeIndex,
                                              int64_t& rowIndex,
                                              int64_t& columnIndex) const
{
    structure = m_surfaceStructure;
    surfaceNumberOfNodes = m_surfaceNumberOfNodes;
    if (m_surfaceNodeIndices.empty() == false) {
        surfaceNodeIndex = m_surfaceNodeIndices[0];
    }
    else {
        surfaceNodeIndex = -1;
    }
    rowIndex = m_rowIndex;
    columnIndex = m_columnIndex;
}

/**
 * Set the surface loading information (MODE_SURFACE_NODE)
 * One of rowIndex or columnIndex must be negative.
 *
 * @param structure
 *    The surface structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param surfaceNodeIndex
 *    Index of the surface node.
 * @param rowIndex
 *    Index of row corresponding to the surface node (may be -1 if none).
 * @param columnIndex
 *    Index of column corresponding to the surface node (may be -1 if none).
 */
void
ConnectivityDataLoaded::setSurfaceNodeLoading(const StructureEnum::Enum structure,
                                              const int32_t surfaceNumberOfNodes,
                                              const int32_t surfaceNodeIndex,
                                              const int64_t rowIndex,
                                              const int64_t columnIndex)
{
    reset();
    
    m_mode = MODE_SURFACE_NODE;
    m_surfaceStructure = structure;
    m_surfaceNumberOfNodes = surfaceNumberOfNodes;
    m_surfaceNodeIndices.push_back(surfaceNodeIndex);
    m_rowIndex = rowIndex;
    m_columnIndex = columnIndex;
    
    if ((rowIndex >= 0)
        && (columnIndex >= 0)) {
        CaretAssertMessage(0, "One of row or column index must be negative.");
    }
}

/**
 * Get the surface average node loading information (MODE_SURFACE_NODE_AVERAGE)
 *
 * @param structure
 *    The surface structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param surfaceNodeIndices
 *    Indices of the surface nodes.
 */
void
ConnectivityDataLoaded::getSurfaceAverageNodeLoading(StructureEnum::Enum& structure,
                                                     int32_t& surfaceNumberOfNodes,
                                                     std::vector<int32_t>& surfaceNodeIndices) const
{
    structure = m_surfaceStructure;
    surfaceNumberOfNodes = m_surfaceNumberOfNodes;
    surfaceNodeIndices = m_surfaceNodeIndices;
}

/**
 * Set the surface average node loading information (MODE_SURFACE_NODE_AVERAGE)
 *
 * @param structure
 *    The surface structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param surfaceNodeIndices
 *    Indices of the surface nodes.
 */
void
ConnectivityDataLoaded::setSurfaceAverageNodeLoading(const StructureEnum::Enum structure,
                                                     const int32_t surfaceNumberOfNodes,
                                                     const std::vector<int32_t>& surfaceNodeIndices)
{
    reset();
    
    m_mode = MODE_SURFACE_NODE_AVERAGE;
    m_surfaceStructure = structure;
    m_surfaceNumberOfNodes = surfaceNumberOfNodes;
    m_surfaceNodeIndices = surfaceNodeIndices;
    m_rowIndex = -1;
    m_columnIndex = -1;
}

/**
 * Get the volume loading XYZ coordinate (MODE_VOXEL_XYZ).
 *
 * @param volumeXYZ
 *    Coordinate of location.
 * @param rowIndex
 *    Index of row corresponding to the voxel (may be -1 if none).
 * @param columnIndex
 *    Index of column corresponding to the voxel (may be -1 if none).
 */
void
ConnectivityDataLoaded::getVolumeXYZLoading(float volumeXYZ[3],
                                            int64_t& rowIndex,
                                            int64_t& columnIndex) const
{
    volumeXYZ[0] = m_volumeXYZ[0];
    volumeXYZ[1] = m_volumeXYZ[1];
    volumeXYZ[2] = m_volumeXYZ[2];
    rowIndex = m_rowIndex;
    columnIndex = m_columnIndex;
}

/**
 * Set the volume loading XYZ coordinate (MODE_VOXEL_XYZ).
 *
 * @param volumeXYZ
 *    Coordinate of location.
 * @param rowIndex
 *    Index of row corresponding to the voxel(may be -1 if none).
 * @param columnIndex
 *    Index of column corresponding to the voxel (may be -1 if none).
 */
void ConnectivityDataLoaded::setVolumeXYZLoading(const float volumeXYZ[3],
                                                 const int64_t rowIndex,
                                                 const int64_t columnIndex)
{
    reset();
    
    m_mode = MODE_VOXEL_XYZ;
    m_volumeXYZ[0] = volumeXYZ[0];
    m_volumeXYZ[1] = volumeXYZ[1];
    m_volumeXYZ[2] = volumeXYZ[2];
    m_rowIndex = rowIndex;
    m_columnIndex = columnIndex;
}

/**
 * Get the voxel average loading voxel IJK indices (MODE_VOXEL_IJK_AVERAGE)
 *
 * @param voxelIndicesIJK
 *    Indices of the voxels.
 */
void
ConnectivityDataLoaded::getVolumeAverageVoxelLoading(int64_t volumeDimensionsIJK[3],
                                                     std::vector<VoxelIJK>& voxelIndicesIJK) const
{
    volumeDimensionsIJK[0] = m_volumeDimensionsIJK[0];
    volumeDimensionsIJK[1] = m_volumeDimensionsIJK[1];
    volumeDimensionsIJK[2] = m_volumeDimensionsIJK[2];
    voxelIndicesIJK = m_voxelIndices;
}

/**
 * Set the voxel average loading voxel IJK indices (MODE_VOXEL_IJK_AVERAGE)
 *
 * @param voxelIndicesIJK
 *    Indices of the voxels.
 */
void
ConnectivityDataLoaded::setVolumeAverageVoxelLoading(const int64_t volumeDimensionsIJK[3],
                                                     const std::vector<VoxelIJK>& voxelIndicesIJK)
{
    reset();
    
    m_volumeDimensionsIJK[0] = volumeDimensionsIJK[0];
    m_volumeDimensionsIJK[1] = volumeDimensionsIJK[1];
    m_volumeDimensionsIJK[2] = volumeDimensionsIJK[2];
    m_mode = MODE_VOXEL_IJK_AVERAGE;
    m_voxelIndices = voxelIndicesIJK;
    m_rowIndex = -1;
    m_columnIndex = -1;
}



/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
ConnectivityDataLoaded::restoreFromScene(const SceneAttributes* sceneAttributes,
                                         const SceneClass* sceneClass)
{
    reset();
    
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    m_mode = MODE_NONE;
    const AString modeName = sceneClass->getStringValue("m_mode");
    if (modeName == "MODE_NONE") {
        m_mode = MODE_NONE;
    }
    else if (modeName == "MODE_ROW") {
        m_mode = MODE_ROW;
    }
    else if (modeName == "MODE_COLUMN") {
        m_mode = MODE_COLUMN;
    }
    else if (modeName == "MODE_SURFACE_NODE_AVERAGE") {
        m_mode = MODE_SURFACE_NODE_AVERAGE;
    }
    else if (modeName == "MODE_SURFACE_NODE") {
        m_mode = MODE_SURFACE_NODE;
    }
    else if (modeName == "MODE_VOXEL_XYZ") {
        m_mode = MODE_VOXEL_XYZ;
    }
    else if (modeName == "MODE_VOXEL_IJK_AVERAGE") {
        m_mode = MODE_VOXEL_IJK_AVERAGE;
    }
    else {
        sceneAttributes->addToErrorMessage("Unrecognized mode=" + modeName);
        return;
    }
    
    const ScenePrimitiveArray* surfaceNodeIndicesArray = sceneClass->getPrimitiveArray("m_surfaceNodeIndices");
    if (surfaceNodeIndicesArray != NULL) {
        const int32_t numNodeIndices = surfaceNodeIndicesArray->getNumberOfArrayElements();
        m_surfaceNodeIndices.reserve(numNodeIndices);
        for (int32_t i = 0; i < numNodeIndices; i++) {
            m_surfaceNodeIndices.push_back(surfaceNodeIndicesArray->integerValue(i));
        }
    }
        
    const ScenePrimitiveArray* voxelIndicesArray = sceneClass->getPrimitiveArray("m_voxelIndices");
    if (voxelIndicesArray != NULL) {
        const int64_t numIndices = voxelIndicesArray->getNumberOfArrayElements();
        const int64_t numVoxelIndices = numIndices / 3;
        
        for (int64_t i = 0; i < numVoxelIndices; i++) {
            const int64_t i3 = i * 3;
            VoxelIJK voxelIJK(voxelIndicesArray->integerValue(i3),
                              voxelIndicesArray->integerValue(i3 + 1),
                              voxelIndicesArray->integerValue(i3 + 2));
            m_voxelIndices.push_back(voxelIJK);
        }
    }
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
ConnectivityDataLoaded::saveToScene(const SceneAttributes* sceneAttributes,
                                    const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ConnectivityDataLoaded",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    AString modeName = "MODE_NONE";
    switch (m_mode) {
        case MODE_NONE:
            modeName = "MODE_NONE";
            break;
        case MODE_ROW:
            modeName = "MODE_ROW";
            break;
        case MODE_COLUMN:
            modeName = "MODE_COLUMN";
            break;
        case MODE_SURFACE_NODE:
            modeName = "MODE_SURFACE_NODE";
            break;
        case MODE_SURFACE_NODE_AVERAGE:
            modeName = "MODE_SURFACE_NODE_AVERAGE";
            break;
        case MODE_VOXEL_XYZ:
            modeName = "MODE_VOXEL_XYZ";
            break;
        case MODE_VOXEL_IJK_AVERAGE:
            modeName = "MODE_VOXEL_IJK_AVERAGE";
            break;
    }
    sceneClass->addString("m_mode",
                          modeName);
    
    if (m_surfaceNodeIndices.empty() == false) {
        sceneClass->addIntegerArray("m_surfaceNodeIndices",
                                    &m_surfaceNodeIndices[0],
                                    m_surfaceNodeIndices.size());
    }
    
    if (m_voxelIndices.empty() == false) {
        const int32_t numVoxels = m_voxelIndices.size();
        std::vector<int32_t> indices;
        for (int32_t i = 0; i < numVoxels; i++) {
            indices.push_back(m_voxelIndices[i].m_ijk[0]);
            indices.push_back(m_voxelIndices[i].m_ijk[1]);
            indices.push_back(m_voxelIndices[i].m_ijk[2]);
        }
        
        sceneClass->addIntegerArray("m_voxelIndices",
                                    &indices[0],
                                    indices.size());
    }
    
    return sceneClass;
}
