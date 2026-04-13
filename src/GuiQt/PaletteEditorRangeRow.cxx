
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

#define __PALETTE_EDITOR_RANGE_ROW_DECLARE__
#include "PaletteEditorRangeRow.h"
#undef __PALETTE_EDITOR_RANGE_ROW_DECLARE__

#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QStackedWidget>
#include <QToolButton>

#include "CaretAssert.h"
#include "WorkbenchToolButton.h"
#include "WuQGridLayoutGroup.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::PaletteEditorRangeRow 
 * \brief Contains widgets for one row in a palette editor range
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param rowIndex
 *    Index of this 'row' (zero at top)
 * @param rangeMode
 *    Range of data in this row (pos/neg/zero)
 * @param colorEditButtonGroup
 *    Button group that keeps edit color radio buttons mutually exclusive
 * @param parentGridLayout
 *    Layout for this row
 * @param spinBoxWidth
 *    Width of spin box
 * @param parent
 *    Parent object
 */
PaletteEditorRangeRow::PaletteEditorRangeRow(const int32_t rowIndex,
                                             const PaletteEditorRangeWidget::DataRangeMode rangeMode,
                                             QButtonGroup* colorEditButtonGroup,
                                             QGridLayout* parentGridLayout,
                                             const int32_t spinBoxWidth,
                                             QObject* parent)
: QObject(parent),
m_rowIndex(rowIndex),
m_rangeMode(rangeMode)
{
    bool posNegFlag(false);
    bool zeroFlag(false);
    switch (m_rangeMode) {
        case PaletteEditorRangeWidget::DataRangeMode::NEGATIVE:
            posNegFlag = true;
            break;
        case PaletteEditorRangeWidget::DataRangeMode::POSITIVE:
            posNegFlag = true;
            break;
        case PaletteEditorRangeWidget::DataRangeMode::ZERO:
            zeroFlag = true;
            break;
    }
    
    m_gridLayoutGroup = new WuQGridLayoutGroup(parentGridLayout,
                                               this);
    
    m_colorEditRadioButton = new QRadioButton("");
    m_colorEditRadioButton->setToolTip("Click to edit this control point's color");
    colorEditButtonGroup->addButton(m_colorEditRadioButton);
    QObject::connect(m_colorEditRadioButton, &QRadioButton::clicked,
                     [=] (bool) { emit signalColorEditingRequested(m_rgb); });

    m_colorSwatchWidget = new QWidget();
    m_colorSwatchWidget->setFixedWidth(40);
    m_colorSwatchWidget->setFixedHeight(m_colorEditRadioButton->sizeHint().height() + 4);
    
    const int row(parentGridLayout->rowCount());
    if (posNegFlag) {
        QWidget* scalarWidget = createScalarEditingWidget(spinBoxWidth);
        
        m_gridLayoutGroup->addWidget(scalarWidget,
                                     row, COLUMN_SCALAR);
    }
    
    if (zeroFlag) {
        QLabel* zeroLabel = new QLabel("Zero");
        zeroLabel->setFixedWidth(spinBoxWidth);
        m_gridLayoutGroup->addWidget(zeroLabel,
                                     row, COLUMN_SCALAR);
    }

    m_gridLayoutGroup->addWidget(m_colorSwatchWidget,
                                 row, COLUMN_COLOR_SWATCH, Qt::AlignCenter);
    m_gridLayoutGroup->addWidget(m_colorEditRadioButton,
                                 row, COLUMN_RADIO_BUTTON);
}

/**
 * Destructor.
 */
PaletteEditorRangeRow::~PaletteEditorRangeRow()
{
}

/**
 * @return The value of the scalar
 */
float
PaletteEditorRangeRow::getScalar() const
{
    if (m_valueSpinBox != NULL) {
        return m_valueSpinBox->value();
    }
    return 0.0f;
}

/**
 * @return The RGB color
 */
CaretRgb
PaletteEditorRangeRow::getRgb() const
{
    return m_rgb;
}

/**
 * Set the scalar value
 * @param scalar
 *    New scalar value
 */
void
PaletteEditorRangeRow::setScalar(const float scalar)
{
    int32_t decimals(3);
    if (m_valueSpinBox != NULL) {
        QSignalBlocker blocker(m_valueSpinBox);
        m_valueSpinBox->setValue(scalar);
        decimals = m_valueSpinBox->decimals();
    }
    
    if (m_valueLabel != NULL) {
        m_valueLabel->setText(QString::number(scalar, 'f', decimals));
    }
}

/**
 * Set the rgb color and update the color swatch
 *
 * @param rgb
 *    New RGB value
 */
void
PaletteEditorRangeRow::setRgb(const CaretRgb& rgb)
{
    m_rgb = rgb;
    
    for (auto c : m_rgb) {
        CaretAssert((c >= 0)
                    && (c <= 255));
    }
    
    m_colorSwatchWidget->setStyleSheet("background-color: rgb("
                                       + AString::number(m_rgb.red())
                                       + ", " + AString::number(m_rgb.green())
                                       + ", " + AString::number(m_rgb.blue())
                                       + ");");
}

/**
 * Update the RGB color and color swatch if this instance's
 * radio button (for color editing) is checked.  This gets
 * called when the user changes the color in the color editor.
 *
 * @param rgb
 *    New RGB value
 */
void
PaletteEditorRangeRow::updateRgbIfRadioButtonChecked(const CaretRgb& rgb)
{
    if (m_colorEditRadioButton->isChecked()) {
        setRgb(rgb);
        emit signalDataChanged();
    }
}

/**
 * Called when the control point value is changed
 *
 * @param value
 *    New value but ignored as owner of this row can
 *    call a method to retrieve the scalar value
 */
void
PaletteEditorRangeRow::scalarValueChangedByUser(double /* value */)
{
    emit signalDataChanged();
}

/**
 * @return New instance of widgets for scalar editing
 * @param spinBoxWidth
 *    Width of spin box
 */
QWidget*
PaletteEditorRangeRow::createScalarEditingWidget(const int32_t spinBoxWidth)
{
    m_valueSpinBox = new QDoubleSpinBox();
    m_valueSpinBox->setMinimum(-1.0);
    m_valueSpinBox->setMaximum(1.0);
    m_valueSpinBox->setDecimals(3);
    m_valueSpinBox->setSingleStep(getScalarValueSpinBoxSingleStep());
    m_valueSpinBox->setFixedWidth(spinBoxWidth);
    QObject::connect(m_valueSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &PaletteEditorRangeRow::scalarValueChangedByUser);
    
    m_valueLabel = new QLabel("     ");
    
    m_valueStackedWidget = new QStackedWidget();
    m_valueStackedWidget->addWidget(m_valueSpinBox);
    m_valueStackedWidget->addWidget(m_valueLabel);
    
    return m_valueStackedWidget;
}

/**
 * Set the widgets visible (show/hide)
 * @param visibleFlag
 *     New visibility status
 */
void
PaletteEditorRangeRow::setWidgetsVisible(const bool visibleFlag)
{
    m_gridLayoutGroup->setVisible(visibleFlag);
}

/**
 * Update the range of the scalar value spin box
 * @param minValueIn
 *    Minimum value
 * @param maxValueIn
 *    Maximum value
 */
void
PaletteEditorRangeRow::updateScalarValueRange(const float minValueIn,
                                              const float maxValueIn)
{
    float minValue(minValueIn);
    float maxValue(maxValueIn);
    
    if (maxValue > minValue) {
        /*
         * Must be slightly less/more so that this scalar
         * is not exactly the same as scalar above or below
         */
        minValue = minValue + getScalarValueSpinBoxSingleStep();
        maxValue = maxValue - getScalarValueSpinBoxSingleStep();
        if (maxValue < minValue) {
            maxValue = (minValueIn + maxValueIn) / 2.0;
            minValue = maxValue;
        }
    }
    
    if (m_valueSpinBox != NULL) {
        QSignalBlocker blocker(m_valueSpinBox);
        m_valueSpinBox->setRange(minValue,
                                 maxValue);
    }
}

/**
 * Set the widgets visible (show/hide)
 * @param visibleFlag
 *     New visibility status
 */
void
PaletteEditorRangeRow::updateContent(const int32_t numberOfControlPoints)
{
    m_insertAboveValidFlag = false;
    m_insertBelowValidFlag = false;
    m_removeValidFlag   = false;
    
    switch (m_rangeMode) {
        case PaletteEditorRangeWidget::DataRangeMode::NEGATIVE:
        case PaletteEditorRangeWidget::DataRangeMode::POSITIVE:
        {
            if (m_rowIndex == 0) {
                m_insertBelowValidFlag = true;
                m_valueStackedWidget->setCurrentWidget(m_valueLabel);
            }
            else if (m_rowIndex == (numberOfControlPoints - 1)) {
                m_insertAboveValidFlag = true;
                m_valueStackedWidget->setCurrentWidget(m_valueLabel);
            }
            else {
                m_insertAboveValidFlag = true;
                m_insertBelowValidFlag = true;
                m_removeValidFlag   = true;
                m_valueStackedWidget->setCurrentWidget(m_valueSpinBox);
            }
        }
            break;
        case PaletteEditorRangeWidget::DataRangeMode::ZERO:
            break;
    }
}

/**
 * Select the radio button
 */
void
PaletteEditorRangeRow::selectRadioButton()
{
    m_colorEditRadioButton->setChecked(true);
}

