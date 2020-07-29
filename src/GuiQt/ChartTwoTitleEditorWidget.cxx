
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

#define __CHART_TWO_TITLE_EDITOR_WIDGET_DECLARE__
#include "ChartTwoTitleEditorWidget.h"
#undef __CHART_TWO_TITLE_EDITOR_WIDGET_DECLARE__

#include <QAction>
#include <QCheckBox>
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
#include "EventChartTwoAttributesChanged.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "ModelChartTwo.h"
#include "WuQDataEntryDialog.h"
#include "WuQDoubleSpinBox.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoTitleEditorWidget
 * \brief Widget for editing chart title
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    The parent widget
 * @param parentObjectName
 *    Name of parent object for macros
 */
ChartTwoTitleEditorWidget::ChartTwoTitleEditorWidget(QWidget* parent,
                                                     const QString& parentObjectName)
: QWidget(parent)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    const QString objectNamePrefix(parentObjectName
                                   + ":ChartTitle:");
    
    m_textLineEdit = new QLineEdit();
    QObject::connect(m_textLineEdit, &QLineEdit::textEdited,
                     this, &ChartTwoTitleEditorWidget::textLineEditChanged);
    
    m_titleSizeSpinBox = new WuQDoubleSpinBox(this);
    m_titleSizeSpinBox->setRangePercentage(0.0, 99.0);
    QObject::connect(m_titleSizeSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     this, &ChartTwoTitleEditorWidget::sizeSpinBoxValueChanged);
    m_titleSizeSpinBox->setToolTip("Set height of title as percentage of tab height");
    m_titleSizeSpinBox->getWidget()->setObjectName(objectNamePrefix
                                       + "Height");
    macroManager->addMacroSupportToObject(m_titleSizeSpinBox->getWidget(),
                                          "Set chart title height");
    
    m_paddingSizeSpinBox = new WuQDoubleSpinBox(this);
    m_paddingSizeSpinBox->setRangePercentage(0.0, 99.0);
    QObject::connect(m_paddingSizeSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     this, &ChartTwoTitleEditorWidget::sizeSpinBoxValueChanged);
    m_paddingSizeSpinBox->getWidget()->setObjectName(objectNamePrefix
                                                     + "Padding");
    macroManager->addMacroSupportToObject(m_paddingSizeSpinBox->getWidget(),
                                          "Set chart padding");
    
    m_paddingSizeSpinBox->setToolTip("Set padding (space between edge and labels) as percentage of tab height");
    QGridLayout* layout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 3, 0);
    int row = 0;
    layout->addWidget(new QLabel("Text"), row, 0);
    layout->addWidget(m_textLineEdit, row, 1);
    row++;
    layout->addWidget(new QLabel("Size"), row, 0);
    layout->addWidget(m_titleSizeSpinBox->getWidget(), row, 1);
    row++;
    layout->addWidget(new QLabel("Pad"), row, 0);
    layout->addWidget(m_paddingSizeSpinBox->getWidget(), row, 1);
    row++;
}

/**
 * Destructor.
 */
ChartTwoTitleEditorWidget::~ChartTwoTitleEditorWidget()
{
}

/**
 * Update content of this tool bar component.
 *
 * @param chartTwoOverlaySet
 *     The chart two overlay set.
 */
void
ChartTwoTitleEditorWidget::updateControls(ChartTwoOverlaySet* chartTwoOverlaySet)
{
    m_chartOverlaySet = chartTwoOverlaySet;
    
    if (m_chartOverlaySet != NULL) {
        setEnabled(true);
        const ChartTwoTitle* chartTitle = m_chartOverlaySet->getChartTitle();
        m_textLineEdit->setText(m_chartOverlaySet->getChartTitle()->getText());
        
        m_titleSizeSpinBox->blockSignals(true);
        m_titleSizeSpinBox->setValue(chartTitle->getTextSize());
        m_titleSizeSpinBox->blockSignals(false);
        
        m_paddingSizeSpinBox->blockSignals(true);
        m_paddingSizeSpinBox->setValue(chartTitle->getPaddingSize());
        m_paddingSizeSpinBox->blockSignals(false);
        
        m_textLineEdit->setFocus();
    }
    else {
        setEnabled(false);
    }
}

/**
 * Called when a size spin box value is changed
 */
void
ChartTwoTitleEditorWidget::sizeSpinBoxValueChanged(double)
{
    if (m_chartOverlaySet != NULL) {
        ChartTwoTitle* chartTitle = m_chartOverlaySet->getChartTitle();
        chartTitle->setTextSize(m_titleSizeSpinBox->value());
        chartTitle->setPaddingSize(m_paddingSizeSpinBox->value());
        updateGraphics();
    }
}

/**
 * Called when title text is edited
 */
void
ChartTwoTitleEditorWidget::textLineEditChanged(const QString& text)
{
    if (m_chartOverlaySet != NULL) {
        ChartTwoTitle* chartTitle = m_chartOverlaySet->getChartTitle();
        chartTitle->setText(text);
        updateGraphics();
    }
}

void
ChartTwoTitleEditorWidget::updateGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}
