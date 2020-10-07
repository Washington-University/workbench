
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_EDITOR_WIDGET_DECLARE__
#include "ChartTwoCartesianCustomSubdivisionsEditorWidget.h"
#undef __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_EDITOR_WIDGET_DECLARE__

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QToolButton>

#include "CaretAssert.h"
#include "CaretResult.h"
#include "CaretResultDialog.h"
#include "ChartTwoCartesianCustomSubdivisions.h"
#include "ChartTwoCartesianCustomSubdivisionsLabel.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "WuQDoubleSpinBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoCartesianCustomSubdivisionsEditorWidget
 * \brief Widget for editing a chart two custom axis
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    The parent widget
 */
ChartTwoCartesianCustomSubdivisionsEditorWidget::ChartTwoCartesianCustomSubdivisionsEditorWidget(QWidget* parent)
: QWidget(parent)
{
    m_rowsGridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(m_rowsGridLayout, 4, 0);
    const int gridRow = m_rowsGridLayout->rowCount();
    m_rowsGridLayout->addWidget(new QLabel("Edit"),
                                gridRow, GRID_COLUMN_CONSTRUCTION);
    m_rowsGridLayout->addWidget(new QLabel("Value"),
                                gridRow, GRID_COLUMN_VALUE);
    m_rowsGridLayout->addWidget(new QLabel("Label"),
                                gridRow, GRID_COLUMN_TEXT);
}

/**
 * Destructor.
 */
ChartTwoCartesianCustomSubdivisionsEditorWidget::~ChartTwoCartesianCustomSubdivisionsEditorWidget()
{
}

/**
 * Update the widget with the given axis
 * @param axis
 *    Axis for editing in this widget
 */
void
ChartTwoCartesianCustomSubdivisionsEditorWidget::updateContent(ChartTwoCartesianCustomSubdivisions* subdivisions)
{
    m_subdivisions = subdivisions;
    loadAxisIntoWidgets();
}

void
ChartTwoCartesianCustomSubdivisionsEditorWidget::loadAxisIntoWidgets()
{
    const int32_t numLabels = ((m_subdivisions != NULL)
                               ? m_subdivisions->getNumberOfLabels()
                               : 0);
    const int32_t numRows = static_cast<int32_t>(m_rows.size());

    /*
     * If needed, add rows
     */
    for (int32_t iRow = numRows; iRow < numLabels; iRow++) {
        /*
         * Construction Tool Button
         * Note: macro support is on each action in menu in 'createConstructionMenu'
         */
        QIcon constructionIcon;
        const bool constructionIconValid = WuQtUtilities::loadIcon(":/LayersPanel/construction.png",
                                                                   constructionIcon);
        QToolButton* constructionToolButton = new QToolButton();
        constructionToolButton->setToolTip("Add/Move/Remove Layers");
        if (constructionIconValid) {
            constructionToolButton->setIcon(constructionIcon);
        }
        else {
            constructionToolButton->setText("C");
        }
        QMenu* constructionMenu = createConstructionMenu(iRow);
        QObject::connect(constructionToolButton, &QToolButton::clicked,
                         [=]() { constructionMenu->exec(constructionToolButton->mapToGlobal(QPoint(0, 0))); } );
        
        const float spinBoxMax(1.0e10);
        WuQDoubleSpinBox* spinBox = new WuQDoubleSpinBox(this);
        spinBox->setRange(-spinBoxMax, spinBoxMax);
        spinBox->setSingleStep(1.0);
        spinBox->setDecimals(5);
        spinBox->setFixedWidth(120);
        QObject::connect(spinBox, &WuQDoubleSpinBox::valueChanged,
                         [=](double value) { this->valueSpinBoxValueChanged(iRow, value); });
        
        QLineEdit* lineEdit = new QLineEdit;
        lineEdit->setFixedWidth(100);
        QObject::connect(lineEdit, &QLineEdit::textEdited,
                         [=](const QString& text) { this->labelLineEditTextChanged(iRow, text); });
        
        std::unique_ptr<Row> rowPtr(new Row(iRow,
                                            constructionToolButton,
                                            spinBox,
                                            lineEdit));
        m_rows.push_back(std::move(rowPtr));
        
        const int gridRow = m_rowsGridLayout->rowCount();
        m_rowsGridLayout->addWidget(constructionToolButton,
                                    gridRow, GRID_COLUMN_CONSTRUCTION);
        m_rowsGridLayout->addWidget(spinBox->getWidget(),
                                    gridRow, GRID_COLUMN_VALUE);
        m_rowsGridLayout->addWidget(lineEdit,
                                    gridRow, GRID_COLUMN_TEXT);
    }

    updateRangesOfValueSpinBoxes();
    
    const int32_t newNumRows = static_cast<int32_t>(m_rows.size());
    for (int32_t iRow = 0; iRow < newNumRows; iRow++) {
        CaretAssertVectorIndex(m_rows, iRow);
        
        const bool validRowFlag(iRow < numLabels);
        if (validRowFlag) {
            m_rows[iRow]->m_valueSpinBox->setValue(m_subdivisions->getLabelNumericValue(iRow));
            m_rows[iRow]->m_labelLineEdit->setText(m_subdivisions->getLabelText(iRow));
        }
        
        m_rows[iRow]->m_constructionToolButton->setVisible(validRowFlag);
        m_rows[iRow]->m_valueSpinBox->getWidget()->setVisible(validRowFlag);
        m_rows[iRow]->m_labelLineEdit->setVisible(validRowFlag);
    }
    
    setEnabled(m_subdivisions != NULL);
    
    adjustSize();
    emit widgetSizeChanged();
}

/**
 * Update the ranges of the value spin boxes
 */
void
ChartTwoCartesianCustomSubdivisionsEditorWidget::updateRangesOfValueSpinBoxes()
{
    const int32_t numLabels = ((m_subdivisions != NULL)
                               ? m_subdivisions->getNumberOfLabels()
                               : 0);
    for (int32_t iRow = 0; iRow < numLabels; iRow++) {
        CaretAssertVectorIndex(m_rows, iRow);
        float rangeMin(0.0), rangeMax(0.0);
        m_subdivisions->getRangeForLabelAtIndex(iRow, rangeMin, rangeMax);
        m_rows[iRow]->m_valueSpinBox->setRange(rangeMin, rangeMax);
    }
}

/**
 * Create a construction menu for the given row
 * @param rowIndex
 *    Index of the row containing the menu
 */
QMenu*
ChartTwoCartesianCustomSubdivisionsEditorWidget::createConstructionMenu(const int32_t rowIndex)
{
    QMenu* menu = new QMenu();
    menu->addAction("Add Label Above",
                    [=]() { constructionMenuItemSelected(rowIndex, ConstructionType::INSERT_ABOVE); });
    menu->addAction("Add Label Below",
                    [=]() { constructionMenuItemSelected(rowIndex, ConstructionType::INSERT_BELOW); });
    menu->addAction("Remove This Label",
                    [=]() { constructionMenuItemSelected(rowIndex, ConstructionType::REMOVE); });
    return menu;
}

/**
 * Called when an item is selected from a row's construction menu
 * @param rowIndex
 *    Index of the row
 * @param constructionType
 *    Enum identifying type of construction
 */
void
ChartTwoCartesianCustomSubdivisionsEditorWidget::constructionMenuItemSelected(const int32_t rowIndex,
                                                                      const ConstructionType constructionType)
{
    std::unique_ptr<CaretResult> result;
    switch (constructionType) {
        case ConstructionType::INSERT_ABOVE:
            result = m_subdivisions->insertLabelAbove(rowIndex);
            break;
        case ConstructionType::INSERT_BELOW:
            result = m_subdivisions->insertLabelBelow(rowIndex);
            break;
        case ConstructionType::REMOVE:
            result = m_subdivisions->removeLabelAtIndex(rowIndex);
            break;
    }
    
    if (CaretResultDialog::isSuccess(result,
                                     this)) {
        loadAxisIntoWidgets();
        updateGraphics();
    }
}


/**
 * Called when a row's spinbox value is changed
 * @param rowIndex
 *    Index of the row
 * @param value
 *    New value for a label
 */
void
ChartTwoCartesianCustomSubdivisionsEditorWidget::valueSpinBoxValueChanged(const int32_t rowIndex,
                                                                          const float value)
{
    m_subdivisions->setLabelNumericValue(rowIndex,
                                         value);
    updateRangesOfValueSpinBoxes();
    updateGraphics();
}

/**
 * Called when a row's spinbox value is changed
 * @param rowIndex
 *    Index of the row
 * @param text
 *    New text for a label
 */
void
ChartTwoCartesianCustomSubdivisionsEditorWidget::labelLineEditTextChanged(const int32_t rowIndex,
                                                                  const QString& text)
{
    m_subdivisions->setLabelText(rowIndex,
                                 text);
    updateGraphics();
}

/**
 * Update the graphics
 */
void
ChartTwoCartesianCustomSubdivisionsEditorWidget::updateGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


