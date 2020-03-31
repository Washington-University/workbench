
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

#define __PALETTE_EDITOR_RANGE_WIDGET_DECLARE__
#include "PaletteEditorRangeWidget.h"
#undef __PALETTE_EDITOR_RANGE_WIDGET_DECLARE__

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
#include "CaretLogger.h"
#include "PaletteEditorRangeRow.h"
#include "PaletteNew.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::PaletteEditorRangeWidget
 * \brief Widget for editing a palette's control points
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param dataRangeMode
 *    Range of data (positive, negative, zero)
 * @param colorEditButtonGroup
 *    Button group for the color edit radio button so that they are mutually exclusive
 * @param columnTitlesMode
 *    Mode for display of column titles
 * @param parent
 *    Parent widget
 */
PaletteEditorRangeWidget::PaletteEditorRangeWidget(const DataRangeMode dataRangeMode,
                                                   QButtonGroup* colorEditButtonGroup,
                                                   const ColumnTitlesMode columnTitlesMode,
                                                   QWidget* parent)
: QWidget(parent),
m_dataRangeMode(dataRangeMode),
m_colorEditButtonGroup(colorEditButtonGroup)
{
    m_controlPointGridLayout = new QGridLayout(this);
    m_controlPointGridLayout->setVerticalSpacing(m_controlPointGridLayout->verticalSpacing() / 2);

    switch (columnTitlesMode) {
        case ColumnTitlesMode::SHOW_NO:
            break;
        case ColumnTitlesMode::SHOW_YES:
            int32_t row(m_controlPointGridLayout->rowCount());
            m_controlPointGridLayout->addWidget(new QLabel("Control Point"),
                                                row, 0, 1, 2, Qt::AlignHCenter);
            m_controlPointGridLayout->addWidget(new QLabel("Color Edit"),
                                                row, 2, 1, 2, Qt::AlignHCenter);
            break;
    }
}

/**
 * Destructor.
 */
PaletteEditorRangeWidget::~PaletteEditorRangeWidget()
{
}

/**
 * @return Scalar colors for the range
 */
std::vector<PaletteNew::ScalarColor>
PaletteEditorRangeWidget::getScalarColors() const
{
    std::vector<PaletteNew::ScalarColor> scalarColorsOut;
    
    for (int32_t i = (m_numberOfValidControlPoints - 1); i >= 0; i--) {
        CaretAssertVectorIndex(m_rowWidgets, i);
        const PaletteEditorRangeRow* row = m_rowWidgets[i];
        
        const CaretRgb rgb = row->getRgb();
        const PaletteNew::ScalarColor scalarColor =
            PaletteNew::ScalarColor::fromRGB255(row->getScalar(),
                                                rgb.red(),
                                                rgb.green(),
                                                rgb.blue());
        scalarColorsOut.push_back(scalarColor);
    }
    
    return scalarColorsOut;
}


/**
 * Update with the given scalar colors
 * @param scalarColorsIn
 *    Update using these scalar colors
 */
void
PaletteEditorRangeWidget::updateContent(const std::vector<PaletteNew::ScalarColor>& scalarColorsIn)
{
    /*
     * Note: Inputs are ordered with ascending scalars but
     * we want them descending to that greatest value is at top
     */
    std::vector<PaletteNew::ScalarColor> scalarColors = scalarColorsIn;
    std::reverse(scalarColors.begin(),
                 scalarColors.end());
    
    int32_t numberOfExistingRows(static_cast<int32_t>(m_rowWidgets.size()));
    
    m_numberOfValidControlPoints = static_cast<int32_t>(scalarColors.size());
    
    /*
     * Create new rows
     */
    for (int32_t i = numberOfExistingRows; i < m_numberOfValidControlPoints; i++) {
        
        PaletteEditorRangeRow* per = new PaletteEditorRangeRow(i,
                                                               m_dataRangeMode,
                                                               m_colorEditButtonGroup,
                                                               m_controlPointGridLayout,
                                                               this);
        QObject::connect(per, &PaletteEditorRangeRow::signalColorEditingRequested,
                         this, &PaletteEditorRangeWidget::signalEditColorRequested);
        QObject::connect(per, &PaletteEditorRangeRow::signalConstructionOperationRequested,
                         this, &PaletteEditorRangeWidget::performConstruction);
        QObject::connect(per, &PaletteEditorRangeRow::signalDataChanged,
                         this, &PaletteEditorRangeWidget::performRowDataChanged);
        
        m_rowWidgets.push_back(per);
    }
    
    resetScalarSpinBoxRanges();
    
    const int32_t numberOfRows(m_rowWidgets.size());
    for (int32_t iRow = 0; iRow < numberOfRows; iRow++) {
        bool showRowFlag(false);
        CaretAssertVectorIndex(m_rowWidgets, iRow);
        PaletteEditorRangeRow* rowWidget = m_rowWidgets[iRow];
        if (iRow < m_numberOfValidControlPoints) {
            CaretAssertVectorIndex(scalarColors, iRow);
            rowWidget->setScalar(scalarColors[iRow].scalar);
            
            int32_t red, green, blue;
            scalarColors[iRow].toRGB255(red, green, blue);
            rowWidget->setRgb(CaretRgb(red, green, blue));
            rowWidget->updateContent(m_numberOfValidControlPoints);
            
            showRowFlag = true;
        }
        
        rowWidget->setWidgetsVisible(showRowFlag);
    }
    
    updateScalarSpinBoxRanges();
}

/**
 * Select the first control point
 */
void
PaletteEditorRangeWidget::selectFirstControlPoint()
{
    if (m_numberOfValidControlPoints > 0) {
        CaretAssertVectorIndex(m_rowWidgets, 0);
        m_rowWidgets[0]->selectRadioButton();
        emit signalEditColorRequested(m_rowWidgets[0]->getRgb());
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
PaletteEditorRangeWidget::updateControlPointColor(const CaretRgb& rgb)
{
    /*
     * Each row will apply the RGB color only if
     * its edit radio button is checked (and they
     * are mutually exclusive)
     */
    for (auto row : m_rowWidgets) {
        row->updateRgbIfRadioButtonChecked(rgb);
    }
}

/**
 * @return The average of the two scalar/colors
 * @param sc1
 *    First scalar color
 * @param sc2
 *    Second scalar color
 */
PaletteNew::ScalarColor
PaletteEditorRangeWidget::averageScalarColor(const PaletteNew::ScalarColor& sc1,
                                             const PaletteNew::ScalarColor& sc2) const
{
    PaletteNew::ScalarColor scOut((sc1.scalar + sc2.scalar) / 2.0,
                                  (sc1.color[0] + sc2.color[0]) / 2.0,
                                  (sc1.color[1] + sc2.color[1]) / 2.0,
                                  (sc1.color[2] + sc2.color[2]) / 2.0);
    return scOut;
}

/**
 * Called when a control point modification is requested
 * @param rowIndex
 * Index of the row widget
 * @param constructionOperation
 * The modification operation
 */
void
PaletteEditorRangeWidget::performConstruction(const int32_t rowIndex,
                                    const PaletteEditorRangeWidget::ConstructionOperation constructionOperation)
{
    std::vector<PaletteNew::ScalarColor> scalarColors = getScalarColors();
    const auto numScalarColors = scalarColors.size();
    const int32_t lastControlPointIndex(m_numberOfValidControlPoints - 1);
    
    int32_t insertAtRowIndex(-1);
    switch (constructionOperation) {
        case PaletteEditorRangeWidget::ConstructionOperation::INSERT_CONTROL_POINT_ABOVE:
            if ((rowIndex > 0)
                && (rowIndex <= lastControlPointIndex)) {
                insertAtRowIndex = rowIndex - 1;
            }
            else {
                QString msg("Invalid row index="
                            + QString::number(rowIndex)
                            + " for inserting ABOVE row (0 at top) in range containing "
                            + AString::number(m_numberOfValidControlPoints)
                            + " rows.");
                CaretAssertMessage(0, msg);
                CaretLogSevere(msg);
            }
            break;
        case PaletteEditorRangeWidget::ConstructionOperation::INSERT_CONTROL_POINT_BELOW:
            if ((rowIndex >= 0)
                && (rowIndex < lastControlPointIndex)) {
                insertAtRowIndex = rowIndex;
            }
            else {
                QString msg("Invalid row index="
                            + QString::number(rowIndex)
                            + " for inserting BELOW row (0 at top) in range containing "
                            + AString::number(m_numberOfValidControlPoints)
                            + " rows.");
                CaretAssertMessage(0, msg);
                CaretLogSevere(msg);
            }
            break;
        case PaletteEditorRangeWidget::ConstructionOperation::REMOVE_CONTROL_POINT:
            /*
             * Not allowed to remove first or last rows
             */
            if ((rowIndex > 0)
                && (rowIndex < lastControlPointIndex)) {
                /*
                 * Row index 0 is at TOP in GUI
                 * But ScalarColor[0] is at BOTTOM in the GUI
                 */
                const int32_t removeAtIndex = (scalarColors.size() - 1 - rowIndex);
                scalarColors.erase(scalarColors.begin() + removeAtIndex);
            }
            else {
                QString msg("Invalid row index="
                            + QString::number(rowIndex)
                            + " for REMOVING row in range containing "
                            + AString::number(m_numberOfValidControlPoints)
                            + " rows.");
                CaretAssertMessage(0, msg);
                CaretLogSevere(msg);
            }
            break;
    }
    
    /*
     * Row index 0 is at TOP in GUI
     * But ScalarColor[0] is at BOTTOM in the GUI
     */
    if (insertAtRowIndex >= 0) {
        const int32_t insertAtIndex(scalarColors.size() - 1 - insertAtRowIndex);
        const int32_t nextIndex(insertAtIndex - 1);
        CaretAssertVectorIndex(scalarColors, insertAtIndex);
        CaretAssertVectorIndex(scalarColors, nextIndex);
        PaletteNew::ScalarColor avgSC = averageScalarColor(scalarColors[insertAtIndex],
                                                           scalarColors[nextIndex]);
        scalarColors.insert(scalarColors.begin() + insertAtIndex,
                            avgSC);
    }
    
    if (scalarColors.size() != numScalarColors) {
        updateContent(scalarColors);
    }
    
    signalDataChanged();
}

/**
 * Called when data in a row is changed
 */
void
PaletteEditorRangeWidget::performRowDataChanged()
{
    emit signalDataChanged();

    updateScalarSpinBoxRanges();
}

/**
 * Resets the ranges (minimum and maximum) of the scalar
 * value spin boxes to the the default for the "range mode".
 * This is needed since the user may switch palettes
 * and the ranges will be may be incorrect for the new
 * palette.
 */
void
PaletteEditorRangeWidget::resetScalarSpinBoxRanges()
{
    float rangeMin(0.0);
    float rangeMax(0.0);
    switch (m_dataRangeMode) {
        case DataRangeMode::NEGATIVE:
            rangeMin = -1.0;
            break;
        case DataRangeMode::POSITIVE:
            rangeMax = 1.0;
            break;
        case DataRangeMode::ZERO:
            break;
    }
    
    for (auto row : m_rowWidgets) {
        row->updateScalarValueRange(rangeMin,
                                    rangeMax);
    }
}

/**
 * The range (minimum and maximum values) for a
 * value spin box must be set to ensure that
 * the scalar values remain in an ascending order.
 */
void
PaletteEditorRangeWidget::updateScalarSpinBoxRanges()
{
    float rangeMin(0.0);
    float rangeMax(0.0);
    switch (m_dataRangeMode) {
        case DataRangeMode::NEGATIVE:
            rangeMin = -1.0;
            rangeMax =  0.0;
            break;
        case DataRangeMode::POSITIVE:
            rangeMin = 0.0;
            rangeMax = 1.0;
            break;
        case DataRangeMode::ZERO:
            break;
    }
    
    /*
     * Note: First row (index == 0) is at top and contains maximum scalar
     * Last row at bottom contains minimum scalar
     */
    for (int32_t iRow = 0; iRow < m_numberOfValidControlPoints; iRow++) {
        float minValue(0.0);
        float maxValue(0.0);
        if (iRow == 0) {
            minValue = rangeMax;
            maxValue = rangeMax;
        }
        else if (iRow == (m_numberOfValidControlPoints - 1)) {
            minValue = rangeMin;
            maxValue = rangeMin;
        }
        else {
            CaretAssertVectorIndex(m_rowWidgets, iRow - 1);
            maxValue = m_rowWidgets[iRow - 1]->getScalar();
            
            CaretAssertVectorIndex(m_rowWidgets, iRow + 1);
            minValue = m_rowWidgets[iRow + 1]->getScalar();
        }
        CaretAssertVectorIndex(m_rowWidgets, iRow);
        m_rowWidgets[iRow]->updateScalarValueRange(minValue, maxValue);
    }
}


