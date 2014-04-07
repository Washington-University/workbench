
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

#define __CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_DECLARE__
#include "CiftiConnectivityMatrixParcelFile.h"
#undef __CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_DECLARE__

#include "CaretLogger.h"
#include "ChartMatrixDisplayProperties.h"
#include "CiftiFacade.h"
#include "CiftiInterface.h"
#include "FastStatistics.h"
#include "NodeAndVoxelColoring.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CiftiConnectivityMatrixParcelFile 
 * \brief Connectivity Parcel x Parcel File
 * \ingroup Files
 *
 * Contains connectivity matrix that measures connectivity from parcels
 * to parcels.
 */

/**
 * Constructor.
 */
CiftiConnectivityMatrixParcelFile::CiftiConnectivityMatrixParcelFile()
: CiftiMappableConnectivityMatrixDataFile(DataFileTypeEnum::CONNECTIVITY_PARCEL,
                                          CiftiMappableDataFile::FILE_READ_DATA_ALL,
                                          CIFTI_INDEX_TYPE_PARCELS,
                                          CIFTI_INDEX_TYPE_PARCELS,
                                          CiftiMappableDataFile::DATA_ACCESS_WITH_ROW_METHODS,
                                          CiftiMappableDataFile::DATA_ACCESS_INVALID)
/*,ChartableBrainordinateInterface()*/
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
        m_chartMatrixDisplayProperties[i] = new ChartMatrixDisplayProperties();
    }
    
    m_selectedParcelColoringMode = CiftiParcelColoringModeEnum::CIFTI_PARCEL_COLORING_OUTLINE;
    m_selectedParcelColor = CaretColorEnum::WHITE;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<CiftiParcelColoringModeEnum, CiftiParcelColoringModeEnum::Enum>("m_selectedParcelColoringMode",
                                                                                          &m_selectedParcelColoringMode);
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_selectedParcelColor",
                                                                &m_selectedParcelColor);
}

/**
 * Destructor.
 */
CiftiConnectivityMatrixParcelFile::~CiftiConnectivityMatrixParcelFile()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_chartMatrixDisplayProperties[i];
    }
    
    delete m_sceneAssistant;
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
CiftiConnectivityMatrixParcelFile::getMatrixDataRGBA(int32_t& numberOfRowsOut,
                                                    int32_t& numberOfColumnsOut,
                                                    std::vector<float>& rgbaOut) const
{
    return helpLoadChartDataMatrixForMap(0,
                                         numberOfRowsOut,
                                         numberOfColumnsOut,
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
CiftiConnectivityMatrixParcelFile::getMatrixCellAttributes(const int32_t rowIndex,
                                                           const int32_t columnIndex,
                                                           float& cellValueOut,
                                                           AString& rowNameOut,
                                                           AString& columnNameOut) const
{
    if ((rowIndex >= 0)
        && (rowIndex < m_ciftiFacade->getNumberOfRows())
        && (columnIndex >= 0)
        && (columnIndex < m_ciftiFacade->getNumberOfColumns())) {
        const CiftiXML& xml = m_ciftiInterface->getCiftiXML();
        
        const std::vector<CiftiParcelsMap::Parcel>& rowsParcelsMap = xml.getParcelsMap(CiftiXML::ALONG_COLUMN).getParcels();
        CaretAssertVectorIndex(rowsParcelsMap, rowIndex);
        rowNameOut = rowsParcelsMap[rowIndex].m_name;
        
        const std::vector<CiftiParcelsMap::Parcel>& columnsParcelsMap = xml.getParcelsMap(CiftiXML::ALONG_ROW).getParcels();
        CaretAssertVectorIndex(columnsParcelsMap, columnIndex);
        columnNameOut = columnsParcelsMap[columnIndex].m_name;
        
        const int32_t numberOfElementsInRow = m_ciftiInterface->getNumberOfColumns();
        std::vector<float> rowData(numberOfElementsInRow);
        m_ciftiInterface->getRow(&rowData[0],
                                 rowIndex);
        CaretAssertVectorIndex(rowData, columnIndex);
        cellValueOut = rowData[columnIndex];
        
        return true;
    }
    
    return false;
}

/**
 * @return Is charting enabled for this file?
 */
bool
CiftiConnectivityMatrixParcelFile::isChartingEnabled(const int32_t tabIndex) const
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
CiftiConnectivityMatrixParcelFile::isChartingSupported() const
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
CiftiConnectivityMatrixParcelFile::setChartingEnabled(const int32_t tabIndex,
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
CiftiConnectivityMatrixParcelFile::getSupportedChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    chartDataTypesOut.push_back(ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX);
}

/**
 * @return Chart matrix display properties (const method).
 */
const ChartMatrixDisplayProperties*
CiftiConnectivityMatrixParcelFile::getChartMatrixDisplayProperties(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartMatrixDisplayProperties, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartMatrixDisplayProperties[tabIndex];
}

/**
 * @return Chart matrix display properties.
 */
ChartMatrixDisplayProperties*
CiftiConnectivityMatrixParcelFile::getChartMatrixDisplayProperties(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_chartMatrixDisplayProperties, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartMatrixDisplayProperties[tabIndex];
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
CiftiConnectivityMatrixParcelFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                                  SceneClass* sceneClass)
{
    CiftiMappableConnectivityMatrixDataFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    sceneClass->addBooleanArray("m_chartingEnabledForTab",
                                m_chartingEnabledForTab,
                                BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);

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
                                           m_chartMatrixDisplayProperties[tabIndex]->saveToScene(sceneAttributes,
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
CiftiConnectivityMatrixParcelFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                       const SceneClass* sceneClass)
{
    CiftiMappableConnectivityMatrixDataFile::restoreFileDataFromScene(sceneAttributes,
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
            m_chartMatrixDisplayProperties[tabIndex]->restoreFromScene(sceneAttributes,
                                                                       sceneClass);
        }
    }
    
}

/**
 * @return Coloring mode for selected parcel.
 */
CiftiParcelColoringModeEnum::Enum
CiftiConnectivityMatrixParcelFile::getSelectedParcelColoringMode() const
{
    return m_selectedParcelColoringMode;
}

/**
 * Set the coloring mode for selected parcel.
 *
 * @param coloringMode
 *    New value for coloring mode.
 */
void
CiftiConnectivityMatrixParcelFile::setSelectedParcelColoringMode(const CiftiParcelColoringModeEnum::Enum coloringMode)
{
    m_selectedParcelColoringMode = coloringMode;
}

/**
 * @return Color for selected parcel.
 */
CaretColorEnum::Enum
CiftiConnectivityMatrixParcelFile::getSelectedParcelColor() const
{
    return m_selectedParcelColor;
}

/**
 * Set color for selected parcel.
 *
 * @param color
 *    New color for selected parcel.
 */
void
CiftiConnectivityMatrixParcelFile::setSelectedParcelColor(const CaretColorEnum::Enum color)
{
    m_selectedParcelColor = color;
}


