
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ATTRIBUTES_DECLARE__
#include "BrainBrowserWindowToolBarChartAttributes.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ATTRIBUTES_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ChartModelDataSeries.h"
#include "ChartModelTimeSeries.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "ModelChart.h"
#include "WuQFactory.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarChartAttributes 
 * \brief Controls for chart attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
BrainBrowserWindowToolBarChartAttributes::BrainBrowserWindowToolBarChartAttributes(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    m_cartesianChartAttributesWidget = createCartesianChartAttributesWidget();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(m_cartesianChartAttributesWidget);
    layout->addStretch();
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarChartAttributes::~BrainBrowserWindowToolBarChartAttributes()
{
}

/**
 * @return Create the cartesian chart attributes widget.
 */
QWidget*
BrainBrowserWindowToolBarChartAttributes::createCartesianChartAttributesWidget()
{
    QLabel* cartesianLineWidthLabel = new QLabel("Line width ");
    m_cartesianLineWidthDoubleSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                                                       10000.0,
                                                                                                       0.1,
                                                                                                       1,
                                                                                                       this,
                                                                                                       SLOT(cartesianLineWidthValueChanged(double)));
    m_cartesianLineWidthDoubleSpinBox->setFixedWidth(65);
    
    QWidget* w = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(w);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 0, 0);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(0, 100);
    gridLayout->addWidget(cartesianLineWidthLabel, 0, 0);
    gridLayout->addWidget(m_cartesianLineWidthDoubleSpinBox, 0, 1);
    return w;
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarChartAttributes::updateContent(BrowserTabContent* /*browserTabContent*/)
{
    ChartModelCartesian* chart = getCartesianChart();
    if (chart != NULL) {
        m_cartesianLineWidthDoubleSpinBox->blockSignals(true);
        m_cartesianLineWidthDoubleSpinBox->setValue(chart->getLineWidth());
        m_cartesianLineWidthDoubleSpinBox->blockSignals(false);
    }
}


/**
 * Called when the cartesian line width is changed.
 *
 * @param value
 *    New value for line width.
 */
void
BrainBrowserWindowToolBarChartAttributes::cartesianLineWidthValueChanged(double value)
{
    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        ChartModelCartesian* chart = getCartesianChart();
        if (chart != NULL) {
            chart->setLineWidth(value);
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        }
    }
}

/**
 * @return Cartesian chart in this widget.  Returned value will
 * be NULL if the chart (or no chart) is not a Cartesian Chart.
 */
ChartModelCartesian*
BrainBrowserWindowToolBarChartAttributes::getCartesianChart()
{
    ChartModelCartesian* cartesianChart = NULL;
    
    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        ModelChart* modelChart = browserTabContent->getDisplayedChartModel();
        
        if (modelChart != NULL) {
            const int32_t tabIndex = browserTabContent->getTabNumber();
            const ChartDataTypeEnum::Enum chartType = modelChart->getSelectedChartDataType(tabIndex);
            
            switch (chartType) {
                case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                    cartesianChart = modelChart->getSelectedDataSeriesChartModel(tabIndex);  //dynamic_cast<ChartModelDataSeries*>(chart);
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                    cartesianChart = modelChart->getSelectedTimeSeriesChartModel(tabIndex);  //dynamic_cast<ChartModelDataSeries*>(chart);
                    break;
            }
        }
    }
    
    return cartesianChart;
}

