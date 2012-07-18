
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
#include "StructureEnum.h"
#include "SurfaceFile.h"
#
    
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
    this->brain = brain;
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
    this->brain->getConnectivityFilesOfAllTypes(connectivityFiles);
    
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
    this->brain->getConnectivityFilesOfAllTypes(connectivityFiles);
    
    bool haveData = false;
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityFiles.begin();
         iter != connectivityFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        if (clf->isEmpty() == false) {
            if (clf->isDense()) {
                clf->loadAverageDataForSurfaceNodes(surfaceFile->getStructure(), nodeIndices);
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
    this->brain->getConnectivityFilesOfAllTypes(connectivityFiles);
    
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
    this->brain->getConnectivityFilesOfAllTypes(connectivityFiles);
    
    AString rowColText;
    bool haveData = false;
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityFiles.begin();
         iter != connectivityFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        if (clf->isEmpty() == false) {
            const int64_t rowIndex = clf->loadDataForVoxelAtCoordinate(xyz);
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
    this->brain->getConnectivityFilesOfAllTypes(connectivityFiles);
    
    for (std::vector<ConnectivityLoaderFile*>::iterator iter = connectivityFiles.begin();
         iter != connectivityFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        
        if (clf->isEmpty() == false) {
            const float* data = clf->getData();
            float* dataRGBA = clf->getDataRGBA();
            const int32_t dataSize = clf->getNumberOfDataElements();
            const FastStatistics* statistics = clf->getMapFastStatistics(0);
            const PaletteColorMapping* paletteColorMapping = clf->getMapPaletteColorMapping(0);
            
            const AString paletteName = paletteColorMapping->getSelectedPaletteName();
            Palette* palette = this->brain->getPaletteFile()->getPaletteByName(paletteName);
            if (palette != NULL) {
                NodeAndVoxelColoring::colorScalarsWithPalette(statistics, 
                                                              paletteColorMapping, 
                                                              palette, 
                                                              data, 
                                                              data, 
                                                              dataSize, 
                                                              dataRGBA);
                
                CaretLogFine("Connectivity Data Average/Min/Max: "
                             + QString::number(statistics->getMean())
                             + " "
                             + QString::number(statistics->getMostNegativeValue())
                             + " "
                             + QString::number(statistics->getMostPositiveValue()));
            }            
        }
    }
}

/**
 * Reset all connectivity loaders.
 */
void 
ConnectivityLoaderManager::reset()
{
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
    this->brain->getConnectivityTimeSeriesFiles(connectivityTimeSeriesFiles);
    
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
    this->brain->getConnectivityTimeSeriesFiles(connectivityTimeSeriesFiles);
    
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
    this->brain->getConnectivityTimeSeriesFiles(connectivityTimeSeriesFiles);
    
    
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
    this->brain->getConnectivityFilesOfAllTypes(connectivityFiles);
    
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
    this->brain->getConnectivityTimeSeriesFiles(connectivityTimeSeriesFiles);
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
