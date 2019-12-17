
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
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiMappableDataFile.h"
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
    const int32_t numTrajFiles = brain->getNumberOfConnectivityFiberTrajectoryFiles();
    for (int32_t iTrajFileIndex = 0; iTrajFileIndex < numTrajFiles; iTrajFileIndex++) {
        CiftiFiberTrajectoryFile* trajFile = brain->getConnectivityFiberTrajectoryFile(iTrajFileIndex);
        const int64_t rowIndex = trajFile->loadDataForSurfaceNode(surfaceFile->getStructure(),
                                         surfaceFile->getNumberOfNodes(),
                                         nodeIndex);
        if (rowIndex >= 0) {
            /*
             * Get row/column info for node
             */
            rowColumnInformationOut.push_back(trajFile->getFileNameNoPath()
                                              + " nodeIndex="
                                              + AString::number(nodeIndex)
                                              + ", row index= "
                                              + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
            htmlTableBuilder.addRow(("Row Index: " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                    trajFile->getFileNameNoPath());
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
    const int32_t numTrajFiles = brain->getNumberOfConnectivityFiberTrajectoryFiles();
    for (int32_t iTrajFileIndex = 0; iTrajFileIndex < numTrajFiles; iTrajFileIndex++) {
        CiftiFiberTrajectoryFile* trajFile = brain->getConnectivityFiberTrajectoryFile(iTrajFileIndex);
        
        try {
            trajFile->loadDataAverageForSurfaceNodes(surfaceFile->getStructure(),
                                                     surfaceFile->getNumberOfNodes(),
                                                     nodeIndices);
            dataWasLoaded = true;
        }
        catch (const DataFileException& dfe) {
            errorMessage.appendWithNewLine(dfe.whatString());
        }
    }

    if (errorMessage.isEmpty() == false) {
        throw DataFileException(errorMessage);
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
    std::vector<CiftiFiberTrajectoryFile*> ciftiTrajFiles;
    brain->getConnectivityFiberTrajectoryFiles(ciftiTrajFiles);
    
    bool haveData = false;
    for (std::vector<CiftiFiberTrajectoryFile*>::iterator iter = ciftiTrajFiles.begin();
         iter != ciftiTrajFiles.end();
         iter++) {
        CiftiFiberTrajectoryFile* trajFile = *iter;
        if (trajFile->isEmpty() == false) {
            const int64_t rowIndex = trajFile->loadMapDataForVoxelAtCoordinate(xyz);
            if (rowIndex >= 0) {
                /*
                 * Get row/column info for node
                 */
                rowColumnInformationOut.push_back(trajFile->getFileNameNoPath()
                                                  + " Voxel XYZ="
                                                  + AString::fromNumbers(xyz, 3, ",")
                                                  + ", row index= "
                                                  + AString::number(rowIndex));
                htmlTableBuilder.addRow(("Row Index: " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                        trajFile->getFileNameNoPath());
                haveData = true;
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
 * @throw DataFileException
 *    If an error occurs.
 */
bool
CiftiFiberTrajectoryManager::loadAverageDataForVoxelIndices(Brain* brain,
                                                            const int64_t volumeDimensionIJK[3],
                                                            const std::vector<VoxelIJK>& voxelIndices)
{
    std::vector<CiftiFiberTrajectoryFile*> ciftiTrajFiles;
    brain->getConnectivityFiberTrajectoryFiles(ciftiTrajFiles);
    
    bool haveData = false;
    for (std::vector<CiftiFiberTrajectoryFile*>::iterator iter = ciftiTrajFiles.begin();
         iter != ciftiTrajFiles.end();
         iter++) {
        CiftiFiberTrajectoryFile* trajFile = *iter;
        if (trajFile->isEmpty() == false) {
            trajFile->loadMapAverageDataForVoxelIndices(volumeDimensionIJK,
                                                        voxelIndices);
            haveData = true;
        }
    }
    
    return haveData;
}

