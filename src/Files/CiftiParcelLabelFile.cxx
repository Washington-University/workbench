
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

#define __CIFTI_PARCEL_LABEL_FILE_DECLARE__
#include "CiftiParcelLabelFile.h"
#undef __CIFTI_PARCEL_LABEL_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartMatrixDisplayProperties.h"
#include "CiftiFile.h"
#include "CiftiParcelReordering.h"
#include "CiftiParcelReorderingModel.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "NodeAndVoxelColoring.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CiftiParcelLabelFile 
 * \brief CIFTI Parcellated Label File
 * \ingroup Files
 */

/**
 * Constructor.
 */
CiftiParcelLabelFile::CiftiParcelLabelFile()
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
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
CiftiParcelLabelFile::~CiftiParcelLabelFile()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_chartMatrixDisplayProperties[i];
    }
    
    delete m_parcelReorderingModel;
    delete m_sceneAssistant;
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
CiftiParcelLabelFile::getMatrixDimensions(int32_t& numberOfRowsOut,
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
CiftiParcelLabelFile::getMatrixDataRGBA(int32_t& numberOfRowsOut,
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
//    CaretAssert(m_ciftiFile);
//    
//    /*
//     * Dimensions of matrix.
//     */
//    numberOfRowsOut    = m_ciftiFile->getNumberOfRows();
//    numberOfColumnsOut = m_ciftiFile->getNumberOfColumns();
//    const int32_t numberOfData = numberOfRowsOut * numberOfColumnsOut;
//    if (numberOfData <= 0) {
//        return false;
//    }
//    
//    /*
//     * Allocate rgba output
//     */
//    const int32_t numberOfRgba = numberOfData * 4;
//    rgbaOut.resize(numberOfRgba);
//    
//    /*
//     * Get each column, color it using its label table, and then
//     * add the column's coloring into the output coloring.
//     */
//    std::vector<float> columnData(numberOfRowsOut);
//    std::vector<float> columnRGBA(numberOfRowsOut * 4);
//    for (int32_t iCol = 0; iCol < numberOfColumnsOut; iCol++) {
//        CaretAssertVectorIndex(m_mapContent, iCol);
//        m_ciftiFile->getColumn(&columnData[0],
//                               iCol);
//        const GiftiLabelTable* labelTable = getMapLabelTable(iCol);
//        NodeAndVoxelColoring::colorIndicesWithLabelTable(labelTable,
//                                                         &columnData[0],
//                                                         numberOfRowsOut,
//                                                         &columnRGBA[0]);
//        
//        for (int32_t iRow = 0; iRow < numberOfRowsOut; iRow++) {
//            const int32_t rgbaOffset = (((iRow * numberOfColumnsOut)
//                                        + iCol) * 4);
//            CaretAssertVectorIndex(rgbaOut, rgbaOffset + 3);
//            const int32_t columnRgbaOffset = (iRow * 4);
//            CaretAssertVectorIndex(columnRGBA, columnRgbaOffset + 3);
//            rgbaOut[rgbaOffset] = columnRGBA[columnRgbaOffset];
//            rgbaOut[rgbaOffset+1] = columnRGBA[columnRgbaOffset+1];
//            rgbaOut[rgbaOffset+2] = columnRGBA[columnRgbaOffset+2];
//            rgbaOut[rgbaOffset+3] = columnRGBA[columnRgbaOffset+3];
//        }
//    }
//        
//    return true;
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
CiftiParcelLabelFile::getMatrixCellAttributes(const int32_t rowIndex,
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
        
        CaretAssertVectorIndex(m_mapContent, columnIndex);
        columnNameOut = getMapName(columnIndex);
        
        const int32_t numberOfElementsInRow = m_ciftiFile->getNumberOfColumns();
        std::vector<float> rowData(numberOfElementsInRow);
        m_ciftiFile->getRow(&rowData[0],
                            rowIndex);
        CaretAssertVectorIndex(rowData, columnIndex);
        const int32_t labelKey = rowData[columnIndex];
        const GiftiLabel* label = getMapLabelTable(columnIndex)->getLabel(labelKey);
        if (label != NULL) {
            cellValueOut = ("(key="
                            + AString::number(labelKey)
                            + ")"
                            + label->getName());
        }
        else {
            cellValueOut = ("Invalid Key="
                            + AString::number(labelKey));
        }
        
        return true;
    }
    
    return false;
}

/**
 * @return Is charting enabled for this file?
 */
bool
CiftiParcelLabelFile::isMatrixChartingEnabled(const int32_t tabIndex) const
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
CiftiParcelLabelFile::isMatrixChartingSupported() const
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
CiftiParcelLabelFile::setMatrixChartingEnabled(const int32_t tabIndex,
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
CiftiParcelLabelFile::getSupportedMatrixChartDataTypes(std::vector<ChartVersionOneDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    chartDataTypesOut.push_back(ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER);
}

/**
 * @return Chart matrix display properties (const method).
 */
const ChartMatrixDisplayProperties*
CiftiParcelLabelFile::getChartMatrixDisplayProperties(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartMatrixDisplayProperties, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartMatrixDisplayProperties[tabIndex];
}

/**
 * @return Chart matrix display properties.
 */
ChartMatrixDisplayProperties*
CiftiParcelLabelFile::getChartMatrixDisplayProperties(const int32_t tabIndex)
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
CiftiParcelLabelFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                                       SceneClass* sceneClass)
{
    CiftiMappableDataFile::saveFileDataToScene(sceneAttributes,
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
CiftiParcelLabelFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                            const SceneClass* sceneClass)
{
    CiftiMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
    
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
CiftiParcelLabelFile::getSelectedParcelColoringMode() const
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
CiftiParcelLabelFile::setSelectedParcelColoringMode(const CiftiParcelColoringModeEnum::Enum coloringMode)
{
    m_selectedParcelColoringMode = coloringMode;
}

/**
 * @return Color for selected parcel.
 */
CaretColorEnum::Enum
CiftiParcelLabelFile::getSelectedParcelColor() const
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
CiftiParcelLabelFile::setSelectedParcelColor(const CaretColorEnum::Enum color)
{
    m_selectedParcelColor = color;
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
CiftiParcelLabelFile::getSelectedParcelLabelFileAndMapForReordering(std::vector<CiftiParcelLabelFile*>& compatibleParcelLabelFilesOut,
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
CiftiParcelLabelFile::setSelectedParcelLabelFileAndMapForReordering(CiftiParcelLabelFile* selectedParcelLabelFile,
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
CiftiParcelLabelFile::getParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
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
 * @param errorMessageOut
 *    Error message output.  Will only be non-empty if NULL is returned.
 * @return
 *    Pointer to parcel reordering or NULL if not found.
 */
bool
CiftiParcelLabelFile::createParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
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
CiftiParcelLabelFile::isSupportsLoadingAttributes()
{
    return false;
}

/**
 * @return The matrix loading type (by row/column).
 */
ChartMatrixLoadingDimensionEnum::Enum
CiftiParcelLabelFile::getMatrixLoadingDimension() const
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
CiftiParcelLabelFile::setMatrixLoadingDimension(const ChartMatrixLoadingDimensionEnum::Enum /* matrixLoadingType */)
{
    CaretLogSevere("Attempting to change matrix loading type for a file that only supports loading by column");
}

/**
 * @return Selected yoking group.
 */
YokingGroupEnum::Enum
CiftiParcelLabelFile::getYokingGroup() const
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
CiftiParcelLabelFile::setYokingGroup(const YokingGroupEnum::Enum /* yokingGroup */)
{
    /* not supported in this file */
}

/**
 * Reorder and map and return the matching parcel indices.
 *
 * @param mapIndex
 *    Index of the map.
 * @param reorderedParcelIndicesOut
 *    The parcel indices with reordering applied
 * @param errorMessageOut
 *    Contains description of error.
 * @return 
 *    True if reordering successful, otherwise, false is returned
 *    and errorMessageOut will contain a descrption of the error.
 */
bool
CiftiParcelLabelFile::getReorderedParcelIndicesFromMap(const int32_t mapIndex,
                                                       std::vector<int32_t>& reorderedParcelIndicesOut,
                                                       AString& errorMessageOut) const
{
    reorderedParcelIndicesOut.clear();
    errorMessageOut.clear();
 
    if ((mapIndex < 0)
        || (mapIndex >= getNumberOfMaps())) {
        errorMessageOut.appendWithNewLine("Invalid map index="
                                          + AString::number(mapIndex));
    }
    const int32_t numberOfRows = m_ciftiFile->getNumberOfRows();
    if (numberOfRows <= 0) {
        errorMessageOut.appendWithNewLine("File contains no rows.");
    }
    
    if ( ! errorMessageOut.isEmpty()) {
        return false;
    }
    
    std::vector<float> columnData(numberOfRows);
    m_ciftiFile->getColumn(&columnData[0],
                           mapIndex);
    
    CaretLogFine("Column values size="
                 + AString::number(columnData.size())
                 + ": "
                 + AString::fromNumbers(columnData, ","));
    
    std::vector<bool> indexProcessed(numberOfRows,
                                     false);
    
    /*
     * Reorder row indices so that identical values are grouped together
     */
    for (int32_t iRow = 0; iRow < numberOfRows; iRow++) {
        if ( ! indexProcessed[iRow]) {
            reorderedParcelIndicesOut.push_back(iRow);
            indexProcessed[iRow] = true;
            const int32_t valueI = columnData[iRow];
            
            for (int32_t jRow = (iRow + 1); jRow < numberOfRows; jRow++) {
                if ( ! indexProcessed[jRow]) {
                    if (valueI == static_cast<int32_t>(columnData[jRow])) {
                        reorderedParcelIndicesOut.push_back(jRow);
                        indexProcessed[jRow] = true;
                    }
                }
            }
        }
    }
    
    if (reorderedParcelIndicesOut.empty()) {
        errorMessageOut.appendWithNewLine("No parcel indices for reordering.");
        return false;
    }
    
    CaretLogFine("Reordered parcel indices (not values!) size="
                   + AString::number(reorderedParcelIndicesOut.size())
                   + ": "
                   + AString::fromNumbers(reorderedParcelIndicesOut, ","));
    
    return true;
}



