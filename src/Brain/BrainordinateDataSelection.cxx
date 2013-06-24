
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __BRAINORDINATE_DATA_SELECTION_DECLARE__
#include "BrainordinateDataSelection.h"
#undef __BRAINORDINATE_DATA_SELECTION_DECLARE__

#include "CaretAssert.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "ScenePrimitiveArray.h"
#include "SurfaceFile.h"

using namespace caret;



/**
 * \class caret::BrainordinateDataSelection
 * \brief Maintains information on loaded brainordinate data.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainordinateDataSelection::BrainordinateDataSelection()
: CaretObject()
{
    reset();
}

/**
 * Destructor.
 */
BrainordinateDataSelection::~BrainordinateDataSelection()
{
    reset();
}

/**
 * Reset the data.
 */
void
BrainordinateDataSelection::reset()
{
    m_mode = MODE_NONE;
    m_surfaceFileName = "";
    m_surfaceFileNodeIndices.clear();
    m_voxelXYZ[0] = 0.0;
    m_voxelXYZ[1] = 0.0;
    m_voxelXYZ[2] = 0.0;
    m_volumeAverageVoxelIndices.clear();
}

/**
 * @return The mode.
 */
BrainordinateDataSelection::Mode
BrainordinateDataSelection::getMode() const
{
    return m_mode;
}

/**
 * @return The name of the surface file.
 */
AString
BrainordinateDataSelection::getSurfaceFileName() const
{
    return m_surfaceFileName;
}

/**
 * @return The surface indices for use when mode is MODE_SURFACE_AVERAGE
 * or MODE_SURFACE_NODE.  For MODE_SURFACE_NODE there will be one node.
 */
std::vector<int32_t>
BrainordinateDataSelection::getSurfaceNodeIndices() const
{
    return m_surfaceFileNodeIndices;
}

/**
 * Get the voxel XYZ-coordinates for use when mode is MODE_VOXEL_XYZ
 * @param xyzOut
 *    Output into which coordinates are loaded.
 */
void
BrainordinateDataSelection::getVoxelXYZ(float xyzOut[3]) const
{
    xyzOut[0] = m_voxelXYZ[0];
    xyzOut[1] = m_voxelXYZ[1];
    xyzOut[2] = m_voxelXYZ[2];
}

/**
 * Setup for single node dense connectivity data.
 * @param surfaceFile
 *     Surface file on which data was selected.
 * @param nodeIndex
 *     Index of node on the surface.
 */
void
BrainordinateDataSelection::setSurfaceLoading(const SurfaceFile* surfaceFile,
                                              const int32_t nodeIndex)
{
    reset();
    m_mode = MODE_SURFACE_NODE;
    m_surfaceFileName      = surfaceFile->getFileNameNoPath();
    m_surfaceFileNodeIndices.push_back(nodeIndex);
}

/**
 * Setup for surface node averaging.
 * @param surfaceFile
 *     Surface file on which data was selected.
 * @param nodeIndices
 *     Indices of node on the surface.
 */
void
BrainordinateDataSelection::setSurfaceAverageLoading(const SurfaceFile* surfaceFile,
                                                     const std::vector<int32_t>& nodeIndices)
{
    reset();
    m_mode = MODE_SURFACE_AVERAGE;
    m_surfaceFileName        = surfaceFile->getFileNameNoPath();
    m_surfaceFileNodeIndices = nodeIndices;
}

/**
 * Setup for voxel loading at a coordinate.
 * @param xyz
 *     Coordinate at a voxel.
 */
void
BrainordinateDataSelection::setVolumeLoading(const float xyz[3])
{
    reset();
    m_mode = MODE_VOXEL_XYZ;
    m_voxelXYZ[0] = xyz[0];
    m_voxelXYZ[1] = xyz[1];
    m_voxelXYZ[2] = xyz[2];
}

/**
 * Set the voxel indices for voxel averaging.
 *
 * @param volumeDimensionIJK
 *    Dimension of the volume.
 * @param voxelIndices
 *    Indices of voxel in the average.
 */
void
BrainordinateDataSelection::setVolumeAverageLoading(const  int64_t volumeDimensionIJK[3],
                                                    const std::vector<VoxelIJK>& voxelIndices)
{
    reset();
    
    m_mode = MODE_VOXEL_AVERAGE;
    
    m_volumeAverageDimensionsIJK[0] = volumeDimensionIJK[0];
    m_volumeAverageDimensionsIJK[1] = volumeDimensionIJK[1];
    m_volumeAverageDimensionsIJK[2] = volumeDimensionIJK[2];
    
    m_volumeAverageVoxelIndices = voxelIndices;
}

/**
 * Get the indices for volume voxel averaging.
 *
 * @param volumeDimensionIJK
 *    Dimension of the volume.
 * @param voxelIndices
 *    Indices of voxel in the average.
 */
void
BrainordinateDataSelection::getVolumeAverageVoxelIndices(int64_t volumeDimensionIJK[3],
                                                         std::vector<VoxelIJK>& voxelIndices) const
{
    volumeDimensionIJK[0] = m_volumeAverageDimensionsIJK[0];
    volumeDimensionIJK[1] = m_volumeAverageDimensionsIJK[1];
    volumeDimensionIJK[2] = m_volumeAverageDimensionsIJK[2];
    voxelIndices = m_volumeAverageVoxelIndices;
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
BrainordinateDataSelection::restoreFromScene(const SceneAttributes* sceneAttributes,
                                             const SceneClass* sceneClass)
{
    reset();
    
    if (sceneClass == NULL) {
        return;
    }
    
    m_mode = MODE_NONE;
    const AString modeName = sceneClass->getStringValue("m_mode");
    if (modeName == "MODE_NONE") {
        m_mode = MODE_NONE;
    }
    else if (modeName == "MODE_SURFACE_AVERAGE") {
        m_mode = MODE_SURFACE_AVERAGE;
    }
    else if (modeName == "MODE_SURFACE_NODE") {
        m_mode = MODE_SURFACE_NODE;
    }
    else if (modeName == "MODE_VOXEL_XYZ") {
        m_mode = MODE_VOXEL_XYZ;
    }
    else if (modeName == "MODE_VOXEL_AVERAGE") {
        m_mode = MODE_VOXEL_AVERAGE;
    }
    else {
        sceneAttributes->addToErrorMessage("Unrecognized mode=" + modeName);
        return;
    }
    
    m_surfaceFileName      = sceneClass->getStringValue("m_surfaceFileName");
    
    m_surfaceFileNodeIndices.clear();
    const ScenePrimitiveArray* nodeIndicesArray = sceneClass->getPrimitiveArray("m_surfaceFileNodeIndices");
    if (nodeIndicesArray != NULL) {
        const int32_t numNodeIndices = nodeIndicesArray->getNumberOfArrayElements();
        m_surfaceFileNodeIndices.reserve(numNodeIndices);
        for (int32_t i = 0; i < numNodeIndices; i++) {
            m_surfaceFileNodeIndices.push_back(nodeIndicesArray->integerValue(i));
        }
    }
    
    sceneClass->getFloatArrayValue("m_voxelXYZ",
                                   m_voxelXYZ,
                                   3,
                                   0.0);
    
    sceneClass->getIntegerArrayValue("m_volumeAverageDimensionsIJK",
                                     m_volumeAverageDimensionsIJK,
                                     3,
                                     0);
    
    const ScenePrimitiveArray* voxelIndicesArray = sceneClass->getPrimitiveArray("m_volumeAverageVoxelIndices");
    if (voxelIndicesArray != NULL) {
        const int64_t numIndices = voxelIndicesArray->getNumberOfArrayElements();
        const int64_t numVoxelIndices = numIndices / 3;
        
        for (int64_t i = 0; i < numVoxelIndices; i++) {
            const int64_t i3 = i * 3;
            VoxelIJK voxelIJK(voxelIndicesArray->integerValue(i3),
                              voxelIndicesArray->integerValue(i3 + 1),
                              voxelIndicesArray->integerValue(i3 + 2));
            m_volumeAverageVoxelIndices.push_back(voxelIJK);
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
BrainordinateDataSelection::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                        const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrainordinateDataSelection",
                                            1);
    
    AString modeName = "MODE_NONE";
    switch (m_mode) {
        case MODE_NONE:
            modeName = "MODE_NONE";
            break;
        case MODE_SURFACE_AVERAGE:
            modeName = "MODE_SURFACE_AVERAGE";
            break;
        case MODE_SURFACE_NODE:
            modeName = "MODE_SURFACE_NODE";
            break;
        case MODE_VOXEL_XYZ:
            modeName = "MODE_VOXEL_XYZ";
            break;
        case MODE_VOXEL_AVERAGE:
            modeName = "MODE_VOXEL_AVERAGE";
            break;
    }
    
    sceneClass->addString("m_mode",
                          modeName);
    
    sceneClass->addString("m_surfaceFileName",
                          m_surfaceFileName);
    
    if (m_surfaceFileNodeIndices.empty() == false) {
        sceneClass->addIntegerArray("m_surfaceFileNodeIndices",
                                    &m_surfaceFileNodeIndices[0],
                                    m_surfaceFileNodeIndices.size());
    }
    
    sceneClass->addFloatArray("m_voxelXYZ",
                              m_voxelXYZ,
                              3);
    
    sceneClass->addIntegerArray("m_volumeAverageDimensionsIJK",
                                m_volumeAverageDimensionsIJK,
                                3);
    
    const int64_t numberOfVoxels = static_cast<int64_t>(m_volumeAverageVoxelIndices.size());
    if (numberOfVoxels > 0) {
        std::vector<int32_t> voxelIndicesVector;
        voxelIndicesVector.reserve(numberOfVoxels * 3);
        
        for (int64_t i = 0; i < numberOfVoxels; i++) {
            voxelIndicesVector.push_back(m_volumeAverageVoxelIndices[i].m_ijk[0]);
            voxelIndicesVector.push_back(m_volumeAverageVoxelIndices[i].m_ijk[1]);
            voxelIndicesVector.push_back(m_volumeAverageVoxelIndices[i].m_ijk[2]);
        }
        
        if (voxelIndicesVector.empty() == false) {
            sceneClass->addIntegerArray("m_volumeAverageVoxelIndices",
                                        &voxelIndicesVector[0],
                                        voxelIndicesVector.size());
        }
    }
    
    return sceneClass;
}
