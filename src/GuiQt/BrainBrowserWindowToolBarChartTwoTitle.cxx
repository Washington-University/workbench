
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_TITLE_DECLARE__
#include "BrainBrowserWindowToolBarChartTwoTitle.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_TITLE_DECLARE__

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationPercentSizeText.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ChartTwoOverlaySet.h"
#include "ChartTwoTitle.h"
#include "ModelChartTwo.h"
#include "WuQDataEntryDialog.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarChartTwoTitle 
 * \brief Toolbar component for chart orientation.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
BrainBrowserWindowToolBarChartTwoTitle::BrainBrowserWindowToolBarChartTwoTitle(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    m_showTitleCheckBox = new QCheckBox("Show Title");
    m_showTitleCheckBox->setToolTip("Show the title at the top of the chart");
    QObject::connect(m_showTitleCheckBox, &QCheckBox::toggled,
                     this, &BrainBrowserWindowToolBarChartTwoTitle::showTitleCheckBoxToggled);
    
    QAction* editTitleAction = new QAction("Edit Title...");
    editTitleAction->setToolTip("Edit the chart title in a dialog");
    QObject::connect(editTitleAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarChartTwoTitle::editTitleActionTriggered);
    
    QToolButton* editTitleToolButton = new QToolButton;
    WuQtUtilities::setToolButtonStyleForQt5Mac(editTitleToolButton);
    editTitleToolButton->setDefaultAction(editTitleAction);
    
    m_titleSizeSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(0.0, 100.0, 1.0, 1);
    m_titleSizeSpinBox->setSuffix("%");
    QObject::connect(m_titleSizeSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoTitle::sizeSpinBoxValueChanged);
    m_titleSizeSpinBox->setToolTip("Set height of title as percentage of tab height");
    
    m_paddingSizeSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(0.0, 100.0, 1.0, 1);
    m_paddingSizeSpinBox->setSuffix("%");
    QObject::connect(m_paddingSizeSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoTitle::sizeSpinBoxValueChanged);
    
    m_paddingSizeSpinBox->setToolTip("Set padding (space between edge and labels) as percentage of tab height");
    QGridLayout* layout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 3, 0); //4, 5);
    int row = 0;
    layout->addWidget(m_showTitleCheckBox, row, 0, 1, 2, Qt::AlignHCenter);
    row++;
    layout->addWidget(new QLabel("Size"), row, 0);
    layout->addWidget(m_titleSizeSpinBox, row, 1);
    row++;
    layout->addWidget(new QLabel("Pad"), row, 0);
    layout->addWidget(m_paddingSizeSpinBox, row, 1);
    row++;
    layout->addWidget(editTitleToolButton, row, 0, 1, 2, Qt::AlignHCenter);
    row++;
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarChartTwoTitle::~BrainBrowserWindowToolBarChartTwoTitle()
{
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarChartTwoTitle::updateContent(BrowserTabContent* browserTabContent)
{
    m_chartOverlaySet = NULL;
    
    if (browserTabContent != NULL) {
        ModelChartTwo* modelChartTwo = browserTabContent->getDisplayedChartTwoModel();
        if (modelChartTwo != NULL) {
            const int32_t tabIndex = browserTabContent->getTabNumber();
            m_chartOverlaySet = modelChartTwo->getChartTwoOverlaySet(tabIndex);
        }
    }
    
    if (m_chartOverlaySet != NULL) {
        setEnabled(true);
        const ChartTwoTitle* chartTitle = m_chartOverlaySet->getChartTitle();
        m_showTitleCheckBox->setChecked(chartTitle->isDisplayed());
        
        m_titleSizeSpinBox->blockSignals(true);
        m_titleSizeSpinBox->setValue(chartTitle->getTextSize());
        m_titleSizeSpinBox->blockSignals(false);
        
        m_paddingSizeSpinBox->blockSignals(true);
        m_paddingSizeSpinBox->setValue(chartTitle->getPaddingSize());
        m_paddingSizeSpinBox->blockSignals(false);
    }
    else {
        setEnabled(false);
    }
}

/**
 * Called when show title checkbox is toggled.
 *
 * @param checked
 *     Status of checkbox.
 */
void
BrainBrowserWindowToolBarChartTwoTitle::showTitleCheckBoxToggled(bool checked)
{
    if (m_chartOverlaySet != NULL) {
        ChartTwoTitle* chartTitle = m_chartOverlaySet->getChartTitle();
        chartTitle->setDisplayed(checked);
        this->updateGraphicsWindow();
        this->updateOtherYokedWindows();
    }
}

/**
 * Called when a size spin box value is changed
 */
void
BrainBrowserWindowToolBarChartTwoTitle::sizeSpinBoxValueChanged(double)
{
    if (m_chartOverlaySet != NULL) {
        ChartTwoTitle* chartTitle = m_chartOverlaySet->getChartTitle();
        chartTitle->setTextSize(m_titleSizeSpinBox->value());
        chartTitle->setPaddingSize(m_paddingSizeSpinBox->value());
        this->updateGraphicsWindow();
        this->updateOtherYokedWindows();
    }
}

/**
 * Called when edit title action is triggered.
 */
void
BrainBrowserWindowToolBarChartTwoTitle::editTitleActionTriggered()
{
    if (m_chartOverlaySet != NULL) {
        WuQDataEntryDialog newNameDialog("Chart Title",
                                         this);
        QLineEdit* lineEdit = newNameDialog.addLineEditWidget("Title");
        lineEdit->setText(m_chartOverlaySet->getChartTitle()->getText());
        if (newNameDialog.exec() == WuQDataEntryDialog::Accepted) {
            const AString name = lineEdit->text().trimmed();
            m_chartOverlaySet->getChartTitle()->setText(name);
            this->updateGraphicsWindow();
            this->updateOtherYokedWindows();
        }
    }
}

