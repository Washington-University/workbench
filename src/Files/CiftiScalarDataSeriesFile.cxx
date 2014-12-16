
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
#include "ChartMatrixDisplayProperties.h"
#include "CiftiFile.h"
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
        m_chartingEnabledForTab[i] = false;
        m_chartMatrixDisplayPropertiesForTab[i] = new ChartMatrixDisplayProperties();
        m_chartMatrixDisplayPropertiesForTab[i]->setGridLinesDisplayed(false);
        m_yokingGroupForTab[i] = OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF;
    }
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<OverlayYokingGroupEnum, OverlayYokingGroupEnum::Enum>("m_yokingGroupForTab",
                                                                                                             m_yokingGroupForTab);
    m_sceneAssistant->addTabIndexedBooleanArray("m_chartingEnabledForTab",
                                                m_chartingEnabledForTab);
}

/**
 * Destructor.
 */
CiftiScalarDataSeriesFile::~CiftiScalarDataSeriesFile()
{
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
    
}

/**
 * @param tabIndex
 *     Index of tab.
 * @return 
 *     Selected yoking group for the given tab.
 */
OverlayYokingGroupEnum::Enum
CiftiScalarDataSeriesFile::getYokingGroup(const int32_t tabIndex) const
{
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
CiftiScalarDataSeriesFile::setYokingGroup(const int32_t tabIndex,
                              const OverlayYokingGroupEnum::Enum yokingGroup)
{
    m_yokingGroupForTab[tabIndex] = yokingGroup;
    
    if (m_yokingGroupForTab[tabIndex] == OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF) {
        return;
    }
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
    if ((rowIndex >= 0)
        && (rowIndex < m_ciftiFile->getNumberOfRows())
        && (columnIndex >= 0)
        && (columnIndex < m_ciftiFile->getNumberOfColumns())) {
        const CiftiXML& xml = m_ciftiFile->getCiftiXML();
        
        const CiftiScalarsMap& scalarsMap = xml.getScalarsMap(CiftiXML::ALONG_COLUMN);
        CaretAssertArrayIndex(scalarsMap, scalarsMap.getLength(), rowIndex);
        rowNameOut = " ";
        
        const CiftiSeriesMap& seriesMap = xml.getSeriesMap(CiftiXML::ALONG_ROW);
        CaretAssertArrayIndex(seriesMap, seriesMap.getLength(), columnIndex);
        const float time = seriesMap.getStart() + seriesMap.getStep() * columnIndex;
        columnNameOut = (AString::number(time, 'f', 3)
                         + " "
                         + NiftiTimeUnitsEnum::toGuiName(getMapIntervalUnits())
                         + " Map Name: "
                         + getMapName(columnIndex));
        
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
CiftiScalarDataSeriesFile::getSupportedMatrixChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    chartDataTypesOut.push_back(ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES);
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


