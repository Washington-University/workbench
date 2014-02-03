
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

#include <limits>

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_AXES_DECLARE__
#include "BrainBrowserWindowToolBarChartAxes.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_AXES_DECLARE__

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>

#include "BrowserTabContent.h"
#include "BrainBrowserWindowToolBar.h"
#include "CaretAssert.h"
#include "ChartAxis.h"
#include "ChartModelDataSeries.h"
#include "ModelChart.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarChartAxes 
 * \brief Component of toolbar for chart axes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 */
BrainBrowserWindowToolBarChartAxes::BrainBrowserWindowToolBarChartAxes(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 0, 0);
    gridLayout->addWidget(new QLabel("Axis"),
                          0, 0,
                          Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Auto"),
                          0, 1,
                          Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Min"),
                          0, 2,
                          Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel("Max"),
                          0, 3,
                          Qt::AlignHCenter);
    
    createAxisWidgets(gridLayout,
                      m_bottomAxisLabel,
                      m_bottomAxisAutoRangeScaleCheckBox,
                      m_bottomAxisMinimumValueSpinBox,
                      m_bottomAxisMaximumValueSpinBox,
                      m_bottomAxisWidgetGroup);
    
    QObject::connect(m_bottomAxisAutoRangeScaleCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(bottomAxisAutoRangeScaleCheckBoxClicked(bool)));
    QObject::connect(m_bottomAxisMinimumValueSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(bottomAxisValueChanged(double)));
    QObject::connect(m_bottomAxisMaximumValueSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(bottomAxisValueChanged(double)));
    
    createAxisWidgets(gridLayout,
                      m_leftAxisLabel,
                      m_leftAxisAutoRangeScaleCheckBox,
                      m_leftAxisMinimumValueSpinBox,
                      m_leftAxisMaximumValueSpinBox,
                      m_leftAxisWidgetGroup);
    
    QObject::connect(m_leftAxisAutoRangeScaleCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(leftAxisAutoRangeScaleCheckBoxClicked(bool)));
    QObject::connect(m_leftAxisMinimumValueSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(leftAxisValueChanged(double)));
    QObject::connect(m_leftAxisMaximumValueSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(leftAxisValueChanged(double)));
    
//    createAxisWidgets(gridLayout,
//                      m_topAxisLabel,
//                      m_topAxisAutoRangeScaleCheckBox,
//                      m_topAxisMinimumValueSpinBox,
//                      m_topAxisMaximumValueSpinBox,
//                      m_topAxisWidgetGroup);
//    
//    createAxisWidgets(gridLayout,
//                      m_rightAxisLabel,
//                      m_rightAxisAutoRangeScaleCheckBox,
//                      m_rightAxisMinimumValueSpinBox,
//                      m_rightAxisMaximumValueSpinBox,
//                      m_rightAxisWidgetGroup);
    
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarChartAxes::~BrainBrowserWindowToolBarChartAxes()
{
}

/**
 * Create the widgets for an axis.
 *
 * @param gridLayout
 *    Layout in which axis widgets are displayed.
 * @param nameLabel
 *    Label that contains name of axis.
 * @param autoRangeScaleCheckBox
 *    Checkbox for enabling auto range scale.
 * @param minimumValueSpinBox
 *    Spin box for minimum value.
 * @param maximumValueSpinBox
 *    Spin box for maximum value.
 * @param widgetGroup
 *    Widget group for the axis' widgets.
 */
void
BrainBrowserWindowToolBarChartAxes::createAxisWidgets(QGridLayout* gridLayout,
                                                      QLabel*& nameLabel,
                                                      QCheckBox*& autoRangeScaleCheckBox,
                                                      QDoubleSpinBox*& minimumValueSpinBox,
                                                      QDoubleSpinBox*& maximumValueSpinBox,
                                                      WuQWidgetObjectGroup*& widgetGroup)
{
    const int32_t spinBoxWidth = 100;
    
    nameLabel = new QLabel();
    
    autoRangeScaleCheckBox = new QCheckBox(" ");
    WuQtUtilities::setWordWrappedToolTip(autoRangeScaleCheckBox,
                                         "Axis Auto Range\n\n"
                                         "When checked, the minimum and maximum values are "
                                         "automatically adjusted to show the full range "
                                         "of values for the axis.\n\n"
                                         "When not checked, the user may adjust the minimum "
                                         "and maximum values for the axis to show a "
                                         "desired range of data for the axis.\n\n"
                                         "Changing the status from not checked to checked "
                                         "will reset the minimum and maximum values to show "
                                         "the full range of data.");
    
    minimumValueSpinBox = new QDoubleSpinBox();
    minimumValueSpinBox->setFixedWidth(spinBoxWidth);
    minimumValueSpinBox->setMinimum(-std::numeric_limits<float>::max());
    minimumValueSpinBox->setMaximum(std::numeric_limits<float>::max());
    minimumValueSpinBox->setSingleStep(1.0);
    minimumValueSpinBox->setDecimals(2);
    WuQtUtilities::setWordWrappedToolTip(minimumValueSpinBox,
                                         "Minimum value displayed for the axis");
    
    maximumValueSpinBox = new QDoubleSpinBox();
    maximumValueSpinBox->setFixedWidth(spinBoxWidth);
    maximumValueSpinBox->setMinimum(-std::numeric_limits<float>::max());
    maximumValueSpinBox->setMaximum(std::numeric_limits<float>::max());
    maximumValueSpinBox->setSingleStep(1.0);
    maximumValueSpinBox->setDecimals(2);
    WuQtUtilities::setWordWrappedToolTip(maximumValueSpinBox,
                                         "Maximum value displayed for the axis");
    
    widgetGroup = new WuQWidgetObjectGroup(this);
    widgetGroup->add(nameLabel);
    widgetGroup->add(autoRangeScaleCheckBox);
    widgetGroup->add(minimumValueSpinBox);
    widgetGroup->add(maximumValueSpinBox);
    
    const int rowIndex = gridLayout->rowCount();
    gridLayout->addWidget(nameLabel,
                          rowIndex, 0,
                          Qt::AlignLeft);
    gridLayout->addWidget(autoRangeScaleCheckBox,
                          rowIndex, 1,
                          Qt::AlignHCenter);
    gridLayout->addWidget(minimumValueSpinBox,
                          rowIndex, 2,
                          Qt::AlignHCenter);
    gridLayout->addWidget(maximumValueSpinBox,
                          rowIndex, 3,
                          Qt::AlignHCenter);
}

///**
// * Called when a radio button is clicked.
// */
//void
//BrainBrowserWindowToolBarChartType::chartTypeRadioButtonClicked(int)
//{
//    ChartDataTypeEnum::Enum chartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_INVALID;
//    
//    if (m_chartDataSeriesTypeRadioButton->isChecked()) {
//        chartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES;
//    }
//    else if (m_chartMatrixTypeRadioButton->isChecked()) {
//        chartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_ADJACENCY_MATRIX;
//    }
//    else if (m_chartTimeSeriesTypeRadioButton->isChecked()) {
//        chartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES;
//    }
//    else {
//        CaretAssertMessage(0, "Has a new chart radio button been added?");
//    }
//    
//    m_parentToolBar->getTabContentFromSelectedTab();
//    BrowserTabContent* btc = m_parentToolBar->getTabContentFromSelectedTab();
//    ModelChart* chartModel = btc->getDisplayedChartModel();
//    const int32_t tabIndex = btc->getTabNumber();
//    chartModel->setSelectedChartDataType(tabIndex,
//                                         chartDataType);
//    
//    //updateContent(btc);
//    
//    invalidateColoringAndUpdateGraphicsWindow();
//    m_parentToolBar->updateUserInterface();
//}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarChartAxes::updateContent(BrowserTabContent* browserTabContent)
{
    m_bottomAxisWidgetGroup->blockAllSignals(true);
    m_leftAxisWidgetGroup->blockAllSignals(true);
//    m_topAxisWidgetGroup->blockAllSignals(true);
//    m_rightAxisWidgetGroup->blockAllSignals(true);
    
    ModelChart* modelChart = browserTabContent->getDisplayedChartModel();
    if (modelChart != NULL) {
        const int32_t tabIndex = browserTabContent->getTabNumber();
        ChartModel* chart = modelChart->getSelectedChartModel(tabIndex);
        if (chart != NULL) {
            const ChartDataTypeEnum::Enum chartType = chart->getChartDataType();
            
            ChartModelDataSeries* lineSeriesChart = NULL;
            
            switch (chartType) {
                case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_ADJACENCY_MATRIX:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                    lineSeriesChart = dynamic_cast<ChartModelDataSeries*>(chart);
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                    lineSeriesChart = dynamic_cast<ChartModelDataSeries*>(chart);
                    break;
            }
            
            if (lineSeriesChart != NULL) {
                updateAxisWidgets(lineSeriesChart,
                                  lineSeriesChart->getLeftAxis(),
                                  m_leftAxisLabel,
                                  m_leftAxisAutoRangeScaleCheckBox,
                                  m_leftAxisMinimumValueSpinBox,
                                  m_leftAxisMaximumValueSpinBox,
                                  m_leftAxisWidgetGroup);
                
                updateAxisWidgets(lineSeriesChart,
                                  lineSeriesChart->getBottomAxis(),
                                  m_bottomAxisLabel,
                                  m_bottomAxisAutoRangeScaleCheckBox,
                                  m_bottomAxisMinimumValueSpinBox,
                                  m_bottomAxisMaximumValueSpinBox,
                                  m_bottomAxisWidgetGroup);
                
//                updateAxisWidgets(lineSeriesChart,
//                                  lineSeriesChart->getRightAxis(),
//                                  m_rightAxisLabel,
//                                  m_rightAxisAutoRangeScaleCheckBox,
//                                  m_rightAxisMinimumValueSpinBox,
//                                  m_rightAxisMaximumValueSpinBox,
//                                  m_rightAxisWidgetGroup);
//                
//                updateAxisWidgets(lineSeriesChart,
//                                  lineSeriesChart->getTopAxis(),
//                                  m_topAxisLabel,
//                                  m_topAxisAutoRangeScaleCheckBox,
//                                  m_topAxisMinimumValueSpinBox,
//                                  m_topAxisMaximumValueSpinBox,
//                                  m_topAxisWidgetGroup);
            }
        }
    }
    
    m_bottomAxisWidgetGroup->blockAllSignals(false);
    m_leftAxisWidgetGroup->blockAllSignals(false);
//    m_topAxisWidgetGroup->blockAllSignals(false);
//    m_rightAxisWidgetGroup->blockAllSignals(false);
    
    
//    m_chartTypeButtonGroup->blockSignals(true);
//
//    const ModelChart* chartModel = browserTabContent->getDisplayedChartModel();
//    if (chartModel != NULL) {
//        const int32_t tabIndex = browserTabContent->getTabNumber();
//        const ChartDataTypeEnum::Enum chartType = chartModel->getSelectedChartDataType(tabIndex);
//        
//        switch (chartType) {
//            case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
//                break;
//            case ChartDataTypeEnum::CHART_DATA_TYPE_ADJACENCY_MATRIX:
//                m_chartMatrixTypeRadioButton->setChecked(true);
//                break;
//            case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
//                m_chartDataSeriesTypeRadioButton->setChecked(true);
//                break;
//            case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
//                m_chartTimeSeriesTypeRadioButton->setChecked(true);
//                break;
//        }
//    }
//    
//    m_chartTypeButtonGroup->blockSignals(false);
}

/**
 * @return Cartesian chart in this widget.  Returned value will
 * be NULL if the chart (or no chart) is not a Cartesian Chart.
 */
ChartModelCartesian*
BrainBrowserWindowToolBarChartAxes::getCartesianChart()
{
    ChartModelCartesian* cartesianChart = NULL;
    
    BrowserTabContent* btc = getTabContentFromSelectedTab();
    if (btc != NULL) {
        ModelChart* modelChart = btc->getDisplayedChartModel();
        
        if (modelChart != NULL) {
            const int32_t tabIndex = btc->getTabNumber();
            ChartModel* chart = modelChart->getSelectedChartModel(tabIndex);
            if (chart != NULL) {
                cartesianChart = dynamic_cast<ChartModelCartesian*>(chart);
            }
        }
    }
    
    return cartesianChart;
}


/**
 * Update the widgets for an axis.
 *
 * @param chart
 *    Chart whose axes are updated.
 * @param chartAxis
 *    Source axis containing data.
 * @param nameLabel
 *    Label that contains name of axis.
 * @param autoRangeScaleCheckBox
 *    Checkbox for enabling auto range scale.
 * @param minimumValueSpinBox
 *    Spin box for minimum value.
 * @param maximumValueSpinBox
 *    Spin box for maximum value.
 * @param widgetGroup
 *    Widget group for the axis' widgets.
 */
void
BrainBrowserWindowToolBarChartAxes::updateAxisWidgets(const ChartModelCartesian* chart,
                                                      const ChartAxis* chartAxis,
                                                      QLabel* nameLabel,
                                                      QCheckBox* autoRangeScaleCheckBox,
                                                      QDoubleSpinBox* minimumValueSpinBox,
                                                      QDoubleSpinBox* maximumValueSpinBox,
                                                      WuQWidgetObjectGroup* widgetGroup)
{
    const bool visible = chartAxis->isVisible();
    
    if (visible) {
        nameLabel->setText(chartAxis->getText());
        autoRangeScaleCheckBox->setChecked(chartAxis->isAutoRangeScale());
        const float minValue = chartAxis->getMinimumValue();
        const float maxValue = chartAxis->getMaximumValue();
        minimumValueSpinBox->setValue(minValue);
        maximumValueSpinBox->setValue(maxValue);
    }
    
    widgetGroup->setVisible(visible);
}

/**
 * Called when left axis auto range scale checkbox is clicked.
 *
 * @param checked
 *    New check status.
 */
void
BrainBrowserWindowToolBarChartAxes::leftAxisAutoRangeScaleCheckBoxClicked(bool checked)
{
    ChartModelCartesian* cartesianChart = getCartesianChart();
    if (cartesianChart != NULL) {
        cartesianChart->getLeftAxis()->setAutoRangeScale(checked);
        
        if (checked) {
            cartesianChart->resetAxesToDefaultRange();
            updateContent(getTabContentFromSelectedTab());
            invalidateColoringAndUpdateGraphicsWindow();
        }
    }
}

/**
 * Called when left axis minimum or maximum value is changed.
 *
 * @param value
 *    New value.
 */
void
BrainBrowserWindowToolBarChartAxes::leftAxisValueChanged(double /*value*/)
{
    ChartModelCartesian* cartesianChart = getCartesianChart();
    if (cartesianChart != NULL) {
        m_leftAxisAutoRangeScaleCheckBox->setChecked(false);
        ChartAxis* axis = cartesianChart->getLeftAxis();
        axis->setAutoRangeScale(false);
        axis->setMinimumValue(m_leftAxisMinimumValueSpinBox->value());
        axis->setMaximumValue(m_leftAxisMaximumValueSpinBox->value());
        invalidateColoringAndUpdateGraphicsWindow();
    }
}

/**
 * Called when bottom axis auto range scale checkbox is clicked.
 *
 * @param checked
 *    New check status.
 */
void
BrainBrowserWindowToolBarChartAxes::bottomAxisAutoRangeScaleCheckBoxClicked(bool checked)
{
    ChartModelCartesian* cartesianChart = getCartesianChart();
    if (cartesianChart != NULL) {
        cartesianChart->getBottomAxis()->setAutoRangeScale(checked);

        if (checked) {
            cartesianChart->resetAxesToDefaultRange();
            updateContent(getTabContentFromSelectedTab());
            invalidateColoringAndUpdateGraphicsWindow();
        }
    }
}

/**
 * Called when bottom axis minimum or maximum value is changed.
 *
 * @param value
 *    New value.
 */
void
BrainBrowserWindowToolBarChartAxes::bottomAxisValueChanged(double /*value*/)
{
    ChartModelCartesian* cartesianChart = getCartesianChart();
    if (cartesianChart != NULL) {
        m_bottomAxisAutoRangeScaleCheckBox->setChecked(false);
        ChartAxis* axis = cartesianChart->getBottomAxis();
        axis->setAutoRangeScale(false);
        axis->setMinimumValue(m_leftAxisMinimumValueSpinBox->value());
        axis->setMaximumValue(m_leftAxisMaximumValueSpinBox->value());
        invalidateColoringAndUpdateGraphicsWindow();
    }
}

