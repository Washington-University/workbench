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

#include <algorithm>
#include <cmath>

#include "AnnotationColorBar.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretLogger.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableMatrixParcelInterface.h"
#include "ChartableMatrixSeriesInterface.h"
#include "ChartMatrixDisplayProperties.h"
#include "ChartableTwoFileMatrixChart.h"
#include "ChartTwoMatrixDisplayProperties.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoOverlaySet.h"
#include "ChartTwoOverlaySetArray.h"
#include "ChartingVersionEnum.h"
#include "ConnectivityDataLoaded.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiMappableDataFile.h"
#include "CiftiScalarDataSeriesFile.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserTabGetAll.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "EventNodeIdentificationColorsGetFromCharts.h"
#include "ModelChart.h"
#include "ModelChartTwo.h"
#include "OverlaySet.h"
#include "OverlaySetArray.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * Constructor.
 *
 */
ModelChartTwo::ModelChartTwo(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_CHART_TWO,
                         brain)
{
    std::vector<StructureEnum::Enum> overlaySurfaceStructures;
    m_overlaySetArray = new OverlaySetArray(overlaySurfaceStructures,
                                            Overlay::INCLUDE_VOLUME_FILES_YES,
                                            "Chart View");
    
    m_histogramChartOverlaySetArray = std::unique_ptr<ChartTwoOverlaySetArray>(new ChartTwoOverlaySetArray(ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM,
                                                                                                     "Histogram Chart Overlays"));
    m_matrixChartOverlaySetArray = std::unique_ptr<ChartTwoOverlaySetArray>(new ChartTwoOverlaySetArray(ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX,
                                                                                                     "Matrix Chart Overlays"));
    m_lineSeriesChartOverlaySetArray = std::unique_ptr<ChartTwoOverlaySetArray>(new ChartTwoOverlaySetArray(ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES,
                                                                                                     "Line Series Chart Overlays"));

    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartTwoMatrixDisplayProperties[i] = new ChartTwoMatrixDisplayProperties();
    }
    
    initializeCharts();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<ChartTwoDataTypeEnum, ChartTwoDataTypeEnum::Enum>("m_selectedChartTwoDataType",
                                                                                                         m_selectedChartTwoDataType);
    m_sceneAssistant->add("m_histogramChartOverlaySetArray",
                          "ChartTwoOverlaySetArray",
                          m_histogramChartOverlaySetArray.get());
    m_sceneAssistant->add("m_matrixChartOverlaySetArray",
                          "ChartTwoOverlaySetArray",
                          m_matrixChartOverlaySetArray.get());
    m_sceneAssistant->add("m_lineSeriesChartOverlaySetArray",
                          "ChartTwoOverlaySetArray",
                          m_lineSeriesChartOverlaySetArray.get());
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS);
}

/**
 * Destructor
 */
ModelChartTwo::~ModelChartTwo()
{
    delete m_overlaySetArray;
    EventManager::get()->removeAllEventsFromListener(this);
    
    removeAllCharts();    

    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_chartTwoMatrixDisplayProperties[i];
        m_chartTwoMatrixDisplayProperties[i] = NULL;
    }
}

void
ModelChartTwo::initializeCharts()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectedChartTwoDataType[i] = ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID;
    }
}

/**
 * Reset this model.
 */
void
ModelChartTwo::reset()
{
    removeAllCharts();
    
    initializeCharts();
}

/**
 * Remove all of the charts.
 */
void
ModelChartTwo::removeAllCharts()
{
}


/**
 * Load chart data for CIFTI Map files yoked to the given yoking group.
 *
 * @param mapYokingGroup
 *     The map yoking group.
 * @param mapIndex
 *     The map index.
 */
void
ModelChartTwo::loadChartDataForYokedCiftiMappableFiles(const MapYokingGroupEnum::Enum mapYokingGroup,
                                                    const int32_t mapIndex)
{
    if (mapYokingGroup == MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        return;
    }
    
    CaretAssertToDoWarning();
    
//    std::map<ChartableLineSeriesRowColumnInterface*, std::vector<int32_t> > chartFileEnabledTabs;
//    getTabsAndRowColumnChartFilesForLineChartLoading(chartFileEnabledTabs);
//    
//    for (std::map<ChartableLineSeriesRowColumnInterface*, std::vector<int32_t> >::iterator fileTabIter = chartFileEnabledTabs.begin();
//         fileTabIter != chartFileEnabledTabs.end();
//         fileTabIter++) {
//        ChartableLineSeriesRowColumnInterface* chartFile = fileTabIter->first;
//        CaretAssert(chartFile);
//        CiftiScalarDataSeriesFile* csdsf = dynamic_cast<CiftiScalarDataSeriesFile*>(chartFile);
//        if (csdsf != NULL) {
//            
//            std::vector<int32_t> matchedTabIndices;
//            const std::vector<int32_t>  tabIndices = fileTabIter->second;
//            for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
//                 tabIter != tabIndices.end();
//                 tabIter++) {
//                const int32_t tabIndex = *tabIter;
//                if (csdsf->getMatrixRowColumnMapYokingGroup(tabIndex) == mapYokingGroup) {
//                    matchedTabIndices.push_back(tabIndex);
//                }
//            }
//            
//            if ( ! matchedTabIndices.empty()) {
//                ChartData* chartData = chartFile->loadLineSeriesChartDataForRow(mapIndex);
//                if (chartData != NULL) {
//                    ChartDataSource* dataSource = chartData->getChartDataSource();
//                    dataSource->setFileRow(chartFile->getLineSeriesChartCaretMappableDataFile()->getFileName(),
//                                           mapIndex);
//                    
//                    addChartToChartModels(matchedTabIndices,
//                                          chartData);
//                }
//            }
//        }
//    }
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
ModelChartTwo::loadChartDataForCiftiMappableFileRow(CiftiMappableDataFile* ciftiMapFile,
                                                 const int32_t rowIndex)
{
//    CaretAssert(ciftiMapFile);
//
//    std::map<ChartableLineSeriesRowColumnInterface*, std::vector<int32_t> > chartFileEnabledTabs;
//    getTabsAndRowColumnChartFilesForLineChartLoading(chartFileEnabledTabs);
//    
//    for (std::map<ChartableLineSeriesRowColumnInterface*, std::vector<int32_t> >::iterator fileTabIter = chartFileEnabledTabs.begin();
//         fileTabIter != chartFileEnabledTabs.end();
//         fileTabIter++) {
//        ChartableLineSeriesRowColumnInterface* chartFile = fileTabIter->first;
//        if (ciftiMapFile == dynamic_cast<CiftiMappableDataFile*>(chartFile)) {
//            const std::vector<int32_t>  tabIndices = fileTabIter->second;
//            
//            CaretAssert(chartFile);
//            ChartData* chartData = chartFile->loadLineSeriesChartDataForRow(rowIndex);
//            if (chartData != NULL) {
//                ChartDataSource* dataSource = chartData->getChartDataSource();
//                dataSource->setFileRow(chartFile->getLineSeriesChartCaretMappableDataFile()->getFileName(),
//                                       rowIndex);
//                
//                addChartToChartModels(tabIndices,
//                                      chartData);
//            }
//        }
//    }
}
/**
 * Receive an event.
 * 
 * @param event
 *     The event that the receive can respond to.
 */
void 
ModelChartTwo::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS) {
        EventNodeIdentificationColorsGetFromCharts* nodeChartID =
           dynamic_cast<EventNodeIdentificationColorsGetFromCharts*>(event);
        CaretAssert(nodeChartID);
        
        EventBrowserTabGetAll allTabsEvent;
        EventManager::get()->sendEvent(allTabsEvent.getPointer());
        std::vector<int32_t> validTabIndices = allTabsEvent.getBrowserTabIndices();
        
        
        const AString structureName = nodeChartID->getStructureName();
        
        std::vector<ChartDataCartesian*> cartesianChartData;
        
//        for (std::list<QWeakPointer<ChartDataCartesian> >::iterator dsIter = m_dataSeriesChartData.begin();
//             dsIter != m_dataSeriesChartData.end();
//             dsIter++) {
//            QSharedPointer<ChartDataCartesian> spCart = dsIter->toStrongRef();
//            if ( ! spCart.isNull()) {
//                cartesianChartData.push_back(spCart.data());
//            }
//        }
//        for (std::list<QWeakPointer<ChartDataCartesian> >::iterator tsIter = m_frequencySeriesChartData.begin();
//             tsIter != m_frequencySeriesChartData.end();
//             tsIter++) {
//            QSharedPointer<ChartDataCartesian> spCart = tsIter->toStrongRef();
//            if ( ! spCart.isNull()) {
//                cartesianChartData.push_back(spCart.data());
//            }
//        }
//        for (std::list<QWeakPointer<ChartDataCartesian> >::iterator tsIter = m_timeSeriesChartData.begin();
//             tsIter != m_timeSeriesChartData.end();
//             tsIter++) {
//            QSharedPointer<ChartDataCartesian> spCart = tsIter->toStrongRef();
//            if ( ! spCart.isNull()) {
//                cartesianChartData.push_back(spCart.data());
//            }
//        }
//        
//        
//        /*
//         * Iterate over node indices for which colors are desired.
//         */
//        const std::vector<int32_t> nodeIndices = nodeChartID->getNodeIndices();
//        for (std::vector<int32_t>::const_iterator nodeIter = nodeIndices.begin();
//             nodeIter != nodeIndices.end();
//             nodeIter++) {
//            const int32_t nodeIndex = *nodeIter;
//            
//            /*
//             * Iterate over the data in the cartesian chart
//             */
//            for (std::vector<ChartDataCartesian*>::iterator cdIter = cartesianChartData.begin();
//                 cdIter != cartesianChartData.end();
//                 cdIter++) {
//                const ChartDataCartesian* cdc = *cdIter;
//                const ChartDataSource* cds = cdc->getChartDataSource();
//                if (cds->isSurfaceNodeSourceOfData(structureName, nodeIndex)) {
//                    /*
//                     * Found node index so add its color to the event
//                     */
//                    const CaretColorEnum::Enum color = cdc->getColor();
//                    const float* rgb = CaretColorEnum::toRGB(color);
//                    nodeChartID->addNode(nodeIndex,
//                                         rgb);
//                    break;
//                }
//            }
//        }

        nodeChartID->setEventProcessed();
    }
}

/**
 * Get the name for use in a GUI.
 *
 * @param includeStructureFlag - Prefix label with structure to which
 *      this structure model belongs.
 * @return   Name for use in a GUI.
 *
 */
AString
ModelChartTwo::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    AString name = "Chart";
    return name;
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model.
 */
AString 
ModelChartTwo::getNameForBrowserTab() const
{
    AString name = "Chart";
    return name;
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet* 
ModelChartTwo::getOverlaySet(const int tabIndex)
{
    CaretAssertArrayIndex(m_overlaySetArray,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_overlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
const OverlaySet* 
ModelChartTwo::getOverlaySet(const int tabIndex) const
{
    CaretAssertArrayIndex(m_overlaySetArray,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_overlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Initilize the overlays for this model.
 */
void 
ModelChartTwo::initializeOverlays()
{
    m_overlaySetArray->initializeOverlaySelections();
}

/**
 * Save version two charting information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param sceneClass
 *    SceneClass to which model specific information is added.
 */
void
ModelChartTwo::saveVersionTwoModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                           SceneClass* sceneClass)
{
    std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
   
    /*
     * Save matrix properties.
     */
    SceneObjectMapIntegerKey* matrixPropsMap = new SceneObjectMapIntegerKey("m_chartTwoMatrixDisplayPropertiesMAP",
                                                                            SceneObjectDataTypeEnum::SCENE_CLASS);
    
    for (int32_t tabIndex : tabIndices) {
        matrixPropsMap->addClass(tabIndex, m_chartTwoMatrixDisplayProperties[tabIndex]->saveToScene(sceneAttributes,
                                                                                                      "m_chartTwoMatrixDisplayProperties"));
    }
    sceneClass->addChild(matrixPropsMap);
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param sceneClass
 *    SceneClass to which model specific information is added.
 */
void 
ModelChartTwo::saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                      SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    saveVersionTwoModelSpecificInformationToScene(sceneAttributes,
                                                  sceneClass);
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ModelChartTwo::restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                     const SceneClass* sceneClass)
{
    reset();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    restoreVersionTwoModelSpecificInformationFromScene(sceneAttributes,
                                                       sceneClass);
}

/**
 * Restore information specific to the type of model from VERSION TWO scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ModelChartTwo::restoreVersionTwoModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                               const SceneClass* sceneClass)
{    /*
      * Restore matrix chart series models from scene.
      */
    const SceneObjectMapIntegerKey* matrixPropsMap = sceneClass->getMapIntegerKey("m_chartTwoMatrixDisplayPropertiesMAP");
    if (matrixPropsMap != NULL) {
        const std::vector<int32_t> tabIndices = matrixPropsMap->getKeys();
        for (const int32_t tabIndex : tabIndices) {
            const SceneClass* sceneClass = matrixPropsMap->classValue(tabIndex);
            m_chartTwoMatrixDisplayProperties[tabIndex]->restoreFromScene(sceneAttributes,
                                                                          sceneClass);
        }
    }

    /*
     * Restore the chart models
     */
    restoreVersionTwoChartModelsFromScene(sceneAttributes,
                                          sceneClass);
}

/**
 * Try to restore a scene from a version one chart model
 * by copying data from the chart one model.
 *
 * @param modelChartOne
 *     The charting version one model.
 */
void
ModelChartTwo::restoreSceneFromChartOneModel(ModelChart* modelChartOne)
{
    if (isRestoredFromScene()) {
        CaretAssert(0);
    }
    setRestoredFromScene(true);
    
    EventBrowserTabGetAll allTabsEvent;
    EventManager::get()->sendEvent(allTabsEvent.getPointer());
    std::vector<int32_t> validTabIndices = allTabsEvent.getBrowserTabIndices();
    
    for (auto tabIndex : validTabIndices) {
        switch (modelChartOne->getSelectedChartOneDataType(tabIndex)) {
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                setSelectedChartTwoDataType(tabIndex, ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES);
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                setSelectedChartTwoDataType(tabIndex,
                                            ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX);
                restoreMatrixChartFromChartOneModel(modelChartOne,
                                                    tabIndex);
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                setSelectedChartTwoDataType(tabIndex,
                                            ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX);
                restoreMatrixChartFromChartOneModel(modelChartOne,
                                                    tabIndex);
                break;
        }
    }
}

/**
 * Restore matrix from model one chart by copying from the model.
 *
 * @param modelChartOne
 *     The charting version one model.
 * @param tabIndex
 *     Index of tab being restored
 */
void
ModelChartTwo::restoreMatrixChartFromChartOneModel(ModelChart* modelChartOne,
                                                   const int32_t tabIndex)
{
    CaretAssert(modelChartOne);
    
    CaretDataFileSelectionModel* fileSelectionModel = NULL;
    switch (modelChartOne->getSelectedChartOneDataType(tabIndex)) {
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
            CaretAssert(0);
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
            fileSelectionModel = modelChartOne->getChartableMatrixParcelFileSelectionModel(tabIndex);
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
            fileSelectionModel = modelChartOne->getChartableMatrixSeriesFileSelectionModel(tabIndex);
            break;
    }
    
    CaretDataFile* caretDataFile = fileSelectionModel->getSelectedFile();
    if (caretDataFile == NULL) {
        return;
    }
    CaretMappableDataFile* mapFile = dynamic_cast<CaretMappableDataFile*>(caretDataFile);
    if (mapFile == NULL) {
        return;
    }
    
    ChartableTwoFileMatrixChart* fileMatrixChart = mapFile->getChartingDelegate()->getMatrixCharting();
    if (fileMatrixChart == NULL) {
        return;
    }
    
    ChartableMatrixParcelInterface* chartOneMatrixParcelFile = dynamic_cast<ChartableMatrixParcelInterface*>(mapFile);
    ChartableMatrixSeriesInterface* chartOneMatrixSeriesFile = dynamic_cast<ChartableMatrixSeriesInterface*>(mapFile);
    
    int32_t selectedRowColumnIndex = -1;
    MapYokingGroupEnum::Enum chartOneYoking = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    ChartMatrixDisplayProperties* chartOneMatrixDisplayProperties = NULL;
    if (chartOneMatrixParcelFile != NULL) {
        chartOneMatrixDisplayProperties = chartOneMatrixParcelFile->getChartMatrixDisplayProperties(tabIndex);
        
        switch (chartOneMatrixParcelFile->getMatrixLoadingDimension()) {
            case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                fileMatrixChart->setSelectedRowColumnDimension(ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN);
                break;
            case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                fileMatrixChart->setSelectedRowColumnDimension(ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW);
                break;
        }
        
        CiftiMappableConnectivityMatrixDataFile* connMatrixFile = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(mapFile);
        if (connMatrixFile != NULL) {
            const ConnectivityDataLoaded* connDataLoaded = connMatrixFile->getConnectivityDataLoaded();
            if (connDataLoaded != NULL) {
                int64_t loadedRowIndex = -1;
                int64_t loadedColumnIndex = -1;
                connDataLoaded->getRowColumnLoading(loadedRowIndex,
                                                    loadedColumnIndex);
                if (loadedRowIndex >= 0) {
                    selectedRowColumnIndex = loadedRowIndex;
                }
                else if (loadedColumnIndex >= 0) {
                    selectedRowColumnIndex = loadedColumnIndex;
                }
            }
        }
    }
    else if (chartOneMatrixSeriesFile != NULL) {
        chartOneMatrixDisplayProperties = chartOneMatrixSeriesFile->getChartMatrixDisplayProperties(tabIndex);
        
        chartOneYoking = chartOneMatrixSeriesFile->getMatrixRowColumnMapYokingGroup(tabIndex);
        fileMatrixChart->setSelectedRowColumnDimension(ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW);
        selectedRowColumnIndex = chartOneMatrixSeriesFile->getSelectedMapIndex(tabIndex);
    }
    else {
        CaretAssertMessage(0, "Has new matrix chart type been added?");
        return;
    }
    
    
    CaretAssert(chartOneMatrixDisplayProperties);
    ChartTwoMatrixDisplayProperties* chartTwoMatrixDisplayProperties = getChartTwoMatrixDisplayProperties(tabIndex);
    CaretAssert(chartTwoMatrixDisplayProperties);
    chartTwoMatrixDisplayProperties->resetPropertiesToDefault();
    chartTwoMatrixDisplayProperties->setGridLinesDisplayed(chartOneMatrixDisplayProperties->isGridLinesDisplayed());
    chartTwoMatrixDisplayProperties->setSelectedRowColumnHighlighted(chartOneMatrixDisplayProperties->isSelectedRowColumnHighlighted());
    AnnotationColorBar* chartOneColorBar = chartOneMatrixDisplayProperties->getColorBar();
    
    EventBrowserTabGet getTabEvent(tabIndex);
    EventManager::get()->sendEvent(getTabEvent.getPointer());
    BrowserTabContent* browserTabContent = getTabEvent.getBrowserTab();
    if (browserTabContent != NULL) {
        ChartTwoOverlaySet* chartTwoOverlaySet = browserTabContent->getChartTwoOverlaySet();
        CaretAssert(chartTwoOverlaySet);
        for (int32_t iOverlay = 0; iOverlay < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; iOverlay++) {
            ChartTwoOverlay* chartTwoOverlay = chartTwoOverlaySet->getOverlay(iOverlay);
            CaretAssert(chartTwoOverlay);
            if (iOverlay == 0) {
                chartTwoOverlay->setEnabled(true);
                chartTwoOverlay->setSelectionData(mapFile,
                                                  selectedRowColumnIndex);
                chartTwoOverlay->setMatrixTriangularViewingMode(ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL);
                chartTwoOverlay->setAllMapsSelected(false);
                chartTwoOverlay->setCartesianVerticalAxisLocation(ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT);
                chartTwoOverlay->setMapYokingGroup(chartOneYoking);
                AnnotationColorBar* chartTwoColorBar = chartTwoOverlay->getColorBar();
                if ((chartOneColorBar != NULL)
                    && (chartTwoColorBar != NULL)) {
                    *chartTwoColorBar = *chartOneColorBar;
                }
            }
            else {
                chartTwoOverlay->setEnabled(false);
            }
        }
    }
}

/**
 * Restore the chart models from a VERSION TWO scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ModelChartTwo::restoreVersionTwoChartModelsFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass)
{
    /* Probably nothing to do here */
}


/**
 * Get a text description of the window's content.
 *
 * @param tabIndex
 *    Index of the tab for content description.
 * @param descriptionOut
 *    Description of the window's content.
 */
void
ModelChartTwo::getDescriptionOfContent(const int32_t tabIndex,
                                    PlainTextStringBuilder& descriptionOut) const
{
//    ChartModel* chartModel = NULL;
//    switch (getSelectedChartOneDataType(tabIndex)) {
//        case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
//            break;
//        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
//        {
//            CaretDataFileSelectionModel* sm = m_chartableMatrixFileSelectionModel[tabIndex];
//            const CaretDataFile* caretFile = sm->getSelectedFile();
//            if (caretFile != NULL) {
//                descriptionOut.addLine("Matrix (layer) chart for: "
//                                       + caretFile->getFileNameNoPath());
//                return;
//            }
//        }
//            break;
//        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
//        {
//            CaretDataFileSelectionModel* sm = m_chartableMatrixSeriesFileSelectionModel[tabIndex];
//            const CaretDataFile* caretFile = sm->getSelectedFile();
//            if (caretFile != NULL) {
//                descriptionOut.addLine("Matrix (series) chart for: "
//                                       + caretFile->getFileNameNoPath());
//                return;
//            }
//        }
//            break;
//        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
//            chartModel = const_cast<ChartModelDataSeries*>(getSelectedDataSeriesChartModel(tabIndex));
//            break;
//        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
//            chartModel = const_cast<ChartModelFrequencySeries*>(getSelectedFrequencySeriesChartModel(tabIndex));
//            break;
//        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
//            chartModel = const_cast<ChartModelTimeSeries*>(getSelectedTimeSeriesChartModel(tabIndex));
//            break;
//    }
//
//    const ChartModel* chartModelConst = chartModel;
//    
//    if (chartModel != NULL) {
//        descriptionOut.addLine("Chart Type: "
//                               + ChartOneDataTypeEnum::toGuiName(chartModel->getChartDataType()));
//
//        descriptionOut.pushIndentation();
//        
//        const std::vector<const ChartData*> cdVec = chartModelConst->getAllChartDatas();
//        for (std::vector<const ChartData*>::const_iterator iter = cdVec.begin();
//             iter != cdVec.end();
//             iter++) {
//            const ChartData* cd = *iter;
//            if (cd->isSelected(tabIndex)) {
//                descriptionOut.addLine(cd->getChartDataSource()->getDescription());
//            }
//        }
//        
//        if (chartModel->isAverageChartDisplaySupported()) {
//            if (chartModel->isAverageChartDisplaySelected()) {
//                descriptionOut.addLine("Average Chart Displayed");
//            }
//        }
//        
//        descriptionOut.popIndentation();
//    }
//    else {
//        descriptionOut.addLine("No charts to display");
//    }
}

/**
 * Copy the tab content from the source tab index to the
 * destination tab index.
 *
 * @param sourceTabIndex
 *    Source from which tab content is copied.
 * @param destinationTabIndex
 *    Destination to which tab content is copied.
 */
void
ModelChartTwo::copyTabContent(const int32_t sourceTabIndex,
                      const int32_t destinationTabIndex)
{
    Model::copyTabContent(sourceTabIndex,
                          destinationTabIndex);
    
    m_overlaySetArray->copyOverlaySet(sourceTabIndex,
                                      destinationTabIndex);
    CaretAssertArrayIndex(m_selectedChartTwoDataType, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, sourceTabIndex);
    CaretAssertArrayIndex(m_selectedChartTwoDataType, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, destinationTabIndex);
    m_selectedChartTwoDataType[destinationTabIndex] = m_selectedChartTwoDataType[sourceTabIndex];
    
    m_histogramChartOverlaySetArray->copyChartOverlaySet(sourceTabIndex,
                                                         destinationTabIndex);
    m_lineSeriesChartOverlaySetArray->copyChartOverlaySet(sourceTabIndex,
                                                         destinationTabIndex);
    m_matrixChartOverlaySetArray->copyChartOverlaySet(sourceTabIndex,
                                                         destinationTabIndex);
    *m_chartTwoMatrixDisplayProperties[destinationTabIndex] = *m_chartTwoMatrixDisplayProperties[sourceTabIndex];
}


/**
 * Set the type of chart selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @param dataType
 *    Type of data for chart.
 */
void
ModelChartTwo::setSelectedChartTwoDataType(const int32_t tabIndex,
                                        const ChartTwoDataTypeEnum::Enum dataType)
{
    CaretAssertArrayIndex(m_selectedChartTwoDataType,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_selectedChartTwoDataType[tabIndex] = dataType;
}

/**
 * Get the valid chart data types based upon the currently loaded files.
 *
 * @param validChartDataTypesOut
 *    Output containing valid chart data types.
 */
void
ModelChartTwo::getValidChartTwoDataTypes(std::vector<ChartTwoDataTypeEnum::Enum>& validChartDataTypesOut) const
{
    validChartDataTypesOut.clear();
    
    /**
     * Get the data files.
     */
    std::vector<CaretMappableDataFile*> allDataFiles;
    EventCaretMappableDataFilesGet eventGetMapDataFiles;
    EventManager::get()->sendEvent(eventGetMapDataFiles.getPointer());
    eventGetMapDataFiles.getAllFiles(allDataFiles);

    std::set<ChartTwoDataTypeEnum::Enum> chartTypeSet;
    
    for (auto mapFile : allDataFiles) {
        ChartableTwoFileDelegate* chartFile = mapFile->getChartingDelegate();
        std::vector<ChartTwoDataTypeEnum::Enum> fileChartTypes;
        chartFile->getSupportedChartTwoDataTypes(fileChartTypes);
        
        chartTypeSet.insert(fileChartTypes.begin(),
                            fileChartTypes.end());
    }
    
    validChartDataTypesOut.insert(validChartDataTypesOut.end(),
                                  chartTypeSet.begin(),
                                  chartTypeSet.end());
}

/**
 * Get the type of chart selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Chart type in the given tab.
 */
ChartTwoDataTypeEnum::Enum
ModelChartTwo::getSelectedChartTwoDataType(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_selectedChartTwoDataType,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    ChartTwoDataTypeEnum::Enum chartDataType = m_selectedChartTwoDataType[tabIndex];
    
    std::vector<ChartTwoDataTypeEnum::Enum> validChartDataTypes;
    getValidChartTwoDataTypes(validChartDataTypes);
    
    /*
     * Test if selected chart type is still valid
     */
    if (std::find(validChartDataTypes.begin(),
                  validChartDataTypes.end(),
                  chartDataType) == validChartDataTypes.end()) {
        chartDataType = ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID;
    }
    
    /*
     * Find a valid chart type?
     */
    if (chartDataType == ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID) {
        if ( ! validChartDataTypes.empty()) {
            CaretAssertVectorIndex(validChartDataTypes, 0);
            chartDataType = validChartDataTypes[0];
        }
    }
    
    m_selectedChartTwoDataType[tabIndex] = chartDataType;
    return chartDataType;
}

/**
 * Update the chart overlay sets.
 * 
 * @param tabIndex
 *     Index of the tab.
 */
void
ModelChartTwo::updateChartOverlaySets(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_selectedChartTwoDataType, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    switch (m_selectedChartTwoDataType[tabIndex]) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
    }
}

/**
 * @return The chart matrix display properties for the given tab.
 *
 * @param tabIndex
 *     The tab index.
 */
ChartTwoMatrixDisplayProperties*
ModelChartTwo::getChartTwoMatrixDisplayProperties(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_chartTwoMatrixDisplayProperties,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_chartTwoMatrixDisplayProperties[tabIndex];
}

/**
 * @return The chart matrix display properties for the given tab.
 *
 * @param tabIndex
 *     The tab index.
 */
const ChartTwoMatrixDisplayProperties*
ModelChartTwo::getChartTwoMatrixDisplayProperties(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartTwoMatrixDisplayProperties,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_chartTwoMatrixDisplayProperties[tabIndex];
}

/**
 * Get the chart overlay set for this model.
 *
 * @param tabIndex
 *     Index for the chart overlay set.
 * @return
 *     Chart overlay set or NULL if not valid for this model.
 */
ChartTwoOverlaySet*
ModelChartTwo::getChartTwoOverlaySet(const int tabIndex)
{
    ChartTwoOverlaySet* chartOverlaySet = NULL;
    
    switch (getSelectedChartTwoDataType(tabIndex)) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            chartOverlaySet = m_histogramChartOverlaySetArray->getChartTwoOverlaySet(tabIndex);
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            chartOverlaySet = m_lineSeriesChartOverlaySetArray->getChartTwoOverlaySet(tabIndex);
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            chartOverlaySet = m_matrixChartOverlaySetArray->getChartTwoOverlaySet(tabIndex);
            break;
    }

    return chartOverlaySet;
}

/**
 * Get the chart overlay set for this model.
 *
 * @param tabIndex
 *     Index for the chart overlay set.
 * @return
 *     Chart overlay set or NULL if not valid for this model.
 */
const ChartTwoOverlaySet*
ModelChartTwo::getChartTwoOverlaySet(const int tabIndex) const
{
    ModelChartTwo* nonConstModelChart = const_cast<ModelChartTwo*>(this);
    CaretAssert(nonConstModelChart);
    const ChartTwoOverlaySet* chartOverlaySet = nonConstModelChart->getChartTwoOverlaySet(tabIndex);
    
    return chartOverlaySet;
}


