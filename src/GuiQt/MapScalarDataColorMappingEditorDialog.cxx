
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
#include <QSlider>

#define __MAP_SCALAR_DATA_COLOR_MAPPING_EDITOR_DIALOG_DECLARE__
#include "MapScalarDataColorMappingEditorDialog.h"
#undef __MAP_SCALAR_DATA_COLOR_MAPPING_EDITOR_DIALOG_DECLARE__

#include "Brain.h"
#include "CaretMappableDataFile.h"
#include "DescriptiveStatistics.h"
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
 * \class MapSettingsScalarDataEditorDialog 
 * \brief Dialog for editing scalar data map settings
 *
 * Presents controls for setting palettes, and thresholding used to color
 * scalar data.
 */

/**
 * Constructor for editing a palette selection.
 *
 * @param parent
 *    Parent widget on which this dialog is displayed.
 */
MapScalarDataColorMappingEditorDialog::MapScalarDataColorMappingEditorDialog(QWidget* parent)
: WuQDialogNonModal("Scalar Data Color Mapping Editor",
                    parent)
{
    this->setDeleteWhenClosed(false);

    this->isUpdateInProgress = false;
    
    /*
     * No apply button
     */
    this->setApplyButtonText("");
    
    this->paletteColorMapping = NULL;
    
    QWidget* histogramWidget = this->createHistogramSection();
    QWidget* paletteWidget = this->createPaletteSection();
    QWidget* thresholdWidget = this->createThresholdSection();
    
    QWidget* w = new QWidget();
    QGridLayout* layout = new QGridLayout(w);
    layout->addWidget(thresholdWidget, 0, 0);
    layout->addWidget(histogramWidget, 0, 1);
    layout->addWidget(paletteWidget, 1, 0, 1, 2);
    
    this->setCentralWidget(w);
}

/**
 * Destructor.
 */
MapScalarDataColorMappingEditorDialog::~MapScalarDataColorMappingEditorDialog()
{
    
}

/**
 * May be called to update the dialog's content.
 */
void 
MapScalarDataColorMappingEditorDialog::updateDialog()
{
}

/**
 * Create the threshold section of the dialog.
 * @return
 *   The threshold section.
 */
QWidget* 
MapScalarDataColorMappingEditorDialog::createThresholdSection()
{
    this->thresholdTypeOffRadioButton = new QRadioButton("On");
    this->thresholdTypeOnRadioButton = new QRadioButton("Off");
    this->thresholdTypeMappedRadioButton = new QRadioButton("Mapped");
    this->thresholdTypeMappedAverageAreaRadioButton = new QRadioButton("Mapped Average Area");
    
    QButtonGroup* thresholdTypeButtonGroup = new QButtonGroup(this);
    QObject::connect(thresholdTypeButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(apply()));
    thresholdTypeButtonGroup->addButton(this->thresholdTypeOffRadioButton);
    thresholdTypeButtonGroup->addButton(this->thresholdTypeOnRadioButton);
    thresholdTypeButtonGroup->addButton(this->thresholdTypeMappedRadioButton);
    thresholdTypeButtonGroup->addButton(this->thresholdTypeMappedAverageAreaRadioButton);
    
    QGroupBox* thresholdTypeGroupBox = new QGroupBox("Threshold Type");
    QGridLayout* thresholdTypeLayout = new QGridLayout(thresholdTypeGroupBox);
    thresholdTypeLayout->addWidget(this->thresholdTypeOffRadioButton, 0, 0);
    thresholdTypeLayout->addWidget(this->thresholdTypeOnRadioButton, 1, 0);
    thresholdTypeLayout->addWidget(this->thresholdTypeMappedRadioButton, 0, 1);
    thresholdTypeLayout->addWidget(this->thresholdTypeMappedAverageAreaRadioButton, 1, 1);
        
    QLabel* thresholdLowLabel = new QLabel("Low");
    QLabel* thresholdHighLabel = new QLabel("High");
    
    this->thresholdLowSlider = new QSlider(Qt::Horizontal);
    QObject::connect(this->thresholdLowSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(apply()));
    this->thresholdHighSlider = new QSlider(Qt::Horizontal);
    QObject::connect(this->thresholdHighSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(apply()));
    
    const int spinBoxWidth = 100.0;
    this->thresholdLowSpinBox = new QDoubleSpinBox();
    this->thresholdLowSpinBox->setFixedWidth(spinBoxWidth);
    this->thresholdLowSpinBox->setSingleStep(1.0);
    QObject::connect(this->thresholdLowSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    this->thresholdHighSpinBox = new QDoubleSpinBox();
    this->thresholdHighSpinBox->setFixedWidth(spinBoxWidth);
    this->thresholdHighSpinBox->setSingleStep(1.0);
    QObject::connect(this->thresholdHighSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));

    this->thresholdShowInsideRadioButton = new QRadioButton("Show Data Inside Thresholds");
    this->thresholdShowOutsideRadioButton = new QRadioButton("Show Data Outside Thresholds");
    
    QButtonGroup* thresholdShowButtonGroup = new QButtonGroup(this);
    thresholdShowButtonGroup->addButton(this->thresholdShowInsideRadioButton);
    thresholdShowButtonGroup->addButton(this->thresholdShowOutsideRadioButton);
    QObject::connect(thresholdShowButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(apply()));
    
    QGroupBox* thresholdAdjustmentGroupBox = new QGroupBox("Threshold Adjustment");
    QGridLayout* thresholdAdjustmentLayout = new QGridLayout(thresholdAdjustmentGroupBox);
    thresholdAdjustmentLayout->setColumnStretch(0, 0);
    thresholdAdjustmentLayout->setColumnStretch(1, 100);
    thresholdAdjustmentLayout->setColumnStretch(2, 0);
    thresholdAdjustmentLayout->addWidget(thresholdLowLabel, 0, 0);
    thresholdAdjustmentLayout->addWidget(this->thresholdLowSlider, 0, 1);
    thresholdAdjustmentLayout->addWidget(this->thresholdLowSpinBox, 0, 2);
    thresholdAdjustmentLayout->addWidget(thresholdHighLabel, 1, 0);
    thresholdAdjustmentLayout->addWidget(this->thresholdHighSlider, 1, 1);
    thresholdAdjustmentLayout->addWidget(this->thresholdHighSpinBox, 1, 2);
    thresholdAdjustmentLayout->addWidget(this->thresholdShowInsideRadioButton, 2, 0, 1, 3, Qt::AlignLeft);
    thresholdAdjustmentLayout->addWidget(this->thresholdShowOutsideRadioButton, 3, 0, 1, 3, Qt::AlignLeft);
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->addWidget(thresholdTypeGroupBox);
    layout->addWidget(thresholdAdjustmentGroupBox);
    //QVBoxLayout* layout = new QVBoxLayout(w);
    //layout->addWidget(paletteSelectionGroupBox);
    //layout->addWidget(colorMappingGroupBox);
    //layout->addWidget(displayModeGroupBox);
    
    return w;
}

/**
 * Create the histogram section of the dialog.
 * @return
 *   The histogram section.
 */
QWidget* 
MapScalarDataColorMappingEditorDialog::createHistogramSection()
{
    QWidget* w = new QWidget();
    //QVBoxLayout* layout = new QVBoxLayout(w);
    //layout->addWidget(paletteSelectionGroupBox);
    //layout->addWidget(colorMappingGroupBox);
    //layout->addWidget(displayModeGroupBox);
    
    return w;
}

/**
 * Create the palette section of the dialog.
 * @return
 *   The palette section.
 */
QWidget* 
MapScalarDataColorMappingEditorDialog::createPaletteSection()
{
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
    this->scaleAutoPercentageRadioButton = new QRadioButton("Percent"); //"Auto Scale Percentage");
    this->scaleFixedRadioButton = new QRadioButton("Fixed"); //"Fixed Scale");
    
    QButtonGroup* scaleButtonGroup = new QButtonGroup(this);
    scaleButtonGroup->addButton(this->scaleAutoRadioButton);
    scaleButtonGroup->addButton(this->scaleAutoPercentageRadioButton);
    scaleButtonGroup->addButton(this->scaleFixedRadioButton);
    QObject::connect(scaleButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(apply()));
    
    /*
     * Spin box width
     */
    const int percentSpinBoxWidth = 75;
    const int fixedSpinBoxWidth   = 100;  // fixed may have much larger data values
    /*
     * Percentage mapping 
     */
    this->scaleAutoPercentageNegativeMaximumSpinBox = new QDoubleSpinBox();
    this->scaleAutoPercentageNegativeMaximumSpinBox->setFixedWidth(percentSpinBoxWidth);
    this->scaleAutoPercentageNegativeMaximumSpinBox->setMinimum(0);
    this->scaleAutoPercentageNegativeMaximumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleAutoPercentageNegativeMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentageNegativeMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleAutoPercentageNegativeMinimumSpinBox = new QDoubleSpinBox();
    this->scaleAutoPercentageNegativeMinimumSpinBox->setFixedWidth(percentSpinBoxWidth);
    this->scaleAutoPercentageNegativeMinimumSpinBox->setMinimum(0.0);
    this->scaleAutoPercentageNegativeMinimumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleAutoPercentageNegativeMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentageNegativeMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleAutoPercentagePositiveMinimumSpinBox = new QDoubleSpinBox();
    this->scaleAutoPercentagePositiveMinimumSpinBox->setFixedWidth(percentSpinBoxWidth);
    this->scaleAutoPercentagePositiveMinimumSpinBox->setMinimum(0.0);
    this->scaleAutoPercentagePositiveMinimumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleAutoPercentagePositiveMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentagePositiveMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleAutoPercentagePositiveMaximumSpinBox = new QDoubleSpinBox();
    this->scaleAutoPercentagePositiveMaximumSpinBox->setFixedWidth(percentSpinBoxWidth);
    this->scaleAutoPercentagePositiveMaximumSpinBox->setMinimum(0.0);
    this->scaleAutoPercentagePositiveMaximumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleAutoPercentagePositiveMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentagePositiveMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    /*
     * Fixed mapping
     */
    this->scaleFixedNegativeMaximumSpinBox = new QDoubleSpinBox();
    this->scaleFixedNegativeMaximumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    this->scaleFixedNegativeMaximumSpinBox->setMinimum(-std::numeric_limits<float>::max());
    this->scaleFixedNegativeMaximumSpinBox->setMaximum(0.0);
    this->scaleFixedNegativeMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleFixedNegativeMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleFixedNegativeMinimumSpinBox = new QDoubleSpinBox();
    this->scaleFixedNegativeMinimumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    this->scaleFixedNegativeMinimumSpinBox->setMinimum(-std::numeric_limits<float>::max());
    this->scaleFixedNegativeMinimumSpinBox->setMaximum(0.0);
    this->scaleFixedNegativeMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleFixedNegativeMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleFixedPositiveMinimumSpinBox = new QDoubleSpinBox();
    this->scaleFixedPositiveMinimumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    this->scaleFixedPositiveMinimumSpinBox->setMinimum(0.0);
    this->scaleFixedPositiveMinimumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleFixedPositiveMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleFixedPositiveMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleFixedPositiveMaximumSpinBox = new QDoubleSpinBox();
    this->scaleFixedPositiveMaximumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    this->scaleFixedPositiveMaximumSpinBox->setMinimum(0.0);
    this->scaleFixedPositiveMaximumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleFixedPositiveMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleFixedPositiveMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
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
     * Options
     */
    this->interpolateColorsCheckBox = new QCheckBox("Interpolate Colors");
    QObject::connect(this->interpolateColorsCheckBox, SIGNAL(toggled(bool)), 
                     this, SLOT(apply()));
    QGroupBox* optionsGroupBox = new QGroupBox("Options");
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroupBox);
    optionsLayout->addWidget(this->interpolateColorsCheckBox);
    
    //QWidget* colorMapHorizLine = WuQtUtilities::createHorizontalLineWidget();
    
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(optionsGroupBox);
    rightLayout->addWidget(displayModeGroupBox);
    rightLayout->addStretch();
    
    QWidget* w = new QWidget();
    QGridLayout* layout = new QGridLayout(w);
    layout->addWidget(paletteSelectionGroupBox, 0, 0, 1, 2);
    layout->addWidget(colorMappingGroupBox, 1, 0);
    layout->addLayout(rightLayout, 1, 1, Qt::AlignTop);

    return w;
}

/**
 * Update contents for editing a map settings for data in a caret
 * mappable data file.
 *
 * @param caretMappableDataFile
 *    Data file containing palette that is edited.
 * @param mapIndex
 *    Index of map for palette that is edited.
 */
void 
MapScalarDataColorMappingEditorDialog::updateEditor(CaretMappableDataFile* caretMappableDataFile,
                                         const int32_t mapIndex)
{
    this->isUpdateInProgress = true;
    
    const AString title =
    caretMappableDataFile->getFileNameNoPath()
    + ": "
    + caretMappableDataFile->getMapName(mapIndex);
    this->setWindowTitle(title);
    
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
        
        float lowValue = 0.0;
        float highValue = 0.0;
        switch (this->paletteColorMapping->getThresholdType()) {
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
                this->thresholdTypeOffRadioButton->setChecked(true);
                break;
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
                this->thresholdTypeOnRadioButton->setChecked(true);
                lowValue = this->paletteColorMapping->getThresholdNormalNegative();
                highValue = this->paletteColorMapping->getThresholdNormalPositive();
                break;
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
                this->thresholdTypeMappedRadioButton->setChecked(true);
                lowValue = this->paletteColorMapping->getThresholdMappedNegative();
                highValue = this->paletteColorMapping->getThresholdMappedPositive();
                break;
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
                this->thresholdTypeMappedAverageAreaRadioButton->setChecked(true);
                lowValue = this->paletteColorMapping->getThresholdMappedAverageAreaNegative();
                highValue = this->paletteColorMapping->getThresholdMappedAverageAreaPositive();
                break;
        }
        
        switch (this->paletteColorMapping->getThresholdTest()) {
            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_ABOVE:
                this->thresholdShowOutsideRadioButton->setChecked(true);
                break;
            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_BELOW:
                this->thresholdShowInsideRadioButton->setChecked(true);
                break;
        }
        
        const DescriptiveStatistics* statistics = caretMappableDataFile->getMapStatistics(mapIndex);
        const float minValue = statistics->getMostNegativeValue();
        const float maxValue = statistics->getMostPositiveValue();
        
        this->thresholdLowSlider->setMinimum(minValue);
        this->thresholdLowSlider->setMaximum(maxValue);
        this->thresholdLowSlider->setValue(lowValue);
        
        this->thresholdHighSlider->setMinimum(minValue);
        this->thresholdHighSlider->setMaximum(maxValue);
        this->thresholdHighSlider->setValue(highValue);
        
        this->thresholdLowSpinBox->setMinimum(minValue);
        this->thresholdLowSpinBox->setMaximum(maxValue);
        this->thresholdLowSpinBox->setValue(lowValue);
        
        this->thresholdHighSpinBox->setMinimum(minValue);
        this->thresholdHighSpinBox->setMaximum(maxValue);
        this->thresholdHighSpinBox->setValue(highValue);
    }
    
    this->isUpdateInProgress = false;
}

/**
 * Called when the apply button is pressed.
 */
void MapScalarDataColorMappingEditorDialog::applyButtonPressed()
{
    if (this->isUpdateInProgress) {
        return;
    }
    
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
    
    float lowValue = this->thresholdLowSpinBox->value();
    float highValue = this->thresholdHighSpinBox->value();
    
    if (this->thresholdTypeOffRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF);
    }
    else if (this->thresholdTypeOnRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL);
        this->paletteColorMapping->setThresholdNormalNegative(lowValue);
        this->paletteColorMapping->setThresholdNormalPositive(highValue);
    }
    else if (this->thresholdTypeMappedRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED);
        this->paletteColorMapping->setThresholdMappedNegative(lowValue);
        this->paletteColorMapping->setThresholdMappedPositive(highValue);
    }
    else if (this->thresholdTypeMappedAverageAreaRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA);
        this->paletteColorMapping->setThresholdMappedAverageAreaNegative(lowValue);
        this->paletteColorMapping->setThresholdMappedAverageAreaPositive(highValue);
    }
    
    if (this->thresholdShowInsideRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdTest(PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_BELOW);
    }
    else if (this->thresholdShowOutsideRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdTest(PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_ABOVE);
    }
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


