
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
#include "NodeAndVoxelColoring.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
#include "PaletteEnums.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQDoubleSlider.h"
#include "WuQtUtilities.h"

#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_histogram.h"

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

    this->isHistogramColored = true;
    
    this->caretMappableDataFile = NULL;
    this->mapFileIndex = -1;
    
    this->paletteWidgetGroup = new WuQWidgetObjectGroup(this);
    this->thresholdWidgetGroup = new WuQWidgetObjectGroup(this);
    
    /*
     * No apply button
     */
    this->setApplyButtonText("");
    
    this->paletteColorMapping = NULL;
    
    QWidget* histogramWidget = this->createHistogramSection();
    QWidget* histogramControlWidget = this->createHistogramControlSection();
    
    QWidget* paletteWidget = this->createPaletteSection();
    QWidget* paletteColorMappingWidget = this->createPaletteColorMappingSection();
    QWidget* paletteDisplayModeWidget = this->createPaletteDisplayModeSection();

    QWidget* statisticsWidget = this->createStatisticsSection();
    QWidget* thresholdWidget = this->createThresholdSection();
    
    QWidget* leftWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    this->setLayoutMargins(leftLayout);
    leftLayout->addWidget(thresholdWidget);
    leftLayout->addWidget(paletteWidget);
    leftLayout->addWidget(paletteColorMappingWidget);
    leftLayout->addWidget(paletteDisplayModeWidget);
    leftLayout->addStretch();
    
    QWidget* bottomRightWidget = new QWidget();
    QHBoxLayout* bottomRightLayout = new QHBoxLayout(bottomRightWidget);
    bottomRightLayout->addWidget(histogramControlWidget);    
    bottomRightLayout->addWidget(statisticsWidget);
    bottomRightWidget->setFixedSize(bottomRightWidget->sizeHint());
    
    QWidget* rightWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    this->setLayoutMargins(rightLayout);
    rightLayout->addWidget(histogramWidget, 100);
    rightLayout->addWidget(bottomRightWidget, 0);
    rightLayout->addStretch();
    
    QWidget* w = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(w);
    this->setLayoutMargins(layout);
    layout->addWidget(leftWidget, 0);
    layout->addWidget(rightWidget, 100);
    
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
    this->updateEditor(this->caretMappableDataFile, 
                       this->mapFileIndex);
}

/**
 * Called when the threshold type is changed.
 */
void 
MapScalarDataColorMappingEditorDialog::thresholdTypeChanged()
{
    if (this->thresholdTypeOffRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF);
    }
    else if (this->thresholdTypeOnRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL);
    }
    else if (this->thresholdTypeMappedRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED);
    }
    else if (this->thresholdTypeMappedAverageAreaRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA);
    }
    
    this->updateEditor(this->caretMappableDataFile, 
                       this->mapFileIndex);
}

/**
 * Called when the threshold control is changed
 * since the adjustment section needs to be 
 * updated.
 * @param button
 *    Button that was clicked.
 */
void 
MapScalarDataColorMappingEditorDialog::thresholdControlChanged()
{
    this->apply();
    
    this->updateEditor(this->caretMappableDataFile, 
                       this->mapFileIndex);
    
}

/**
 * Called when low value spin box changed.
 * @param d
 *    New value.
 */
void 
MapScalarDataColorMappingEditorDialog::thresholdLowSpinBoxValueChanged(double d)
{
    if (d > this->thresholdHighSpinBox->value()) {
        this->thresholdHighSpinBox->blockSignals(true);
        this->thresholdHighSpinBox->setValue(d);
        this->thresholdHighSpinBox->blockSignals(false);
        this->thresholdHighSlider->blockSignals(true);
        this->thresholdHighSlider->setValue(d);
        this->thresholdHighSlider->blockSignals(false);
    }
    this->thresholdLowSlider->blockSignals(true);
    this->thresholdLowSlider->setValue(d);
    this->thresholdLowSlider->blockSignals(false);
    
    this->thresholdControlChanged();
}

/**
 * Called when high value spin box changed.
 * @param d
 *    New value.
 */
void 
MapScalarDataColorMappingEditorDialog::thresholdHighSpinBoxValueChanged(double d)
{
    if (d < this->thresholdLowSpinBox->value()) {
        this->thresholdLowSpinBox->blockSignals(true);
        this->thresholdLowSpinBox->setValue(d);
        this->thresholdLowSpinBox->blockSignals(false);
        this->thresholdLowSlider->blockSignals(true);
        this->thresholdLowSlider->setValue(d);
        this->thresholdLowSlider->blockSignals(false);
    }
    this->thresholdHighSlider->blockSignals(true);
    this->thresholdHighSlider->setValue(d);
    this->thresholdHighSlider->blockSignals(false);
    
    this->thresholdControlChanged();
}

/**
 * Called when low value slider changed.
 * @param d
 *    New value.
 */
void 
MapScalarDataColorMappingEditorDialog::thresholdLowSliderValueChanged(double d)
{
    if (d > this->thresholdHighSlider->value()) {
        this->thresholdHighSlider->blockSignals(true);
        this->thresholdHighSlider->setValue(d);
        this->thresholdHighSlider->blockSignals(false);
        this->thresholdHighSpinBox->blockSignals(true);
        this->thresholdHighSpinBox->setValue(d);
        this->thresholdHighSpinBox->blockSignals(false);
    }
    this->thresholdLowSpinBox->blockSignals(true);
    this->thresholdLowSpinBox->setValue(d);
    this->thresholdLowSpinBox->blockSignals(false);
    
    this->thresholdControlChanged();
}

/**
 * Called when high value slider changed.
 * @param d
 *    New value.
 */
void 
MapScalarDataColorMappingEditorDialog::thresholdHighSliderValueChanged(double d)
{
    if (d < this->thresholdLowSlider->value()) {
        this->thresholdLowSlider->blockSignals(true);
        this->thresholdLowSlider->setValue(d);
        this->thresholdLowSlider->blockSignals(false);
        this->thresholdLowSpinBox->blockSignals(true);
        this->thresholdLowSpinBox->setValue(d);
        this->thresholdLowSpinBox->blockSignals(false);
    }
    this->thresholdHighSpinBox->blockSignals(true);
    this->thresholdHighSpinBox->setValue(d);
    this->thresholdHighSpinBox->blockSignals(false);
    
    this->thresholdControlChanged();
}

/**
 * Create the statistics section
 * @return the statistics section widget.
 */
QWidget* 
MapScalarDataColorMappingEditorDialog::createStatisticsSection()
{
    const AString blankText("                ");
    this->statisticsMinimumValueLabel = new QLabel(blankText);
    this->statisticsMaximumValueLabel = new QLabel(blankText);
    this->statisticsMeanValueLabel = new QLabel(blankText);
    this->statisticsStandardDeviationLabel = new QLabel(blankText);
    
    QGroupBox* groupBox = new QGroupBox("Statistics");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    this->setLayoutMargins(gridLayout);
    gridLayout->addWidget(new QLabel("Mean"), 0, 0);
    gridLayout->addWidget(this->statisticsMeanValueLabel, 0, 1);
    gridLayout->addWidget(new QLabel("Std Dev"), 1, 0);
    gridLayout->addWidget(this->statisticsStandardDeviationLabel, 1, 1);
    gridLayout->addWidget(new QLabel("Maximum"), 2, 0);
    gridLayout->addWidget(this->statisticsMaximumValueLabel, 2, 1);
    gridLayout->addWidget(new QLabel("Minimum"), 3, 0);
    gridLayout->addWidget(this->statisticsMinimumValueLabel, 3, 1);
    groupBox->setFixedHeight(groupBox->sizeHint().height());
    
    return groupBox;
}

/**
 * Create the threshold section of the dialog.
 * @return
 *   The threshold section.
 */
QWidget* 
MapScalarDataColorMappingEditorDialog::createThresholdSection()
{
    this->thresholdTypeOffRadioButton = new QRadioButton("Off");
    this->thresholdTypeOnRadioButton = new QRadioButton("On");
    this->thresholdTypeMappedRadioButton = new QRadioButton("Mapped");
    this->thresholdTypeMappedAverageAreaRadioButton = new QRadioButton("Mapped Average Area");
    
    QButtonGroup* thresholdTypeButtonGroup = new QButtonGroup(this);
    this->thresholdWidgetGroup->add(thresholdTypeButtonGroup);
    QObject::connect(thresholdTypeButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(thresholdTypeChanged()));
    thresholdTypeButtonGroup->addButton(this->thresholdTypeOffRadioButton);
    thresholdTypeButtonGroup->addButton(this->thresholdTypeOnRadioButton);
    thresholdTypeButtonGroup->addButton(this->thresholdTypeMappedRadioButton);
    thresholdTypeButtonGroup->addButton(this->thresholdTypeMappedAverageAreaRadioButton);
    
    QGroupBox* thresholdTypeGroupBox = new QGroupBox("Threshold Type");
    QGridLayout* thresholdTypeLayout = new QGridLayout(thresholdTypeGroupBox);
    this->setLayoutMargins(thresholdTypeLayout);
    thresholdTypeLayout->addWidget(this->thresholdTypeOffRadioButton, 0, 0);
    thresholdTypeLayout->addWidget(this->thresholdTypeOnRadioButton, 1, 0);
    thresholdTypeLayout->addWidget(this->thresholdTypeMappedRadioButton, 0, 1);
    thresholdTypeLayout->addWidget(this->thresholdTypeMappedAverageAreaRadioButton, 1, 1);
    thresholdTypeGroupBox->setFixedSize(thresholdTypeGroupBox->sizeHint());
        
    QLabel* thresholdLowLabel = new QLabel("Low");
    QLabel* thresholdHighLabel = new QLabel("High");
    
    this->thresholdLowSlider = new WuQDoubleSlider(Qt::Horizontal,
                                                   this);
    this->thresholdWidgetGroup->add(this->thresholdLowSlider);
    QObject::connect(this->thresholdLowSlider, SIGNAL(valueChanged(double)),
                     this, SLOT(thresholdLowSliderValueChanged(double)));
    this->thresholdHighSlider = new WuQDoubleSlider(Qt::Horizontal,
                                                    this);
    this->thresholdWidgetGroup->add(this->thresholdHighSlider);
    QObject::connect(this->thresholdHighSlider, SIGNAL(valueChanged(double)),
                     this, SLOT(thresholdHighSliderValueChanged(double)));
    
    const int spinBoxWidth = 80.0;
    this->thresholdLowSpinBox = new QDoubleSpinBox();
    this->thresholdWidgetGroup->add(this->thresholdLowSpinBox);
    this->thresholdLowSpinBox->setFixedWidth(spinBoxWidth);
    this->thresholdLowSpinBox->setSingleStep(0.10);
    QObject::connect(this->thresholdLowSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(thresholdLowSpinBoxValueChanged(double)));
    this->thresholdHighSpinBox = new QDoubleSpinBox();
    this->thresholdWidgetGroup->add(this->thresholdHighSpinBox);
    this->thresholdHighSpinBox->setFixedWidth(spinBoxWidth);
    this->thresholdHighSpinBox->setSingleStep(0.10);
    QObject::connect(this->thresholdHighSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(thresholdHighSpinBoxValueChanged(double)));

    this->thresholdShowInsideRadioButton = new QRadioButton("Show Data Inside Thresholds");
    this->thresholdShowOutsideRadioButton = new QRadioButton("Show Data Outside Thresholds");
    
    QButtonGroup* thresholdShowButtonGroup = new QButtonGroup(this);
    this->thresholdWidgetGroup->add(thresholdShowButtonGroup);
    thresholdShowButtonGroup->addButton(this->thresholdShowInsideRadioButton);
    thresholdShowButtonGroup->addButton(this->thresholdShowOutsideRadioButton);
    QObject::connect(thresholdShowButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(thresholdControlChanged()));
    
    QGroupBox* thresholdAdjustmentGroupBox = new QGroupBox("Threshold Adjustment");
    QGridLayout* thresholdAdjustmentLayout = new QGridLayout(thresholdAdjustmentGroupBox);
    this->setLayoutMargins(thresholdAdjustmentLayout);
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
    thresholdAdjustmentGroupBox->setFixedHeight(thresholdAdjustmentGroupBox->sizeHint().height());
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    this->setLayoutMargins(layout);
    layout->addWidget(thresholdTypeGroupBox);
    layout->addWidget(thresholdAdjustmentGroupBox);
    
    return w;
}

void 
MapScalarDataColorMappingEditorDialog::histogramControlChanged()
{
    this->updateDialog();
}

/**
 * Create the statistics section
 * @return the statistics section widget.
 */
QWidget* 
MapScalarDataColorMappingEditorDialog::createHistogramControlSection()
{
    this->histogramAllRadioButton = new QRadioButton("All");
    this->histogramTwoNinetyEightRadioButton = new QRadioButton("2% to 98%");
    
    this->histogramAllRadioButton->setChecked(true);
    
    QButtonGroup* buttGroup = new QButtonGroup(this);
    buttGroup->addButton(this->histogramAllRadioButton);
    buttGroup->addButton(this->histogramTwoNinetyEightRadioButton);
    QObject::connect(buttGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(histogramControlChanged()));
    
    QWidget* horizLine = WuQtUtilities::createHorizontalLineWidget();
    
    this->histogramUsePaletteColors = new QCheckBox("Colorize");
    this->histogramUsePaletteColors->setChecked(true);
    QObject::connect(this->histogramUsePaletteColors, SIGNAL(toggled(bool)),
                     this, SLOT(histogramControlChanged()));
    
    QGroupBox* groupBox = new QGroupBox("Histogram");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    this->setLayoutMargins(layout);
    layout->addWidget(this->histogramAllRadioButton);
    layout->addWidget(this->histogramTwoNinetyEightRadioButton);
    layout->addWidget(horizLine);
    layout->addWidget(this->histogramUsePaletteColors);
    groupBox->setFixedSize(groupBox->sizeHint());
    
    return groupBox;
}

/**
 * Create the histogram section of the dialog.
 * @return
 *   The histogram section.
 */
QWidget* 
MapScalarDataColorMappingEditorDialog::createHistogramSection()
{
    this->thresholdPlot = new QwtPlot();
    //this->thresholdPlot->setAutoReplot(true);
    
    return this->thresholdPlot;
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
     * Selection
     */
    this->paletteNameComboBox = new QComboBox();
    this->paletteWidgetGroup->add(this->paletteNameComboBox);
    QObject::connect(this->paletteNameComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(apply()));
    /*
     * Interpolate Colors
     */
    this->interpolateColorsCheckBox = new QCheckBox("Interpolate Colors");
    this->paletteWidgetGroup->add(this->interpolateColorsCheckBox);
    QObject::connect(this->interpolateColorsCheckBox, SIGNAL(toggled(bool)), 
                     this, SLOT(apply()));
    
    QGroupBox* paletteSelectionGroupBox = new QGroupBox("Palette Selection");
    QVBoxLayout* paletteSelectionLayout = new QVBoxLayout(paletteSelectionGroupBox);
    this->setLayoutMargins(paletteSelectionLayout);
    paletteSelectionLayout->addWidget(this->paletteNameComboBox);
    paletteSelectionLayout->addWidget(this->interpolateColorsCheckBox);
    paletteSelectionGroupBox->setFixedHeight(paletteSelectionGroupBox->sizeHint().height());
    
    return paletteSelectionGroupBox;
}

/**
 * Create the palette color mapping section of the dialog.
 * @return
 *   The palette color mapping section.
 */
QWidget* 
MapScalarDataColorMappingEditorDialog::createPaletteColorMappingSection()
{
    /*
     * Color Mapping
     */
    this->scaleAutoRadioButton = new QRadioButton("Full"); //Auto Scale");
    this->scaleAutoPercentageRadioButton = new QRadioButton("Percent"); //"Auto Scale Percentage");
    this->scaleFixedRadioButton = new QRadioButton("Fixed"); //"Fixed Scale");
    
    QButtonGroup* scaleButtonGroup = new QButtonGroup(this);
    this->paletteWidgetGroup->add(scaleButtonGroup);
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
    this->paletteWidgetGroup->add(this->scaleAutoPercentageNegativeMaximumSpinBox);
    this->scaleAutoPercentageNegativeMaximumSpinBox->setFixedWidth(percentSpinBoxWidth);
    this->scaleAutoPercentageNegativeMaximumSpinBox->setMinimum(0);
    this->scaleAutoPercentageNegativeMaximumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleAutoPercentageNegativeMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentageNegativeMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleAutoPercentageNegativeMinimumSpinBox = new QDoubleSpinBox();
    this->paletteWidgetGroup->add(this->scaleAutoPercentageNegativeMinimumSpinBox);
    this->scaleAutoPercentageNegativeMinimumSpinBox->setFixedWidth(percentSpinBoxWidth);
    this->scaleAutoPercentageNegativeMinimumSpinBox->setMinimum(0.0);
    this->scaleAutoPercentageNegativeMinimumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleAutoPercentageNegativeMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentageNegativeMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleAutoPercentagePositiveMinimumSpinBox = new QDoubleSpinBox();
    this->paletteWidgetGroup->add(this->scaleAutoPercentagePositiveMinimumSpinBox);
    this->scaleAutoPercentagePositiveMinimumSpinBox->setFixedWidth(percentSpinBoxWidth);
    this->scaleAutoPercentagePositiveMinimumSpinBox->setMinimum(0.0);
    this->scaleAutoPercentagePositiveMinimumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleAutoPercentagePositiveMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentagePositiveMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleAutoPercentagePositiveMaximumSpinBox = new QDoubleSpinBox();
    this->paletteWidgetGroup->add(this->scaleAutoPercentagePositiveMaximumSpinBox);
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
    this->paletteWidgetGroup->add(this->scaleFixedNegativeMaximumSpinBox);
    this->scaleFixedNegativeMaximumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    this->scaleFixedNegativeMaximumSpinBox->setMinimum(-std::numeric_limits<float>::max());
    this->scaleFixedNegativeMaximumSpinBox->setMaximum(0.0);
    this->scaleFixedNegativeMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleFixedNegativeMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleFixedNegativeMinimumSpinBox = new QDoubleSpinBox();
    this->paletteWidgetGroup->add(this->scaleFixedNegativeMinimumSpinBox);
    this->scaleFixedNegativeMinimumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    this->scaleFixedNegativeMinimumSpinBox->setMinimum(-std::numeric_limits<float>::max());
    this->scaleFixedNegativeMinimumSpinBox->setMaximum(0.0);
    this->scaleFixedNegativeMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleFixedNegativeMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleFixedPositiveMinimumSpinBox = new QDoubleSpinBox();
    this->paletteWidgetGroup->add(this->scaleFixedPositiveMinimumSpinBox);
    this->scaleFixedPositiveMinimumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    this->scaleFixedPositiveMinimumSpinBox->setMinimum(0.0);
    this->scaleFixedPositiveMinimumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleFixedPositiveMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleFixedPositiveMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleFixedPositiveMaximumSpinBox = new QDoubleSpinBox();
    this->paletteWidgetGroup->add(this->scaleFixedPositiveMaximumSpinBox);
    this->scaleFixedPositiveMaximumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    this->scaleFixedPositiveMaximumSpinBox->setMinimum(0.0);
    this->scaleFixedPositiveMaximumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleFixedPositiveMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleFixedPositiveMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    QGroupBox* colorMappingGroupBox = new QGroupBox("Color Mapping");
    QGridLayout* colorMappingLayout = new QGridLayout(colorMappingGroupBox);
    this->setLayoutMargins(colorMappingLayout);
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
    colorMappingGroupBox->setFixedSize(colorMappingGroupBox->sizeHint());
    
    return colorMappingGroupBox;
}

/**
 * Create the palette display mode section of the dialog.
 * @return
 *   The palette display mode section.
 */
QWidget* 
MapScalarDataColorMappingEditorDialog::createPaletteDisplayModeSection()
{
    /*
     * Display Mode
     */
    this->displayModePositiveCheckBox = new QCheckBox("Positive");
    this->paletteWidgetGroup->add(this->displayModePositiveCheckBox);
    QObject::connect(this->displayModePositiveCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(apply()));
    this->displayModeZeroCheckBox = new QCheckBox("Zero");
    this->paletteWidgetGroup->add(this->displayModeZeroCheckBox);
    QObject::connect(this->displayModeZeroCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(apply()));
    this->displayModeNegativeCheckBox = new QCheckBox("Negative");
    this->paletteWidgetGroup->add(this->displayModeNegativeCheckBox);
    QObject::connect(this->displayModeNegativeCheckBox , SIGNAL(toggled(bool)),
                     this, SLOT(apply()));
    QGroupBox* displayModeGroupBox = new QGroupBox("Display Mode");
    QHBoxLayout* displayModeLayout = new QHBoxLayout(displayModeGroupBox);
    WuQtUtilities::setLayoutMargins(displayModeLayout, 10, 3, 3);
    displayModeLayout->addWidget(this->displayModeNegativeCheckBox);
    displayModeLayout->addWidget(this->displayModeZeroCheckBox);
    displayModeLayout->addWidget(this->displayModePositiveCheckBox);
    displayModeGroupBox->setFixedSize(displayModeGroupBox->sizeHint());
    

    return displayModeGroupBox;
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
    
    this->paletteWidgetGroup->blockSignals(true);
    this->thresholdWidgetGroup->blockSignals(true);
    
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
                lowValue = this->paletteColorMapping->getThresholdNormalMinimum();
                highValue = this->paletteColorMapping->getThresholdNormalMaximum();
                break;
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
                this->thresholdTypeMappedRadioButton->setChecked(true);
                lowValue = this->paletteColorMapping->getThresholdMappedMinimum();
                highValue = this->paletteColorMapping->getThresholdMappedMaximum();
                break;
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
                this->thresholdTypeMappedAverageAreaRadioButton->setChecked(true);
                lowValue = this->paletteColorMapping->getThresholdMappedAverageAreaMinimum();
                highValue = this->paletteColorMapping->getThresholdMappedAverageAreaMaximum();
                break;
        }
        
        switch (this->paletteColorMapping->getThresholdTest()) {
            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
                this->thresholdShowOutsideRadioButton->setChecked(true);
                break;
            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
                this->thresholdShowInsideRadioButton->setChecked(true);
                break;
        }
        
        const DescriptiveStatistics* statistics = this->caretMappableDataFile->getMapStatistics(this->mapFileIndex);
        float meanValue = 0;
        float stdDev = 0;
        float minValue = 0;
        float maxValue = 0;
        int64_t numHistogramValues = 0;
        int64_t* histogram = NULL;
        
        if (this->histogramAllRadioButton->isChecked()) {
            meanValue = statistics->getMean();
            stdDev    = statistics->getStandardDeviationSample();
            minValue  = statistics->getMinimumValue();
            maxValue  = statistics->getMaximumValue();

            numHistogramValues = statistics->getHistogramNumberOfElements();
            histogram = const_cast<int64_t*>(statistics->getHistogram());
        }
        else {
            meanValue = statistics->getMean96();
            stdDev    = statistics->getStandardDeviationSample96();
            minValue  = statistics->getMinimumValue96();
            maxValue  = statistics->getMaximumValue96();
            
            numHistogramValues = statistics->getHistogramNumberOfElements();
            histogram = const_cast<int64_t*>(statistics->getHistogram96());
        }
        this->thresholdLowSlider->setRange(minValue, maxValue);
        this->thresholdLowSlider->setValue(lowValue);
        
        this->thresholdHighSlider->setRange(minValue, maxValue);
        this->thresholdHighSlider->setValue(highValue);
        
        this->thresholdLowSpinBox->setMinimum(minValue);
        this->thresholdLowSpinBox->setMaximum(maxValue);
        this->thresholdLowSpinBox->setValue(lowValue);
        
        this->thresholdHighSpinBox->setMinimum(minValue);
        this->thresholdHighSpinBox->setMaximum(maxValue);
        this->thresholdHighSpinBox->setValue(highValue);
        
        
        this->statisticsMeanValueLabel->setText(QString::number(meanValue, 'f', 4));
        this->statisticsStandardDeviationLabel->setText(QString::number(stdDev, 'f', 4));
        this->statisticsMaximumValueLabel->setText(QString::number(maxValue, 'f', 4));
        this->statisticsMinimumValueLabel->setText(QString::number(minValue, 'f', 4));
    }
    
    this->updateHistogramPlot();
    
    this->paletteWidgetGroup->blockSignals(false);
    this->thresholdWidgetGroup->blockSignals(false);
}

/**
 * Update the histogram plot.
 */
void 
MapScalarDataColorMappingEditorDialog::updateHistogramPlot()
{
    /*
     * Remove all previously attached items from the histogram plot.
     * The items are automatically deleted by the plot.
     */
    this->thresholdPlot->detachItems();
    
    if (this->paletteColorMapping != NULL) {
        PaletteFile* paletteFile = GuiManager::get()->getBrain()->getPaletteFile();
        const DescriptiveStatistics* statistics = this->caretMappableDataFile->getMapStatistics(this->mapFileIndex);
        
        /*
         * Get data for this histogram.
         */
        int64_t numHistogramValues = 0;
        int64_t* histogram = NULL;
        float minValue = 0.0;
        float maxValue = 0.0;
        if (this->histogramAllRadioButton->isChecked()) {
            minValue  = statistics->getMinimumValue();
            maxValue  = statistics->getMaximumValue();
            
            numHistogramValues = statistics->getHistogramNumberOfElements();
            histogram = const_cast<int64_t*>(statistics->getHistogram());
        }
        else {
            minValue  = statistics->getMinimumValue96();
            maxValue  = statistics->getMaximumValue96();
            
            numHistogramValues = statistics->getHistogramNumberOfElements();
            histogram = const_cast<int64_t*>(statistics->getHistogram96());
        }
        
        /*
         * Display using palette colors
         */
//        if (isHistogramColored) {
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
                 * Compute color for 'bar' in the histogram
                 */
                dataValues = new float[numHistogramValues];
                dataRGBA   = new float[numHistogramValues * 4];
                for (int64_t ix = 0; ix < numHistogramValues; ix++) {
                    const float value = (minValue
                                         + (ix * step));
                    dataValues[ix] = value;
                }

                const Palette* palette = paletteFile->getPaletteByName(this->paletteColorMapping->getSelectedPaletteName());
                if (this->histogramUsePaletteColors->isChecked()
                    && (palette != NULL)) {
                    NodeAndVoxelColoring::colorScalarsWithPalette(statistics, 
                                                                  paletteColorMapping, 
                                                                  palette, 
                                                                  dataValues, 
                                                                  dataValues, 
                                                                  numHistogramValues, 
                                                                  dataRGBA);
                }
                else {
                    for (int64_t i = 0; i < numHistogramValues; i++) {
                        const int64_t i4 = i * 4;
                        dataRGBA[i4]   = 1.0;
                        dataRGBA[i4+1] = 0.0;
                        dataRGBA[i4+2] = 0.0;
                        dataRGBA[i4+3] = 1.0;
                    }
                }
            }
            
            const int numBars = numHistogramValues - 1;
            for (int64_t ix = 0; ix < numBars; ix++) {
                QColor color;
                const int64_t ix4 = ix * 4;
                color.setRedF(dataRGBA[ix4]);
                color.setGreenF(dataRGBA[ix4+1]);
                color.setBlueF(dataRGBA[ix4+2]);
                color.setAlphaF(1.0);
                
                const float startValue = dataValues[ix];
                const float stopValue  = dataValues[ix + 1];
                float dataFrequency = histogram[ix];
                
                /*
                 * If color is not displayed ('none' or thresholded), 
                 * set its frequncey value to a small value so that the plot
                 * retains its shape and color is still slightly visible
                 */
                Qt::BrushStyle brushStyle = Qt::SolidPattern;
                if (dataRGBA[ix4+3] <= 0.0) {
                    brushStyle = Qt::Dense6Pattern;
                    //dataFrequency = 0.5;
                }
                
                QVector<QPointF> samples;
                samples.push_back(QPointF(startValue, 0));
                samples.push_back(QPointF(stopValue, 0));
                samples.push_back(QPointF(stopValue, dataFrequency));
                samples.push_back(QPointF(startValue, dataFrequency));
                
                QwtPlotCurve* curve = new QwtPlotCurve();
                curve->setRenderHint(QwtPlotItem::RenderAntialiased);
                curve->setVisible(true);
                curve->setStyle(QwtPlotCurve::Steps);
                
                curve->setBrush(QBrush(color, brushStyle));
                curve->setPen(QPen(color));
                curve->setSamples(samples);
                
                curve->attach(this->thresholdPlot);
            }
            
            if (dataValues != NULL) {
                delete dataValues;
            }
            if (dataRGBA != NULL) {
                delete dataRGBA;
            }
//        }
//        else {
//            QVector<QwtIntervalSample> samples(numHistogramValues);
//            for (int64_t i = 0; i < numHistogramValues; i++) {
//                const float startValue = (minValue
//                                          + (i * step));
//                const float stopValue  = startValue + step;
//                
//                QwtInterval interval(startValue,
//                                     stopValue);
//                interval.setBorderFlags(QwtInterval::ExcludeMaximum);
//                samples[i] = QwtIntervalSample(histogram[i],
//                                               interval);
//            }
//            
//            const QColor color(255, 0, 0);
//            
//            QwtPlotHistogram* thresholdHistogram = new QwtPlotHistogram("Histogram");
//            thresholdHistogram->setStyle(QwtPlotHistogram::Columns);
//            thresholdHistogram->setBrush(QBrush(color));
//            thresholdHistogram->setPen(QPen(color));
//            
//            thresholdHistogram->setData(new QwtIntervalSeriesData(samples));
//            thresholdHistogram->attach(this->thresholdPlot);
//        }
        
        /*
         * Causes updates of plots.
         */
        this->thresholdPlot->replot();
    }
}

/**
 * Called when the apply button is pressed.
 */
void MapScalarDataColorMappingEditorDialog::applyButtonPressed()
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
    
    if (this->thresholdTypeOffRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF);
    }
    else if (this->thresholdTypeOnRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL);
        this->paletteColorMapping->setThresholdNormalMinimum(lowValue);
        this->paletteColorMapping->setThresholdNormalMaximum(highValue);
    }
    else if (this->thresholdTypeMappedRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED);
        this->paletteColorMapping->setThresholdMappedMinimum(lowValue);
        this->paletteColorMapping->setThresholdMappedMaximum(highValue);
    }
    else if (this->thresholdTypeMappedAverageAreaRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdType(PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA);
        this->paletteColorMapping->setThresholdMappedAverageAreaMinimum(lowValue);
        this->paletteColorMapping->setThresholdMappedAverageAreaMaximum(highValue);
    }
    
    if (this->thresholdShowInsideRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdTest(PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE);
    }
    else if (this->thresholdShowOutsideRadioButton->isChecked()) {
        this->paletteColorMapping->setThresholdTest(PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE);
    }
    
    this->updateHistogramPlot();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

void 
MapScalarDataColorMappingEditorDialog::setLayoutMargins(QLayout* layout)
{
    WuQtUtilities::setLayoutMargins(layout, 5, 3, 3);
}


