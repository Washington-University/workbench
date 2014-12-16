
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

#include <QBrush>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>

#define __CHART_HISTORY_VIEW_CONTROLLER_DECLARE__
#include "ChartHistoryViewController.h"
#undef __CHART_HISTORY_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ChartDataCartesian.h"
#include "ChartDataSource.h"
#include "ChartModelDataSeries.h"
#include "ChartModelTimeSeries.h"
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
ChartHistoryViewController::ChartHistoryViewController(const Qt::Orientation orientation,
                                                       const int32_t browserWindowIndex,
                                                       QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_averageCheckBox = new QCheckBox("Show Average");
    WuQtUtilities::setWordWrappedToolTip(m_averageCheckBox,
                                         "Display an average of the displayed chart data.   "
                                         "NOTE: If the charts contain a different number of points "
                                         "the average will be that of those charts that contain "
                                         "the same number of points as the most recently displayed "
                                         "chart.");
    QObject::connect(m_averageCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(averageCheckBoxClicked(bool)));
    
    QPushButton* clearPushButton = new QPushButton("Clear");
    clearPushButton->setFixedWidth(clearPushButton->sizeHint().width() + 20);
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
    
    m_chartDataTableWidget = new QTableWidget();
    m_chartDataTableWidget->horizontalHeader()->hide();
    m_chartDataTableWidget->verticalHeader()->hide();
    m_chartDataTableWidget->setGridStyle(Qt::NoPen);
    m_chartDataTableWidget->setColumnCount(COLUMN_COUNT);

    QObject::connect(m_chartDataTableWidget, SIGNAL(cellChanged(int,int)),
                     this, SLOT(chartDataTableCellChanged(int,int)));
    
    QVBoxLayout* leftOrTopLayout = new QVBoxLayout();
    leftOrTopLayout->addWidget(m_averageCheckBox);
    leftOrTopLayout->addLayout(maxDisplayedLayout);
    leftOrTopLayout->addWidget(clearPushButton);
    leftOrTopLayout->addStretch();
    
    QVBoxLayout* rightOrBottomLayout = new QVBoxLayout();
    rightOrBottomLayout->addWidget(m_chartDataTableWidget);
    rightOrBottomLayout->addStretch();
    
    switch (orientation) {
        case Qt::Horizontal:
        {
            QHBoxLayout* layout = new QHBoxLayout(this);
            layout->addLayout(leftOrTopLayout, 0);
            layout->addLayout(rightOrBottomLayout, 100);
            layout->addStretch();
        }
            break;
        case Qt::Vertical:
        {
            QVBoxLayout* layout = new QVBoxLayout(this);
            layout->addLayout(leftOrTopLayout, 0);
            layout->addLayout(rightOrBottomLayout, 100);
            layout->addStretch();
        }
            break;
    }
    

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
ChartHistoryViewController::~ChartHistoryViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Called when the content of a cell changes.
 * Update corresponding item in the spec file.
 *
 * @param rowIndex
 *    The row of the cell that was clicked.
 * @param columnIndex
 *    The columnof the cell that was clicked.
 */
void
ChartHistoryViewController::chartDataTableCellChanged(int rowIndex, int columnIndex)
{
    QTableWidgetItem* item = m_chartDataTableWidget->item(rowIndex, columnIndex);
    if (item != NULL) {
        if (columnIndex == COLUMN_CHART_DATA_CHECKBOX) {
            bool isSelected = WuQtUtilities::checkStateToBool(item->checkState());
            
            ChartModel* chartModel = NULL;
            int32_t tabIndex = -1;
            getSelectedChartModelAndTab(chartModel,
                                        tabIndex);
            if (chartModel == NULL) {
                return;
            }
            
            switch (chartModel->getChartSelectionMode()) {
                case ChartSelectionModeEnum::CHART_SELECTION_MODE_ANY:
                    break;
                case ChartSelectionModeEnum::CHART_SELECTION_MODE_SINGLE:
                    isSelected = true;
                    break;
            }
            
            std::vector<ChartData*> chartDataVector = chartModel->getAllChartDatas();
            CaretAssertVectorIndex(chartDataVector, rowIndex);
            chartDataVector[rowIndex]->setSelected(tabIndex,
                                                   isSelected);
            
            updateAfterSelectionsChanged();
        }
    }
}

/**
 * Gets called when the average checkbox is clicked.
 *
 * @param clicked
 *    New status.
 */
void
ChartHistoryViewController::averageCheckBoxClicked(bool clicked)
{
    ChartModel* chartModel = NULL;
    int32_t tabIndex = -1;
    getSelectedChartModelAndTab(chartModel,
                                tabIndex);
    if (chartModel == NULL) {
        return;
    }
    
    chartModel->setAverageChartDisplaySelected(clicked);
    
    updateAfterSelectionsChanged();
}

/**
 * Called when clear push button is clicked.
 */
void
ChartHistoryViewController::clearPushButtonClicked()
{
    ChartModel* chartModel = NULL;
    int32_t tabIndex = -1;
    getSelectedChartModelAndTab(chartModel,
                                tabIndex);
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
    ChartModel* chartModel = NULL;
    int32_t tabIndex = -1;
    getSelectedChartModelAndTab(chartModel,
                                tabIndex);
    if (chartModel == NULL) {
        return;
    }
    
    switch (chartModel->getChartSelectionMode()) {
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_SINGLE:
            break;
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_ANY:
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
    ChartModel* chartModel = NULL;
    int32_t tabIndex = -1;
    getSelectedChartModelAndTab(chartModel,
                                tabIndex);
    if (chartModel == NULL) {
        return;
    }
    
    switch (chartModel->getChartSelectionMode()) {
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_ANY:
            m_maximumDisplayedSpinBox->setEnabled(true);
            m_maximumDisplayedSpinBox->setValue(chartModel->getMaximumNumberOfChartDatasToDisplay());
            break;
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_SINGLE:
            m_maximumDisplayedSpinBox->setValue(1);
            m_maximumDisplayedSpinBox->setEnabled(false);
            break;
    }
    
    const std::vector<ChartData*> chartDataVector = chartModel->getAllChartDatas();
    const int32_t numData = static_cast<int32_t>(chartDataVector.size());

    m_chartDataTableWidget->setRowCount(numData);
    
    /*
     * Load the table widget
     */
    m_chartDataTableWidget->blockSignals(true);
    for (int32_t i = 0; i < numData; i++) {
        const ChartData* chartData = chartDataVector[i];
        
        const ChartDataSource* dataSource = chartData->getChartDataSource();
        const AString name = dataSource->getDescription();
        
        /*
         * Update checked status
         */
        QTableWidgetItem* checkItem = m_chartDataTableWidget->item(i,
                                                                   COLUMN_CHART_DATA_CHECKBOX);
        if (checkItem == NULL) {
            checkItem = new QTableWidgetItem();
            checkItem->setFlags(checkItem->flags()
                                | Qt::ItemIsUserCheckable);
            m_chartDataTableWidget->setItem(i,
                                            COLUMN_CHART_DATA_CHECKBOX,
                                            checkItem);
        }
        
        if (chartData->isSelected(tabIndex)) {
            checkItem->setCheckState(Qt::Checked);
        }
        else {
            checkItem->setCheckState(Qt::Unchecked);
        }
        
        /*
         * Update name
         */
        QTableWidgetItem* nameItem = m_chartDataTableWidget->item(i,
                                                                  COLUMN_CHART_DATA_NAME);
        if (nameItem == NULL) {
            nameItem = new QTableWidgetItem();
            m_chartDataTableWidget->setItem(i,
                                            COLUMN_CHART_DATA_NAME,
                                            nameItem);
        }
        nameItem->setText(name);
        
        /*
         * Update color
         */
        QTableWidgetItem* colorItem = m_chartDataTableWidget->item(i,
                                                                   COLUMN_CHART_DATA_COLOR);
        if (colorItem == NULL) {
            colorItem = new QTableWidgetItem();
            m_chartDataTableWidget->setItem(i,
                                            COLUMN_CHART_DATA_COLOR,
                                            colorItem);
            colorItem->setText("   ");
        }
        
        /**
         * Use the background color from the name's item
         * as the default color.
         */
        QColor chartColor = nameItem->background().color();
        const ChartDataCartesian* chartDataCartesian = dynamic_cast<const ChartDataCartesian*>(chartData);
        if (chartDataCartesian != NULL) {
            const CaretColorEnum::Enum color = chartDataCartesian->getColor();
            const float* rgb = CaretColorEnum::toRGB(color);
            chartColor.setRgbF(rgb[0], rgb[1], rgb[2]);
        }
        colorItem->setBackground(QBrush(chartColor));
    }
    m_chartDataTableWidget->blockSignals(false);
    
    m_chartDataTableWidget->resizeColumnsToContents();
    
    /*
     * Update averaging.
     */
    bool enableAvergeWidgetsFlag = false;
    if (chartModel != NULL) {
        if (chartModel->isAverageChartDisplaySupported()) {
            enableAvergeWidgetsFlag = true;
            m_averageCheckBox->setChecked(chartModel->isAverageChartDisplaySelected());
        }
    }
    m_averageCheckBox->setEnabled(enableAvergeWidgetsFlag);
}

/**
 * Get the chart model and the selected tab.
 *
 * @param chartModelOut
 *    Output containing chart model (may be NULL).
 * @param tabIndexOut
 *    Output containing tab index (negative if invalid).
 */
void
ChartHistoryViewController::getSelectedChartModelAndTab(ChartModel* &chartModelOut,
                                                        int32_t& tabIndexOut)
{
    chartModelOut = NULL;
    tabIndexOut   = -1;
    
    Brain* brain = GuiManager::get()->getBrain();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    tabIndexOut = browserTabContent->getTabNumber();
    
    ModelChart* modelChart = brain->getChartModel();
    if (modelChart != NULL) {
        switch (modelChart->getSelectedChartDataType(tabIndexOut)) {
            case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                chartModelOut = modelChart->getSelectedDataSeriesChartModel(tabIndexOut);
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                chartModelOut = modelChart->getSelectedTimeSeriesChartModel(tabIndexOut);
                break;
        }
    }
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

