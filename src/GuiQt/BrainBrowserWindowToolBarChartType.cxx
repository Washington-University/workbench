
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
    m_chartTypeButtonGroup = new QButtonGroup(this);
    
    QVBoxLayout* radioButtonLayout = new QVBoxLayout(this);
    
    std::vector<ChartVersionOneDataTypeEnum::Enum> allChartTypes;
    ChartVersionOneDataTypeEnum::getAllEnums(allChartTypes);
    
    for (std::vector<ChartVersionOneDataTypeEnum::Enum>::iterator chartIter = allChartTypes.begin();
         chartIter != allChartTypes.end();
         chartIter++) {
        const ChartVersionOneDataTypeEnum::Enum ct = *chartIter;
        if (ct == ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_INVALID) {
            continue;
        }
        
        QRadioButton* rb = new QRadioButton(ChartVersionOneDataTypeEnum::toGuiName(ct));
        m_chartTypeButtonGroup->addButton(rb,
                                          m_chartTypeRadioButtons.size());
        
        radioButtonLayout->addWidget(rb);
        
        m_chartTypeRadioButtons.push_back(std::make_pair(ct, rb));
    }

    WuQtUtilities::setLayoutSpacingAndMargins(radioButtonLayout, 4, 5);
    radioButtonLayout->addStretch();

//    m_chartMatrixLayerTypeRadioButton = new QRadioButton(ChartVersionOneDataTypeEnum::toGuiName(ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER));
//    
//    m_chartMatrixSeriesTypeRadioButton = new QRadioButton(ChartVersionOneDataTypeEnum::toGuiName(ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES));
//    
//    m_chartDataSeriesTypeRadioButton = new QRadioButton(ChartVersionOneDataTypeEnum::toGuiName(ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES));
//
//    m_chartTimeSeriesTypeRadioButton = new QRadioButton(ChartVersionOneDataTypeEnum::toGuiName(ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES));


    QObject::connect(m_chartTypeButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(chartTypeRadioButtonClicked(int)));
//    m_chartTypeButtonGroup->addButton(m_chartMatrixLayerTypeRadioButton);
//    m_chartTypeButtonGroup->addButton(m_chartMatrixSeriesTypeRadioButton);
//    m_chartTypeButtonGroup->addButton(m_chartDataSeriesTypeRadioButton);
//    m_chartTypeButtonGroup->addButton(m_chartTimeSeriesTypeRadioButton);
    
//    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
//    layout->addWidget(m_chartDataSeriesTypeRadioButton);
//    layout->addWidget(m_chartMatrixLayerTypeRadioButton);
//    layout->addWidget(m_chartMatrixSeriesTypeRadioButton);
//    layout->addWidget(m_chartTimeSeriesTypeRadioButton);
//    layout->addStretch();
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarChartType::~BrainBrowserWindowToolBarChartType()
{
}

/**
 * Called when a radio button is clicked.
 *
 * @param buttonIndex
 *    Index of button that is clicked.
 */
void
BrainBrowserWindowToolBarChartType::chartTypeRadioButtonClicked(int buttonIndex)
{
    CaretAssertVectorIndex(m_chartTypeRadioButtons, buttonIndex);
    
    ChartVersionOneDataTypeEnum::Enum chartDataType = m_chartTypeRadioButtons[buttonIndex].first;
    
//    ChartVersionOneDataTypeEnum::Enum chartDataType = ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_INVALID;
//    
//    if (m_chartDataSeriesTypeRadioButton->isChecked()) {
//        chartDataType = ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES;
//    }
//    else if (m_chartMatrixLayerTypeRadioButton->isChecked()) {
//        chartDataType = ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER;
//    }
//    else if (m_chartTimeSeriesTypeRadioButton->isChecked()) {
//        chartDataType = ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES;
//    }
//    else if (m_chartMatrixSeriesTypeRadioButton->isChecked()) {
//        chartDataType = ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES;
//    }
//    else {
//        CaretAssertMessage(0, "Has a new chart radio button been added?");
//    }
    
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
        const ChartVersionOneDataTypeEnum::Enum selectedChartType = chartModel->getSelectedChartDataType(tabIndex);

        std::vector<ChartVersionOneDataTypeEnum::Enum> validChartDataTypes;
        chartModel->getValidChartDataTypes(validChartDataTypes);

        for (std::vector<std::pair<ChartVersionOneDataTypeEnum::Enum, QRadioButton*> >::iterator buttIter = m_chartTypeRadioButtons.begin();
             buttIter != m_chartTypeRadioButtons.end();
             buttIter++) {
            ChartVersionOneDataTypeEnum::Enum chartType = buttIter->first;
            QRadioButton* radioButton         = buttIter->second;
            
            const bool validTypeFlag =  (std::find(validChartDataTypes.begin(),
                                                   validChartDataTypes.end(),
                                                   chartType) != validChartDataTypes.end());
            radioButton->setEnabled(validTypeFlag);
            
            if (chartType == selectedChartType) {
                radioButton->setChecked(true);
            }
        }
        
//        bool dataSeriesValidFlag   = false;
//        bool matrixLayerValidFlag  = false;
//        bool matrixSeriesValidFlag = false;
//        bool timeSeriesValidFlag   = false;
//        for (std::vector<ChartVersionOneDataTypeEnum::Enum>::iterator iter = validChartDataTypes.begin();
//             iter != validChartDataTypes.end();
//             iter++) {
//            switch (*iter) {
//                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
//                    break;
//                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
//                    matrixLayerValidFlag = true;
//                    break;
//                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
//                    matrixSeriesValidFlag = true;
//                    break;
//                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
//                    dataSeriesValidFlag = true;
//                    break;
//                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
//                    CaretAssertToDoFatal();
//                    break;
//                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
//                    timeSeriesValidFlag = true;
//                    break;
//            }
//        }
//        
//        m_chartDataSeriesTypeRadioButton->setEnabled(dataSeriesValidFlag);
//        m_chartMatrixLayerTypeRadioButton->setEnabled(matrixLayerValidFlag);
//        m_chartTimeSeriesTypeRadioButton->setEnabled(timeSeriesValidFlag);
//        
//        switch (chartType) {
//            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
//                break;
//            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
//                m_chartMatrixLayerTypeRadioButton->setChecked(true);
//                break;
//            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
//                m_chartMatrixSeriesTypeRadioButton->setChecked(true);
//                break;
//            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
//                m_chartDataSeriesTypeRadioButton->setChecked(true);
//                break;
//            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
//                CaretAssertToDoFatal();
//                break;
//            case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
//                m_chartTimeSeriesTypeRadioButton->setChecked(true);
//                break;
//        }
    }
    
    m_chartTypeButtonGroup->blockSignals(false);
}

