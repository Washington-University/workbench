
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
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>

#include "Brain.h"
#include "CaretAssert.h"
#include "EventPaletteNewOperation.h"
#include "EventPalettesGetOperations.h"
#include "GuiManager.h"
#include "Palette.h"
#include "PaletteNew.h"
#include "PalettePixmapPainter.h"
#include "PaletteSelectionWidget.h"
#include "WuQMessageBoxTwo.h"

using namespace caret;
    
/**
 * \class caret::PaletteCreateNewDialog 
 * \brief Dialog for creation of a new palette
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param paletteType
 *    Type of palette to create
 * @param pixmapMode
 * palette pixmap Interpolation mode (on/off)
 * @param parent
 *    The parent widget
 */
PaletteCreateNewDialog::PaletteCreateNewDialog(const PaletteType paletteType,
                                               const PalettePixmapPainter::Mode pixmapMode,
                                               QWidget* parent)
: WuQDialogModal("Create New Palette",
                 parent),
m_paletteType(paletteType),
m_pixmapMode(pixmapMode)
{
    QLabel* nameLabel(new QLabel("Name for New Palette"));
    m_newPaletteNameLineEdit = new QLineEdit();
    
    m_copyPaletteRadioButton = new QRadioButton("Copy Existing Palette");
    m_newPaletteRadioButton = new QRadioButton("Create New Palette");

    QButtonGroup* buttonGroup(new QButtonGroup(this));
    buttonGroup->addButton(m_copyPaletteRadioButton);
    buttonGroup->addButton(m_newPaletteRadioButton);
    QObject::connect(buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
                     this, &PaletteCreateNewDialog::radioButtonClicked);
    
    m_copyPaletteWidget = createCopyPaletteSection();
    m_newPaletteWidget  = createNewPaletteSection();
    
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    gridLayout->setColumnStretch(3, 100);
    int row(0);
    gridLayout->addWidget(nameLabel,
                          row, 0, 1, 2);
    gridLayout->addWidget(m_newPaletteNameLineEdit,
                          row, 2);
    row++;
    gridLayout->addWidget(m_copyPaletteRadioButton,
                          row, 0, 1, 3);
    row++;
    gridLayout->addWidget(m_copyPaletteWidget,
                          row, 1, 1, 2);
    row++;
    gridLayout->addWidget(m_newPaletteRadioButton,
                          row, 0, 1, 3);
    row++;
    gridLayout->addWidget(m_newPaletteWidget,
                          row, 1, 1, 2);
    row++;

    setCentralWidget(widget, SCROLL_AREA_NEVER);
    
    m_newPaletteRadioButton->setChecked(true);
    radioButtonClicked(m_newPaletteRadioButton);
}

/**
 * Destructor.
 */
PaletteCreateNewDialog::~PaletteCreateNewDialog()
{
}

/**
 * @return New instance of create palette section
 */
QWidget*
PaletteCreateNewDialog::createNewPaletteSection()
{
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
    gridLayout->addWidget(positiveLabel,
                          row, 1);
    gridLayout->addWidget(m_newPalettePositiveSpinBox,
                          row, 2);
    row++;
    gridLayout->addWidget(negativeLabel,
                          row, 1);
    gridLayout->addWidget(m_newPaletteNegativeSpinBox,
                          row, 2);
    row++;

    return widget;
}

/**
 * @return New instance of copy palette section
 */
QWidget*
PaletteCreateNewDialog::createCopyPaletteSection()
{
    std::vector<PaletteDesignTypeEnum::Enum> paletteDesignTypes;
    paletteDesignTypes.push_back(PaletteDesignTypeEnum::PALETTE_NEW);
    paletteDesignTypes.push_back(PaletteDesignTypeEnum::PALETTE);

    m_copyPaletteSelectionWidget = new PaletteSelectionWidget(PaletteSelectionWidget::WidgetType::COMBO_BOX,
                                                              paletteDesignTypes);

    return m_copyPaletteSelectionWidget;
}

void
PaletteCreateNewDialog::radioButtonClicked(QAbstractButton *button)
{
    m_copyPaletteWidget->setEnabled(button == m_copyPaletteRadioButton);
    m_newPaletteWidget->setEnabled(button == m_newPaletteRadioButton);
}

/**
 * Called when OK button clicked
 */
void
PaletteCreateNewDialog::okButtonClicked()
{
    const AString newPaletteName(m_newPaletteNameLineEdit->text().trimmed());
    if (newPaletteName.isEmpty()) {
        WuQMessageBoxTwo::criticalOk(this, "ERROR", "Name for new palette is empty");
        return;
    }
    
    const PaletteBase* existingPalette(EventPalettesGetOperations::getPaletteWithName(newPaletteName));
    if (existingPalette != NULL) {
        WuQMessageBoxTwo::criticalOk(this, "ERROR",
                                     ("Palette with name \"" + newPaletteName + "\" already exists.\n"
                                      "Duplicate palette names are not allowed."));
        return;
    }
    switch (m_paletteType) {
        case PaletteType::USER_CUSTOM_PALETTE:
        {
            if (m_newPaletteRadioButton->isChecked()) {
                FunctionResultValue<const PaletteNew*> result(EventPaletteNewOperation::createNewPalette(newPaletteName,
                                                                                                         m_newPalettePositiveSpinBox->value(),
                                                                                                         m_newPaletteNegativeSpinBox->value()));
                if (result.isOk()) {
                    m_palette = result.getValue();
                    CaretAssert(m_palette);
                }
                else {
                    m_errorMessage = result.getErrorMessage();
                }
            }
            else if (m_copyPaletteRadioButton->isChecked()) {
                const AString paletteNameToCopy(m_copyPaletteSelectionWidget->getSelectedPaletteName());
                if ( ! paletteNameToCopy.isEmpty()) {
                    const PaletteBase* paletteToCopy(EventPalettesGetOperations::getPaletteWithName(paletteNameToCopy));
                    if (paletteToCopy != NULL) {
                        FunctionResultValue<const PaletteNew*> result(
                        EventPaletteNewOperation::copyPalette(paletteToCopy,
                                                              newPaletteName));
                        if (result.isOk()) {
                            m_palette = result.getValue();
                        }
                        else {
                            WuQMessageBoxTwo::criticalOk(this, "ERROR", ("Failed to copy palette:\n"
                                                                         + result.getErrorMessage()));
                            return;
                        }
                    }
                    else {
                        WuQMessageBoxTwo::criticalOk(this, "ERROR",
                                                     ("Unable to find palette named \""
                                                      + paletteNameToCopy
                                                      + "\" for copying."));
                    }
                }
            }
            else {
                WuQMessageBoxTwo::criticalOk(this, "ERROR", ("Choose on of the modes:\n"
                                                             + m_copyPaletteRadioButton->text() + "\n"
                                                             + m_newPaletteRadioButton->text()));
                return;
            }
        }
            break;
    }

    WuQDialogModal::okButtonClicked();
}

/**
 * @return The new palette or NULL if failure to create palette.
 * Call getErrorMessage() to find out why palette creation failed.
 * Note: Palette returned has been added to user's custom palettes.
 */
const PaletteNew*
PaletteCreateNewDialog::getPalette() const
{
    if (m_palette != NULL) {
        return m_palette;
    }
    return NULL;
}

/*
 * @return Error message if creation of palette failed (getPalette() == NULL)
 */
AString
PaletteCreateNewDialog::getErrorMessage() const
{
    return m_errorMessage;
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
