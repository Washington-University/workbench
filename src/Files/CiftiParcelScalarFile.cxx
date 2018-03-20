
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

#define __CIFTI_PARCEL_SCALAR_FILE_DECLARE__
#include "CiftiParcelScalarFile.h"
#undef __CIFTI_PARCEL_SCALAR_FILE_DECLARE__

#include "CaretLogger.h"
#include "ChartDataCartesian.h"
#include "ChartMatrixDisplayProperties.h"
#include "CiftiFile.h"
#include "CiftiParcelReordering.h"
#include "CiftiParcelReorderingModel.h"
#include "CiftiXML.h"
#include "FastStatistics.h"
#include "NodeAndVoxelColoring.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CiftiParcelScalarFile 
 * \brief CIFTI Parcel by Scalar File
 * \ingroup Files
 *
 */

/**
 * Constructor.
 */
CiftiParcelScalarFile::CiftiParcelScalarFile()
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_brainordinateChartingEnabledForTab[i] = false;
        m_matrixChartingEnabledForTab[i] = false;
        m_chartMatrixDisplayProperties[i] = new ChartMatrixDisplayProperties();
    }
    
    m_selectedParcelColoringMode = CiftiParcelColoringModeEnum::CIFTI_PARCEL_COLORING_OUTLINE;
    m_selectedParcelColor = CaretColorEnum::WHITE;
    
    m_parcelReorderingModel = new CiftiParcelReorderingModel(this);
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<CiftiParcelColoringModeEnum, CiftiParcelColoringModeEnum::Enum>("m_selectedParcelColoringMode",
                                                                                          &m_selectedParcelColoringMode);
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_selectedParcelColor",
                                                                &m_selectedParcelColor);
    m_sceneAssistant->add("m_parcelReorderingModel",
                          "CiftiParcelReorderingModel",
                          m_parcelReorderingModel);
}

/**
 * Destructor.
 */
CiftiParcelScalarFile::~CiftiParcelScalarFile()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_chartMatrixDisplayProperties[i];
    }
    
    delete m_parcelReorderingModel;
    delete m_sceneAssistant;
}

/**
 * @return Is charting enabled for this file?
 */
bool
CiftiParcelScalarFile::isLineSeriesChartingEnabled(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_brainordinateChartingEnabledForTab[tabIndex];
}

/**
 * @return Return true if the file's current state supports
 * charting data, else false.  Typically a brainordinate file
 * is chartable if it contains more than one map.
 */
bool
CiftiParcelScalarFile::isLineSeriesChartingSupported() const
{
    if (getNumberOfMaps() > 1) {
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
CiftiParcelScalarFile::getSupportedLineSeriesChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const
{
    helpGetSupportedLineSeriesChartDataTypes(chartDataTypesOut);
}

/**
 * Set charting enabled for this file.
 *
 * @param enabled
 *    New status for charting enabled.
 */
void
CiftiParcelScalarFile::setLineSeriesChartingEnabled(const int32_t tabIndex,
                                          const bool enabled)
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_brainordinateChartingEnabledForTab[tabIndex] = enabled;
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
CiftiParcelScalarFile::loadLineSeriesChartDataForSurfaceNode(const StructureEnum::Enum structure,
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
CiftiParcelScalarFile::loadAverageLineSeriesChartDataForSurfaceNodes(const StructureEnum::Enum structure,
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
CiftiParcelScalarFile::loadLineSeriesChartDataForVoxelAtCoordinate(const float xyz[3])
{
    ChartDataCartesian* chartData = helpLoadChartDataForVoxelAtCoordinate(xyz);
    return chartData;
}

/**
 * Get the matrix dimensions.
 *
 * @param numberOfRowsOut
 *    Number of rows in the matrix.
 * @param numberOfColumnsOut
 *    Number of rows in the matrix.
 */
void
CiftiParcelScalarFile::getMatrixDimensions(int32_t& numberOfRowsOut,
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
CiftiParcelScalarFile::getMatrixDataRGBA(int32_t& numberOfRowsOut,
                                         int32_t& numberOfColumnsOut,
                                         std::vector<float>& rgbaOut) const
{
    CiftiParcelLabelFile* parcelLabelFile = NULL;
    int32_t parcelLabelFileMapIndex = -1;
    bool enabled = false;
    
    std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
    getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
                                                  parcelLabelFile,
                                                  parcelLabelFileMapIndex,
                                                  enabled);
    
    std::vector<int32_t> rowIndices;
    if (enabled) {
        const CiftiParcelReordering* parcelReordering = getParcelReordering(parcelLabelFile,
                                                                            parcelLabelFileMapIndex);
        if (parcelReordering != NULL) {
            rowIndices = parcelReordering->getReorderedParcelIndices();
        }
    }
    
    return helpMapFileLoadChartDataMatrixRGBA(numberOfRowsOut,
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
CiftiParcelScalarFile::getMatrixCellAttributes(const int32_t rowIndex,
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
        
        const std::vector<CiftiParcelsMap::Parcel>& rowsParcelsMap = xml.getParcelsMap(CiftiXML::ALONG_COLUMN).getParcels();
        CaretAssertVectorIndex(rowsParcelsMap, rowIndex);
        rowNameOut = rowsParcelsMap[rowIndex].m_name;
        
        columnNameOut = ("Map "
                         + AString::number(columnIndex + 1));
        
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
CiftiParcelScalarFile::isMatrixChartingEnabled(const int32_t tabIndex) const
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
CiftiParcelScalarFile::isMatrixChartingSupported() const
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
CiftiParcelScalarFile::setMatrixChartingEnabled(const int32_t tabIndex,
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
CiftiParcelScalarFile::getSupportedMatrixChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    chartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER);
}

/**
 * @return Chart matrix display properties (const method).
 */
const ChartMatrixDisplayProperties*
CiftiParcelScalarFile::getChartMatrixDisplayProperties(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartMatrixDisplayProperties, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartMatrixDisplayProperties[tabIndex];
}

/**
 * @return Chart matrix display properties.
 */
ChartMatrixDisplayProperties*
CiftiParcelScalarFile::getChartMatrixDisplayProperties(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_chartMatrixDisplayProperties, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartMatrixDisplayProperties[tabIndex];
}

/**
 * @return Coloring mode for selected parcel.
 */
CiftiParcelColoringModeEnum::Enum
CiftiParcelScalarFile::getSelectedParcelColoringMode() const
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
CiftiParcelScalarFile::setSelectedParcelColoringMode(const CiftiParcelColoringModeEnum::Enum coloringMode)
{
    m_selectedParcelColoringMode = coloringMode;
}

/**
 * @return Color for selected parcel.
 */
CaretColorEnum::Enum
CiftiParcelScalarFile::getSelectedParcelColor() const
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
CiftiParcelScalarFile::setSelectedParcelColor(const CaretColorEnum::Enum color)
{
    m_selectedParcelColor = color;
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
CiftiParcelScalarFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                                      SceneClass* sceneClass)
{
    CiftiMappableDataFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    sceneClass->addBooleanArray("m_brainordinateChartingEnabledForTab",
                                m_brainordinateChartingEnabledForTab,
                                BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    
    sceneClass->addBooleanArray("m_matrixChartingEnabledForTab",
                                m_matrixChartingEnabledForTab,
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
CiftiParcelScalarFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                           const SceneClass* sceneClass)
{
    CiftiMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);

    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    /*
     * Originally, charting was "per file": m_chartingEnabled
     * Later, charting became "per tab": m_chartingEnabledForTab
     * Even Later, it needed to clearly be for brainordinates: m_brainordinateChartingEnabledForTab
     */
    const ScenePrimitiveArray* brainChartingForTab = sceneClass->getPrimitiveArray("m_brainordinateChartingEnabledForTab");
    const ScenePrimitiveArray* oldChartingEnabledForTab = sceneClass->getPrimitiveArray("m_chartingEnabledForTab");
    if (brainChartingForTab != NULL) {
        sceneClass->getBooleanArrayValue("brainChartingForTab",
                                         m_brainordinateChartingEnabledForTab,
                                         BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    }
    else if (oldChartingEnabledForTab != NULL) {
        sceneClass->getBooleanArrayValue("m_chartingEnabledForTab",
                                         m_brainordinateChartingEnabledForTab,
                                         BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    }
    else {
        /*
         * Obsolete value when charting was not 'per tab'
         */
        const bool chartingEnabled = sceneClass->getBooleanValue("m_chartingEnabled",
                                                                 false);
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            m_brainordinateChartingEnabledForTab[i] = chartingEnabled;
        }
    }
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_matrixChartingEnabledForTab[i] = false;
        m_chartMatrixDisplayProperties[i]->resetPropertiesToDefault();
    }
    sceneClass->getBooleanArrayValue("m_matrixChartingEnabledForTab",
                                     m_matrixChartingEnabledForTab,
                                     BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    
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
CiftiParcelScalarFile::getSelectedParcelLabelFileAndMapForReordering(std::vector<CiftiParcelLabelFile*>& compatibleParcelLabelFilesOut,
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
CiftiParcelScalarFile::setSelectedParcelLabelFileAndMapForReordering(CiftiParcelLabelFile* selectedParcelLabelFile,
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
CiftiParcelScalarFile::getParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
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
CiftiParcelScalarFile::createParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                                          const int32_t parcelLabelFileMapIndex,
                                                          AString& errorMessageOut)
{
    return m_parcelReorderingModel->createParcelReordering(parcelLabelFile,
                                                           parcelLabelFileMapIndex,
                                                           errorMessageOut);
}


/**
 * @return True if loading attributes (column/row, yoking) are
 * supported by this file type.
 */
bool
CiftiParcelScalarFile::isSupportsLoadingAttributes()
{
    return false;
}

/**
 * @return The matrix loading type (by row/column).
 */
ChartMatrixLoadingDimensionEnum::Enum
CiftiParcelScalarFile::getMatrixLoadingDimension() const
{
    /*
     * This file supports loading by column only !
     */
    return ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN;
}

/**
 * Set the matrix loading type (by row/column).
 *
 * @param matrixLoadingType
 *    New value for matrix loading type.
 */
void
CiftiParcelScalarFile::setMatrixLoadingDimension(const ChartMatrixLoadingDimensionEnum::Enum /* matrixLoadingType */)
{
    CaretLogSevere("Attempting to change matrix loading type for a file that only supports loading by column");
}

/**
 * @return Selected yoking group.
 */
YokingGroupEnum::Enum
CiftiParcelScalarFile::getYokingGroup() const
{
    /* not supported in this file */
    return YokingGroupEnum::YOKING_GROUP_OFF;
}

/**
 * Set the selected yoking group.
 *
 * @param yokingGroup
 *    New value for yoking group.
 */
void
CiftiParcelScalarFile::setYokingGroup(const YokingGroupEnum::Enum /* yokingGroup */)
{
    /* not supported in this file */
}




