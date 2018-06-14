
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

#define __CIFTI_BRAINORDINATE_LABEL_FILE_DECLARE__
#include "CiftiBrainordinateLabelFile.h"
#undef __CIFTI_BRAINORDINATE_LABEL_FILE_DECLARE__

#include "CiftiFile.h"
#include "CiftiLabelsMap.h"

using namespace caret;


    
/**
 * \class caret::CiftiBrainordinateLabelFile 
 * \brief CIFTI Brainordinate by Label File
 * \ingroup Files
 */

/**
 * Constructor.
 */
CiftiBrainordinateLabelFile::CiftiBrainordinateLabelFile()
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL)
{
    
}

/**
 * Constructor for sub-classes.
 *
 * @param dataFileType
 *     Type of data file.
 */
CiftiBrainordinateLabelFile::CiftiBrainordinateLabelFile(const DataFileTypeEnum::Enum dataFileType)
: CiftiMappableDataFile(dataFileType)
{
    
}


/**
 * Destructor.
 */
CiftiBrainordinateLabelFile::~CiftiBrainordinateLabelFile()
{
    
}

/**
 * For the given structure with the given number of nodes, get indices of all
 * nodes in the map that have the same label key (node value).
 *
 * @param structure
 *     Structure of surface.
 * @param surfaceNumberOfNodes
 *     Number of nodes in surface.
 * @param mapIndex
 *     Index of the map.
 * @param labelKey
 *     Desired label key.
 */
void
CiftiBrainordinateLabelFile::getNodeIndicesWithLabelKey(const StructureEnum::Enum structure,
                                const int32_t surfaceNumberOfNodes,
                                const int32_t mapIndex,
                                const int32_t labelKey,
                                std::vector<int32_t>& nodeIndicesOut) const
{
    nodeIndicesOut.clear();
    std::vector<int64_t> dataIndices;
    
    if (getSurfaceDataIndicesForMappingToBrainordinates(structure,
                                                                   surfaceNumberOfNodes,
                                                        dataIndices)) {
        std::vector<float> mapData;
        getMapData(mapIndex,
                   mapData);
        
        for (int32_t i = 0; i < surfaceNumberOfNodes; i++) {
            const int64_t dataIndex = dataIndices[i];
            if (dataIndex >= 0) {
                CaretAssertVectorIndex(mapData, dataIndex);
                const int32_t dataKey = static_cast<int32_t>(mapData[dataIndex]);
                if (dataKey == labelKey) {
                    nodeIndicesOut.push_back(i);
                }
            }
        }
    }
}

/**
 * Get the voxel indices of all voxels in the given map with the given label key.
 *
 * @param mapIndex
 *    Index of map.
 * @param labelKey
 *    Key of the label.
 * @param voxelIndicesOut
 *    Output containing indices of voxels with the given label key.
 */
void
CiftiBrainordinateLabelFile::getVoxelIndicesWithLabelKey(const int32_t mapIndex,
                                                         const int32_t labelKey,
                                                         std::vector<VoxelIJK>& voxelIndicesOut) const
{
    voxelIndicesOut.clear();
    const CiftiXML& myXML = m_ciftiFile->getCiftiXML();
    
    if (myXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) {
        return;
    }
    std::vector<CiftiBrainModelsMap::VolumeMap> volumeMaps = myXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN).getFullVolumeMap();
    
    
    std::vector<float> mapData;
    getMapData(mapIndex,
               mapData);
    
    for (std::vector<CiftiBrainModelsMap::VolumeMap>::iterator iter = volumeMaps.begin();
         iter != volumeMaps.end();
         iter++) {
        const CiftiBrainModelsMap::VolumeMap& vm = *iter;
        const int64_t dataOffset = vm.m_ciftiIndex;
        
        CaretAssertVectorIndex(mapData, dataOffset);
        const int32_t key = static_cast<int32_t>(mapData[dataOffset]);
        
        if (key == labelKey) {
            voxelIndicesOut.push_back(VoxelIJK(vm.m_ijk));
        }
    }
}

/**
 * Get the voxel indices of all voxels in the given map with the given label key.
 *
 * @param mapIndex
 *    Index of map.
 * @param labelKey
 *    Key of the label.
 * @param voxelXyzOut
 *    Output containing coordinates of voxels with the given label key.
 */
void
CiftiBrainordinateLabelFile::getVoxelCoordinatesWithLabelKey(const int32_t mapIndex,
                                                             const int32_t labelKey,
                                                             std::vector<float>& voxelXyzOut) const
{
    voxelXyzOut.clear();
    std::vector<VoxelIJK> voxelIJK;
    
    getVoxelIndicesWithLabelKey(mapIndex,
                                labelKey,
                                voxelIJK);
    if (voxelIJK.empty()) {
        return;
    }
    const CiftiXML& myXML = m_ciftiFile->getCiftiXML();
    
    if (myXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) {
        return;
    }
    const VolumeSpace volumeSpace = myXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN).getVolumeSpace();
    
    const int64_t numVoxels = static_cast<int64_t>(voxelIJK.size());
    for (int64_t i = 0; i < numVoxels; i++) {
        float xyz[3];
        volumeSpace.indexToSpace(voxelIJK[i].m_ijk, xyz);
        voxelXyzOut.push_back(xyz[0]);
        voxelXyzOut.push_back(xyz[1]);
        voxelXyzOut.push_back(xyz[2]);
    }
}
