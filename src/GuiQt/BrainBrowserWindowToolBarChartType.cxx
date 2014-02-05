
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
        
        switch (chartType) {
            case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                m_chartMatrixTypeRadioButton->setChecked(true);
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
                m_chartDataSeriesTypeRadioButton->setChecked(true);
                break;
            case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
                m_chartTimeSeriesTypeRadioButton->setChecked(true);
                break;
        }
    }
    
    m_chartTypeButtonGroup->blockSignals(false);
}

