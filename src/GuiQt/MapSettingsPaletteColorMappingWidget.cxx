
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QToolButton>

#define __MAP_SETTINGS_PALETTE_COLOR_MAPPING_WIDGET_DECLARE__
#include "MapSettingsPaletteColorMappingWidget.h"
#undef __MAP_SETTINGS_PALETTE_COLOR_MAPPING_WIDGET_DECLARE__

#include "Brain.h"
#include "CaretColorEnumComboBox.h"
#include "CaretMappableDataFile.h"
#include "CaretMappableDataFileAndMapSelectorObject.h"
#include "CopyPaletteColorMappingToFilesDialog.h"
#include "CursorDisplayScoped.h"
#include "EnumComboBoxTemplate.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "FastStatistics.h"
#include "GuiManager.h"
#include "Histogram.h"
#include "MapSettingsColorBarPaletteOptionsWidget.h"
#include "MathFunctions.h"
#include "NodeAndVoxelColoring.h"
#include "NumericTextFormatting.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
#include "PalettePixmapPainter.h"
#include "ThresholdingSetMapsDialog.h"
#include "VolumeFile.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQDoubleSlider.h"
#include "WuQDoubleSpinBox.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQMessageBoxTwo.h"
#include "WuQtUtilities.h"
#include "WuQwtPlot.h"

#include "qwt_plot_curve.h"
#include "qwt_plot_histogram.h"
#include "qwt_plot_intervalcurve.h"
#include "qwt_plot_layout.h"
#include "PlotMagnifier.h"
#include "PlotPanner.h"

using namespace caret;

/* The QSlider uses integer for min/max so use max-int / 4  (approximately) */
static const int32_t BIG_NUMBER = (std::numeric_limits<int32_t>::max() - 1);

    
/**
 * \class caret::MapSettingsScalarDataEditorDialog 
 * \brief Dialog for editing scalar data map settings
 * \ingroup GuiQt
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
MapSettingsPaletteColorMappingWidget::MapSettingsPaletteColorMappingWidget(QWidget* parent)
: QWidget(parent)
{
    /*
     * No context menu, it screws things up
     * but one is used on the histogram plot
     */
    this->setContextMenuPolicy(Qt::NoContextMenu);
    
    this->caretMappableDataFile = NULL;
    this->mapFileIndex = -1;
    
    this->paletteWidgetGroup = new WuQWidgetObjectGroup(this);
    this->thresholdWidgetGroup = new WuQWidgetObjectGroup(this);
    
    this->paletteColorMapping = NULL;
    
    QWidget* histogramWidget = this->createHistogramSection();
    QWidget* histogramControlWidget = this->createHistogramControlSection();
    
    m_paletteOptionsWidget = new MapSettingsColorBarPaletteOptionsWidget();
    
    QWidget* normalizationWidget = this->createNormalizationControlSection();
    
    QWidget* paletteWidget = this->createPaletteSection();

    QWidget* thresholdWidget = this->createThresholdSection();
    thresholdWidget->setMaximumWidth(paletteWidget->sizeHint().width() + 50);
    
    m_preColorAllMapsCheckBox = new QCheckBox("Pre-Color All Maps");
    m_preColorAllMapsCheckBox->setToolTip("Maps will be colored when this\n"
                                          "file is loaded by a scene");
    QObject::connect(m_preColorAllMapsCheckBox, &QCheckBox::clicked,
                     this, &MapSettingsPaletteColorMappingWidget::preColorAllMapsClicked);
    
    QWidget* leftWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    this->setLayoutSpacingAndMargins(leftLayout);
    leftLayout->addWidget(thresholdWidget);
    leftLayout->addWidget(paletteWidget);
    
    QVBoxLayout* dataLayout = new QVBoxLayout();
    dataLayout->addWidget(normalizationWidget);
    dataLayout->addWidget(m_paletteOptionsWidget);
    dataLayout->addWidget(m_preColorAllMapsCheckBox);
    
    QWidget* bottomRightWidget = new QWidget();
    QHBoxLayout* bottomRightLayout = new QHBoxLayout(bottomRightWidget);
    this->setLayoutSpacingAndMargins(bottomRightLayout);
    bottomRightLayout->addWidget(histogramControlWidget);
    bottomRightLayout->addLayout(dataLayout);
    bottomRightLayout->addStretch();
    bottomRightWidget->setFixedHeight(bottomRightWidget->sizeHint().height());
    
    QWidget* rightWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    this->setLayoutSpacingAndMargins(rightLayout);
    rightLayout->addWidget(histogramWidget, 100);
    rightLayout->addWidget(bottomRightWidget, 0);
    rightWidget->setFixedHeight(leftWidget->sizeHint().height());
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    this->setLayoutSpacingAndMargins(layout);
    layout->addWidget(leftWidget, 0);
    layout->addWidget(rightWidget, 100);
    
    updatePaletteNameComboBox();
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
MapSettingsPaletteColorMappingWidget::~MapSettingsPaletteColorMappingWidget()
{
}

/**
 * May be called to update the widget's content.
 */
void 
MapSettingsPaletteColorMappingWidget::updateWidget()
{
    this->updateEditorInternal(this->caretMappableDataFile, 
                       this->mapFileIndex);
}

/**
 * Called when the threshold type is changed.
 */
void 
MapSettingsPaletteColorMappingWidget::thresholdTypeChanged(int indx)
{
    PaletteThresholdTypeEnum::Enum paletteThresholdType = static_cast<PaletteThresholdTypeEnum::Enum>(this->thresholdTypeComboBox->itemData(indx).toInt());
    this->paletteColorMapping->setThresholdType(paletteThresholdType);
    
    this->updateEditorInternal(this->caretMappableDataFile,
                       this->mapFileIndex);
    
    this->applySelections();
}

/**
 * Called when set all maps is selected for thresholding
 */
void
MapSettingsPaletteColorMappingWidget::thresholdSetAllMapsToolButtonClicked()
{
    if (this->caretMappableDataFile->getNumberOfMaps() <= 1) {
        WuQMessageBox::errorOk(this,
                               "Data file contains one map (must have more than one map for this option)");
        return;
    }
    
    CaretMappableDataFile* threshFile = this->thresholdMapFileIndexSelector->getModel()->getSelectedFile();
    if (threshFile == NULL) {
        WuQMessageBox::errorOk(this,
                               "No thresholding file is selected");
        return;
    }
    
    const int32_t threshFileNumberOfMaps = threshFile->getNumberOfMaps();
    if (threshFileNumberOfMaps <= 0) {
        WuQMessageBox::errorOk(this,
                               "Thresholding file contains no data");
        return;
    }
    
    ThresholdingSetMapsDialog threshDialog(this->caretMappableDataFile,
                                           threshFile,
                                           (this->thresholdMapFileIndexSelector->getModel()->getSelectedMapIndex()),
                                           this);
    if (threshDialog.exec() == ThresholdingSetMapsDialog::Accepted) {
        this->updateEditorInternal(this->caretMappableDataFile,
                                   this->mapFileIndex);
        this->applySelections();
    }
}

/**
 * Update the minimum and maximum values for the thresholding controls.
 */
void
MapSettingsPaletteColorMappingWidget::updateThresholdControlsMinimumMaximumRangeValues()
{
    if (paletteColorMapping != NULL) {
        if (this->caretMappableDataFile != NULL) {
            if ((this->mapFileIndex >= 0)
                && (this->mapFileIndex < this->caretMappableDataFile->getNumberOfMaps())) {
                
                const PaletteThresholdRangeModeEnum::Enum thresholdRangeMode = paletteColorMapping->getThresholdRangeMode();
                this->thresholdRangeModeComboBox->setSelectedItem<PaletteThresholdRangeModeEnum, PaletteThresholdRangeModeEnum::Enum>(thresholdRangeMode);
                
                float maxValue = BIG_NUMBER;
                float minValue = -maxValue;
                float stepMax = maxValue;
                float stepMin = minValue;
                
                switch (thresholdRangeMode) {
                    case PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_FILE:
                        this->caretMappableDataFile->getDataRangeFromAllMaps(minValue,
                                                                             maxValue);
                        stepMin = minValue;
                        stepMax = maxValue;
                        break;
                    case PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_MAP:
                    {
                        
                        FastStatistics* statistics = NULL;
                        switch (this->caretMappableDataFile->getPaletteNormalizationMode()) {
                            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                                statistics = const_cast<FastStatistics*>(this->caretMappableDataFile->getFileFastStatistics());
                                break;
                            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                                statistics = const_cast<FastStatistics*>(this->caretMappableDataFile->getMapFastStatistics(this->mapFileIndex));
                                break;
                        }
                        
                        if (statistics != NULL) {
                            minValue = statistics->getMin();
                            maxValue = statistics->getMax();
                            stepMin = minValue;
                            stepMax = maxValue;
                        }
                    }
                        break;
                    case PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_UNLIMITED:
                    {
                        /*
                         * For unlimited range, use twice the maximum value in the file
                         * Using very large values can cause problems with some
                         * Qt widgets.
                         */
                        float allMapMinValue = 0.0;
                        float allMapMaxValue = 0.0;
                        this->caretMappableDataFile->getDataRangeFromAllMaps(allMapMinValue,
                                                                             allMapMaxValue);
                        if (allMapMaxValue > allMapMinValue) {
                            const float absMax = std::max(std::fabs(allMapMaxValue),
                                                          std::fabs(allMapMinValue));
                            minValue = -absMax * 2.0;
                            maxValue =  absMax * 2.0;
                        }
                        
                        /*
                         * Use file data for step size
                         * Use larger of double file value or a BIG number
                         */
                        stepMin = minValue;
                        stepMax = maxValue;
                        minValue = std::min(minValue, (float)-BIG_NUMBER);
                        maxValue = std::max(maxValue, (float)BIG_NUMBER);
                    }
                        break;
                }
                
                /*
                 * Set the spin box step value to one percent of 
                 * the data's range.
                 */
                float stepValue = 1.0;
                const float diff = stepMax - stepMin;
                if (diff > 0.0) {
                    stepValue = diff / 100.0;
                }
                
                if (thresholdRangeMode == PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_UNLIMITED) {
                    if (stepValue > 1.0) {
                        stepValue = 1.0;
                    }
                }
                float lowMin = minValue;
                float lowMax = maxValue;
                float highMin = minValue;
                float highMax = maxValue;
                
                if (this->paletteColorMapping->isThresholdNegMinPosMaxLinked()) {
                    const float absMax = std::max(std::fabs(minValue),
                                                  std::fabs(maxValue));
                    
                    lowMin = -absMax;
                    lowMax = 0.0;
                    highMin = 0.0;
                    highMax = absMax;
                }
                
                this->thresholdLowSlider->setRange(lowMin,
                                                   lowMax);
                this->thresholdHighSlider->setRange(highMin,
                                                    highMax);
                
                /*
                 * Since there are multiple ways for the user to adjust
                 * a threshold (slider or spin box) these controls must
                 * dispaly the same values.  In addition, linking the 
                 * thresholds requires updating the spin boxes and sliders
                 * for both low and high thresholding.
                 * 
                 * The spin box allows the user to hold down one of the 
                 * arrow keys to continuously update the data.  However,
                 * if any of the spin box's "set" methods are called
                 * while the user holds down the arrow, holding down of
                 * the arrow key will not work.  So, when the signal is
                 * emitted for the spin box value being changed, we need
                 * to avoid updating that spin box during that time.
                 */
                if (allowUpdateOfThresholdLowSpinBox) {
                    this->thresholdLowSpinBox->setRange(lowMin,
                                                        lowMax);
                    this->thresholdLowSpinBox->setSingleStep(stepValue);
                }

                if (allowUpdateOfThresholdHighSpinBox) {
                    this->thresholdHighSpinBox->setRange(highMin,
                                                         highMax);
                    this->thresholdHighSpinBox->setSingleStep(stepValue);
                }
            }
        }
    }
}


/**
 * Should be called when a control is changed and
 * the change requires and update to other controls.
 */
void 
MapSettingsPaletteColorMappingWidget::applyAndUpdate()
{
    this->applySelections();
    
    this->updateEditorInternal(this->caretMappableDataFile,
                              this->mapFileIndex);
}

/**
 * Update after a threshold value is changed.
 *
 * @param lowThreshold
 *    New value for low threshold.
 * @param highThreshold
 *    New value for high threshold.
 */
void
MapSettingsPaletteColorMappingWidget::updateAfterThresholdValuesChanged(const float lowThreshold,
                                                                        const float highThreshold)
{
    const PaletteThresholdTypeEnum::Enum threshType = this->paletteColorMapping->getThresholdType();
    this->paletteColorMapping->setThresholdMinimum(threshType, lowThreshold);
    this->paletteColorMapping->setThresholdMaximum(threshType, highThreshold);
    updateThresholdSection();
    this->applySelections();
}

/**
 * Called when low value spin box changed.
 * @param thresholdLow
 *    New value.
 */
void 
MapSettingsPaletteColorMappingWidget::thresholdLowSpinBoxValueChanged(double thresholdLow)
{
    const PaletteThresholdTypeEnum::Enum threshType = this->paletteColorMapping->getThresholdType();
    float thresholdHigh = this->paletteColorMapping->getThresholdMaximum(threshType);
    if (this->paletteColorMapping->isThresholdNegMinPosMaxLinked()) {
        thresholdHigh = -thresholdLow;
    }
    else {
        if (thresholdLow > thresholdHigh) {
            thresholdHigh = thresholdLow;
        }
    }
    
    allowUpdateOfThresholdLowSpinBox = false;
    updateAfterThresholdValuesChanged(thresholdLow,
                                      thresholdHigh);
    allowUpdateOfThresholdLowSpinBox = true;
}

/**
 * Called when high value spin box changed.
 * @param thresholdHigh
 *    New value.
 */
void 
MapSettingsPaletteColorMappingWidget::thresholdHighSpinBoxValueChanged(double thresholdHigh)
{
    const PaletteThresholdTypeEnum::Enum threshType = this->paletteColorMapping->getThresholdType();
    float thresholdLow = this->paletteColorMapping->getThresholdMinimum(threshType);
    if (this->paletteColorMapping->isThresholdNegMinPosMaxLinked()) {
        thresholdLow = -thresholdHigh;
    }
    else {
        if (thresholdHigh < thresholdLow) {
            thresholdLow = thresholdHigh;
        }
    }

    allowUpdateOfThresholdHighSpinBox = false;
    updateAfterThresholdValuesChanged(thresholdLow,
                                      thresholdHigh);    
    allowUpdateOfThresholdHighSpinBox = true;
}

/**
 * Called when low value slider changed.
 * @param thresholdLow
 *    New value.
 */
void
MapSettingsPaletteColorMappingWidget::thresholdLowSliderValueChanged(double thresholdLow)
{
    const PaletteThresholdTypeEnum::Enum threshType = this->paletteColorMapping->getThresholdType();
    float thresholdHigh = this->paletteColorMapping->getThresholdMaximum(threshType);
    if (this->paletteColorMapping->isThresholdNegMinPosMaxLinked()) {
        thresholdHigh = -thresholdLow;
    }
    else {
        if (thresholdLow > thresholdHigh) {
            thresholdHigh = thresholdLow;
        }
    }
    updateAfterThresholdValuesChanged(thresholdLow,
                                      thresholdHigh);
}

/**
 * Called when high value slider changed.
 * @param thresholdHigh
 *    New value.
 */
void
MapSettingsPaletteColorMappingWidget::thresholdHighSliderValueChanged(double thresholdHigh)
{
    const PaletteThresholdTypeEnum::Enum threshType = this->paletteColorMapping->getThresholdType();
    float thresholdLow = this->paletteColorMapping->getThresholdMinimum(threshType);
    if (this->paletteColorMapping->isThresholdNegMinPosMaxLinked()) {
        thresholdLow = -thresholdHigh;
    }
    else {
        if (thresholdHigh < thresholdLow) {
            thresholdLow = thresholdHigh;
        }
    }
    updateAfterThresholdValuesChanged(thresholdLow,
                                      thresholdHigh);
}

/**
 * Gets called when the threshold map file/index is changed.
 */
void
MapSettingsPaletteColorMappingWidget::thresholdMapFileIndexSelectorChanged()
{
    this->applySelections();
}

/**
 * Called when the threshold link check box is toggled.
 *
 * @param checked
 *    Checked status of the checkbox.
 */
void
MapSettingsPaletteColorMappingWidget::thresholdLinkCheckBoxToggled(bool checked)
{
    if (this->paletteColorMapping != NULL) {
        this->paletteColorMapping->setThresholdNegMinPosMaxLinked(checked);
        
        const PaletteThresholdTypeEnum::Enum threshType = this->paletteColorMapping->getThresholdType();
        float lowValue = this->paletteColorMapping->getThresholdMinimum(threshType);
        float highValue = this->paletteColorMapping->getThresholdMaximum(threshType);
        
        if (checked) {
            if (highValue > 0.0) {
                lowValue = -highValue;
            }
            else if (lowValue < 0.0) {
                highValue = -lowValue;
            }
            else {
                highValue =  1.0;
                lowValue  = -1.0;
            }
        }
        
        updateAfterThresholdValuesChanged(lowValue,
                                          highValue);
    }
}

/**
 * Create the threshold section of the dialog.
 * @return
 *   The threshold section.
 */
QWidget* 
MapSettingsPaletteColorMappingWidget::createThresholdSection()
{
    allowUpdateOfThresholdLowSpinBox  = true;
    allowUpdateOfThresholdHighSpinBox = true;
    
    /*
     * Threshold Outline
     */
    QLabel* thresholdOutlineLabel = new QLabel("Outline ");
    this->thresholdOutlineDrawingModeComboBox = new EnumComboBoxTemplate(this);
    this->thresholdOutlineDrawingModeComboBox->setup<PaletteThresholdOutlineDrawingModeEnum, PaletteThresholdOutlineDrawingModeEnum::Enum>();
    QObject::connect(this->thresholdOutlineDrawingModeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &MapSettingsPaletteColorMappingWidget::applySelections);
    QLabel* thresholdOutlineColorLabel = new QLabel(" Color ");
    this->thresholdOutlineDrawingColorComboBox = new CaretColorEnumComboBox(this);
    QObject::connect(this->thresholdOutlineDrawingColorComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(applySelections()));
    this->thresholdOutlineDrawingWidget = new QWidget();
    QHBoxLayout* thresholdOutlineLayout = new QHBoxLayout(this->thresholdOutlineDrawingWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(thresholdOutlineLayout, 2, 0);
    thresholdOutlineLayout->addWidget(thresholdOutlineLabel);
    thresholdOutlineLayout->addWidget(this->thresholdOutlineDrawingModeComboBox->getWidget());
    thresholdOutlineLayout->addWidget(thresholdOutlineColorLabel);
    thresholdOutlineLayout->addWidget(this->thresholdOutlineDrawingColorComboBox->getWidget());
    thresholdOutlineLayout->addStretch();
    
    /*
     * Threshold types on/off
     */
    QLabel* thresholdTypeLabel = new QLabel("Source");
    std::vector<PaletteThresholdTypeEnum::Enum> thresholdTypes;
    PaletteThresholdTypeEnum::getAllEnums(thresholdTypes);
    const int32_t numThresholdTypes = static_cast<int32_t>(thresholdTypes.size());
    
    this->thresholdTypeComboBox = new QComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdTypeComboBox, 
                                          "Select thresholding off/on");
    for (int32_t i = 0; i < numThresholdTypes; i++) {
        this->thresholdTypeComboBox->addItem(PaletteThresholdTypeEnum::toGuiName(thresholdTypes[i]));
        this->thresholdTypeComboBox->setItemData(i, static_cast<int>(thresholdTypes[i]));
    }
    QObject::connect(this->thresholdTypeComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(thresholdTypeChanged(int)));
    
    this->thresholdSetAllMapsToolButton = new QToolButton();
    this->thresholdSetAllMapsToolButton->setText("Set All Maps...");
    this->thresholdSetAllMapsToolButton->setToolTip("Setup thresholding for all maps in the data file");
    QObject::connect(this->thresholdSetAllMapsToolButton, &QToolButton::clicked,
                     this, &MapSettingsPaletteColorMappingWidget::thresholdSetAllMapsToolButtonClicked);
    
    QLabel* thresholdRangeLabel = new QLabel("Range");
    this->thresholdRangeModeComboBox = new EnumComboBoxTemplate(this);
    QObject::connect(this->thresholdRangeModeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(thresholdRangeModeChanged()));
    this->thresholdRangeModeComboBox->setup<PaletteThresholdRangeModeEnum, PaletteThresholdRangeModeEnum::Enum>();
    const AString rangeModeToolTip = ("Controls range of threshold controls\n"
                                      "   File: Range is from all values in file.\n"
                                      "   Map:  Range is from all values in selected map.\n"
                                      "   Unlimited: Range is +/- infinity.");
    this->thresholdRangeModeComboBox->getWidget()->setToolTip(WuQtUtilities::createWordWrappedToolTipText(rangeModeToolTip));
    
    /*
     * Threshold file and map selector
     */
    this->thresholdMapFileIndexSelector = new CaretMappableDataFileAndMapSelectorObject(CaretMappableDataFileAndMapSelectorObject::OPTION_SHOW_MAP_INDEX_SPIN_BOX,
                                                                                        this);
    QObject::connect(this->thresholdMapFileIndexSelector, &CaretMappableDataFileAndMapSelectorObject::selectionWasPerformed,
                     this, &MapSettingsPaletteColorMappingWidget::thresholdMapFileIndexSelectorChanged);
    QWidget* threshFileComboBox(0);
    QWidget* threshMapIndexSpinBox(0);
    QWidget* threshMapNameComboBox(0);
    this->thresholdMapFileIndexSelector->getWidgetsForAddingToLayout(threshFileComboBox,
                                                                     threshMapIndexSpinBox,
                                                                     threshMapNameComboBox);
    
    this->thresholdFileWidget = new QWidget();
    QGridLayout* threshFileLayout = new QGridLayout(this->thresholdFileWidget);
    QLabel* threshFileLabel = new QLabel("File");
    QLabel* threshMapLabel  = new QLabel("Map");
    threshFileLayout->setContentsMargins(0, 0, 0, 0);
    threshFileLayout->setSpacing(4);
    threshFileLayout->setColumnStretch(0, 0);
    threshFileLayout->setColumnStretch(1, 0);
    threshFileLayout->setColumnStretch(2, 100);
    threshFileLayout->addWidget(threshFileLabel, 0, 0);
    threshFileLayout->addWidget(threshFileComboBox, 0, 1, 1, 2);
    threshFileLayout->addWidget(threshMapLabel, 1, 0);
    threshFileLayout->addWidget(threshMapIndexSpinBox, 1, 1);
    threshFileLayout->addWidget(threshMapNameComboBox, 1, 2);

    /*
     * Linking of low/high thresholds
     */
    QPixmap chainLinkPixmap;
    const bool chainLinkPixmapValid = WuQtUtilities::loadPixmap(":/PaletteSettings/chain_link_icon.png",
                                                                chainLinkPixmap);
    
    
    const AString linkToolTipText("When linked, both low and high are the same\n"
                                  "ABSOLUTE value with low always being negative and\n"
                                  "high always being positive.\n"
                                  "   low range:  [- maximum-absolute value, 0]\n"
                                  "   high range: [0, + maximum-absolute-value]\n"
                                  "\n"
                                  "When NOT linked, the low and high range controls\n"
                                  "operate independently.\n"
                                  "   low and high range: [minimum-value, maximum-value]\n"
                                  "\n"
                                  "NOTE: When 'Link' is unchecked, the thresholds may \n"
                                  "change due to a difference in the allowable range of \n"
                                  "values while linked and unlinked.");

    this->thresholdLinkCheckBox = new QCheckBox("");
    QObject::connect(this->thresholdLinkCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(thresholdLinkCheckBoxToggled(bool)));
    this->thresholdLinkCheckBox->setToolTip(linkToolTipText);
    this->thresholdWidgetGroup->add(this->thresholdLinkCheckBox);
    
    QLabel* linkLabel = new QLabel();
    if (chainLinkPixmapValid) {
        linkLabel->setPixmap(chainLinkPixmap);
    }
    else {
        linkLabel->setText("Link");
    }
    linkLabel->setToolTip(linkToolTipText);
    
    QVBoxLayout* linkLayout = new QVBoxLayout();
    linkLayout->addWidget(this->thresholdLinkCheckBox);
    linkLayout->addWidget(linkLabel);
    
    /*
     * Sliders and Spin Boxes for adjustment
     */
    QLabel* thresholdLowLabel = new QLabel("Low");
    QLabel* thresholdHighLabel = new QLabel("High");
    const float thresholdMinimum = -BIG_NUMBER;
    const float thresholdMaximum =  BIG_NUMBER;
    
    this->thresholdLowSlider = new WuQDoubleSlider(Qt::Horizontal,
                                                   this);
    this->thresholdLowSlider->setRange(thresholdMinimum, thresholdMaximum);
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdLowSlider->getWidget(), 
                                          "Adjust the low threshold value");
    this->thresholdWidgetGroup->add(this->thresholdLowSlider);
    QObject::connect(this->thresholdLowSlider, SIGNAL(valueChanged(double)),
                     this, SLOT(thresholdLowSliderValueChanged(double)));
    this->thresholdHighSlider = new WuQDoubleSlider(Qt::Horizontal,
                                                    this);
    this->thresholdHighSlider->setRange(thresholdMinimum, thresholdMaximum);
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdHighSlider->getWidget(),
                                          "Adjust the high threshold value");
    this->thresholdWidgetGroup->add(this->thresholdHighSlider);
    QObject::connect(this->thresholdHighSlider, SIGNAL(valueChanged(double)),
                     this, SLOT(thresholdHighSliderValueChanged(double)));
    
    this->thresholdLowSpinBox =
       WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(thresholdMinimum,
                                                                   thresholdMaximum,
                                                                   1.0,
                                                                   6,
                                                                   this,
                                                                   SLOT(thresholdLowSpinBoxValueChanged(double)));
    this->thresholdLowSpinBox->setAccelerated(true);
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdLowSpinBox,
                                          "Adjust the low threshold value");
    this->thresholdWidgetGroup->add(this->thresholdLowSpinBox);

    this->thresholdHighSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(thresholdMinimum,
                                                                   thresholdMaximum,
                                                                   1.0,
                                                                   6,
                                                                   this,
                                                                   SLOT(thresholdHighSpinBoxValueChanged(double)));
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdHighSpinBox,
                                          "Adjust the high threshold value");
    this->thresholdWidgetGroup->add(this->thresholdHighSpinBox);

    this->thresholdShowInsideRadioButton = new QRadioButton("Show Data Inside Thresholds");
    WuQtUtilities::setWordWrappedToolTip(this->thresholdShowInsideRadioButton,
                                          "Displays data that is greater than or equal to "
                                          "the minimum threshold value and less than or "
                                          "equal to the maximum threshold value.");
    
    this->thresholdShowOutsideRadioButton = new QRadioButton("Show Data Outside Thresholds");
    WuQtUtilities::setWordWrappedToolTip(this->thresholdShowOutsideRadioButton, 
                                         "Displays data that is less than the minimum "
                                         "threshold value or greater than the maximum "
                                         "threshold value.");
    
    QButtonGroup* thresholdShowButtonGroup = new QButtonGroup(this);
    this->thresholdWidgetGroup->add(thresholdShowButtonGroup);
    thresholdShowButtonGroup->addButton(this->thresholdShowInsideRadioButton);
    thresholdShowButtonGroup->addButton(this->thresholdShowOutsideRadioButton);
#if QT_VERSION >= 0x060000
    QObject::connect(thresholdShowButtonGroup, &QButtonGroup::idClicked,
                     this, &MapSettingsPaletteColorMappingWidget::applyAndUpdate);
#else
    QObject::connect(thresholdShowButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(applyAndUpdate()));
#endif
    QHBoxLayout* rangeLayout = new QHBoxLayout();
    rangeLayout->addWidget(thresholdRangeLabel);
    rangeLayout->addWidget(this->thresholdRangeModeComboBox->getWidget());
    
    this->thresholdAdjustmentWidget = new QWidget();
    QGridLayout* thresholdAdjustmentLayout = new QGridLayout(thresholdAdjustmentWidget);
    this->setLayoutSpacingAndMargins(thresholdAdjustmentLayout);
    thresholdAdjustmentLayout->setColumnStretch(0, 0);
    thresholdAdjustmentLayout->setColumnStretch(1, 0);
    thresholdAdjustmentLayout->setColumnStretch(2, 100);
    thresholdAdjustmentLayout->setColumnStretch(3, 100);
    thresholdAdjustmentLayout->setColumnStretch(4, 0);
    thresholdAdjustmentLayout->addLayout(linkLayout, 0, 0, 2, 1, Qt::AlignCenter);
    thresholdAdjustmentLayout->addWidget(thresholdHighLabel, 0, 1);
    thresholdAdjustmentLayout->addWidget(this->thresholdHighSlider->getWidget(), 0, 2, 1, 2);
    thresholdAdjustmentLayout->addWidget(this->thresholdHighSpinBox, 0, 4);
    thresholdAdjustmentLayout->addWidget(thresholdLowLabel, 1, 1);
    thresholdAdjustmentLayout->addWidget(this->thresholdLowSlider->getWidget(), 1, 2, 1, 2);
    thresholdAdjustmentLayout->addWidget(this->thresholdLowSpinBox, 1, 4);
    thresholdAdjustmentLayout->addWidget(this->thresholdShowInsideRadioButton, 2, 0, 1, 3, Qt::AlignLeft);
    thresholdAdjustmentLayout->addWidget(this->thresholdShowOutsideRadioButton, 3, 0, 1, 3, Qt::AlignLeft);
    thresholdAdjustmentLayout->addLayout(rangeLayout, 2, 3, 2, 2, Qt::AlignRight);
    thresholdAdjustmentWidget->setFixedHeight(thresholdAdjustmentWidget->sizeHint().height());
    
    QWidget* topWidget = new QWidget();
    QHBoxLayout* topLayout = new QHBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addWidget(thresholdTypeLabel);
    topLayout->addWidget(this->thresholdTypeComboBox);
    topLayout->addWidget(this->thresholdSetAllMapsToolButton);
    topLayout->addStretch();
    
    QGroupBox* thresholdGroupBox = new QGroupBox("Threshold");
    QVBoxLayout* layout = new QVBoxLayout(thresholdGroupBox);
    this->setLayoutSpacingAndMargins(layout);
    layout->addWidget(topWidget, 0, Qt::AlignLeft);
    layout->addWidget(this->thresholdFileWidget);
    layout->addWidget(this->thresholdOutlineDrawingWidget);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addWidget(thresholdAdjustmentWidget);
    thresholdGroupBox->setFixedHeight(thresholdGroupBox->sizeHint().height());
    
    this->thresholdAdjustmentWidgetGroup = new WuQWidgetObjectGroup(this);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdLinkCheckBox);
    this->thresholdAdjustmentWidgetGroup->add(thresholdLowLabel);
    this->thresholdAdjustmentWidgetGroup->add(thresholdHighLabel);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdLowSlider->getWidget());
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdHighSlider->getWidget());
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdLowSpinBox);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdHighSpinBox);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdShowInsideRadioButton);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdShowOutsideRadioButton);
    this->thresholdAdjustmentWidgetGroup->add(thresholdRangeLabel);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdRangeModeComboBox->getWidget());
    this->thresholdAdjustmentWidgetGroup->add(threshFileComboBox);
    this->thresholdAdjustmentWidgetGroup->add(threshMapIndexSpinBox);
    this->thresholdAdjustmentWidgetGroup->add(threshMapNameComboBox);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdSetAllMapsToolButton);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdOutlineDrawingColorComboBox->getWidget());
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdOutlineDrawingModeComboBox->getWidget());
    
    return thresholdGroupBox;
}

/**
 * Called when threshold range mode is changed.
 */
void
MapSettingsPaletteColorMappingWidget::thresholdRangeModeChanged()
{
    const PaletteThresholdRangeModeEnum::Enum thresholdRange = this->thresholdRangeModeComboBox->getSelectedItem<PaletteThresholdRangeModeEnum, PaletteThresholdRangeModeEnum::Enum>();
    this->paletteColorMapping->setThresholdRangeMode(thresholdRange);    
    updateThresholdControlsMinimumMaximumRangeValues();
    applySelections();
}

/**
 * Create the statistics section
 * @return the statistics section widget.
 */
QWidget* 
MapSettingsPaletteColorMappingWidget::createHistogramControlSection()
{
    QLabel* showLabel = new QLabel("Show ");
    m_histogramBarsVisibleCheckBox = new QCheckBox("Bars");
    QObject::connect(m_histogramBarsVisibleCheckBox, &QCheckBox::clicked,
                     [=] { this->applyAndUpdate(); } );
                     
    m_histogramEnvelopeVisibleCheckBox = new QCheckBox("Envelope");
    QObject::connect(m_histogramEnvelopeVisibleCheckBox, &QCheckBox::clicked,
                     [=] { this->applyAndUpdate(); });
    
    QHBoxLayout* showLayout = new QHBoxLayout();
    showLayout->addWidget(m_histogramBarsVisibleCheckBox);
    showLayout->addStretch();
    showLayout->addWidget(m_histogramEnvelopeVisibleCheckBox);
    
    QLabel* horizRangeLabel = new QLabel("Range");
    m_histogramHorizontalRangeComboBox = new EnumComboBoxTemplate(this);
    QObject::connect(this->m_histogramHorizontalRangeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(applyAndUpdate()));
    this->m_histogramHorizontalRangeComboBox->setup<PaletteHistogramRangeModeEnum, PaletteHistogramRangeModeEnum::Enum>();
    this->m_histogramHorizontalRangeComboBox->getWidget()->setToolTip("Horizontal range of histogram");

    QIcon colorBarIcon;
    const bool colorBarIconValid = WuQtUtilities::loadIcon(":/LayersPanel/colorbar.png",
                                                           colorBarIcon);
    
    QLabel* barsColorLabel = new QLabel("Bars Color");
    m_histogramBarsColorComboBox = new CaretColorEnumComboBox(CaretColorEnumComboBox::CustomColorModeEnum::FIXED,
                                                              CaretColorEnumComboBox::NoneColorModeEnum::DISABLED,
                                                              this);
    m_histogramBarsColorComboBox->setCustomColorName("Palette");
    if (colorBarIconValid) {
        m_histogramBarsColorComboBox->setCustomColorIcon(colorBarIcon);
    }
    WuQtUtilities::setToolTipAndStatusTip(m_histogramBarsColorComboBox->getWidget(),
                                          "Set histogram bars coloring");
    QObject::connect(m_histogramBarsColorComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(applyAndUpdate()));
    
    QLabel* envelopeColorLabel = new QLabel("Envelope");
    m_histogramEnvelopeColorComboBox = new CaretColorEnumComboBox(CaretColorEnumComboBox::CustomColorModeEnum::FIXED,
                                                                  CaretColorEnumComboBox::NoneColorModeEnum::DISABLED,
                                                                  this);
    m_histogramEnvelopeColorComboBox->setCustomColorName("Palette");
    if (colorBarIconValid) {
        m_histogramEnvelopeColorComboBox->setCustomColorIcon(colorBarIcon);
    }
    WuQtUtilities::setToolTipAndStatusTip(m_histogramEnvelopeColorComboBox->getWidget(),
                                          "Set histogram envelope coloring");
    QObject::connect(m_histogramEnvelopeColorComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(applyAndUpdate()));
    
    QLabel* envelopeLineWidthLabel = new QLabel("Envelope Width");
    m_histogramEnvelopeLineWidthPercentageSpinBox = new WuQDoubleSpinBox(this);
    m_histogramEnvelopeLineWidthPercentageSpinBox->setRangePercentage(0.0, 100.0);
    QObject::connect(m_histogramEnvelopeLineWidthPercentageSpinBox,  static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     [=](double) { this->applyAndUpdate(); });
    
    const int32_t maxBuckets = 100000;
    QLabel* bucketsLabel = new QLabel("Buckets");
    m_histogramBucketsSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(1, maxBuckets, 1);
    QObject::connect(m_histogramBucketsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(applyAndUpdate()));
    
    QWidget* controlWidget = new QWidget();
    QGridLayout* controlLayout = new QGridLayout(controlWidget);
    this->setLayoutSpacingAndMargins(controlLayout);
    int rowIndex = 0;
    controlLayout->addWidget(showLabel, rowIndex, 0);
    controlLayout->addLayout(showLayout, rowIndex, 1);
    rowIndex++;
    controlLayout->addWidget(barsColorLabel, rowIndex, 0);
    controlLayout->addWidget(m_histogramBarsColorComboBox->getWidget(), rowIndex, 1);
    rowIndex++;
    controlLayout->addWidget(envelopeColorLabel, rowIndex, 0);
    controlLayout->addWidget(m_histogramEnvelopeColorComboBox->getWidget(), rowIndex, 1);
    rowIndex++;
    controlLayout->addWidget(envelopeLineWidthLabel, rowIndex, 0);
    controlLayout->addWidget(m_histogramEnvelopeLineWidthPercentageSpinBox->getWidget(), rowIndex, 1);
    rowIndex++;
    controlLayout->addWidget(horizRangeLabel, rowIndex, 0);
    controlLayout->addWidget(m_histogramHorizontalRangeComboBox->getWidget(), rowIndex, 1);
    rowIndex++;
    controlLayout->addWidget(bucketsLabel, rowIndex, 0);
    controlLayout->addWidget(m_histogramBucketsSpinBox, rowIndex, 1);
    rowIndex++;
    controlWidget->setFixedSize(controlWidget->sizeHint());
    
    /*
     * Statistics
     */
    const AString blankText("");
    this->statisticsMinimumValueLabel = new QLabel(blankText);
    this->statisticsMinimumValueLabel->setAlignment(Qt::AlignRight);
    this->statisticsMaximumValueLabel = new QLabel(blankText);
    this->statisticsMaximumValueLabel->setAlignment(Qt::AlignRight);
    this->statisticsMeanValueLabel = new QLabel(blankText);
    this->statisticsMeanValueLabel->setAlignment(Qt::AlignRight);
    this->statisticsStandardDeviationLabel = new QLabel(blankText);
    this->statisticsStandardDeviationLabel->setAlignment(Qt::AlignRight);
    
    QWidget* statisticsWidget = new QWidget();
    QGridLayout* statisticsLayout = new QGridLayout(statisticsWidget);
    statisticsLayout->setColumnStretch(0, 0);
    statisticsLayout->setColumnStretch(0, 100);
    statisticsLayout->setColumnMinimumWidth(1, 10);
    this->setLayoutSpacingAndMargins(statisticsLayout);
    statisticsLayout->addWidget(new QLabel("Mean"), 0, 0);
    statisticsLayout->addWidget(this->statisticsMeanValueLabel, 0, 1);
    statisticsLayout->addWidget(new QLabel("Std Dev"), 1, 0);
    statisticsLayout->addWidget(this->statisticsStandardDeviationLabel, 1, 1);
    statisticsLayout->addWidget(new QLabel("Max"), 2, 0);
    statisticsLayout->addWidget(this->statisticsMaximumValueLabel, 2, 1);
    statisticsLayout->addWidget(new QLabel("Min"), 3, 0);
    statisticsLayout->addWidget(this->statisticsMinimumValueLabel, 3, 1);
    statisticsLayout->setRowStretch(statisticsLayout->rowCount(), 100);
    
    
    QGroupBox* groupBox = new QGroupBox("Histogram");
    QHBoxLayout* layout = new QHBoxLayout(groupBox);
    this->setLayoutSpacingAndMargins(layout);
    layout->addWidget(controlWidget);
    layout->addWidget(WuQtUtilities::createVerticalLineWidget());
    layout->addWidget(statisticsWidget);
    groupBox->setSizePolicy(QSizePolicy::Minimum,
                            QSizePolicy::Fixed);
    return groupBox;
}

/**
 * Create the histogram section of the dialog.
 * @return
 *   The histogram section.
 */
QWidget* 
MapSettingsPaletteColorMappingWidget::createHistogramSection()
{
    this->thresholdPlot = new WuQwtPlot();
    QObject::connect(this->thresholdPlot, SIGNAL(contextMenuDisplay(QContextMenuEvent*,
                                                                    float,
                                                                    float)),
                     this, SLOT(contextMenuDisplayRequested(QContextMenuEvent*,
                                                            float,
                                                            float)));
    this->thresholdPlot->plotLayout()->setAlignCanvasToScales(true);
    
    /*
     * Allow zooming
     */
    PlotMagnifier* magnifier = new PlotMagnifier(qobject_cast<QwtPlotCanvas*>(this->thresholdPlot->canvas()));
    magnifier->setAxisEnabled(QwtPlot::yLeft, true);
    magnifier->setAxisEnabled(QwtPlot::yRight, true);
    
    /*
     * Allow panning
     */
    (void)new PlotPanner(qobject_cast<QwtPlotCanvas*>(this->thresholdPlot->canvas()));
    
    /*
     * Auto scaling
     */
    this->thresholdPlot->setAxisAutoScale(QwtPlot::xBottom);
    this->thresholdPlot->setAxisAutoScale(QwtPlot::yLeft);

    /*
     * Reset View tool button
     */
    QAction* resetViewAction = WuQtUtilities::createAction("Reset View", 
                                                           "Remove any zooming/panning from histogram chart", 
                                                           this, 
                                                           this, 
                                                           SLOT(histogramResetViewButtonClicked()));

    QToolButton* resetViewToolButton = new QToolButton();
    resetViewToolButton->setDefaultAction(resetViewAction);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(this->thresholdPlot);
    layout->addWidget(resetViewToolButton, 0, Qt::AlignHCenter);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    return  widget;
}

/**
 * Called when the context menu is to be displayed.
 *
 * @param event
 *    The context menu event.
 * @param graphX
 *    X-coordinate on plot.
 * @param graphY
 *    Y-coordinate on plot.
 */
void
MapSettingsPaletteColorMappingWidget::contextMenuDisplayRequested(QContextMenuEvent* event,
                                                                  float graphX,
                                                                  float /*graphY*/)
{
    if (this->paletteColorMapping != NULL) {
        const PaletteThresholdTypeEnum::Enum threshType = this->paletteColorMapping->getThresholdType();
        if (threshType != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF) {
            const float absValue = std::fabs(graphX);
            AString textValue = NumericTextFormatting::formatValue(absValue);
            CursorDisplayScoped cursor;
            cursor.showCursor(Qt::ArrowCursor);
            QMenu menu(this);
            
            QAction* linkedAction = NULL;
            QAction* minThreshAction = NULL;
            QAction* maxThreshAction = NULL;
            if (this->paletteColorMapping->isThresholdNegMinPosMaxLinked()) {
                linkedAction = menu.addAction("Set Thresholds to -"
                                              + textValue
                                              + " and " + textValue);
            }
            else {
                minThreshAction = menu.addAction("Set Minimum Threshold to " + textValue);
                maxThreshAction = menu.addAction("Set Maximum Threshold to " + textValue);
            }
            
            QAction* selectedAction = menu.exec(event->globalPos());
            if (selectedAction != NULL) {
                float minThresh = this->paletteColorMapping->getThresholdMinimum(threshType);
                float maxThresh = this->paletteColorMapping->getThresholdMaximum(threshType);
                
                if (selectedAction == linkedAction) {
                    minThresh = -absValue;
                    maxThresh =  absValue;
                }
                else if (selectedAction == minThreshAction) {
                    minThresh = graphX;
                }
                else if (selectedAction == maxThreshAction) {
                    maxThresh = graphX;
                }
                
                updateAfterThresholdValuesChanged(minThresh,
                                                  maxThresh);
            }
        }
    }
}

/**
 * Called to reset the view of the histogram chart.
 */
void 
MapSettingsPaletteColorMappingWidget::histogramResetViewButtonClicked()
{
    this->thresholdPlot->setAxisAutoScale(QwtPlot::xBottom,true);
    this->thresholdPlot->setAxisAutoScale(QwtPlot::yLeft,true);
    this->updateHistogramPlot();
    this->thresholdPlot->setAxisAutoScale(QwtPlot::xBottom,false);
    this->thresholdPlot->setAxisAutoScale(QwtPlot::yLeft,false);
}

/**
 * Called when the auto percentage negative maximum value changes.
 *
 * @param value
 *     The new value.
 */
void
MapSettingsPaletteColorMappingWidget::scaleAutoPercentageNegativeMaximumValueChanged(double value)
{
    /*
     * Ensure maximum >= minimum
     */
    if (value < this->scaleAutoPercentageNegativeMinimumSpinBox->value()) {
        this->scaleAutoPercentageNegativeMinimumSpinBox->blockSignals(true);
        this->scaleAutoPercentageNegativeMinimumSpinBox->setValue(value);
        this->scaleAutoPercentageNegativeMinimumSpinBox->blockSignals(false);
    }
    applySelections();
    updatePaletteMappedToDataValueLabels();
}

/**
 * Called when the auto percentage negative minimum value changes.
 *
 * @param value
 *     The new value.
 */
void
MapSettingsPaletteColorMappingWidget::scaleAutoPercentageNegativeMinimumValueChanged(double value)
{
    /*
     * Ensure maximum >= minimum
     */
    if (value > this->scaleAutoPercentageNegativeMaximumSpinBox->value()) {
        this->scaleAutoPercentageNegativeMaximumSpinBox->blockSignals(true);
        this->scaleAutoPercentageNegativeMaximumSpinBox->setValue(value);
        this->scaleAutoPercentageNegativeMaximumSpinBox->blockSignals(false);
    }
    applySelections();
    updatePaletteMappedToDataValueLabels();
}


/**
 * Called when the auto percentage positive minimum value changes.
 *
 * @param value
 *     The new value.
 */
void
MapSettingsPaletteColorMappingWidget::scaleAutoPercentagePositiveMinimumValueChanged(double value)
{
    /*
     * Ensure maximum >= minimum
     */
    if (value > this->scaleAutoPercentagePositiveMaximumSpinBox->value()) {
        this->scaleAutoPercentagePositiveMaximumSpinBox->blockSignals(true);
        this->scaleAutoPercentagePositiveMaximumSpinBox->setValue(value);
        this->scaleAutoPercentagePositiveMaximumSpinBox->blockSignals(false);
    }
    applySelections();
    updatePaletteMappedToDataValueLabels();
}


/**
 * Called when the auto percentage maximum value changes.
 *
 * @param value
 *     The new value.
 */
void
MapSettingsPaletteColorMappingWidget::scaleAutoPercentagePositiveMaximumValueChanged(double value)
{
    /*
     * Ensure maximum >= minimum
     */
    if (value < this->scaleAutoPercentagePositiveMinimumSpinBox->value()) {
        this->scaleAutoPercentagePositiveMinimumSpinBox->blockSignals(true);
        this->scaleAutoPercentagePositiveMinimumSpinBox->setValue(value);
        this->scaleAutoPercentagePositiveMinimumSpinBox->blockSignals(false);
    }
    applySelections();
    updatePaletteMappedToDataValueLabels();
}

/**
 * Called when the auto absolute percentage minimum value changes.
 *
 * @param value
 *     The new value.
 */
void
MapSettingsPaletteColorMappingWidget::scaleAutoAbsolutePercentageMinimumValueChanged(double value)
{
    /*
     * Ensure maximum >= minimum
     */
    if (value > this->scaleAutoAbsolutePercentageMaximumSpinBox->value()) {
        this->scaleAutoAbsolutePercentageMaximumSpinBox->blockSignals(true);
        this->scaleAutoAbsolutePercentageMaximumSpinBox->setValue(value);
        this->scaleAutoAbsolutePercentageMaximumSpinBox->blockSignals(false);
    }
    applySelections();
    updatePaletteMappedToDataValueLabels();
}

/**
 * Called when the auto absolute percentage maximum value changes.
 *
 * @param value
 *     The new value.
 */
void
MapSettingsPaletteColorMappingWidget::scaleAutoAbsolutePercentageMaximumValueChanged(double value)
{
    /*
     * Ensure maximum >= minimum
     */
    if (value < this->scaleAutoAbsolutePercentageMinimumSpinBox->value()) {
        this->scaleAutoAbsolutePercentageMinimumSpinBox->blockSignals(true);
        this->scaleAutoAbsolutePercentageMinimumSpinBox->setValue(value);
        this->scaleAutoAbsolutePercentageMinimumSpinBox->blockSignals(false);
    }
    applySelections();
    updatePaletteMappedToDataValueLabels();
}

/**
 * Called when the fixed negative maximum value changes.
 *
 * @param value
 *     The new value.
 */
void
MapSettingsPaletteColorMappingWidget::scaleFixedNegativeMaximumValueChanged(double value)
{
    /*
     * Ensure maximum >= minimum
     */
    if (value > this->scaleFixedNegativeMinimumSpinBox->value()) {
        this->scaleFixedNegativeMinimumSpinBox->blockSignals(true);
        this->scaleFixedNegativeMinimumSpinBox->setValue(value);
        this->scaleFixedNegativeMinimumSpinBox->blockSignals(false);
    }
    applySelections();
    updatePaletteMappedToDataValueLabels();
}

/**
 * Called when the fixed negative minimum value changes.
 *
 * @param value
 *     The new value.
 */
void
MapSettingsPaletteColorMappingWidget::scaleFixedNegativeMinimumValueChanged(double value)
{
    /*
     * Ensure maximum >= minimum
     */
    if (value < this->scaleFixedNegativeMaximumSpinBox->value()) {
        this->scaleFixedNegativeMaximumSpinBox->blockSignals(true);
        this->scaleFixedNegativeMaximumSpinBox->setValue(value);
        this->scaleFixedNegativeMaximumSpinBox->blockSignals(false);
    }
    applySelections();
    updatePaletteMappedToDataValueLabels();
}

/**
 * Called when the fixed positive minimum value changes.
 *
 * @param value
 *     The new value.
 */
void
MapSettingsPaletteColorMappingWidget::scaleFixedPositiveMinimumValueChanged(double value)
{
    /*
     * Ensure maximum >= minimum
     */
    if (value > this->scaleFixedPositiveMaximumSpinBox->value()) {
        this->scaleFixedPositiveMaximumSpinBox->blockSignals(true);
        this->scaleFixedPositiveMaximumSpinBox->setValue(value);
        this->scaleFixedPositiveMaximumSpinBox->blockSignals(false);
    }
    applySelections();
    updatePaletteMappedToDataValueLabels();
}

/**
 * Called when the fixed positive maximum value changes.
 *
 * @param value
 *     The new value.
 */
void
MapSettingsPaletteColorMappingWidget::scaleFixedPositiveMaximumValueChanged(double value)
{
    /*
     * Ensure maximum >= minimum
     */
    if (value < this->scaleFixedPositiveMinimumSpinBox->value()) {
        this->scaleFixedPositiveMinimumSpinBox->blockSignals(true);
        this->scaleFixedPositiveMinimumSpinBox->setValue(value);
        this->scaleFixedPositiveMinimumSpinBox->blockSignals(false);
    }
    applySelections();
    updatePaletteMappedToDataValueLabels();
}

/**
 * Create the palette section of the dialog.
 * @return
 *   The palette section.
 */
QWidget*
MapSettingsPaletteColorMappingWidget::createPaletteSection()
{
    /*
     * Selection
     */
    QLabel* paletteNameLabel = new QLabel("Name");
    this->paletteNameComboBox = new QComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->paletteNameComboBox, 
                                          "Select palette for coloring map data");
    this->paletteWidgetGroup->add(this->paletteNameComboBox);
    QObject::connect(this->paletteNameComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(applySelections()));
    /*
     * Interpolate Colors
     */
    this->interpolateColorsCheckBox = new QCheckBox("Interpolate\nColors");
    WuQtUtilities::setToolTipAndStatusTip(this->interpolateColorsCheckBox, 
                                          "Smooth colors for data between palette colors");
    this->paletteWidgetGroup->add(this->interpolateColorsCheckBox);
    QObject::connect(this->interpolateColorsCheckBox, SIGNAL(toggled(bool)), 
                     this, SLOT(applySelections()));
    
    /*
     * Invert palette checkbox
     */
    QLabel* invertLabel = new QLabel("Invert");
    this->invertPaletteModeComboBox = new EnumComboBoxTemplate(this);
    QObject::connect(this->invertPaletteModeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(applySelections()));
    this->invertPaletteModeComboBox->setup<PaletteInvertModeEnum, PaletteInvertModeEnum::Enum>();
    const AString invertModeToolTip = ("<html>"
                                         "Controls inversion of Palette:<p>"
                                         "   " + PaletteInvertModeEnum::toGuiName(PaletteInvertModeEnum::POSITIVE_WITH_NEGATIVE)
                                       + " - Positive and negative sections of the palette are swapped<p>"
                                       "   " + PaletteInvertModeEnum::toGuiName(PaletteInvertModeEnum::POSITIVE_NEGATIVE_SEPARATE)
                                       + " - Swaps the negative range within the negative range AND "
                                         "     swaps the positive range within the positive range"
                                         "</html>");
    this->invertPaletteModeComboBox->getWidget()->setToolTip(invertModeToolTip);
    this->paletteWidgetGroup->add(this->invertPaletteModeComboBox->getComboBox());
    
    QWidget* paletteSelectionWidget = new QWidget();
    QGridLayout* paletteSelectionLayout = new QGridLayout(paletteSelectionWidget);
    paletteSelectionLayout->setColumnStretch(0,   0);
    paletteSelectionLayout->setColumnStretch(1,   0);
    paletteSelectionLayout->setColumnStretch(2, 100);
    this->setLayoutSpacingAndMargins(paletteSelectionLayout);
    paletteSelectionLayout->addWidget(paletteNameLabel, 0, 0);
    paletteSelectionLayout->addWidget(this->paletteNameComboBox, 0, 1);
    paletteSelectionLayout->addWidget(this->interpolateColorsCheckBox, 0, 2, 2, 1, Qt::AlignCenter);
    paletteSelectionLayout->addWidget(invertLabel, 1, 0);
    paletteSelectionLayout->addWidget(this->invertPaletteModeComboBox->getComboBox(), 1, 1);
    
    paletteSelectionWidget->setFixedHeight(paletteSelectionWidget->sizeHint().height());
    
    
    /*
     * Color Mapping
     */
    this->scaleAutoRadioButton = new QRadioButton("Full");
    this->scaleAutoAbsolutePercentageRadioButton = new QRadioButton("Abs Pct");
    this->scaleAutoPercentageRadioButton = new QRadioButton("Percent");
    this->scaleFixedRadioButton = new QRadioButton("Fixed");
    
    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoRadioButton, 
                                          "Map (most negative, zero, most positive) data values to (-1, 0, 0, 1) in palette");
    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoAbsolutePercentageRadioButton,
                                          "Map (most absolute percentiles (NOT percentages) data values to (-1, 0, 0, 1) in palette");
    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentageRadioButton,
                                          "Map percentiles (NOT percentages) of (most neg, least neg, least pos, most pos) data values to (-1, 0, 0, 1) in palette");
    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedRadioButton, 
                                          "Map specified values (most neg, least neg, least pos, most pos) to (-1, 0, 0, 1) in palette");
    QButtonGroup* scaleButtonGroup = new QButtonGroup(this);
    this->paletteWidgetGroup->add(scaleButtonGroup);
    scaleButtonGroup->addButton(this->scaleAutoRadioButton);
    scaleButtonGroup->addButton(this->scaleAutoAbsolutePercentageRadioButton);
    scaleButtonGroup->addButton(this->scaleAutoPercentageRadioButton);
    scaleButtonGroup->addButton(this->scaleFixedRadioButton);
#if QT_VERSION >= 0x060000
    QObject::connect(scaleButtonGroup, &QButtonGroup::idClicked,
                     this, &MapSettingsPaletteColorMappingWidget::applyAndUpdate);
#else
    QObject::connect(scaleButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(applyAndUpdate()));
#endif
    /*
     * Spin box width (fixed may have much larger data values)
     */
    const int fixedSpinBoxWidth   = 90;
    const int percentageDigitsRightOfDecimal(2);
    const int fixedDigitsRightOfDecimal(4);
    const QSizePolicy fixedPolicy(QSizePolicy::Fixed,
                                  QSizePolicy::Fixed);
    
    /*
     * Percentage mapping 
     */
    this->scaleAutoPercentageNegativeMaximumSpinBox =
       WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   100.0,
                                                                   1.0,
                                                                   percentageDigitsRightOfDecimal,
                                                                   this,
                                                                   SLOT(scaleAutoPercentageNegativeMaximumValueChanged(double)));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentageNegativeMaximumSpinBox,
                                          "Map percentile (NOT percentage) most negative value to -1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoPercentageNegativeMaximumSpinBox);
    this->scaleAutoPercentageNegativeMaximumSpinBox->setSizePolicy(fixedPolicy);
    
    this->scaleAutoPercentageNegativeMinimumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   100.0,
                                                                   1.0,
                                                                   percentageDigitsRightOfDecimal,
                                                                   this,
                                                                   SLOT(scaleAutoPercentageNegativeMinimumValueChanged(double)));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentageNegativeMinimumSpinBox,
                                          "Map percentile (NOT percentage) least negative value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoPercentageNegativeMinimumSpinBox);
    this->scaleAutoPercentageNegativeMinimumSpinBox->setSizePolicy(fixedPolicy);
    
    this->scaleAutoPercentagePositiveMinimumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   100.0,
                                                                   1.0,
                                                                   percentageDigitsRightOfDecimal,
                                                                   this,
                                                                   SLOT(scaleAutoPercentagePositiveMinimumValueChanged(double)));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentagePositiveMinimumSpinBox,
                                          "Map percentile (NOT percentage) least positive value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoPercentagePositiveMinimumSpinBox);
    this->scaleAutoPercentagePositiveMinimumSpinBox->setSizePolicy(fixedPolicy);
    
    this->scaleAutoPercentagePositiveMaximumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   100.0,
                                                                   1.0,
                                                                   percentageDigitsRightOfDecimal,
                                                                   this,
                                                                   SLOT(scaleAutoPercentagePositiveMaximumValueChanged(double)));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentagePositiveMaximumSpinBox,
                                          "Map percentile (NOT percentage) most positive value to 1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoPercentagePositiveMaximumSpinBox);
    this->scaleAutoPercentagePositiveMaximumSpinBox->setSizePolicy(fixedPolicy);
    
    /*
     * Absolute percentage mapping
     */
    this->scaleAutoAbsolutePercentageMinimumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   100.0,
                                                                   1.0,
                                                                   percentageDigitsRightOfDecimal,
                                                                   this,
                                                                   SLOT(scaleAutoAbsolutePercentageMinimumValueChanged(double)));
    
    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoAbsolutePercentageMinimumSpinBox,
                                          "Map percentile (NOT percentage) least absolute value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoAbsolutePercentageMinimumSpinBox);
    scaleAutoAbsolutePercentageMinimumSpinBox->setSizePolicy(fixedPolicy);
    this->scaleAutoAbsolutePercentageMaximumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   100.0,
                                                                   1.0,
                                                                   percentageDigitsRightOfDecimal,
                                                                   this,
                                                                   SLOT(scaleAutoAbsolutePercentageMaximumValueChanged(double)));
    
    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoAbsolutePercentageMaximumSpinBox,
                                          "Map percentile (NOT percentage) most absolute value to 1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoAbsolutePercentageMaximumSpinBox);
    this->scaleAutoAbsolutePercentageMaximumSpinBox->setSizePolicy(fixedPolicy);

    /*
     * Fixed mapping
     */
    this->scaleFixedNegativeMaximumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(-BIG_NUMBER,
                                                                   0.0,
                                                                   1.0,
                                                                   fixedDigitsRightOfDecimal,
                                                                   this,
                                                                   SLOT(scaleFixedNegativeMaximumValueChanged(double)));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedNegativeMaximumSpinBox,
                                          "Map this value to -1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleFixedNegativeMaximumSpinBox);
    this->scaleFixedNegativeMaximumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    
    this->scaleFixedNegativeMinimumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(-BIG_NUMBER,
                                                                   0.0,
                                                                   1.0,
                                                                   fixedDigitsRightOfDecimal,
                                                                   this,
                                                                   SLOT(scaleFixedNegativeMinimumValueChanged(double)));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedNegativeMinimumSpinBox,
                                          "Map this value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleFixedNegativeMinimumSpinBox);
    this->scaleFixedNegativeMinimumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    
    this->scaleFixedPositiveMinimumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   BIG_NUMBER,
                                                                   1.0,
                                                                   fixedDigitsRightOfDecimal,
                                                                   this,
                                                                   SLOT(scaleFixedPositiveMinimumValueChanged(double)));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedPositiveMinimumSpinBox,
                                          "Map this value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleFixedPositiveMinimumSpinBox);
    this->scaleFixedPositiveMinimumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    
    this->scaleFixedPositiveMaximumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   BIG_NUMBER,
                                                                   1.0,
                                                                   fixedDigitsRightOfDecimal,
                                                                   this,
                                                                   SLOT(scaleFixedPositiveMaximumValueChanged(double)));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedPositiveMaximumSpinBox,
                                          "Map this value to 1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleFixedPositiveMaximumSpinBox);
    this->scaleFixedPositiveMaximumSpinBox->setFixedWidth(fixedSpinBoxWidth);

    /*
     * Labels for values at palette percentages
     */
    const AString bigText(7, QChar(' '));
    this->scaleNegativeMaximumValueLabel = new QLabel(bigText);
    this->scaleNegativeMinimumValueLabel = new QLabel(bigText);
    this->scalePositiveMinimumValueLabel = new QLabel(bigText);
    this->scalePositiveMaximumValueLabel = new QLabel(bigText);
    
    /*
     * Layout widgets
     */
    QWidget* colorMappingWidget = new QWidget();
    QGridLayout* colorMappingLayout = new QGridLayout(colorMappingWidget);
    colorMappingLayout->setColumnStretch(0, 0);
    colorMappingLayout->setColumnStretch(1, 0);
    colorMappingLayout->setColumnStretch(2, 0);
    colorMappingLayout->setColumnStretch(3, 0);
    colorMappingLayout->setColumnStretch(4, 100);
    this->setLayoutSpacingAndMargins(colorMappingLayout);
    colorMappingLayout->addWidget(this->scaleAutoRadioButton, 0, 0, Qt::AlignHCenter);
    colorMappingLayout->addWidget(this->scaleAutoAbsolutePercentageRadioButton, 0, 1, Qt::AlignHCenter);
    colorMappingLayout->addWidget(this->scaleAutoPercentageRadioButton, 0, 2, Qt::AlignHCenter);
    colorMappingLayout->addWidget(this->scaleFixedRadioButton, 0, 3, Qt::AlignHCenter);
    colorMappingLayout->addWidget(new QLabel("Data"), 0, 4, Qt::AlignRight);
    colorMappingLayout->addWidget(new QLabel("Pos Max"), 1, 0, Qt::AlignRight);
    colorMappingLayout->addWidget(new QLabel("Pos Min"), 2, 0, Qt::AlignRight);
    colorMappingLayout->addWidget(new QLabel("Neg Min"), 3, 0, Qt::AlignRight);
    colorMappingLayout->addWidget(new QLabel("Neg Max"), 4, 0, Qt::AlignRight);
    colorMappingLayout->addWidget(this->scaleAutoAbsolutePercentageMaximumSpinBox, 1, 1);
    colorMappingLayout->addWidget(this->scaleAutoAbsolutePercentageMinimumSpinBox, 2, 1);
    colorMappingLayout->addWidget(this->scaleAutoPercentagePositiveMaximumSpinBox, 1, 2);
    colorMappingLayout->addWidget(this->scaleAutoPercentagePositiveMinimumSpinBox, 2, 2);
    colorMappingLayout->addWidget(this->scaleAutoPercentageNegativeMinimumSpinBox, 3, 2);
    colorMappingLayout->addWidget(this->scaleAutoPercentageNegativeMaximumSpinBox, 4, 2);
    colorMappingLayout->addWidget(this->scaleFixedPositiveMaximumSpinBox, 1, 3);
    colorMappingLayout->addWidget(this->scaleFixedPositiveMinimumSpinBox, 2, 3);
    colorMappingLayout->addWidget(this->scaleFixedNegativeMinimumSpinBox, 3, 3);
    colorMappingLayout->addWidget(this->scaleFixedNegativeMaximumSpinBox, 4, 3);
    colorMappingLayout->addWidget(this->scalePositiveMaximumValueLabel, 1, 4, Qt::AlignRight);
    colorMappingLayout->addWidget(this->scalePositiveMinimumValueLabel, 2, 4, Qt::AlignRight);
    colorMappingLayout->addWidget(this->scaleNegativeMinimumValueLabel, 3, 4, Qt::AlignRight);
    colorMappingLayout->addWidget(this->scaleNegativeMaximumValueLabel, 4, 4, Qt::AlignRight);

    /*
     * Display Mode
     */
    this->displayModePositiveCheckBox = new QCheckBox("Positive");
    this->paletteWidgetGroup->add(this->displayModePositiveCheckBox);
    QObject::connect(this->displayModePositiveCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(applySelections()));
    this->displayModeZeroCheckBox = new QCheckBox("Zero");
    this->paletteWidgetGroup->add(this->displayModeZeroCheckBox);
    QObject::connect(this->displayModeZeroCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(applySelections()));
    this->displayModeNegativeCheckBox = new QCheckBox("Negative");
    this->paletteWidgetGroup->add(this->displayModeNegativeCheckBox);
    QObject::connect(this->displayModeNegativeCheckBox , SIGNAL(toggled(bool)),
                     this, SLOT(applySelections()));
    
    WuQtUtilities::setToolTipAndStatusTip(this->displayModePositiveCheckBox, 
                                          "Enable/Disable the display of positive data");
    WuQtUtilities::setToolTipAndStatusTip(this->displayModeZeroCheckBox, 
                                          "Enable/Disable the display of zero data.\n"
                                          "A value in the range ["
                                          + AString::number(PaletteColorMapping::SMALL_NEGATIVE, 'f', 6)
                                          + ", "
                                          + AString::number(PaletteColorMapping::SMALL_POSITIVE, 'f', 6)
                                          + "]\n"
                                          "is considered to be zero.");
    WuQtUtilities::setToolTipAndStatusTip(this->displayModeNegativeCheckBox, 
                                          "Enable/Disable the display of negative data");
    
    QWidget* displayModeWidget = new QWidget();
    QHBoxLayout* displayModeLayout = new QHBoxLayout(displayModeWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(displayModeLayout, 10, 3);
    displayModeLayout->addWidget(this->displayModeNegativeCheckBox);
    displayModeLayout->addStretch();
    displayModeLayout->addWidget(this->displayModeZeroCheckBox);
    displayModeLayout->addStretch();
    displayModeLayout->addWidget(this->displayModePositiveCheckBox);
    
    /*
     * Layout widgets
     */
    QGroupBox* paletteGroupBox = new QGroupBox("Palette");
    QVBoxLayout* paletteLayout = new QVBoxLayout(paletteGroupBox);
    this->setLayoutSpacingAndMargins(paletteLayout);
    paletteLayout->addWidget(paletteSelectionWidget);
    paletteLayout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    paletteLayout->addWidget(colorMappingWidget, 0, Qt::AlignLeft);
    paletteLayout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    paletteLayout->addWidget(displayModeWidget);
    paletteGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    return paletteGroupBox;
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
MapSettingsPaletteColorMappingWidget::updateEditor(CaretMappableDataFile* caretMappableDataFile,
                                                   const int32_t mapIndex)
{
    updateEditorInternal(caretMappableDataFile, mapIndex);
}

/**
 * Update the threshold section.
 */
void
MapSettingsPaletteColorMappingWidget::updateThresholdSection()
{
    this->thresholdWidgetGroup->blockAllSignals(true);
    
    const int32_t numTypes = this->thresholdTypeComboBox->count();
    for (int32_t i = 0; i < numTypes; i++) {
        const int value = this->thresholdTypeComboBox->itemData(i).toInt();
        if (value == static_cast<int>(this->paletteColorMapping->getThresholdType())) {
            this->thresholdTypeComboBox->setCurrentIndex(i);
            break;
        }
    }

    CaretAssert(this->caretMappableDataFile);
    this->thresholdMapFileIndexSelector->updateFileAndMapSelector(this->caretMappableDataFile->getMapThresholdFileSelectionModel(this->mapFileIndex));
    const bool enableThresholdControls = (this->paletteColorMapping->getThresholdType() != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF);
    this->thresholdAdjustmentWidgetGroup->setEnabled(enableThresholdControls);
    const float lowValue = this->paletteColorMapping->getThresholdMinimum(this->paletteColorMapping->getThresholdType());
    const float highValue = this->paletteColorMapping->getThresholdMaximum(this->paletteColorMapping->getThresholdType());
    
    switch (this->paletteColorMapping->getThresholdTest()) {
        case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
            this->thresholdShowOutsideRadioButton->setChecked(true);
            break;
        case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
            this->thresholdShowInsideRadioButton->setChecked(true);
            break;
    }
    
    const PaletteThresholdRangeModeEnum::Enum thresholdRangeMode = paletteColorMapping->getThresholdRangeMode();
    this->thresholdRangeModeComboBox->blockSignals(true);
    this->thresholdRangeModeComboBox->setSelectedItem<PaletteThresholdRangeModeEnum, PaletteThresholdRangeModeEnum::Enum>(thresholdRangeMode);
    this->thresholdRangeModeComboBox->blockSignals(false);
    updateThresholdControlsMinimumMaximumRangeValues();
    
    this->thresholdLowSlider->setValue(lowValue);
    
    this->thresholdHighSlider->setValue(highValue);
    
    if (allowUpdateOfThresholdLowSpinBox) {
        this->thresholdLowSpinBox->setValue(lowValue);
    }
    
    if (allowUpdateOfThresholdHighSpinBox) {
        this->thresholdHighSpinBox->setValue(highValue);
    }
    
    this->thresholdLinkCheckBox->setChecked(this->paletteColorMapping->isThresholdNegMinPosMaxLinked());
    
    this->thresholdAdjustmentWidget->setEnabled(paletteColorMapping->getThresholdType() != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF);
    this->thresholdFileWidget->setEnabled(paletteColorMapping->getThresholdType() == PaletteThresholdTypeEnum::THRESHOLD_TYPE_FILE);
    this->thresholdOutlineDrawingModeComboBox->setSelectedItem<PaletteThresholdOutlineDrawingModeEnum, PaletteThresholdOutlineDrawingModeEnum::Enum>(this->paletteColorMapping->getThresholdOutlineDrawingMode());
    this->thresholdOutlineDrawingColorComboBox->setSelectedColor(this->paletteColorMapping->getThresholdOutlineDrawingColor());
    this->thresholdOutlineDrawingWidget->setEnabled(this->paletteColorMapping->getThresholdType() != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF);
    this->thresholdWidgetGroup->blockAllSignals(false);
    
    bool enableSetAllMapsButtonFlag = false;
    if (this->paletteColorMapping->getThresholdType() == PaletteThresholdTypeEnum::THRESHOLD_TYPE_FILE) {
        if (this->caretMappableDataFile->getNumberOfMaps() > 1) {
            const CaretMappableDataFile* threshFile = this->caretMappableDataFile->getMapThresholdFileSelectionModel(this->mapFileIndex)->getSelectedFile();
            if (threshFile != NULL) {
                if (threshFile->getNumberOfMaps() > 0) {
                    enableSetAllMapsButtonFlag = true;
                }
            }
        }
    }
    this->thresholdSetAllMapsToolButton->setEnabled(enableSetAllMapsButtonFlag);
}

/**
 * Update the labels that list values that correspond to the palette
 * mapping values (percent neg/pos min/max, full, etc).
 */
void
MapSettingsPaletteColorMappingWidget::updatePaletteMappedToDataValueLabels()
{
    if (this->caretMappableDataFile == NULL) {
        return;
    }
    else if (this->mapFileIndex < 0) {
        return;
    }
    
    FastStatistics* statistics = NULL;
    switch (this->caretMappableDataFile->getPaletteNormalizationMode()) {
        case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
            statistics = const_cast<FastStatistics*>(this->caretMappableDataFile->getFileFastStatistics());
            break;
        case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
            statistics = const_cast<FastStatistics*>(this->caretMappableDataFile->getMapFastStatistics(this->mapFileIndex));
            break;
    }
    
    double posMaxLabelValue = 0.0;
    double posMinLabelValue = 0.0;
    double negMinLabelValue = 0.0;
    double negMaxLabelValue = 0.0;
    
    if (statistics != NULL) {
        switch (this->paletteColorMapping->getScaleMode()) {
            case PaletteScaleModeEnum::MODE_AUTO_SCALE:
                posMaxLabelValue = statistics->getMostPositiveValue();
                posMinLabelValue = 0.0;
                negMinLabelValue = 0.0;
                negMaxLabelValue = statistics->getMostNegativeValue();
                break;
            case PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE:
                posMaxLabelValue = statistics->getApproxPositivePercentile(this->paletteColorMapping->getAutoScaleAbsolutePercentageMaximum());
                posMinLabelValue = statistics->getApproxPositivePercentile(this->paletteColorMapping->getAutoScaleAbsolutePercentageMinimum());
                negMinLabelValue = -posMinLabelValue;
                negMaxLabelValue = -posMaxLabelValue;
                break;
            case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
                posMaxLabelValue = statistics->getApproxPositivePercentile(this->paletteColorMapping->getAutoScalePercentagePositiveMaximum());
                posMinLabelValue = statistics->getApproxPositivePercentile(this->paletteColorMapping->getAutoScalePercentagePositiveMinimum());
                negMinLabelValue = statistics->getApproxNegativePercentile(this->paletteColorMapping->getAutoScalePercentageNegativeMinimum());
                negMaxLabelValue = statistics->getApproxNegativePercentile(this->paletteColorMapping->getAutoScalePercentageNegativeMaximum());
                break;
            case PaletteScaleModeEnum::MODE_USER_SCALE:
                posMaxLabelValue = this->paletteColorMapping->getUserScalePositiveMaximum();
                posMinLabelValue = this->paletteColorMapping->getUserScalePositiveMinimum();
                negMinLabelValue = this->paletteColorMapping->getUserScaleNegativeMinimum();
                negMaxLabelValue = this->paletteColorMapping->getUserScaleNegativeMaximum();
                break;
        }
    }
    
    const int32_t digitsRightOfDecimal(4);
    this->scalePositiveMaximumValueLabel->setText(QString::number(posMaxLabelValue, 'f', digitsRightOfDecimal));
    this->scalePositiveMinimumValueLabel->setText(QString::number(posMinLabelValue, 'f', digitsRightOfDecimal));
    this->scaleNegativeMinimumValueLabel->setText(QString::number(negMinLabelValue, 'f', digitsRightOfDecimal));
    this->scaleNegativeMaximumValueLabel->setText(QString::number(negMaxLabelValue, 'f', digitsRightOfDecimal));
}

void
MapSettingsPaletteColorMappingWidget::updatePaletteNameComboBox()
{
    this->paletteNameComboBox->clear();

    PaletteFile* paletteFile = GuiManager::get()->getBrain()->getPaletteFile();
    
    bool firstValidPixmapFlag(true);
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
        
        const bool showColorMappingFlag(true);
        if (showColorMappingFlag) {
            PalettePixmapPainter palettePainter(palette,
                                                PalettePixmapPainter::Mode::INTERPOLATE_ON);
            QPixmap pixmap = palettePainter.getPixmap();
            if (pixmap.isNull()) {
                this->paletteNameComboBox->addItem(name,
                                                   paletteUniqueID);
            }
            else {
                if (firstValidPixmapFlag) {
                    firstValidPixmapFlag = false;
                    this->paletteNameComboBox->setIconSize(pixmap.size());
                }
                this->paletteNameComboBox->addItem(pixmap,
                                                   " " + name,
                                                   paletteUniqueID);
            }
        }
        else {
            this->paletteNameComboBox->addItem(name,
                                               paletteUniqueID);
        }
    }
}

/**
 * This PRIVATE method updates the editor content and MUST always be used
 * when something within this class requires updating the displayed data.
 *
 * Update contents for editing a map settings for data in a caret
 * mappable data file.
 *
 * @param caretMappableDataFile
 *    Data file containing palette that is edited.
 * @param mapIndexIn
 *    Index of map for palette that is edited.
 */
void 
MapSettingsPaletteColorMappingWidget::updateEditorInternal(CaretMappableDataFile* caretMappableDataFile,
                                                           const int32_t mapIndexIn)
{
    this->caretMappableDataFile = caretMappableDataFile;
    this->mapFileIndex = mapIndexIn;
    
    if (this->caretMappableDataFile == NULL) {
        return;
    }
    else if (this->mapFileIndex < 0) {
        return;
    }
    
    this->paletteWidgetGroup->blockAllSignals(true);
    
    const AString title =
    this->caretMappableDataFile->getFileNameNoPath()
    + ": "
    + this->caretMappableDataFile->getMapName(this->mapFileIndex);
    this->setWindowTitle(title);
    
    this->paletteColorMapping = this->caretMappableDataFile->getMapPaletteColorMapping(this->mapFileIndex);
    
    if (this->paletteColorMapping != NULL) {
        int32_t paletteComboBoxIndex(0);
        const AString paletteName = this->paletteColorMapping->getSelectedPaletteName();
        for (int32_t i = 0; i < this->paletteNameComboBox->count(); i++) {
            if (this->paletteNameComboBox->itemData(i).toString() == paletteName) {
                paletteComboBoxIndex = i;
                break;
            }
        }
        if (paletteComboBoxIndex < this->paletteNameComboBox->count()) {
            this->paletteNameComboBox->setCurrentIndex(paletteComboBoxIndex);
        }
        
        bool isPercentageSpinBoxesEnabled = false;
        bool isAbsolutePercentageSpinBoxesEnabled = false;
        bool isFixedSpinBoxesEnabled = false;
        switch (this->paletteColorMapping->getScaleMode()) {
            case PaletteScaleModeEnum::MODE_AUTO_SCALE:
                this->scaleAutoRadioButton->setChecked(true);
                break;
            case PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE:
                this->scaleAutoAbsolutePercentageRadioButton->setChecked(true);
                isAbsolutePercentageSpinBoxesEnabled = true;
                break;
            case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
                this->scaleAutoPercentageRadioButton->setChecked(true);
                isPercentageSpinBoxesEnabled = true;
                break;
            case PaletteScaleModeEnum::MODE_USER_SCALE:
                this->scaleFixedRadioButton->setChecked(true);
                isFixedSpinBoxesEnabled = true;
                break;
        }
        
        this->scaleAutoPercentageNegativeMaximumSpinBox->setValue(this->paletteColorMapping->getAutoScalePercentageNegativeMaximum());
        this->scaleAutoPercentageNegativeMinimumSpinBox->setValue(this->paletteColorMapping->getAutoScalePercentageNegativeMinimum());
        this->scaleAutoPercentagePositiveMinimumSpinBox->setValue(this->paletteColorMapping->getAutoScalePercentagePositiveMinimum());
        this->scaleAutoPercentagePositiveMaximumSpinBox->setValue(this->paletteColorMapping->getAutoScalePercentagePositiveMaximum());
        this->scaleAutoPercentageNegativeMaximumSpinBox->setEnabled(isPercentageSpinBoxesEnabled);
        this->scaleAutoPercentageNegativeMinimumSpinBox->setEnabled(isPercentageSpinBoxesEnabled);
        this->scaleAutoPercentagePositiveMinimumSpinBox->setEnabled(isPercentageSpinBoxesEnabled);
        this->scaleAutoPercentagePositiveMaximumSpinBox->setEnabled(isPercentageSpinBoxesEnabled);
        
        this->scaleAutoAbsolutePercentageMaximumSpinBox->setValue(this->paletteColorMapping->getAutoScaleAbsolutePercentageMaximum());
        this->scaleAutoAbsolutePercentageMinimumSpinBox->setValue(this->paletteColorMapping->getAutoScaleAbsolutePercentageMinimum());
        this->scaleAutoAbsolutePercentageMaximumSpinBox->setEnabled(isAbsolutePercentageSpinBoxesEnabled);
        this->scaleAutoAbsolutePercentageMinimumSpinBox->setEnabled(isAbsolutePercentageSpinBoxesEnabled);
        
        this->scaleFixedNegativeMaximumSpinBox->setValue(this->paletteColorMapping->getUserScaleNegativeMaximum());
        this->scaleFixedNegativeMinimumSpinBox->setValue(this->paletteColorMapping->getUserScaleNegativeMinimum());
        this->scaleFixedPositiveMinimumSpinBox->setValue(this->paletteColorMapping->getUserScalePositiveMinimum());
        this->scaleFixedPositiveMaximumSpinBox->setValue(this->paletteColorMapping->getUserScalePositiveMaximum());
        this->scaleFixedNegativeMaximumSpinBox->setEnabled(isFixedSpinBoxesEnabled);
        this->scaleFixedNegativeMinimumSpinBox->setEnabled(isFixedSpinBoxesEnabled);
        this->scaleFixedPositiveMinimumSpinBox->setEnabled(isFixedSpinBoxesEnabled);
        this->scaleFixedPositiveMaximumSpinBox->setEnabled(isFixedSpinBoxesEnabled);
        
        this->displayModePositiveCheckBox->setChecked(this->paletteColorMapping->isDisplayPositiveDataFlag());
        this->displayModeZeroCheckBox->setChecked(this->paletteColorMapping->isDisplayZeroDataFlag());
        this->displayModeNegativeCheckBox->setChecked(this->paletteColorMapping->isDisplayNegativeDataFlag());
    
        this->interpolateColorsCheckBox->setChecked(this->paletteColorMapping->isInterpolatePaletteFlag());
        this->invertPaletteModeComboBox->setSelectedItem<PaletteInvertModeEnum, PaletteInvertModeEnum::Enum>(this->paletteColorMapping->getInvertedMode());
        
        m_histogramBarsColorComboBox->setSelectedColor(this->paletteColorMapping->getHistogramBarsColor());
        m_histogramEnvelopeColorComboBox->setSelectedColor(this->paletteColorMapping->getHistogramEnvelopeColor());
        m_histogramEnvelopeLineWidthPercentageSpinBox->setValue(this->paletteColorMapping->getHistogramEnvelopeLineWidthPercentage());
        m_histogramBarsVisibleCheckBox->setChecked(this->paletteColorMapping->isHistogramBarsVisible());
        m_histogramEnvelopeVisibleCheckBox->setChecked(this->paletteColorMapping->isHistogramEnvelopeVisible());

        m_histogramHorizontalRangeComboBox->setSelectedItem<PaletteHistogramRangeModeEnum, PaletteHistogramRangeModeEnum::Enum>(this->paletteColorMapping->getHistogramRangeMode());

        updatePaletteMappedToDataValueLabels();
        updateThresholdSection();
        
        float minValue  = 0.0;
        float maxValue  = 0.0;
        
        m_histogramBucketsSpinBox->blockSignals(true);
        switch (this->caretMappableDataFile->getPaletteNormalizationMode()) {
            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                m_histogramBucketsSpinBox->setValue(this->caretMappableDataFile->getFileHistogramNumberOfBuckets());
                break;
            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                m_histogramBucketsSpinBox->setValue(this->paletteColorMapping->getHistogramNumberOfBuckets());
                break;
        }
        m_histogramBucketsSpinBox->blockSignals(false);
        
        FastStatistics* statistics = NULL;
        switch (this->caretMappableDataFile->getPaletteNormalizationMode()) {
            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                statistics = const_cast<FastStatistics*>(this->caretMappableDataFile->getFileFastStatistics());
                break;
            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                statistics = const_cast<FastStatistics*>(this->caretMappableDataFile->getMapFastStatistics(this->mapFileIndex));
                break;
        }
        
        if (statistics != NULL) {
            minValue  = statistics->getMin();
            maxValue  = statistics->getMax();
        }
        
        /*
         * Set fixed spin boxes so that they increment by 1% of data.
         */
        float stepValue = 1.0;
        const float diff = maxValue - minValue;
        if (diff > 0.0) {
            stepValue = diff / 100.0;
        }
        this->scaleFixedNegativeMaximumSpinBox->setSingleStep(stepValue);
        this->scaleFixedNegativeMinimumSpinBox->setSingleStep(stepValue);
        this->scaleFixedPositiveMinimumSpinBox->setSingleStep(stepValue);
        this->scaleFixedPositiveMaximumSpinBox->setSingleStep(stepValue);
    }
    
    m_paletteOptionsWidget->updateEditor(this->caretMappableDataFile,
                                         this->mapFileIndex);
    
    this->updateHistogramPlot();
    
    this->updateNormalizationControlSection();
    
    m_preColorAllMapsCheckBox->setChecked(this->caretMappableDataFile->isPreColorAllMaps());
    
    this->paletteWidgetGroup->blockAllSignals(false);
}

/**
 * Get histogram for displaying data
 * @param statisticsForAll
 *    Statistics for all data.
 * @param
 *    Histogram.
 */
const Histogram* 
MapSettingsPaletteColorMappingWidget::getHistogram(const FastStatistics* statisticsForAll) const
{
    float mostPos  = 0.0;
    float leastPos = 0.0;
    float leastNeg = 0.0;
    float mostNeg  = 0.0;
    bool matchFlag = false;
    
    switch (this->paletteColorMapping->getHistogramRangeMode()) {
        case PaletteHistogramRangeModeEnum::PALETTE_HISTOGRAM_RANGE_ALL:
        {
            float dummy;
            statisticsForAll->getNonzeroRanges(mostNeg, dummy, dummy, mostPos);
        }
            break;
        case PaletteHistogramRangeModeEnum::PALETTE_HISTOGRAM_RANGE_MATCH_PALETTE:
        {
            matchFlag = true;
            switch (this->paletteColorMapping->getScaleMode()) {
                case PaletteScaleModeEnum::MODE_AUTO_SCALE:
                    mostPos  = statisticsForAll->getMax();
                    leastPos = 0.0;
                    leastNeg = 0.0;
                    mostNeg  = statisticsForAll->getMin();
                    break;
                case PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE:
                    mostPos  =  statisticsForAll->getApproxAbsolutePercentile(this->scaleAutoAbsolutePercentageMaximumSpinBox->value());
                    leastPos =  statisticsForAll->getApproxAbsolutePercentile(this->scaleAutoAbsolutePercentageMinimumSpinBox->value());
                    leastNeg = -statisticsForAll->getApproxAbsolutePercentile(this->scaleAutoAbsolutePercentageMinimumSpinBox->value());
                    mostNeg  = -statisticsForAll->getApproxAbsolutePercentile(this->scaleAutoAbsolutePercentageMaximumSpinBox->value());
                    break;
                case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
                    mostPos  = statisticsForAll->getApproxPositivePercentile(this->scaleAutoPercentagePositiveMaximumSpinBox->value());
                    leastPos = statisticsForAll->getApproxPositivePercentile(this->scaleAutoPercentagePositiveMinimumSpinBox->value());
                    leastNeg = statisticsForAll->getApproxNegativePercentile(this->scaleAutoPercentageNegativeMinimumSpinBox->value());
                    mostNeg  = statisticsForAll->getApproxNegativePercentile(this->scaleAutoPercentageNegativeMaximumSpinBox->value());
                    break;
                case PaletteScaleModeEnum::MODE_USER_SCALE:
                    mostPos  = this->scaleFixedPositiveMaximumSpinBox->value();
                    leastPos = this->scaleFixedPositiveMinimumSpinBox->value();
                    leastNeg = this->scaleFixedNegativeMinimumSpinBox->value();
                    mostNeg  = this->scaleFixedNegativeMaximumSpinBox->value();
                    break;
            }
        }
            break;
    }
    
    const PaletteNormalizationModeEnum::Enum normMode = this->caretMappableDataFile->getPaletteNormalizationMode();
    
    /*
     * Remove data that is not displayed
     */
    bool isZeroIncluded = true;
    const Histogram* ret = NULL;
    if (matchFlag) {
        isZeroIncluded = this->displayModeZeroCheckBox->isChecked();
        
        if (this->displayModeNegativeCheckBox->isChecked() == false) {
            mostNeg  = 0.0;
            leastNeg = 0.0;
        }
        if (this->displayModePositiveCheckBox->isChecked() == false) {
            mostPos  = 0.0;
            leastPos = 0.0;
        }
        
        switch (normMode) {
            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                ret = this->caretMappableDataFile->getFileHistogram(mostPos,
                                                                    leastPos,
                                                                    leastNeg,
                                                                    mostNeg,
                                                                    isZeroIncluded);
                break;
            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                ret = this->caretMappableDataFile->getMapHistogram(this->mapFileIndex,
                                                                   mostPos,
                                                                   leastPos,
                                                                   leastNeg,
                                                                   mostNeg,
                                                                   isZeroIncluded);
                break;
        }
    } else {
        switch (normMode) {
            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                ret = caretMappableDataFile->getFileHistogram();
                break;
            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                ret = caretMappableDataFile->getMapHistogram(this->mapFileIndex);
                break;
        }
    }
    
    CaretAssert(ret);
    
    return ret;
}

/**
 * Update the histogram plot.
 */
void 
MapSettingsPaletteColorMappingWidget::updateHistogramPlot()
{
    /*
     * Remove all previously attached items from the histogram plot.
     * The items are automatically deleted by the plot.
     */
    this->thresholdPlot->detachItems();
    
    
    
    if (this->paletteColorMapping == NULL) {
        return;
    }
    FastStatistics* statistics = NULL;
    switch (this->caretMappableDataFile->getPaletteNormalizationMode()) {
        case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
            statistics = const_cast<FastStatistics*>(this->caretMappableDataFile->getFileFastStatistics());
            break;
        case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
            statistics = const_cast<FastStatistics*>(this->caretMappableDataFile->getMapFastStatistics(this->mapFileIndex));
            break;
    }
    if (statistics == NULL) {
        return;
    }
    if (this->paletteColorMapping->isHistogramBarsVisible()
        || this->paletteColorMapping->isHistogramEnvelopeVisible()) {
        /* okay */
    }
    else {
        /* nothing to display */
        return;
    }
    
    /*
     * Data values table
     */
    const float statsMean   = statistics->getMean();
    const float statsStdDev = statistics->getSampleStdDev();
    const float statsMin    = statistics->getMin();
    const float statsMax    = statistics->getMax();
    this->statisticsMeanValueLabel->setText(QString::number(statsMean, 'f', 4));
    this->statisticsStandardDeviationLabel->setText(QString::number(statsStdDev, 'f', 4));
    this->statisticsMaximumValueLabel->setText(QString::number(statsMax, 'f', 4));
    this->statisticsMinimumValueLabel->setText(QString::number(statsMin, 'f', 4));
    
    /*
     * Get data for this histogram.
     */
    const Histogram* myHist = getHistogram(statistics);
    float minValue, maxValue;
    myHist->getRange(minValue, maxValue);
    const std::vector<float>& displayDataReference = myHist->getHistogramDisplay();
    std::vector<float> displayData = displayDataReference;
    const int64_t numHistogramValues = (int64_t)(displayData.size());
    
    /*
     * Width of each 'bar' in the histogram
     */
    float step = 1.0;
    if (numHistogramValues > 1) {
        step = ((maxValue - minValue)
                / numHistogramValues);
    }
    
    const CaretColorEnum::Enum histogramBarsColor = paletteColorMapping->getHistogramBarsColor();
    const CaretColorEnum::Enum histogramEnvelopeColor = paletteColorMapping->getHistogramEnvelopeColor();
    
    float* dataValues = NULL;
    float* dataRGBA = NULL;
    if (numHistogramValues > 0) {
        /*
         * Compute color for 'bar' in the histogram.
         * Note that number of data values is one more
         * than the number of histogram values due to that
         * a data value is needed on the right of the last
         * histogram bar.  Otherwise, if there is an outlier
         * value, the histogram will not be drawn
         * correctly.
         */
        const int64_t numDataValues = numHistogramValues + 1;
        dataValues = new float[numDataValues];
        dataRGBA   = new float[numDataValues * 4];
        for (int64_t ix = 0; ix < numDataValues; ix++) {
            const float value = (minValue
                                 + (ix * step));
            dataValues[ix] = value;
        }
        dataValues[0] = minValue;
        dataValues[numDataValues - 1] = maxValue;
        
        /*
         * Color with palette so that alpha values are zero for regions not displayed
         */
        NodeAndVoxelColoring::colorScalarsWithPalette(statistics,
                                                      paletteColorMapping,
                                                      dataValues,
                                                      paletteColorMapping,
                                                      dataValues,
                                                      numDataValues,
                                                      dataRGBA,
                                                      true); /* ignore thresholding */
        
        /*
         * If bucket is not colored (zero alpha) set bucket height to zero
         */
        for (int32_t i = 0; i < numHistogramValues; i++) {
            const int32_t alphaIndex = i * 4 + 3;
            CaretAssertArrayIndex(dataRGBA, (numDataValues * 4), alphaIndex);
            if (dataRGBA[alphaIndex] <= 0.0) {
                CaretAssertVectorIndex(displayData, i);
                displayData[i] = 0.0;
            }
        }
    }
    
    float z = 0.0;
    float maxDataFrequency = 0.0;
    for (int32_t drawModeInt = 0; drawModeInt < 2; drawModeInt++) {
        bool drawBarsFlag = false;
        bool drawEnvelopeFlag = false;
        if (drawModeInt == 0) {
            drawBarsFlag = this->paletteColorMapping->isHistogramBarsVisible();
        }
        else if (drawModeInt == 1) {
            drawEnvelopeFlag = this->paletteColorMapping->isHistogramEnvelopeVisible();
        }
        
        if (drawBarsFlag
            || drawEnvelopeFlag) {
            float barsSolidRGBA[4];
            CaretColorEnum::toRGBAFloat(histogramBarsColor,
                                        barsSolidRGBA);
            float envelopeSolidRGBA[4];
            CaretColorEnum::toRGBAFloat(histogramEnvelopeColor,
                                        envelopeSolidRGBA);
            
            const int32_t lastIndex = numHistogramValues - 1;
            for (int64_t ix = 0; ix < numHistogramValues; ix++) {
                QColor color;
                const int64_t ix4 = ix * 4;
                if (drawBarsFlag) {
                    if (histogramBarsColor == CaretColorEnum::CUSTOM) {
                        color.setRedF(dataRGBA[ix4]);
                        color.setGreenF(dataRGBA[ix4+1]);
                        color.setBlueF(dataRGBA[ix4+2]);
                        color.setAlphaF(1.0);
                    }
                    else {
                        color.setRgbF(barsSolidRGBA[0], barsSolidRGBA[1], barsSolidRGBA[2]);
                    }
                }
                else if (drawEnvelopeFlag) {
                    if (histogramEnvelopeColor == CaretColorEnum::CUSTOM) {
                        color.setRedF(dataRGBA[ix4]);
                        color.setGreenF(dataRGBA[ix4+1]);
                        color.setBlueF(dataRGBA[ix4+2]);
                        color.setAlphaF(1.0);
                    }
                    else {
                        color.setRgbF(envelopeSolidRGBA[0], envelopeSolidRGBA[1], envelopeSolidRGBA[2]);
                    }
                }
                else {
                    CaretAssert(0);
                }
                
                const float startValue = dataValues[ix];
                const float stopValue  = dataValues[ix + 1];
                float dataFrequency = displayData[ix];
                
                bool displayIt = true;
                
                if (dataFrequency > maxDataFrequency) {
                    maxDataFrequency = dataFrequency;
                }
                
                /*
                 * If color is not displayed ('none' or thresholded),
                 * set its frequncey value to a small value so that the plot
                 * retains its shape and color is still slightly visible
                 */
                if (dataRGBA[ix4+3] <= 0.0) {
                    displayIt = false;
                }
                
                if ( ! displayIt) {
                    if (drawBarsFlag) {
                        color.setAlpha(0);
                    }
                    if (drawEnvelopeFlag) {
                        dataFrequency = 0.0f;
                    }
                }
                
                QVector<QPointF> samples;
                
                QwtPlotCurve* curve = new QwtPlotCurve();
                curve->setRenderHint(QwtPlotItem::RenderAntialiased);
                curve->setVisible(true);
                
                curve->setPen(QPen(color));
                
                if (drawBarsFlag) {
                    curve->setStyle(QwtPlotCurve::Steps);
                    curve->setBrush(QBrush(color));
                    samples.push_back(QPointF(startValue, dataFrequency));
                    samples.push_back(QPointF(stopValue, dataFrequency));
                }
                if (drawEnvelopeFlag) {
                    curve->setStyle(QwtPlotCurve::Lines);
                    
                    static float lastX = 0.0f;
                    static float lastY = 0.0f;
                    
                    /*
                     * Left side
                     */
                    if (ix == 0) {
                        samples.push_back(QPointF(startValue, 0.0));
                        samples.push_back(QPointF(startValue, dataFrequency));
                        lastX = startValue;
                        lastY = dataFrequency;
                    }
                    else {
                        CaretAssertVectorIndex(displayData, ix - 1);
                        const float lastValue = dataValues[ix - 1];
                        const float halfX = (startValue - lastValue) / 2.0f;
                        const float x = startValue + halfX;
                        const float y = dataFrequency;
                        samples.push_back(QPointF(lastX, lastY));
                        samples.push_back(QPointF(x, y));
                        lastX = x;
                        lastY = y;
                    }
                    
                    /*
                     * Right side
                     */
                    if (ix == lastIndex) {
                        samples.push_back(QPointF(stopValue, dataFrequency));
                        samples.push_back(QPointF(stopValue, 0.0));
                        lastX = stopValue;
                        lastY = 0.0f;
                    }
                }
                
                curve->setSamples(samples);
                curve->attach(this->thresholdPlot);
                
                if (ix == 0) {
                    z = curve->z();
                }
            }
        }
        
        z--;
    }
    
    z--;
    
    if ((numHistogramValues > 2)
        && (this->paletteColorMapping->getThresholdType() != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF)) {
        float threshMinValue = this->paletteColorMapping->getThresholdNormalMinimum();
        float threshMaxValue = this->paletteColorMapping->getThresholdNormalMaximum();
        
        maxDataFrequency *= 1.05;
        
        bool showBlueThresholdedRegionsFlag(false);
        if (this->paletteColorMapping->getThresholdType() == PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL) {
            showBlueThresholdedRegionsFlag = true;
        }
        
        if (showBlueThresholdedRegionsFlag) {
            switch (this->paletteColorMapping->getThresholdTest()) {
                case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
                {
                    const float plotMinValue = this->thresholdPlot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
                    const float plotMaxValue = this->thresholdPlot->axisScaleDiv(QwtPlot::xBottom).upperBound();
                    
                    /*
                     * Draw shaded region to left of minimum threshold
                     */
                    QVector<QPointF> minSamples;
                    minSamples.push_back(QPointF(plotMinValue, maxDataFrequency));
                    minSamples.push_back(QPointF(threshMinValue, maxDataFrequency));
                    
                    QwtPlotCurve* minBox = new QwtPlotCurve();
                    minBox->setRenderHint(QwtPlotItem::RenderAntialiased);
                    minBox->setVisible(true);
                    
                    QColor minColor(100, 100, 255, 160);
                    minBox->setBrush(QBrush(minColor));
                    minBox->setPen(QPen(minColor));
                    minBox->setSamples(minSamples);
                    
                    minBox->setZ(z);
                    minBox->attach(this->thresholdPlot);
                    
                    /*
                     * Draw shaded region to right of maximum threshold
                     */
                    QVector<QPointF> maxSamples;
                    maxSamples.push_back(QPointF(threshMaxValue, maxDataFrequency));
                    maxSamples.push_back(QPointF(plotMaxValue, maxDataFrequency));
                    
                    QwtPlotCurve* maxBox = new QwtPlotCurve();
                    maxBox->setRenderHint(QwtPlotItem::RenderAntialiased);
                    maxBox->setVisible(true);
                    
                    QColor maxColor(100, 100, 255, 160);
                    maxBox->setBrush(QBrush(maxColor));
                    maxBox->setPen(QPen(maxColor));
                    maxBox->setSamples(maxSamples);
                    
                    maxBox->setZ(z);
                    maxBox->attach(this->thresholdPlot);
                }
                    break;
                case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
                {
                    /*
                     * Draw shaded region between minimum and maximum threshold
                     */
                    QVector<QPointF> minSamples;
                    minSamples.push_back(QPointF(threshMinValue, maxDataFrequency));
                    minSamples.push_back(QPointF(threshMaxValue, maxDataFrequency));
                    
                    QwtPlotCurve* minBox = new QwtPlotCurve();
                    minBox->setRenderHint(QwtPlotItem::RenderAntialiased);
                    minBox->setVisible(true);
                    
                    QColor minColor(100, 100, 255, 160);
                    minBox->setBrush(QBrush(minColor));
                    minBox->setPen(QPen(minColor));
                    minBox->setSamples(minSamples);
                    
                    minBox->setZ(z);
                    
                    minBox->attach(this->thresholdPlot);
                }
                    break;
            }
        }
        
        const bool showLinesFlag = false;
        if (showLinesFlag) {
            /*
             * Line for high threshold
             */
            QVector<QPointF> minSamples;
            minSamples.push_back(QPointF(threshMinValue, 0));
            minSamples.push_back(QPointF(threshMinValue, maxDataFrequency));
            
            QwtPlotCurve* minLine = new QwtPlotCurve();
            minLine->setRenderHint(QwtPlotItem::RenderAntialiased);
            minLine->setVisible(true);
            minLine->setStyle(QwtPlotCurve::Lines);
            
            QColor minColor(0.0, 0.0, 1.0);
            minLine->setPen(QPen(minColor));
            minLine->setSamples(minSamples);
            
            minLine->setZ(1.0);
            minLine->attach(this->thresholdPlot);
            
            /*
             * Line for high threshold
             */
            QVector<QPointF> maxSamples;
            maxSamples.push_back(QPointF(threshMaxValue, 0));
            maxSamples.push_back(QPointF(threshMaxValue, maxDataFrequency));
            
            QwtPlotCurve* maxLine = new QwtPlotCurve();
            maxLine->setRenderHint(QwtPlotItem::RenderAntialiased);
            maxLine->setVisible(true);
            maxLine->setStyle(QwtPlotCurve::Lines);
            
            QColor maxColor(1.0, 0.0, 0.0);
            maxLine->setPen(QPen(maxColor));
            maxLine->setSamples(maxSamples);
            
            maxLine->setZ(1.0);
            maxLine->attach(this->thresholdPlot);
        }
    }
    
    if (dataValues != NULL) {
        delete[] dataValues;
    }
    if (dataRGBA != NULL) {
        delete[] dataRGBA;
    }
    
    /*
     * Causes updates of plots.
     */
    this->thresholdPlot->replot();
}

/**
 * Called to apply selections.
 */
void MapSettingsPaletteColorMappingWidget::applySelections()
{
    if (this->caretMappableDataFile == NULL) {
        return;
    }
    else if (this->mapFileIndex < 0) {
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
    else if (this->scaleAutoAbsolutePercentageRadioButton->isChecked()) {
        this->paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE);
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
    
    this->paletteColorMapping->setAutoScaleAbsolutePercentageMaximum(this->scaleAutoAbsolutePercentageMaximumSpinBox->value());
    this->paletteColorMapping->setAutoScaleAbsolutePercentageMinimum(this->scaleAutoAbsolutePercentageMinimumSpinBox->value());
    
    this->paletteColorMapping->setDisplayPositiveDataFlag(this->displayModePositiveCheckBox->isChecked());
    this->paletteColorMapping->setDisplayNegativeDataFlag(this->displayModeNegativeCheckBox->isChecked());
    this->paletteColorMapping->setDisplayZeroDataFlag(this->displayModeZeroCheckBox->isChecked());
    
    this->paletteColorMapping->setInterpolatePaletteFlag(this->interpolateColorsCheckBox->isChecked());
    this->paletteColorMapping->setInvertedMode(this->invertPaletteModeComboBox->getSelectedItem<PaletteInvertModeEnum, PaletteInvertModeEnum::Enum>());
    
    this->paletteColorMapping->setHistogramBarsColor(m_histogramBarsColorComboBox->getSelectedColor());
    this->paletteColorMapping->setHistogramEnvelopeColor(m_histogramEnvelopeColorComboBox->getSelectedColor());
    this->paletteColorMapping->setHistogramEnvelopeLineWidthPercentage(m_histogramEnvelopeLineWidthPercentageSpinBox->value());
    this->paletteColorMapping->setHistogramBarsVisible(m_histogramBarsVisibleCheckBox->isChecked());
    this->paletteColorMapping->setHistogramEnvelopeVisible(m_histogramEnvelopeVisibleCheckBox->isChecked());
    this->paletteColorMapping->setHistogramRangeMode(m_histogramHorizontalRangeComboBox->getSelectedItem<PaletteHistogramRangeModeEnum, PaletteHistogramRangeModeEnum::Enum>());
    switch (this->caretMappableDataFile->getPaletteNormalizationMode()) {
        case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
            this->caretMappableDataFile->setFileHistogramNumberOfBuckets(m_histogramBucketsSpinBox->value());
            break;
        case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
            this->paletteColorMapping->setHistogramNumberOfBuckets(m_histogramBucketsSpinBox->value());
            break;
    }
    
    float lowValue = this->thresholdLowSpinBox->value();
    float highValue = this->thresholdHighSpinBox->value();
    
    const int thresholdTypeIndex = this->thresholdTypeComboBox->currentIndex();
    PaletteThresholdTypeEnum::Enum paletteThresholdType = static_cast<PaletteThresholdTypeEnum::Enum>(this->thresholdTypeComboBox->itemData(thresholdTypeIndex).toInt());
    this->paletteColorMapping->setThresholdType(paletteThresholdType);
    
    this->paletteColorMapping->setThresholdMinimum(paletteThresholdType, lowValue);
    this->paletteColorMapping->setThresholdMaximum(paletteThresholdType, highValue);
    
    if (this->thresholdShowInsideRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdTest(PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE);
    }
    else if (this->thresholdShowOutsideRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdTest(PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE);
    }
    
    this->paletteColorMapping->setThresholdOutlineDrawingMode(this->thresholdOutlineDrawingModeComboBox->getSelectedItem<PaletteThresholdOutlineDrawingModeEnum, PaletteThresholdOutlineDrawingModeEnum::Enum>());
    this->paletteColorMapping->setThresholdOutlineDrawingColor(this->thresholdOutlineDrawingColorComboBox->getSelectedColor());
    
    this->updateHistogramPlot();
    
    m_paletteOptionsWidget->applyOptions();
}

/**
 * Set the layout margins.
 * @param layout
 *   Layout for which margins are set.
 */
void 
MapSettingsPaletteColorMappingWidget::setLayoutSpacingAndMargins(QLayout* layout)
{
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 5, 3);
}

/**
 * @return The normalization control section.
 */
QWidget*
MapSettingsPaletteColorMappingWidget::createNormalizationControlSection()
{
    m_normalizationModeComboBox = new QComboBox();
    QObject::connect(m_normalizationModeComboBox, SIGNAL(activated(int)),
                     this, SLOT(normalizationModeComboBoxActivated(int)));

    /*
     * Initially load with all modes so that the combo box can
     * be set to a fixed size that allows display of all text
     */
    std::vector<PaletteNormalizationModeEnum::Enum> allModes;
    PaletteNormalizationModeEnum::getAllEnums(allModes);
    for (std::vector<PaletteNormalizationModeEnum::Enum>::iterator allModesIter = allModes.begin();
         allModesIter != allModes.end();
         allModesIter++) {
        const PaletteNormalizationModeEnum::Enum normalMode = *allModesIter;
        m_normalizationModeComboBox->addItem(PaletteNormalizationModeEnum::toGuiName(normalMode),
                                                 (int)PaletteNormalizationModeEnum::toIntegerCode(normalMode));
    }
    
    m_normalizationModeComboBox->setToolTip(PaletteNormalizationModeEnum::getEnumToolTopInHTML());
    
    QGroupBox* groupBox = new QGroupBox("Data Normalization");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->addWidget(m_normalizationModeComboBox);
    groupBox->setSizePolicy(QSizePolicy::Fixed,
                            QSizePolicy::Fixed);
    
    return groupBox;
}

/**
 * Update the normalization control section.
 */
void
MapSettingsPaletteColorMappingWidget::updateNormalizationControlSection()
{
    m_normalizationModeComboBox->clear();
    
    std::vector<PaletteNormalizationModeEnum::Enum> validModes;
    
    if (this->caretMappableDataFile != NULL) {
        this->caretMappableDataFile->getPaletteNormalizationModesSupported(validModes);
        const int32_t numValidModes = static_cast<int32_t>(validModes.size());
        if (numValidModes > 0) {
            const PaletteNormalizationModeEnum::Enum selectedMode = this->caretMappableDataFile->getPaletteNormalizationMode();
            int selectedModeIndex = -1;
            
            /*
             * Loop through ALL modes so that the selections are always
             * in a consistent order.
             */
            std::vector<PaletteNormalizationModeEnum::Enum> allModes;
            PaletteNormalizationModeEnum::getAllEnums(allModes);
            for (std::vector<PaletteNormalizationModeEnum::Enum>::iterator allModesIter = allModes.begin();
                 allModesIter != allModes.end();
                 allModesIter++) {
                const PaletteNormalizationModeEnum::Enum normalMode = *allModesIter;
                if (std::find(validModes.begin(),
                              validModes.end(),
                              normalMode) != validModes.end()) {
                    if (selectedMode == normalMode) {
                        selectedModeIndex = m_normalizationModeComboBox->count();
                    }
                    m_normalizationModeComboBox->addItem(PaletteNormalizationModeEnum::toGuiName(normalMode),
                                                         (int)PaletteNormalizationModeEnum::toIntegerCode(normalMode));
                }
            }
            
            if (selectedModeIndex >= 0) {
                m_normalizationModeComboBox->setCurrentIndex(selectedModeIndex);
            }
        }
    }
}

/**
 * Called when normalization combo box is changed by user.
 */
void
MapSettingsPaletteColorMappingWidget::normalizationModeComboBoxActivated(int)
{
    if (this->caretMappableDataFile != NULL) {
        
        const int selectedIndex = m_normalizationModeComboBox->currentIndex();
        if ((selectedIndex >= 0)
            && (selectedIndex < m_normalizationModeComboBox->count())) {
            const int32_t enumIntegerCode = m_normalizationModeComboBox->itemData(selectedIndex).toInt();
            bool validFlag = false;
            const PaletteNormalizationModeEnum::Enum mode = PaletteNormalizationModeEnum::fromIntegerCode(enumIntegerCode,
                                                                                                          &validFlag);
            if (validFlag) {
                if (mode != this->caretMappableDataFile->getPaletteNormalizationMode()) {
                    bool doItFlag = true;
                    if (mode == PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA) {
                        doItFlag = WuQMessageBox::warningLargeFileSizeOkCancel(m_normalizationModeComboBox,
                                                                               this->caretMappableDataFile);
                    }
                    
                    if (doItFlag) {
                        CursorDisplayScoped cursor;
                        cursor.showCursor(Qt::WaitCursor);
                        
                        this->caretMappableDataFile->setPaletteNormalizationMode(mode);
                        this->updateEditorInternal(this->caretMappableDataFile,
                                                   this->mapFileIndex);
                        m_paletteOptionsWidget->applyOptions();
                    }
                    else {
                        this->updateNormalizationControlSection();
                    }
                }
            }
        }
    }
}

/**
 * Called when pre-color all maps checkbox is toggled
 * @param checked
 *    New checked status
 */
void
MapSettingsPaletteColorMappingWidget::preColorAllMapsClicked(bool checked)
{
    if (this->caretMappableDataFile != NULL) {
        this->caretMappableDataFile->setPreColorAllMaps(checked);
        
        if (checked) {
            if (WuQMessageBoxTwo::question(this,
                                           "Color Maps",
                                           "Color all maps now?\n"
                                           "(Note: Maps will be colored when scene is loaded)") == WuQMessageBoxTwo::StandardButton::Yes) {
                CursorDisplayScoped cursor;
                cursor.showWaitCursor();
                this->caretMappableDataFile->updateScalarColoringForAllMaps();
            }
        }
    }
}
