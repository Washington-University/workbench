
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

#include <QBrush>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>

#define __CHART_HISTORY_VIEW_CONTROLLER_DECLARE__
#include "ChartHistoryViewController.h"
#undef __CHART_HISTORY_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ChartDataCartesian.h"
#include "ChartDataSource.h"
#include "ChartModelCartesian.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "GuiManager.h"
#include "ModelChart.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ChartHistoryViewController 
 * \brief Shows history of loaded charts in the selected tab.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ChartHistoryViewController::ChartHistoryViewController(const Qt::Orientation /*orientation*/,
                                                       const int32_t browserWindowIndex,
                                                       QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QPushButton* clearPushButton = new QPushButton("Clear");
    QObject::connect(clearPushButton, SIGNAL(clicked()),
                     this, SLOT(clearPushButtonClicked()));
    WuQtUtilities::setWordWrappedToolTip(clearPushButton,
                                         "Remove all charts of the selected type in this tab");
    
    QLabel* maximumDisplayedLabel = new QLabel("Show last ");
    m_maximumDisplayedSpinBox = new QSpinBox();
    m_maximumDisplayedSpinBox->setMinimum(1);
    m_maximumDisplayedSpinBox->setMaximum(1000);
    QObject::connect(m_maximumDisplayedSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(maximumDisplayedSpinBoxValueChanged(int)));
    QHBoxLayout* maxDisplayedLayout = new QHBoxLayout();
    maxDisplayedLayout->addWidget(maximumDisplayedLabel);
    maxDisplayedLayout->addWidget(m_maximumDisplayedSpinBox);
    maxDisplayedLayout->addStretch();
    WuQtUtilities::setWordWrappedToolTip(m_maximumDisplayedSpinBox,
                                         "Maximum number of charts of the selected type "
                                         "displayed in this tab");
    
    m_chartDataListWidget = new QListWidget();
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(clearPushButton);
    leftLayout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    leftLayout->addLayout(maxDisplayedLayout);
    leftLayout->addStretch();
    
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_chartDataListWidget);
    rightLayout->addStretch();
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addLayout(leftLayout, 0);
    layout->addLayout(rightLayout, 100); 
}

/**
 * Destructor.
 */
ChartHistoryViewController::~ChartHistoryViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Called when clear push button is clicked.
 */
void
ChartHistoryViewController::clearPushButtonClicked()
{
    ChartModel* chartModel = getSelectedChartModel();
    if (chartModel == NULL) {
        return;
    }
    
    chartModel->removeChartData();
    
    updateAfterSelectionsChanged();
}

/**
 * Called when maximum number of displayed charts is changed.
 *
 * @param value
 *    New value.
 */
void
ChartHistoryViewController::maximumDisplayedSpinBoxValueChanged(int value)
{
    ChartModel* chartModel = getSelectedChartModel();
    if (chartModel == NULL) {
        return;
    }
    
    switch (chartModel->getSupportForMultipleChartDisplay()) {
        case ChartModel::SUPPORTS_MULTIPLE_CHART_DISPLAY_TYPE_NO:
            break;
        case ChartModel::SUPPORTS_MULTIPLE_CHART_DISPLAY_TYPE_YES:
            chartModel->setMaximumNumberOfChartDatasToDisplay(value);
            break;
    }
    
    updateAfterSelectionsChanged();
}

/**
 * Update after selections in this view controller are changed.
 */
void
ChartHistoryViewController::updateAfterSelectionsChanged()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
    updateHistoryViewController();
}

/**
 * Update this view controller.
 */
void
ChartHistoryViewController::updateHistoryViewController()
{
    m_chartDataListWidget->clear();
    
    ChartModel* chartModel = getSelectedChartModel();
    if (chartModel == NULL) {
        return;
    }
    
    switch (chartModel->getSupportForMultipleChartDisplay()) {
        case ChartModel::SUPPORTS_MULTIPLE_CHART_DISPLAY_TYPE_NO:
            m_maximumDisplayedSpinBox->setValue(1);
            m_maximumDisplayedSpinBox->setEnabled(false);
            break;
        case ChartModel::SUPPORTS_MULTIPLE_CHART_DISPLAY_TYPE_YES:
            m_maximumDisplayedSpinBox->setEnabled(true);
            m_maximumDisplayedSpinBox->setValue(chartModel->getMaximumNumberOfChartDatasToDisplay());
            break;
    }
    
    ChartModelCartesian* cartesianChart = dynamic_cast<ChartModelCartesian*>(chartModel);
    
    if (cartesianChart != NULL) {
        std::vector<ChartData*> chartDataVector = cartesianChart->getChartDatasForDisplay();
        for (std::vector<ChartData*>::iterator chartDataIter = chartDataVector.begin();
             chartDataIter != chartDataVector.end();
             chartDataIter++) {
            const ChartData* chartData = *chartDataIter;
            const ChartDataCartesian* chartDataCartesian = dynamic_cast<const ChartDataCartesian*>(chartData);
            CaretAssert(chartDataCartesian);
            
            const ChartDataSource* dataSource = chartDataCartesian->getChartDataSource();
            const AString name = dataSource->toString();
            
            const CaretColorEnum::Enum color = chartDataCartesian->getColor();
            const float* rgb = CaretColorEnum::toRGB(color);
            QListWidgetItem* item = new QListWidgetItem(name);
            QBrush brush = item->foreground();
            brush.setColor(QColor::fromRgbF(rgb[0], rgb[1], rgb[2]));
            item->setForeground(brush);
            
            m_chartDataListWidget->addItem(item);
        }
    }
}

/**
 * @return Chart model selected in the selected tab (NULL if not valid)
 */
ChartModel*
ChartHistoryViewController::getSelectedChartModel()
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
    if (modelChart != NULL) {
        chartModel = modelChart->getSelectedChartModel(browserTabIndex);
    }
    
    return chartModel;
}


/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartHistoryViewController::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(m_browserWindowIndex)
            || uiEvent->isToolBoxUpdate()) {
            this->updateHistoryViewController();
            uiEvent->setEventProcessed();
        }
    }
}

