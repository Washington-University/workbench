
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

#define __CIFTI_BRAINORDINATE_DATA_SERIES_FILE_DECLARE__
#include "CiftiBrainordinateDataSeriesFile.h"
#undef __CIFTI_BRAINORDINATE_DATA_SERIES_FILE_DECLARE__

#include "CaretLogger.h"
#include "ChartDataCartesian.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::CiftiBrainordinateDataSeriesFile 
 * \brief CIFTI Brainordinate by Data-Series File.
 * \ingroup Files
 */

/**
 * Constructor.
 */
CiftiBrainordinateDataSeriesFile::CiftiBrainordinateDataSeriesFile()
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
}

/**
 * Destructor.
 */
CiftiBrainordinateDataSeriesFile::~CiftiBrainordinateDataSeriesFile()
{
}

/**
 * @return Is charting enabled for this file?
 */
bool
CiftiBrainordinateDataSeriesFile::isLineSeriesChartingEnabled(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_chartingEnabledForTab[tabIndex];
}

/**
 * @return Return true if the file's current state supports
 * charting data, else false.  Typically a brainordinate file
 * is chartable if it contains more than one map.
 */
bool
CiftiBrainordinateDataSeriesFile::isLineSeriesChartingSupported() const
{
    if (getNumberOfMaps() > 1) {
        return true;
    }
    
    return false;
}

/**
 * Set charting enabled for this file.
 *
 * @param enabled
 *    New status for charting enabled.
 */
void
CiftiBrainordinateDataSeriesFile::setLineSeriesChartingEnabled(const int32_t tabIndex,
                                                     const bool enabled)
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_chartingEnabledForTab[tabIndex] = enabled;
}

/**
 * Load charting data for the surface with the given structure and node index.
 *
 * @param structure
 *     The surface's structure.
 * @param nodeIndex
 *     Index of the node.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will return true.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
CiftiBrainordinateDataSeriesFile::loadLineSeriesChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                               const int32_t nodeIndex)
{
    ChartDataCartesian* chartData = helpLoadChartDataForSurfaceNode(structure,
                                                           nodeIndex);
    return chartData;
}

/**
 * Load average charting data for the surface with the given structure and node indices.
 *
 * @param structure
 *     The surface's structure.
 * @param nodeIndices
 *     Indices of the node.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will be NULL.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
CiftiBrainordinateDataSeriesFile::loadAverageLineSeriesChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                                      const std::vector<int32_t>& nodeIndices)
{
    ChartDataCartesian* chartData = helpLoadChartDataForSurfaceNodeAverage(structure,
                                                                  nodeIndices);
    return chartData;
}

/**
 * Load charting data for the voxel enclosing the given coordinate.
 *
 * @param xyz
 *     Coordinate of voxel.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will be NULL.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
CiftiBrainordinateDataSeriesFile::loadLineSeriesChartDataForVoxelAtCoordinate(const float xyz[3])
{
    ChartDataCartesian* chartData = helpLoadChartDataForVoxelAtCoordinate(xyz);
    return chartData;
}

/**
 * Get chart data types supported by the file.
 *
 * @param chartDataTypesOut
 *    Chart types supported by this file.
 */
void
CiftiBrainordinateDataSeriesFile::getSupportedLineSeriesChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const
{
    helpGetSupportedLineSeriesChartDataTypes(chartDataTypesOut);
}

/**
 * Save file data from the scene.  For subclasses that need to
 * save to a scene, this method should be overriden.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
CiftiBrainordinateDataSeriesFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                 SceneClass* sceneClass)
{
    CiftiMappableDataFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);
    sceneClass->addBooleanArray("m_chartingEnabledForTab",
                                m_chartingEnabledForTab,
                                BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
}

/**
 * Restore file data from the scene.  For subclasses that need to
 * restore from a scene, this method should be overridden. The scene class
 * will be valid and any scene data may be obtained from it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
CiftiBrainordinateDataSeriesFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    CiftiMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    
    const ScenePrimitiveArray* tabArray = sceneClass->getPrimitiveArray("m_chartingEnabledForTab");
    if (tabArray != NULL) {
        sceneClass->getBooleanArrayValue("m_chartingEnabledForTab",
                                         m_chartingEnabledForTab,
                                         BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    }
    else {
        /*
         * Obsolete value when charting was not 'per tab'
         */
        const bool chartingEnabled = sceneClass->getBooleanValue("m_chartingEnabled",
                                                                   false);
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            m_chartingEnabledForTab[i] = chartingEnabled;
        }
    }
}


