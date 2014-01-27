/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <algorithm>
#include <cmath>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ChartData.h"
#include "ChartModelLineSeries.h"
#include "EventBrowserTabGetAll.h"
#include "EventChartsNewNotification.h"
#include "EventManager.h"
#include "ModelChart.h"
#include "OverlaySet.h"
#include "OverlaySetArray.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

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
                                            "Volume View");
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectedChartDataType[i] = ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES;
        
        m_chartModelDataSeries[i] =
        new ChartModelLineSeries(ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES,
                                 ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
                                 ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
        
        m_chartModelTimeSeries[i] =
        new ChartModelLineSeries(ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES,
                                 ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME,
                                 ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
    }
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_CHARTS_NEW_NOTIFICATION);
}

/**
 * Destructor
 */
ModelChart::~ModelChart()
{
    delete m_overlaySetArray;
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_chartModelDataSeries[i];
        
        delete m_chartModelTimeSeries[i];
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
    if (event->getEventType() == EventTypeEnum::EVENT_CHARTS_NEW_NOTIFICATION) {
        EventChartsNewNotification* newChartsEvent =
        dynamic_cast<EventChartsNewNotification*>(event);
        
        EventBrowserTabGetAll allTabsEvent;
        EventManager::get()->sendEvent(allTabsEvent.getPointer());
        
        const std::vector<int32_t> tabIndices = allTabsEvent.getBrowserTabIndices();
        for (std::vector<int32_t>::const_iterator iter = tabIndices.begin();
             iter != tabIndices.end();
             iter++) {
            const int32_t tabIndex = *iter;
            
            std::vector<QSharedPointer<ChartData> > chartDatas =
            newChartsEvent->getChartDatasForTabIndex(tabIndex);
            
            if ( ! chartDatas.empty()) {
                std::cout << "New charts for tab " << tabIndex << std::endl;
                
                for (std::vector<QSharedPointer<ChartData> >::iterator iter = chartDatas.begin();
                     iter != chartDatas.end();
                     iter++) {
                    QSharedPointer<ChartData> cdm = *iter;
                    const ChartDataTypeEnum::Enum chartDataDataType = cdm->getChartDataType();
                    switch (chartDataDataType) {
                        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                            CaretAssert(0);
                            break;
                        case ChartDataTypeEnum::CHART_DATA_TYPE_ADJACENCY_MATRIX:
                            CaretAssert(0);
                            break;
                        case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                            m_chartModelDataSeries[tabIndex]->addChartData(cdm);
                            break;
                        case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                            m_chartModelTimeSeries[tabIndex]->addChartData(cdm);
                            break;
                    }
                }
            }
        }
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
    AString name = "Chart";
    return name;
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model.
 */
AString 
ModelChart::getNameForBrowserTab() const
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
ModelChart::saveModelSpecificInformationToScene(const SceneAttributes* /*sceneAttributes*/,
                                                      SceneClass* /*sceneClass*/)
{
    /* nothing to add to scene */
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
ModelChart::restoreModelSpecificInformationFromScene(const SceneAttributes* /*sceneAttributes*/,
                                                           const SceneClass* /*sceneClass*/)
{
    /* nothing to restore from scene */
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
ModelChart::getDescriptionOfContent(const int32_t /*tabIndex*/,
                                      PlainTextStringBuilder& descriptionOut) const
{
    descriptionOut.addLine("Chart");
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
ModelChart::setSelectedChartDataType(const int32_t tabIndex,
                              const ChartDataTypeEnum::Enum dataType)
{
    CaretAssertArrayIndex(m_selectedChartDataType,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_selectedChartDataType[tabIndex] = dataType;
}

/**
 * Get the type of chart selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Chart type in the given tab.
 */
ChartDataTypeEnum::Enum
ModelChart::getSelectedChartDataType(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_selectedChartDataType,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_selectedChartDataType[tabIndex];
}

/**
 * Get the chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Chart model in the given tab or none if not valid.
 */
ChartModel*
ModelChart::getSelectedChartModel(const int32_t tabIndex)
{
    const ChartModel* model = getSelectedChartModelHelper(tabIndex);
    if (model == NULL) {
        return NULL;
    }
    ChartModel* nonConstModel = const_cast<ChartModel*>(model);
    return nonConstModel;
}

/**
 * Get the chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Chart model in the given tab or none if not valid.
 */
const ChartModel*
ModelChart::getSelectedChartModel(const int32_t tabIndex) const
{
    return getSelectedChartModelHelper(tabIndex);
}

/**
 * Get the chart model selected in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Chart model in the given tab or none if not valid.
 */
const ChartModel*
ModelChart::getSelectedChartModelHelper(const int32_t tabIndex) const
{
    const ChartDataTypeEnum::Enum chartType = getSelectedChartDataType(tabIndex);
    
    ChartModel* model = NULL;
    
    switch (chartType) {
        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_ADJACENCY_MATRIX:
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
            model = m_chartModelDataSeries[tabIndex];
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
            model = m_chartModelTimeSeries[tabIndex];
            break;
    }
    
    return model;
    
}


