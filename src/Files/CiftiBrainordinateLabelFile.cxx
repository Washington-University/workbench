
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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
    
    const std::vector<CiftiVolumeMap>* volumeMapsPointer = m_ciftiFacade->getVolumeMapForMappingDataToBrainordinates();
    if (volumeMapsPointer == NULL) {
        return;
    }
    const std::vector<CiftiVolumeMap>& volumeMaps = *volumeMapsPointer;
    
    std::vector<float> mapData;
    getMapData(mapIndex,
               mapData);
    
    for (std::vector<CiftiVolumeMap>::const_iterator iter = volumeMaps.begin();
         iter != volumeMaps.end();
         iter++) {
        const CiftiVolumeMap& vm = *iter;
        const int64_t dataOffset = vm.m_ciftiIndex;
        
        CaretAssertVectorIndex(mapData, dataOffset);
        const int32_t key = static_cast<int32_t>(mapData[dataOffset]);
        
        if (key == labelKey) {
            voxelIndicesOut.push_back(VoxelIJK(vm.m_ijk));
        }
    }
}


