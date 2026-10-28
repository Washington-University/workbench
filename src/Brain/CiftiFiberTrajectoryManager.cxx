
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

#define __CIFTI_FIBER_TRAJECTORY_MANAGER_DECLARE__
#include "CiftiFiberTrajectoryManager.h"
#undef __CIFTI_FIBER_TRAJECTORY_MANAGER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiMappableDataFile.h"
#include "EventSurfaceNodesGetNearXYZ.h"
#include "HtmlTableBuilder.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SurfaceFile.h"

using namespace caret;


    
/**
 * \class caret::CiftiFiberTrajectoryManager 
 * \brief Manages loading of trajectory data.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
CiftiFiberTrajectoryManager::CiftiFiberTrajectoryManager()
: CaretObject()
{
}

/**
 * Destructor.
 */
CiftiFiberTrajectoryManager::~CiftiFiberTrajectoryManager()
{
}

/**
 * Load data for the given surface node index.
 *
 * @param brain
 *    The brain
 * @param surfaceFile
 *    Surface File that contains the node (uses its structure).
 * @param nodeIndex
 *    Index of the surface node.
 * @param rowColumnInformationOut
 *    Appends one string for each row/column loaded
 * @param htmlTableBuilder
 *     Html table builder for cifti loading info
 * @return
 *    true if any data was loaded, else false.
 */
bool
CiftiFiberTrajectoryManager::loadDataForSurfaceNode(Brain* brain,
                                                    const SurfaceFile* surfaceFile,
                                                    const int32_t nodeIndex,
                                                    std::vector<AString>& rowColumnInformationOut,
                                                    HtmlTableBuilder& htmlTableBuilder)
{
    bool dataWasLoaded = false;
    
    /*
     * Load fiber trajectory data
     */
    std::vector<CiftiFiberTrajectoryLoadingInterface*> trajFiles(getFilesEnabledForDataLoading(brain));

    for (CiftiFiberTrajectoryLoadingInterface* tf : trajFiles) {
        const int64_t rowIndex = tf->loadDataForSurfaceNode(surfaceFile->getStructure(),
                                         surfaceFile->getNumberOfNodes(),
                                         nodeIndex);
        if (rowIndex >= 0) {
            /*
             * Get row/column info for node
             */
            rowColumnInformationOut.push_back(tf->getMapFileNameNoPath()
                                              + " nodeIndex="
                                              + AString::number(nodeIndex)
                                              + ", row index= "
                                              + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
            htmlTableBuilder.addRow(("Row Index: " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                    tf->getMapFileNameNoPath());
            dataWasLoaded = true;
        }
    }
    
    return dataWasLoaded;
}

/**
 * Load data for the given surface node index.
 *
 * @param brain
 *    The brain
 * @param surfaceFile
 *    Surface File that contains the node (uses its structure).
 * @param nodeIndex
 *    Index of the surface node.
 * @return
 *    true if any data was loaded, else false.
 */
bool
CiftiFiberTrajectoryManager::loadDataAverageForSurfaceNodes(Brain* brain,
                                                            const SurfaceFile* surfaceFile,
                                                            const std::vector<int32_t>& nodeIndices)
{
    bool dataWasLoaded = false;
    AString errorMessage;
    
    /*
     * Load fiber trajectory data
     */
    std::vector<CiftiFiberTrajectoryLoadingInterface*> trajFiles(getFilesEnabledForDataLoading(brain));
    
    for (CiftiFiberTrajectoryLoadingInterface* tf : trajFiles) {
        if (tf->loadDataAverageForSurfaceNodes(surfaceFile->getStructure(),
                                               surfaceFile->getNumberOfNodes(),
                                               nodeIndices)) {
            dataWasLoaded = true;
        }
    }

    if (errorMessage.isEmpty() == false) {
        CaretLogWarning(errorMessage);
    }
    
    return dataWasLoaded;
}

/**
 * Load data for the voxel near the given coordinate.
 * @param brain
 *    The brain
 * @param xyz
 *     Coordinate of voxel.
 * @param rowColumnInformationOut
 *    Appends one string for each row/column loaded
 * @param htmlTableBuilder
 *     Html table builder for cifti loading info
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool
CiftiFiberTrajectoryManager::loadDataForVoxelAtCoordinate(Brain* brain,
                                                          const float xyz[3],
                                                          std::vector<AString>& rowColumnInformationOut,
                                                          HtmlTableBuilder& htmlTableBuilder)
{
    bool haveData = false;

    /*
     * Load fiber trajectory data
     */
    std::vector<CiftiFiberTrajectoryLoadingInterface*> trajFiles(getFilesEnabledForDataLoading(brain));
    
    for (CiftiFiberTrajectoryLoadingInterface* tf : trajFiles) {
        CaretMappableDataFile* mapFile(tf->getMapFile());
        CaretAssert(mapFile);
        if ( ! mapFile->isEmpty()) {
            const int64_t rowIndex = tf->loadDataForVoxelAtCoordinate(xyz);
            if (rowIndex >= 0) {
                /*
                 * Get row/column info for node
                 */
                rowColumnInformationOut.push_back(tf->getMapFileNameNoPath()
                                                  + " Voxel XYZ="
                                                  + AString::fromNumbers(xyz, 3, ",")
                                                  + ", row index= "
                                                  + AString::number(rowIndex));
                htmlTableBuilder.addRow(("Row Index: " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                        tf->getMapFileNameNoPath());
                haveData = true;
            }
            else {
                /*
                 * Look for nearby surface nodes
                 * 'maxDist' is maximum distance a coordinate may be
                 * from the query (XYZ).
                 */
                const float maxDist(2.0);
                EventSurfaceNodesGetNearXYZ nearbyNodesEvent(xyz,
                                                             maxDist);
                EventManager::get()->sendEvent(nearbyNodesEvent.getPointer());
                const int32_t numNearbyNodes(nearbyNodesEvent.getNumberOfNearbyNodes());
                for (int32_t i = 0; i < numNearbyNodes; i++) {
                    const EventSurfaceNodesGetNearXYZ::NodeInfo nodeInfo(nearbyNodesEvent.getNearbyNode(i));
                    
                    const bool dataValidFlag = loadDataForSurfaceNode(brain,
                                                                      nodeInfo.getSurfaceFile(),
                                                                      nodeInfo.getNodeIndex(),
                                                                      rowColumnInformationOut,
                                                                      htmlTableBuilder);
                    if (dataValidFlag) {
                        haveData = true;
                        break;
                    }
                }
            }
        }
    }
    
    return haveData;
}

/**
 * Load average data for the given voxel indices.
 *
 * @param brain
 *    The brain
 * @param volumeDimensionIJK
 *    Dimensions of the volume.
 * @param voxelIndices
 *    Indices for averaging of data.
 * @return
 *    true if any data was loaded, else false.
 */
bool
CiftiFiberTrajectoryManager::loadAverageDataForVoxelIndices(Brain* brain,
                                                            const int64_t volumeDimensionIJK[3],
                                                            const std::vector<VoxelIJK>& voxelIndices)
{
    bool haveData = false;

    /*
     * Load fiber trajectory data
     */
    std::vector<CiftiFiberTrajectoryLoadingInterface*> trajFiles(getFilesEnabledForDataLoading(brain));
    
    for (CiftiFiberTrajectoryLoadingInterface* tf : trajFiles) {
        if (tf->loadAverageDataForVoxelIndices(volumeDimensionIJK,
                                               voxelIndices)) {
            haveData = true;
        }
    }
    
    return haveData;
}

/**
 * @return Fiber trajectory files that support data loading and are enabled for loading
 * @param brain
 *    Get files from this brain
 */
std::vector<CiftiFiberTrajectoryLoadingInterface*>
CiftiFiberTrajectoryManager::getFilesEnabledForDataLoading(Brain* brain)
{
    std::vector<CiftiFiberTrajectoryLoadingInterface*> trajFilesOut;
    
    std::vector<CiftiFiberTrajectoryLoadingInterface*> allTrajFiles(brain->getFiberTrajectoryLoadingInterfaceFiles());
    for (CiftiFiberTrajectoryLoadingInterface* tf : allTrajFiles) {
        if (tf->isDataLoadingSupported()) {
            if (tf->isDataLoadingEnabled()) {
                if ( ! tf->getMapFile()->isEmpty()) {
                    trajFilesOut.push_back(tf);
                }
            }
        }
    }
    
    return trajFilesOut;
}

