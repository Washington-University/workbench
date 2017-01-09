
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

#define __CHART_TOOL_BOX_VIEW_CONTROLLER_DECLARE__
#include "ChartToolBoxViewController.h"
#undef __CHART_TOOL_BOX_VIEW_CONTROLLER_DECLARE__

#include <QStackedWidget>
#include <QTabWidget>
#include <QVBoxLayout>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ChartHistoryViewController.h"
#include "ChartModel.h"
#include "ChartModelDataSeries.h"
#include "ChartModelFrequencySeries.h"
#include "ChartModelTimeSeries.h"
#include "ChartSelectionViewController.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ModelChart.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;



    
/**
 * \class caret::ChartToolBoxViewController 
 * \brief View controller for the "Charting" tab of the overlay toolbox
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ChartToolBoxViewController::ChartToolBoxViewController(const Qt::Orientation orientation,
                                                       const int32_t browserWindowIndex,
                                                       QWidget* parent)
: QWidget(parent),
EventListenerInterface(),
m_browserWindowIndex(browserWindowIndex)
{
    /*
     * Data series widgets
     */
    m_chartSelectionViewController = new ChartSelectionViewController(orientation,
                                                                           browserWindowIndex,
                                                                           NULL);
    m_chartHistoryViewController = new ChartHistoryViewController(orientation,
                                                                       browserWindowIndex,
                                                                       NULL);
    m_tabWidget = new QTabWidget();
    m_tabWidget->addTab(m_chartSelectionViewController,
                                  "Loading");
    m_tabWidget->addTab(m_chartHistoryViewController,
                                  "History");
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_tabWidget);
    layout->addStretch();
    
    m_sceneAssistant = new SceneClassAssistant();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
ChartToolBoxViewController::~ChartToolBoxViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    delete m_sceneAssistant;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartToolBoxViewController::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(m_browserWindowIndex)
            || uiEvent->isToolBoxUpdate()) {
            uiEvent->setEventProcessed();
            
            ChartModel* chartModel = getSelectedChartModel();
            bool historyWidgetValid = false;
            
            if (chartModel != NULL) {
                
                switch (chartModel->getChartDataType()) {
                    case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                        break;
                    case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                        break;
                    case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                        break;
                    case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                        historyWidgetValid = true;
                        break;
                    case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                        historyWidgetValid = true;
                        break;
                    case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                        historyWidgetValid = true;
                        break;
                }
            }
            
            const int32_t historyTabIndex = m_tabWidget->indexOf(m_chartHistoryViewController);
            if (historyTabIndex >= 0) {
                m_tabWidget->setTabEnabled(historyTabIndex,
                                           historyWidgetValid);
            }
            if ( ! historyWidgetValid) {
                m_tabWidget->setCurrentWidget(m_chartSelectionViewController);
            }
        }
    }
}

/**
 * @return Chart model selected in the selected tab (NULL if not valid)
 */
ChartModel*
ChartToolBoxViewController::getSelectedChartModel()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return NULL;
    }
    
    ChartModel* chartModel = NULL;
    
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        const int32_t browserTabIndex = browserTabContent->getTabNumber();
        switch (modelChart->getSelectedChartOneDataType(browserTabIndex)) {
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                chartModel = modelChart->getSelectedDataSeriesChartModel(browserTabIndex);
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                chartModel = modelChart->getSelectedFrequencySeriesChartModel(browserTabIndex);
                break;
            case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                chartModel = modelChart->getSelectedTimeSeriesChartModel(browserTabIndex);
                break;
        }
    }
    
    return chartModel;
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
ChartToolBoxViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartToolBoxViewController",
                                            1);

    AString tabName;
    const int tabIndex = m_tabWidget->currentIndex();
    if ((tabIndex >= 0)
        && tabIndex < m_tabWidget->count()) {
        tabName = m_tabWidget->tabText(tabIndex);
    }
    sceneClass->addString("selectedChartTabName",
                          tabName);

    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
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
ChartToolBoxViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const AString tabName = sceneClass->getStringValue("selectedChartTabName",
                                                       "");
    for (int32_t i = 0; i < m_tabWidget->count(); i++) {
        if (m_tabWidget->tabText(i) == tabName) {
            m_tabWidget->setCurrentIndex(i);
            break;
        }
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

