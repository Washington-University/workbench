
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
#include "WuQMacroManager.h"
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
BrainBrowserWindowToolBarChartType::BrainBrowserWindowToolBarChartType(BrainBrowserWindowToolBar* parentToolBar,
                                                                       const QString& parentObjectName)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    
    m_chartTypeButtonGroup = new QButtonGroup(this);
    
    QVBoxLayout* radioButtonLayout = new QVBoxLayout(this);
    
    std::vector<ChartOneDataTypeEnum::Enum> allChartTypes;
    ChartOneDataTypeEnum::getAllEnums(allChartTypes);
    
    for (std::vector<ChartOneDataTypeEnum::Enum>::iterator chartIter = allChartTypes.begin();
         chartIter != allChartTypes.end();
         chartIter++) {
        const ChartOneDataTypeEnum::Enum ct = *chartIter;
        if (ct == ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID) {
            continue;
        }
        
        QRadioButton* rb = new QRadioButton(ChartOneDataTypeEnum::toGuiName(ct));
        m_chartTypeButtonGroup->addButton(rb,
                                          m_chartTypeRadioButtons.size());
        
        rb->setToolTip("Set chart to "
                       + rb->text());
        
        QString chartTypeName = rb->text();
        chartTypeName = chartTypeName.replace(" ", "");
        rb->setObjectName(parentObjectName
                          + ":ChartOneType:"
                          + chartTypeName);
        WuQMacroManager::instance()->addMacroSupportToObject(rb,
                                                             "Select " + chartTypeName + " chart");
        
        radioButtonLayout->addWidget(rb);
        
        m_chartTypeRadioButtons.push_back(std::make_pair(ct, rb));
    }

    WuQtUtilities::setLayoutSpacingAndMargins(radioButtonLayout, 4, 5);
    radioButtonLayout->addStretch();

    QObject::connect(m_chartTypeButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(chartTypeRadioButtonClicked(int)));
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
    
    ChartOneDataTypeEnum::Enum chartDataType = m_chartTypeRadioButtons[buttonIndex].first;
    
    m_parentToolBar->getTabContentFromSelectedTab();
    BrowserTabContent* btc = m_parentToolBar->getTabContentFromSelectedTab();
    ModelChart* chartModel = btc->getDisplayedChartOneModel();
    const int32_t tabIndex = btc->getTabNumber();
    chartModel->setSelectedChartOneDataType(tabIndex,
                                         chartDataType);
    
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
    
    
    const ModelChart* chartModel = browserTabContent->getDisplayedChartOneModel();
    if (chartModel != NULL) {
        const int32_t tabIndex = browserTabContent->getTabNumber();
        const ChartOneDataTypeEnum::Enum selectedChartType = chartModel->getSelectedChartOneDataType(tabIndex);

        std::vector<ChartOneDataTypeEnum::Enum> validChartDataTypes;
        chartModel->getValidChartOneDataTypes(validChartDataTypes);

        for (std::vector<std::pair<ChartOneDataTypeEnum::Enum, QRadioButton*> >::iterator buttIter = m_chartTypeRadioButtons.begin();
             buttIter != m_chartTypeRadioButtons.end();
             buttIter++) {
            ChartOneDataTypeEnum::Enum chartType = buttIter->first;
            QRadioButton* radioButton         = buttIter->second;
            
            const bool validTypeFlag =  (std::find(validChartDataTypes.begin(),
                                                   validChartDataTypes.end(),
                                                   chartType) != validChartDataTypes.end());
            radioButton->setEnabled(validTypeFlag);
            
            if (chartType == selectedChartType) {
                radioButton->setChecked(true);
            }
        }
    }
    
    m_chartTypeButtonGroup->blockSignals(false);
}

