
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
    QLabel* nameLabel = new QLabel("Name");
    m_nameLineEdit = new QLineEdit();
    
    m_copyTemplatePaletteRadioButton = new QRadioButton("Copy Template Palette");
    m_copyUserPaletteRadioButton     = new QRadioButton("Copy User Palette");
    m_copyFilePaletteRadioButton     = new QRadioButton("Copy File Palette");
    m_newPaletteRadioButton          = new QRadioButton("New Palette");
    QButtonGroup* buttonGroup = new QButtonGroup();
    buttonGroup->addButton(m_copyTemplatePaletteRadioButton);
    buttonGroup->addButton(m_copyUserPaletteRadioButton);
    buttonGroup->addButton(m_newPaletteRadioButton);
    QObject::connect(buttonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
                     this, &PaletteCreateNewDialog::typeButtonClicked);
    
    m_templatePalettesComboBox = new QComboBox();
    loadTemplatePalettes();
    
    m_userPalettesComboBox = new QComboBox();
    loadUserPalettes();
    
    QComboBox* fileComboBox = new QComboBox();
    fileComboBox->setEnabled(false);
    m_copyFilePaletteRadioButton->setEnabled(false);
    
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
    gridLayout->addWidget(nameLabel,
                          row, 0);
    gridLayout->addWidget(m_nameLineEdit,
                          row, 1, 1, 2);
    row++;
    gridLayout->addWidget(m_copyTemplatePaletteRadioButton,
                          row, 0, 1, 2);
    gridLayout->addWidget(m_templatePalettesComboBox,
                          row, 2, 1, 2);
    row++;
    gridLayout->addWidget(m_copyUserPaletteRadioButton,
                          row, 0, 1, 2);
    gridLayout->addWidget(m_userPalettesComboBox,
                          row, 2, 1, 2);
    row++;
    gridLayout->addWidget(m_copyFilePaletteRadioButton,
                          row, 0, 1, 2);
    gridLayout->addWidget(fileComboBox,
                          row, 2, 1, 2);
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
 * Called when OK button clicked
 */
void
PaletteCreateNewDialog::okButtonClicked()
{
    const AString name = m_nameLineEdit->text().trimmed();
    if (name.isEmpty()) {
        WuQMessageBox::errorOk(this, "Palette name is invalid");
        return;
    }
    
    if (m_copyTemplatePaletteRadioButton->isChecked()) {
        WuQMessageBox::errorOk(this, "Copy template not implemented");
    }
    else if (m_copyUserPaletteRadioButton->isChecked()) {
        WuQMessageBox::errorOk(this, "Copy user palette not implemented");
    }
    else if (m_copyFilePaletteRadioButton->isChecked()) {
        WuQMessageBox::errorOk(this, "Copy file palette not implemented");
    }
    else if (m_newPaletteRadioButton->isChecked()) {
        m_palette.reset(createPaletteNew(name,
                                         m_newPalettePositiveSpinBox->value(),
                                         m_newPaletteNegativeSpinBox->value()));
    }
    else {
        WuQMessageBox::errorOk(this, "Choose a palette type");
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
    m_userPalettesComboBox->setEnabled(m_copyUserPaletteRadioButton->isChecked());
    m_templatePalettesComboBox->setEnabled(m_copyTemplatePaletteRadioButton->isChecked());
    
    m_newPaletteNegativeSpinBox->setEnabled(m_newPaletteRadioButton->isChecked());
    m_newPalettePositiveSpinBox->setEnabled(m_newPaletteRadioButton->isChecked());

    if (button == m_copyTemplatePaletteRadioButton) {
        
    }
    else if (button == m_copyUserPaletteRadioButton) {
        
    }
    else if (button == m_newPaletteRadioButton) {
        
    }
}

/**
 * @return The new palette
 */
PaletteNew*
PaletteCreateNewDialog::getPalette()
{
    if (m_palette) {
        return m_palette.release();
    }
    
    return NULL;
}

/**
 * Called to load the combo box  containing the template (built-in) palettes
 */
void
PaletteCreateNewDialog::loadTemplatePalettes()
{
    QSize iconSize(80, 18);
    
    PaletteFile* paletteFile = GuiManager::get()->getBrain()->getPaletteFile();
    
    const int32_t numPalettes = paletteFile->getNumberOfPalettes();
    for (int32_t i = 0; i < numPalettes; i++) {
        Palette* palette = paletteFile->getPalette(i);
        const AString name = palette->getName();
        /*
         * Second parameter is user data.  In the future, there may be user-editable
         * palettes and it is possible there may be palettes with the same name.
         * Thus, the user-data may change to a unique-identifier that is different
         * than the palette name.
         */
        const AString paletteUniqueID(name);
        
        PalettePixmapPainter palettePainter(palette,
                                            iconSize,
                                            m_pixmapMode);
        QPixmap pixmap = palettePainter.getPixmap();
        if (pixmap.isNull()) {
            m_templatePalettesComboBox->addItem(name);
        }
        else {
            m_templatePalettesComboBox->addItem(pixmap, name);
        }
    }
    m_templatePalettesComboBox->setIconSize(iconSize);
}

/**
 * Called to load the combo box  containing the user's palettes from the user's preferences
 */
void
PaletteCreateNewDialog::loadUserPalettes()
{
    QSize iconSize(80, 18);
    PaletteFile* paletteFile = GuiManager::get()->getBrain()->getPaletteFile();
    
    const int32_t numPalettes = std::min(3, paletteFile->getNumberOfPalettes());
    for (int32_t i = 0; i < numPalettes; i++) {
        Palette* palette = paletteFile->getPalette(i);
        AString name = palette->getName();
        switch (i) {
            case 0:
                name = "Colors";
                break;
            case 1:
                name = "Rainbow";
                break;
            case 2:
                name = "Gray";
                break;
        }
        /*
         * Second parameter is user data.  In the future, there may be user-editable
         * palettes and it is possible there may be palettes with the same name.
         * Thus, the user-data may change to a unique-identifier that is different
         * than the palette name.
         */
        const AString paletteUniqueID(name);
        
        PalettePixmapPainter palettePainter(palette,
                                            iconSize,
                                            m_pixmapMode);
        QPixmap pixmap = palettePainter.getPixmap();
        if (pixmap.isNull()) {
            m_userPalettesComboBox->addItem(name);
        }
        else {
            m_userPalettesComboBox->addItem(pixmap, name);
        }
    }
    m_userPalettesComboBox->setIconSize(iconSize);
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
