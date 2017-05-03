
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __CIFTI_SCALAR_DATA_SERIES_FILE_DECLARE__
#include "CiftiScalarDataSeriesFile.h"
#undef __CIFTI_SCALAR_DATA_SERIES_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartDataCartesian.h"
#include "ChartMatrixDisplayProperties.h"
#include "CiftiFile.h"
#include "CiftiSeriesMap.h"
#include "CiftiXML.h"
#include "DataFileException.h"
#include "EventBrowserTabIndicesGetAll.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventMapYokingValidation.h"
#include "FastStatistics.h"
#include "NodeAndVoxelColoring.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CiftiScalarDataSeriesFile 
 * \brief CIFTI Scalar Data Series File
 * \ingroup Files
 */

/**
 * Constructor.
 */
CiftiScalarDataSeriesFile::CiftiScalarDataSeriesFile()
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_lineSeriesChartingEnabledForTab[i] = false;
        m_matrixChartingEnabledForTab[i] = false;
        m_chartMatrixDisplayPropertiesForTab[i] = new ChartMatrixDisplayProperties();
        m_chartMatrixDisplayPropertiesForTab[i]->setGridLinesDisplayed(false);
        m_yokingGroupForTab[i] = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
        m_selectedMapIndices[i] = 0;
    }
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<MapYokingGroupEnum, MapYokingGroupEnum::Enum>("m_yokingGroupForTab",
                                                                                                     m_yokingGroupForTab);
    m_sceneAssistant->addTabIndexedBooleanArray("m_lineSeriesChartingEnabledForTab",
                                                m_lineSeriesChartingEnabledForTab);
    
    m_sceneAssistant->addTabIndexedBooleanArray("m_matrixChartingEnabledForTab",
                                                m_matrixChartingEnabledForTab);
    
    m_sceneAssistant->addTabIndexedIntegerArray("m_selectedMapIndices",
                                                m_selectedMapIndices);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_VALIDATION);
}

/**
 * Destructor.
 */
CiftiScalarDataSeriesFile::~CiftiScalarDataSeriesFile()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_chartMatrixDisplayPropertiesForTab[i];
    }
    
    delete m_sceneAssistant;
}

/**
 * Receive an event.
 *
 * @param event
 *    The event.
 */
void
CiftiScalarDataSeriesFile::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_VALIDATION) {
        EventMapYokingValidation* yokeMapEvent = dynamic_cast<EventMapYokingValidation*>(event);
        CaretAssert(yokeMapEvent);
        
        yokeMapEvent->addMapYokedFileAllTabs(this,
                                             m_yokingGroupForTab);
        
        yokeMapEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP) {
        /*
         * The events intended for overlays are received here so that
         * only DISPLAYED overlays are updated.
         */
        EventMapYokingSelectMap* selectMapEvent = dynamic_cast<EventMapYokingSelectMap*>(event);
        CaretAssert(selectMapEvent);
        
        const MapYokingGroupEnum::Enum mapYokingGroup = selectMapEvent->getMapYokingGroup();
        if (mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            const int32_t yokedMapIndex = selectMapEvent->getMapIndex();
            
            EventBrowserTabIndicesGetAll tabIndicesEvent;
            EventManager::get()->sendEvent(tabIndicesEvent.getPointer());
            const std::vector<int32_t> tabIndices = tabIndicesEvent.getAllBrowserTabIndices();
            for (std::vector<int32_t>::const_iterator iter = tabIndices.begin();
                 iter != tabIndices.end();
                 iter++) {
                const int32_t tabIndex = *iter;
                
                if (getMatrixRowColumnMapYokingGroup(tabIndex) == mapYokingGroup) {
                    setSelectedMapIndex(tabIndex, yokedMapIndex);
                }
            }
        }
        
        selectMapEvent->setEventProcessed();
    }
    
    CiftiMappableDataFile::receiveEvent(event);
}

/**
 * @param tabIndex
 *     Index of tab.
 * @return 
 *     Selected yoking group for the given tab.
 */
MapYokingGroupEnum::Enum
CiftiScalarDataSeriesFile::getMatrixRowColumnMapYokingGroup(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_yokingGroupForTab, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_yokingGroupForTab[tabIndex];
}

/**
 * Set the selected yoking group for the given tab.
 *
 * @param tabIndex
 *     Index of tab.
 * @param yokingGroup
 *    New value for yoking group.
 */
void
CiftiScalarDataSeriesFile::setMatrixRowColumnMapYokingGroup(const int32_t tabIndex,
                              const MapYokingGroupEnum::Enum yokingGroup)
{
    CaretAssertArrayIndex(m_yokingGroupForTab, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_yokingGroupForTab[tabIndex] = yokingGroup;
    
    if (m_yokingGroupForTab[tabIndex] == MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        return;
    }
}

/**
 * @param tabIndex
 *     Index of tab.
 * @return
 *     Selected map index in the given tab.
 */
int32_t 
CiftiScalarDataSeriesFile::getSelectedMapIndex(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_selectedMapIndices, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_selectedMapIndices[tabIndex];
}

/**
 * Set the selected map index for the given tab.
 *
 * @param tabIndex
 *     Index of tab.
 * @param mapIndex
 *    New value for selected map index.
 */
void
CiftiScalarDataSeriesFile::setSelectedMapIndex(const int32_t tabIndex,
                                 const int32_t mapIndex)
{
    CaretAssertArrayIndex(m_selectedMapIndices, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_selectedMapIndices[tabIndex] = mapIndex;
}



/**
 * Get the matrix dimensions.
 *
 * @param numberOfRowsOut
 *    Number of rows in the matrix.
 * @param numberOfColumnsOut
 *    Number of columns in the matrix.
 */
void
CiftiScalarDataSeriesFile::getMatrixDimensions(int32_t& numberOfRowsOut,
                                                       int32_t& numberOfColumnsOut) const
{
    helpMapFileGetMatrixDimensions(numberOfRowsOut,
                                   numberOfColumnsOut);
}

/**
 * Get the matrix RGBA coloring for this matrix data creator.
 *
 * @param numberOfRowsOut
 *    Number of rows in the coloring matrix.
 * @param numberOfColumnsOut
 *    Number of rows in the coloring matrix.
 * @param rgbaOut
 *    RGBA coloring output with number of elements
 *    (numberOfRowsOut * numberOfColumnsOut * 4).
 * @return
 *    True if data output data is valid, else false.
 */
bool
CiftiScalarDataSeriesFile::getMatrixDataRGBA(int32_t& numberOfRowsOut,
                                                     int32_t& numberOfColumnsOut,
                                                     std::vector<float>& rgbaOut) const
{
    std::vector<int32_t> rowIndices;
    return helpMatrixFileLoadChartDataMatrixRGBA(numberOfRowsOut,
                                                 numberOfColumnsOut,
                                                 rowIndices,
                                                 rgbaOut);
}

/**
 * Get the value, row name, and column name for a cell in the matrix.
 *
 * @param rowIndex
 *     The row index.
 * @param columnIndex
 *     The column index.
 * @param cellValueOut
 *     Output containing value in the cell.
 * @param rowNameOut
 *     Name of row corresponding to row index.
 * @param columnNameOut
 *     Name of column corresponding to column index.
 * @return
 *     True if the output values are valid (valid row/column indices).
 */
bool
CiftiScalarDataSeriesFile::getMatrixCellAttributes(const int32_t rowIndex,
                                                           const int32_t columnIndex,
                                                           AString& cellValueOut,
                                                           AString& rowNameOut,
                                                           AString& columnNameOut) const
{
    rowNameOut    = " ";
    columnNameOut = " ";
    
    if ((rowIndex >= 0)
        && (rowIndex < m_ciftiFile->getNumberOfRows())
        && (columnIndex >= 0)
        && (columnIndex < m_ciftiFile->getNumberOfColumns())) {
        const CiftiXML& xml = m_ciftiFile->getCiftiXML();
        
        const CiftiScalarsMap& scalarsMap = xml.getScalarsMap(CiftiXML::ALONG_COLUMN);
        CaretAssertArrayIndex(scalarsMap, scalarsMap.getLength(), rowIndex);
        rowNameOut = scalarsMap.getMapName(rowIndex);
        
        const CiftiSeriesMap& seriesMap = xml.getSeriesMap(CiftiXML::ALONG_ROW);
        CaretAssertArrayIndex(seriesMap, seriesMap.getLength(), columnIndex);
        const float time = seriesMap.getStart() + seriesMap.getStep() * columnIndex;
        
        AString timeUnitsString;
        switch (seriesMap.getUnit()) {
            case CiftiSeriesMap::HERTZ:
                timeUnitsString = NiftiTimeUnitsEnum::toGuiName(NiftiTimeUnitsEnum::NIFTI_UNITS_HZ);
                break;
            case CiftiSeriesMap::METER:
                CaretLogWarning("CIFTI Units METER not implemented");
                break;
            case CiftiSeriesMap::RADIAN:
                CaretLogWarning("CIFTI Units RADIAN not implemented");
                break;
            case CiftiSeriesMap::SECOND:
                timeUnitsString = NiftiTimeUnitsEnum::toGuiName(NiftiTimeUnitsEnum::NIFTI_UNITS_SEC);
                break;
        }

        columnNameOut = (AString::number(time, 'f', 3)
                         + " "
                         + timeUnitsString);
//        columnNameOut = (AString::number(time, 'f', 3)
//                         + " "
//                         + NiftiTimeUnitsEnum::toGuiName(getMapIntervalUnits())
//                         + " Map Name: "
//                         + getMapName(columnIndex));
        
        const int32_t numberOfElementsInRow = m_ciftiFile->getNumberOfColumns();
        std::vector<float> rowData(numberOfElementsInRow);
        m_ciftiFile->getRow(&rowData[0],
                            rowIndex);
        CaretAssertVectorIndex(rowData, columnIndex);
        cellValueOut = AString::number(rowData[columnIndex], 'f', 6);
        
        return true;
    }
    
    return false;
}

/**
 * @return Is charting enabled for this file?
 */
bool
CiftiScalarDataSeriesFile::isMatrixChartingEnabled(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_matrixChartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_matrixChartingEnabledForTab[tabIndex];
}

/**
 * @return Return true if the file's current state supports
 * charting data, else false.  Typically a brainordinate file
 * is chartable if it contains more than one map.
 */
bool
CiftiScalarDataSeriesFile::isMatrixChartingSupported() const
{
    return true;
}

/**
 * Set charting enabled for this file.
 *
 * @param enabled
 *    New status for charting enabled.
 */
void
CiftiScalarDataSeriesFile::setMatrixChartingEnabled(const int32_t tabIndex,
                                                            const bool enabled)
{
    CaretAssertArrayIndex(m_matrixChartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_matrixChartingEnabledForTab[tabIndex] = enabled;
}

/**
 * Get chart data types supported by the file.
 *
 * @param chartDataTypesOut
 *    Chart types supported by this file.
 */
void
CiftiScalarDataSeriesFile::getSupportedMatrixChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    chartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES);
}

/**
 * @return Chart matrix display properties (const method).
 */
const ChartMatrixDisplayProperties*
CiftiScalarDataSeriesFile::getChartMatrixDisplayProperties(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartMatrixDisplayProperties, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartMatrixDisplayPropertiesForTab[tabIndex];
}

/**
 * @return Chart matrix display properties.
 */
ChartMatrixDisplayProperties*
CiftiScalarDataSeriesFile::getChartMatrixDisplayProperties(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_chartMatrixDisplayProperties, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartMatrixDisplayPropertiesForTab[tabIndex];
}

/**
 * Load charting data for the given column index.
 *
 * @param columnIndex
 *     Index of the column.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will be NULL.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
CiftiScalarDataSeriesFile::loadLineSeriesChartDataForColumn(const int32_t /*columnIndex*/)
{
    CaretAssertMessage(0, "Loading of columns is not used at this time");
    return NULL;
}

/**
 * Load charting data for the given row index.
 *
 * @param rowIndex
 *     Index of the row.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will be NULL.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
CiftiScalarDataSeriesFile::loadLineSeriesChartDataForRow(const int32_t rowIndex)
{
    ChartDataCartesian* chartData = NULL;
    
    try {
        if ((rowIndex >= 0)
            && (rowIndex < m_ciftiFile->getNumberOfRows())) {
            const int32_t numberOfElementsInRow = m_ciftiFile->getNumberOfColumns();
            if (numberOfElementsInRow > 0) {
                std::vector<float> rowData(numberOfElementsInRow);
                m_ciftiFile->getRow(&rowData[0],
                                    rowIndex);
                
                const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
                const CiftiSeriesMap& seriesMap = ciftiXML.getSeriesMap(CiftiXML::ALONG_ROW);
                
                ChartOneDataTypeEnum::Enum chartDataType = ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID;
                AString timeUnitsString;
                switch (seriesMap.getUnit()) {
                    case CiftiSeriesMap::HERTZ:
                        chartDataType = ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES;
                        break;
                    case CiftiSeriesMap::METER:
                        CaretLogWarning("CIFTI Units METER not implemented");
                        break;
                    case CiftiSeriesMap::RADIAN:
                        CaretLogWarning("CIFTI Units RADIAN not implemented");
                        break;
                    case CiftiSeriesMap::SECOND:
                        chartDataType = ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES;
                        break;
                }
                
                if (chartDataType != ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                    chartData = new ChartDataCartesian(chartDataType,
                                                       ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
                                                       ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
                    
                    if (chartData != NULL) {
                        float timeStart = seriesMap.getStart();
                        float timeStep  = seriesMap.getStep();
                        chartData->setTimeStartInSecondsAxisX(timeStart);
                        chartData->setTimeStepInSecondsAxisX(timeStep);
                        
                        for (int64_t i = 0; i < numberOfElementsInRow; i++) {
                            const float xValue = timeStart + (i * timeStep);
                            
                            chartData->addPoint(xValue,
                                                rowData[i]);
                        }
                    }
                }
            }
        }
    }
    catch (const DataFileException& dfe) {
        if (chartData != NULL) {
            delete chartData;
            chartData = NULL;
        }
        
        throw dfe;
    }
    
    return chartData;
}

/**
 * @return Is charting enabled for this file?
 */
bool
CiftiScalarDataSeriesFile::isLineSeriesChartingEnabled(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_lineSeriesChartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_lineSeriesChartingEnabledForTab[tabIndex];
}

/**
 * @return Return true if the file's current state supports
 * charting data, else false.  Typically a brainordinate file
 * is chartable if it contains more than one map.
 */
bool
CiftiScalarDataSeriesFile::isLineSeriesChartingSupported() const
{
    if ((m_ciftiFile->getNumberOfColumns() > 0)
        && (m_ciftiFile->getNumberOfRows() > 0)) {
        return true;
    }
    
    return false;
}

/**
 * Get chart data types supported by the file.
 *
 * @param chartDataTypesOut
 *    Chart types supported by this file.
 */
void
CiftiScalarDataSeriesFile::getSupportedLineSeriesChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    
    const CiftiXML& ciftiXML = m_ciftiFile->getCiftiXML();
    CiftiMappingType::MappingType mapType = ciftiXML.getMappingType(CiftiXML::ALONG_ROW);
    const AString message("Mapping type should always be SERIES for CIFTI Scalar Data Series File");
    CaretAssertMessage(mapType == CiftiMappingType::SERIES,
                       message);
    if (mapType == CiftiMappingType::SERIES) {
        const CiftiSeriesMap& seriesMap = ciftiXML.getSeriesMap(CiftiXML::ALONG_ROW);
        switch (seriesMap.getUnit()) {
            case CiftiSeriesMap::HERTZ:
                chartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES);
                break;
            case CiftiSeriesMap::METER:
                CaretLogWarning("CIFTI Units METER not implemented");
                break;
            case CiftiSeriesMap::RADIAN:
                CaretLogWarning("CIFTI Units RADIAN not implemented");
                break;
            case CiftiSeriesMap::SECOND:
                chartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES);
                break;
        }
    }
    else {
        CaretLogSevere(message);
    }
}

/**
 * Set charting enabled for this file.
 *
 * @param enabled
 *    New status for charting enabled.
 */
void
CiftiScalarDataSeriesFile::setLineSeriesChartingEnabled(const int32_t tabIndex,
                                                    const bool enabled)
{
    CaretAssertArrayIndex(m_lineSeriesChartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_lineSeriesChartingEnabledForTab[tabIndex] = enabled;
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
CiftiScalarDataSeriesFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                                             SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    /*
     * Save chart matrix properties
     */
    SceneObjectMapIntegerKey* chartMatrixPropertiesMap = new SceneObjectMapIntegerKey("m_chartMatrixDisplayPropertiesMap",
                                                                                      SceneObjectDataTypeEnum::SCENE_CLASS);
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
         tabIter != tabIndices.end();
         tabIter++) {
        const int32_t tabIndex = *tabIter;
        
        chartMatrixPropertiesMap->addClass(tabIndex,
                                           m_chartMatrixDisplayPropertiesForTab[tabIndex]->saveToScene(sceneAttributes,
                                                                                                 "m_chartMatrixDisplayProperties"));
    }
    sceneClass->addChild(chartMatrixPropertiesMap);
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
CiftiScalarDataSeriesFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                                  const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    /*
     * Restore chart matrix properties
     */
    const SceneObjectMapIntegerKey* chartMatrixPropertiesMap = sceneClass->getMapIntegerKey("m_chartMatrixDisplayPropertiesMap");
    if (chartMatrixPropertiesMap != NULL) {
        const std::vector<int32_t> tabIndices = chartMatrixPropertiesMap->getKeys();
        for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
             tabIter != tabIndices.end();
             tabIter++) {
            const int32_t tabIndex = *tabIter;
            const SceneClass* sceneClass = chartMatrixPropertiesMap->classValue(tabIndex);
            m_chartMatrixDisplayPropertiesForTab[tabIndex]->restoreFromScene(sceneAttributes,
                                                                       sceneClass);
        }
    }
}


