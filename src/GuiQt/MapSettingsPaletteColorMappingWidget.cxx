
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
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QToolButton>

#define __MAP_SETTINGS_PALETTE_COLOR_MAPPING_WIDGET_DECLARE__
#include "MapSettingsPaletteColorMappingWidget.h"
#undef __MAP_SETTINGS_PALETTE_COLOR_MAPPING_WIDGET_DECLARE__

#include "Brain.h"
#include "CaretMappableDataFile.h"
#include "EnumComboBoxTemplate.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventManager.h"
#include "FastStatistics.h"
#include "GuiManager.h"
#include "Histogram.h"
#include "NodeAndVoxelColoring.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
#include "VolumeFile.h"
#include "WuQDataEntryDialog.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQDoubleSlider.h"
#include "WuQFactory.h"
#include "WuQtUtilities.h"

#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_histogram.h"
#include "qwt_plot_intervalcurve.h"
#include "qwt_plot_layout.h"
#include "PlotMagnifier.h"
#include "PlotPanner.h"

using namespace caret;


    
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
     */
    this->setContextMenuPolicy(Qt::NoContextMenu);
    
    this->isHistogramColored = true;
    
    this->caretMappableDataFile = NULL;
    this->mapFileIndex = -1;
    
    this->paletteWidgetGroup = new WuQWidgetObjectGroup(this);
    this->thresholdWidgetGroup = new WuQWidgetObjectGroup(this);
    
    this->paletteColorMapping = NULL;
    
    QWidget* histogramWidget = this->createHistogramSection();
    QWidget* histogramControlWidget = this->createHistogramControlSection();
    
    QWidget* dataOptionsWidget = this->createDataOptionsSection();
    
    QWidget* paletteWidget = this->createPaletteSection();

    QWidget* thresholdWidget = this->createThresholdSection();
    
    QWidget* leftWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    this->setLayoutSpacingAndMargins(leftLayout);
    leftLayout->addWidget(thresholdWidget);
    leftLayout->addWidget(paletteWidget);
    leftLayout->addStretch();
    
    QVBoxLayout* optionsLayout = new QVBoxLayout();
    optionsLayout->addWidget(dataOptionsWidget);
    optionsLayout->addStretch(10000);
    
    QWidget* bottomRightWidget = new QWidget();
    QHBoxLayout* bottomRightLayout = new QHBoxLayout(bottomRightWidget);
    this->setLayoutSpacingAndMargins(bottomRightLayout);
    bottomRightLayout->addWidget(histogramControlWidget);   
    bottomRightLayout->addLayout(optionsLayout);
    bottomRightWidget->setFixedSize(bottomRightWidget->sizeHint());
    
    QWidget* rightWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    this->setLayoutSpacingAndMargins(rightLayout);
    rightLayout->addWidget(histogramWidget, 100);
    rightLayout->addWidget(bottomRightWidget, 0);
    rightLayout->addStretch();
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    this->setLayoutSpacingAndMargins(layout);
    layout->addWidget(leftWidget, 0);
    layout->addWidget(rightWidget, 100);
    
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
    this->updateEditor(this->caretMappableDataFile, 
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
    
    this->updateEditor(this->caretMappableDataFile, 
                       this->mapFileIndex);
    
    this->applySelections();
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
                
                float maxValue = std::numeric_limits<float>::max();
                float minValue = -maxValue;
                
                bool enableSliders = true;
                
                switch (thresholdRangeMode) {
                    case PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_FILE:
                        this->caretMappableDataFile->getDataRangeFromAllMaps(minValue,
                                                                             maxValue);
                        break;
                    case PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_MAP:
                    {
                        const FastStatistics* stats = this->caretMappableDataFile->getMapFastStatistics(this->mapFileIndex);
                        if (stats != NULL) {
                            minValue = stats->getMin();
                            maxValue = stats->getMax();
                        }
                    }
                        break;
                    case PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_UNLIMITED:
                        enableSliders = false;
                        break;
                }
                
                this->thresholdLowSlider->setRange(minValue,
                                                   maxValue);
                this->thresholdHighSlider->setRange(minValue,
                                                    maxValue);
                this->thresholdLowSpinBox->setRange(minValue,
                                                    maxValue);
                this->thresholdHighSpinBox->setRange(minValue,
                                                     maxValue);
                
                this->thresholdLowSlider->getWidget()->setEnabled(enableSliders);
                this->thresholdHighSlider->getWidget()->setEnabled(enableSliders);
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
    
    this->updateEditor(this->caretMappableDataFile, 
                       this->mapFileIndex);
    
}

/**
 * Called when low value spin box changed.
 * @param thresholdLow
 *    New value.
 */
void 
MapSettingsPaletteColorMappingWidget::thresholdLowSpinBoxValueChanged(double thresholdLow)
{
    float thresholdHigh = this->thresholdHighSpinBox->value();
    if (thresholdLow > thresholdHigh) {
        thresholdHigh = thresholdLow;
    }
    
    /*
     * Update OTHER threshold controls with new value.
     */
    this->thresholdHighSpinBox->blockSignals(true);
    this->thresholdHighSpinBox->setValue(thresholdHigh);
    this->thresholdHighSpinBox->blockSignals(false);
    
    this->thresholdHighSlider->blockSignals(true);
    this->thresholdHighSlider->setValue(thresholdHigh);
    this->thresholdHighSlider->blockSignals(false);
    
//    this->thresholdLowSpinBox->blockSignals(true);
//    this->thresholdLowSpinBox->setValue(thresholdLow);
//    this->thresholdLowSpinBox->blockSignals(false);
    
    this->thresholdLowSlider->blockSignals(true);
    this->thresholdLowSlider->setValue(thresholdLow);
    this->thresholdLowSlider->blockSignals(false);
    
    this->applySelections();
}

/**
 * Called when high value spin box changed.
 * @param thresholdHigh
 *    New value.
 */
void 
MapSettingsPaletteColorMappingWidget::thresholdHighSpinBoxValueChanged(double thresholdHigh)
{
    float thresholdLow = this->thresholdLowSpinBox->value();
    if (thresholdHigh < thresholdLow) {
        thresholdLow = thresholdHigh;
    }
    //    this->thresholdHighSpinBox->blockSignals(true);
    //    this->thresholdHighSpinBox->setValue(thresholdHigh);
    //    this->thresholdHighSpinBox->blockSignals(false);
    
    this->thresholdHighSlider->blockSignals(true);
    this->thresholdHighSlider->setValue(thresholdHigh);
    this->thresholdHighSlider->blockSignals(false);
    
    this->thresholdLowSpinBox->blockSignals(true);
    this->thresholdLowSpinBox->setValue(thresholdLow);
    this->thresholdLowSpinBox->blockSignals(false);
    
    this->thresholdLowSlider->blockSignals(true);
    this->thresholdLowSlider->setValue(thresholdLow);
    this->thresholdLowSlider->blockSignals(false);
    
    this->applySelections();
}

/**
 * Called when low value slider changed.
 * @param thresholdLow
 *    New value.
 */
void 
MapSettingsPaletteColorMappingWidget::thresholdLowSliderValueChanged(double thresholdLow)
{
    float thresholdHigh = this->thresholdHighSpinBox->value();
    if (thresholdLow > thresholdHigh) {
        thresholdHigh = thresholdLow;
    }
    
    this->thresholdHighSpinBox->blockSignals(true);
    this->thresholdHighSpinBox->setValue(thresholdHigh);
    this->thresholdHighSpinBox->blockSignals(false);
    
    this->thresholdHighSlider->blockSignals(true);
    this->thresholdHighSlider->setValue(thresholdHigh);
    this->thresholdHighSlider->blockSignals(false);
    
    this->thresholdLowSpinBox->blockSignals(true);
    this->thresholdLowSpinBox->setValue(thresholdLow);
    this->thresholdLowSpinBox->blockSignals(false);
    
//    this->thresholdLowSlider->blockSignals(true);
//    this->thresholdLowSlider->setValue(thresholdLow);
//    this->thresholdLowSlider->blockSignals(false);
    
    this->applySelections();
}

/**
 * Called when high value slider changed.
 * @param thresholdHigh
 *    New value.
 */
void 
MapSettingsPaletteColorMappingWidget::thresholdHighSliderValueChanged(double thresholdHigh)
{
    float thresholdLow = this->thresholdLowSpinBox->value();
    if (thresholdHigh < thresholdLow) {
        thresholdLow = thresholdHigh;
    }
    
    this->thresholdHighSpinBox->blockSignals(true);
    this->thresholdHighSpinBox->setValue(thresholdHigh);
    this->thresholdHighSpinBox->blockSignals(false);
    
//    this->thresholdHighSlider->blockSignals(true);
//    this->thresholdHighSlider->setValue(thresholdHigh);
//    this->thresholdHighSlider->blockSignals(false);
    
    this->thresholdLowSpinBox->blockSignals(true);
    this->thresholdLowSpinBox->setValue(thresholdLow);
    this->thresholdLowSpinBox->blockSignals(false);
    
    this->thresholdLowSlider->blockSignals(true);
    this->thresholdLowSlider->setValue(thresholdLow);
    this->thresholdLowSlider->blockSignals(false);
    
    this->applySelections();
}

/**
 * Create the threshold section of the dialog.
 * @return
 *   The threshold section.
 */
QWidget* 
MapSettingsPaletteColorMappingWidget::createThresholdSection()
{
    QLabel* thresholdTypeLabel = new QLabel("Type");
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
                                      
    QLabel* thresholdLowLabel = new QLabel("Low");
    QLabel* thresholdHighLabel = new QLabel("High");
    const float thresholdMinimum = -std::numeric_limits<float>::max();
    const float thresholdMaximum =  std::numeric_limits<float>::max();
    
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
    
    const int spinBoxWidth = 80.0;
    this->thresholdLowSpinBox =
       WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(thresholdMinimum,
                                                                   thresholdMaximum,
                                                                   1.0,
                                                                   3,
                                                                   this,
                                                                   SLOT(thresholdLowSpinBoxValueChanged(double)));
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdLowSpinBox,
                                          "Adjust the low threshold value");
    this->thresholdWidgetGroup->add(this->thresholdLowSpinBox);
    this->thresholdLowSpinBox->setFixedWidth(spinBoxWidth);

    this->thresholdHighSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(thresholdMinimum,
                                                                   thresholdMaximum,
                                                                   1.0,
                                                                   3,
                                                                   this,
                                                                   SLOT(thresholdHighSpinBoxValueChanged(double)));
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdHighSpinBox,
                                          "Adjust the high threshold value");
    this->thresholdWidgetGroup->add(this->thresholdHighSpinBox);
    this->thresholdHighSpinBox->setFixedWidth(spinBoxWidth);

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
    QObject::connect(thresholdShowButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(applyAndUpdate()));
    
    QWidget* thresholdAdjustmentWidget = new QWidget();
    QGridLayout* thresholdAdjustmentLayout = new QGridLayout(thresholdAdjustmentWidget);
    this->setLayoutSpacingAndMargins(thresholdAdjustmentLayout);
    thresholdAdjustmentLayout->setColumnStretch(0, 0);
    thresholdAdjustmentLayout->setColumnStretch(1, 100);
    thresholdAdjustmentLayout->setColumnStretch(2, 0);
    thresholdAdjustmentLayout->addWidget(thresholdHighLabel, 0, 0);
    thresholdAdjustmentLayout->addWidget(this->thresholdHighSlider->getWidget(), 0, 1);
    thresholdAdjustmentLayout->addWidget(this->thresholdHighSpinBox, 0, 2);
    thresholdAdjustmentLayout->addWidget(thresholdLowLabel, 1, 0);
    thresholdAdjustmentLayout->addWidget(this->thresholdLowSlider->getWidget(), 1, 1);
    thresholdAdjustmentLayout->addWidget(this->thresholdLowSpinBox, 1, 2);
    thresholdAdjustmentLayout->addWidget(this->thresholdShowInsideRadioButton, 2, 0, 1, 3, Qt::AlignLeft);
    thresholdAdjustmentLayout->addWidget(this->thresholdShowOutsideRadioButton, 3, 0, 1, 3, Qt::AlignLeft);
    thresholdAdjustmentWidget->setFixedHeight(thresholdAdjustmentWidget->sizeHint().height());
    
    QWidget* topWidget = new QWidget();
    QHBoxLayout* topLayout = new QHBoxLayout(topWidget);
    this->setLayoutSpacingAndMargins(topLayout);
    topLayout->addWidget(thresholdTypeLabel);
    topLayout->addWidget(this->thresholdTypeComboBox);
    topLayout->addWidget(thresholdRangeLabel);
    topLayout->addWidget(this->thresholdRangeModeComboBox->getWidget());
    topLayout->addStretch();
    
    QGroupBox* thresholdGroupBox = new QGroupBox("Threshold");
    QVBoxLayout* layout = new QVBoxLayout(thresholdGroupBox);
    this->setLayoutSpacingAndMargins(layout);
    layout->addWidget(topWidget, 0, Qt::AlignLeft);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addWidget(thresholdAdjustmentWidget);
    thresholdGroupBox->setFixedHeight(thresholdGroupBox->sizeHint().height());
    
    this->thresholdAdjustmentWidgetGroup = new WuQWidgetObjectGroup(this);
    this->thresholdAdjustmentWidgetGroup->add(thresholdLowLabel);
    this->thresholdAdjustmentWidgetGroup->add(thresholdHighLabel);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdLowSlider);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdHighSlider);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdLowSpinBox);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdHighSpinBox);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdShowInsideRadioButton);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdShowOutsideRadioButton);
    this->thresholdAdjustmentWidgetGroup->add(thresholdRangeLabel);
    this->thresholdAdjustmentWidgetGroup->add(this->thresholdRangeModeComboBox->getWidget());
    
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
 * Called when a histogram control is changed.
 */
void 
MapSettingsPaletteColorMappingWidget::histogramControlChanged()
{
    this->updateWidget();
}

/**
 * Create the statistics section
 * @return the statistics section widget.
 */
QWidget* 
MapSettingsPaletteColorMappingWidget::createHistogramControlSection()
{
    /*
     * Control section
     */
    this->histogramAllRadioButton = new QRadioButton("All");
    WuQtUtilities::setToolTipAndStatusTip(this->histogramAllRadioButton, 
                                          "Displays all map data in the histogram");
    this->histogramMatchPaletteRadioButton = new QRadioButton("Match\nPalette");
    WuQtUtilities::setToolTipAndStatusTip(this->histogramMatchPaletteRadioButton, 
                                          "Use the palette mapping selections");
    
    this->histogramAllRadioButton->setChecked(true);
    
    QButtonGroup* buttGroup = new QButtonGroup(this);
    buttGroup->addButton(this->histogramAllRadioButton);
    buttGroup->addButton(this->histogramMatchPaletteRadioButton);
    QObject::connect(buttGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(histogramControlChanged()));
    
    this->histogramUsePaletteColors = new QCheckBox("Colorize");
    WuQtUtilities::setToolTipAndStatusTip(this->histogramUsePaletteColors, 
                                          "If checked, histogram colors match colors mapped to data");
    this->histogramUsePaletteColors->setChecked(true);
    QObject::connect(this->histogramUsePaletteColors, SIGNAL(toggled(bool)),
                     this, SLOT(histogramControlChanged()));
    
    QWidget* controlWidget = new QWidget();
    QGridLayout* controlLayout = new QGridLayout(controlWidget);
    this->setLayoutSpacingAndMargins(controlLayout);
    controlLayout->addWidget(this->histogramAllRadioButton);
    controlLayout->addWidget(this->histogramMatchPaletteRadioButton);
    controlLayout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    controlLayout->addWidget(this->histogramUsePaletteColors);
    controlWidget->setFixedSize(controlWidget->sizeHint());
    
    /*
     * Statistics
     */
    const AString blankText("                ");
    this->statisticsMinimumValueLabel = new QLabel(blankText);
    this->statisticsMaximumValueLabel = new QLabel(blankText);
    this->statisticsMeanValueLabel = new QLabel(blankText);
    this->statisticsStandardDeviationLabel = new QLabel(blankText);
    
    QWidget* statisticsWidget = new QWidget();
    QGridLayout* statisticsLayout = new QGridLayout(statisticsWidget);
    this->setLayoutSpacingAndMargins(statisticsLayout);
    statisticsLayout->addWidget(new QLabel("Mean"), 0, 0);
    statisticsLayout->addWidget(this->statisticsMeanValueLabel, 0, 1);
    statisticsLayout->addWidget(new QLabel("Std Dev"), 1, 0);
    statisticsLayout->addWidget(this->statisticsStandardDeviationLabel, 1, 1);
    statisticsLayout->addWidget(new QLabel("Max"), 2, 0);
    statisticsLayout->addWidget(this->statisticsMaximumValueLabel, 2, 1);
    statisticsLayout->addWidget(new QLabel("Min"), 3, 0);
    statisticsLayout->addWidget(this->statisticsMinimumValueLabel, 3, 1);
    statisticsWidget->setFixedHeight(statisticsWidget->sizeHint().height());
    
    
    QGroupBox* groupBox = new QGroupBox("Histogram");
    QHBoxLayout* layout = new QHBoxLayout(groupBox);
    this->setLayoutSpacingAndMargins(layout);
    layout->addWidget(controlWidget);
    layout->addWidget(WuQtUtilities::createVerticalLineWidget());
    layout->addWidget(statisticsWidget);
    groupBox->setFixedHeight(groupBox->sizeHint().height());
    
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
    this->thresholdPlot = new QwtPlot();
    this->thresholdPlot->plotLayout()->setAlignCanvasToScales(true);
    
    /*
     * Allow zooming
     */
    PlotMagnifier* magnifier = new PlotMagnifier(this->thresholdPlot->canvas());
    magnifier->setAxisEnabled(QwtPlot::yLeft, true);
    magnifier->setAxisEnabled(QwtPlot::yRight, true);
    
    /*
     * Allow panning
     */
    (void)new PlotPanner(this->thresholdPlot->canvas());
    
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
    this->paletteNameComboBox = new QComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->paletteNameComboBox, 
                                          "Select palette for coloring map data");
    this->paletteWidgetGroup->add(this->paletteNameComboBox);
    QObject::connect(this->paletteNameComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(applySelections()));
    /*
     * Interpolate Colors
     */
    this->interpolateColorsCheckBox = new QCheckBox("Interpolate Colors");
    WuQtUtilities::setToolTipAndStatusTip(this->interpolateColorsCheckBox, 
                                          "Smooth colors for data between palette colors");
    this->paletteWidgetGroup->add(this->interpolateColorsCheckBox);
    QObject::connect(this->interpolateColorsCheckBox, SIGNAL(toggled(bool)), 
                     this, SLOT(applySelections()));
        
    QWidget* paletteSelectionWidget = new QWidget();
    QVBoxLayout* paletteSelectionLayout = new QVBoxLayout(paletteSelectionWidget);
    this->setLayoutSpacingAndMargins(paletteSelectionLayout);
    paletteSelectionLayout->addWidget(this->paletteNameComboBox);
    paletteSelectionLayout->addWidget(this->interpolateColorsCheckBox);
    paletteSelectionWidget->setFixedHeight(paletteSelectionWidget->sizeHint().height());
    
    
    /*
     * Color Mapping
     */
    this->scaleAutoRadioButton = new QRadioButton("Full"); //Auto Scale");
    this->scaleAutoPercentageRadioButton = new QRadioButton("Percent"); //"Auto Scale Percentage");
    this->scaleFixedRadioButton = new QRadioButton("Fixed"); //"Fixed Scale");
    
    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoRadioButton, 
                                          "Map (most negative, zero, most positive) data values to (-1, 0, 1) in palette");
    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentageRadioButton, 
                                          "Map percentiles (NOT percentages) of (most neg, least neg, least pos, most pos) data values to (-1, 0, 0, 1) in palette");
    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedRadioButton, 
                                          "Map specified values (most neg, least neg, least pos, most pos) to (-1, 0, 0, 1) in palette");
    QButtonGroup* scaleButtonGroup = new QButtonGroup(this);
    this->paletteWidgetGroup->add(scaleButtonGroup);
    scaleButtonGroup->addButton(this->scaleAutoRadioButton);
    scaleButtonGroup->addButton(this->scaleAutoPercentageRadioButton);
    scaleButtonGroup->addButton(this->scaleFixedRadioButton);
    QObject::connect(scaleButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(applyAndUpdate()));
    
    /*
     * Spin box width
     */
    const int percentSpinBoxWidth = 75;
    const int fixedSpinBoxWidth   = 100;  // fixed may have much larger data values
    /*
     * Percentage mapping 
     */
    this->scaleAutoPercentageNegativeMaximumSpinBox =
       WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   100.0,
                                                                   1.0,
                                                                   2,
                                                                   this,
                                                                   SLOT(applySelections()));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentageNegativeMaximumSpinBox,
                                          "Map percentile (NOT percentage) most negative value to -1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoPercentageNegativeMaximumSpinBox);
    this->scaleAutoPercentageNegativeMaximumSpinBox->setFixedWidth(percentSpinBoxWidth);
    
    this->scaleAutoPercentageNegativeMinimumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   100.0,
                                                                   1.0,
                                                                   2,
                                                                   this,
                                                                   SLOT(applySelections()));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentageNegativeMinimumSpinBox,
                                          "Map percentile (NOT percentage) least negative value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoPercentageNegativeMinimumSpinBox);
    this->scaleAutoPercentageNegativeMinimumSpinBox->setFixedWidth(percentSpinBoxWidth);
    
    this->scaleAutoPercentagePositiveMinimumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   100.0,
                                                                   1.0,
                                                                   2,
                                                                   this,
                                                                   SLOT(applySelections()));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentagePositiveMinimumSpinBox,
                                          "Map percentile (NOT percentage) least positive value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoPercentagePositiveMinimumSpinBox);
    this->scaleAutoPercentagePositiveMinimumSpinBox->setFixedWidth(percentSpinBoxWidth);
    
    this->scaleAutoPercentagePositiveMaximumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   100.0,
                                                                   1.0,
                                                                   2,
                                                                   this,
                                                                   SLOT(applySelections()));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentagePositiveMaximumSpinBox,
                                          "Map percentile (NOT percentage) most positive value to 1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoPercentagePositiveMaximumSpinBox);
    this->scaleAutoPercentagePositiveMaximumSpinBox->setFixedWidth(percentSpinBoxWidth);
    
    /*
     * Fixed mapping
     */
    this->scaleFixedNegativeMaximumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(-std::numeric_limits<float>::max(),
                                                                   0.0,
                                                                   1.0,
                                                                   3,
                                                                   this,
                                                                   SLOT(applySelections()));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedNegativeMaximumSpinBox,
                                          "Map this value to -1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleFixedNegativeMaximumSpinBox);
    this->scaleFixedNegativeMaximumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    
    this->scaleFixedNegativeMinimumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(-std::numeric_limits<float>::max(),
                                                                   0.0,
                                                                   1.0,
                                                                   3,
                                                                   this,
                                                                   SLOT(applySelections()));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedNegativeMinimumSpinBox,
                                          "Map this value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleFixedNegativeMinimumSpinBox);
    this->scaleFixedNegativeMinimumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    
    this->scaleFixedPositiveMinimumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   std::numeric_limits<float>::max(),
                                                                   1.0,
                                                                   3,
                                                                   this,
                                                                   SLOT(applySelections()));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedPositiveMinimumSpinBox,
                                          "Map this value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleFixedPositiveMinimumSpinBox);
    this->scaleFixedPositiveMinimumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    
    this->scaleFixedPositiveMaximumSpinBox =
    WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                   std::numeric_limits<float>::max(),
                                                                   1.0,
                                                                   3,
                                                                   this,
                                                                   SLOT(applySelections()));

    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedPositiveMaximumSpinBox,
                                          "Map this value to 1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleFixedPositiveMaximumSpinBox);
    this->scaleFixedPositiveMaximumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    
    QWidget* colorMappingWidget = new QWidget();
    QGridLayout* colorMappingLayout = new QGridLayout(colorMappingWidget);
    this->setLayoutSpacingAndMargins(colorMappingLayout);
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
    colorMappingWidget->setFixedSize(colorMappingWidget->sizeHint());


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
                                          + AString::number(NodeAndVoxelColoring::SMALL_NEGATIVE, 'f', 6)
                                          + ", "
                                          + AString::number(NodeAndVoxelColoring::SMALL_POSITIVE, 'f', 6)
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
    //displayModeWidget->setFixedSize(displayModeWidget->sizeHint());

    
    /*
     * Layout widgets
     */
    QGroupBox* paletteGroupBox = new QGroupBox("Palette");
    QVBoxLayout* paletteLayout = new QVBoxLayout(paletteGroupBox);
    this->setLayoutSpacingAndMargins(paletteLayout);
    paletteLayout->addWidget(paletteSelectionWidget);
    paletteLayout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    paletteLayout->addWidget(colorMappingWidget);
    paletteLayout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    paletteLayout->addWidget(displayModeWidget);
    paletteGroupBox->setFixedHeight(paletteGroupBox->sizeHint().height());
    
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
    this->thresholdWidgetGroup->blockAllSignals(true);
    
    const AString title =
    this->caretMappableDataFile->getFileNameNoPath()
    + ": "
    + this->caretMappableDataFile->getMapName(this->mapFileIndex);
    this->setWindowTitle(title);
    
    this->paletteNameComboBox->clear();

    
    this->paletteColorMapping = this->caretMappableDataFile->getMapPaletteColorMapping(this->mapFileIndex); 
    
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
        
        bool isPercentageSpinBoxesEnabled = false;
        bool isFixedSpinBoxesEnabled = false;
        switch (this->paletteColorMapping->getScaleMode()) {
            case PaletteScaleModeEnum::MODE_AUTO_SCALE:
                this->scaleAutoRadioButton->setChecked(true);
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
        
        const int32_t numTypes = this->thresholdTypeComboBox->count();
        for (int32_t i = 0; i < numTypes; i++) {
            const int value = this->thresholdTypeComboBox->itemData(i).toInt();
            if (value == static_cast<int>(this->paletteColorMapping->getThresholdType())) {
                this->thresholdTypeComboBox->setCurrentIndex(i);
                break;
            }
        }
        
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

        float minValue  = 0.0;
        float maxValue  = 0.0;
        const FastStatistics* statistics = this->caretMappableDataFile->getMapFastStatistics(this->mapFileIndex);
        if (statistics != NULL) {
            minValue  = 0.0;
            maxValue  = 0.0;
        }
        
        this->thresholdLowSlider->setValue(lowValue);
        
        this->thresholdHighSlider->setValue(highValue);
        
        this->thresholdLowSpinBox->setValue(lowValue);
        
        this->thresholdHighSpinBox->setValue(highValue);
        
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
    
    this->updateHistogramPlot();
    
    this->paletteWidgetGroup->blockAllSignals(false);
    this->thresholdWidgetGroup->blockAllSignals(false);
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
    if (this->histogramAllRadioButton->isChecked()) {
        float dummy;
        statisticsForAll->getNonzeroRanges(mostNeg, dummy, dummy, mostPos);
    }
    else if (this->histogramMatchPaletteRadioButton->isChecked()) {
        matchFlag = true;
        switch (this->paletteColorMapping->getScaleMode()) {
            case PaletteScaleModeEnum::MODE_AUTO_SCALE:
                //mostPos  = std::numeric_limits<float>::max();
                mostPos  = statisticsForAll->getMax();
                leastPos = 0.0;
                leastNeg = 0.0;
                //mostNeg  = -std::numeric_limits<float>::max();
                mostNeg  = statisticsForAll->getMin();
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
    else {
        CaretAssert(0);
    }
    
    /*
     * Remove data that is not displayed
     */
    bool isZeroIncluded = true;
    const Histogram* ret;
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
        ret = this->caretMappableDataFile->getMapHistogram(this->mapFileIndex, 
                                                         mostPos, 
                                                         leastPos, 
                                                         leastNeg, 
                                                         mostNeg, 
                                                         isZeroIncluded);
    } else {
        ret = caretMappableDataFile->getMapHistogram(this->mapFileIndex);
    }
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
    
    const FastStatistics* fastStatistics = caretMappableDataFile->getMapFastStatistics(mapFileIndex);
    if ((this->paletteColorMapping != NULL)
        && (fastStatistics != NULL)) {
        PaletteFile* paletteFile = GuiManager::get()->getBrain()->getPaletteFile();
        
        /*
         * Data values table
         */
        const float statsMean   = fastStatistics->getMean();
        const float statsStdDev = fastStatistics->getSampleStdDev();
        const float statsMin    = fastStatistics->getMin();
        const float statsMax    = fastStatistics->getMax();
        this->statisticsMeanValueLabel->setText(QString::number(statsMean, 'f', 4));
        this->statisticsStandardDeviationLabel->setText(QString::number(statsStdDev, 'f', 4));
        this->statisticsMaximumValueLabel->setText(QString::number(statsMax, 'f', 4));
        this->statisticsMinimumValueLabel->setText(QString::number(statsMin, 'f', 4));
        
        /*
         * Get data for this histogram.
         */
        const Histogram* myHist = getHistogram(fastStatistics);
        //const int64_t* histogram = const_cast<int64_t*>(statistics->getHistogram());
        float minValue, maxValue;
        myHist->getRange(minValue, maxValue);
        const std::vector<float>& displayData = myHist->getHistogramDisplay();
        const int64_t numHistogramValues = (int64_t)(displayData.size());
        
        /*
         * Width of each 'bar' in the histogram
         */
        float step = 1.0;
        if (numHistogramValues > 1) {
            step = ((maxValue - minValue)
                    / numHistogramValues);
        }
        
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
            
            const Palette* palette = paletteFile->getPaletteByName(this->paletteColorMapping->getSelectedPaletteName());
            if (this->histogramUsePaletteColors->isChecked()
                && (palette != NULL)) {
                NodeAndVoxelColoring::colorScalarsWithPalette(fastStatistics, 
                                                              paletteColorMapping, 
                                                              palette, 
                                                              dataValues, 
                                                              dataValues, 
                                                              numDataValues, 
                                                              dataRGBA,
                                                              true); // ignore thresholding
            }
            else {
                for (int64_t i = 0; i < numDataValues; i++) {
                    const int64_t i4 = i * 4;
                    dataRGBA[i4]   = 1.0;
                    dataRGBA[i4+1] = 0.0;
                    dataRGBA[i4+2] = 0.0;
                    dataRGBA[i4+3] = 1.0;
                }
            }
        }
        
        const bool displayZeros = paletteColorMapping->isDisplayZeroDataFlag();
        
        float z = 0.0;
        float maxDataFrequency = 0.0;
        for (int64_t ix = 0; ix < numHistogramValues; ix++) {
            QColor color;
            const int64_t ix4 = ix * 4;
            color.setRedF(dataRGBA[ix4]);
            color.setGreenF(dataRGBA[ix4+1]);
            color.setBlueF(dataRGBA[ix4+2]);
            color.setAlphaF(1.0);
            
            const float startValue = dataValues[ix];
            const float stopValue  = dataValues[ix + 1];
            float dataFrequency = displayData[ix];
            
            bool displayIt = true;
            
            if (displayZeros == false) {
                if ((startValue <= 0.0) && (stopValue >= 0.0)) {
                    dataFrequency = 0.0;
                    displayIt = false;
                }
            }
            
            if (dataFrequency > maxDataFrequency) {
                maxDataFrequency = dataFrequency;
            }
            
            /*
             * If color is not displayed ('none' or thresholded), 
             * set its frequncey value to a small value so that the plot
             * retains its shape and color is still slightly visible
             */
            //Qt::BrushStyle brushStyle = Qt::SolidPattern;
            if (dataRGBA[ix4+3] <= 0.0) {
                displayIt = false;
            }
            
            if (displayIt == false) {
                color.setAlpha(0);
            }
            
            QVector<QPointF> samples;
            samples.push_back(QPointF(startValue, dataFrequency));
            samples.push_back(QPointF(stopValue, dataFrequency));
            //samples.push_back(QPointF(startValue, 0));
            //samples.push_back(QPointF(stopValue, 0));
            //samples.push_back(QPointF(stopValue, dataFrequency));
            //samples.push_back(QPointF(startValue, dataFrequency));
            
            QwtPlotCurve* curve = new QwtPlotCurve();
            curve->setRenderHint(QwtPlotItem::RenderAntialiased);
            curve->setVisible(true);
            curve->setStyle(QwtPlotCurve::Steps);
            
            curve->setBrush(QBrush(color)); //, brushStyle));
            curve->setPen(QPen(color));
            curve->setSamples(samples);
            
            curve->attach(this->thresholdPlot);
            
            if (ix == 0) {
                z = curve->z();
            }
        }
        
        z = z - 1;
        
        if ((numHistogramValues > 2) 
            && (this->paletteColorMapping->getThresholdType() != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF)) {
            float threshMinValue = this->paletteColorMapping->getThresholdNormalMinimum();
            float threshMaxValue = this->paletteColorMapping->getThresholdNormalMaximum();
            
            maxDataFrequency *= 1.05;
            
            switch (this->paletteColorMapping->getThresholdTest()) {
                case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
                {
                    const float plotMinValue = this->thresholdPlot->axisScaleDiv(QwtPlot::xBottom)->lowerBound();
                    const float plotMaxValue = this->thresholdPlot->axisScaleDiv(QwtPlot::xBottom)->upperBound();
                    
                    /* 
                     * Draw shaded region to left of minimum threshold
                     */
                    QVector<QPointF> minSamples;
                    //minSamples.push_back(QPointF(dataValues[0], maxDataFrequency));
                    minSamples.push_back(QPointF(plotMinValue, maxDataFrequency));
                    minSamples.push_back(QPointF(threshMinValue, maxDataFrequency));
                    
                    QwtPlotCurve* minBox = new QwtPlotCurve();
                    minBox->setRenderHint(QwtPlotItem::RenderAntialiased);
                    minBox->setVisible(true);
                    minBox->setStyle(QwtPlotCurve::Dots);
                    
                    QColor minColor(100, 100, 255, 160);
                    minBox->setBrush(QBrush(minColor, Qt::Dense4Pattern));
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
                    //maxSamples.push_back(QPointF(dataValues[numHistogramValues - 1], maxDataFrequency));
                    
                    QwtPlotCurve* maxBox = new QwtPlotCurve();
                    maxBox->setRenderHint(QwtPlotItem::RenderAntialiased);
                    maxBox->setVisible(true);
                    maxBox->setStyle(QwtPlotCurve::Dots);
                    
                    QColor maxColor(100, 100, 255, 160);
                    maxBox->setBrush(QBrush(maxColor, Qt::Dense4Pattern));
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
                    //minBox->setStyle(QwtPlotCurve::Dots);
                    
                    QColor minColor(100, 100, 255, 160);
                    minBox->setBrush(QBrush(minColor)); //, Qt::Dense4Pattern));
                    minBox->setPen(QPen(minColor));
                    minBox->setSamples(minSamples);
                    
                    minBox->setZ(z);
                    
                    minBox->attach(this->thresholdPlot);
                }
                    break;
            }
            
            //z = z - 1;
            
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
    
    bool assignToAllMaps = false;
    if (this->applyAllMapsCheckBox->checkState() == Qt::Checked) {
        assignToAllMaps = true;
        const int numMaps = this->caretMappableDataFile->getNumberOfMaps();
        for (int32_t i = 0; i < numMaps; i++) {
            PaletteColorMapping* pcm = this->caretMappableDataFile->getMapPaletteColorMapping(i);
            if (pcm != this->paletteColorMapping) {
                pcm->copy(*this->paletteColorMapping);
            }
        }
    }
    
    /*
     * If no map attributes (one palette applies to all maps),
     * update coloring for all maps since all of the maps will
     * need their coloring updated.
     */
    if (this->caretMappableDataFile->hasMapAttributes() == false) {
        assignToAllMaps = true;
    }
    
    this->updateHistogramPlot();
    
    PaletteFile* paletteFile = GuiManager::get()->getBrain()->getPaletteFile();
    
    if (assignToAllMaps) {
        this->caretMappableDataFile->updateScalarColoringForAllMaps(paletteFile);
    }
    else {
        this->caretMappableDataFile->updateScalarColoringForMap(this->mapFileIndex,
                                                             paletteFile);
    }
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
 * Called when the state of the apply all maps checkbox is changed.
 * @param state
 *    New state of checkbox.
 */
void 
MapSettingsPaletteColorMappingWidget::applyAllMapsCheckBoxStateChanged(int /*state*/)
{
    //const bool checked = (state == Qt::Checked);
    this->applySelections();
}

/**
 * @return A widget containing the data options.
 */
QWidget*
MapSettingsPaletteColorMappingWidget::createDataOptionsSection()
{
    this->applyAllMapsCheckBox = new QCheckBox("Apply to All Maps");
    this->applyAllMapsCheckBox->setCheckState(Qt::Checked);
    QObject::connect(this->applyAllMapsCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(applyAndUpdate()));
    this->applyAllMapsCheckBox->setToolTip("If checked, settings are applied to all maps\n"
                                           "in the file containing the selected map");
    
    this->applyToMultipleFilesPushButton = new QPushButton("Apply to Files...");
    const QString tt("Displays a dialog that allows selection of data files to which the "
                     "palette settings are applied.");
    this->applyToMultipleFilesPushButton->setToolTip(WuQtUtilities::createWordWrappedToolTipText(tt));
    QObject::connect(this->applyToMultipleFilesPushButton, SIGNAL(clicked()),
                     this, SLOT(applyToMultipleFilesPushbuttonClicked()));
    
    QGroupBox* optionsGroupBox = new QGroupBox("Data Options");
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroupBox);
    this->setLayoutSpacingAndMargins(optionsLayout);
    optionsLayout->addWidget(this->applyAllMapsCheckBox);
    optionsLayout->addWidget(this->applyToMultipleFilesPushButton);
    optionsGroupBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                               QSizePolicy::Fixed));
    
    return optionsGroupBox;
}

/**
 * Allows user to select files to which palette settings are applied.
 */
void
MapSettingsPaletteColorMappingWidget::applyToMultipleFilesPushbuttonClicked()
{
    EventCaretMappableDataFilesGet mapFilesGet;
    EventManager::get()->sendEvent(mapFilesGet.getPointer());
    
    std::vector<CaretMappableDataFile*> mappableFiles;
    mapFilesGet.getAllFiles(mappableFiles);
    
    const QString filePointerPropertyName("filePointer");
    
    WuQDataEntryDialog ded("Apply Palettes Settings",
                           this->applyToMultipleFilesPushButton,
                           true);
    ded.setTextAtTop("Palette settings will be applied to all maps in the selected files.",
                     true);
    
    std::vector<QCheckBox*> mapFileCheckBoxes;
    for (std::vector<CaretMappableDataFile*>::iterator iter = mappableFiles.begin();
         iter != mappableFiles.end();
         iter++) {
        CaretMappableDataFile* cmdf = *iter;
        if (cmdf->isMappedWithPalette()) {
            QCheckBox* cb = ded.addCheckBox(cmdf->getFileNameNoPath());
            cb->setProperty(filePointerPropertyName.toAscii().constData(),
                            qVariantFromValue((void*)cmdf));
            mapFileCheckBoxes.push_back(cb);
            
            if (previousApplyPaletteToMapFilesSelected.find(cmdf) != previousApplyPaletteToMapFilesSelected.end()) {
                cb->setChecked(true);
            }
        }
    }
    
    previousApplyPaletteToMapFilesSelected.clear();
    
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        PaletteFile* paletteFile = GuiManager::get()->getBrain()->getPaletteFile();
        
        for (std::vector<QCheckBox*>::iterator iter = mapFileCheckBoxes.begin();
             iter != mapFileCheckBoxes.end();
             iter++) {
            QCheckBox* cb = *iter;
            if (cb->isChecked()) {
                void* pointer = cb->property(filePointerPropertyName.toAscii().constData()).value<void*>();
                CaretMappableDataFile* cmdf = (CaretMappableDataFile*)pointer;
                
                const int32_t numMaps = cmdf->getNumberOfMaps();
                for (int32_t iMap = 0; iMap < numMaps; iMap++) {
                    PaletteColorMapping* pcm = cmdf->getMapPaletteColorMapping(iMap);
                    if (pcm != this->paletteColorMapping) {
                        pcm->copy(*this->paletteColorMapping);
                    }
                }
                
                cmdf->updateScalarColoringForAllMaps(paletteFile);
                
                previousApplyPaletteToMapFilesSelected.insert(cmdf);
            }
        }
        
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}


