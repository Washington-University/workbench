
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

#include <algorithm>
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
#include <QToolButton>

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
#include "qwt_plot_intervalcurve.h"
#include "qwt_plot_layout.h"
#include "qwt_plot_magnifier.h"
#include "qwt_plot_panner.h"

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
    /*
     * No context menu, it screws things up
     */
    this->setContextMenuPolicy(Qt::NoContextMenu);
    
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

    QWidget* thresholdWidget = this->createThresholdSection();
    
    QWidget* leftWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    this->setLayoutMargins(leftLayout);
    leftLayout->addWidget(thresholdWidget);
    leftLayout->addWidget(paletteWidget);
    leftLayout->addStretch();
    
    QWidget* bottomRightWidget = new QWidget();
    QHBoxLayout* bottomRightLayout = new QHBoxLayout(bottomRightWidget);
    this->setLayoutMargins(bottomRightLayout);
    bottomRightLayout->addWidget(histogramControlWidget);    
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
MapScalarDataColorMappingEditorDialog::thresholdTypeChanged(int indx)
{
    PaletteThresholdTypeEnum::Enum paletteThresholdType = static_cast<PaletteThresholdTypeEnum::Enum>(this->thresholdTypeComboBox->itemData(indx).toInt());
    this->paletteColorMapping->setThresholdType(paletteThresholdType);
    
    this->updateEditor(this->caretMappableDataFile, 
                       this->mapFileIndex);
    
    this->apply();
}

/**
 * Should be called when a control is changed and 
 * the change requires and update to other controls.
 */
void 
MapScalarDataColorMappingEditorDialog::applyAndUpdate()
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
    
    this->applyAndUpdate();
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
    
    this->applyAndUpdate();
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
    
    this->applyAndUpdate();
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
    
    this->applyAndUpdate();
}

/**
 * Create the threshold section of the dialog.
 * @return
 *   The threshold section.
 */
QWidget* 
MapScalarDataColorMappingEditorDialog::createThresholdSection()
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
    
    QLabel* thresholdLowLabel = new QLabel("Low");
    QLabel* thresholdHighLabel = new QLabel("High");
    
    this->thresholdLowSlider = new WuQDoubleSlider(Qt::Horizontal,
                                                   this);
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdLowSlider->getWidget(), 
                                          "Adjust the low threshold value");
    this->thresholdWidgetGroup->add(this->thresholdLowSlider);
    QObject::connect(this->thresholdLowSlider, SIGNAL(valueChanged(double)),
                     this, SLOT(thresholdLowSliderValueChanged(double)));
    this->thresholdHighSlider = new WuQDoubleSlider(Qt::Horizontal,
                                                    this);
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdHighSlider->getWidget(), 
                                          "Adjust the high threshold value");
    this->thresholdWidgetGroup->add(this->thresholdHighSlider);
    QObject::connect(this->thresholdHighSlider, SIGNAL(valueChanged(double)),
                     this, SLOT(thresholdHighSliderValueChanged(double)));
    
    const int spinBoxWidth = 80.0;
    this->thresholdLowSpinBox = new QDoubleSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdLowSpinBox, 
                                          "Adjust the low threshold value");
    this->thresholdWidgetGroup->add(this->thresholdLowSpinBox);
    this->thresholdLowSpinBox->setFixedWidth(spinBoxWidth);
    this->thresholdLowSpinBox->setSingleStep(0.10);
    QObject::connect(this->thresholdLowSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(thresholdLowSpinBoxValueChanged(double)));
    this->thresholdHighSpinBox = new QDoubleSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdHighSpinBox, 
                                          "Adjust the high threshold value");
    this->thresholdWidgetGroup->add(this->thresholdHighSpinBox);
    this->thresholdHighSpinBox->setFixedWidth(spinBoxWidth);
    this->thresholdHighSpinBox->setSingleStep(0.10);
    QObject::connect(this->thresholdHighSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(thresholdHighSpinBoxValueChanged(double)));

    this->thresholdShowInsideRadioButton = new QRadioButton("Show Data Inside Thresholds");
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdShowInsideRadioButton, 
                                          "Displays only data greater than low threshold AND less than high threshold");
    this->thresholdShowOutsideRadioButton = new QRadioButton("Show Data Outside Thresholds");
    WuQtUtilities::setToolTipAndStatusTip(this->thresholdShowOutsideRadioButton, 
                                          "Displays data less than low threshold OR greater than high threshold");
    
    QButtonGroup* thresholdShowButtonGroup = new QButtonGroup(this);
    this->thresholdWidgetGroup->add(thresholdShowButtonGroup);
    thresholdShowButtonGroup->addButton(this->thresholdShowInsideRadioButton);
    thresholdShowButtonGroup->addButton(this->thresholdShowOutsideRadioButton);
    QObject::connect(thresholdShowButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(applyAndUpdate()));
    
    QWidget* thresholdAdjustmentWidget = new QWidget();
    QGridLayout* thresholdAdjustmentLayout = new QGridLayout(thresholdAdjustmentWidget);
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
    thresholdAdjustmentWidget->setFixedHeight(thresholdAdjustmentWidget->sizeHint().height());
    
    QWidget* typeWidget = new QWidget();
    QHBoxLayout* typeLayout = new QHBoxLayout(typeWidget);
    this->setLayoutMargins(typeLayout);
    typeLayout->addWidget(thresholdTypeLabel, 0);
    typeLayout->addWidget(thresholdTypeComboBox, 100);
    
    QGroupBox* thresholdGroupBox = new QGroupBox("Threshold");
    QVBoxLayout* layout = new QVBoxLayout(thresholdGroupBox);
    this->setLayoutMargins(layout);
    layout->addWidget(typeWidget, 0, Qt::AlignLeft);
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
    
    return thresholdGroupBox;
}

/**
 * Called when a histogram control is changed.
 */
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
    /*
     * Control section
     */
    this->histogramAllRadioButton = new QRadioButton("All");
    WuQtUtilities::setToolTipAndStatusTip(this->histogramAllRadioButton, 
                                          "Displays all map data in the histogram");
    this->histogramAllNoTwoNinetyEightRadioButton = new QRadioButton("All (no 2%/98%)");
    WuQtUtilities::setToolTipAndStatusTip(this->histogramAllNoTwoNinetyEightRadioButton, 
                                          "Excludes bottom 2% and top 2% but scales as if all data is displayed");
    this->histogramTwoNinetyEightRadioButton = new QRadioButton("2% to 98%");
    WuQtUtilities::setToolTipAndStatusTip(this->histogramTwoNinetyEightRadioButton, 
                                          "Excludes bottom 2% and top 2% of map data from the histogram");
    
    this->histogramAllRadioButton->setChecked(true);
    
    QButtonGroup* buttGroup = new QButtonGroup(this);
    buttGroup->addButton(this->histogramAllRadioButton);
    buttGroup->addButton(this->histogramAllNoTwoNinetyEightRadioButton);
    buttGroup->addButton(this->histogramTwoNinetyEightRadioButton);
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
    this->setLayoutMargins(controlLayout);
    controlLayout->addWidget(this->histogramAllRadioButton);
    controlLayout->addWidget(this->histogramAllNoTwoNinetyEightRadioButton);
    controlLayout->addWidget(this->histogramTwoNinetyEightRadioButton);
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
    this->setLayoutMargins(statisticsLayout);
    statisticsLayout->addWidget(new QLabel("Mean"), 0, 0);
    statisticsLayout->addWidget(this->statisticsMeanValueLabel, 0, 1);
    statisticsLayout->addWidget(new QLabel("Std Dev"), 1, 0);
    statisticsLayout->addWidget(this->statisticsStandardDeviationLabel, 1, 1);
    statisticsLayout->addWidget(new QLabel("Maximum"), 2, 0);
    statisticsLayout->addWidget(this->statisticsMaximumValueLabel, 2, 1);
    statisticsLayout->addWidget(new QLabel("Minimum"), 3, 0);
    statisticsLayout->addWidget(this->statisticsMinimumValueLabel, 3, 1);
    statisticsWidget->setFixedHeight(statisticsWidget->sizeHint().height());
    
    
    QGroupBox* groupBox = new QGroupBox("Histogram");
    QHBoxLayout* layout = new QHBoxLayout(groupBox);
    this->setLayoutMargins(layout);
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
MapScalarDataColorMappingEditorDialog::createHistogramSection()
{
    this->thresholdPlot = new QwtPlot();
    this->thresholdPlot->plotLayout()->setAlignCanvasToScales(true);
    
    /*
     * Allow zooming
     */
    QwtPlotMagnifier* magnifier = new QwtPlotMagnifier(this->thresholdPlot->canvas());
    magnifier->setAxisEnabled(QwtPlot::yLeft, true);
    magnifier->setAxisEnabled(QwtPlot::yRight, true);
    
    /*
     * Allow panning
     */
    (void)new QwtPlotPanner(this->thresholdPlot->canvas());
    
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
    WuQtUtilities::setLayoutMargins(layout, 2, 0);
    return  widget;
}

/**
 * Called to reset the view of the histogram chart.
 */
void 
MapScalarDataColorMappingEditorDialog::histogramResetViewButtonClicked()
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
MapScalarDataColorMappingEditorDialog::createPaletteSection()
{
    /*
     * Selection
     */
    this->paletteNameComboBox = new QComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->paletteNameComboBox, 
                                          "Select palette for coloring map data");
    this->paletteWidgetGroup->add(this->paletteNameComboBox);
    QObject::connect(this->paletteNameComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(apply()));
    /*
     * Interpolate Colors
     */
    this->interpolateColorsCheckBox = new QCheckBox("Interpolate Colors");
    WuQtUtilities::setToolTipAndStatusTip(this->interpolateColorsCheckBox, 
                                          "Smooth colors for data between palette colors");
    this->paletteWidgetGroup->add(this->interpolateColorsCheckBox);
    QObject::connect(this->interpolateColorsCheckBox, SIGNAL(toggled(bool)), 
                     this, SLOT(apply()));
        
    QWidget* paletteSelectionWidget = new QWidget();
    QVBoxLayout* paletteSelectionLayout = new QVBoxLayout(paletteSelectionWidget);
    this->setLayoutMargins(paletteSelectionLayout);
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
    this->scaleAutoPercentageNegativeMaximumSpinBox = new QDoubleSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentageNegativeMaximumSpinBox, 
                                          "Map percentile (NOT percentage) most negative value to -1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoPercentageNegativeMaximumSpinBox);
    this->scaleAutoPercentageNegativeMaximumSpinBox->setFixedWidth(percentSpinBoxWidth);
    this->scaleAutoPercentageNegativeMaximumSpinBox->setMinimum(0);
    this->scaleAutoPercentageNegativeMaximumSpinBox->setMaximum(100.0);
    this->scaleAutoPercentageNegativeMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentageNegativeMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleAutoPercentageNegativeMinimumSpinBox = new QDoubleSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentageNegativeMinimumSpinBox, 
                                          "Map percentile (NOT percentage) least negative value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoPercentageNegativeMinimumSpinBox);
    this->scaleAutoPercentageNegativeMinimumSpinBox->setFixedWidth(percentSpinBoxWidth);
    this->scaleAutoPercentageNegativeMinimumSpinBox->setMinimum(0.0);
    this->scaleAutoPercentageNegativeMinimumSpinBox->setMaximum(100.0);
    this->scaleAutoPercentageNegativeMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentageNegativeMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleAutoPercentagePositiveMinimumSpinBox = new QDoubleSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentagePositiveMinimumSpinBox, 
                                          "Map percentile (NOT percentage) least positive value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoPercentagePositiveMinimumSpinBox);
    this->scaleAutoPercentagePositiveMinimumSpinBox->setFixedWidth(percentSpinBoxWidth);
    this->scaleAutoPercentagePositiveMinimumSpinBox->setMinimum(0.0);
    this->scaleAutoPercentagePositiveMinimumSpinBox->setMaximum(100.0);
    this->scaleAutoPercentagePositiveMinimumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentagePositiveMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleAutoPercentagePositiveMaximumSpinBox = new QDoubleSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->scaleAutoPercentagePositiveMaximumSpinBox, 
                                          "Map percentile (NOT percentage) most positive value to 1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleAutoPercentagePositiveMaximumSpinBox);
    this->scaleAutoPercentagePositiveMaximumSpinBox->setFixedWidth(percentSpinBoxWidth);
    this->scaleAutoPercentagePositiveMaximumSpinBox->setMinimum(0.0);
    this->scaleAutoPercentagePositiveMaximumSpinBox->setMaximum(100.0);
    this->scaleAutoPercentagePositiveMaximumSpinBox->setSingleStep(1.0);
    QObject::connect(this->scaleAutoPercentagePositiveMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    /*
     * Fixed mapping
     */
    this->scaleFixedNegativeMaximumSpinBox = new QDoubleSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedNegativeMaximumSpinBox, 
                                          "Map this value to -1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleFixedNegativeMaximumSpinBox);
    this->scaleFixedNegativeMaximumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    this->scaleFixedNegativeMaximumSpinBox->setMinimum(-std::numeric_limits<float>::max());
    this->scaleFixedNegativeMaximumSpinBox->setMaximum(0.0);
    this->scaleFixedNegativeMaximumSpinBox->setSingleStep(1.0);
    this->scaleFixedNegativeMaximumSpinBox->setDecimals(3);
    QObject::connect(this->scaleFixedNegativeMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleFixedNegativeMinimumSpinBox = new QDoubleSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedNegativeMinimumSpinBox, 
                                          "Map this value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleFixedNegativeMinimumSpinBox);
    this->scaleFixedNegativeMinimumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    this->scaleFixedNegativeMinimumSpinBox->setMinimum(-std::numeric_limits<float>::max());
    this->scaleFixedNegativeMinimumSpinBox->setMaximum(0.0);
    this->scaleFixedNegativeMinimumSpinBox->setSingleStep(1.0);
    this->scaleFixedNegativeMinimumSpinBox->setDecimals(3);
    QObject::connect(this->scaleFixedNegativeMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleFixedPositiveMinimumSpinBox = new QDoubleSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedPositiveMinimumSpinBox, 
                                          "Map this value to 0.0 in palette");
    this->paletteWidgetGroup->add(this->scaleFixedPositiveMinimumSpinBox);
    this->scaleFixedPositiveMinimumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    this->scaleFixedPositiveMinimumSpinBox->setMinimum(0.0);
    this->scaleFixedPositiveMinimumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleFixedPositiveMinimumSpinBox->setSingleStep(1.0);
    this->scaleFixedPositiveMinimumSpinBox->setDecimals(3);
    QObject::connect(this->scaleFixedPositiveMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    this->scaleFixedPositiveMaximumSpinBox = new QDoubleSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->scaleFixedPositiveMaximumSpinBox, 
                                          "Map this value to 1.0 in palette");
    this->paletteWidgetGroup->add(this->scaleFixedPositiveMaximumSpinBox);
    this->scaleFixedPositiveMaximumSpinBox->setFixedWidth(fixedSpinBoxWidth);
    this->scaleFixedPositiveMaximumSpinBox->setMinimum(0.0);
    this->scaleFixedPositiveMaximumSpinBox->setMaximum(std::numeric_limits<float>::max());
    this->scaleFixedPositiveMaximumSpinBox->setSingleStep(1.0);
    this->scaleFixedPositiveMaximumSpinBox->setDecimals(3);
    QObject::connect(this->scaleFixedPositiveMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(apply()));
    
    QWidget* colorMappingWidget = new QWidget();
    QGridLayout* colorMappingLayout = new QGridLayout(colorMappingWidget);
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
    colorMappingWidget->setFixedSize(colorMappingWidget->sizeHint());


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
    WuQtUtilities::setLayoutMargins(displayModeLayout, 10, 3);
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
    this->setLayoutMargins(paletteLayout);
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
        
        const DescriptiveStatistics* statistics = this->caretMappableDataFile->getMapStatistics(this->mapFileIndex);
        float meanValue = 0;
        float stdDev = 0;
        float minValue = 0;
        float maxValue = 0;
        
        if (this->histogramAllRadioButton->isChecked()) {
            meanValue = statistics->getMean();
            stdDev    = statistics->getStandardDeviationSample();
            minValue  = statistics->getMinimumValue();
            maxValue  = statistics->getMaximumValue();
        }
        else if (this->histogramAllNoTwoNinetyEightRadioButton->isChecked()) {
            meanValue = statistics->getMean();
            stdDev    = statistics->getStandardDeviationSample();
            minValue  = statistics->getMinimumValue();
            maxValue  = statistics->getMaximumValue();
        }
        else {
            meanValue = statistics->getMean96();
            stdDev    = statistics->getStandardDeviationSample96();
            minValue  = statistics->getMinimumValue96();
            maxValue  = statistics->getMaximumValue96();
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
        const int64_t numHistogramValues = statistics->getHistogramNumberOfElements();
        int64_t* histogram = const_cast<int64_t*>(statistics->getHistogram());
        float minValue = statistics->getMinimumValue();
        float maxValue = statistics->getMaximumValue();
        float displayedMinValue = minValue;
        float displayedMaxValue = maxValue;
        if (this->histogramAllNoTwoNinetyEightRadioButton->isChecked()) {
            displayedMinValue  = statistics->getMinimumValue96();
            displayedMaxValue  = statistics->getMaximumValue96();
        }
        else if (this->histogramTwoNinetyEightRadioButton->isChecked()) {
            histogram = const_cast<int64_t*>(statistics->getHistogram96());
            minValue = statistics->getMinimumValue96();
            maxValue = statistics->getMaximumValue96();
            displayedMinValue = minValue;
            displayedMaxValue = maxValue;
        }
        
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
                NodeAndVoxelColoring::colorScalarsWithPalette(statistics, 
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
            float dataFrequency = histogram[ix];
            
            bool displayIt = true;
            if (startValue > displayedMaxValue) {
                dataFrequency = 0.0;
                displayIt = false;
            }
            else if (stopValue < displayedMinValue) {
                dataFrequency = 0.0;
                displayIt = false;
            }
            
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
                    /* 
                     * Draw shaded region to left of minimum threshold
                     */
                    QVector<QPointF> minSamples;
                    minSamples.push_back(QPointF(dataValues[0], maxDataFrequency));
                    //minSamples.push_back(QPointF(threshMinValue, 0));
                    minSamples.push_back(QPointF(threshMinValue, maxDataFrequency));
                    //minSamples.push_back(QPointF(dataValues[0], maxFrequency));
                    
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
                     * Draw shaded region to left of minimum threshold
                     */
                    QVector<QPointF> maxSamples;
                    //maxSamples.push_back(QPointF(dataValues[0], 0));
                    //maxSamples.push_back(QPointF(threshMinValue, 0));
                    maxSamples.push_back(QPointF(threshMaxValue, maxDataFrequency));
                    maxSamples.push_back(QPointF(dataValues[numHistogramValues - 1], maxDataFrequency));
                    
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
                     * Draw shaded region to left of minimum threshold
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
            
            z = z - 1;
            
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
    
    this->updateHistogramPlot();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

void 
MapScalarDataColorMappingEditorDialog::setLayoutMargins(QLayout* layout)
{
    WuQtUtilities::setLayoutMargins(layout, 5, 3);
}


