
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

#define __CHARTING_DATA_MANAGER_DECLARE__
#include "ChartingDataManager.h"
#undef __CHARTING_DATA_MANAGER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "ModelChart.h"
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
}

/**
 * Destructor.
 */
ChartingDataManager::~ChartingDataManager()
{
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
                                                     const std::vector<int32_t>& nodeIndices) const throw (DataFileException)
{
    CaretAssert(surfaceFile);
    
    ModelChart* modelChart = m_brain->getChartModel();
    modelChart->loadAverageChartDataForSurfaceNodes(surfaceFile->getStructure(),
                                                    surfaceFile->getNumberOfNodes(),
                                                    nodeIndices);
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
                                             const int32_t nodeIndex) const throw (DataFileException)
{
    CaretAssert(surfaceFile);
    
    ModelChart* modelChart = m_brain->getChartModel();
    modelChart->loadChartDataForSurfaceNode(surfaceFile->getStructure(),
                                            surfaceFile->getNumberOfNodes(),
                                            nodeIndex);
}

/**
 * Load chart data for a voxel.
 *
 * @param xyz
 *     Coordinate of voxel.
 */
void
ChartingDataManager::loadChartForVoxelAtCoordinate(const float xyz[3]) const throw (DataFileException)
{
    ModelChart* modelChart = m_brain->getChartModel();
    modelChart->loadChartDataForVoxelAtCoordinate(xyz);
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
    
//    std::vector<ChartableBrainordinateInterface*> chartFiles;
//    if (requireChartingEnableInFiles) {
//        m_brain->getAllChartableDataFilesWithChartingEnabled(chartFiles);
//    }
//    else {
//        m_brain->getAllChartableDataFiles(chartFiles);
//    }
//    
//    for (std::vector<ChartableBrainordinateInterface*>::iterator fileIter = chartFiles.begin();
//         fileIter != chartFiles.end();
//         fileIter++) {
//        ChartableBrainordinateInterface* chartFile = *fileIter;
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


