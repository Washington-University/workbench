
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <limits>

#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>

#define __PALETTE_COLOR_MAPPING_EDITOR_DIALOG_DECLARE__
#include "PaletteColorMappingEditorDialog.h"
#undef __PALETTE_COLOR_MAPPING_EDITOR_DIALOG_DECLARE__

#include "Brain.h"
#include "CaretMappableDataFile.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
#include "PaletteEnums.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class PaletteEditorDialog 
 * \brief Dialog for editing palettes.
 *
 * Presents controls for editing palettes used to color
 * scalar data.
 */

/**
 * Constructor for editing a palette selection.
 *
 * @param parent
 *    Parent widget on which this dialog is displayed.
 */
PaletteColorMappingEditorDialog::PaletteColorMappingEditorDialog(QWidget* parent)
: WuQDialogNonModal("Palette ColorMapping Editor",
                    parent)
{
    this->setDeleteWhenClosed(false);

    /*
     * No apply button
     */
    this->setApplyButtonText("");
    
    this->paletteColorMapping = NULL;
    
    /*
     * Palette Selection
     */
    this->paletteNameComboBox = new QComboBox();
    QObject::connect(this->paletteNameComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(apply()));
    QGroupBox* paletteSelectionGroupBox = new QGroupBox("Palette Selection");
    QVBoxLayout* paletteSelectionLayout = new QVBoxLayout(paletteSelectionGroupBox);
    paletteSelectionLayout->addWidget(this->paletteNameComboBox);
    
    /*
     * Color Mapping
     */
    this->scaleAutoRadioButton = new QRadioButton("Full"); //Auto Scale");
    this->scaleAutoPercentageRadioButton = new QRadioButton("Percentage"); //"Auto Scale Percentage");
    this->scaleFixedRadioButton = new QRadioButton("Fixed"); //"Fixed Scale");
    
    QButtonGroup* scaleButtonGroup = new QButtonGroup(this);
    scaleButtonGroup->addButton(this->scaleAutoRadioButton);
    scaleButtonGroup->addButton(this->scaleAutoPercentageRadioButton);
    scaleButtonGroup->addButton(this->scaleFixedRadioButton);
    QObject::connect(scaleButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(apply()));
    
    /*
     * Percentage mapping 
     */
    this->scaleAutoPercentageNegativeMaximumSpinBox = new QDoubleSpinBox();
    this->scaleAutoPercentageNegativeMaximumSpinBox->setMinimum(0);
    this->scaleAutoPercentageNegativeMaximumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleAutoPercentageNegativeMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentageNegativeMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleAutoPercentageNegativeMinimumSpinBox = new QDoubleSpinBox();
    this->scaleAutoPercentageNegativeMinimumSpinBox->setMinimum(0.0);
    this->scaleAutoPercentageNegativeMinimumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleAutoPercentageNegativeMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentageNegativeMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleAutoPercentagePositiveMinimumSpinBox = new QDoubleSpinBox();
    this->scaleAutoPercentagePositiveMinimumSpinBox->setMinimum(0.0);
    this->scaleAutoPercentagePositiveMinimumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleAutoPercentagePositiveMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentagePositiveMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleAutoPercentagePositiveMaximumSpinBox = new QDoubleSpinBox();
    this->scaleAutoPercentagePositiveMaximumSpinBox->setMinimum(0.0);
    this->scaleAutoPercentagePositiveMaximumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleAutoPercentagePositiveMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentagePositiveMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    /*
     * Fixed mapping
     */
    this->scaleFixedNegativeMaximumSpinBox = new QDoubleSpinBox();
    this->scaleFixedNegativeMaximumSpinBox->setMinimum(-std::numeric_limits<float>::max());
    this->scaleFixedNegativeMaximumSpinBox->setMaximum(0.0);
    this->scaleFixedNegativeMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleFixedNegativeMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleFixedNegativeMinimumSpinBox = new QDoubleSpinBox();
    this->scaleFixedNegativeMinimumSpinBox->setMinimum(-std::numeric_limits<float>::max());
    this->scaleFixedNegativeMinimumSpinBox->setMaximum(0.0);
    this->scaleFixedNegativeMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleFixedNegativeMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleFixedPositiveMinimumSpinBox = new QDoubleSpinBox();
    this->scaleFixedPositiveMinimumSpinBox->setMinimum(0.0);
    this->scaleFixedPositiveMinimumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleFixedPositiveMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleFixedPositiveMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleFixedPositiveMaximumSpinBox = new QDoubleSpinBox();
    this->scaleFixedPositiveMaximumSpinBox->setMinimum(0.0);
    this->scaleFixedPositiveMaximumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleFixedPositiveMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleFixedPositiveMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->interpolateColorsCheckBox = new QCheckBox("Interpolate Colors");
    QObject::connect(this->interpolateColorsCheckBox, SIGNAL(toggled(bool)), 
                     this, SLOT(apply()));
    QWidget* colorMapHorizLine = WuQtUtilities::createHorizontalLineWidget();
    
    QGroupBox* colorMappingGroupBox = new QGroupBox("Color Mapping");
    QGridLayout* colorMappingLayout = new QGridLayout(colorMappingGroupBox);
    colorMappingLayout->addWidget(this->scaleAutoRadioButton, 0, 0);
    colorMappingLayout->addWidget(this->scaleAutoPercentageRadioButton, 0, 1);
    colorMappingLayout->addWidget(this->scaleFixedRadioButton, 0, 2);
    colorMappingLayout->addWidget(new QLabel("Pos Max"), 1, 0, Qt::AlignRight);
    colorMappingLayout->addWidget(new QLabel("Pos Min"), 2, 0, Qt::AlignRight);
    colorMappingLayout->addWidget(new QLabel("Neg Min"), 3, 0, Qt::AlignRight);
    colorMappingLayout->addWidget(new QLabel("Neg Max"), 4, 0, Qt::AlignRight);
    colorMappingLayout->addWidget(this->scaleAutoPercentagePositiveMaximumSpinBox, 1, 1);
    colorMappingLayout->addWidget(this->scaleAutoPercentagePositiveMinimumSpinBox, 2, 1);
    colorMappingLayout->addWidget(this->scaleAutoPercentageNegativeMinimumSpinBox, 3, 1);
    colorMappingLayout->addWidget(this->scaleAutoPercentageNegativeMaximumSpinBox, 4, 1);
    colorMappingLayout->addWidget(this->scaleFixedPositiveMaximumSpinBox, 1, 2);
    colorMappingLayout->addWidget(this->scaleFixedPositiveMinimumSpinBox, 2, 2);
    colorMappingLayout->addWidget(this->scaleFixedNegativeMinimumSpinBox, 3, 2);
    colorMappingLayout->addWidget(this->scaleFixedNegativeMaximumSpinBox, 4, 2);
    colorMappingLayout->addWidget(colorMapHorizLine, 5, 0, 1, 3);
    colorMappingLayout->addWidget(this->interpolateColorsCheckBox, 6, 0, 1, 3);
    
    /*
     * Display Mode
     */
    this->displayModePositiveCheckBox = new QCheckBox("Positive");
    QObject::connect(this->displayModePositiveCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(apply()));
    this->displayModeZeroCheckBox = new QCheckBox("Zero");
    QObject::connect(this->displayModeZeroCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(apply()));
    this->displayModeNegativeCheckBox = new QCheckBox("Negative");
    QObject::connect(this->displayModeNegativeCheckBox , SIGNAL(toggled(bool)),
                     this, SLOT(apply()));
    QGroupBox* displayModeGroupBox = new QGroupBox("Display Mode");
    QVBoxLayout* displayModeLayout = new QVBoxLayout(displayModeGroupBox);
    displayModeLayout->addWidget(this->displayModePositiveCheckBox);
    displayModeLayout->addWidget(this->displayModeZeroCheckBox);
    displayModeLayout->addWidget(this->displayModeNegativeCheckBox);
    
    /*
     * Widget group used to block signals when updating.
     */
    this->widgetGroup = new WuQWidgetObjectGroup(this);
    this->widgetGroup->add(this->paletteNameComboBox);
    this->widgetGroup->add(scaleButtonGroup);
    this->widgetGroup->add(this->interpolateColorsCheckBox);
    this->widgetGroup->add(this->displayModePositiveCheckBox);
    this->widgetGroup->add(this->displayModeZeroCheckBox);
    this->widgetGroup->add(this->displayModeNegativeCheckBox);
    this->widgetGroup->add(this->scaleAutoPercentageNegativeMaximumSpinBox);
    this->widgetGroup->add(this->scaleAutoPercentageNegativeMinimumSpinBox);
    this->widgetGroup->add(this->scaleAutoPercentagePositiveMinimumSpinBox);
    this->widgetGroup->add(this->scaleAutoPercentagePositiveMaximumSpinBox);
    this->widgetGroup->add(this->scaleFixedNegativeMaximumSpinBox);
    this->widgetGroup->add(this->scaleFixedNegativeMinimumSpinBox);
    this->widgetGroup->add(this->scaleFixedPositiveMinimumSpinBox);
    this->widgetGroup->add(this->scaleFixedPositiveMaximumSpinBox);
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->addWidget(paletteSelectionGroupBox);
    layout->addWidget(colorMappingGroupBox);
    layout->addWidget(displayModeGroupBox);
    this->setCentralWidget(w);
}

/**
 * Destructor.
 */
PaletteColorMappingEditorDialog::~PaletteColorMappingEditorDialog()
{
    
}

/**
 * Update contents for editing a palette in a caret
 * mappable data file.
 *
 * @param caretMappableDataFile
 *    Data file containing palette that is edited.
 * @param mapIndex
 *    Index of map for palette that is edited.
 */
void 
PaletteColorMappingEditorDialog::updatePaletteEditor(CaretMappableDataFile* caretMappableDataFile,
                                         const int32_t mapIndex)
{
    const AString title =
    caretMappableDataFile->getFileNameNoPath()
    + ": "
    + caretMappableDataFile->getMapName(mapIndex);
    this->setWindowTitle(title);
    
    this->widgetGroup->blockSignals(true);
    
    this->paletteNameComboBox->clear();
    
    this->paletteColorMapping = caretMappableDataFile->getMapPaletteColorMapping(mapIndex); 
    
    if (this->paletteColorMapping != NULL) {
        PaletteFile* paletteFile = GuiManager::get()->getBrain()->getPaletteFile();
        
        int defaultIndex = 0;
        const int32_t numPalettes = paletteFile->getNumberOfPalettes();
        for (int32_t i = 0; i < numPalettes; i++) {
            Palette* palette = paletteFile->getPalette(i);
            const AString name = palette->getName();
            if (name == this->paletteColorMapping->getSelectedPaletteName()) {
                defaultIndex = i;
            }
            this->paletteNameComboBox->addItem(name,
                                               name);
        }
        
        if (defaultIndex < this->paletteNameComboBox->count()) {
            this->paletteNameComboBox->setCurrentIndex(defaultIndex);
        }
        
        switch (this->paletteColorMapping->getScaleMode()) {
            case PaletteScaleModeEnum::MODE_AUTO_SCALE:
                this->scaleAutoRadioButton->setChecked(true);
                break;
            case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
                this->scaleAutoPercentageRadioButton->setChecked(true);
                break;
            case PaletteScaleModeEnum::MODE_USER_SCALE:
                this->scaleFixedRadioButton->setChecked(true);
                break;
        }
        
        this->scaleAutoPercentageNegativeMaximumSpinBox->setValue(this->paletteColorMapping->getAutoScalePercentageNegativeMaximum());
        this->scaleAutoPercentageNegativeMinimumSpinBox->setValue(this->paletteColorMapping->getAutoScalePercentageNegativeMinimum());
        this->scaleAutoPercentagePositiveMinimumSpinBox->setValue(this->paletteColorMapping->getAutoScalePercentagePositiveMinimum());
        this->scaleAutoPercentagePositiveMaximumSpinBox->setValue(this->paletteColorMapping->getAutoScalePercentagePositiveMaximum());

        this->scaleFixedNegativeMaximumSpinBox->setValue(this->paletteColorMapping->getUserScaleNegativeMaximum());
        this->scaleFixedNegativeMinimumSpinBox->setValue(this->paletteColorMapping->getUserScaleNegativeMinimum());
        this->scaleFixedPositiveMinimumSpinBox->setValue(this->paletteColorMapping->getUserScalePositiveMinimum());
        this->scaleFixedPositiveMaximumSpinBox->setValue(this->paletteColorMapping->getUserScalePositiveMaximum());
        
        this->displayModePositiveCheckBox->setChecked(this->paletteColorMapping->isDisplayPositiveDataFlag());
        this->displayModeZeroCheckBox->setChecked(this->paletteColorMapping->isDisplayZeroDataFlag());
        this->displayModeNegativeCheckBox->setChecked(this->paletteColorMapping->isDisplayNegativeDataFlag());
    
        this->interpolateColorsCheckBox->setChecked(this->paletteColorMapping->isInterpolatePaletteFlag());
    }
    
    this->widgetGroup->blockSignals(false);
}

/**
 * Called when the apply button is pressed.
 */
void PaletteColorMappingEditorDialog::applyButtonPressed()
{
    const int itemIndex = this->paletteNameComboBox->currentIndex();
    if (itemIndex >= 0) {
        const AString name = this->paletteNameComboBox->itemData(itemIndex).toString();
        if (this->paletteColorMapping != NULL) {
            this->paletteColorMapping->setSelectedPaletteName(name);
        }
    }
    
    if (this->scaleAutoRadioButton->isChecked()) {
        this->paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE);
    }
    else if (this->scaleAutoPercentageRadioButton->isChecked()) {
        this->paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE);
    }
    else if (this->scaleFixedRadioButton->isChecked()) {
        this->paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_USER_SCALE);
    }
        
    this->paletteColorMapping->setUserScaleNegativeMaximum(this->scaleFixedNegativeMaximumSpinBox->value());
    this->paletteColorMapping->setUserScaleNegativeMinimum(this->scaleFixedNegativeMinimumSpinBox->value());
    this->paletteColorMapping->setUserScalePositiveMinimum(this->scaleFixedPositiveMinimumSpinBox->value());
    this->paletteColorMapping->setUserScalePositiveMaximum(this->scaleFixedPositiveMaximumSpinBox->value());

    this->paletteColorMapping->setAutoScalePercentageNegativeMaximum(this->scaleAutoPercentageNegativeMaximumSpinBox->value());
    this->paletteColorMapping->setAutoScalePercentageNegativeMinimum(this->scaleAutoPercentageNegativeMinimumSpinBox->value());
    this->paletteColorMapping->setAutoScalePercentagePositiveMinimum(this->scaleAutoPercentagePositiveMinimumSpinBox->value());
    this->paletteColorMapping->setAutoScalePercentagePositiveMaximum(this->scaleAutoPercentagePositiveMaximumSpinBox->value());
    
    this->paletteColorMapping->setDisplayPositiveDataFlag(this->displayModePositiveCheckBox->isChecked());
    this->paletteColorMapping->setDisplayNegativeDataFlag(this->displayModeNegativeCheckBox->isChecked());
    this->paletteColorMapping->setDisplayZeroDataFlag(this->displayModeZeroCheckBox->isChecked());
    
    this->paletteColorMapping->setInterpolatePaletteFlag(this->interpolateColorsCheckBox->isChecked());
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


