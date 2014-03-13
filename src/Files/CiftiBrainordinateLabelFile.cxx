
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

#include "CiftiFacade.h"

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
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL,
                        CiftiMappableDataFile::FILE_READ_DATA_ALL,
                        CIFTI_INDEX_TYPE_LABELS,
                        CIFTI_INDEX_TYPE_BRAIN_MODELS,
                        CiftiMappableDataFile::DATA_ACCESS_WITH_COLUMN_METHODS,
                        CiftiMappableDataFile::DATA_ACCESS_WITH_ROW_METHODS)
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
    const std::vector<int64_t>* dataIndices = m_ciftiFacade->getSurfaceDataIndicesForMappingToBrainordinates(structure,
                                                                   surfaceNumberOfNodes);
    if (dataIndices != NULL) {
        std::vector<float> mapData;
        getMapData(mapIndex,
                   mapData);
        
        for (int32_t i = 0; i < surfaceNumberOfNodes; i++) {
            const int64_t dataIndex = (*dataIndices)[i];
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
    
    const std::vector<CiftiBrainModelsMap::VolumeMap>* volumeMapsPointer = m_ciftiFacade->getVolumeMapForMappingDataToBrainordinates();
    if (volumeMapsPointer == NULL) {
        return;
    }
    const std::vector<CiftiBrainModelsMap::VolumeMap>& volumeMaps = *volumeMapsPointer;
    
    std::vector<float> mapData;
    getMapData(mapIndex,
               mapData);
    
    for (std::vector<CiftiBrainModelsMap::VolumeMap>::const_iterator iter = volumeMaps.begin();
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


