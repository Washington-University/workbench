
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
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::PaletteEditorControlPointGroupWidget
 * \brief Widget for editing a palette's control points
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    Parent widget
 */
PaletteEditorControlPointGroupWidget::PaletteEditorControlPointGroupWidget(QWidget* parent,
                                                                           QButtonGroup* colorEditButtonGroup,
                                                                           const bool showColumnTitles)
: QWidget(parent),
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
 * Update the content with the given control point group
 * @param controlPointGroup
 * The control point group (may be NULL)
 */
void
PaletteEditorControlPointGroupWidget::updateContent(void* controlPointGroup,
                                                    const int32_t numberOfControlsPointsForLayoutTesting)
{
    int32_t numberOfExistingRows(static_cast<int32_t>(m_paletteControlPointRows.size()));
    
    float firstValue(0.0);
    float lastValue(0.0);
    int32_t numberOfControlPoints(1);
    if (numberOfControlsPointsForLayoutTesting > 0) {
        numberOfControlPoints = numberOfControlsPointsForLayoutTesting;
        firstValue = 1.0;
        lastValue  = 0.0;
    }
    else if (numberOfControlsPointsForLayoutTesting < 0) {
        numberOfControlPoints = -numberOfControlsPointsForLayoutTesting;
        firstValue = 0.0;
        lastValue  = -1.0;
    }
    float step(lastValue - firstValue);
    if (numberOfControlPoints > 2) {
        step /= (numberOfControlPoints - 1);
    }
    
    if (controlPointGroup != NULL) {
        
    }
    
    /*
     * Create new rows
     */
    float value(firstValue);
    for (int32_t i = numberOfExistingRows; i < numberOfControlPoints; i++) {
        PaletteControlPointRow* per = new PaletteControlPointRow(this,
                                                                 m_colorEditButtonGroup,
                                                                 m_controlPointGridLayout,
                                                                 i,
                                                                 m_showColumnTitles);
        QObject::connect(per, &PaletteControlPointRow::editColorRequested,
                         this, &PaletteEditorControlPointGroupWidget::editColorRequested);
        
        per->m_valueSpinBox->setValue(value);
        value += step;
        m_paletteControlPointRows.push_back(per);
    }
    
    const int32_t numberOfRows(m_paletteControlPointRows.size());
    for (int32_t iRow = 0; iRow < numberOfRows; iRow++) {
        void* controlPoint(NULL);
        if (iRow < numberOfControlPoints) {
            /* controlPoint = */
        }
        CaretAssertVectorIndex(m_paletteControlPointRows, iRow);
        m_paletteControlPointRows[iRow]->updateContent(controlPoint,
                                                       numberOfControlPoints);
    }
}

/* ==================================================================================== */

/**
 * Constructor
 * @param paletteEditorControlPointGroupWidget
 *    The parent palette control points group widget
 * @param gridLayout
 *    Grid layout for widgets
 * @param controlPointIndex
 *    Index of the control point
 */
PaletteControlPointRow::PaletteControlPointRow(PaletteEditorControlPointGroupWidget* paletteEditorControlPointGroupWidget,
                                               QButtonGroup* colorEditButtonGroup,
                                               QGridLayout* gridLayout,
                                               const int32_t controlPointIndex,
                                               const bool showColumnTitles)
: QObject(paletteEditorControlPointGroupWidget),
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
    m_insertBelowAction = constructionMenu->addAction("Insert Control Point Below");
    m_removeAction = constructionMenu->addAction("Remove This Control Point");
    
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
    m_valueSpinBox->setSingleStep(0.01);
    
    m_valueLabel = new QLabel();
    
    m_valueStackedWidget = new QStackedWidget();
    m_valueStackedWidget->addWidget(m_valueSpinBox);
    m_valueStackedWidget->addWidget(m_valueLabel);
    
    const uint8_t red(static_cast<uint8_t>((static_cast<float>(std::rand()) / RAND_MAX) * 255.0));
    const uint8_t green(static_cast<uint8_t>((static_cast<float>(std::rand()) / RAND_MAX) * 255.0));
    const uint8_t blue(static_cast<uint8_t>((static_cast<float>(std::rand()) / RAND_MAX) * 255.0));

    m_editColorRadioButton = new QRadioButton("");
    m_editColorRadioButton->setToolTip("Click to edit this control point's color");
    colorEditButtonGroup->addButton(m_editColorRadioButton);
    QObject::connect(m_editColorRadioButton, &QRadioButton::clicked,
                     [=](bool) { emit editColorRequested(red, green, blue); } );
    
    m_colorSwatchWidget = new QWidget();
    m_colorSwatchWidget->setFixedWidth(40);
    m_colorSwatchWidget->setFixedHeight(std::max(10, m_valueSpinBox->sizeHint().height() - 2));
    m_colorSwatchWidget->setStyleSheet("background-color: rgb("
                                       + AString::number(red)
                                       + ", " + AString::number(green)
                                       + ", " + AString::number(blue)
                                       + ");");
    
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
 * Update the content with the given control point
 * @param controlPoint
 *    The control point (may be NULL)
 * @param numberOfControlPoints
 *    Number of control points
 */
void
PaletteControlPointRow::updateContent(void* controlPoint,
                                      const int32_t numberOfControlPoints)
{
    if (controlPoint != NULL) {
        
    }
    
    const bool firstControlPointFlag(m_controlPointIndex == 0);
    const bool lastControlPointFlag(m_controlPointIndex == (numberOfControlPoints - 1));
    
    m_insertAboveAction->setEnabled( ! firstControlPointFlag);
    m_insertBelowAction->setEnabled( ! lastControlPointFlag);
    m_removeAction->setEnabled( ( ! firstControlPointFlag)
                               && ( ! lastControlPointFlag) );
}


