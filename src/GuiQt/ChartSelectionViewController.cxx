
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

#define __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "ChartSelectionViewController.h"
#undef __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__

#include <QBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ChartLinesSelectionViewController.h"
#include "ChartMatrixParcelSelectionViewController.h"
#include "ChartMatrixSeriesSelectionViewController.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ModelChart.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::ChartSelectionViewController 
 * \brief Handles selection of charts displayed in chart model.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ChartSelectionViewController::ChartSelectionViewController(const Qt::Orientation orientation,
                                                           const int32_t browserWindowIndex,
                                                           QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_mode = MODE_INVALID;
    
    m_brainordinateChartWidget = new ChartLinesSelectionViewController(orientation,
                                                                       browserWindowIndex,
                                                                       parent);
    
    m_matrixParcelChartWidget = new ChartMatrixParcelSelectionViewController(orientation,
                                                                         browserWindowIndex,
                                                                         parent);
    
    m_matrixSeriesChartWidget = new ChartMatrixSeriesSelectionViewController(orientation,
                                                                         browserWindowIndex,
                                                                         parent);
    
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_brainordinateChartWidget);
    m_stackedWidget->addWidget(m_matrixParcelChartWidget);
    m_stackedWidget->addWidget(m_matrixSeriesChartWidget);

    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(m_stackedWidget);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
ChartSelectionViewController::~ChartSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update the view controller.
 */
void
ChartSelectionViewController::updateSelectionViewController()
{
    m_mode = MODE_INVALID;
    
    Brain* brain = GuiManager::get()->getBrain();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();

    ChartVersionOneDataTypeEnum::Enum chartDataType = ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_INVALID;
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        chartDataType = modelChart->getSelectedChartOneDataType(browserTabIndex);
    }
    
    switch (chartDataType) {
        case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
            m_mode = MODE_MATRIX_LAYER;
            break;
        case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
            m_mode = MODE_MATRIX_SERIES;
            break;
        case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
            m_mode = MODE_BRAINORDINATE;
            break;
        case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
            m_mode = MODE_BRAINORDINATE;
            break;
        case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
            m_mode = MODE_BRAINORDINATE;
            break;
    }
    
    switch (m_mode) {
        case MODE_INVALID:
            break;
        case MODE_BRAINORDINATE:
            m_stackedWidget->setCurrentWidget(m_brainordinateChartWidget);
            //m_brainordinateChartWidget->updateSelectionViewController();
            break;
        case MODE_MATRIX_LAYER:
            m_stackedWidget->setCurrentWidget(m_matrixParcelChartWidget);
            //m_matrixParcelChartWidget->updateSelectionViewController();
            break;
        case MODE_MATRIX_SERIES:
            m_stackedWidget->setCurrentWidget(m_matrixSeriesChartWidget);
            //m_matrixSeriesChartWidget->updateSelectionViewController();
            break;
    }
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartSelectionViewController::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(m_browserWindowIndex)
            || uiEvent->isToolBoxUpdate()) {
            this->updateSelectionViewController();
            uiEvent->setEventProcessed();
        }
    }
}


