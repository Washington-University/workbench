
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
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiLabelsMap.h"
#include "CiftiXML.h"
#include "Cluster.h"
#include "ClusterContainer.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventSurfaceFileGet.h"
#include "StringTableModel.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
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
        case FindMode::CIFTI_DENSE_LABEL:
        {
            const CiftiBrainordinateLabelFile* ciftiLabelsFile(dynamic_cast<const CiftiBrainordinateLabelFile*>(m_mapFile));
            if (ciftiLabelsFile == NULL) {
                return CaretResult::newInstanceError(m_mapFile->getFileName()
                                                     + " is not a CIFTI Labels file");
            }
            if ( ! ciftiLabelsFile->isMappedWithLabelTable()) {
                return CaretResult::newInstanceError(m_mapFile->getFileName()
                                                     + " is not a label mapped file");
            }
            
            CaretLogWarning("Label clusters not supported for Cifti Label File.");
            
            result = findLabelCiftiClusters(ciftiLabelsFile);
        }
            break;
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
                    std::vector<Vector3D> clusterCoordsXYZ;
                    clusterCoordsXYZ.push_back(voxelXYZ);
                    
                    std::vector<VoxelIJK> neighboringVoxelIJKsToSearch;
                    neighboringVoxelIJKsToSearch.reserve(500); /* avoid reallocations for small clusters */
                    
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
                        const VoxelIJK vijk(neighboringVoxelIJKsToSearch[index]); //TSC: needs to be a copy in case a large cluster reallocates and invalidates all references
                        clusterCoordsXYZ.push_back(volumeSpace.indexToSpace(vijk));
                        
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
                    CaretAssert( ! clusterCoordsXYZ.empty());
                    Cluster* cluster(new Cluster(labelTable->getLabelName(labelKey),
                                                 labelKey,
                                                 clusterCoordsXYZ));
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
    
    const bool printClustersFlag(false);
    if (printClustersFlag) {
        const AString txt(m_clusterContainer->getClustersInFormattedString());
        std::cout << txt << std::endl;
        auto mergedContainer(m_clusterContainer->mergeDisjointRightLeftClusters());
        std::cout << std::endl << "MERGED" << std::endl;
        std::cout << mergedContainer->getClustersInFormattedString() << std::endl;
    }

    /*
     * Combine left (right) clusters with other left (right) clusters for each key
     */
    m_clusterContainer = m_clusterContainer->mergeDisjointRightLeftClusters();
    
    return CaretResult::newInstanceSuccess();
}

/**
 * Find clusters in a cifti label file
 * @param ciftiLabelFile
 *    CIFTI label file that is searched
 * @return The result
 */
std::unique_ptr<CaretResult>
CaretMappableDataFileClusterFinder::findLabelCiftiClusters(const CiftiBrainordinateLabelFile* ciftiLabelFile)
{
    /* Disable until finished */
    return CaretResult::newInstanceSuccess();

    CaretAssert(ciftiLabelFile);
    
    const CiftiXML& ciftiXML(ciftiLabelFile->getCiftiXML());
    const CiftiBrainModelsMap& brainModelsMap(ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN));
    const CiftiLabelsMap& ciftiLabelsMap(ciftiXML.getLabelsMap(CiftiXML::ALONG_ROW));

    if (brainModelsMap.hasVolumeData()) {
        const int64_t* dimPtr(brainModelsMap.getVolumeSpace().getDims());
        if (dimPtr != NULL) {
            int64_t dims[3] { dimPtr[0], dimPtr[1], dimPtr[2] };
            const int64_t numVolumeVoxels(dims[0] * dims[1] * dims[2]);
            if (numVolumeVoxels > 0) {
                
            }
        }
    }
    
    const std::vector<StructureEnum::Enum> surfaceStructures(brainModelsMap.getSurfaceStructureList());
    for (const StructureEnum::Enum structure : surfaceStructures) {
        findLabelCiftiSurfaceClusters(ciftiLabelFile,
                                      structure);
        std::cout << "Maps to structures: " << StructureEnum::toName(structure) << std::endl;
    }
    return CaretResult::newInstanceSuccess();
}

/**
 * Find surface clusters for the given structure in a cifti label file
 * @param ciftiLabelFile
 *    CIFTI label file that is searched
 * @return The result
 */
std::unique_ptr<CaretResult>
CaretMappableDataFileClusterFinder::findLabelCiftiSurfaceClusters(const CiftiBrainordinateLabelFile* ciftiLabelFile,
                                                                  const StructureEnum::Enum structure)
{
    const CiftiXML& ciftiXML(ciftiLabelFile->getCiftiXML());
    const CiftiBrainModelsMap& brainModelsMap(ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN));
    const int32_t numberOfNodes(brainModelsMap.getSurfaceNumberOfNodes(structure));
    if (numberOfNodes < 0) {
        return CaretResult::newInstanceSuccess(); /* ignore structure */
    }
    
    /*
     * Get surface file so that we can use its topology helper
     */
    EventSurfaceFileGet surfaceFileEvent(structure, numberOfNodes);
    EventManager::get()->sendEvent(surfaceFileEvent.getPointer());
    const int32_t numSurfacesFound(surfaceFileEvent.getNumberOfSurfaceFiles());
    if (numSurfacesFound <= 0) {
        return CaretResult::newInstanceSuccess();
    }
    const SurfaceFile* surfaceFile(surfaceFileEvent.getSurfaceFile(0));
    CaretPointer<TopologyHelper> topologyHelper(surfaceFile->getTopologyHelper());
    
    
    /*
     * Get data values (label indices) for the map
     */
    std::vector<float> mapValuesFloat;
    ciftiLabelFile->getMapData(m_mapIndex, mapValuesFloat);
    std::cout << "Map length: " << mapValuesFloat.size() << std::endl;
    
    /*
     * Get the values for the nodes in the current surface
     */
    std::vector<int32_t> nodeValues(numberOfNodes, -1);
    std::vector<CiftiBrainModelsMap::SurfaceMap> surfaceMaps(brainModelsMap.getSurfaceMap(structure));
    for (CiftiBrainModelsMap::SurfaceMap sm : surfaceMaps) {
        CaretAssertVectorIndex(mapValuesFloat, sm.m_ciftiIndex);
        CaretAssertVectorIndex(nodeValues, sm.m_surfaceNode);
        nodeValues[sm.m_surfaceNode] = mapValuesFloat[sm.m_ciftiIndex];
    }

    /*
     * Loop through all nodes until all have been searchedc
     */
    std::vector<bool> nodeVisited(numberOfNodes, 0);
    for (int32_t iNode = 0; iNode < numberOfNodes; iNode++) {
        if ( ! nodeVisited[iNode]) {
            nodeVisited[iNode] = true;
            
            if (nodeValues[iNode] > 0) {
                std::vector<int32_t> neighborNodesToSearch;
                neighborNodesToSearch.reserve(500); /* avoid reallocations*/
                
                /*
                 * Label key for current node
                 */
                const int32_t labelKey(nodeValues[iNode]);
                
                /*
                 * Get unsearched neighbors of current node
                 */
                const std::vector<int32_t> neighborNodes(topologyHelper->getNodeNeighbors(nodeValues[iNode]));
                for (const int32_t neighNode : neighborNodes) {
                    if ( ! nodeVisited[neighNode]) {
                        if (nodeValues[neighNode] == labelKey) {
                            neighborNodesToSearch.push_back(neighNode);
                        }
                    }
                }
                
                /*
                 * Search neighbors of neighbors
                 */
                for (int32_t iNeigh = 0; iNeigh < neighborNodesToSearch.size(); iNeigh) {
                    if ( ! nodeVisited[iNeigh]) {
                        if (nodeValues[iNeigh] == labelKey) {
                            neighborNodesToSearch.push_back(iNeigh);
                        }
                    }

                }
            }
        }
    }
    
    return CaretResult::newInstanceSuccess();
}

/**
 * Find volume clusters in a cifti label file
 * @param ciftiLabelFile
 *    CIFTI label file that is searched
 * @return The result
 */
std::unique_ptr<CaretResult>
CaretMappableDataFileClusterFinder::findLabelCiftiVolumeClusters(const CiftiBrainordinateLabelFile* ciftiLabelFile)
{
    return CaretResult::newInstanceSuccess();
}
