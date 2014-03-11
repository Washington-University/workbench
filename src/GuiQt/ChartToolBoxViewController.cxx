
/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
                    case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                        historyWidgetValid = true;
                        break;
                    case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                        break;
                    case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                        break;
                    case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
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
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    ChartModel* chartModel = NULL;
    
    ModelChart* modelChart = brain->getChartModel();
    switch (modelChart->getSelectedChartDataType(browserTabIndex)) {
        case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
            chartModel = modelChart->getSelectedDataSeriesChartModel(browserTabIndex);
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
            chartModel = modelChart->getSelectedTimeSeriesChartModel(browserTabIndex);
            break;
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
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

