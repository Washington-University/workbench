
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __CARET_MAPPABLE_DATA_FILE_CLUSTER_FINDER_DECLARE__
#include "CaretMappableDataFileClusterFinder.h"
#undef __CARET_MAPPABLE_DATA_FILE_CLUSTER_FINDER_DECLARE__

#include "CaretLogger.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "CaretAssert.h"
#include "StringTableModel.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::CaretMappableDataFileClusterFinder 
 * \brief Finds cluster in surface (in the future) and volume files for different types of data
 * \ingroup Files
 */

/**
 * Constructor.
 * @param mapFile
 *   Map file for searching
 * @param findMode
 *   The find mode
 */
CaretMappableDataFileClusterFinder::CaretMappableDataFileClusterFinder(const FindMode findMode,
                                                                       const CaretMappableDataFile* mapFile,
                                                                       const int32_t mapIndex)
: CaretObject(),
m_findMode(findMode),
m_mapFile(mapFile),
m_mapIndex(mapIndex)
{
    CaretAssert(m_mapFile);
}

/**
 * Destructor.
 */
CaretMappableDataFileClusterFinder::~CaretMappableDataFileClusterFinder()
{
}

/**
 * Run the search for custers
 * @return String describing this object's content.
 */
std::unique_ptr<CaretResult>
CaretMappableDataFileClusterFinder::findClusters()
{
    if (m_mapFile == NULL) {
        return CaretResult::newInstanceError("Input file is invalid (NULL).");
    }
    if ((m_mapIndex < 0)
        || (m_mapIndex >= m_mapFile->getNumberOfMaps())) {
        return CaretResult::newInstanceError("Map index="
                                             + AString::number(m_mapIndex)
                                             + " is invalid for "
                                             + m_mapFile->getFileName());
    }
    
    switch (m_findMode) {
        case FindMode::VOLUME_LABEL:
        {
            const VolumeFile* volumeFile(dynamic_cast<const VolumeFile*>(m_mapFile));
            if (volumeFile == NULL) {
                return CaretResult::newInstanceError(m_mapFile->getFileName()
                                                     + " is not a volume file");
            }
            if ( ! volumeFile->isMappedWithLabelTable()) {
                return CaretResult::newInstanceError(m_mapFile->getFileName()
                                                     + " is not a label volume file");
            }
            
            return findLabelVolumeClusters(volumeFile);
        }
            break;
    }
    return CaretResult::newInstanceError("Not implemented");
}

/**
 * @return The clusters that were found
 */
const std::vector<BrainordinateCluster>&
CaretMappableDataFileClusterFinder::getClusters() const
{
    return m_clusters;
}

/**
 * @return The clusters in a formatted text string
 */
AString
CaretMappableDataFileClusterFinder::getClustersInFormattedString() const
{
    const int32_t numClusters(m_clusters.size());
    if (numClusters <= 0) {
        return "No clusters were found.";
    }
    
    StringTableModel stm(numClusters + 1, 6);
    stm.setColumnAlignment(0, StringTableModel::ALIGN_RIGHT);
    stm.setElement(0, 0, "Key");
    stm.setColumnAlignment(1, StringTableModel::ALIGN_RIGHT);
    stm.setElement(0, 1, "Count");
    stm.setColumnAlignment(2, StringTableModel::ALIGN_RIGHT);
    stm.setElement(0, 2, "X");
    stm.setColumnAlignment(3, StringTableModel::ALIGN_RIGHT);
    stm.setElement(0, 3, "Y");
    stm.setColumnAlignment(4, StringTableModel::ALIGN_RIGHT);
    stm.setElement(0, 4, "Z");
    stm.setColumnAlignment(5, StringTableModel::ALIGN_LEFT);
    stm.setElement(0, 5, "Cluster Name");
    for (int32_t i = 0; i < numClusters; i++) {
        const BrainordinateCluster& c = m_clusters[i];
        const int32_t row(i + 1);
        stm.setElement(row, 0, c.getKey());
        stm.setElement(row, 1, c.getNumberOfBrainordinates());
        const Vector3D& cog = c.getCenterOfGravityXYZ();
        stm.setElement(row, 2, cog[0]);
        stm.setElement(row, 3, cog[1]);
        stm.setElement(row, 4, cog[2]);
        stm.setElement(row, 5, c.getName());
    }
    
    return stm.getInString();
}

/**
 * Find clusters in a label volume
 * @return The result
 */
std::unique_ptr<CaretResult>
CaretMappableDataFileClusterFinder::findLabelVolumeClusters(const VolumeFile* volumeFile)
{
    int64_t dimI, dimJ, dimK, dimComp, dimMaps;
    volumeFile->getDimensions(dimI, dimJ, dimK, dimMaps, dimComp);
    
    const GiftiLabelTable* labelTable(volumeFile->getMapLabelTable(m_mapIndex));
    
    const int64_t numVoxels(dimI * dimJ * dimK);
    std::vector<char> voxelHasBeenSearchedFlags(numVoxels, 0);
    
    const float* voxelData(volumeFile->getFrame(m_mapIndex));
    CaretAssert(voxelData);

    /*
     * Do not search unassiged labels
     */
    const int32_t unassignedLabelKey(labelTable->getUnassignedLabelKey());
    for (int64_t m = 0; m < numVoxels; m++) {
        if (static_cast<int32_t>(voxelData[m]) == unassignedLabelKey) {
            CaretAssertVectorIndex(voxelHasBeenSearchedFlags, m);
            voxelHasBeenSearchedFlags[m] = 1;
        }
    }
            
    const VolumeSpace& volumeSpace(volumeFile->getVolumeSpace());
    
    for (int64_t i = 0; i < dimI; i++) {
        for (int64_t j = 0; j < dimJ; j++) {
            for (int64_t k = 0; k < dimK; k++) {
                const int64_t voxelOffset(volumeSpace.getIndex(i, j, k));
                if ( ! voxelHasBeenSearchedFlags[voxelOffset]) {
                    voxelHasBeenSearchedFlags[voxelOffset] = 1;
                    const float labelKey(volumeFile->getValue(i, j, k, m_mapIndex, 0));
                    if (labelTable->getLabel(labelKey) == NULL) {
                        CaretLogInfo("Finding clusters, skipping label key="
                                     + AString::number(labelKey)
                                     + " that does not have a label");
                        continue;
                    }
                    
                    CaretAssert((i >= 0) && (i < dimI));
                    CaretAssert((j >= 0) && (j < dimJ));
                    CaretAssert((k >= 0) && (k < dimK));

                    VoxelIJK voxelIJK(i, j, k);
                    Vector3D voxelXYZ;
                    volumeSpace.indexToSpace(voxelIJK, voxelXYZ);
                    
                    Vector3D voxelClusterSumXYZ(voxelXYZ);
                    float numVoxelsInCluster(1.0);
                    
                    std::vector<VoxelIJK> neighboringVoxelIJKsToSearch;
                    neighboringVoxelIJKsToSearch.reserve(500); /* avoid reallocations */
                    
                    volumeFile->getNeigbors26(voxelIJK, 
                                              voxelData,
                                              labelKey,
                                              labelKey,
                                              voxelHasBeenSearchedFlags,
                                              neighboringVoxelIJKsToSearch);
                    
                    for (int64_t index = 0; index < static_cast<int64_t>(neighboringVoxelIJKsToSearch.size()); index++) {
                        CaretAssertVectorIndex(neighboringVoxelIJKsToSearch, index);
                        const VoxelIJK& vijk(neighboringVoxelIJKsToSearch[index]);
                        const int64_t vOffset(volumeSpace.getIndex(vijk.m_ijk));
                        Vector3D xyz;
                        voxelClusterSumXYZ += volumeSpace.indexToSpace(vijk);
                        numVoxelsInCluster += 1.0;
                        
                        volumeFile->getNeigbors26(vijk,
                                                  voxelData,
                                                  labelKey,
                                                  labelKey,
                                                  voxelHasBeenSearchedFlags,
                                                  neighboringVoxelIJKsToSearch);
                    }
                    
                    const Vector3D clusterCogXYZ(voxelClusterSumXYZ / numVoxelsInCluster);
                    m_clusters.emplace_back(labelTable->getLabelName(labelKey),
                                            labelKey,
                                            clusterCogXYZ,
                                            static_cast<int64_t>(numVoxelsInCluster));
                }
            }
        }
    }
    
    std::sort(m_clusters.begin(), m_clusters.end());
    for (const auto& cluster : m_clusters) {
        std::cout << cluster.toString() << std::endl;
    }
    
    return CaretResult::newInstanceSuccess();
}

