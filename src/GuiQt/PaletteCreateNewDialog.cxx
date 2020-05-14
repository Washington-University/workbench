
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

#define __PALETTE_CREATE_NEW_DIALOG_DECLARE__
#include "PaletteCreateNewDialog.h"
#undef __PALETTE_CREATE_NEW_DIALOG_DECLARE__

#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>

#include "Brain.h"
#include "CaretAssert.h"
#include "GuiManager.h"
#include "Palette.h"
#include "PaletteFile.h"
#include "PaletteNew.h"
#include "PalettePixmapPainter.h"
#include "PaletteSelectionWidget.h"
#include "WuQMessageBox.h"

using namespace caret;
    
/**
 * \class caret::PaletteCreateNewDialog 
 * \brief Dialog for creation of a new palette
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param pixmapMode
 * palette pixmap Interpolation mode (on/off)
 * @param parent
 *    The parent widget
 */
PaletteCreateNewDialog::PaletteCreateNewDialog(const PalettePixmapPainter::Mode pixmapMode,
                                               QWidget* parent)
: WuQDialogModal("Create New Palette",
                 parent),
m_pixmapMode(pixmapMode)
{
    m_copyPaletteRadioButton = new QRadioButton("Copy Palette");
    m_newPaletteRadioButton  = new QRadioButton("New Palette");
    
    m_paletteSelectionWidget = new PaletteSelectionWidget();
    QObject::connect(m_paletteSelectionWidget, &PaletteSelectionWidget::paletteSelectionChanged,
                     this, &PaletteCreateNewDialog::paletteSelected);
    
    QButtonGroup* buttonGroup = new QButtonGroup();
    buttonGroup->addButton(m_copyPaletteRadioButton);
    buttonGroup->addButton(m_newPaletteRadioButton);
    QObject::connect(buttonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
                     this, &PaletteCreateNewDialog::typeButtonClicked);
    
    QLabel* positiveLabel = new QLabel("Positive Control Points");
    m_newPalettePositiveSpinBox = new QSpinBox();
    m_newPalettePositiveSpinBox->setRange(2, 999);
    m_newPalettePositiveSpinBox->setSingleStep(1);
    m_newPalettePositiveSpinBox->setValue(5);
    
    QLabel* negativeLabel = new QLabel("Negative Control Points");
    m_newPaletteNegativeSpinBox = new QSpinBox();
    m_newPaletteNegativeSpinBox->setRange(2, 999);
    m_newPaletteNegativeSpinBox->setSingleStep(1);
    m_newPaletteNegativeSpinBox->setValue(5);

    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    gridLayout->setColumnStretch(3, 100);
    int row(0);
    gridLayout->addWidget(m_copyPaletteRadioButton,
                          row, 0, 1, 2);
    row++;
    gridLayout->addWidget(m_paletteSelectionWidget,
                          row, 1, 1, 2);
    row++;
    gridLayout->addWidget(m_newPaletteRadioButton,
                         row, 0, 1, 2);
    row++;
    gridLayout->addWidget(positiveLabel,
                          row, 1);
    gridLayout->addWidget(m_newPalettePositiveSpinBox,
                          row, 2);
    row++;
    gridLayout->addWidget(negativeLabel,
                          row, 1);
    gridLayout->addWidget(m_newPaletteNegativeSpinBox,
                          row, 2);

    setCentralWidget(widget, SCROLL_AREA_NEVER);
    
    m_newPaletteRadioButton->setChecked(true);
    typeButtonClicked(buttonGroup->checkedButton());
}

/**
 * Destructor.
 */
PaletteCreateNewDialog::~PaletteCreateNewDialog()
{
}

/**
 * Called when a  palette is selected
 * @param palette
 *     Palette selected by the user (may be NULL)
 */
void
PaletteCreateNewDialog::paletteSelected()
{
}

/**
 * Called when OK button clicked
 */
void
PaletteCreateNewDialog::okButtonClicked()
{
    if (m_copyPaletteRadioButton->isChecked()) {
        m_palette.reset();
        std::unique_ptr<PaletteNew> palette = m_paletteSelectionWidget->getSelectedPalette();
        if (palette) {
            m_palette.reset(new PaletteNew(*palette));
        }
        else {
            WuQMessageBox::errorOk(this,
                                   "No palette is selected for copying");
            return;
        }
    }
    else if (m_newPaletteRadioButton->isChecked()) {
        m_palette.reset(createPaletteNew("",
                                         m_newPalettePositiveSpinBox->value(),
                                         m_newPaletteNegativeSpinBox->value()));
    }
    else {
        WuQMessageBox::errorOk(this,
                               "Choose a palette mode");
        return;
    }

    WuQDialogModal::okButtonClicked();
}

/**
 * Called when a palette type button is clicked
 * @param button
 * Button that was clicked
 */
void
PaletteCreateNewDialog::typeButtonClicked(QAbstractButton* button)
{
    m_paletteSelectionWidget->setEnabled(m_copyPaletteRadioButton->isChecked());
    
    m_newPaletteNegativeSpinBox->setEnabled(m_newPaletteRadioButton->isChecked());
    m_newPalettePositiveSpinBox->setEnabled(m_newPaletteRadioButton->isChecked());

    if (button == m_copyPaletteRadioButton) {
        
    }
    else if (button == m_newPaletteRadioButton) {
        
    }
}

/**
 * @return The new palette or NULL if no palette
 */
std::unique_ptr<PaletteNew>
PaletteCreateNewDialog::getPalette()
{
    std::unique_ptr<PaletteNew> paletteOut;
    if (m_palette) {
        paletteOut.reset(new PaletteNew(*m_palette));
    }
    
    return paletteOut;
}

/**
 * @return A new palette containing the given number of positive and negative control points.  The
 *  positive region is shades or red, the negative region is shades for blue, and zero is green.
 *
 * @param name
 *    Name for the palette
 * @param numberOfPositiveControlPoints
 *    Number of positive points
 * @param numberOfNegativeControlPoints
 *    Number of negative points
 */
PaletteNew*
PaletteCreateNewDialog::createPaletteNew(const AString& name,
                                         const int32_t numberOfPositiveControlPoints,
                                         const int32_t numberOfNegativeControlPoints)
{
    const int32_t numPos(std::max(2, numberOfPositiveControlPoints));
    const int32_t numNeg(std::max(2, numberOfNegativeControlPoints));
    
    const float posStep(1.0 / numPos);
    
    const float redStep(1.0 / (numPos + 3));
    float redCompontent(redStep * 2.0);

    std::vector<PaletteNew::ScalarColor> posRange;
    float posScalar(0.0);
    for (int32_t i = 0; i < numPos; i++) {
        if (i == (numPos - 1)) {
            posScalar = 1.0f;
            redCompontent = 1.0f;
        }
        posRange.emplace_back(posScalar, redCompontent, 0.0f, 0.0f);
        
        posScalar     += posStep;
        redCompontent += redStep;
    }
    
    float negStep(1.0 / numNeg);
    
    float blueStep(1.0 / (numNeg + 3));
    float blueComponent(1.0);

    std::vector<PaletteNew::ScalarColor> negRange;
    float negScalar(-1.0);
    for (int32_t i = 0; i < numNeg; i++) {
        if (i == (numNeg - 1)) {
            negScalar = 0.0f;
        }
        negRange.emplace_back(negScalar, 0.0, 0.0f, blueComponent);
        
        blueComponent -= blueStep;
        negScalar     += negStep;
    }
    
    float zeroGreen[3] { 0.0, 1.0, 0.0 };
    PaletteNew* paletteNew = new PaletteNew(posRange,
                                            zeroGreen,
                                            negRange);
    paletteNew->setName(name);
    
    return paletteNew;
}
