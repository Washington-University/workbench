
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

#include <QAction>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QSignalMapper>
#include <QSpinBox>
#include <QToolButton>


#define __CHART_HISTORY_VIEW_CONTROLLER_DECLARE__
#include "ChartHistoryViewController.h"
#undef __CHART_HISTORY_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretColorEnumComboBox.h"
#include "ChartDataCartesian.h"
#include "ChartDataSource.h"
#include "ChartModelDataSeries.h"
#include "ChartModelFrequencySeries.h"
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
m_orientation(orientation),
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
    
    m_chartDataCheckBoxesSignalMapper = new QSignalMapper(this);
    QObject::connect(m_chartDataCheckBoxesSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(chartDataCheckBoxSignalMapped(int)));
    
    m_chartDataColorComboBoxesSignalMapper = new QSignalMapper(this);
    QObject::connect(m_chartDataColorComboBoxesSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(chartDataColorComboBoxSignalMapped(int)));
    
    m_chartDataColorConstructionButtonSignalMapper = new QSignalMapper(this);
    QObject::connect(m_chartDataColorConstructionButtonSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(chartDataConstructionToolButtonSignalMapped(int)));
    
    QWidget* chartDataWidget = new QWidget();
    m_chartDataGridLayout = new QGridLayout(chartDataWidget);
    m_chartDataGridLayout->setHorizontalSpacing(6);
    m_chartDataGridLayout->setVerticalSpacing(2);
    m_chartDataGridLayout->setContentsMargins(0, 0, 0, 0);
    m_chartDataGridLayout->setColumnStretch(COLUMN_CHART_DATA_CHECKBOX,     0);
    m_chartDataGridLayout->setColumnStretch(COLUMN_CHART_DATA_CONSTRUCTION, 0);
    m_chartDataGridLayout->setColumnStretch(COLUMN_CHART_DATA_COLOR,        0);
    m_chartDataGridLayout->setColumnStretch(COLUMN_CHART_DATA_NAME,       100);
    
    m_chartDataGridLayout->addWidget(new QLabel("On"),
                                     0, COLUMN_CHART_DATA_CHECKBOX,
                                     Qt::AlignHCenter);
    m_chartDataGridLayout->addWidget(new QLabel("Move"),
                                     0, COLUMN_CHART_DATA_CONSTRUCTION,
                                     Qt::AlignHCenter);
    m_chartDataGridLayout->addWidget(new QLabel("Color"),
                                     0, COLUMN_CHART_DATA_COLOR,
                                     Qt::AlignHCenter);
    
    QVBoxLayout* leftOrTopLayout = new QVBoxLayout();
    leftOrTopLayout->addWidget(m_averageCheckBox);
    leftOrTopLayout->addLayout(maxDisplayedLayout);
    leftOrTopLayout->addWidget(clearPushButton);
    leftOrTopLayout->addStretch();
    
    QVBoxLayout* rightOrBottomLayout = new QVBoxLayout();
    rightOrBottomLayout->addWidget(chartDataWidget);
    rightOrBottomLayout->addStretch();
    
    switch (m_orientation) {
        case Qt::Horizontal:
        {
            m_chartDataGridLayout->addWidget(new QLabel("Name"),
                                             0, COLUMN_CHART_DATA_NAME,
                                             Qt::AlignHCenter);
            
            QHBoxLayout* layout = new QHBoxLayout(this);
            layout->addLayout(leftOrTopLayout, 0);
            layout->addWidget(WuQtUtilities::createVerticalLineWidget());
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

    const int32_t numWidgetRows = static_cast<int32_t>(m_chartDataCheckBoxes.size());
    const int32_t maxItems = std::max(numData,
                                      numWidgetRows);
    for (int32_t i = 0; i < maxItems; i++) {
        if (i >= static_cast<int32_t>(m_chartDataCheckBoxes.size())) {
            /*
             * Checkbox
             */
            QCheckBox* checkBox = new QCheckBox(" ");
            QObject::connect(checkBox, SIGNAL(clicked(bool)),
                             m_chartDataCheckBoxesSignalMapper, SLOT(map()));
            m_chartDataCheckBoxesSignalMapper->setMapping(checkBox, i);
            m_chartDataCheckBoxes.push_back(checkBox);

            /*
             * Construction Tool Button
             */
            QIcon constructionIcon;
            const bool constructionIconValid = WuQtUtilities::loadIcon(":/LayersPanel/construction.png",
                                                                       constructionIcon);
            QToolButton* constructionToolButton = new QToolButton();
            if (constructionIconValid) {
                constructionToolButton->setIcon(constructionIcon);
            }
            else {
                constructionToolButton->setText("M");
            }
            m_chartDataContructionToolButtons.push_back(constructionToolButton);
            QObject::connect(constructionToolButton, SIGNAL(clicked()),
                             m_chartDataColorConstructionButtonSignalMapper, SLOT(map()));
            m_chartDataColorConstructionButtonSignalMapper->setMapping(constructionToolButton, i);
            
            /*
             * Color
             */
            CaretColorEnumComboBox* colorComboBox = new CaretColorEnumComboBox(this);
            QObject::connect(colorComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                             m_chartDataColorComboBoxesSignalMapper, SLOT(map()));
            m_chartDataColorComboBoxesSignalMapper->setMapping(colorComboBox, i);
            m_chartDataColorComboBoxes.push_back(colorComboBox);
            
            /*
             * Label
             */
            m_chartDataNameLabels.push_back(new QLabel());
            
            /*
             * Layout
             */
            const int widgetIndex = static_cast<int>(m_chartDataCheckBoxes.size()) - 1;
            CaretAssertVectorIndex(m_chartDataCheckBoxes, widgetIndex);
            
            const int row = m_chartDataGridLayout->rowCount();
            m_chartDataGridLayout->addWidget(m_chartDataCheckBoxes[widgetIndex],
                                             row, COLUMN_CHART_DATA_CHECKBOX);
            m_chartDataGridLayout->addWidget(constructionToolButton,
                                             row, COLUMN_CHART_DATA_CONSTRUCTION);
            m_chartDataGridLayout->addWidget(m_chartDataColorComboBoxes[widgetIndex]->getWidget(),
                                             row, COLUMN_CHART_DATA_COLOR);
            switch (m_orientation) {
                case Qt::Horizontal:
                {
                    m_chartDataGridLayout->addWidget(m_chartDataNameLabels[widgetIndex],
                                                     row, COLUMN_CHART_DATA_NAME);
                }
                    break;
                case Qt::Vertical:
                {
                    const int nextRow = m_chartDataGridLayout->rowCount();
                    m_chartDataGridLayout->addWidget(m_chartDataNameLabels[widgetIndex],
                                                     nextRow, COLUMN_CHART_DATA_CHECKBOX,
                                                     1, COLUMN_COUNT,
                                                     Qt::AlignLeft);
                }
                    break;
            }
        }
        
        CaretAssertVectorIndex(m_chartDataCheckBoxes, i);
        CaretAssertVectorIndex(m_chartDataContructionToolButtons, i);
        CaretAssertVectorIndex(m_chartDataColorComboBoxes, i);
        CaretAssertVectorIndex(m_chartDataNameLabels, i);
        
        QCheckBox* checkBox = m_chartDataCheckBoxes[i];
        QToolButton* constructToolButton = m_chartDataContructionToolButtons[i];
        CaretColorEnumComboBox* colorComboBox = m_chartDataColorComboBoxes[i];
        QLabel* nameLabel = m_chartDataNameLabels[i];
        
        bool showRowFlag = false;
        if (i < numData) {
            showRowFlag = true;

            const ChartData* chartData = chartDataVector[i];
            const ChartDataCartesian* chartDataCartesian = dynamic_cast<const ChartDataCartesian*>(chartData);
            const ChartDataSource* dataSource = chartData->getChartDataSource();
            
            checkBox->setChecked(chartData->isSelected(tabIndex));
            if (chartDataCartesian != NULL) {
                colorComboBox->setSelectedColor(chartDataCartesian->getColor());
                colorComboBox->getWidget()->setEnabled(true);
            }
            else {
                colorComboBox->getWidget()->setEnabled(false);
            }
            nameLabel->setText(dataSource->getDescription());
        }
        
        checkBox->setVisible(showRowFlag);
        constructToolButton->setVisible(showRowFlag);
        colorComboBox->getWidget()->setVisible(showRowFlag);
        nameLabel->setVisible(showRowFlag);
    }
    
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
            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                break;
            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                break;
            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                chartModelOut = modelChart->getSelectedDataSeriesChartModel(tabIndexOut);
                break;
            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                chartModelOut = modelChart->getSelectedFrequencySeriesChartModel(tabIndexOut);
                break;
            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                chartModelOut = modelChart->getSelectedTimeSeriesChartModel(tabIndexOut);
                break;
        }
    }
}

/**
 * Called when a check box is changed.
 *
 * @param indx
 *    Index of checkbox.
 */
void
ChartHistoryViewController::chartDataCheckBoxSignalMapped(int indx)
{
    CaretAssertVectorIndex(m_chartDataCheckBoxes, indx);
    ChartModel* chartModel = NULL;
    int32_t tabIndex = -1;
    getSelectedChartModelAndTab(chartModel,
                                tabIndex);
    if (chartModel == NULL) {
        return;
    }
    
    std::vector<ChartData*> chartDataVector = chartModel->getAllChartDatas();
    CaretAssertVectorIndex(chartDataVector, indx);
    chartDataVector[indx]->setSelected(tabIndex,
                                       m_chartDataCheckBoxes[indx]->isChecked());
    
    updateAfterSelectionsChanged();
}

/**
 * Called when a check box is changed.
 *
 * @param indx
 *    Index of checkbox.
 */
void
ChartHistoryViewController::chartDataColorComboBoxSignalMapped(int indx)
{
    CaretAssertVectorIndex(m_chartDataColorComboBoxes, indx);
    ChartModel* chartModel = NULL;
    int32_t tabIndex = -1;
    getSelectedChartModelAndTab(chartModel,
                                tabIndex);
    if (chartModel == NULL) {
        return;
    }
    
    std::vector<ChartData*> chartDataVector = chartModel->getAllChartDatas();
    CaretAssertVectorIndex(chartDataVector, indx);
    ChartDataCartesian* chartDataCartesian = dynamic_cast<ChartDataCartesian*>(chartDataVector[indx]);
    if (chartDataCartesian != NULL) {
        chartDataCartesian->setColor(m_chartDataColorComboBoxes[indx]->getSelectedColor());
    }
    
    updateAfterSelectionsChanged();
}

/**
 * Called when construction tool button is clicked
 *
 * @param indx
 *    Index of tool button.
 */
void
ChartHistoryViewController::chartDataConstructionToolButtonSignalMapped(int indx)
{
    CaretAssertVectorIndex(m_chartDataContructionToolButtons, indx);

    ChartModel* chartModel = NULL;
    int32_t tabIndex = -1;
    getSelectedChartModelAndTab(chartModel,
                                tabIndex);
    if (chartModel == NULL) {
        return;
    }
    
    std::vector<ChartData*> chartDataVector = chartModel->getAllChartDatas();
    const int32_t numCharts = static_cast<int32_t>(chartDataVector.size());
    
    QMenu menu(m_chartDataContructionToolButtons[indx]);
    
    QAction* moveUpAction = menu.addAction("Move Chart Up");
    if (indx <= 0) {
        moveUpAction->setEnabled(false);
    }
    
    QAction* moveDownAction = menu.addAction("Move Chart Down");
    if (indx >= (numCharts - 1)) {
        moveDownAction->setEnabled(false);
    }
    
    QAction* removeAction = menu.addAction("Remove Chart");
    
    QAction* selectedAction = menu.exec(m_chartDataContructionToolButtons[indx]->mapToGlobal(QPoint(0,0)));
    if (selectedAction != NULL) {
        if (selectedAction == moveUpAction) {
            chartModel->moveChartDataAtIndexToOneLowerIndex(indx);
        }
        else if (selectedAction == moveDownAction) {
            chartModel->moveChartDataAtIndexToOneHigherIndex(indx);
        }
        else if (selectedAction == removeAction) {
            chartModel->removeChartAtIndex(indx);
        }
        else {
            CaretAssertMessage(0, "Has a new action been added but not processed?");
        }
        
        this->updateAfterSelectionsChanged();
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

