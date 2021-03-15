
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_TYPE_DECLARE__
#include "BrainBrowserWindowToolBarChartTwoType.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_TYPE_DECLARE__

#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>

#include "BrowserTabContent.h"
#include "BrainBrowserWindowToolBar.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "ModelChartTwo.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarChartTwoType
 * \brief Chart Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 * @param parentObjectName
 *    Name of parent object for macros
 */
BrainBrowserWindowToolBarChartTwoType::BrainBrowserWindowToolBarChartTwoType(BrainBrowserWindowToolBar* parentToolBar,
                                                                             const QString& parentObjectName)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    m_chartTypeButtonGroup = new QButtonGroup(this);
    
    QVBoxLayout* radioButtonLayout = new QVBoxLayout(this);
    
    std::vector<ChartTwoDataTypeEnum::Enum> allChartTypes;
    ChartTwoDataTypeEnum::getAllEnums(allChartTypes);
    
    for (auto ct : allChartTypes) {
        if (ct == ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID) {
            continue;
        }
        
        QRadioButton* rb = new QRadioButton(ChartTwoDataTypeEnum::toGuiName(ct));
        m_chartTypeButtonGroup->addButton(rb,
                                          m_chartTypeRadioButtons.size());
        rb->setToolTip(ChartTwoDataTypeEnum::toToolTipText(ct));
        
        QString chartTypeName = rb->text();
        chartTypeName = chartTypeName.replace(" ", "");
        rb->setObjectName(parentObjectName
                          + ":ChartType:"
                          + chartTypeName);
        WuQMacroManager::instance()->addMacroSupportToObject(rb,
                                                             "Select " + chartTypeName + " chart");
        
        radioButtonLayout->addWidget(rb);
        
        m_chartTypeRadioButtons.push_back(std::make_pair(ct, rb));
    }

    WuQtUtilities::setLayoutSpacingAndMargins(radioButtonLayout, 4, 5);
    radioButtonLayout->addStretch();

#if QT_VERSION >= 0x060000
    QObject::connect(m_chartTypeButtonGroup, &QButtonGroup::idClicked,
                     this, &BrainBrowserWindowToolBarChartTwoType::chartTypeRadioButtonClicked);
#else
    QObject::connect(m_chartTypeButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(chartTypeRadioButtonClicked(int)));
#endif
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarChartTwoType::~BrainBrowserWindowToolBarChartTwoType()
{
}

/**
 * Called when a radio button is clicked.
 *
 * @param buttonIndex
 *    Index of button that is clicked.
 */
void
BrainBrowserWindowToolBarChartTwoType::chartTypeRadioButtonClicked(int buttonIndex)
{
    CaretAssertVectorIndex(m_chartTypeRadioButtons, buttonIndex);
    
    ChartTwoDataTypeEnum::Enum chartDataType = m_chartTypeRadioButtons[buttonIndex].first;
    
    m_parentToolBar->getTabContentFromSelectedTab();
    BrowserTabContent* btc = m_parentToolBar->getTabContentFromSelectedTab();
    ModelChartTwo* chartModelTwo = btc->getDisplayedChartTwoModel();
    const int32_t tabIndex = btc->getTabNumber();
    chartModelTwo->setSelectedChartTwoDataType(tabIndex,
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
BrainBrowserWindowToolBarChartTwoType::updateContent(BrowserTabContent* browserTabContent)
{
    m_chartTypeButtonGroup->blockSignals(true);
    
    
    const ModelChartTwo* chartModelTwo = browserTabContent->getDisplayedChartTwoModel();
    if (chartModelTwo != NULL) {
        const int32_t tabIndex = browserTabContent->getTabNumber();
        const ChartTwoDataTypeEnum::Enum selectedChartType = chartModelTwo->getSelectedChartTwoDataType(tabIndex);

        std::vector<ChartTwoDataTypeEnum::Enum> validChartDataTypes;
        chartModelTwo->getValidChartTwoDataTypes(validChartDataTypes);

        for (auto chartTypeButton :  m_chartTypeRadioButtons) {
            ChartTwoDataTypeEnum::Enum chartType = chartTypeButton.first;
            QRadioButton* radioButton            = chartTypeButton.second;
            
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

