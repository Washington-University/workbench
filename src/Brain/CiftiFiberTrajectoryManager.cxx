
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
 * @param brain
 *    Brain whose trajectory files are managed.
 */
CiftiFiberTrajectoryManager::CiftiFiberTrajectoryManager(Brain* brain)
: CaretObject(),
m_brain(brain)
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
CiftiFiberTrajectoryManager::loadDataForSurfaceNode(const SurfaceFile* surfaceFile,
                                                    const int32_t nodeIndex) throw (DataFileException)
{
    bool dataWasLoaded = false;
    
    /*
     * Load fiber trajectory data
     */
    const int32_t numFiberFiles = m_brain->getNumberOfConnectivityFiberOrientationFiles();
    if (numFiberFiles > 0) {
        const int32_t numTrajFiles = m_brain->getNumberOfConnectivityFiberTrajectoryFiles();
        for (int32_t iTrajFileIndex = 0; iTrajFileIndex < numTrajFiles; iTrajFileIndex++) {
            int32_t fiberFileIndex = iTrajFileIndex;
            if (fiberFileIndex >= numFiberFiles) {
                fiberFileIndex = 0;
            }
            
            CiftiFiberTrajectoryFile* trajFile = m_brain->getConnectivityFiberTrajectoryFile(iTrajFileIndex);
            CiftiFiberOrientationFile* connFiberFile = m_brain->getConnectivityFiberOrientationFile(fiberFileIndex);
            trajFile->loadDataForSurfaceNode(connFiberFile,
                                             surfaceFile->getStructure(),
                                             surfaceFile->getNumberOfNodes(),
                                             nodeIndex);
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
CiftiFiberTrajectoryManager::loadDataAverageForSurfaceNodes(const SurfaceFile* surfaceFile,
                                                            const std::vector<int32_t>& nodeIndices) throw (DataFileException)
{
    bool dataWasLoaded = false;
    AString errorMessage;
    
    /*
     * Load fiber trajectory data
     */
    const int32_t numFiberFiles = m_brain->getNumberOfConnectivityFiberOrientationFiles();
    if (numFiberFiles > 0) {
        const int32_t numTrajFiles = m_brain->getNumberOfConnectivityFiberTrajectoryFiles();
        for (int32_t iTrajFileIndex = 0; iTrajFileIndex < numTrajFiles; iTrajFileIndex++) {
            int32_t fiberFileIndex = iTrajFileIndex;
            if (fiberFileIndex >= numFiberFiles) {
                fiberFileIndex = 0;
            }
            
            CiftiFiberTrajectoryFile* trajFile = m_brain->getConnectivityFiberTrajectoryFile(iTrajFileIndex);
            CiftiFiberOrientationFile* connFiberFile = m_brain->getConnectivityFiberOrientationFile(fiberFileIndex);
            
            try {
                trajFile->loadDataAverageForSurfaceNodes(connFiberFile,
                                                         surfaceFile->getStructure(),
                                                         surfaceFile->getNumberOfNodes(),
                                                         nodeIndices);
                dataWasLoaded = true;
            }
            catch (const DataFileException& dfe) {
                errorMessage.appendWithNewLine(dfe.whatString());
            }
        }
    }
    
    if (errorMessage.isEmpty() == false) {
        throw DataFileException(errorMessage);
    }
    
    return dataWasLoaded;
}

/**
 * Reset all data.
 */
void
CiftiFiberTrajectoryManager::reset()
{
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
CiftiFiberTrajectoryManager::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                                    const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "CiftiFiberTrajectoryManager",
                                            1);
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
CiftiFiberTrajectoryManager::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                                         const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }    
}



