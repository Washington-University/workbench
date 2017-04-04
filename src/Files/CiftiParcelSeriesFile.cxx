
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

#define __CIFTI_PARCEL_SERIES_FILE_DECLARE__
#include "CiftiParcelSeriesFile.h"
#undef __CIFTI_PARCEL_SERIES_FILE_DECLARE__

#include "CaretLogger.h"
#include "ChartDataCartesian.h"
#include "CiftiParcelReorderingModel.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CiftiParcelSeriesFile 
 * \brief CIFTI Parcel by Data-Series File.
 * \ingroup Files
 */

/**
 * Constructor.
 */
CiftiParcelSeriesFile::CiftiParcelSeriesFile()
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
    
    m_parcelReorderingModel = std::unique_ptr<CiftiParcelReorderingModel>(new CiftiParcelReorderingModel(this));
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_parcelReorderingModel",
                          "CiftiParcelReorderingModel",
                          m_parcelReorderingModel.get());
}

/**
 * Destructor.
 */
CiftiParcelSeriesFile::~CiftiParcelSeriesFile()
{
    
}

/**
 * @return Is charting enabled for this file?
 */
bool
CiftiParcelSeriesFile::isLineSeriesChartingEnabled(const int32_t tabIndex) const
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
CiftiParcelSeriesFile::isLineSeriesChartingSupported() const
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
CiftiParcelSeriesFile::setLineSeriesChartingEnabled(const int32_t tabIndex,
                                          const bool enabled)
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_chartingEnabledForTab[tabIndex] = enabled;
}

/**
 * Get chart data types supported by the file.
 *
 * @param chartDataTypesOut
 *    Chart types supported by this file.
 */
void
CiftiParcelSeriesFile::getSupportedLineSeriesChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const
{
    helpGetSupportedLineSeriesChartDataTypes(chartDataTypesOut);
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
CiftiParcelSeriesFile::loadLineSeriesChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                               const int32_t nodeIndex)
{
    ChartDataCartesian* chartData = helpLoadChartDataForSurfaceNode(structure,
                                                           nodeIndex);
    return chartData;

//    ChartDataCartesian* chartData = NULL;
//    
//    try {
//        std::vector<float> data;
//        if (getSeriesDataForSurfaceNode(structure,
//                                        nodeIndex,
//                                        data)) {
//            const int64_t numData = static_cast<int64_t>(data.size());
//            
//            bool timeSeriesFlag = false;
//            bool dataSeriesFlag = false;
//            float convertTimeToSeconds = 1.0;
//            switch (getMapIntervalUnits()) {
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
//                    timeSeriesFlag = true;
//                    convertTimeToSeconds = 1000.0;
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
//                    convertTimeToSeconds = 1.0;
//                    timeSeriesFlag = true;
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
//                    dataSeriesFlag = true;
//                    break;
//                case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
//                    convertTimeToSeconds = 1000000.0;
//                    timeSeriesFlag = true;
//                    break;
//            }
//            
//            if (dataSeriesFlag) {
//                chartData = new ChartDataCartesian(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES,
//                                                   ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
//                                                   ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
//            }
//            else if (timeSeriesFlag) {
//                chartData = new ChartDataCartesian(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES,
//                                                   ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS,
//                                                   ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
//            }
//            
//            if (chartData != NULL) {
//                float timeStart = 0.0;
//                float timeStep  = 1.0;
//                if (timeSeriesFlag) {
//                    getMapIntervalStartAndStep(timeStart,
//                                               timeStep);
//                    timeStart *= convertTimeToSeconds;
//                    timeStep  *= convertTimeToSeconds;
//                    chartData->setTimeStartInSecondsAxisX(timeStart);
//                    chartData->setTimeStepInSecondsAxisX(timeStep);
//                }
//                
//                for (int64_t i = 0; i < numData; i++) {
//                    float xValue = i;
//                    
//                    if (timeSeriesFlag) {
//                        xValue = timeStart + (i * timeStep);
//                    }
//                    
//                    chartData->addPoint(xValue,
//                                        data[i]);
//                }
//                
//                const AString description = (getFileNameNoPath()
//                                             + " node "
//                                             + AString::number(nodeIndex));
//                chartData->setDescription(description);
//            }
//            else {
//                const AString msg = "New type of units for data series flag, needs updating for charting";
//                CaretAssertMessage(0, msg);
//                throw DataFileException(msg);
//            }
//        }
//    }
//    catch (const DataFileException& dfe) {
//        if (chartData != NULL) {
//            delete chartData;
//            chartData = NULL;
//        }
//        
//        throw dfe;
//    }
//    
//    return chartData;
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
CiftiParcelSeriesFile::loadAverageLineSeriesChartDataForSurfaceNodes(const StructureEnum::Enum structure,
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
CiftiParcelSeriesFile::loadLineSeriesChartDataForVoxelAtCoordinate(const float xyz[3])
{
    ChartDataCartesian* chartData = helpLoadChartDataForVoxelAtCoordinate(xyz);
    return chartData;
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
CiftiParcelSeriesFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                 SceneClass* sceneClass)
{
    CiftiMappableDataFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
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
CiftiParcelSeriesFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    CiftiMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
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

/**
 * Get the selected parcel label file used for reordering of parcels.
 *
 * @param compatibleParcelLabelFilesOut
 *    All Parcel Label files that are compatible with file implementing
 *    this interface.
 * @param selectedParcelLabelFileOut
 *    The selected parcel label file used for reordering the parcels.
 *    May be NULL!
 * @param selectedParcelLabelFileMapIndexOut
 *    Map index in the selected parcel label file.
 * @param enabledStatusOut
 *    Enabled status of reordering.
 */
void
CiftiParcelSeriesFile::getSelectedParcelLabelFileAndMapForReordering(std::vector<CiftiParcelLabelFile*>& compatibleParcelLabelFilesOut,
                                                                     CiftiParcelLabelFile* &selectedParcelLabelFileOut,
                                                                     int32_t& selectedParcelLabelFileMapIndexOut,
                                                                     bool& enabledStatusOut) const
{
    m_parcelReorderingModel->getSelectedParcelLabelFileAndMapForReordering(compatibleParcelLabelFilesOut,
                                                                           selectedParcelLabelFileOut,
                                                                           selectedParcelLabelFileMapIndexOut,
                                                                           enabledStatusOut);
}

/**
 * Set the selected parcel label file used for reordering of parcels.
 *
 * @param selectedParcelLabelFile
 *    The selected parcel label file used for reordering the parcels.
 *    May be NULL!
 * @param selectedParcelLabelFileMapIndex
 *    Map index in the selected parcel label file.
 * @param enabledStatus
 *    Enabled status of reordering.
 */
void
CiftiParcelSeriesFile::setSelectedParcelLabelFileAndMapForReordering(CiftiParcelLabelFile* selectedParcelLabelFile,
                                                                     const int32_t selectedParcelLabelFileMapIndex,
                                                                     const bool enabledStatus)
{
    m_parcelReorderingModel->setSelectedParcelLabelFileAndMapForReordering(selectedParcelLabelFile,
                                                                           selectedParcelLabelFileMapIndex,
                                                                           enabledStatus);
}

/**
 * Get the parcel reordering for the given map index that was created using
 * the given parcel label file and its map index.
 *
 * @param parcelLabelFile
 *    The selected parcel label file used for reordering the parcels.
 * @param parcelLabelFileMapIndex
 *    Map index in the selected parcel label file.
 * @return
 *    Pointer to parcel reordering or NULL if not found.
 */
const CiftiParcelReordering*
CiftiParcelSeriesFile::getParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                           const int32_t parcelLabelFileMapIndex) const
{
    return m_parcelReorderingModel->getParcelReordering(parcelLabelFile,
                                                        parcelLabelFileMapIndex);
}

/**
 * Create the parcel reordering for the given map index using
 * the given parcel label file and its map index.
 *
 * @param parcelLabelFile
 *    The selected parcel label file used for reordering the parcels.
 * @param parcelLabelFileMapIndex
 *    Map index in the selected parcel label file.
 * @param ciftiParcelsMap
 *    The CIFTI parcels map that will or has been reordered.
 * @param errorMessageOut
 *    Error message output.  Will only be non-empty if NULL is returned.
 * @return
 *    Pointer to parcel reordering or NULL if not found.
 */
bool
CiftiParcelSeriesFile::createParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                              const int32_t parcelLabelFileMapIndex,
                                              AString& errorMessageOut)
{
    return m_parcelReorderingModel->createParcelReordering(parcelLabelFile,
                                                           parcelLabelFileMapIndex,
                                                           errorMessageOut);
}


