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
#include <set>

#include "AnnotationColorBar.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretLogger.h"
#include "ChartData.h"
#include "ChartDataCartesian.h"
#include "ChartDataSource.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableMatrixParcelInterface.h"
#include "ChartableMatrixSeriesInterface.h"
#include "ChartMatrixDisplayProperties.h"
#include "ChartModelDataSeries.h"
#include "ChartModelFrequencySeries.h"
#include "ChartModelTimeSeries.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileMatrixChart.h"
#include "ChartableTwoFileLineLayerChart.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "ChartTwoLineSeriesHistory.h"
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
#include "EventBrowserTabIndicesGetAll.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventChartTwoLoadLineSeriesData.h"
#include "EventManager.h"
#include "EventNodeIdentificationColorsGetFromCharts.h"
#include "MapFileDataSelector.h"
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
    m_lineLayerChartOverlaySetArray = std::unique_ptr<ChartTwoOverlaySetArray>(new ChartTwoOverlaySetArray(ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER,
                                                                                                     "Line Layer Chart Overlays"));
    m_lineSeriesChartOverlaySetArray = std::unique_ptr<ChartTwoOverlaySetArray>(new ChartTwoOverlaySetArray(ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES,
                                                                                                            "Line Series Chart Overlays"));
    initializeCharts();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<ChartTwoDataTypeEnum, ChartTwoDataTypeEnum::Enum>("m_selectedChartTwoDataType",
                                                                                                         m_selectedChartTwoDataType);
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
 * Load chart data for a surface node.
 *
 * @param structure
 *     The surface structure
 * @param surfaceNumberOfNodes
 *     Number of nodes in surface.
 * @param nodeIndex
 *     Index of node.
 * @throws
 *     DataFileException if there is an error loading data.
 */
void
ModelChartTwo::loadChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                        const int32_t surfaceNumberOfNodes,
                                        const int32_t nodeIndex)
{
    std::vector<int32_t> tabIndices;
    EventBrowserTabIndicesGetAll tabIndicesEvent;
    EventManager::get()->sendEvent(tabIndicesEvent.getPointer());
    
    MapFileDataSelector mapFileDataSelector;
    mapFileDataSelector.setSurfaceVertex(structure,
                                         surfaceNumberOfNodes,
                                         nodeIndex);
    EventChartTwoLoadLineSeriesData chartTwoLineSeriesEvent(tabIndicesEvent.getAllBrowserTabIndices(),
                                                            mapFileDataSelector);
    EventManager::get()->sendEvent(chartTwoLineSeriesEvent.getPointer());
}

/**
 * Load chart data for an average of surface nodes.
 *
 * @param structure
 *     The surface structure
 * @param surfaceNumberOfNodes
 *     Number of nodes in surface.
 * @param nodeIndices
 *     Indices of node.
 * @throws
 *     DataFileException if there is an error loading data.
 */
void
ModelChartTwo::loadAverageChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                const int32_t surfaceNumberOfNodes,
                                                const std::vector<int32_t>& nodeIndices)
{
    std::vector<int32_t> tabIndices;
    EventBrowserTabIndicesGetAll tabIndicesEvent;
    EventManager::get()->sendEvent(tabIndicesEvent.getPointer());
    
    MapFileDataSelector mapFileDataSelector;
    mapFileDataSelector.setSurfaceVertexAverage(structure,
                                                surfaceNumberOfNodes,
                                                nodeIndices);
    EventChartTwoLoadLineSeriesData chartTwoLineSeriesEvent(tabIndicesEvent.getAllBrowserTabIndices(),
                                                            mapFileDataSelector);
    EventManager::get()->sendEvent(chartTwoLineSeriesEvent.getPointer());
}

/**
 * Load chart data for voxel at the given coordinate.
 *
 * @param xyz
 *     Coordinate of voxel.
 * @throws
 *     DataFileException if there is an error loading data.
 */
void
ModelChartTwo::loadChartDataForVoxelAtCoordinate(const float xyz[3])
{
    std::vector<int32_t> tabIndices;
    EventBrowserTabIndicesGetAll tabIndicesEvent;
    EventManager::get()->sendEvent(tabIndicesEvent.getPointer());
    
    MapFileDataSelector mapFileDataSelector;
    mapFileDataSelector.setVolumeVoxelXYZ(xyz);
    EventChartTwoLoadLineSeriesData chartTwoLineSeriesEvent(tabIndicesEvent.getAllBrowserTabIndices(),
                                                            mapFileDataSelector);
    EventManager::get()->sendEvent(chartTwoLineSeriesEvent.getPointer());
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
    CaretAssert(ciftiMapFile);
    
    EventBrowserTabIndicesGetAll tabIndicesEvent;
    EventManager::get()->sendEvent(tabIndicesEvent.getPointer());
    
    MapFileDataSelector mapFileDataSelector;
    mapFileDataSelector.setRowIndex(ciftiMapFile,
                                    ciftiMapFile->getFileName(),
                                    rowIndex);
    EventChartTwoLoadLineSeriesData chartTwoLineSeriesEvent(tabIndicesEvent.getAllBrowserTabIndices(),
                                                            mapFileDataSelector);
    EventManager::get()->sendEvent(chartTwoLineSeriesEvent.getPointer());
}

/**
 * Load line chart data for yoked CIFTI scalar data series files
 *
 * @param mapYokingGroup
 *     The map yoking group.
 * @param mapIndex
 *     The map index.
 */
void
ModelChartTwo::loadChartDataForYokedScalarDataSeriesFiles(const MapYokingGroupEnum::Enum mapYokingGroup,
                                                          const int32_t mapIndex)
{
    if (mapYokingGroup == MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        return;
    }
    
    EventBrowserTabIndicesGetAll tabIndicesEvent;
    EventManager::get()->sendEvent(tabIndicesEvent.getPointer());
    const std::vector<int32_t> tabIndices = tabIndicesEvent.getAllBrowserTabIndices();
    
    /*
     * Find Cifti Scalar Data Series Files in valid Chart Overlays that
     * have chart line series data loading enabled
     */
    std::set<CiftiScalarDataSeriesFile*> dataSeriesFiles;
    for (auto tabIndex : tabIndices) {
        CaretAssertArrayIndex(m_selectedChartTwoDataType, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
        if (m_selectedChartTwoDataType[tabIndex] == ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES) {
            const ChartTwoOverlaySet* overlaySet = m_lineSeriesChartOverlaySetArray->getChartTwoOverlaySet(tabIndex);
            CaretAssert(overlaySet);
            
            const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
            for (int32_t iOverlay = 0; iOverlay < numOverlays; iOverlay++) {
                const ChartTwoOverlay* overlay = overlaySet->getOverlay(iOverlay);
                CaretAssert(overlay);
                
                if (overlay->isEnabled()
                    && overlay->isLineSeriesLoadingEnabled()) {
                    if (overlay->getMapYokingGroup() == mapYokingGroup) {
                        CaretMappableDataFile* mapFile = overlay->getSelectedMapFile();
                        if (mapFile != NULL) {
                            if (mapFile->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES) {
                                CiftiScalarDataSeriesFile* scalarDataSeriesFile = dynamic_cast<CiftiScalarDataSeriesFile*>(mapFile);
                                CaretAssert(scalarDataSeriesFile);
                                dataSeriesFiles.insert(scalarDataSeriesFile);
                            }
                        }
                    }
                }
            }
        }
    }
    
    for (auto seriesFile : dataSeriesFiles) {
        loadChartDataForCiftiMappableFileRow(seriesFile, mapIndex);
    }
}

/**
 * Select yoked row/column in scalar data series file's overlay
 *
 * @param mapYokingGroup
 *     The map yoking group.
 * @param mapIndex
 *     The map index.
 */
void
ModelChartTwo::selectRowColumnInYokedScalarDataSeriesFileOverlay(const MapYokingGroupEnum::Enum mapYokingGroup,
                                                          const int32_t mapIndex)
{
    if (mapYokingGroup == MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        return;
    }
    
    EventBrowserTabIndicesGetAll tabIndicesEvent;
    EventManager::get()->sendEvent(tabIndicesEvent.getPointer());
    const std::vector<int32_t> tabIndices = tabIndicesEvent.getAllBrowserTabIndices();
    
    /*
     * Find Cifti Scalar Data Series Files in valid Chart Overlays that
     * have the matching yoking group
     */
    for (auto tabIndex : tabIndices) {
        CaretAssertArrayIndex(m_selectedChartTwoDataType, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
        if (m_selectedChartTwoDataType[tabIndex] == ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX) {
            ChartTwoOverlaySet* overlaySet = m_matrixChartOverlaySetArray->getChartTwoOverlaySet(tabIndex);
            CaretAssert(overlaySet);
            
            const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
            for (int32_t iOverlay = 0; iOverlay < numOverlays; iOverlay++) {
                ChartTwoOverlay* overlay = overlaySet->getOverlay(iOverlay);
                CaretAssert(overlay);
                
                if (overlay->isEnabled()) {
                    if (overlay->getMapYokingGroup() == mapYokingGroup) {
                        CaretMappableDataFile* mapFile = overlay->getSelectedMapFile();
                        if (mapFile != NULL) {
                            if (mapFile->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES) {
                                overlay->setSelectionData(mapFile, mapIndex);
                            }
                        }
                    }
                }
            }
            
        }
    }
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
    const std::vector<int32_t> validTabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    std::vector<int32_t> histogramChartTabIndices;
    std::vector<int32_t> lineLayerChartTabIndices;
    std::vector<int32_t> lineSeriesChartTabIndices;
    std::vector<int32_t> matrixChartTabIndices;
    for (auto tabIndex : validTabIndices) {
        switch (m_selectedChartTwoDataType[tabIndex]) {
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                histogramChartTabIndices.push_back(tabIndex);
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                lineLayerChartTabIndices.push_back(tabIndex);
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                lineSeriesChartTabIndices.push_back(tabIndex);
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                matrixChartTabIndices.push_back(tabIndex);
                break;
        }
    }
    
    if ( ! histogramChartTabIndices.empty()) {
        SceneClass* histogramClass = m_histogramChartOverlaySetArray->saveTabIndicesToScene(histogramChartTabIndices,
                                                                                            sceneAttributes,
                                                                                            "m_histogramChartOverlaySetArray");
        if (histogramClass != NULL) {
            sceneClass->addClass(histogramClass);
        }
    }

    if ( ! lineSeriesChartTabIndices.empty()) {
        SceneClass* lineSeriesClass = m_lineSeriesChartOverlaySetArray->saveTabIndicesToScene(lineSeriesChartTabIndices,
                                                                                              sceneAttributes,
                                                                                              "m_lineSeriesChartOverlaySetArray");
        if (lineSeriesClass != NULL) {
            sceneClass->addClass(lineSeriesClass);
        }
    }
    
    if ( ! lineLayerChartTabIndices.empty()) {
        SceneClass* lineLayerClass = m_lineLayerChartOverlaySetArray->saveTabIndicesToScene(lineLayerChartTabIndices,
                                                                                              sceneAttributes,
                                                                                              "m_lineLayerChartOverlaySetArray");
        if (lineLayerClass != NULL) {
            sceneClass->addClass(lineLayerClass);
        }
    }
    
    if ( ! matrixChartTabIndices.empty()) {
        SceneClass* matrixClass = m_matrixChartOverlaySetArray->saveTabIndicesToScene(matrixChartTabIndices,
                                                                                      sceneAttributes,
                                                                                      "m_matrixChartOverlaySetArray");
        if (matrixClass != NULL) {
            sceneClass->addClass(matrixClass);
        }
    }
    
    m_sceneAssistant->saveMembers(sceneAttributes,
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
    
    m_histogramChartOverlaySetArray->restoreFromScene(sceneAttributes,
                                                      sceneClass->getClass("m_histogramChartOverlaySetArray"));
    m_lineLayerChartOverlaySetArray->restoreFromScene(sceneAttributes,
                                                      sceneClass->getClass("m_lineLayerChartOverlaySetArray"));
    m_lineSeriesChartOverlaySetArray->restoreFromScene(sceneAttributes,
                                                       sceneClass->getClass("m_lineSeriesChartOverlaySetArray"));
    m_matrixChartOverlaySetArray->restoreFromScene(sceneAttributes,
                                                      sceneClass->getClass("m_matrixChartOverlaySetArray"));
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
    
    bool haveLineSeriesModelsFlag = true;
    
    for (auto tabIndex : validTabIndices) {
        switch (modelChartOne->getSelectedChartOneDataType(tabIndex)) {
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                setSelectedChartTwoDataType(tabIndex,
                                            ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES);
                haveLineSeriesModelsFlag = true;
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
    
    if (haveLineSeriesModelsFlag) {
        restoreLineSeriesChartFromChartOneModel(modelChartOne);
    }
}

/**
 * Attempt to restore line series data from a chart version one scene.
 *
 * @param modelChartOne
 *     The chart one model.
 */
void
ModelChartTwo::restoreLineSeriesChartFromChartOneModel(ModelChart* modelChartOne)
{
    CaretAssert(modelChartOne);
    
    std::set<ChartDataSource> dataSources;
    
    int32_t maxDisplayCount = 0;
    
    std::vector<int32_t> validTabIndices;
    std::map<AString, CaretColorEnum::Enum> dataFileNamesAndColors;
    
    /*
     * Loop through all tabs to find those containing line-series style charts
     */
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        ChartModelCartesian* cartesianModel = NULL;
        switch (modelChartOne->getSelectedChartOneDataType(iTab)) {
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                cartesianModel = modelChartOne->getSelectedDataSeriesChartModel(iTab);
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                cartesianModel = modelChartOne->getSelectedFrequencySeriesChartModel(iTab);
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                cartesianModel = modelChartOne->getSelectedTimeSeriesChartModel(iTab);
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                break;
        }
        
        if (cartesianModel != NULL) {
            validTabIndices.push_back(iTab);
            
            maxDisplayCount = std::max(maxDisplayCount,
                                       cartesianModel->getMaximumNumberOfChartDatasToDisplay());
            
            /*
             * Examine all lines in the tab
             */
            const int32_t numChartLines   = cartesianModel->getNumberOfChartData();
            for (int32_t i = (numChartLines - 1); i >= 0; i--) {
                const ChartData* chartData = cartesianModel->getChartDataAtIndex(i);
                if (chartData != NULL) {
                    const ChartDataSource* chartDataSource = chartData->getChartDataSource();
                    if (chartData->isSelected(iTab)) {
                        ChartDataSource dataSourceCopy(*chartDataSource);
                        dataSources.insert(dataSourceCopy);
                        const ChartDataCartesian* cartesianData = dynamic_cast<const ChartDataCartesian*>(chartData);
                        CaretColorEnum::Enum color = CaretColorEnum::RED;
                        if (cartesianData != NULL) {
                            color = cartesianData->getColor();
                        }
                        dataFileNamesAndColors.insert(std::make_pair(chartDataSource->getChartableFileName(),
                                                                     color));
                    }
                }
            }
        }
    }

    std::vector<CaretMappableDataFile*> allDataFiles;
    EventCaretMappableDataFilesGet eventGetMapDataFiles;
    EventManager::get()->sendEvent(eventGetMapDataFiles.getPointer());
    eventGetMapDataFiles.getAllFiles(allDataFiles);
    
    std::vector<CaretMappableDataFile*> overlayMapFiles;
    
    /*
     * Find the data files and setup for loading of
     * lines series data.
     */
    for (const auto& filenameAndColor : dataFileNamesAndColors) {
        const AString filename = filenameAndColor.first;
        CaretColorEnum::Enum color = filenameAndColor.second;
        CaretMappableDataFile* mapFileFullPath = NULL;
        CaretMappableDataFile* mapFileNoPath   = NULL;
        for (auto mapFile : allDataFiles) {
            if (filename == mapFile->getFileName()) {
                mapFileFullPath = mapFile;
            }
            if (mapFile->getFileName().endsWith(filename)) {
                mapFileNoPath = mapFile;
            }
        }
        
        ChartTwoLineSeriesHistory* lineSeriesHistory = NULL;
        if (mapFileFullPath != NULL) {
            lineSeriesHistory = mapFileFullPath->getChartingDelegate()->getLineSeriesCharting()->getHistory();
            overlayMapFiles.push_back(mapFileFullPath);
        }
        else if (mapFileNoPath != NULL) {
            lineSeriesHistory = mapFileNoPath->getChartingDelegate()->getLineSeriesCharting()->getHistory();
            overlayMapFiles.push_back(mapFileNoPath);
        }
        else {
            CaretLogWarning("Unable to find data file with name: "
                            + filename);
        }
        if (lineSeriesHistory != NULL) {
            lineSeriesHistory->setLoadingEnabled(true);
            if (maxDisplayCount <= 0) {
                maxDisplayCount = 1;
            }
            lineSeriesHistory->setDisplayCount(maxDisplayCount);
            lineSeriesHistory->setDefaultColor(color);
        }
    }
    
    /*
     * Send events to load line charts for the brainordinates
     * found in the older charts
     */
    for (const auto& ds : dataSources) {
        MapFileDataSelector mapFileDataSelector;
        switch (ds.getDataSourceMode()) {
            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_FILE_ROW:
                break;
            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_INVALID:
                break;
            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDEX:
            {
                int32_t surfaceNumberOfVertices = -1;
                int32_t vertexIndex = -1;
                AString structureName;
                ds.getSurfaceNode(structureName, surfaceNumberOfVertices, vertexIndex);
                bool validFlag = false;
                const StructureEnum::Enum structure = StructureEnum::fromName(structureName,
                                                                              &validFlag);
                if (validFlag) {
                    mapFileDataSelector.setSurfaceVertex(structure,
                                                         surfaceNumberOfVertices,
                                                         vertexIndex);
                }
                else {
                    CaretLogWarning("Unrecogized structure name :"
                                    + structureName);
                }
            }
                break;
            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDICES_AVERAGE:
            {
                int32_t surfaceNumberOfVertices = -1;
                AString structureName;
                std::vector<int32_t> vertices;
                ds.getSurfaceNodeAverage(structureName, surfaceNumberOfVertices, vertices);
                bool validFlag = false;
                const StructureEnum::Enum structure = StructureEnum::fromName(structureName,
                                                                              &validFlag);
                if (validFlag) {
                    mapFileDataSelector.setSurfaceVertexAverage(structure,
                                                                surfaceNumberOfVertices,
                                                                vertices);
                }
                else {
                    CaretLogWarning("Unrecogized structure name :"
                                    + structureName);
                }
            }
                break;
            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_VOXEL_IJK:
            {
                float xyz[3];
                ds.getVolumeVoxel(xyz);
                mapFileDataSelector.setVolumeVoxelXYZ(xyz);
            }
                break;
        }
        
        if (mapFileDataSelector.getDataSelectionType() != MapFileDataSelector::DataSelectionType::INVALID) {
            EventChartTwoLoadLineSeriesData loadLineDataEvent(validTabIndices,
                                                              mapFileDataSelector);
            EventManager::get()->sendEvent(loadLineDataEvent.getPointer());
        }
    }
    
    /*
     * Put line-series chart files into chart overlays
     */
    const int32_t maxChartOverlays = 3;
    const int32_t numChartOverlayMapFiles = std::min(maxChartOverlays, static_cast<int32_t>(overlayMapFiles.size()));
    for (auto tabIndex : validTabIndices) {
        for (int32_t i = 0; i < numChartOverlayMapFiles; i++) {
            CaretAssertVectorIndex(overlayMapFiles, i);
            getChartTwoOverlaySet(tabIndex)->getOverlay(i)->setSelectionData(overlayMapFiles[i], 0);
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
    m_lineLayerChartOverlaySetArray->copyChartOverlaySet(sourceTabIndex,
                                                         destinationTabIndex);
    m_lineSeriesChartOverlaySetArray->copyChartOverlaySet(sourceTabIndex,
                                                          destinationTabIndex);
    m_matrixChartOverlaySetArray->copyChartOverlaySet(sourceTabIndex,
                                                         destinationTabIndex);
}

/**
 * Copy cartesian axes from one tab to another.
 *
 * @param sourceTabIndex
 *    Source from which tab content is copied.
 * @param destinationTabIndex
 *    Destination to which tab content is copied.
 */
void
ModelChartTwo::copyChartTwoCartesianAxes(const int32_t sourceTabIndex,
                                         const int32_t destinationTabIndex)
{
    CaretAssertArrayIndex(m_histogramChartOverlaySetArray, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, sourceTabIndex);
    CaretAssertArrayIndex(m_lineSeriesChartOverlaySetArray, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, destinationTabIndex);

    m_histogramChartOverlaySetArray->copyChartOverlaySetCartesianAxes(sourceTabIndex,
                                                                      destinationTabIndex);
    m_lineLayerChartOverlaySetArray->copyChartOverlaySetCartesianAxes(sourceTabIndex,
                                                                       destinationTabIndex);
    m_lineSeriesChartOverlaySetArray->copyChartOverlaySetCartesianAxes(sourceTabIndex,
                                                                       destinationTabIndex);
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
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
    }
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
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
            chartOverlaySet = m_lineLayerChartOverlaySetArray->getChartTwoOverlaySet(tabIndex);
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

/**
 * @return All chart two overlay sets in this model for the given tab index
 * @param tabIndex
 *    Index of the tab
 * @return vector containing the overlay sets
 *
 */
std::vector<ChartTwoOverlaySet*>
ModelChartTwo::getAllChartTwoOverlaySets(const int32_t tabIndex) const
{
    std::vector<ChartTwoOverlaySet*> overlaySets;
    
    overlaySets.push_back(m_histogramChartOverlaySetArray->getChartTwoOverlaySet(tabIndex));
    overlaySets.push_back(m_lineLayerChartOverlaySetArray->getChartTwoOverlaySet(tabIndex));
    overlaySets.push_back(m_lineSeriesChartOverlaySetArray->getChartTwoOverlaySet(tabIndex));
    overlaySets.push_back(m_matrixChartOverlaySetArray->getChartTwoOverlaySet(tabIndex));

    return overlaySets;
}

