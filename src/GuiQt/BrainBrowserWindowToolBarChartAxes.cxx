
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
#include "ChartAxisCartesian.h"
#include "ChartModelDataSeries.h"
#include "ChartModelFrequencySeries.h"
#include "ChartModelTimeSeries.h"
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

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarChartAxes::updateContent(BrowserTabContent* /*browserTabContent*/)
{
    m_bottomAxisWidgetGroup->blockAllSignals(true);
    m_leftAxisWidgetGroup->blockAllSignals(true);
//    m_topAxisWidgetGroup->blockAllSignals(true);
//    m_rightAxisWidgetGroup->blockAllSignals(true);
    
    ChartModelCartesian* cartesianChart = getCartesianChart();
    if (cartesianChart != NULL) {
        if (cartesianChart != NULL) {
            updateAxisWidgets(cartesianChart->getLeftAxis(),
                              m_leftAxisLabel,
                              m_leftAxisAutoRangeScaleCheckBox,
                              m_leftAxisMinimumValueSpinBox,
                              m_leftAxisMaximumValueSpinBox,
                              m_leftAxisWidgetGroup);
            
            updateAxisWidgets(cartesianChart->getBottomAxis(),
                              m_bottomAxisLabel,
                              m_bottomAxisAutoRangeScaleCheckBox,
                              m_bottomAxisMinimumValueSpinBox,
                              m_bottomAxisMaximumValueSpinBox,
                              m_bottomAxisWidgetGroup);
            
            //                updateAxisWidgets(lineSeriesChart->getRightAxis(),
            //                                  m_rightAxisLabel,
            //                                  m_rightAxisAutoRangeScaleCheckBox,
            //                                  m_rightAxisMinimumValueSpinBox,
            //                                  m_rightAxisMaximumValueSpinBox,
            //                                  m_rightAxisWidgetGroup);
            //
            //                updateAxisWidgets(lineSeriesChart->getTopAxis(),
            //                                  m_topAxisLabel,
            //                                  m_topAxisAutoRangeScaleCheckBox,
            //                                  m_topAxisMinimumValueSpinBox,
            //                                  m_topAxisMaximumValueSpinBox,
            //                                  m_topAxisWidgetGroup);
        }
    }
    
    m_bottomAxisWidgetGroup->blockAllSignals(false);
    m_leftAxisWidgetGroup->blockAllSignals(false);
//    m_topAxisWidgetGroup->blockAllSignals(false);
//    m_rightAxisWidgetGroup->blockAllSignals(false);
    
}

/**
 * @return Cartesian chart in this widget.  Returned value will
 * be NULL if the chart (or no chart) is not a Cartesian Chart.
 */
ChartModelCartesian*
BrainBrowserWindowToolBarChartAxes::getCartesianChart()
{
    ChartModelCartesian* cartesianChart = NULL;
    
    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        ModelChart* modelChart = browserTabContent->getDisplayedChartModel();
        
        if (modelChart != NULL) {
            const int32_t tabIndex = browserTabContent->getTabNumber();
            const ChartVersionOneDataTypeEnum::Enum chartType = modelChart->getSelectedChartOneDataType(tabIndex);
            
            switch (chartType) {
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                    cartesianChart = modelChart->getSelectedDataSeriesChartModel(tabIndex);  //dynamic_cast<ChartModelDataSeries*>(chart);
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                    cartesianChart = modelChart->getSelectedFrequencySeriesChartModel(tabIndex);
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                    cartesianChart = modelChart->getSelectedTimeSeriesChartModel(tabIndex);  //dynamic_cast<ChartModelDataSeries*>(chart);
                    break;
            }
        }
    }
    
    return cartesianChart;
}


/**
 * Update the widgets for an axis.
 *
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
BrainBrowserWindowToolBarChartAxes::updateAxisWidgets(const ChartAxis* chartAxis,
                                                      QLabel* nameLabel,
                                                      QCheckBox* autoRangeScaleCheckBox,
                                                      QDoubleSpinBox* minimumValueSpinBox,
                                                      QDoubleSpinBox* maximumValueSpinBox,
                                                      WuQWidgetObjectGroup* widgetGroup)
{
    if (chartAxis == NULL) {
        return;
    }
    const ChartAxisCartesian* chartAxisCartesian = dynamic_cast<const ChartAxisCartesian*>(chartAxis);
    CaretAssert(chartAxisCartesian);
    
    const bool visible = chartAxis->isVisible();
    
    if (visible) {
        nameLabel->setText(chartAxis->getText());
        autoRangeScaleCheckBox->setChecked(chartAxis->isAutoRangeScaleEnabled());
        const float minValue = chartAxisCartesian->getMinimumValue();
        const float maxValue = chartAxisCartesian->getMaximumValue();
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
        cartesianChart->getLeftAxis()->setAutoRangeScaleEnabled(checked);
        
        if (checked) {
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
        ChartAxisCartesian* axis = dynamic_cast<ChartAxisCartesian*>(cartesianChart->getLeftAxis());
        CaretAssert(axis);
        axis->setAutoRangeScaleEnabled(false);
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
        cartesianChart->getBottomAxis()->setAutoRangeScaleEnabled(checked);

        if (checked) {
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
        ChartAxisCartesian* axis = dynamic_cast<ChartAxisCartesian*>(cartesianChart->getBottomAxis());
        CaretAssert(axis);
        axis->setAutoRangeScaleEnabled(false);
        axis->setMinimumValue(m_bottomAxisMinimumValueSpinBox->value());
        axis->setMaximumValue(m_bottomAxisMaximumValueSpinBox->value());
        invalidateColoringAndUpdateGraphicsWindow();
    }
}

