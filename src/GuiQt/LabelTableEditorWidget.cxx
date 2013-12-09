
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#define __LABEL_TABLE_EDITOR_WIDGET_DECLARE__
#include "LabelTableEditorWidget.h"
#undef __LABEL_TABLE_EDITOR_WIDGET_DECLARE__

#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "GiftiLabelTable.h"
#include "WuQTableWidget.h"
#include "WuQTableWidgetModel.h"

using namespace caret;


    
/**
 * \class caret::LabelTableEditorWidget 
 * \brief Widget for editing label tables.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
LabelTableEditorWidget::LabelTableEditorWidget(QWidget* parent)
: QWidget(parent)
{
    m_caretMappableDataFile = NULL;
    m_mapFileIndex = -1;
    m_labelTable = NULL;

    m_tableWidget = new WuQTableWidget(this);
    
    
//    m_tableWidget = new QTableWidget();
//    m_tableWidget->setAlternatingRowColors(true);
//    m_tableWidget->setSelectionBehavior(QTableWidget::SelectItems);
//    m_tableWidget->setSelectionMode(QTableWidget::SingleSelection);
//    QObject::connect(m_tableWidget, SIGNAL(cellChanged(int,int)),
//                     this, SLOT(filesTableWidgetCellChanged(int,int)));
//    QObject::connect(m_tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)),
//                     this, SLOT(horizontalHeaderSelectedForSorting(int)));
//    const QString headerToolTip = ("Click on the column names (of those columns that contain text) to sort.");
//    m_tableWidget->horizontalHeader()->setToolTip(WuQtUtilities::createWordWrappedToolTipText(headerToolTip));
    
//    WuQTableWidgetHelper::insertHorizontalHeaderTextItem(m_tableWidget,
//                                                         m_COLUMN_KEY,
//                                                         "Key");
//    WuQTableWidgetHelper::insertHorizontalHeaderTextItem(m_tableWidget,
//                                                         m_COLUMN_NAME,
//                                                         "Name");
//    WuQTableWidgetHelper::insertHorizontalHeaderTextItem(m_tableWidget,
//                                                         m_COLUMN_RED,
//                                                         "Red");
//    WuQTableWidgetHelper::insertHorizontalHeaderTextItem(m_tableWidget,
//                                                         m_COLUMN_GREEN,
//                                                         "Green");
//    WuQTableWidgetHelper::insertHorizontalHeaderTextItem(m_tableWidget,
//                                                         m_COLUMN_BLUE,
//                                                         "Blue");
//    WuQTableWidgetHelper::insertHorizontalHeaderTextItem(m_tableWidget,
//                                                         m_COLUMN_ALPHA,
//                                                         "Alpha");
//    WuQTableWidgetHelper::insertHorizontalHeaderTextItem(m_tableWidget,
//                                                         m_COLUMN_COLOR_SWATCH,
//                                                         "Color");
    
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_tableWidget->getWidget(), 100);
    
//    std::vector<WuQTableWidgetModelColumnContent*> columnContent;
//    columnContent.push_back(new WuQTableWidgetModelColumnCheckBox(WuQTableWidgetModelColumnContent::COLUMN_ALIGN_LEFT));
//    
//    columnContent.push_back(new WuQTableWidgetModelColumnDoubleSpinBox(WuQTableWidgetModelColumnContent::COLUMN_ALIGN_LEFT,
//                                                                       0.0,
//                                                                       255.0,
//                                                                       1.0,
//                                                                       2));
//    
//    columnContent.push_back(new WuQTableWidgetModelColumnText(WuQTableWidgetModelColumnContent::COLUMN_ALIGN_LEFT));
//
//
//    WuQTableWidgetModel* model = new WuQTableWidgetModel(columnContent,
//                                                         10);
//    m_tableWidget->setModel(model);
}

/**
 * Destructor.
 */
LabelTableEditorWidget::~LabelTableEditorWidget()
{
}

/**
 * May be called to update the widget's content.
 */
void
LabelTableEditorWidget::updateWidget()
{
    this->updateEditor(m_caretMappableDataFile,
                       m_mapFileIndex);
    
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
LabelTableEditorWidget::updateEditor(CaretMappableDataFile* caretMappableDataFile,
                                                   const int32_t mapIndexIn)
{
    m_caretMappableDataFile = caretMappableDataFile;
    m_mapFileIndex = mapIndexIn;
    
    bool isValidLabelTable = true;
    
    if (m_caretMappableDataFile == NULL) {
        isValidLabelTable = false;
    }
    else if (m_mapFileIndex < 0) {
        isValidLabelTable = false;
    }

    if ( ! isValidLabelTable) {
        m_caretMappableDataFile = NULL;
        m_mapFileIndex = -1;
        m_labelTable = NULL;
        m_tableWidget->setModel(NULL);
        return;
    }
    
    GiftiLabelTable* labelTable = m_caretMappableDataFile->getMapLabelTable(m_mapFileIndex);
    WuQTableWidgetModel* model = labelTable->getInWuQTableWidgetModel();
    m_tableWidget->setModel(model);

//    this->paletteWidgetGroup->blockAllSignals(true);
//    this->thresholdWidgetGroup->blockAllSignals(true);
//    
//    const AString title =
//    this->caretMappableDataFile->getFileNameNoPath()
//    + ": "
//    + this->caretMappableDataFile->getMapName(this->mapFileIndex);
//    this->setWindowTitle(title);
//    
//    this->paletteNameComboBox->clear();
//    
//    
//    this->paletteColorMapping = this->caretMappableDataFile->getMapPaletteColorMapping(this->mapFileIndex);
//    
//    if (this->paletteColorMapping != NULL) {
//        PaletteFile* paletteFile = GuiManager::get()->getBrain()->getPaletteFile();
//        
//        int defaultIndex = 0;
//        const int32_t numPalettes = paletteFile->getNumberOfPalettes();
//        for (int32_t i = 0; i < numPalettes; i++) {
//            Palette* palette = paletteFile->getPalette(i);
//            const AString name = palette->getName();
//            if (name == this->paletteColorMapping->getSelectedPaletteName()) {
//                defaultIndex = i;
//            }
//            this->paletteNameComboBox->addItem(name,
//                                               name);
//        }
//        
//        if (defaultIndex < this->paletteNameComboBox->count()) {
//            this->paletteNameComboBox->setCurrentIndex(defaultIndex);
//        }
//        
//        bool isPercentageSpinBoxesEnabled = false;
//        bool isFixedSpinBoxesEnabled = false;
//        switch (this->paletteColorMapping->getScaleMode()) {
//            case PaletteScaleModeEnum::MODE_AUTO_SCALE:
//                this->scaleAutoRadioButton->setChecked(true);
//                break;
//            case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
//                this->scaleAutoPercentageRadioButton->setChecked(true);
//                isPercentageSpinBoxesEnabled = true;
//                break;
//            case PaletteScaleModeEnum::MODE_USER_SCALE:
//                this->scaleFixedRadioButton->setChecked(true);
//                isFixedSpinBoxesEnabled = true;
//                break;
//        }
//        
//        this->scaleAutoPercentageNegativeMaximumSpinBox->setValue(this->paletteColorMapping->getAutoScalePercentageNegativeMaximum());
//        this->scaleAutoPercentageNegativeMinimumSpinBox->setValue(this->paletteColorMapping->getAutoScalePercentageNegativeMinimum());
//        this->scaleAutoPercentagePositiveMinimumSpinBox->setValue(this->paletteColorMapping->getAutoScalePercentagePositiveMinimum());
//        this->scaleAutoPercentagePositiveMaximumSpinBox->setValue(this->paletteColorMapping->getAutoScalePercentagePositiveMaximum());
//        this->scaleAutoPercentageNegativeMaximumSpinBox->setEnabled(isPercentageSpinBoxesEnabled);
//        this->scaleAutoPercentageNegativeMinimumSpinBox->setEnabled(isPercentageSpinBoxesEnabled);
//        this->scaleAutoPercentagePositiveMinimumSpinBox->setEnabled(isPercentageSpinBoxesEnabled);
//        this->scaleAutoPercentagePositiveMaximumSpinBox->setEnabled(isPercentageSpinBoxesEnabled);
//        
//        this->scaleFixedNegativeMaximumSpinBox->setValue(this->paletteColorMapping->getUserScaleNegativeMaximum());
//        this->scaleFixedNegativeMinimumSpinBox->setValue(this->paletteColorMapping->getUserScaleNegativeMinimum());
//        this->scaleFixedPositiveMinimumSpinBox->setValue(this->paletteColorMapping->getUserScalePositiveMinimum());
//        this->scaleFixedPositiveMaximumSpinBox->setValue(this->paletteColorMapping->getUserScalePositiveMaximum());
//        this->scaleFixedNegativeMaximumSpinBox->setEnabled(isFixedSpinBoxesEnabled);
//        this->scaleFixedNegativeMinimumSpinBox->setEnabled(isFixedSpinBoxesEnabled);
//        this->scaleFixedPositiveMinimumSpinBox->setEnabled(isFixedSpinBoxesEnabled);
//        this->scaleFixedPositiveMaximumSpinBox->setEnabled(isFixedSpinBoxesEnabled);
//        
//        this->displayModePositiveCheckBox->setChecked(this->paletteColorMapping->isDisplayPositiveDataFlag());
//        this->displayModeZeroCheckBox->setChecked(this->paletteColorMapping->isDisplayZeroDataFlag());
//        this->displayModeNegativeCheckBox->setChecked(this->paletteColorMapping->isDisplayNegativeDataFlag());
//        
//        this->interpolateColorsCheckBox->setChecked(this->paletteColorMapping->isInterpolatePaletteFlag());
//        
//        const int32_t numTypes = this->thresholdTypeComboBox->count();
//        for (int32_t i = 0; i < numTypes; i++) {
//            const int value = this->thresholdTypeComboBox->itemData(i).toInt();
//            if (value == static_cast<int>(this->paletteColorMapping->getThresholdType())) {
//                this->thresholdTypeComboBox->setCurrentIndex(i);
//                break;
//            }
//        }
//        
//        const bool enableThresholdControls = (this->paletteColorMapping->getThresholdType() != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF);
//        this->thresholdAdjustmentWidgetGroup->setEnabled(enableThresholdControls);
//        const float lowValue = this->paletteColorMapping->getThresholdMinimum(this->paletteColorMapping->getThresholdType());
//        const float highValue = this->paletteColorMapping->getThresholdMaximum(this->paletteColorMapping->getThresholdType());
//        
//        switch (this->paletteColorMapping->getThresholdTest()) {
//            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
//                this->thresholdShowOutsideRadioButton->setChecked(true);
//                break;
//            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
//                this->thresholdShowInsideRadioButton->setChecked(true);
//                break;
//        }
//        
//        const PaletteThresholdRangeModeEnum::Enum thresholdRangeMode = paletteColorMapping->getThresholdRangeMode();
//        this->thresholdRangeModeComboBox->blockSignals(true);
//        this->thresholdRangeModeComboBox->setSelectedItem<PaletteThresholdRangeModeEnum, PaletteThresholdRangeModeEnum::Enum>(thresholdRangeMode);
//        this->thresholdRangeModeComboBox->blockSignals(false);
//        updateThresholdControlsMinimumMaximumRangeValues();
//        
//        const FastStatistics* statistics = this->caretMappableDataFile->getMapFastStatistics(this->mapFileIndex);
//        float minValue  = statistics->getMin();
//        float maxValue  = statistics->getMax();
//        
//        this->thresholdLowSlider->setValue(lowValue);
//        
//        this->thresholdHighSlider->setValue(highValue);
//        
//        this->thresholdLowSpinBox->setValue(lowValue);
//        
//        this->thresholdHighSpinBox->setValue(highValue);
//        
//        /*
//         * Set fixed spin boxes so that they increment by 1% of data.
//         */
//        float stepValue = 1.0;
//        const float diff = maxValue - minValue;
//        if (diff > 0.0) {
//            stepValue = diff / 100.0;
//        }
//        this->scaleFixedNegativeMaximumSpinBox->setSingleStep(stepValue);
//        this->scaleFixedNegativeMinimumSpinBox->setSingleStep(stepValue);
//        this->scaleFixedPositiveMinimumSpinBox->setSingleStep(stepValue);
//        this->scaleFixedPositiveMaximumSpinBox->setSingleStep(stepValue);
//    }
//    
//    this->updateHistogramPlot();
//    
//    this->paletteWidgetGroup->blockAllSignals(false);
//    this->thresholdWidgetGroup->blockAllSignals(false);
}
