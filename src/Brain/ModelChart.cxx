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

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "ChartAxis.h"
#include "ChartableLineSeriesBrainordinateInterface.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretLogger.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "ChartableLineSeriesRowColumnInterface.h"
#include "ChartableMatrixInterface.h"
#include "ChartData.h"
#include "ChartDataCartesian.h"
#include "ChartDataSource.h"
#include "ChartModelDataSeries.h"
#include "ChartModelFrequencySeries.h"
#include "ChartModelTimeSeries.h"
#include "ChartingVersionEnum.h"
#include "CiftiMappableDataFile.h"
#include "CiftiScalarDataSeriesFile.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserTabIndicesGetAll.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "EventNodeIdentificationColorsGetFromCharts.h"
#include "ModelChart.h"
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
ModelChart::ModelChart(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_CHART,
                         brain)
{
    std::vector<StructureEnum::Enum> overlaySurfaceStructures;
    m_overlaySetArray = new OverlaySetArray(overlaySurfaceStructures,
                                            Overlay::INCLUDE_VOLUME_FILES_YES,
                                            "Chart View");
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartableMatrixFileSelectionModel[i] = CaretDataFileSelectionModel::newInstanceForChartableMatrixParcelInterface();
        m_chartableMatrixSeriesFileSelectionModel[i] = CaretDataFileSelectionModel::newInstanceForCaretDataFileType(
                                                                                       DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES);
    }
    
    initializeCharts();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS);
}

/**
 * Destructor
 */
ModelChart::~ModelChart()
{
    delete m_overlaySetArray;
    EventManager::get()->removeAllEventsFromListener(this);
    
    removeAllCharts();    

    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_chartableMatrixFileSelectionModel[i];
        m_chartableMatrixFileSelectionModel[i] = NULL;
        
        delete m_chartableMatrixSeriesFileSelectionModel[i];
        m_chartableMatrixSeriesFileSelectionModel[i] = NULL;
    }
}

void
ModelChart::initializeCharts()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectedChartOneDataType[i] = ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID;
        
        m_chartModelDataSeries[i] = new ChartModelDataSeries();
        m_chartModelDataSeries[i]->getLeftAxis()->setText("Value");
        m_chartModelDataSeries[i]->getBottomAxis()->setText("Map Index");
        
        m_chartModelFrequencySeries[i] = new ChartModelFrequencySeries();
        m_chartModelFrequencySeries[i]->getLeftAxis()->setText("Value");
        m_chartModelFrequencySeries[i]->getBottomAxis()->setText("Frequency");
        
        m_chartModelTimeSeries[i] = new ChartModelTimeSeries();
        m_chartModelTimeSeries[i]->getLeftAxis()->setText("Activity");
        m_chartModelTimeSeries[i]->getBottomAxis()->setText("Time");
    }    
}

/**
 * Reset this model.
 */
void
ModelChart::reset()
{
    removeAllCharts();
    
    initializeCharts();
}

/**
 * Remove all of the charts.
 */
void
ModelChart::removeAllCharts()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        if (m_chartModelDataSeries[i] != NULL) {
            delete m_chartModelDataSeries[i];
            m_chartModelDataSeries[i] = NULL;
        }
        
        if (m_chartModelFrequencySeries[i] != NULL) {
            delete m_chartModelFrequencySeries[i];
            m_chartModelFrequencySeries[i] = NULL;
        }
        
        if (m_chartModelTimeSeries[i] != NULL) {
            delete m_chartModelTimeSeries[i];
            m_chartModelTimeSeries[i] = NULL;
        }
    }

    
    m_dataSeriesChartData.clear();
    m_frequencySeriesChartData.clear();
    m_timeSeriesChartData.clear();
    
    m_previousChartMatrixFiles.clear();
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
ModelChart::loadAverageChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                         const int32_t surfaceNumberOfNodes,
                                         const std::vector<int32_t>& nodeIndices)
{
    std::map<ChartableLineSeriesBrainordinateInterface*, std::vector<int32_t> > chartFileEnabledTabs;
    getTabsAndBrainordinateChartFilesForLineChartLoading(chartFileEnabledTabs);
    
    for (std::map<ChartableLineSeriesBrainordinateInterface*, std::vector<int32_t> >::iterator fileTabIter = chartFileEnabledTabs.begin();
         fileTabIter != chartFileEnabledTabs.end();
         fileTabIter++) {
        ChartableLineSeriesBrainordinateInterface* chartFile = fileTabIter->first;
        const std::vector<int32_t>  tabIndices = fileTabIter->second;
        
        CaretAssert(chartFile);
        ChartData* chartData = chartFile->loadAverageLineSeriesChartDataForSurfaceNodes(structure,
                                                                              nodeIndices);
        if (chartData != NULL) {
            ChartDataSource* dataSource = chartData->getChartDataSource();
            dataSource->setSurfaceNodeAverage(chartFile->getLineSeriesChartCaretMappableDataFile()->getFileName(),
                                              StructureEnum::toName(structure),
                                              surfaceNumberOfNodes, nodeIndices);
            
            addChartToChartModels(tabIndices,
                                  chartData);
        }
    }
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
ModelChart::loadChartDataForVoxelAtCoordinate(const float xyz[3])
{
    std::map<ChartableLineSeriesBrainordinateInterface*, std::vector<int32_t> > chartFileEnabledTabs;
    getTabsAndBrainordinateChartFilesForLineChartLoading(chartFileEnabledTabs);
    
    for (std::map<ChartableLineSeriesBrainordinateInterface*, std::vector<int32_t> >::iterator fileTabIter = chartFileEnabledTabs.begin();
         fileTabIter != chartFileEnabledTabs.end();
         fileTabIter++) {
        ChartableLineSeriesBrainordinateInterface* chartFile = fileTabIter->first;
        const std::vector<int32_t>  tabIndices = fileTabIter->second;
        
        CaretAssert(chartFile);
        ChartData* chartData = chartFile->loadLineSeriesChartDataForVoxelAtCoordinate(xyz);
        if (chartData != NULL) {
            ChartDataSource* dataSource = chartData->getChartDataSource();
            dataSource->setVolumeVoxel(chartFile->getLineSeriesChartCaretMappableDataFile()->getFileName(),
                                       xyz);
            
            addChartToChartModels(tabIndices,
                                  chartData);
        }
    }
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
ModelChart::loadChartDataForYokedCiftiMappableFiles(const MapYokingGroupEnum::Enum mapYokingGroup,
                                                    const int32_t mapIndex)
{
    if (mapYokingGroup == MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        return;
    }
    
    std::map<ChartableLineSeriesRowColumnInterface*, std::vector<int32_t> > chartFileEnabledTabs;
    getTabsAndRowColumnChartFilesForLineChartLoading(chartFileEnabledTabs);
    
    for (std::map<ChartableLineSeriesRowColumnInterface*, std::vector<int32_t> >::iterator fileTabIter = chartFileEnabledTabs.begin();
         fileTabIter != chartFileEnabledTabs.end();
         fileTabIter++) {
        ChartableLineSeriesRowColumnInterface* chartFile = fileTabIter->first;
        CaretAssert(chartFile);
        CiftiScalarDataSeriesFile* csdsf = dynamic_cast<CiftiScalarDataSeriesFile*>(chartFile);
        if (csdsf != NULL) {
            
            std::vector<int32_t> matchedTabIndices;
            const std::vector<int32_t>  tabIndices = fileTabIter->second;
            for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
                 tabIter != tabIndices.end();
                 tabIter++) {
                const int32_t tabIndex = *tabIter;
                if (csdsf->getMatrixRowColumnMapYokingGroup(tabIndex) == mapYokingGroup) {
                    matchedTabIndices.push_back(tabIndex);
                }
            }
            
            if ( ! matchedTabIndices.empty()) {
                ChartData* chartData = chartFile->loadLineSeriesChartDataForRow(mapIndex);
                if (chartData != NULL) {
                    ChartDataSource* dataSource = chartData->getChartDataSource();
                    dataSource->setFileRow(chartFile->getLineSeriesChartCaretMappableDataFile()->getFileName(),
                                           mapIndex);
                    
                    addChartToChartModels(matchedTabIndices,
                                          chartData);
                }
            }
        }
    }
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
ModelChart::loadChartDataForCiftiMappableFileRow(CiftiMappableDataFile* ciftiMapFile,
                                                 const int32_t rowIndex)
{
    CaretAssert(ciftiMapFile);

    std::map<ChartableLineSeriesRowColumnInterface*, std::vector<int32_t> > chartFileEnabledTabs;
    getTabsAndRowColumnChartFilesForLineChartLoading(chartFileEnabledTabs);
    
    for (std::map<ChartableLineSeriesRowColumnInterface*, std::vector<int32_t> >::iterator fileTabIter = chartFileEnabledTabs.begin();
         fileTabIter != chartFileEnabledTabs.end();
         fileTabIter++) {
        ChartableLineSeriesRowColumnInterface* chartFile = fileTabIter->first;
        if (ciftiMapFile == dynamic_cast<CiftiMappableDataFile*>(chartFile)) {
            const std::vector<int32_t>  tabIndices = fileTabIter->second;
            
            CaretAssert(chartFile);
            ChartData* chartData = chartFile->loadLineSeriesChartDataForRow(rowIndex);
            if (chartData != NULL) {
                ChartDataSource* dataSource = chartData->getChartDataSource();
                dataSource->setFileRow(chartFile->getLineSeriesChartCaretMappableDataFile()->getFileName(),
                                       rowIndex);
                
                addChartToChartModels(tabIndices,
                                      chartData);
            }
        }
    }
}

/**
 * Add the chart to the given tabs.
 *
 * @param tabIndices
 *    Indices of tabs for chart data
 * @param chartData
 *    Chart data that is added.
 */
void
ModelChart::addChartToChartModels(const std::vector<int32_t>& tabIndices,
                                  ChartData* chartData)
{
    CaretAssert(chartData);
    
    const ChartOneDataTypeEnum::Enum chartDataDataType = chartData->getChartDataType();
    
    switch (chartDataDataType) {
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
            CaretAssert(0);
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
        {
            ChartDataCartesian* cdc = dynamic_cast<ChartDataCartesian*>(chartData);
            CaretAssert(cdc);
            QSharedPointer<ChartDataCartesian> cdcPtr(cdc);
            for (std::vector<int32_t>::const_iterator iter = tabIndices.begin();
                 iter != tabIndices.end();
                 iter++) {
                const int32_t tabIndex = *iter;
                CaretAssertArrayIndex(m_chartModelDataSeries, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
                m_chartModelDataSeries[tabIndex]->addChartData(cdcPtr);
            }
            m_dataSeriesChartData.push_front(cdcPtr.toWeakRef());
        }
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
        {
            ChartDataCartesian* cdc = dynamic_cast<ChartDataCartesian*>(chartData);
            CaretAssert(cdc);
            QSharedPointer<ChartDataCartesian> cdcPtr(cdc);
            for (std::vector<int32_t>::const_iterator iter = tabIndices.begin();
                 iter != tabIndices.end();
                 iter++) {
                const int32_t tabIndex = *iter;
                CaretAssertArrayIndex(m_chartModelFrequencySeries, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
                m_chartModelFrequencySeries[tabIndex]->addChartData(cdcPtr);
            }
            m_frequencySeriesChartData.push_front(cdcPtr.toWeakRef());
        }
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
        {
            ChartDataCartesian* cdc = dynamic_cast<ChartDataCartesian*>(chartData);
            CaretAssert(cdc);
            QSharedPointer<ChartDataCartesian> cdcPtr(cdc);
            for (std::vector<int32_t>::const_iterator iter = tabIndices.begin();
                 iter != tabIndices.end();
                 iter++) {
                const int32_t tabIndex = *iter;
                CaretAssertArrayIndex(m_chartModelTimeSeries, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
                m_chartModelTimeSeries[tabIndex]->addChartData(cdcPtr);
            }
            m_timeSeriesChartData.push_front(cdcPtr.toWeakRef());
        }
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
            CaretAssert(0);
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
            break;
    }
}

/**
 * Get tabs and brainordinate chart files for loading chart data.
 *
 * @param chartBrainordinateFileEnabledTabsOut
 *    Map with first being a chartable file and the second being
 *    tabs for which that chartable file is enabled.
 */
void
ModelChart::getTabsAndBrainordinateChartFilesForLineChartLoading(std::map<ChartableLineSeriesBrainordinateInterface*, std::vector<int32_t> >& chartBrainordinateFileEnabledTabsOut) const
{
    chartBrainordinateFileEnabledTabsOut.clear();
    
    std::map<ChartableLineSeriesInterface*, std::vector<int32_t> > chartFileEnabledTabs;
    getTabsAndLineSeriesChartFilesForLineChartLoading(chartFileEnabledTabs);
    
    for (std::map<ChartableLineSeriesInterface*, std::vector<int32_t> >::iterator iter = chartFileEnabledTabs.begin();
         iter != chartFileEnabledTabs.end();
         iter++) {
        ChartableLineSeriesBrainordinateInterface* brainChartFile = dynamic_cast<ChartableLineSeriesBrainordinateInterface*>(iter->first);
        if (brainChartFile != NULL) {
            chartBrainordinateFileEnabledTabsOut.insert(std::make_pair(brainChartFile,
                                                                       iter->second));
        }
    }
}


/**
 * Get tabs and row column chart files for loading chart data.
 *
 * @param chartRowColumnFilesEnabledTabsOut
 *    Map with first being a chartable file and the second being
 *    tabs for which that chartable file is enabled.
 */
void
ModelChart::getTabsAndRowColumnChartFilesForLineChartLoading(std::map<ChartableLineSeriesRowColumnInterface*, std::vector<int32_t> >& chartRowColumnFilesEnabledTabsOut) const
{
    chartRowColumnFilesEnabledTabsOut.clear();
    
    std::map<ChartableLineSeriesInterface*, std::vector<int32_t> > chartFileEnabledTabs;
    getTabsAndLineSeriesChartFilesForLineChartLoading(chartFileEnabledTabs);
    
    for (std::map<ChartableLineSeriesInterface*, std::vector<int32_t> >::iterator iter = chartFileEnabledTabs.begin();
         iter != chartFileEnabledTabs.end();
         iter++) {
        ChartableLineSeriesRowColumnInterface* rowColChartFile = dynamic_cast<ChartableLineSeriesRowColumnInterface*>(iter->first);
        if (rowColChartFile != NULL) {
            chartRowColumnFilesEnabledTabsOut.insert(std::make_pair(rowColChartFile,
                                                                       iter->second));
        }
    }
}

/**
 * Get line series chart files for loading chart data.
 *
 * @param chartFileEnabledTabsOut
 *    Map with first being a chartable file and the second being
 *    tabs for which that chartable file is enabled.
 */
void
ModelChart::getTabsAndLineSeriesChartFilesForLineChartLoading(std::map<ChartableLineSeriesInterface*, std::vector<int32_t> >& chartFileEnabledTabsOut) const
{
    chartFileEnabledTabsOut.clear();
    
    EventBrowserTabGetAll allTabsEvent;
    EventManager::get()->sendEvent(allTabsEvent.getPointer());
    std::vector<int32_t> validTabIndices = allTabsEvent.getBrowserTabIndices();
    
    std::vector<ChartableLineSeriesInterface*> chartFiles;
    m_brain->getAllChartableLineSeriesDataFiles(chartFiles);
    
    for (std::vector<ChartableLineSeriesInterface*>::iterator iter = chartFiles.begin();
         iter != chartFiles.end();
         iter++) {
        ChartableLineSeriesInterface* cf = *iter;
        std::vector<int32_t> chartFileTabIndices;
        
        for (std::vector<int32_t>::iterator tabIter = validTabIndices.begin();
             tabIter != validTabIndices.end();
             tabIter++) {
            const int32_t tabIndex = *tabIter;
            if (cf->isLineSeriesChartingEnabled(tabIndex)) {
                chartFileTabIndices.push_back(tabIndex);
            }
        }
        
        if ( ! chartFileTabIndices.empty()) {
            chartFileEnabledTabsOut.insert(std::make_pair(cf, chartFileTabIndices));
        }
    }
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
ModelChart::loadChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                        const int32_t surfaceNumberOfNodes,
                                        const int32_t nodeIndex)
{
    std::map<ChartableLineSeriesBrainordinateInterface*, std::vector<int32_t> > chartFileEnabledTabs;
    getTabsAndBrainordinateChartFilesForLineChartLoading(chartFileEnabledTabs);
    
    for (std::map<ChartableLineSeriesBrainordinateInterface*, std::vector<int32_t> >::iterator fileTabIter = chartFileEnabledTabs.begin();
         fileTabIter != chartFileEnabledTabs.end();
         fileTabIter++) {
        ChartableLineSeriesBrainordinateInterface* chartFile = fileTabIter->first;
        const std::vector<int32_t>  tabIndices = fileTabIter->second;

        CaretAssert(chartFile);
        ChartData* chartData = chartFile->loadLineSeriesChartDataForSurfaceNode(structure,
                                               nodeIndex);
        if (chartData != NULL) {
            ChartDataSource* dataSource = chartData->getChartDataSource();
            dataSource->setSurfaceNode(chartFile->getLineSeriesChartCaretMappableDataFile()->getFileName(),
                                       StructureEnum::toName(structure),
                                       surfaceNumberOfNodes,
                                       nodeIndex);
            
            addChartToChartModels(tabIndices,
                                  chartData);
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
ModelChart::receiveEvent(Event* event)
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
        
        for (std::list<QWeakPointer<ChartDataCartesian> >::iterator dsIter = m_dataSeriesChartData.begin();
             dsIter != m_dataSeriesChartData.end();
             dsIter++) {
            QSharedPointer<ChartDataCartesian> spCart = dsIter->toStrongRef();
            if ( ! spCart.isNull()) {
                cartesianChartData.push_back(spCart.data());
            }
        }
        for (std::list<QWeakPointer<ChartDataCartesian> >::iterator tsIter = m_frequencySeriesChartData.begin();
             tsIter != m_frequencySeriesChartData.end();
             tsIter++) {
            QSharedPointer<ChartDataCartesian> spCart = tsIter->toStrongRef();
            if ( ! spCart.isNull()) {
                cartesianChartData.push_back(spCart.data());
            }
        }
        for (std::list<QWeakPointer<ChartDataCartesian> >::iterator tsIter = m_timeSeriesChartData.begin();
             tsIter != m_timeSeriesChartData.end();
             tsIter++) {
            QSharedPointer<ChartDataCartesian> spCart = tsIter->toStrongRef();
            if ( ! spCart.isNull()) {
                cartesianChartData.push_back(spCart.data());
            }
        }
        
        
        /*
         * Iterate over node indices for which colors are desired.
         */
        const std::vector<int32_t> nodeIndices = nodeChartID->getNodeIndices();
        for (std::vector<int32_t>::const_iterator nodeIter = nodeIndices.begin();
             nodeIter != nodeIndices.end();
             nodeIter++) {
            const int32_t nodeIndex = *nodeIter;
            
            /*
             * Iterate over the data in the cartesian chart
             */
            for (std::vector<ChartDataCartesian*>::iterator cdIter = cartesianChartData.begin();
                 cdIter != cartesianChartData.end();
                 cdIter++) {
                const ChartDataCartesian* cdc = *cdIter;
                const ChartDataSource* cds = cdc->getChartDataSource();
                if (cds->isSurfaceNodeSourceOfData(structureName, nodeIndex)) {
                    /*
                     * Found node index so add its color to the event
                     */
                    const CaretColorEnum::Enum color = cdc->getColor();
                    const float* rgb = CaretColorEnum::toRGB(color);
                    nodeChartID->addNode(nodeIndex,
                                         rgb);
                    break;
                }
            }
        }

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
ModelChart::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    AString name = "ChartOld";
    return name;
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model.
 */
AString 
ModelChart::getNameForBrowserTab() const
{
    AString name = "ChartOld";
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
ModelChart::getOverlaySet(const int tabIndex)
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
ModelChart::getOverlaySet(const int tabIndex) const
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
ModelChart::initializeOverlays()
{
    m_overlaySetArray->initializeOverlaySelections();
}

/**
 * Save version one charting information specific to this type of model to the scene.
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
ModelChart::saveVersionOneModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                           SceneClass* sceneClass)
{
    std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    std::set<AString> validChartDataIDs;
    saveChartModelsToScene(sceneAttributes,
                           sceneClass,
                           tabIndices,
                           validChartDataIDs);
    
    sceneClass->addEnumeratedTypeArrayForTabIndices<ChartOneDataTypeEnum, ChartOneDataTypeEnum::Enum>("m_selectedChartDataType",
                                                                                                      m_selectedChartOneDataType,
                                                                                                      tabIndices);
    
    /*
     * Save matrix chart models to scene.
     */
    SceneObjectMapIntegerKey* matrixSceneMap = new SceneObjectMapIntegerKey("chartableMatrixFileSelectionModelMap",
                                                                            SceneObjectDataTypeEnum::SCENE_CLASS);
    
    std::vector<SceneClass*> matrixSelectionVector;
    for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
         tabIter != tabIndices.end();
         tabIter++) {
        const int32_t tabIndex = *tabIter;
        
        matrixSceneMap->addClass(tabIndex, m_chartableMatrixFileSelectionModel[tabIndex]->saveToScene(sceneAttributes,
                                                                                                      "m_chartableMatrixFileSelectionModel"));
    }
    sceneClass->addChild(matrixSceneMap);
    
    /*
     * Save matrix series chart models to scene.
     */
    SceneObjectMapIntegerKey* matrixSeriesSceneMap = new SceneObjectMapIntegerKey("chartableMatrixSeriesFileSelectionModelMap",
                                                                                  SceneObjectDataTypeEnum::SCENE_CLASS);
    
    std::vector<SceneClass*> matrixSeriesSelectionVector;
    for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
         tabIter != tabIndices.end();
         tabIter++) {
        const int32_t tabIndex = *tabIter;
        
        matrixSeriesSceneMap->addClass(tabIndex, m_chartableMatrixSeriesFileSelectionModel[tabIndex]->saveToScene(sceneAttributes,
                                                                                                                  "m_chartableMatrixSeriesFileSelectionModel"));
    }
    sceneClass->addChild(matrixSeriesSceneMap);
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
ModelChart::saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                      SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    saveVersionOneModelSpecificInformationToScene(sceneAttributes,
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
ModelChart::restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                     const SceneClass* sceneClass)
{
    reset();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    restoreVersionOneModelSpecificInformationFromScene(sceneAttributes,
                                                       sceneClass);
}

/**
 * Restore information specific to the type of model from VERSION ONE scene.
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
ModelChart::restoreVersionOneModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                           const SceneClass* sceneClass)
{
    /*
     * Restore the chart models
     */
    restoreVersionOneChartModelsFromScene(sceneAttributes,
                                          sceneClass);
    
    sceneClass->getEnumerateTypeArrayForTabIndices<ChartOneDataTypeEnum, ChartOneDataTypeEnum::Enum>("m_selectedChartDataType",
                                                                                               m_selectedChartOneDataType);

    /*
     * Restore matrix chart models from scene.
     */
    const SceneObjectMapIntegerKey* matrixSceneMap = sceneClass->getMapIntegerKey("chartableMatrixFileSelectionModelMap");
    if (matrixSceneMap != NULL) {
        const std::vector<int32_t> tabIndices = matrixSceneMap->getKeys();
        for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
             tabIter != tabIndices.end();
             tabIter++) {
            const int32_t tabIndex = *tabIter;
            const SceneClass* sceneClass = matrixSceneMap->classValue(tabIndex);
            m_chartableMatrixFileSelectionModel[tabIndex]->restoreFromScene(sceneAttributes,
                                                                            sceneClass);
        }
    }
    
    /*
     * Restore matrix chart series models from scene.
     */
    const SceneObjectMapIntegerKey* matrixSeriesSceneMap = sceneClass->getMapIntegerKey("chartableMatrixSeriesFileSelectionModelMap");
    if (matrixSeriesSceneMap != NULL) {
        const std::vector<int32_t> tabIndices = matrixSeriesSceneMap->getKeys();
        for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
             tabIter != tabIndices.end();
             tabIter++) {
            const int32_t tabIndex = *tabIter;
            const SceneClass* sceneClass = matrixSeriesSceneMap->classValue(tabIndex);
            m_chartableMatrixSeriesFileSelectionModel[tabIndex]->restoreFromScene(sceneAttributes,
                                                                            sceneClass);
        }
    }
}

/**
 * Save chart models to the scene.
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
ModelChart::saveChartModelsToScene(const SceneAttributes* sceneAttributes,
                            SceneClass* sceneClass,
                            const std::vector<int32_t>& tabIndices,
                            std::set<AString>& validChartDataIDsOut)
{
    validChartDataIDsOut.clear();
    
    std::set<ChartData*> chartDataForSavingToSceneSet;
    
    /*
     * Save chart models to scene.
     */
    std::vector<SceneClass*> chartModelVector;
    for (std::vector<int32_t>::const_iterator tabIter = tabIndices.begin();
         tabIter != tabIndices.end();
         tabIter++) {
        const int32_t tabIndex = *tabIter;
        
        ChartModel* chartModel = NULL;
        switch (getSelectedChartOneDataType(tabIndex)) {
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                chartModel = getSelectedFrequencySeriesChartModel(tabIndex);
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                chartModel = getSelectedDataSeriesChartModel(tabIndex);
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                chartModel = getSelectedTimeSeriesChartModel(tabIndex);
                break;
        }
        
        if (chartModel != NULL) {
            SceneClass* chartModelClass = chartModel->saveToScene(sceneAttributes,
                                                                  "chartModel");
            if (chartModelClass == NULL) {
                continue;
            }
            
            SceneClass* chartClassContainer = new SceneClass("chartClassContainer",
                                                             "ChartClassContainer",
                                                             1);
            chartClassContainer->addInteger("tabIndex", tabIndex);
            chartClassContainer->addEnumeratedType<ChartOneDataTypeEnum,ChartOneDataTypeEnum::Enum>("chartDataType",
                                                                                              chartModel->getChartDataType());
            chartClassContainer->addClass(chartModelClass);
            
            chartModelVector.push_back(chartClassContainer);
            
            /*
             * Add chart data that is in models saved to scene.
             *
             */
            std::vector<ChartData*> chartDatasInModel = chartModel->getAllChartDatas();
            chartDataForSavingToSceneSet.insert(chartDatasInModel.begin(),
                                                chartDatasInModel.end());
        }
    }

    if ( ! chartModelVector.empty()) {
        SceneClassArray* modelArray = new SceneClassArray("chartModelArray",
                                                      chartModelVector);
        sceneClass->addChild(modelArray);
    }

    if ( ! chartDataForSavingToSceneSet.empty()) {
        std::vector<SceneClass*> chartDataClassVector;
        for (std::set<ChartData*>::iterator cdIter = chartDataForSavingToSceneSet.begin();
             cdIter != chartDataForSavingToSceneSet.end();
             cdIter++) {
            ChartData* chartData = *cdIter;
            SceneClass* chartDataClass = chartData->saveToScene(sceneAttributes,
                                                                "chartData");
            
            SceneClass* chartDataContainer = new SceneClass("chartDataContainer",
                                                            "ChartDataContainer",
                                                            1);
            chartDataContainer->addEnumeratedType<ChartOneDataTypeEnum, ChartOneDataTypeEnum::Enum>("chartDataType",
                                                                                              chartData->getChartDataType());
            chartDataContainer->addClass(chartDataClass);
            
            chartDataClassVector.push_back(chartDataContainer);
        }
        
        if ( ! chartDataClassVector.empty()) {
            SceneClassArray* dataArray = new SceneClassArray("chartDataArray",
                                                             chartDataClassVector);
            sceneClass->addChild(dataArray);
        }
    }
}

/**
 * Restore the chart models from a VERSION ONE scene.
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
ModelChart::restoreVersionOneChartModelsFromScene(const SceneAttributes* sceneAttributes,
                                 const SceneClass* sceneClass)
{
    /*
     * Restore the chart models
     */
    const SceneClassArray* chartModelArray = sceneClass->getClassArray("chartModelArray");
    if (chartModelArray != NULL) {
        const int numElements = chartModelArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numElements; i++) {
            const SceneClass* chartClassContainer = chartModelArray->getClassAtIndex(i);
            if (chartClassContainer != NULL) {
                const int32_t tabIndex = chartClassContainer->getIntegerValue("tabIndex", -1);
                const ChartOneDataTypeEnum::Enum chartDataType =  chartClassContainer->getEnumeratedTypeValue<ChartOneDataTypeEnum, ChartOneDataTypeEnum::Enum>("chartDataType",
                                                                                                        ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID);
                const SceneClass* chartModelClass = chartClassContainer->getClass("chartModel");
                
                if ((tabIndex >= 0)
                    && (chartDataType != ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID)
                    && (chartModelClass != NULL)) {
                    CaretAssertArrayIndex(m_chartModelDataSeries,
                                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                          tabIndex);
                    
                    switch (chartDataType) {
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                            CaretAssert(0);
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                            m_chartModelDataSeries[tabIndex]->restoreFromScene(sceneAttributes,
                                                                               chartModelClass);
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                            m_chartModelFrequencySeries[tabIndex]->restoreFromScene(sceneAttributes,
                                                                                    chartModelClass);
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                            m_chartModelTimeSeries[tabIndex]->restoreFromScene(sceneAttributes,
                                                                               chartModelClass);
                            break;
                    }
                }
            }
        }
    }
    
    /*
     * Restore the chart data.
     */
    std::vector<QSharedPointer<ChartData> > restoredChartData;
    const SceneClassArray* chartDataArray = sceneClass->getClassArray("chartDataArray");
    if (chartDataArray != NULL) {
        const int numElements = chartDataArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numElements; i++) {
            const SceneClass* chartDataContainer = chartDataArray->getClassAtIndex(i);
            if (chartDataContainer != NULL) {
                const ChartOneDataTypeEnum::Enum chartDataType = chartDataContainer->getEnumeratedTypeValue<ChartOneDataTypeEnum, ChartOneDataTypeEnum::Enum>("chartDataType",
                                                                                                                                                       ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID);
                const SceneClass* chartDataClass = chartDataContainer->getClass("chartData");
                if ((chartDataType != ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID)
                    && (chartDataClass != NULL)) {
                    ChartData* chartData = ChartData::newChartDataForChartDataType(chartDataType);
                    CaretAssert(chartData);
                    
                    /*
                     * The chart's points are saved in the scene and this
                     * function call restores the points.  This is part of
                     * the original implementation.  However, it was decided
                     * that the when the scene is restored, the data should
                     * be loaded from the file to reflect any changes made
                     * to the data file.  But, we still load the chart points
                     * saved to the scene in case the file is missing.
                     */
                    chartData->restoreFromScene(sceneAttributes, chartDataClass);
                    
                    /*
                     * Now load the chart data from the file using the 
                     * information in the chart's data source.  If this is
                     * successful, then overwrite the chart data points 
                     * that were loaded from the scene file.
                     *
                     * Also need to copy the unique identifier so that
                     * the chart data goes to the correct model.
                     */
                    ChartData* newChartData = loadCartesianChartWhenRestoringScene(chartData);
                    if (newChartData != NULL) {
                        newChartData->setUniqueIdentifier(chartData->getUniqueIdentifier());
                        newChartData->copySelectionStatusForAllTabs(chartData);
                        delete chartData;
                        chartData = newChartData;
                    }
                    else {
                        const AString msg("FAILED to load line chart data from file for "
                                          + chartData->getChartDataSource()->getDescription()
                                          + " so chart points from scene will be used.  If content of the file "
                                          " has changed since the scene was created, the chart may not be accurate.");
                        sceneAttributes->addToErrorMessage(msg);
                        CaretLogWarning(msg);
                    }

                    restoredChartData.push_back(QSharedPointer<ChartData>(chartData));
                }
            }
        }
    }
    
    /*
     * Have chart models restore pointers to chart data
     * The chart models use shared pointers are used since the chart 
     * data may be in multiple tabs.  User may remove the charts
     * from some tabs but not others and shared pointers make management
     * easier.
     */
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartModelDataSeries[i]->restoreChartDataFromScene(sceneAttributes,
                                                             restoredChartData);
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartModelFrequencySeries[i]->restoreChartDataFromScene(sceneAttributes,
                                                                  restoredChartData);
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartModelTimeSeries[i]->restoreChartDataFromScene(sceneAttributes,
                                                             restoredChartData);
    }
    
    /*
     * The chart data are also saved here as weak pointers so
     * that they can be saved to a scene only one time.  If 
     */
    for (std::vector<QSharedPointer<ChartData> >::iterator rcdIter = restoredChartData.begin();
         rcdIter != restoredChartData.end();
         rcdIter++) {
        QSharedPointer<ChartData> chartPointer = *rcdIter;
        
        switch (chartPointer->getChartDataType()) {
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                CaretAssert(0);
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                CaretAssert(0);
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                CaretAssert(0);
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
            {
                QSharedPointer<ChartDataCartesian> cartChartPointer = chartPointer.dynamicCast<ChartDataCartesian>();
                CaretAssert( ! cartChartPointer.isNull());
                m_dataSeriesChartData.push_back(cartChartPointer);
            }
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
            {
                QSharedPointer<ChartDataCartesian> cartChartPointer = chartPointer.dynamicCast<ChartDataCartesian>();
                CaretAssert( ! cartChartPointer.isNull());
                m_frequencySeriesChartData.push_back(cartChartPointer);
            }
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
            {
                QSharedPointer<ChartDataCartesian> cartChartPointer = chartPointer.dynamicCast<ChartDataCartesian>();
                CaretAssert( ! cartChartPointer.isNull());
                m_timeSeriesChartData.push_back(cartChartPointer);
            }
                break;
        }
    }
}

/**
 * Load the cartesian chart data using the given chart data source.
 *
 * @param chartData
 *    ChartData that is cast to ChartDataCartesian and if successful,
 *    the chart is duplicated using data from files.
 * @return
 *    Pointer to successfully loaded chart data or NULL if not found or error.
 */
ChartData*
ModelChart::loadCartesianChartWhenRestoringScene(const ChartData* chartData)
{
    CaretAssert(chartData);
    
    ChartDataCartesian* chartDataOut = NULL;
    
    const ChartDataCartesian* cartesianChart = dynamic_cast<const ChartDataCartesian*>(chartData);
    if (cartesianChart != NULL) {
        const ChartOneDataTypeEnum::Enum chartDataType = cartesianChart->getChartDataType();

        std::vector<ChartableLineSeriesInterface*> chartableDataFiles;
        m_brain->getAllChartableLineSeriesDataFiles(chartableDataFiles);
        
        for (std::vector<ChartableLineSeriesInterface*>::iterator iter = chartableDataFiles.begin();
             iter != chartableDataFiles.end();
             iter++) {
            
            ChartableLineSeriesBrainordinateInterface* chartableBrainFile = dynamic_cast<ChartableLineSeriesBrainordinateInterface*>(*iter);
            ChartableLineSeriesRowColumnInterface* chartableRowColumnFile = dynamic_cast<ChartableLineSeriesRowColumnInterface*>(*iter);
            
            if (chartableBrainFile != NULL) {
                if (chartableBrainFile->isLineSeriesChartDataTypeSupported(chartDataType)) {
                    CaretMappableDataFile* chartMapFile = chartableBrainFile->getLineSeriesChartCaretMappableDataFile();
                    CaretAssert(chartMapFile);
                    
                    const ChartDataSource* chartDataSource = cartesianChart->getChartDataSource();
                    CaretAssert(chartDataSource);
                    
                    const AString chartMapFileName = chartMapFile->getFileName();
                    const AString chartSourceFileName = chartDataSource->getChartableFileName();
                    
                    if (chartMapFileName == chartSourceFileName) {
                        const ChartDataSourceModeEnum::Enum sourceMode = chartDataSource->getDataSourceMode();
                        
                        switch (sourceMode) {
                            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_INVALID:
                                break;
                            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_FILE_ROW:
                            {
                                CaretAssert(0);
                            }
                                break;
                            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDEX:
                            {
                                AString structureName;
                                int32_t surfaceNumberOfNodes = -1;
                                int32_t nodeIndex = -1;
                                chartDataSource->getSurfaceNode(structureName,
                                                                surfaceNumberOfNodes,
                                                                nodeIndex);
                                
                                bool structureNameValid = false;
                                const StructureEnum::Enum structure = StructureEnum::fromName(structureName,
                                                                                              &structureNameValid);
                                
                                chartDataOut = chartableBrainFile->loadLineSeriesChartDataForSurfaceNode(structure,
                                                                                                     nodeIndex);
                            }
                                break;
                            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDICES_AVERAGE:
                            {
                                AString structureName;
                                int32_t surfaceNumberOfNodes = -1;
                                std::vector<int32_t> nodeIndices;
                                chartDataSource->getSurfaceNodeAverage(structureName,
                                                                       surfaceNumberOfNodes,
                                                                       nodeIndices);
                                
                                bool structureNameValid = false;
                                const StructureEnum::Enum structure = StructureEnum::fromName(structureName,
                                                                                              &structureNameValid);
                                
                                chartDataOut = chartableBrainFile->loadAverageLineSeriesChartDataForSurfaceNodes(structure,
                                                                                                             nodeIndices);
                            }
                                break;
                            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_VOXEL_IJK:
                            {
                                float voxelXYZ[3];
                                chartDataSource->getVolumeVoxel(voxelXYZ);
                                
                                chartDataOut= chartableBrainFile->loadLineSeriesChartDataForVoxelAtCoordinate(voxelXYZ);
                            }
                                break;
                        }
                    }
                }
            }
            
            if (chartableRowColumnFile != NULL) {
                if (chartableRowColumnFile->isLineSeriesChartDataTypeSupported(chartDataType)) {
                    CaretMappableDataFile* chartMapFile = chartableRowColumnFile->getLineSeriesChartCaretMappableDataFile();
                    CaretAssert(chartMapFile);
                    
                    const ChartDataSource* chartDataSource = cartesianChart->getChartDataSource();
                    CaretAssert(chartDataSource);
                    
                    const AString chartMapFileName = chartMapFile->getFileName();
                    const AString chartSourceFileName = chartDataSource->getChartableFileName();
                    
                    if (chartMapFileName == chartSourceFileName) {
                        const ChartDataSourceModeEnum::Enum sourceMode = chartDataSource->getDataSourceMode();
                        
                        switch (sourceMode) {
                            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_INVALID:
                                break;
                            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_FILE_ROW:
                            {
                                AString chartFileName;
                                int32_t fileRowIndex;
                                chartDataSource->getFileRow(chartFileName,
                                                            fileRowIndex);
                                
                                chartDataOut = chartableRowColumnFile->loadLineSeriesChartDataForRow(fileRowIndex);
                            }
                                break;
                            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDEX:
                            {
                                CaretAssert(0);
                            }
                                break;
                            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDICES_AVERAGE:
                            {
                                CaretAssert(0);
                            }
                                break;
                            case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_VOXEL_IJK:
                            {
                                CaretAssert(0);
                            }
                                break;
                        }
                    }
                }
            }
        }
    }

    if (chartDataOut != NULL) {
        /*
         * Copy the source of the chart (node, surface, voxel, etc)
         */
        chartDataOut->getChartDataSource()->copy(chartData->getChartDataSource());
        const ChartDataCartesian* chartCartData = dynamic_cast<const ChartDataCartesian*>(chartData);
        if (chartCartData != NULL) {
            chartDataOut->setColor(chartCartData->getColor());
        }
    }
    
    return chartDataOut;
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
ModelChart::getDescriptionOfContent(const int32_t tabIndex,
                                    PlainTextStringBuilder& descriptionOut) const
{
    ChartModel* chartModel = NULL;
    switch (getSelectedChartOneDataType(tabIndex)) {
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
        {
            CaretDataFileSelectionModel* sm = m_chartableMatrixFileSelectionModel[tabIndex];
            const CaretDataFile* caretFile = sm->getSelectedFile();
            if (caretFile != NULL) {
                descriptionOut.addLine("Matrix (layer) chart for: "
                                       + caretFile->getFileNameNoPath());
                return;
            }
        }
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
        {
            CaretDataFileSelectionModel* sm = m_chartableMatrixSeriesFileSelectionModel[tabIndex];
            const CaretDataFile* caretFile = sm->getSelectedFile();
            if (caretFile != NULL) {
                descriptionOut.addLine("Matrix (series) chart for: "
                                       + caretFile->getFileNameNoPath());
                return;
            }
        }
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
            chartModel = const_cast<ChartModelDataSeries*>(getSelectedDataSeriesChartModel(tabIndex));
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
            chartModel = const_cast<ChartModelFrequencySeries*>(getSelectedFrequencySeriesChartModel(tabIndex));
            break;
        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
            chartModel = const_cast<ChartModelTimeSeries*>(getSelectedTimeSeriesChartModel(tabIndex));
            break;
    }

    const ChartModel* chartModelConst = chartModel;
    
    if (chartModel != NULL) {
        descriptionOut.addLine("Chart Type: "
                               + ChartOneDataTypeEnum::toGuiName(chartModel->getChartDataType()));

        descriptionOut.pushIndentation();
        
        const std::vector<const ChartData*> cdVec = chartModelConst->getAllChartDatas();
        for (std::vector<const ChartData*>::const_iterator iter = cdVec.begin();
             iter != cdVec.end();
             iter++) {
            const ChartData* cd = *iter;
            if (cd->isSelected(tabIndex)) {
                descriptionOut.addLine(cd->getChartDataSource()->getDescription());
            }
        }
        
        if (chartModel->isAverageChartDisplaySupported()) {
            if (chartModel->isAverageChartDisplaySelected()) {
                descriptionOut.addLine("Average Chart Displayed");
            }
        }
        
        descriptionOut.popIndentation();
    }
    else {
        descriptionOut.addLine("No charts to display");
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
ModelChart::copyTabContent(const int32_t sourceTabIndex,
                      const int32_t destinationTabIndex)
{
    Model::copyTabContent(sourceTabIndex,
                          destinationTabIndex);
    
    m_overlaySetArray->copyOverlaySet(sourceTabIndex,
                                      destinationTabIndex);
    
    m_selectedChartOneDataType[destinationTabIndex] = m_selectedChartOneDataType[sourceTabIndex];
    *m_chartModelDataSeries[destinationTabIndex] = *m_chartModelDataSeries[sourceTabIndex];
    *m_chartModelFrequencySeries[destinationTabIndex] = *m_chartModelFrequencySeries[sourceTabIndex];
    *m_chartModelTimeSeries[destinationTabIndex] = *m_chartModelTimeSeries[sourceTabIndex];
    m_chartableMatrixFileSelectionModel[destinationTabIndex]->setSelectedFile(m_chartableMatrixFileSelectionModel[sourceTabIndex]->getSelectedFile());
    m_chartableMatrixSeriesFileSelectionModel[destinationTabIndex]->setSelectedFile(m_chartableMatrixSeriesFileSelectionModel[sourceTabIndex]->getSelectedFile());
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
ModelChart::setSelectedChartOneDataType(const int32_t tabIndex,
                              const ChartOneDataTypeEnum::Enum dataType)
{
    CaretAssertArrayIndex(m_selectedChartOneDataType,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_selectedChartOneDataType[tabIndex] = dataType;
}

/**
 * Get the valid chart data types based upon the currently loaded files.
 * 
 * @param validChartDataTypesOut
 *    Output containing valid chart data types.
 */
void
ModelChart::getValidChartOneDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& validChartDataTypesOut) const
{
    validChartDataTypesOut.clear();
    
    bool haveDataSeries   = false;
    bool haveFrequencySeries = false;
    bool haveMatrixLayers = false;
    bool haveMatrixSeries = false;
    bool haveTimeSeries   = false;

    std::vector<ChartableLineSeriesInterface*> allLineChartableFiles;
    m_brain->getAllChartableLineSeriesDataFiles(allLineChartableFiles);

    for (std::vector<ChartableLineSeriesInterface*>::iterator fileIter = allLineChartableFiles.begin();
         fileIter != allLineChartableFiles.end();
         fileIter++) {
        ChartableLineSeriesInterface* chartFile = *fileIter;
        
        std::vector<ChartOneDataTypeEnum::Enum> chartDataTypes;
        chartFile->getSupportedLineSeriesChartDataTypes(chartDataTypes);
        
        for (std::vector<ChartOneDataTypeEnum::Enum>::iterator typeIter = chartDataTypes.begin();
             typeIter != chartDataTypes.end();
             typeIter++) {
            const ChartOneDataTypeEnum::Enum cdt = *typeIter;
            switch (cdt) {
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                    haveDataSeries = true;
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                    haveFrequencySeries = true;
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                    haveTimeSeries = true;
                    break;
            }
        }
    }
    
    std::vector<ChartableMatrixInterface*> allMatrixChartableFiles;
    m_brain->getAllChartableMatrixDataFiles(allMatrixChartableFiles);
    
    for (std::vector<ChartableMatrixInterface*>::iterator fileIter = allMatrixChartableFiles.begin();
         fileIter != allMatrixChartableFiles.end();
         fileIter++) {
        ChartableMatrixInterface* chartFile = *fileIter;
        
        std::vector<ChartOneDataTypeEnum::Enum> chartDataTypes;
        chartFile->getSupportedMatrixChartDataTypes(chartDataTypes);
        
        for (std::vector<ChartOneDataTypeEnum::Enum>::iterator typeIter = chartDataTypes.begin();
             typeIter != chartDataTypes.end();
             typeIter++) {
            const ChartOneDataTypeEnum::Enum cdt = *typeIter;
            switch (cdt) {
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                    haveMatrixLayers = true;
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                    haveMatrixSeries = true;
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                    break;
                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                    break;
            }
        }
    }
    
    if (haveDataSeries) {
        validChartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES);
    }
    if (haveFrequencySeries) {
        validChartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES);
    }
    if (haveMatrixLayers) {
        validChartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER);
    }
    if (haveMatrixSeries) {
        validChartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES);
    }
    if (haveTimeSeries) {
        validChartDataTypesOut.push_back(ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES);
    }
}


/**
 * Get the type of chart selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Chart type in the given tab.
 */
ChartOneDataTypeEnum::Enum
ModelChart::getSelectedChartOneDataType(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_selectedChartOneDataType,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    ChartOneDataTypeEnum::Enum chartDataType = m_selectedChartOneDataType[tabIndex];
    
    /*
     * Verify that the selected chart data type is valid.
     */
    std::vector<ChartOneDataTypeEnum::Enum> validChartDataTypes;
    getValidChartOneDataTypes(validChartDataTypes);
    if (std::find(validChartDataTypes.begin(),
                  validChartDataTypes.end(),
                  chartDataType) == validChartDataTypes.end()) {
        chartDataType = ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID;
    }
    
    /*
     * If selected chart data type is invalid, find a valid chart type,
     * preferably one that contains data.
     */
    if (chartDataType == ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID) {
        if ( ! validChartDataTypes.empty()) {
            /*
             * Will become the the first valid chart data type that contains
             * data (if there is one)
             */
            ChartOneDataTypeEnum::Enum chartDataTypeWithValidData = ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID;
            
            /*
             * Loop through all chart types (some or all valid charts 
             * types may not contain data until the user commands loading of data)
             */
            std::vector<ChartOneDataTypeEnum::Enum> allChartDataTypes;
            ChartOneDataTypeEnum::getAllEnums(allChartDataTypes);
            for (std::vector<ChartOneDataTypeEnum::Enum>::iterator iter = allChartDataTypes.begin();
                 iter != allChartDataTypes.end();
                 iter++) {
                const ChartOneDataTypeEnum::Enum cdt = *iter;
                if (std::find(validChartDataTypes.begin(),
                              validChartDataTypes.end(),
                              cdt) != validChartDataTypes.end()) {
                    if (chartDataType == ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                        chartDataType = cdt;
                    }
                    
                    switch (cdt) {
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                            if (chartDataTypeWithValidData == ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                                chartDataTypeWithValidData = ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER;
                            }
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                            if (chartDataTypeWithValidData == ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                                chartDataTypeWithValidData = ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES;
                            }
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                            if (m_chartModelDataSeries[tabIndex]->getNumberOfChartData() > 0) {
                                if (chartDataTypeWithValidData == ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                                    chartDataTypeWithValidData = ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES;
                                }
                            }
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                            if (m_chartModelFrequencySeries[tabIndex]->getNumberOfChartData() > 0) {
                                if (chartDataTypeWithValidData == ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                                    chartDataTypeWithValidData = ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES;
                                }
                            }
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                            if (m_chartModelTimeSeries[tabIndex]->getNumberOfChartData() > 0) {
                                if (chartDataTypeWithValidData == ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                                    chartDataTypeWithValidData = ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES;
                                }
                            }
                            break;
                    }
                }
            }
            
            if (chartDataTypeWithValidData != ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                chartDataType = chartDataTypeWithValidData;
            }
            else if (chartDataType == ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID) {
                chartDataType = validChartDataTypes[0];
            }
        }
    }
    
    /*
     * Selected type may have changed due to loaded files changing
     */
    m_selectedChartOneDataType[tabIndex] = chartDataType;
    
    return chartDataType;
}


/**
 * Update the chart overlay sets.
 * 
 * @param tabIndex
 *     Index of the tab.
 */
void
ModelChart::updateChartOverlaySets(const int32_t /*tabIndex*/)
{
}

/**
 * Get the data series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Data series chart model in the given tab.
 */
ChartModelDataSeries*
ModelChart::getSelectedDataSeriesChartModel(const int32_t tabIndex)
{
    const ChartModelDataSeries* model = getSelectedDataSeriesChartModelHelper(tabIndex);
    if (model == NULL) {
        return NULL;
    }
    ChartModelDataSeries* nonConstModel = const_cast<ChartModelDataSeries*>(model);
    return nonConstModel;
}

/**
 * Get the data series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Data series chart model in the given tab.
 */
const ChartModelDataSeries*
ModelChart::getSelectedDataSeriesChartModel(const int32_t tabIndex) const
{
    return getSelectedDataSeriesChartModelHelper(tabIndex);
}

/**
 * Helper to get the data series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Data series chart model in the given tab.
 */
const ChartModelDataSeries*
ModelChart::getSelectedDataSeriesChartModelHelper(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartModelDataSeries, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartModelDataSeries[tabIndex];
}

/**
 * Get the time series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    time series chart model in the given tab.
 */
ChartModelFrequencySeries*
ModelChart::getSelectedFrequencySeriesChartModel(const int32_t tabIndex)
{
    const ChartModelFrequencySeries* model = getSelectedFrequencySeriesChartModelHelper(tabIndex);
    if (model == NULL) {
        return NULL;
    }
    ChartModelFrequencySeries* nonConstModel = const_cast<ChartModelFrequencySeries*>(model);
    return nonConstModel;
}

/**
 * Get the time series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    time series chart model in the given tab.
 */
const ChartModelFrequencySeries*
ModelChart::getSelectedFrequencySeriesChartModel(const int32_t tabIndex) const
{
    return getSelectedFrequencySeriesChartModelHelper(tabIndex);
}

/**
 * Helper to get the time series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    time series chart model in the given tab.
 */
const ChartModelFrequencySeries*
ModelChart::getSelectedFrequencySeriesChartModelHelper(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartModelFrequencySeries, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartModelFrequencySeries[tabIndex];
}

/**
 * Get the time series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    time series chart model in the given tab.
 */
ChartModelTimeSeries*
ModelChart::getSelectedTimeSeriesChartModel(const int32_t tabIndex)
{
    const ChartModelTimeSeries* model = getSelectedTimeSeriesChartModelHelper(tabIndex);
    if (model == NULL) {
        return NULL;
    }
    ChartModelTimeSeries* nonConstModel = const_cast<ChartModelTimeSeries*>(model);
    return nonConstModel;
}

/**
 * Get the time series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    time series chart model in the given tab.
 */
const ChartModelTimeSeries*
ModelChart::getSelectedTimeSeriesChartModel(const int32_t tabIndex) const
{
    return getSelectedTimeSeriesChartModelHelper(tabIndex);
}

/**
 * Helper to get the time series chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    time series chart model in the given tab.
 */
const ChartModelTimeSeries*
ModelChart::getSelectedTimeSeriesChartModelHelper(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartModelTimeSeries, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartModelTimeSeries[tabIndex];
}

/**
 * Get the chartable matrix parcel file selection model for the given tab.
 * 
 * @param tabIndex
 *     Index of the tab.
 * @return 
 *     Chartable file selection model for the tab.
 */
CaretDataFileSelectionModel*
ModelChart::getChartableMatrixParcelFileSelectionModel(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_chartableMatrixFileSelectionModel,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    
    return m_chartableMatrixFileSelectionModel[tabIndex];
}

/**
 * Get the chartable matrix series file selection model for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 *     Chartable file selection model for the tab.
 */
CaretDataFileSelectionModel*
ModelChart::getChartableMatrixSeriesFileSelectionModel(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_chartableMatrixSeriesFileSelectionModel,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    
    return m_chartableMatrixSeriesFileSelectionModel[tabIndex];
}

