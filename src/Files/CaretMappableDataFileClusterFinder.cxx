
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
#include "Cluster.h"
#include "ClusterContainer.h"
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
 * @param mapIndex
 *   Index of map that is searched
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
    
    m_clusterContainer.reset(new ClusterContainer);
}

/**
 * Destructor.
 */
CaretMappableDataFileClusterFinder::~CaretMappableDataFileClusterFinder()
{
}

/**
 * Run the search for custers
 * @return Result of the search.
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
    
    std::unique_ptr<CaretResult> result;
    
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
            
            result = findLabelVolumeClusters(volumeFile);
        }
            break;
    }
    
    CaretAssert(result);
    return result;
}

/**
 * @return The clusters that were found
 */
const ClusterContainer*
CaretMappableDataFileClusterFinder::getClusterContainer() const
{
    return m_clusterContainer.get();
}

/**
 * @return A pointer to the cluster contain that the caller takes ownership
 * of and is responsible for deleting.  After calling this function, calling this
 * function again or calling 'getClusterContainer()' will return NULL.
 */
ClusterContainer*
CaretMappableDataFileClusterFinder::takeClusterContainer()
{
    return m_clusterContainer.release();
}

/**
 * @return The clusters in a formatted text string
 */
AString
CaretMappableDataFileClusterFinder::getClustersInFormattedString() const
{
    if (m_clusterContainer) {
        return m_clusterContainer->getClustersInFormattedString();
    }
    return AString("");
}

/**
 * Find clusters in a label volume
 * @param volumeFile
 *    Volume that is searched
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
     * Set unassiged labels as searched
     */
    const int32_t unassignedLabelKey(labelTable->getUnassignedLabelKey());
    for (int64_t m = 0; m < numVoxels; m++) {
        if (static_cast<int32_t>(voxelData[m]) == unassignedLabelKey) {
            CaretAssertVectorIndex(voxelHasBeenSearchedFlags, m);
            voxelHasBeenSearchedFlags[m] = 1;
        }
    }
            
    const VolumeSpace& volumeSpace(volumeFile->getVolumeSpace());
    
    std::set<int32_t> labelKeysWithClusters;
    
    const int32_t debugKey(-1);
    
    /*
     * Loop through all voxels
     */
    for (int64_t i = 0; i < dimI; i++) {
        for (int64_t j = 0; j < dimJ; j++) {
            for (int64_t k = 0; k < dimK; k++) {
                const int64_t voxelOffset(volumeSpace.getIndex(i, j, k));
                CaretAssertVectorIndex(voxelHasBeenSearchedFlags, voxelOffset);
                if ( ! voxelHasBeenSearchedFlags[voxelOffset]) {
                    voxelHasBeenSearchedFlags[voxelOffset] = 1;
                    
                    /*
                     * Search for connected voxels with 'labelKey'
                     */
                    const float labelKey(volumeFile->getValue(i, j, k, m_mapIndex, 0));
                    if (labelTable->getLabel(labelKey) == NULL) {
                        CaretLogInfo("Finding clusters, skipping label key="
                                     + AString::number(labelKey)
                                     + " that does not have a label");
                        continue;
                    }
                    
                    labelKeysWithClusters.insert(labelKey);
                    
                    const VoxelIJK voxelIJK(i, j, k);
                    Vector3D voxelXYZ;
                    volumeSpace.indexToSpace(voxelIJK, voxelXYZ);
                    
                    if (debugKey == labelKey) {
                        std::cout << "Debug: Starting search for key=" << labelKey
                        << " Name=" << labelTable->getLabelName(labelKey)
                        << " Voxel=(" << i << ", " << j << ", " << k << ")"
                        << " XYZ=" << voxelXYZ.toString() << std::endl;
                    }
                    
                    /*
                     * Sum is used to compute cluster's center of gravity
                     */
                    Vector3D voxelClusterSumXYZ(voxelXYZ);
                    float numVoxelsInCluster(1.0);
                    
                    std::vector<VoxelIJK> neighboringVoxelIJKsToSearch;
                    neighboringVoxelIJKsToSearch.reserve(500); /* avoid reallocations */
                    
                    /*
                     * Get neighbors of current voxel
                     */
                    volumeFile->getNeigbors26(voxelIJK,
                                              voxelData,
                                              labelKey,
                                              labelKey,
                                              voxelHasBeenSearchedFlags,
                                              neighboringVoxelIJKsToSearch);
                    
                    /*
                     * Loop through neighbors
                     * 'neighboringVoxelIJKsToSearch' will increase as neighbors of neighbors are added
                     */
                    for (int64_t index = 0; index < static_cast<int64_t>(neighboringVoxelIJKsToSearch.size()); index++) {
                        CaretAssertVectorIndex(neighboringVoxelIJKsToSearch, index);
                        const VoxelIJK& vijk(neighboringVoxelIJKsToSearch[index]);
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
                    
                    /*
                     * Save the cluster
                     */
                    CaretAssert(numVoxelsInCluster >= 1.0);
                    const Vector3D clusterCogXYZ(voxelClusterSumXYZ / numVoxelsInCluster);
                    Cluster* cluster = new Cluster(labelTable->getLabelName(labelKey),
                                                   labelKey,
                                                   clusterCogXYZ,
                                                   static_cast<int64_t>(numVoxelsInCluster));
                    m_clusterContainer->addCluster(cluster);
                    
                    if (debugKey == labelKey) {
                        std::cout << "   Cluster: " << cluster->toString() << std::endl << std::flush;
                    }
                }
            }
        }
    }
    
    const std::set<int32_t> allKeys(labelTable->getKeys());
    std::set<AString> labelNames;
    for (const int32_t key : allKeys) {
        if (labelKeysWithClusters.find(key) == labelKeysWithClusters.end()) {
            if (key != unassignedLabelKey) {
                m_clusterContainer->addKeyThatIsNotInAnyCluster(key);
                labelNames.insert(labelTable->getLabelName(key));
            }
        }
    }
    if ( ! labelNames.empty()) {
        AString text("File: "
                     + volumeFile->getFileNameNoPath()
                     + " map: "
                     + volumeFile->getMapName(m_mapIndex)
                     + "   \nLabels are not used by any voxels:");
        for (const AString& name : labelNames) {
            text.appendWithNewLine("      " + name);
        }
        CaretLogInfo(text);
    }
    
    
    return CaretResult::newInstanceSuccess();
}

