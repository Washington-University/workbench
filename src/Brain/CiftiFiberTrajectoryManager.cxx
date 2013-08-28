
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

#define __CIFTI_FIBER_TRAJECTORY_MANAGER_DECLARE__
#include "CiftiFiberTrajectoryManager.h"
#undef __CIFTI_FIBER_TRAJECTORY_MANAGER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
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
 * @param surfaceFile
 *    Surface File that contains the node (uses its structure).
 * @param nodeIndex
 *    Index of the surface node.
 * @return
 *    true if any data was loaded, else false.
 */
bool
CiftiFiberTrajectoryManager::loadDataForSurfaceNode(Brain* brain,
                                                    const SurfaceFile* surfaceFile,
                                                    const int32_t nodeIndex,
                                                    std::vector<AString>& rowColumnInformationOut) throw (DataFileException)
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
                                              + AString::number(rowIndex));
            dataWasLoaded = true;
        }
    }
    
    return dataWasLoaded;
}

/**
 * Load data for the given surface node index.
 *
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
                                                            const std::vector<int32_t>& nodeIndices) throw (DataFileException)
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
 * @param xyz
 *     Coordinate of voxel.
 * @param rowColumnInformationOut
 *    Appends one string for each row/column loaded
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool
CiftiFiberTrajectoryManager::loadDataForVoxelAtCoordinate(Brain* brain,
                                                          const float xyz[3],
                                                          std::vector<AString>& rowColumnInformationOut) throw (DataFileException)
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
                haveData = true;
            }
        }
    }
    
    return haveData;
}

/**
 * Load average data for the given voxel indices.
 *
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
                                                            const std::vector<VoxelIJK>& voxelIndices) throw (DataFileException)
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

