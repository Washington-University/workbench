
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

#define __PALETTE_EDITOR_CONTROL_POINT_GROUP_WIDGET_DECLARE__
#include "PaletteEditorControlPointGroupWidget.h"
#undef __PALETTE_EDITOR_CONTROL_POINT_GROUP_WIDGET_DECLARE__

#include <cstdlib>

#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QRadioButton>
#include <QStackedWidget>
#include <QToolButton>

#include "CaretAssert.h"
#include "PaletteNew.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::PaletteEditorControlPointGroupWidget
 * \brief Widget for editing a palette's control points
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param dataRangeMode
 *    Range of data (positive, negative, zero)
 * @param colorEditButtonGroup
 *    Button group for the color edit radio button so that they are mutually exclusive
 * @param showColumnTitles
 *    True if color titles show be show, else false
 * @param parent
 *    Parent widget
 */
PaletteEditorControlPointGroupWidget::PaletteEditorControlPointGroupWidget(std::vector<PaletteNew::ScalarColor>& scalarColors,
                                                                           const DataRangeMode dataRangeMode,
                                                                           QButtonGroup* colorEditButtonGroup,
                                                                           const bool showColumnTitles,
                                                                           QWidget* parent)
: QWidget(parent),
m_scalarColors(scalarColors),
m_dataRangeMode(dataRangeMode),
m_colorEditButtonGroup(colorEditButtonGroup),
m_showColumnTitles(showColumnTitles)
{
    m_controlPointGridLayout = new QGridLayout(this);
    m_controlPointGridLayout->setVerticalSpacing(m_controlPointGridLayout->verticalSpacing() / 2);
}

/**
 * Destructor.
 */
PaletteEditorControlPointGroupWidget::~PaletteEditorControlPointGroupWidget()
{
}

/**
 * Update the content with the given palette
 * @param palette
 * The palette (may be NULL)
 */
void
PaletteEditorControlPointGroupWidget::updateContent()
{
    int32_t numberOfExistingRows(static_cast<int32_t>(m_paletteControlPointRows.size()));
    
    int32_t numberOfControlPoints(0);

//    std::vector<PaletteNew::ScalarColor> scalarColors;
//    if (palette != NULL) {
//        switch (m_dataRangeMode) {
//            case DataRangeMode::NEGATIVE:
//                scalarColors = palette->getNegRange();
//                break;
//            case DataRangeMode::POSITIVE:
//                scalarColors = palette->getPosRange();
//                break;
//            case DataRangeMode::ZERO:
//            {
//                float zeroColor[3];
//                palette->getZeroColor(zeroColor);
//                PaletteNew::ScalarColor sc(0.0, zeroColor);
//                scalarColors.push_back(sc);
//            }
//                break;
//        }
//    }
    
    numberOfControlPoints = static_cast<int32_t>(m_scalarColors.size());
    
    /*
     * Create new rows
     */
    for (int32_t i = numberOfExistingRows; i < numberOfControlPoints; i++) {
        PaletteControlPointRow* per = new PaletteControlPointRow(m_dataRangeMode,
                                                                 i,
                                                                 this,
                                                                 m_colorEditButtonGroup,
                                                                 m_controlPointGridLayout,
                                                                 m_showColumnTitles);
        QObject::connect(per, &PaletteControlPointRow::editColorRequested,
                         this, &PaletteEditorControlPointGroupWidget::editColorRequested);
        QObject::connect(per, &PaletteControlPointRow::modificationRequested,
                         this, &PaletteEditorControlPointGroupWidget::modificationRequest);
        QObject::connect(per, &PaletteControlPointRow::controlPointValueChanged,
                         this, &PaletteEditorControlPointGroupWidget::controlPointValueChangeRequested);
        
        m_paletteControlPointRows.push_back(per);
    }
    
    const int32_t numberOfRows(m_paletteControlPointRows.size());
    for (int32_t iRow = 0; iRow < numberOfRows; iRow++) {
        PaletteNew::ScalarColor* sc(NULL);
        if (iRow < numberOfControlPoints) {
            CaretAssertVectorIndex(m_scalarColors, iRow);
            sc = &m_scalarColors[iRow];
        }
        CaretAssertVectorIndex(m_paletteControlPointRows, iRow);
        m_paletteControlPointRows[iRow]->updateContent(sc,
                                                       numberOfControlPoints);
    }
}

/**
 * Update the control point color.  Typically called as user changes color in the color editor.
 *
 * @param red
 * New red component.
 * @param green
 * New green component.
 * @param blue
 * New blue component.
 */
void
PaletteEditorControlPointGroupWidget::updateControlPointColor(const uint8_t red,
                                                              const uint8_t green,
                                                              const uint8_t blue)
{
    for (auto cpRow : m_paletteControlPointRows) {
        if (cpRow->m_editColorRadioButton->isVisible()) {
            if (cpRow->m_editColorRadioButton->isEnabled()) {
                if (cpRow->m_editColorRadioButton->isChecked()) {
                    cpRow->updateColor(red, green, blue);
                }
            }
        }
    }
}

/**
 * Called when a control point modification is requested
 * @param controlPointIndex
 * Index of the control point
 * @param modificationOperation
 * The modification operation
 */
void
PaletteEditorControlPointGroupWidget::modificationRequest(const int32_t controlPointIndex,
                                    const PaletteEditorControlPointGroupWidget::ModificationOperation modificationOperation)
{
    switch (modificationOperation) {
        case PaletteEditorControlPointGroupWidget::ModificationOperation::INSERT_CONTROL_POINT_ABOVE:
            std::cout << "Insert above ";
            break;
        case PaletteEditorControlPointGroupWidget::ModificationOperation::INSERT_CONTROL_POINT_BELOW:
            std::cout << "Insert below ";
            break;
        case PaletteEditorControlPointGroupWidget::ModificationOperation::REMOVE_CONTROL_POINT:
            std::cout << "Remove ";
            break;
    }
    
    std::cout << controlPointIndex << std::endl;
}

/**
 * Called when a control point scalar is changed
 * @param controlPointIndex
 * Index of the control point
 * @param value
 * New value for the control point at the given index
 */
void
PaletteEditorControlPointGroupWidget::controlPointValueChangeRequested(const int32_t controlPointIndex,
                                                               float value)
{
//    NEED TO GET PARENT DIALOG TO UPDATE THE COLOR
//
//    THIS CLASS NEEDS TO UPDATE MIN/MAX of SPIN BOXES
}


/* ==================================================================================== */

/**
 * Constructor
 * @param paletteEditorControlPointGroupWidget
 *    The parent palette control points group widget
 * @param colorEditButtonGroup
 *    Button group for the color edit radio button so that they are mutually exclusive
 * @param gridLayout
 *    Grid layout for widgets
 * @param controlPointIndex
 *    Index of the control point
 * @param showColumnTitles
 *    True if color titles show be show, else false
 */
PaletteControlPointRow::PaletteControlPointRow(const PaletteEditorControlPointGroupWidget::DataRangeMode dataRangeMode,
                                               const int32_t controlPointIndex,
                                               PaletteEditorControlPointGroupWidget* paletteEditorControlPointGroupWidget,
                                               QButtonGroup* colorEditButtonGroup,
                                               QGridLayout* gridLayout,
                                               const bool showColumnTitles)
: QObject(paletteEditorControlPointGroupWidget),
m_dataRangeMode(dataRangeMode),
m_controlPointIndex(controlPointIndex)
{
    int32_t columnCount(0);
    const int32_t columnConstruction(columnCount++);
    const int32_t columnControlPoint(columnCount++);
    const int32_t columnColorSwatch(columnCount++);
    const int32_t columnEdit(columnCount++);
    
    if (m_controlPointIndex == 0) {
        if (showColumnTitles) {
            const bool showAllTitlesFlag(false);
            if (showAllTitlesFlag) {
                int32_t row(gridLayout->rowCount());
                gridLayout->addWidget(new QLabel("Modify"),
                                      row, columnConstruction, Qt::AlignHCenter);
                gridLayout->addWidget(new QLabel("Control"),
                                      row, columnControlPoint, Qt::AlignHCenter);
                gridLayout->addWidget(new QLabel("Point"),
                                      row + 1, columnControlPoint, Qt::AlignHCenter);
                gridLayout->addWidget(new QLabel("Color"),
                                      row, columnColorSwatch, Qt::AlignHCenter);
                gridLayout->addWidget(new QLabel("Color"),
                                      row, columnEdit, Qt::AlignHCenter);
                gridLayout->addWidget(new QLabel("Edit"),
                                      row + 1, columnEdit, Qt::AlignHCenter);
            }
            else {
                int32_t row(gridLayout->rowCount());
                gridLayout->addWidget(new QLabel("Control Point"),
                                      row, columnConstruction, 1, 2, Qt::AlignHCenter);
                gridLayout->addWidget(new QLabel("Color Edit"),
                                      row, columnColorSwatch, 1, 2, Qt::AlignHCenter);
            }
        }
        
        gridLayout->setColumnStretch(columnCount, 100);
    }
    
    QMenu* constructionMenu = new QMenu();
    m_insertAboveAction = constructionMenu->addAction("Insert Control Point Above");
    QObject::connect(m_insertAboveAction, &QAction::triggered,
                     this, [=](bool) { modificationRequested(m_controlPointIndex,
                                                                  PaletteEditorControlPointGroupWidget::ModificationOperation::INSERT_CONTROL_POINT_ABOVE); });

    m_insertBelowAction = constructionMenu->addAction("Insert Control Point Below");
    QObject::connect(m_insertBelowAction, &QAction::triggered,
                     this, [=](bool) {modificationRequested(m_controlPointIndex,
                                                                  PaletteEditorControlPointGroupWidget::ModificationOperation::INSERT_CONTROL_POINT_BELOW); });

    m_removeAction = constructionMenu->addAction("Remove This Control Point");
    QObject::connect(m_removeAction, &QAction::triggered,
                     this, [=](bool) { modificationRequested(m_controlPointIndex,
                                                                  PaletteEditorControlPointGroupWidget::ModificationOperation::REMOVE_CONTROL_POINT); });

    QIcon constructionIcon;
    const bool constructionIconValid = WuQtUtilities::loadIcon(":/LayersPanel/construction.png",
                                                               constructionIcon);
    m_constructionToolButton = new QToolButton();
    if (constructionIconValid) {
        m_constructionToolButton->setIcon(constructionIcon);
    }
    else {
        m_constructionToolButton->setText("C");
    }
    m_constructionToolButton->setPopupMode(QToolButton::InstantPopup);
    m_constructionToolButton->setMenu(constructionMenu);
    
    m_valueSpinBox = new QDoubleSpinBox();
    m_valueSpinBox->setMinimum(-1.0);
    m_valueSpinBox->setMaximum(1.0);
    m_valueSpinBox->setDecimals(3);
    m_valueSpinBox->setSingleStep(0.001);
    QObject::connect(m_valueSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &PaletteControlPointRow::controlPointValueChangedByUser);
    
    m_valueLabel = new QLabel();
    
    m_valueStackedWidget = new QStackedWidget();
    m_valueStackedWidget->addWidget(m_valueSpinBox);
    m_valueStackedWidget->addWidget(m_valueLabel);
    
    m_editColorRadioButton = new QRadioButton("");
    m_editColorRadioButton->setToolTip("Click to edit this control point's color");
    colorEditButtonGroup->addButton(m_editColorRadioButton);
    QObject::connect(m_editColorRadioButton, &QRadioButton::clicked,
        [=](bool) { if (m_scalarColor != NULL) {
                        uint8_t r(m_scalarColor->color[0] * 255.0);
                        uint8_t g(m_scalarColor->color[1] * 255.0);
                        uint8_t b(m_scalarColor->color[2] * 255.0);
                        emit editColorRequested(m_controlPointIndex,
                                                r, g, b); } } );
    
    m_colorSwatchWidget = new QWidget();
    m_colorSwatchWidget->setFixedWidth(40);
    m_colorSwatchWidget->setFixedHeight(std::max(10, m_valueSpinBox->sizeHint().height() - 2));
    
    const int32_t row(gridLayout->rowCount());
    gridLayout->addWidget(m_constructionToolButton,
                          row, columnConstruction, Qt::AlignCenter);
    gridLayout->addWidget(m_valueStackedWidget,
                          row, columnControlPoint, Qt::AlignCenter);
    gridLayout->addWidget(m_colorSwatchWidget,
                          row, columnColorSwatch, Qt::AlignCenter);
    gridLayout->addWidget(m_editColorRadioButton,
                          row, columnEdit, Qt::AlignRight);
}

/**
 * Destructor
 */
PaletteControlPointRow::~PaletteControlPointRow()
{
}

/**
 * Called when the control point value is changed
 *
 * @param value
 *    New valuel
 */
void
PaletteControlPointRow::controlPointValueChangedByUser(double value)
{
    if (m_scalarColor != NULL) { m_scalarColor->scalar = value;
        emit controlPointValueChanged(m_controlPointIndex,
                                      value);
    }
}

/**
 * Update the content with the given control point
 * @param controlPoint
 *    The control point (may be NULL)
 * @param numberOfControlPoints
 *    Number of control points
 */
void
PaletteControlPointRow::updateContent(PaletteNew::ScalarColor* scalarColor,
                                      const int32_t numberOfControlPoints)
{
    QSignalBlocker valueSpinBoxBlocker(m_valueSpinBox);
    
    m_scalarColor = scalarColor;
    
    bool showWidgetsFlag(false);
    if (m_scalarColor != NULL) {
        showWidgetsFlag = true;
    }
    
    const bool firstControlPointFlag(m_controlPointIndex == 0);
    const bool lastControlPointFlag(m_controlPointIndex == (numberOfControlPoints - 1));
    
    m_insertAboveAction->setEnabled( ! firstControlPointFlag);
    m_insertBelowAction->setEnabled( ! lastControlPointFlag);
    m_removeAction->setEnabled( ( ! firstControlPointFlag)
                               && ( ! lastControlPointFlag) );
    
    m_constructionToolButton->setVisible(showWidgetsFlag);
    m_valueSpinBox->setVisible(showWidgetsFlag);
    m_colorSwatchWidget->setVisible(showWidgetsFlag);
    m_editColorRadioButton->setVisible(showWidgetsFlag);
    
    if (scalarColor != NULL) {
        QSignalBlocker valueBlocker(m_valueSpinBox);
        m_valueSpinBox->setValue(scalarColor->scalar);
        const uint8_t red(static_cast<uint8_t>(scalarColor->color[0] * 255.0));
        const uint8_t green(static_cast<uint8_t>(scalarColor->color[1] * 255.0));
        const uint8_t blue(static_cast<uint8_t>(scalarColor->color[2] * 255.0));
        updateColor(red,
                    green,
                    blue);
    }
}

void
PaletteControlPointRow::updateColor(const uint8_t red,
                                    const uint8_t green,
                                    const uint8_t blue)
{
    if (m_scalarColor != NULL) {
        m_scalarColor->color[0] = red / 255.0;
        m_scalarColor->color[1] = green / 255.0;
        m_scalarColor->color[2] = blue / 255.0;
    }
    
    m_colorSwatchWidget->setStyleSheet("background-color: rgb("
                                       + AString::number(red)
                                       + ", " + AString::number(green)
                                       + ", " + AString::number(blue)
                                       + ");");
}

