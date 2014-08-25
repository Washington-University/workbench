
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_DECLARE__
#include "BrainBrowserWindowToolBarChartType.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_DECLARE__

#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>

#include "BrowserTabContent.h"
#include "BrainBrowserWindowToolBar.h"
#include "CaretAssert.h"
#include "ChartModel.h"
#include "EnumComboBoxTemplate.h"
#include "ModelChart.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarChartType
 * \brief Chart Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 */
BrainBrowserWindowToolBarChartType::BrainBrowserWindowToolBarChartType(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    m_chartMatrixTypeRadioButton = new QRadioButton(ChartDataTypeEnum::toGuiName(ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX));
    
    m_chartDataSeriesTypeRadioButton = new QRadioButton(ChartDataTypeEnum::toGuiName(ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES));

    m_chartTimeSeriesTypeRadioButton = new QRadioButton(ChartDataTypeEnum::toGuiName(ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES));


    m_chartTypeButtonGroup = new QButtonGroup(this);
    QObject::connect(m_chartTypeButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(chartTypeRadioButtonClicked(int)));
    m_chartTypeButtonGroup->addButton(m_chartMatrixTypeRadioButton);
    m_chartTypeButtonGroup->addButton(m_chartDataSeriesTypeRadioButton);
    m_chartTypeButtonGroup->addButton(m_chartTimeSeriesTypeRadioButton);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addWidget(m_chartDataSeriesTypeRadioButton);
    layout->addWidget(m_chartMatrixTypeRadioButton);
    layout->addWidget(m_chartTimeSeriesTypeRadioButton);
    layout->addStretch();
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarChartType::~BrainBrowserWindowToolBarChartType()
{
}

/**
 * Called when a radio button is clicked.
 */
void
BrainBrowserWindowToolBarChartType::chartTypeRadioButtonClicked(int)
{
    ChartDataTypeEnum::Enum chartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_INVALID;
    
    if (m_chartDataSeriesTypeRadioButton->isChecked()) {
        chartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES;
    }
    else if (m_chartMatrixTypeRadioButton->isChecked()) {
        chartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX;
    }
    else if (m_chartTimeSeriesTypeRadioButton->isChecked()) {
        chartDataType = ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES;
    }
    else {
        CaretAssertMessage(0, "Has a new chart radio button been added?");
    }
    
    m_parentToolBar->getTabContentFromSelectedTab();
    BrowserTabContent* btc = m_parentToolBar->getTabContentFromSelectedTab();
    ModelChart* chartModel = btc->getDisplayedChartModel();
    const int32_t tabIndex = btc->getTabNumber();
    chartModel->setSelectedChartDataType(tabIndex,
                                         chartDataType);
    
    //updateContent(btc);
    
    invalidateColoringAndUpdateGraphicsWindow();
    m_parentToolBar->updateUserInterface();
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarChartType::updateContent(BrowserTabContent* browserTabContent)
{
    m_chartTypeButtonGroup->blockSignals(true);
    
    
    const ModelChart* chartModel = browserTabContent->getDisplayedChartModel();
    if (chartModel != NULL) {
        const int32_t tabIndex = browserTabContent->getTabNumber();
        const ChartDataTypeEnum::Enum chartType = chartModel->getSelectedChartDataType(tabIndex);
        
        bool dataSeriesValidFlag = false;
        bool matrixValidFlag     = false;
        bool timeSeriesValidFlag = false;
        std::vector<ChartDataTypeEnum::Enum> validChartDataTypes;
        chartModel->getValidChartDataTypes(validChartDataTypes);
        for (std::vector<ChartDataTypeEnum::Enum>::iterator iter = validChartDataTypes.begin();
             iter != validChartDataTypes.end();
             iter++) {
            switch (*iter) {
                case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                    dataSeriesValidFlag = true;
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                    matrixValidFlag = true;
                    break;
                case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                    timeSeriesValidFlag = true;
                    break;
            }
        }
        
        m_chartDataSeriesTypeRadioButton->setEnabled(dataSeriesValidFlag);
        m_chartMatrixTypeRadioButton->setEnabled(matrixValidFlag);
        m_chartTimeSeriesTypeRadioButton->setEnabled(timeSeriesValidFlag);
        
        switch (chartType) {
            case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                m_chartDataSeriesTypeRadioButton->setChecked(true);
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                m_chartMatrixTypeRadioButton->setChecked(true);
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                m_chartTimeSeriesTypeRadioButton->setChecked(true);
                break;
        }
    }
    
    m_chartTypeButtonGroup->blockSignals(false);
}

