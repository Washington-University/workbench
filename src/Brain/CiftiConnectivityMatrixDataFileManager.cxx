
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

#define __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_MANAGER_DECLARE__
#include "CiftiConnectivityMatrixDataFileManager.h"
#undef __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_MANAGER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CiftiConnectivityMatrixDataFile.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "SceneClass.h"
#include "SurfaceFile.h"

using namespace caret;
    
/**
 * \class caret::CiftiConnectivityMatrixDataFileManager 
 * \brief Manages loading data from cifti connectivity files
 * \ingroup Brain
 */

/**
 * Constructor.
 * 
 * @param brain
 *    Brain that uses this instance.
 */
CiftiConnectivityMatrixDataFileManager::CiftiConnectivityMatrixDataFileManager(Brain* brain)
: CaretObject()
{
    m_brain = brain;
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE);
}

/**
 * Destructor.
 */
CiftiConnectivityMatrixDataFileManager::~CiftiConnectivityMatrixDataFileManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CiftiConnectivityMatrixDataFileManager::toString() const
{
    return "CiftiConnectivityMatrixDataFileManager";
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   The event.
 */
void
CiftiConnectivityMatrixDataFileManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE) {
        EventSurfaceColoringInvalidate* colorEvent =
        dynamic_cast<EventSurfaceColoringInvalidate*>(event);
        CaretAssert(colorEvent);
        
//        this->colorConnectivityData();
        
        colorEvent->setEventProcessed();
    }
    /*else if (event->getEventType() == EventTypeEnum::UPDATE_TIME_COURSE_DIALOG)
     {
     Event
     }*/
}

/**
 * Load data for the given surface node index.
 * @param surfaceFile
 *    Surface File that contains the node (uses its structure).
 * @param nodeIndex
 *    Index of the surface node.
 * @param rowColumnInformationOut
 *    Appends one string for each row/column loaded
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool
CiftiConnectivityMatrixDataFileManager::loadDataForSurfaceNode(const SurfaceFile* surfaceFile,
                                                  const int32_t nodeIndex,
                                                  std::vector<AString>& rowColumnInformationOut) throw (DataFileException)
{
    std::vector<CiftiConnectivityMatrixDataFile*> ciftiMatrixFiles;
    m_brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
    
    PaletteFile* paletteFile = m_brain->getPaletteFile();
    
    bool haveData = false;
    for (std::vector<CiftiConnectivityMatrixDataFile*>::iterator iter = ciftiMatrixFiles.begin();
         iter != ciftiMatrixFiles.end();
         iter++) {
        CiftiConnectivityMatrixDataFile* cmf = *iter;
        if (cmf->isEmpty() == false) {
            const int32_t mapIndex = 0;
            const int64_t rowIndex = cmf->loadMapDataForSurfaceNode(mapIndex,
                                                                    surfaceFile->getNumberOfNodes(),
                                                                    surfaceFile->getStructure(),
                                                                    nodeIndex);
            cmf->updateScalarColoringForMap(mapIndex,
                                            paletteFile);
            haveData = true;
            
            if (rowIndex >= 0) {
                /*
                 * Get row/column info for node
                 */
                rowColumnInformationOut.push_back(cmf->getFileNameNoPath()
                                                  + " nodeIndex="
                                                  + AString::number(nodeIndex)
                                                  + ", row index= "
                                                  + AString::number(rowIndex));
            }
            
//            m_denseDataLoadedForScene.setSurfaceLoading(surfaceFile,
//                                                        nodeIndex);
        }
    }
    
    if (haveData) {
        //this->colorConnectivityData();
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    return haveData;
}

/**
 * Load data for each of the given surface node indices and average the data.
 * @param surfaceFile
 *    Surface File that contains the node (uses its structure).
 * @param nodeIndices
 *    Indices of the surface nodes.
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool
CiftiConnectivityMatrixDataFileManager::loadAverageDataForSurfaceNodes(const SurfaceFile* surfaceFile,
                                                          const std::vector<int32_t>& nodeIndices) throw (DataFileException)
{
    std::vector<CiftiConnectivityMatrixDataFile*> ciftiMatrixFiles;
    m_brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
    
    PaletteFile* paletteFile = m_brain->getPaletteFile();
    
    bool haveData = false;
    for (std::vector<CiftiConnectivityMatrixDataFile*>::iterator iter = ciftiMatrixFiles.begin();
         iter != ciftiMatrixFiles.end();
         iter++) {
        CiftiConnectivityMatrixDataFile* cmf = *iter;
        if (cmf->isEmpty() == false) {
            const int32_t mapIndex = 0;
            cmf->loadMapAverageDataForSurfaceNodes(mapIndex,
                                                   surfaceFile->getNumberOfNodes(),
                                                   surfaceFile->getStructure(),
                                                   nodeIndices);
            cmf->updateScalarColoringForMap(mapIndex,
                                            paletteFile);
//                m_denseDataLoadedForScene.setSurfaceAverageLoading(surfaceFile,
//                                                                   nodeIndices);
                haveData = true;
        }
    }
    
    if (haveData) {
//        this->colorConnectivityData();
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    return haveData;
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
CiftiConnectivityMatrixDataFileManager::loadDataForVoxelAtCoordinate(const float xyz[3],
                                                        std::vector<AString>& rowColumnInformationOut) throw (DataFileException)
{
    PaletteFile* paletteFile = m_brain->getPaletteFile();
    
    std::vector<CiftiConnectivityMatrixDataFile*> ciftiMatrixFiles;
    m_brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
    
    bool haveData = false;
    for (std::vector<CiftiConnectivityMatrixDataFile*>::iterator iter = ciftiMatrixFiles.begin();
         iter != ciftiMatrixFiles.end();
         iter++) {
        CiftiConnectivityMatrixDataFile* cmf = *iter;
        
        const int32_t mapIndex = 0;
        const int64_t rowIndex = cmf->loadMapDataForVoxelAtCoordinate(mapIndex,
                                                                      xyz);
        cmf->updateScalarColoringForMap(mapIndex,
                                        paletteFile);
        haveData = true;
//            m_denseDataLoadedForScene.setVolumeLoading(xyz);
        if (rowIndex >= 0) {
            /*
             * Get row/column info for node
             */
            rowColumnInformationOut.push_back(cmf->getFileNameNoPath()
                                              + " Voxel XYZ="
                                              + AString::fromNumbers(xyz, 3, ",")
                                              + ", row index= "
                                              + AString::number(rowIndex));
        }
    }
    
    if (haveData) {
//        this->colorConnectivityData();
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    return haveData;
}

/**
 * Reset all connectivity loaders.
 */
void
CiftiConnectivityMatrixDataFileManager::reset()
{
//    m_denseDataLoadedForScene.reset();
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
CiftiConnectivityMatrixDataFileManager::saveToScene(const SceneAttributes* sceneAttributes,
                                       const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ConnectivityLoaderManager",
                                            1);
//    sceneClass->addClass(m_denseDataLoadedForScene.saveToScene(sceneAttributes,
//                                                               "m_denseDataLoadedForScene"));
    
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
CiftiConnectivityMatrixDataFileManager::restoreFromScene(const SceneAttributes* sceneAttributes,
                                            const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const SceneClass* denseDataSceneClass = sceneClass->getClass("m_denseDataLoadedForScene");
//    m_denseDataLoadedForScene.restoreFromScene(sceneAttributes,
//                                               denseDataSceneClass,
//                                               m_brain,
//                                               this);
}

