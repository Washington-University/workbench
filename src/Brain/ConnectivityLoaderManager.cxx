
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __CONNECTIVITY_LOADER_MANAGER_DECLARE__
#include "ConnectivityLoaderManager.h"
#undef __CONNECTIVITY_LOADER_MANAGER_DECLARE__

using namespace caret;

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ConnectivityLoaderFile.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventManager.h"
#include "FastStatistics.h"
#include "NodeAndVoxelColoring.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "ScenePrimitiveArray.h"
#include "StructureEnum.h"
#include "Surface.h"
#include "SurfaceFile.h"
    
/**
 * \class ConnectivityLoaderManager 
 * \brief Manages on-demand loading of connectivity data.
 *
 * Manages connectivity files for on-demand loading of data.
 */
/**
 * Constructor.
 */
ConnectivityLoaderManager::ConnectivityLoaderManager(Brain* brain)
: CaretObject()
{
    m_brain = brain;

    this->reset();

    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE);
}

/**
 * Destructor.
 */
ConnectivityLoaderManager::~ConnectivityLoaderManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    this->reset();
}

/**
 * Load data for the given surface node index.
 * @param surfaceFile
 *    Surface File that contains the node (uses its structure).
 * @param nodeIndex
 *    Index of the surface node.
 * @param rowColumnInformationOut
 *    If not NULL, the string will be contain information listing the 
 *    row/column that corresponds to the surface node.
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool 
ConnectivityLoaderManager::loadDataForSurfaceNode(const SurfaceFile* surfaceFile,
                                                  const int32_t nodeIndex,
                                                  AString* rowColumnInformationOut) throw (DataFileException)
{
    std::vector<ConnectivityLoaderFile*> connectivityFiles;
    m_brain->getMappableConnectivityFilesOfAllTypes(connectivityFiles);
    
    AString rowColText;
    
    bool haveData = false;
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityFiles.begin();
         iter != connectivityFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        if ((clf->isEmpty() == false)
            && clf->isDense()) {
            const int64_t rowIndex = clf->loadDataForSurfaceNode(surfaceFile->getStructure(), nodeIndex);
            haveData = true;
            
            if ((rowColumnInformationOut != NULL)
                && (rowIndex >= 0)) {
                /*
                 * Get row/column info for node 
                 */
                if (rowColText.isEmpty() == false) {
                    rowColText += "\n";
                }
                
                rowColText += ("   "
                               + clf->getFileNameNoPath()
                               + " nodeIndex="
                               + AString::number(nodeIndex)
                               + ", row index= "
                               + AString::number(rowIndex));
            }
            
            m_denseDataLoadedForScene.setSurfaceLoading(surfaceFile,
                                                        nodeIndex);
        }
    }
    
    if (rowColumnInformationOut != NULL) {
        *rowColumnInformationOut = rowColText;
    }
    
    if (haveData) {
        this->colorConnectivityData();
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
ConnectivityLoaderManager::loadAverageDataForSurfaceNodes(const SurfaceFile* surfaceFile,
                                                const std::vector<int32_t>& nodeIndices) throw (DataFileException)
{
    std::vector<ConnectivityLoaderFile*> connectivityFiles;
    m_brain->getMappableConnectivityFilesOfAllTypes(connectivityFiles);
    
    bool haveData = false;
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityFiles.begin();
         iter != connectivityFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        if (clf->isEmpty() == false) {
            if (clf->isDense()) {
                clf->loadAverageDataForSurfaceNodes(surfaceFile->getStructure(), nodeIndices);
                m_denseDataLoadedForScene.setSurfaceAverageLoading(surfaceFile,
                                                                   nodeIndices);
                haveData = true;
            }
            else if(clf->isDenseTimeSeries() && clf->isTimeSeriesGraphEnabled())
            {
                //moved to loadaveragetimeseriesforsurfacenodes
                //clf->loadAverageDataForSurfaceNodes(surfaceFile->getStructure(), nodeIndices);
            }
        }
    }
    
    if (haveData) {
        this->colorConnectivityData();
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    return haveData;
}

/**
 * Load time series for each of the given surface node indices and average the data.
 * @param surfaceFile
 *    Surface File that contains the node (uses its structure).
 * @param nodeIndices
 *    Indices of the surface nodes.
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool 
ConnectivityLoaderManager::loadAverageTimeSeriesForSurfaceNodes(const SurfaceFile* surfaceFile,
                                                const std::vector<int32_t>& nodeIndices, const TimeLine &timeLine) throw (DataFileException)
{
    std::vector<ConnectivityLoaderFile*> connectivityFiles;
    m_brain->getMappableConnectivityFilesOfAllTypes(connectivityFiles);
    
    bool haveData = false;
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityFiles.begin();
         iter != connectivityFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        if (clf->isEmpty() == false) {            
            if(clf->isDenseTimeSeries() && clf->isTimeSeriesGraphEnabled())
            {
                clf->loadAverageTimeSeriesForSurfaceNodes(surfaceFile->getStructure(), nodeIndices, timeLine);
            }
        }
    }

    return haveData;
}


/**
 * Load data for the voxel near the given coordinate.
 * @param xyz
 *     Coordinate of voxel.
 * @param rowColumnInformationOut
 *    If not NULL, the string will be contain information listing the 
 *    row/column that corresponds to the voxel.
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool 
ConnectivityLoaderManager::loadDataForVoxelAtCoordinate(const float xyz[3],
                                                        AString* rowColumnInformationOut) throw (DataFileException)
{
    std::vector<ConnectivityLoaderFile*> connectivityFiles;
    m_brain->getMappableConnectivityFilesOfAllTypes(connectivityFiles);
    
    AString rowColText;
    bool haveData = false;
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityFiles.begin();
         iter != connectivityFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        if (clf->isEmpty() == false) {
            const int64_t rowIndex = clf->loadDataForVoxelAtCoordinate(xyz);
            haveData = true;
            m_denseDataLoadedForScene.setVolumeLoading(xyz);
            if ((rowColumnInformationOut != NULL)
                && (rowIndex >= 0)) {
                /*
                 * Get row/column info for node 
                 */
                if (rowColText.isEmpty() == false) {
                    rowColText += "\n";
                }
                
                rowColText += ("   "
                               + clf->getFileNameNoPath()
                               + " Voxel XYZ="
                               + AString::fromNumbers(xyz, 3, ",")
                               + ", row index= "
                               + AString::number(rowIndex));
            }
        }
    }
    
    if (haveData) {
        this->colorConnectivityData();
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    if (rowColumnInformationOut != NULL) {
        *rowColumnInformationOut = rowColText;
    }
    return haveData;
}

void 
ConnectivityLoaderManager::colorConnectivityData()
{
    std::vector<ConnectivityLoaderFile*> connectivityFiles;
    m_brain->getMappableConnectivityFilesOfAllTypes(connectivityFiles);
    
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityFiles.begin();
         iter != connectivityFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        
        if (clf->isEmpty() == false) {
            const PaletteColorMapping* paletteColorMapping = clf->getMapPaletteColorMapping(0);
            
            const AString paletteName = paletteColorMapping->getSelectedPaletteName();
            Palette* palette = m_brain->getPaletteFile()->getPaletteByName(paletteName);
            if (palette != NULL) {
                clf->updateRGBAColoring(palette, 0);
            }
//           
//            
//            
//            
//            const float* data = clf->getData();
//            float* dataRGBA = clf->getDataRGBA();
//            const int32_t dataSize = clf->getNumberOfDataElements();
//            const FastStatistics* statistics = clf->getMapFastStatistics(0);
//            const PaletteColorMapping* paletteColorMapping = clf->getMapPaletteColorMapping(0);
//            
//            const AString paletteName = paletteColorMapping->getSelectedPaletteName();
//            Palette* palette = m_brain->getPaletteFile()->getPaletteByName(paletteName);
//            if (palette != NULL) {
//                NodeAndVoxelColoring::colorScalarsWithPalette(statistics, 
//                                                              paletteColorMapping, 
//                                                              palette, 
//                                                              data, 
//                                                              data, 
//                                                              dataSize, 
//                                                              dataRGBA);
//                
//                CaretLogFine("Connectivity Data Average/Min/Max: "
//                             + QString::number(statistics->getMean())
//                             + " "
//                             + QString::number(statistics->getMostNegativeValue())
//                             + " "
//                             + QString::number(statistics->getMostPositiveValue()));
//            }            
        }
    }
}

/**
 * Reset all connectivity loaders.
 */
void 
ConnectivityLoaderManager::reset()
{
    m_denseDataLoadedForScene.reset();
}


/**
 * Receive events from the event manager.
 * 
 * @param event
 *   The event.
 */
void 
ConnectivityLoaderManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE) {
        EventSurfaceColoringInvalidate* colorEvent =
        dynamic_cast<EventSurfaceColoringInvalidate*>(event);
        CaretAssert(colorEvent);
        
        this->colorConnectivityData();
        
        colorEvent->setEventProcessed();
    }
    /*else if (event->getEventType() == EventTypeEnum::UPDATE_TIME_COURSE_DIALOG)
    {
        Event
    }*/
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ConnectivityLoaderManager::toString() const
{
    return "ConnectivityLoaderManager";
}

/**
 * Load a time point from a connectivity file.
 * @param clf
 *    Connectivity file from which connectivity file is loaded.
 * @param seconds
 *    Time, in seconds, of the timepoint.
 * @return
 *    true if data was loaded, else false.
 */
/*bool 
ConnectivityLoaderManager::loadTimePointAtTime(ConnectivityLoaderFile* clf,
                                               const float seconds) throw (DataFileException)
{
    bool haveData = false;
    if (clf->isEmpty() == false) {
        clf->loadTimePointAtTime(seconds);
        haveData = true;
    }

    if (haveData) {
        this->colorConnectivityData();
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }

    return haveData;
}*/

bool ConnectivityLoaderManager::loadFrame(ConnectivityLoaderFile* clf,
    const int frame) throw (DataFileException)
{
    bool haveData = false;
    if (clf->isEmpty() == false) {
        clf->loadFrame(frame);
        haveData = true;
    }

    if (haveData) {
        this->colorConnectivityData();
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }

    return haveData;
}

/**
  * Get Surface TimeLines
  */
void
ConnectivityLoaderManager::getSurfaceTimeLines(QList<TimeLine> &tlV)
{
    std::vector<ConnectivityLoaderFile*> connectivityTimeSeriesFiles;
    m_brain->getConnectivityTimeSeriesFiles(connectivityTimeSeriesFiles);
    
    int indx = 0;
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityTimeSeriesFiles.begin();
         iter != connectivityTimeSeriesFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        if(clf->isDenseTimeSeries() &&
           clf->isSurfaceMappable() &&
           clf->isTimeSeriesGraphEnabled())
        {
            TimeLine tl;            
            clf->getTimeLine(tl);
            tl.clmID = indx+1;
            tl.timeStep = clf->getTimeStep();
            tlV.push_back(tl);
            
        }
        indx++;
    }

}

/**
  * Get Volume TimeLines
  */
void
ConnectivityLoaderManager::getVolumeTimeLines(QList<TimeLine> &tlV)
{
    std::vector<ConnectivityLoaderFile*> connectivityTimeSeriesFiles;
    m_brain->getConnectivityTimeSeriesFiles(connectivityTimeSeriesFiles);
    
    int indx = 0;
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityTimeSeriesFiles.begin();
         iter != connectivityTimeSeriesFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        if(clf->isDenseTimeSeries() &&
           clf->isVolumeMappable() &&
           clf->isTimeSeriesGraphEnabled())
        {
            TimeLine tl;            
            clf->getTimeLine(tl);
            tl.clmID = indx+1;
            tl.timeStep = clf->getTimeStep();
            tlV.push_back(tl);
        }
        indx++;
    }
}

/**
 * Load data for the given surface node index.
 * @param surfaceFile
 *    Surface File that contains the node (uses its structure).
 * @param nodeIndex
 *    Index of the surface node.
 * @param timeLine
 *    Output timeline.
 * @param rowColumnInformationOut
 *    If not NULL, the string will be contain information listing the 
 *    row/column that corresponds to the surface node.
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool 
ConnectivityLoaderManager::loadTimeLineForSurfaceNode(const SurfaceFile* surfaceFile,
                                                      const int32_t nodeIndex,
                                                      const TimeLine &timeLine,
                                                      AString* rowColumnInformationOut) throw (DataFileException)
{
    bool haveData = false;
    
    AString rowColText;
    
    std::vector<ConnectivityLoaderFile*> connectivityTimeSeriesFiles;
    m_brain->getConnectivityTimeSeriesFiles(connectivityTimeSeriesFiles);
    
    
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityTimeSeriesFiles.begin();
         iter != connectivityTimeSeriesFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        if (clf->isEmpty() == false) {
            
            const int64_t rowIndex = clf->loadTimeLineForSurfaceNode(surfaceFile->getStructure(), nodeIndex,timeLine);            
            haveData = true;
            
            if ((rowColumnInformationOut != NULL)
                && (rowIndex >= 0)) {
                /*
                 * Get row/column info for node 
                 */
                if (rowColText.isEmpty() == false) {
                    rowColText += "\n";
                }
                
                rowColText += ("   "
                               + clf->getFileNameNoPath()
                               + " nodeIndex="
                               + AString::number(nodeIndex)
                               + ", row index= "
                               + AString::number(rowIndex));
            }
        }
    }

    
    //if (haveData) {
        //this->colorConnectivityData();
        //EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    //}
    
    if (rowColumnInformationOut != NULL) {
        *rowColumnInformationOut = rowColText;
    }
    
    return haveData;
}

/**
 * @return True if there are enabled connectivity
 * files that retrieve data from the network.
 */
bool 
ConnectivityLoaderManager::hasNetworkFiles() const
{
    std::vector<ConnectivityLoaderFile*> connectivityFiles;
    m_brain->getMappableConnectivityFilesOfAllTypes(connectivityFiles);
    
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityFiles.begin();
         iter != connectivityFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        
        if (clf->isEmpty() == false) {
            const AString filename = clf->getFileName();
            if (filename.startsWith("http://")
                || filename.startsWith("https://")) {
                if (clf->isDataLoadingEnabled()) {
                    return true;
                }
            }
        }
    }

    return false; 
}

/**
 * Load data for the voxel near the given coordinate.
 * @param xyz
 *     Coordinate of voxel.
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool 
ConnectivityLoaderManager::loadTimeLineForVoxelAtCoordinate(const float xyz[3],
                                                            AString* rowColumnInformationOut) throw (DataFileException)
{
    AString rowColText;
    
    bool haveData = false;
    std::vector<ConnectivityLoaderFile*> connectivityTimeSeriesFiles;
    m_brain->getConnectivityTimeSeriesFiles(connectivityTimeSeriesFiles);
    TimeLine tl;
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityTimeSeriesFiles.begin();
         iter != connectivityTimeSeriesFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        if (clf->isEmpty() == false) {
            AString structure = StructureEnum::toGuiName(clf->getStructure());
            tl.label = structure + ":[" + AString::fromNumbers(xyz,3,AString(", ")) + "]";
            const int64_t rowIndex = clf->loadTimeLineForVoxelAtCoordinate(xyz,tl);
            haveData = true;
            
            if ((rowColumnInformationOut != NULL)
                && (rowIndex >= 0)) {
                /*
                 * Get row/column info for node 
                 */
                if (rowColText.isEmpty() == false) {
                    rowColText += "\n";
                }
                
                rowColText += ("   "
                               + clf->getFileNameNoPath()
                               + " Voxel XYZ="
                               + AString::fromNumbers(xyz, 3, ",")
                               + ", row index= "
                               + AString::number(rowIndex));
            }
       }
    }
    
    //if (haveData) {
        //this->colorConnectivityData();
        //EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    //}
    
    if (rowColumnInformationOut != NULL) {
        *rowColumnInformationOut = rowColText;
    }
    
    return haveData;
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
ConnectivityLoaderManager::saveToScene(const SceneAttributes* sceneAttributes,
                                       const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ConnectivityLoaderManager",
                                            1);
    sceneClass->addClass(m_denseDataLoadedForScene.saveToScene(sceneAttributes,
                                                               "m_denseDataLoadedForScene"));
    
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
ConnectivityLoaderManager::restoreFromScene(const SceneAttributes* sceneAttributes,
                                            const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const SceneClass* denseDataSceneClass = sceneClass->getClass("m_denseDataLoadedForScene");
    m_denseDataLoadedForScene.restoreFromScene(sceneAttributes,
                                               denseDataSceneClass,
                                               m_brain,
                                               this);
}

/*============================================================================*/
/**
 * \class caret::ConnectivityLoaderManager::DenseDataLoaded
 * \brief Holds information on last loaded connectivity data.
 */

/**
 * Constructor.
 */
ConnectivityLoaderManager::DenseDataLoaded::DenseDataLoaded()
{
    reset();
}

/**
 * Destructor.
 */
ConnectivityLoaderManager::DenseDataLoaded::~DenseDataLoaded()
{
    reset();
}

void
ConnectivityLoaderManager::DenseDataLoaded::reset()
{
    m_mode = MODE_NONE;
    m_surfaceFileName = "";
    m_surfaceFileNodeIndices.clear();
}

/**
 * Setup for single node dense connectivity data.
 * @param surfaceFile
 *     Surface file on which data was selected.
 * @param nodeIndex
 *     Index of node on the surface.
 */
void
ConnectivityLoaderManager::DenseDataLoaded::setSurfaceLoading(const SurfaceFile* surfaceFile,
                                                              const int32_t nodeIndex)
{
    reset();
    m_mode = MODE_SURFACE_NODE;
    m_surfaceFileName      = surfaceFile->getFileNameNoPath();
    m_surfaceFileNodeIndices.push_back(nodeIndex);
}

/**
 * Setup for multiple nodes average connectivity data.
 * @param surfaceFile
 *     Surface file on which data was selected.
 * @param nodeIndices
 *     Indices of node on the surface.
 */
void
ConnectivityLoaderManager::DenseDataLoaded::setSurfaceAverageLoading(const SurfaceFile* surfaceFile,
                                                                     const std::vector<int32_t>& nodeIndices)
{
    reset();
    m_mode = MODE_SURFACE_AVERAGE;
    m_surfaceFileName        = surfaceFile->getFileNameNoPath();
    m_surfaceFileNodeIndices = nodeIndices;
}

/**
 * Setup for voxel loading at a coordinate.
 * @param xyz
 *     Coordinate at a voxel.
 */
void
ConnectivityLoaderManager::DenseDataLoaded::setVolumeLoading(const float xyz[3])
{
    reset();
    m_mode = MODE_VOXEL_XYZ;
    m_voxelXYZ[0] = xyz[0];
    m_voxelXYZ[1] = xyz[1];
    m_voxelXYZ[2] = xyz[2];
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
ConnectivityLoaderManager::DenseDataLoaded::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                             const SceneClass* sceneClass,
                                                             Brain* brain,
                                                             ConnectivityLoaderManager* connMan)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_mode = MODE_NONE;
    const AString modeName = sceneClass->getStringValue("m_mode");
    if (modeName == "MODE_NONE") {
        m_mode = MODE_NONE;
    }
    else if (modeName == "MODE_SURFACE_AVERAGE") {
        m_mode = MODE_SURFACE_AVERAGE;
    }
    else if (modeName == "MODE_SURFACE_NODE") {
        m_mode = MODE_SURFACE_NODE;
    }
    else if (modeName == "MODE_VOXEL_XYZ") {
        m_mode = MODE_VOXEL_XYZ;
    }
    else {
        sceneAttributes->addToErrorMessage("Unrecognized mode=" + modeName);
        return;
    }
    
    m_surfaceFileName      = sceneClass->getStringValue("m_surfaceFileName");
    m_surfaceFileNodeIndices.clear();
    const ScenePrimitiveArray* nodeIndicesArray = sceneClass->getPrimitiveArray("m_surfaceFileNodeIndices");
    const int32_t numNodeIndices = nodeIndicesArray->getNumberOfArrayElements();
    m_surfaceFileNodeIndices.reserve(numNodeIndices);
    for (int32_t i = 0; i < numNodeIndices; i++) {
        m_surfaceFileNodeIndices.push_back(nodeIndicesArray->integerValue(i));
    }
    sceneClass->getFloatArrayValue("m_voxelXYZ",
                                   m_voxelXYZ,
                                   3);
    
    switch (m_mode) {
        case MODE_NONE:
            break;
        case MODE_SURFACE_AVERAGE:
        {
            if ((m_surfaceFileName.isEmpty() == false)
                && (numNodeIndices > 0)) {
                Surface* surface = brain->getSurfaceWithName(m_surfaceFileName,
                                                             false);
                if (surface != NULL) {
                    connMan->loadAverageDataForSurfaceNodes(surface,
                                                            m_surfaceFileNodeIndices);
                }
                else {
                    sceneAttributes->addToErrorMessage("Surface named "
                                                       + m_surfaceFileName
                                                       + " is missing.");
                }
            }
        }
            break;
        case MODE_SURFACE_NODE:
        {
            if ((m_surfaceFileName.isEmpty() == false)
                && (numNodeIndices > 0)) {
                Surface* surface = brain->getSurfaceWithName(m_surfaceFileName,
                                                             false);
                if (surface != NULL) {
                    if (numNodeIndices == 1) {
                        const int32_t nodeIndex = m_surfaceFileNodeIndices[0];
                        if (nodeIndex < surface->getNumberOfNodes()) {
                            connMan->loadDataForSurfaceNode(surface,
                                                            nodeIndex);
                        }
                    }
                }
                else {
                    sceneAttributes->addToErrorMessage("Surface named "
                                                       + m_surfaceFileName
                                                       + " is missing.");
                }
            }
        }
            break;
        case MODE_VOXEL_XYZ:
            connMan->loadDataForVoxelAtCoordinate(m_voxelXYZ);
            break;
    }
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
ConnectivityLoaderManager::DenseDataLoaded::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                                        const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DenseDataLoaded",
                                            1);
    
    AString modeName = "MODE_NONE";
    switch (m_mode) {
        case MODE_NONE:
            modeName = "MODE_NONE";
            break;
        case MODE_SURFACE_AVERAGE:
            modeName = "MODE_SURFACE_AVERAGE";
            break;
        case MODE_SURFACE_NODE:
            modeName = "MODE_SURFACE_NODE";
            break;
        case MODE_VOXEL_XYZ:
            modeName = "MODE_VOXEL_XYZ";
            break;
    }
    
    sceneClass->addString("m_mode",
                          modeName);
    sceneClass->addString("m_surfaceFileName",
                          m_surfaceFileName);
    sceneClass->addIntegerArray("m_surfaceFileNodeIndices",
                                &m_surfaceFileNodeIndices[0],
                                m_surfaceFileNodeIndices.size());
    sceneClass->addFloatArray("m_voxelXYZ",
                              m_voxelXYZ,
                              3);
    
    return sceneClass;
}

