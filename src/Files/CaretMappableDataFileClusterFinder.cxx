
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
#include "LabelFile.h"
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
            const CiftiBrainordinateLabelFile* ciftiLabelFile(dynamic_cast<const CiftiBrainordinateLabelFile*>(m_mapFile));
            if (ciftiLabelFile == NULL) {
                return CaretResult::newInstanceError(m_mapFile->getFileName()
                                                     + " is not a CIFTI Labels file");
            }
            if ( ! ciftiLabelFile->isMappedWithLabelTable()) {
                return CaretResult::newInstanceError(m_mapFile->getFileName()
                                                     + " is not a label mapped file");
            }
  
            const CiftiXML& ciftiXML(ciftiLabelFile->getCiftiXML());
            const CiftiBrainModelsMap& brainModelsMap(ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN));
            const std::vector<StructureEnum::Enum> allStructures(brainModelsMap.getSurfaceStructureList());
            
            /*
             * Get data values (label indices) for the map
             * Applies to all structures
             */
            std::vector<float> mapValuesFloat;
            ciftiLabelFile->getMapData(m_mapIndex, mapValuesFloat);

            for (const StructureEnum::Enum structure : allStructures) {
                const int32_t surfaceNumberOfNodes(brainModelsMap.getSurfaceNumberOfNodes(structure));
                
                
                const GiftiLabelTable* labelTable(ciftiLabelFile->getMapLabelTable(m_mapIndex));
                CaretAssert(labelTable);
                                
                /*
                 * Allocate data to number of nodes in surface and initialize
                 * with the unassigned label
                 */
                m_numData = surfaceNumberOfNodes;
                m_dataStorage.resize(m_numData);
                m_dataPointer = &m_dataStorage[0];
                const int32_t unassignedLabelKey(labelTable->getUnassignedLabelKey());
                std::fill(m_dataStorage.begin(), m_dataStorage.end(), unassignedLabelKey);

                /*
                 * Load vertex label indices into data storage
                 */
                std::vector<CiftiBrainModelsMap::SurfaceMap> surfaceMaps(brainModelsMap.getSurfaceMap(structure));
                for (CiftiBrainModelsMap::SurfaceMap sm : surfaceMaps) {
                    CaretAssertVectorIndex(mapValuesFloat, sm.m_ciftiIndex);
                    CaretAssertVectorIndex(m_dataStorage, sm.m_surfaceNode);
                    m_dataStorage[sm.m_surfaceNode] = mapValuesFloat[sm.m_ciftiIndex];
                }

                result = findClustersInFile(structure);
                if (result->isError()) {
                    return result;
                }
            }
        }
            break;
        case FindMode::GIFTI_LABEL:
        {
            const LabelFile* labelFile(dynamic_cast<const LabelFile*>(m_mapFile));
            if (labelFile == NULL) {
                return CaretResult::newInstanceError(m_mapFile->getFileName()
                                                     + " is not a CIFTI Labels file");
            }
            if ( ! labelFile->isMappedWithLabelTable()) {
                return CaretResult::newInstanceError(m_mapFile->getFileName()
                                                     + " is not a label mapped file");
            }
            
            const int32_t* dataPtr(labelFile->getLabelKeyPointerForColumn(m_mapIndex));
            CaretAssert(dataPtr);
            
            const StructureEnum::Enum structure(labelFile->getStructure());
            const int32_t surfaceNumberOfNodes(labelFile->getNumberOfNodes());
            
            /*
             * Allocate data to number of nodes in surface and initialize
             * with the unassigned label
             */
            m_numData = surfaceNumberOfNodes;
            m_dataStorage.resize(m_numData);
            m_dataPointer = &m_dataStorage[0];

            for (int32_t i = 0; i < surfaceNumberOfNodes; i++) {
                CaretAssertVectorIndex(m_dataStorage, i);
                m_dataStorage[i] = dataPtr[i];
            }
            
            result = findClustersInFile(structure);
        }
            break;
        case FindMode::VOLUME_LABEL:
        {
            m_volumeFile = dynamic_cast<const VolumeFile*>(m_mapFile);
            if (m_volumeFile == NULL) {
                return CaretResult::newInstanceError(m_mapFile->getFileName()
                                                     + " is not a volume file");
            }
            if ( ! m_volumeFile->isMappedWithLabelTable()) {
                return CaretResult::newInstanceError(m_mapFile->getFileName()
                                                     + " is not a label volume file");
            }
            
            int64_t dimComp, dimMaps;
            m_volumeFile->getDimensions(m_volumeDimI, m_volumeDimJ, m_volumeDimK, dimMaps, dimComp);
            m_numData = (m_volumeDimI * m_volumeDimJ * m_volumeDimK);
            if (m_numData <= 0) {
                return CaretResult::newInstanceSuccess(); /* ignore empty volume*/
            }
            m_dataPointer = m_volumeFile->getFrame();
            CaretAssert(m_dataPointer);
            
            const VolumeSpace& volumeSpaceReference(m_volumeFile->getVolumeSpace());
            m_volumeSpace = &volumeSpaceReference;

            result = findClustersInFile(StructureEnum::INVALID);
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
 * Find clusters in a label type file
 * @param surfaceStructure
 *    Structure for surface data
 * @return The result
 */
std::unique_ptr<CaretResult>
CaretMappableDataFileClusterFinder::findClustersInFile(const StructureEnum::Enum surfaceStructure)
{
    m_labelTable = m_mapFile->getMapLabelTable(m_mapIndex);
    CaretAssert(m_labelTable);
    CaretAssert(m_dataPointer);
    CaretAssert(m_numData > 0);
    
    /*
     * Set unassiged labels as searched
     */
    std::vector<char> dataHasBeenSearchedFlags(m_numData, 0);
    const int32_t unassignedLabelKey(m_labelTable->getUnassignedLabelKey());
    for (int64_t m = 0; m < m_numData; m++) {
        if (static_cast<int32_t>(m_dataPointer[m]) == unassignedLabelKey) {
            CaretAssertVectorIndex(dataHasBeenSearchedFlags, m);
            dataHasBeenSearchedFlags[m] = 1;
        }
    }
    
    std::set<int32_t> labelKeysWithClusters;
    
    const SurfaceFile* surfaceFile;
    CaretPointer<TopologyHelper> topologyHelper;
    switch (m_findMode) {
        case FindMode::CIFTI_DENSE_LABEL:
        case FindMode::GIFTI_LABEL:
            {
                /*
                 * Get surface file so that we can use its topology helper
                 */
                EventSurfaceFileGet surfaceFileEvent(surfaceStructure,
                                                     m_numData);
                EventManager::get()->sendEvent(surfaceFileEvent.getPointer());

                surfaceFile = surfaceFileEvent.getMidthicknessAnatomicalSurface();
                if (surfaceFile == NULL) {
                    return CaretResult::newInstanceError("Anatomical surface with structure="
                                                         + StructureEnum::toName(surfaceStructure)
                                                         + " not found for finding surface clusters.");
                }
                CaretAssert(surfaceFile);
                topologyHelper = surfaceFile->getTopologyHelper();
            }
            break;
        case FindMode::VOLUME_LABEL:
            CaretAssert(m_volumeFile);
            CaretAssert(m_volumeSpace);
            break;
    }

    /*
     * Loop through data
     */
    for (int64_t iData = 0; iData < m_numData; iData++) {
        CaretAssertVectorIndex(dataHasBeenSearchedFlags, iData);
        if ( ! dataHasBeenSearchedFlags[iData]) {
            dataHasBeenSearchedFlags[iData] = 1;
            
            /*
             * Search for connected voxels with 'labelKey'
             */
            const float labelKey(m_dataPointer[iData]);
            if (m_labelTable->getLabel(labelKey) == NULL) {
                CaretLogInfo("Finding clusters, skipping label key="
                             + AString::number(labelKey)
                             + " that does not have a label");
                continue;
            }
            
            labelKeysWithClusters.insert(labelKey);
            
            /*
             * XYZ of brainordinate
             */
            VoxelIJK voxelIJK;
            Vector3D dataXYZ;
            switch (m_findMode) {
                case FindMode::CIFTI_DENSE_LABEL:
                case FindMode::GIFTI_LABEL:
                    surfaceFile->getCoordinate(iData,
                                               dataXYZ);
                    break;
                case FindMode::VOLUME_LABEL:
                {
                    int64_t voxelI, voxelJ, voxelK;
                    ijkFromIndex(iData, voxelI, voxelJ, voxelK);
                    voxelIJK = VoxelIJK(voxelI, voxelJ, voxelK);
                    m_volumeSpace->indexToSpace(voxelIJK, dataXYZ);
                }
                    break;
            }
            
            /*
             * Get neighbors
             */
            std::vector<int64_t> surfaceVertexIndicesToSearch;
            surfaceVertexIndicesToSearch.reserve(500); /* avoid reallocations */
            std::vector<VoxelIJK> neighboringVoxelIJKsToSearch;
            switch (m_findMode) {
                case FindMode::CIFTI_DENSE_LABEL:
                case FindMode::GIFTI_LABEL:
                {
                    std::vector<int32_t> surfaceVertexIndices = getSurfaceNeighbors(topologyHelper,
                                                                                    iData,
                                                                                    dataHasBeenSearchedFlags);
                    surfaceVertexIndicesToSearch.insert(surfaceVertexIndicesToSearch.end(),
                                                        surfaceVertexIndices.begin(),
                                                        surfaceVertexIndices.end());
                }
                    break;
                case FindMode::VOLUME_LABEL:
                {
                    neighboringVoxelIJKsToSearch.reserve(500); /* avoid reallocations for small clusters */
                    
                    /*
                     * Get neighbors of current voxel
                     */
                    m_volumeFile->getNeigbors26(voxelIJK,
                                                m_dataPointer,
                                                labelKey,
                                                labelKey,
                                                dataHasBeenSearchedFlags,
                                                neighboringVoxelIJKsToSearch);
                }
                    break;
            }
            
            /*
             * Sum is used to compute cluster's center of gravity
             */
            std::vector<Vector3D> clusterCoordsXYZ;
            clusterCoordsXYZ.push_back(dataXYZ);
            
            /*
             * Loop through the neighbors
             */
            switch (m_findMode) {
                case FindMode::CIFTI_DENSE_LABEL:
                case FindMode::GIFTI_LABEL:
                    for (int64_t index = 0; index < static_cast<int64_t>(surfaceVertexIndicesToSearch.size()); index++) {
                        CaretAssertVectorIndex(surfaceVertexIndicesToSearch, index);
                        const int64_t vertexIndex(surfaceVertexIndicesToSearch[index]);
                        Vector3D vertexXYZ;
                        surfaceFile->getCoordinate(vertexIndex,
                                                   vertexXYZ);
                        clusterCoordsXYZ.push_back(vertexXYZ);
                        
                        std::vector<int32_t> surfaceVertexIndices = getSurfaceNeighbors(topologyHelper,
                                                                                        vertexIndex,
                                                                                        dataHasBeenSearchedFlags);
                        surfaceVertexIndicesToSearch.insert(surfaceVertexIndicesToSearch.end(),
                                                            surfaceVertexIndices.begin(),
                                                            surfaceVertexIndices.end());
                    }
                    break;
                case FindMode::VOLUME_LABEL:
                {
                    /*
                     * Loop through neighbors
                     * 'neighboringVoxelIJKsToSearch' will increase as neighbors of neighbors are added
                     */
                    for (int64_t index = 0; index < static_cast<int64_t>(neighboringVoxelIJKsToSearch.size()); index++) {
                        CaretAssertVectorIndex(neighboringVoxelIJKsToSearch, index);
                        const VoxelIJK vijk(neighboringVoxelIJKsToSearch[index]); //TSC: needs to be a copy in case a large cluster reallocates and invalidates all references
                        clusterCoordsXYZ.push_back(m_volumeSpace->indexToSpace(vijk));
                        
                        m_volumeFile->getNeigbors26(vijk,
                                                    m_dataPointer,
                                                    labelKey,
                                                    labelKey,
                                                    dataHasBeenSearchedFlags,
                                                    neighboringVoxelIJKsToSearch);
                    }
                }
                    break;
            }
            
            
            /*
             * Save the cluster
             */
            CaretAssert( ! clusterCoordsXYZ.empty());
            Cluster* cluster(new Cluster(m_labelTable->getLabelName(labelKey),
                                         labelKey,
                                         clusterCoordsXYZ));
            m_clusterContainer->addCluster(cluster);
        }
    }
        
    const std::set<int32_t> allKeys(m_labelTable->getKeys());
    std::set<AString> labelNames;
    for (const int32_t key : allKeys) {
        if (labelKeysWithClusters.find(key) == labelKeysWithClusters.end()) {
            if (key != unassignedLabelKey) {
                m_clusterContainer->addKeyThatIsNotInAnyCluster(key);
                labelNames.insert(m_labelTable->getLabelName(key));
            }
        }
    }
    if ( ! labelNames.empty()) {
        AString text("File: "
                     + m_mapFile->getFileNameNoPath()
                     + " map: "
                     + m_mapFile->getMapName(m_mapIndex)
                     + "   \nLabels are not used by any brainordinates:");
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
 * @return Unvisited neighbors with same data value as the given node.
 * @param topologyHelper
 *    The topology helper for getting neighbors
 * @param nodeNumber
 *    Number of node to get neighbors for
 * @param nodeVisited
 *    Visisted status of all nodes, will get updated
 * @return
 *    A vector containing the surface vertex neighbors
 */
std::vector<int32_t>
CaretMappableDataFileClusterFinder::getSurfaceNeighbors(const TopologyHelper* topologyHelper,
                                                        const int32_t nodeNumber,
                                                        std::vector<char>& nodeVisited)
{
    std::vector<int32_t> neighborsOut;
    
    CaretAssertVectorIndex(m_dataStorage, nodeNumber);
    const int32_t labelKey(m_dataStorage[nodeNumber]);
    
    const std::vector<int32_t> neighborNodes(topologyHelper->getNodeNeighbors(nodeNumber));
    for (const int32_t neighNode : neighborNodes) {
        CaretAssertVectorIndex(nodeVisited, neighNode);
        if ( ! nodeVisited[neighNode]) {
            CaretAssertVectorIndex(m_dataStorage, neighNode);
            if (m_dataStorage[neighNode] == labelKey) {
                nodeVisited[neighNode] = 1;
                neighborsOut.push_back(neighNode);
            }
        }
    }
    
    return neighborsOut;
}

/**
 * Convert a data index to a volume IJK for volume file searches.
 * @param index
 *    The data index (flat index)
 * @param iOut
 *    Output with voxel I-index
 * @param jOut
 *    Output with voxel J-index
 * @param kOut
 *    Output with voxel K-index
 */
void
CaretMappableDataFileClusterFinder::ijkFromIndex(const int64_t index,
                                                 int64_t& iOut,
                                                 int64_t& jOut,
                                                 int64_t& kOut) const
{
    const int64_t dimIJ(m_volumeDimI * m_volumeDimJ);
    kOut = (index / dimIJ);
    const int64_t remainderOne(index - (kOut * dimIJ));
    jOut = (remainderOne / m_volumeDimI);
    iOut = (remainderOne - (jOut * m_volumeDimI));
}
