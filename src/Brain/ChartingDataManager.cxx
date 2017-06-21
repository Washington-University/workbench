
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

#define __CHARTING_DATA_MANAGER_DECLARE__
#include "ChartingDataManager.h"
#undef __CHARTING_DATA_MANAGER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "ModelChart.h"
#include "ModelChartTwo.h"
#include "SurfaceFile.h"

using namespace caret;



/**
 * \class caret::ChartingDataManager
 * \brief Manages charting data.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
ChartingDataManager::ChartingDataManager(Brain* brain)
: CaretObject(),
m_brain(brain)
{
    CaretAssert(brain);
    
    /*
     * Need PROCESSED event (after others have handled the event)
     */
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP);
}

/**
 * Destructor.
 */
ChartingDataManager::~ChartingDataManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
ChartingDataManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP) {
        EventMapYokingSelectMap* yokeMapEvent = dynamic_cast<EventMapYokingSelectMap*>(event);
        CaretAssert(yokeMapEvent);
        
        ModelChart* modelChart = m_brain->getChartModel();
        if (modelChart != NULL) {
            modelChart->loadChartDataForYokedCiftiMappableFiles(yokeMapEvent->getMapYokingGroup(),
                                                                yokeMapEvent->getMapIndex());
        }
    }
}

/**
 * Load the average of chart data for a group of surface nodes.
 *
 * @param surfaceFile
 *     The surface file
 * @param nodeIndices
 *     Indices of nodes whose chart data is averaged
 */
void
ChartingDataManager::loadAverageChartForSurfaceNodes(const SurfaceFile* surfaceFile,
                                                     const std::vector<int32_t>& nodeIndices) const
{
    CaretAssert(surfaceFile);
    
    ModelChart* modelChart = m_brain->getChartModel();
    if (modelChart != NULL) {
        modelChart->loadAverageChartDataForSurfaceNodes(surfaceFile->getStructure(),
                                                        surfaceFile->getNumberOfNodes(),
                                                        nodeIndices);
    }
    
    ModelChartTwo* modelChartTwo = m_brain->getChartTwoModel();
    if (modelChartTwo != NULL) {
        modelChartTwo->loadAverageChartDataForSurfaceNodes(surfaceFile->getStructure(),
                                                           surfaceFile->getNumberOfNodes(),
                                                           nodeIndices);
    }
}

/**
 * Load chart data for a surface node.
 *
 * @param surfaceFile
 *     The surface file
 * @param nodeIndex
 *     Index of node.
 */
void
ChartingDataManager::loadChartForSurfaceNode(const SurfaceFile* surfaceFile,
                                             const int32_t nodeIndex) const
{
    CaretAssert(surfaceFile);
    
    ModelChart* modelChart = m_brain->getChartModel();
    if (modelChart != NULL) {
        modelChart->loadChartDataForSurfaceNode(surfaceFile->getStructure(),
                                                surfaceFile->getNumberOfNodes(),
                                                nodeIndex);
    }
    
    ModelChartTwo* modelChartTwo = m_brain->getChartTwoModel();
    if (modelChartTwo != NULL) {
        modelChartTwo->loadChartDataForSurfaceNode(surfaceFile->getStructure(),
                                                surfaceFile->getNumberOfNodes(),
                                                nodeIndex);
    }
}

/**
 * Load chart data from given file at the given row.
 *
 * @param ciftiMapFile
 *     The CIFTI file.
 * @param rowIndex
 *     Index of row in the file.
 */
void
ChartingDataManager::loadChartForCiftiMappableFileRow(CiftiMappableDataFile* ciftiMapFile,
                                                      const int32_t rowIndex) const
{
    CaretAssert(ciftiMapFile);
    
    ModelChart* modelChart = m_brain->getChartModel();
    if (modelChart != NULL) {
        modelChart->loadChartDataForCiftiMappableFileRow(ciftiMapFile,
                                                         rowIndex);
    }
    
    ModelChartTwo* modelChartTwo = m_brain->getChartTwoModel();
    if (modelChartTwo != NULL) {
        modelChartTwo->loadChartDataForCiftiMappableFileRow(ciftiMapFile,
                                                         rowIndex);
    }
}


/**
 * Load chart data for a voxel.
 *
 * @param xyz
 *     Coordinate of voxel.
 */
void
ChartingDataManager::loadChartForVoxelAtCoordinate(const float xyz[3]) const
{
    ModelChart* modelChart = m_brain->getChartModel();
    if (modelChart != NULL) {
        modelChart->loadChartDataForVoxelAtCoordinate(xyz);
    }
    
    ModelChartTwo* modelChartTwo = m_brain->getChartTwoModel();
    if (modelChartTwo != NULL) {
        modelChartTwo->loadChartDataForVoxelAtCoordinate(xyz);
    }
}

/**
 * @return True if there are charting that retrieve data from the network.
 */
bool
ChartingDataManager::hasNetworkFiles() const
{
    /*
     * At this time, all 'chartable' files are read in their entirety
     * so all data is local once the file is read.
     */
    return false;
    
//    std::vector<ChartableLineSeriesBrainordinateInterface*> chartFiles;
//    if (requireChartingEnableInFiles) {
//        m_brain->getAllChartableDataFilesWithChartingEnabled(chartFiles);
//    }
//    else {
//        m_brain->getAllChartableDataFiles(chartFiles);
//    }
//    
//    for (std::vector<ChartableLineSeriesBrainordinateInterface*>::iterator fileIter = chartFiles.begin();
//         fileIter != chartFiles.end();
//         fileIter++) {
//        ChartableLineSeriesBrainordinateInterface* chartFile = *fileIter;
//        
//        CaretDataFile* caretDataFile = dynamic_cast<CaretDataFile*>(chartFile);
//        CaretAssert(caretDataFile);
//        if (caretDataFile->isEmpty() == false) {
//            const AString filename = caretDataFile->getFileName();
//            if (CaretDataFile::isFileOnNetwork(filename)) {
//                return true;
//            }
//        }
//    }
//    
//    return false;
}


