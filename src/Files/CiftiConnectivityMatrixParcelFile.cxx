
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
#include "CiftiFile.h"
#include "CiftiParcelReordering.h"
#include "CiftiParcelReorderingModel.h"
#include "ConnectivityDataLoaded.h"
#include "EventChartMatrixParcelYokingValidation.h"
#include "EventManager.h"
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
: CiftiMappableConnectivityMatrixDataFile(DataFileTypeEnum::CONNECTIVITY_PARCEL),
ChartableMatrixParcelInterface(),
EventListenerInterface()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
        m_chartMatrixDisplayProperties[i] = new ChartMatrixDisplayProperties();
    }
    
    m_selectedParcelColoringMode = CiftiParcelColoringModeEnum::CIFTI_PARCEL_COLORING_OUTLINE;
    m_selectedParcelColor = CaretColorEnum::WHITE;

    m_parcelReorderingModel = new CiftiParcelReorderingModel(this);

    m_chartLoadingYokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
    

    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<CiftiParcelColoringModeEnum, CiftiParcelColoringModeEnum::Enum>("m_selectedParcelColoringMode",
                                                                                          &m_selectedParcelColoringMode);
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_selectedParcelColor",
                                                                &m_selectedParcelColor);
    m_sceneAssistant->add("m_parcelReorderingModel",
                          "CiftiParcelReorderingModel",
                          m_parcelReorderingModel);
    m_sceneAssistant->add<YokingGroupEnum, YokingGroupEnum::Enum>("m_chartLoadingYokingGroup",
                                                                                        &m_chartLoadingYokingGroup);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_CHART_MATRIX_YOKING_VALIDATION);
}

/**
 * Destructor.
 */
CiftiConnectivityMatrixParcelFile::~CiftiConnectivityMatrixParcelFile()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_chartMatrixDisplayProperties[i];
    }
    
    delete m_parcelReorderingModel;
    delete m_sceneAssistant;
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
CiftiConnectivityMatrixParcelFile::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_CHART_MATRIX_YOKING_VALIDATION) {
        EventChartMatrixParcelYokingValidation* yokeEvent = dynamic_cast<EventChartMatrixParcelYokingValidation*>(event);
        CaretAssert(yokeEvent);
        
        if (yokeEvent->getChartableMatrixParcelInterface() != this) {
            switch (yokeEvent->getMode()) {
                case EventChartMatrixParcelYokingValidation::MODE_APPLY_YOKING:
                {
//                    YokingGroupEnum::Enum yokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
//                    int32_t rowOrColumnIndex = -1;
//                    yokeEvent->getApplyYokingSelections(yokingGroup,
//                                                        rowOrColumnIndex);
//                    if ((yokingGroup != YokingGroupEnum::YOKING_GROUP_OFF)
//                        && (rowOrColumnIndex >= 0)) {
//                        int32_t numRows = -1;
//                        int32_t numCols = -1;
//                        getMatrixDimensions(numRows,
//                                            numCols);
//                        switch (getMatrixLoadingDimension()) {
//                            case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
//                                if (rowOrColumnIndex < numCols) {
//                                    loadDataForColumnIndex(rowOrColumnIndex);
//                                }
//                                else {
//                                }
//                                break;
//                            case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
//                                if (rowOrColumnIndex < numRows) {
//                                    loadDataForRowIndex(rowOrColumnIndex);
//                                }
//                                break;
//                        }
//                    }
                }
                    break;
                case EventChartMatrixParcelYokingValidation::MODE_VALIDATE_YOKING:
                {
                    const ConnectivityDataLoaded* connData = getConnectivityDataLoaded();
                    int64_t rowIndex    = -1;
                    int64_t columnIndex = -1;
                    connData->getRowColumnLoading(rowIndex, columnIndex);
                    
                    int64_t selectedRowColumnIndex = -1;
                    if (rowIndex >= 0) {
                        selectedRowColumnIndex = rowIndex;
                    }
                    else if (columnIndex >= 0) {
                        selectedRowColumnIndex = columnIndex;
                    }
                    
                    yokeEvent->addValidateYokingChartableInterface(this,
                                                                   selectedRowColumnIndex);
                }
                    break;
            }
        }
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
CiftiConnectivityMatrixParcelFile::getMatrixDimensions(int32_t& numberOfRowsOut,
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
CiftiConnectivityMatrixParcelFile::getMatrixDataRGBA(int32_t& numberOfRowsOut,
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
CiftiConnectivityMatrixParcelFile::getMatrixCellAttributes(const int32_t rowIndex,
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
        
        const std::vector<CiftiParcelsMap::Parcel>& columnsParcelsMap = xml.getParcelsMap(CiftiXML::ALONG_ROW).getParcels();
        CaretAssertVectorIndex(columnsParcelsMap, columnIndex);
        columnNameOut = columnsParcelsMap[columnIndex].m_name;
        
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
CiftiConnectivityMatrixParcelFile::isMatrixChartingEnabled(const int32_t tabIndex) const
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
CiftiConnectivityMatrixParcelFile::isMatrixChartingSupported() const
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
CiftiConnectivityMatrixParcelFile::setMatrixChartingEnabled(const int32_t tabIndex,
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
CiftiConnectivityMatrixParcelFile::getSupportedMatrixChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    chartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER);
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
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
CiftiConnectivityMatrixParcelFile::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                              SceneClass* sceneClass)
{
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
 * Restore file data from the scene.
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
CiftiConnectivityMatrixParcelFile::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                   const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    //    CiftiMappableConnectivityMatrixDataFile::restoreFileDataFromScene(sceneAttributes,
    //                                                                      sceneClass);
    
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

///**
// * Save file data from the scene.  For subclasses that need to
// * save to a scene, this method should be overriden.  sceneClass
// * will be valid and any scene data should be added to it.
// *
// * @param sceneAttributes
// *    Attributes for the scene.  Scenes may be of different types
// *    (full, generic, etc) and the attributes should be checked when
// *    restoring the scene.
// *
// * @param sceneClass
// *     sceneClass to which data members should be added.
// */
//void
//CiftiConnectivityMatrixParcelFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
//                                                  SceneClass* sceneClass)
//{
//    CiftiMappableConnectivityMatrixDataFile::saveFileDataToScene(sceneAttributes,
//                                               sceneClass);
//    
//    m_sceneAssistant->saveMembers(sceneAttributes,
//                                  sceneClass);
//    
//    sceneClass->addBooleanArray("m_chartingEnabledForTab",
//                                m_chartingEnabledForTab,
//                                BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
//
//    /*
//     * Save chart matrix properties
//     */
//    SceneObjectMapIntegerKey* chartMatrixPropertiesMap = new SceneObjectMapIntegerKey("m_chartMatrixDisplayPropertiesMap",
//                                                                                      SceneObjectDataTypeEnum::SCENE_CLASS);
//    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
//    for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
//         tabIter != tabIndices.end();
//         tabIter++) {
//        const int32_t tabIndex = *tabIter;
//        
//        chartMatrixPropertiesMap->addClass(tabIndex,
//                                           m_chartMatrixDisplayProperties[tabIndex]->saveToScene(sceneAttributes,
//                                                                                                 "m_chartMatrixDisplayProperties"));
//    }
//    sceneClass->addChild(chartMatrixPropertiesMap);
//}
//
///**
// * Restore file data from the scene.  For subclasses that need to
// * restore from a scene, this method should be overridden. The scene class
// * will be valid and any scene data may be obtained from it.
// *
// * @param sceneAttributes
// *    Attributes for the scene.  Scenes may be of different types
// *    (full, generic, etc) and the attributes should be checked when
// *    restoring the scene.
// *
// * @param sceneClass
// *     sceneClass for the instance of a class that implements
// *     this interface.  Will NEVER be NULL.
// */
//void
//CiftiConnectivityMatrixParcelFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
//                                                       const SceneClass* sceneClass)
//{
//    CiftiMappableConnectivityMatrixDataFile::restoreFileDataFromScene(sceneAttributes,
//                                                    sceneClass);
//    
//    m_sceneAssistant->restoreMembers(sceneAttributes,
//                                     sceneClass);
//   
//    /*
//     * The chart loading type is restored by the scene assistant.
//     * Swap its value so that calling setMatrixLoadingDimension requires
//     * the value to change for it to have any affect including
//     * setting size of data.
//     */
//    switch (m_chartLoadingType) {
//        case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
//            m_chartLoadingType = ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW;
//            break;
//        case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
//            m_chartLoadingType = ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN;
//            break;
//    }
//    setMatrixLoadingDimension(m_chartLoadingType);
//    
////    CiftiMappableConnectivityMatrixDataFile::restoreFileDataFromScene(sceneAttributes,
////                                                                      sceneClass);
//    
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        m_chartingEnabledForTab[i] = false;
//    }
//    
//    const ScenePrimitiveArray* tabArray = sceneClass->getPrimitiveArray("m_chartingEnabledForTab");
//    if (tabArray != NULL) {
//        sceneClass->getBooleanArrayValue("m_chartingEnabledForTab",
//                                         m_chartingEnabledForTab,
//                                         BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
//    }
//    else {
//        /*
//         * Obsolete value when charting was not 'per tab'
//         */
//        const bool chartingEnabled = sceneClass->getBooleanValue("m_chartingEnabled",
//                                                                 false);
//        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//            m_chartingEnabledForTab[i] = chartingEnabled;
//        }
//    }
//    
//    /*
//     * Restore chart matrix properties
//     */
//    const SceneObjectMapIntegerKey* chartMatrixPropertiesMap = sceneClass->getMapIntegerKey("m_chartMatrixDisplayPropertiesMap");
//    if (chartMatrixPropertiesMap != NULL) {
//        const std::vector<int32_t> tabIndices = chartMatrixPropertiesMap->getKeys();
//        for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
//             tabIter != tabIndices.end();
//             tabIter++) {
//            const int32_t tabIndex = *tabIter;
//            const SceneClass* sceneClass = chartMatrixPropertiesMap->classValue(tabIndex);
//            m_chartMatrixDisplayProperties[tabIndex]->restoreFromScene(sceneAttributes,
//                                                                       sceneClass);
//        }
//    }
//    
//}

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

/**
 * Get the selected parcel label file used for reordering of parcels.
 *
 * @param compatibleParcelLabelFilesOut
 *    All Parcel Label files that are compatible with file implementing
 *    this interface
 * @param selectedParcelLabelFileOut
 *    The selected parcel label file used for reordering the parcels.
 *    May be NULL!
 * @param selectedParcelLabelFileMapIndexOut
 *    Map index in the selected parcel label file.
 * @param enabledStatusOut
 *    Enabled status of reordering.
 */
void
CiftiConnectivityMatrixParcelFile::getSelectedParcelLabelFileAndMapForReordering(std::vector<CiftiParcelLabelFile*>& compatibleParcelLabelFilesOut,
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
CiftiConnectivityMatrixParcelFile::setSelectedParcelLabelFileAndMapForReordering(CiftiParcelLabelFile* selectedParcelLabelFile,
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
CiftiConnectivityMatrixParcelFile::getParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
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
CiftiConnectivityMatrixParcelFile::createParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
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
CiftiConnectivityMatrixParcelFile::isSupportsLoadingAttributes()
{
    return true;
}

/**
 * @return The matrix loading type (by row/column).
 */
ChartMatrixLoadingDimensionEnum::Enum
CiftiConnectivityMatrixParcelFile::getMatrixLoadingDimension() const
{
    return getChartMatrixLoadingDimension();
}

/**
 * Set the matrix loading type (by row/column).
 *
 * @param matrixLoadingType
 *    New value for matrix loading type.
 */
void
CiftiConnectivityMatrixParcelFile::setMatrixLoadingDimension(const ChartMatrixLoadingDimensionEnum::Enum matrixLoadingType)
{
    /*
     * Ignore when the loading dimension does not change
     */
    if (matrixLoadingType != getMatrixLoadingDimension()) {
        setChartMatrixLoadingDimension(matrixLoadingType);
    }
}

/**
 * @return Selected yoking group.
 */
YokingGroupEnum::Enum
CiftiConnectivityMatrixParcelFile::getYokingGroup() const
{
    return m_chartLoadingYokingGroup;
}

/**
 * Set the selected yoking group.
 *
 * @param yokingGroup
 *    New value for yoking group.
 */
void
CiftiConnectivityMatrixParcelFile::setYokingGroup(const YokingGroupEnum::Enum yokingGroup)
{
    m_chartLoadingYokingGroup = yokingGroup;
    
    if (m_chartLoadingYokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return;
    }
    
    /* 
     * Updated selected row/column to match yoking.
     */
}

