
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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

#include <limits>

#include <QAction>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QToolButton>
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

#define __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_DECLARE__
#include "CaretMappableDataFileAndMapSelector.h"
#undef __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_DECLARE__

#include "Brain.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "GiftiLabel.h"
#include "GiftiLabelTableEditor.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "SpecFile.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::CaretMappableDataFileAndMapSelector 
 * \brief Widget for selecting a map file and map.
 *
 * Dialog that allows the user to select a map file and
 * a map within the map file.  The dialog also allows 
 * the user to create a new map file and/or map.
 */

/**
 * Constructor.
 *
 * @param brainStructure
 *    Brain structure that contains map files.
 * @param mapFileType
 *    Type of map file.
 * @param parent
 *    The parent.
 */
CaretMappableDataFileAndMapSelector::CaretMappableDataFileAndMapSelector(BrainStructure* brainStructure,
                                                                         QObject* parent)
: WuQWidget(parent)
{
    CaretAssert(brainStructure);
    this->brainStructure = brainStructure;

    this->supportedMapFileTypes.push_back(DataFileTypeEnum::LABEL);
    this->supportedMapFileTypes.push_back(DataFileTypeEnum::METRIC);
    
    /*
     * File Type
     */
    QLabel* mapFileTypeLabel = new QLabel("Type: ");
    this->mapFileTypeComboBox = new QComboBox();
    for (std::vector<DataFileTypeEnum::Enum>::iterator dataFileTypeIterator = this->supportedMapFileTypes.begin();
         dataFileTypeIterator != this->supportedMapFileTypes.end();
         dataFileTypeIterator++) {
        const DataFileTypeEnum::Enum dataFileType = *dataFileTypeIterator;
        const AString name = DataFileTypeEnum::toGuiName(dataFileType);
        this->mapFileTypeComboBox->addItem(name,
                                           (int)DataFileTypeEnum::toIntegerCode(dataFileType));
    }
    QObject::connect(this->mapFileTypeComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapFileTypeComboBoxSelected(int)));
    
    /*
     * File Selection
     */
    QLabel* mapFileLabel     = new QLabel("File: ");
    this->mapFileComboBox = new QComboBox();
    QObject::connect(this->mapFileComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapFileComboBoxSelected(int)));
    QAction* newMapFileAction = WuQtUtilities::createAction("New...", 
                                                            "", 
                                                            this, 
                                                            this, 
                                                            SLOT(newMapFileToolButtonSelected()));
    QToolButton* newMapFileToolButton = new QToolButton();
    newMapFileToolButton->setDefaultAction(newMapFileAction);
    
    /*
     * Map Name Selection
     */
    QLabel* mapNameLabel     = new QLabel("Map: ");
    this->mapNameComboBox     = new QComboBox();
    QObject::connect(this->mapNameComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapNameComboBoxSelected(int)));
    QAction* newMapAction = WuQtUtilities::createAction("New...", 
                                                        "", 
                                                        this, 
                                                        this, 
                                                        SLOT(newMapToolButtonSelected()));
    QToolButton* newMapToolButton     = new QToolButton();
    newMapToolButton->setDefaultAction(newMapAction);
    
    /*
     * Label selection
     */
    QLabel* labelNameLabel = new QLabel("Label Name: ");
    this->labelSelectionComboBox = new QComboBox();
    QObject::connect(this->labelSelectionComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(labelNameComboBoxSelected(int)));
    QAction* editLabelsAction = WuQtUtilities::createAction("Edit...", 
                                                            "Add/Edit/Delete Labels\nand edit their colors", 
                                                            this, 
                                                            this, 
                                                            SLOT(showLabelsEditor()));
    QToolButton* editLabelsToolButton = new QToolButton();
    editLabelsToolButton->setDefaultAction(editLabelsAction);
    this->labelValueControlsGroup = new WuQWidgetObjectGroup(this);
    this->labelValueControlsGroup->add(labelNameLabel);
    this->labelValueControlsGroup->add(this->labelSelectionComboBox);
    this->labelValueControlsGroup->add(editLabelsToolButton);
    
    /*
     * Metric Value Entry
     */
    QLabel* metricValueLabel = new QLabel("Value: ");
    this->metricValueSpinBox = WuQFactory::newDoubleSpinBox();
    this->metricValueSpinBox->setMaximumWidth(100);
    this->metricValueSpinBox->setRange(-std::numeric_limits<float>::max(), 
                                       std::numeric_limits<float>::max());
    this->metricValueSpinBox->setValue(1.0);
    QObject::connect(this->metricValueSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(metricValueChanged(double)));
    this->metricValueControlsGroup = new WuQWidgetObjectGroup(this);
    this->metricValueControlsGroup->add(metricValueLabel);
    this->metricValueControlsGroup->add(this->metricValueSpinBox);
    
    /*
     * Layout widgets
     */
    this->widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(this->widget);
    WuQtUtilities::setLayoutMargins(gridLayout, 
                                    4, 
                                    2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->addWidget(mapFileTypeLabel, 0, 0);
    gridLayout->addWidget(this->mapFileTypeComboBox, 0, 1);
    gridLayout->addWidget(mapFileLabel, 1, 0);
    gridLayout->addWidget(this->mapFileComboBox, 1, 1);
    gridLayout->addWidget(newMapFileToolButton, 1, 2);
    gridLayout->addWidget(mapNameLabel, 2, 0);
    gridLayout->addWidget(this->mapNameComboBox, 2, 1);
    gridLayout->addWidget(newMapToolButton, 2, 2);
    gridLayout->addWidget(labelNameLabel, 3, 0);
    gridLayout->addWidget(this->labelSelectionComboBox, 3, 1);
    gridLayout->addWidget(editLabelsToolButton, 3, 2);
    gridLayout->addWidget(metricValueLabel, 4, 0, Qt::AlignLeft);
    gridLayout->addWidget(this->metricValueSpinBox, 4, 1);

    this->setMapFileTypeComboBoxCurrentIndex(0);
    
    this->applyPreviousSelection(true);
}

/**
 * Destructor.
 */
CaretMappableDataFileAndMapSelector::~CaretMappableDataFileAndMapSelector()
{
    
}

/**
 * Load the map file combo box.
 *
 * @param selectedFileIndex
 *    Index of selected file.
 */
void 
CaretMappableDataFileAndMapSelector::loadMapFileComboBox(const int32_t selectedFileIndex)
{
    /*
     * Fill widgets
     */
    std::vector<GiftiTypeFile*> mapFiles;
    
    const DataFileTypeEnum::Enum dataFileType = this->getSelectedMapFileType();
    
    switch (dataFileType) {
        case DataFileTypeEnum::LABEL:
        {
            std::vector<LabelFile*> labelFiles;
            brainStructure->getLabelFiles(labelFiles);
            mapFiles.insert(mapFiles.end(),
                            labelFiles.begin(),
                            labelFiles.end());
        }
            break;
        case DataFileTypeEnum::METRIC:
        {
            std::vector<MetricFile*> metricFiles;
            brainStructure->getMetricFiles(metricFiles);
            mapFiles.insert(mapFiles.end(),
                            metricFiles.begin(),
                            metricFiles.end());
        }
            break;
        default:
            CaretAssertMessage(0,
                               ("File Type "
                                + DataFileTypeEnum::toName(dataFileType)
                                + " not allowed."));
            break;
    }
    
    this->mapFileComboBox->clear();
    
    for (std::vector<GiftiTypeFile*>::iterator iter = mapFiles.begin();
         iter != mapFiles.end();
         iter++) {
        GiftiTypeFile* gtf = *iter;
        this->mapFileComboBox->addItem(gtf->getFileNameNoPath(),
                                       qVariantFromValue((void*)gtf));
    }
    
    if ((selectedFileIndex >= 0) 
        && (selectedFileIndex < this->mapFileComboBox->count())) {
        this->setMapFileComboBoxCurrentIndex(selectedFileIndex);
    }
    
    this->loadMapNameComboBox(0);
}


/**
 * Called when the user makes a selection from the map file type combo box.
 * A signal indicating that the selections have changed will be emitted.
 *
 * @param indx
 *   Index of item selected.
 */
void 
CaretMappableDataFileAndMapSelector::mapFileTypeComboBoxSelected(int indx)
{
    this->setMapFileTypeComboBoxCurrentIndex(indx);
    this->applyPreviousSelection(false);
    emit selectionChanged(this);
}

/**
 * Set the map file type combo box to the given index.
 * @param indx
 *    Index for selection.
 */
void 
CaretMappableDataFileAndMapSelector::setMapFileTypeComboBoxCurrentIndex(int indx)
{
    this->mapFileTypeComboBox->setCurrentIndex(indx);
    const int32_t integerCode = this->mapFileTypeComboBox->itemData(indx).toInt();
    const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromIntegerCode(integerCode, NULL);
    this->updateFileTypeSelections(dataFileType);
    this->loadMapFileComboBox(0); 
}

/**
 * Update the selections for specific file types.
 */
void 
CaretMappableDataFileAndMapSelector::updateFileTypeSelections(const DataFileTypeEnum::Enum dataFileType)
{
    switch (dataFileType) {
        case DataFileTypeEnum::LABEL:
            this->labelValueControlsGroup->setVisible(true);
            this->metricValueControlsGroup->setVisible(false);
            break;
        case DataFileTypeEnum::METRIC:
            this->labelValueControlsGroup->setVisible(false);
            this->metricValueControlsGroup->setVisible(true);
            break;
        default:
            CaretAssertMessage(0,
                               ("File Type "
                                + DataFileTypeEnum::toName(dataFileType)
                                + " not allowed."));
            break;            
    }
}


/**
 * Called when the user makes a selection from the map file combo box.
 * A signal indicating that the selections have changed will be emitted.
 *
 * @param indx
 *   Index of item selected.
 */
void 
CaretMappableDataFileAndMapSelector::mapFileComboBoxSelected(int indx)
{
    this->setMapFileComboBoxCurrentIndex(indx);
    emit selectionChanged(this);
}

/**
 * Set the map file combo box to the given index.
 * @param indx
 *    Index for selection.
 */
void 
CaretMappableDataFileAndMapSelector::setMapFileComboBoxCurrentIndex(int indx)
{
    this->mapFileComboBox->setCurrentIndex(indx);
    
    this->loadMapNameComboBox(0);
    this->loadLabelNameComboBox();
}

/**
 * Set the map name combo box to the given index.
 * @param indx
 *    Index for selection.
 */
void 
CaretMappableDataFileAndMapSelector::setMapNameComboBoxCurrentIndex(int indx)
{
    this->mapNameComboBox->setCurrentIndex(indx);
}

/**
 * Load the contents of the map name combo box.
 * @param selectedMapIndex
 *    Index of the map that is selected.
 */
void 
CaretMappableDataFileAndMapSelector::loadMapNameComboBox(const int32_t selectedMapIndex)
{
    this->mapNameComboBox->clear();
    
    CaretMappableDataFile* cmdf = this->getSelectedMapFile();
    if (cmdf != NULL) {
        const int32_t numMaps = cmdf->getNumberOfMaps();
        for (int32_t i = 0; i < numMaps; i++) {
            this->mapNameComboBox->addItem(cmdf->getMapName(i));
        }
    }
    
    this->mapNameComboBox->setCurrentIndex(selectedMapIndex);
}

/**
 * Called when the user makes a selection from the map name combo box.
 *
 * @param indx
 *   Index of item selected.
 */
void 
CaretMappableDataFileAndMapSelector::mapNameComboBoxSelected(int /*indx*/)
{
    emit selectionChanged(this);
}

/**
 * Called when the new map file tool button is pressed.
 */
void 
CaretMappableDataFileAndMapSelector::newMapFileToolButtonSelected()
{
    const DataFileTypeEnum::Enum dataFileType = this->getSelectedMapFileType();
    
    QString fileExtension = "";
    switch (dataFileType) {
        case DataFileTypeEnum::LABEL:
            fileExtension = DataFileTypeEnum::toFileExtension(DataFileTypeEnum::LABEL);
            break;
        case DataFileTypeEnum::METRIC:
            fileExtension = DataFileTypeEnum::toFileExtension(DataFileTypeEnum::METRIC);
            break;
        default:
            CaretAssertMessage(0,
                               ("File Type "
                                + DataFileTypeEnum::toName(dataFileType)
                                + " not allowed."));
            break;
    }
    QString newFileName = "NewFile." + fileExtension;

    WuQDataEntryDialog newFileMapDialog("New Map File and Map",
                                        this->widget);
    QLineEdit* mapFileNameLineEdit = newFileMapDialog.addLineEditWidget("New Map File Name", 
                                                                        newFileName);
    QLineEdit* mapNameLineEdit     = newFileMapDialog.addLineEditWidget("New Map Name", 
                                                                        "Map Name");
    
    if (newFileMapDialog.exec() == WuQDataEntryDialog::Accepted) {
        QString mapFileName   = mapFileNameLineEdit->text();
        const QString mapName = mapNameLineEdit->text();
        
        try {
            if (mapFileName.endsWith(fileExtension) == false) {
                mapFileName += ("."
                                + fileExtension);
            }

            const int32_t numberOfNodes = this->brainStructure->getNumberOfNodes();

            int32_t fileIndex = 0;
            
            switch (dataFileType) {
                case DataFileTypeEnum::LABEL:
                {
                    LabelFile* labelFile = new LabelFile();
                    labelFile->setNumberOfNodesAndColumns(numberOfNodes, 1);
                    labelFile->setMapName(0, mapName);
                    labelFile->setStructure(this->brainStructure->getStructure());
                    labelFile->setFileName(mapFileName);
                    this->brainStructure->addLabelFile(labelFile,
                                                       false);
                    fileIndex = this->brainStructure->getNumberOfLabelFiles() - 1;
                    brainStructure->getBrain()->getSpecFile()->addCaretDataFile(labelFile);
                    brainStructure->getBrain()->getSpecFile()->addDataFile(dataFileType,
                                            brainStructure->getStructure(),
                                            mapFileName,
                                            true,
                                            true,
                                            false);
                }
                    break;
                case DataFileTypeEnum::METRIC:
                {
                    MetricFile* metricFile = new MetricFile();
                    metricFile->setNumberOfNodesAndColumns(numberOfNodes, 1);
                    metricFile->setMapName(0, mapName);
                    metricFile->setStructure(this->brainStructure->getStructure());
                    metricFile->setFileName(mapFileName);
                    this->brainStructure->addMetricFile(metricFile,
                                                        false);
                    fileIndex = this->brainStructure->getNumberOfMetricFiles() - 1;
                    brainStructure->getBrain()->getSpecFile()->addCaretDataFile(metricFile);//TSC: I stole this code out of Brain::readOrReloadDataFile and added some includes to get it working
                    brainStructure->getBrain()->getSpecFile()->addDataFile(dataFileType,
                                            brainStructure->getStructure(),
                                            mapFileName,
                                            true,
                                            true,
                                            false);
                }
                    break;
                default:
                    CaretAssertMessage(0,
                                       ("File Type "
                                        + DataFileTypeEnum::toName(dataFileType)
                                        + " not allowed."));
                    break;
            }
            
            
            this->loadMapFileComboBox(fileIndex);
            this->setMapFileComboBoxCurrentIndex(fileIndex);
            this->setMapNameComboBoxCurrentIndex(0);
            
            emit selectionChanged(this);
        }
        catch (const DataFileException& dfe) {
            WuQMessageBox::errorOk(this->widget, 
                                   dfe.whatString());
        }
    }
}

/**
 * @return The widget is for displaying this selector.
 */
QWidget* 
CaretMappableDataFileAndMapSelector::getWidget()
{
    return this->widget;
}

/**
 * Called when the new map tool button is pressed.
 */
void 
CaretMappableDataFileAndMapSelector::newMapToolButtonSelected()
{
    GiftiTypeFile* gtf = this->getSelectedMapFile();
    if (gtf != NULL) {
        if (gtf->getNumberOfNodes() <= 0) {
            WuQMessageBox::errorOk(this->getWidget(), 
                                   "The selected file is invalid, use the New button to create a new file.");
            return;
        }
        
        bool valid = false;
        const QString newMapName = QInputDialog::getText(this->mapFileComboBox, 
                                                         "Map Name", 
                                                         "Map Name", 
                                                         QLineEdit::Normal, 
                                                         "map", &valid);
        if (valid) {
            try {
                gtf->addMaps(gtf->getNumberOfNodes(),
                             1);
                
                const int32_t mapIndex = gtf->getNumberOfMaps() - 1;
                gtf->setMapName(mapIndex, newMapName);
                this->loadMapNameComboBox(mapIndex);
                
                this->loadLabelNameComboBox();
            }
            catch (const DataFileException& e) {
                WuQMessageBox::errorOk(this->getWidget(), 
                                       e.whatString());
            }
        }
    }
    else {
        WuQMessageBox::errorOk(this->getWidget(), 
                               "The selected file is invalid, use the New button to create a new file.");
    }
}

/**
 * @return   The selected map file.  Value will be NULL
 * if no map file is selected.
 */
GiftiTypeFile* 
CaretMappableDataFileAndMapSelector::getSelectedMapFile()
{
    const int indx = this->mapFileComboBox->currentIndex();
    if ((indx >= 0) 
        && (indx < this->mapFileComboBox->count())) {
        void* pointer = this->mapFileComboBox->itemData(indx).value<void*>();
        GiftiTypeFile* gtf = (GiftiTypeFile*)pointer;
        return gtf;
    }
    
    return NULL;
}

/**
 * @return The index of the selected map.  Will be negative 
 * value if there is no map file or the map file contains
 * no maps.
 */
int32_t 
CaretMappableDataFileAndMapSelector::getSelectedMapIndex()
{
    const int indx = this->mapNameComboBox->currentIndex();
    CaretMappableDataFile* cmdf = this->getSelectedMapFile();
    if (cmdf != NULL) {
        if ((indx >= 0)
            && (indx < cmdf->getNumberOfMaps())) {
            return indx;
        }
    }
    
    return -1;
}

/**
 * @return The name of the selected map file type.
 */
AString 
CaretMappableDataFileAndMapSelector::getNameOfSelectedMapFileType()
{
    const DataFileTypeEnum::Enum dataFileType = this->getSelectedMapFileType();
    const AString name = DataFileTypeEnum::toGuiName(dataFileType);
    return name;
}

/**
 * @return The selected map file type.
 */
DataFileTypeEnum::Enum 
CaretMappableDataFileAndMapSelector::getSelectedMapFileType() const
{
    const int indx = this->mapFileTypeComboBox->currentIndex();
    if ((indx >= 0) 
        && (indx < this->mapFileTypeComboBox->count())) {
        const int32_t integerCode = this->mapFileTypeComboBox->itemData(indx).toInt();
        DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromIntegerCode(integerCode, 
                                                                                NULL);
        return dataFileType;
    }
    
    CaretAssertMessage(0, "Invalid data file type");
    return DataFileTypeEnum::UNKNOWN;
}

/**
 * Are the selections valid?
 * @param errorMessageOut
 *    Contains error messages on output if selections
 *    are not valid.
 * @return true if the selections are valid, else false.
 */
bool 
CaretMappableDataFileAndMapSelector::isValidSelections(AString& errorMessageOut)
{
    errorMessageOut = "";
    
    if (this->getSelectedMapFileType() == DataFileTypeEnum::UNKNOWN) {
        errorMessageOut = "Selected file type is invalid.";
    }
    else if (this->getSelectedMapFile() == NULL) {
        errorMessageOut = "No map file is selected, create a file with New button.";
    }
    else if (this->getSelectedMapIndex() < 0) {
        errorMessageOut = "No map is selected, create a map with New button.";
    }
    
    const bool valid = errorMessageOut.isEmpty();
    return valid;
}

/**
 * Called when a selection is made from the label combo box.
 * @param indx
 *    Index of selection.
 */
void 
CaretMappableDataFileAndMapSelector::labelNameComboBoxSelected(int /*indx*/)
{
    emit selectionChanged(this);
}

/**
 * Called when a selection is made from the label combo box.
 * @param value
 *    New value.
 */
void 
CaretMappableDataFileAndMapSelector::metricValueChanged(double /*value*/)
{
    emit selectionChanged(this);
}


/**
 * Load the label names into the label selection combo box.
 */
void 
CaretMappableDataFileAndMapSelector::loadLabelNameComboBox()
{
    const int32_t selectedKey = this->getSelectedLabelKey();
    
    this->labelSelectionComboBox->clear();
    
    if (this->getSelectedMapFileType() == DataFileTypeEnum::LABEL) {
        GiftiTypeFile* gtf = this->getSelectedMapFile();
        LabelFile* labelFile = dynamic_cast<LabelFile*>(gtf);
        if (labelFile != NULL) {
            GiftiLabelTable* labelTable = labelFile->getLabelTable();
            
            int32_t selectedIndex = 0;
            
            std::vector<int32_t> labelKeys = labelTable->getLabelKeysSortedByName();
            const int numKeys = static_cast<int32_t>(labelKeys.size());
            for (int32_t i = 0; i < numKeys; i++) {
                const int32_t key = labelKeys[i];
                if (selectedKey == key) {
                    selectedIndex = i;
                }
                const AString name = labelTable->getLabelName(key);
                this->labelSelectionComboBox->addItem(name,
                                                      (int)key);
            }
            
            if ((selectedIndex >= 0)
                && (selectedIndex < this->labelSelectionComboBox->count())) {
                this->labelSelectionComboBox->setCurrentIndex(selectedIndex);
            }
        }
    }
}

/**
 * Display the label editor.
 */
void 
CaretMappableDataFileAndMapSelector::showLabelsEditor()
{
    if (this->getSelectedMapFileType() == DataFileTypeEnum::LABEL) {
        GiftiTypeFile* gtf = this->getSelectedMapFile();
        LabelFile* labelFile = dynamic_cast<LabelFile*>(gtf);
        if (labelFile != NULL) {
            GiftiLabelTable* labelTable = labelFile->getLabelTable();
            GiftiLabelTableEditor labelsEditor(labelTable,
                                               "Edit Labels",
                                               GiftiLabelTableEditor::OPTION_NONE,
                                               this->getWidget());

            const int dialogResult = labelsEditor.exec();
            this->loadLabelNameComboBox();
            
            if (dialogResult == GiftiLabelTableEditor::Accepted) {
                
                const AString labelName = labelsEditor.getLastSelectedLabelName();
                const int32_t labelKey  = labelTable->getLabelKeyFromName(labelName);
                
                const int labelIndex = this->labelSelectionComboBox->findData((int)labelKey);
                if (labelIndex >= 0) {
                    if (labelKey != labelTable->getUnassignedLabelKey()) {
                        this->labelSelectionComboBox->setCurrentIndex(labelIndex);
                    }
                }
            }
        }
    }
}

/**
 * @return The metric value from the metric value spin box.
 */
float 
CaretMappableDataFileAndMapSelector::getSelectedMetricValue() const
{
    const float value = this->metricValueSpinBox->value();
    return value;
}

/**
 * @return Key of the selected label.
 */
int32_t 
CaretMappableDataFileAndMapSelector::getSelectedLabelKey() const
{
    int32_t key = 0;
    
    const int indx = this->labelSelectionComboBox->currentIndex();
    if ((indx >= 0) 
        && (indx < this->labelSelectionComboBox->count())) {
        key = this->labelSelectionComboBox->itemData(indx).toInt();
    }
    
    return key;
}

/**
 * @return Name of the selected label, empty string if no selection.
 */
AString 
CaretMappableDataFileAndMapSelector::getSelectedLabelName() const
{
    AString name = "";
    const int indx = this->labelSelectionComboBox->currentIndex();
    if ((indx >= 0) 
        && (indx < this->labelSelectionComboBox->count())) {
        name = this->labelSelectionComboBox->currentText();
    }
    return name;
}

/**
 * Apply previous selections when dialog launched or
 * file type changed.
 */
void 
CaretMappableDataFileAndMapSelector::applyPreviousSelection(const bool useFileTypeFromPreviousSelection)
{
    if (this->brainStructure == NULL) {
        return;
    }
    const StructureEnum::Enum structure = this->brainStructure->getStructure();
    Selections* s = this->getPreviousSelection(structure);
    
    DataFileTypeEnum::Enum dataFileType = s->dataFileType;
    if (useFileTypeFromPreviousSelection == false) {
        dataFileType = this->getSelectedMapFileType();
    }
    if (dataFileType == DataFileTypeEnum::UNKNOWN) { 
        dataFileType = DataFileTypeEnum::LABEL;
        
        if (this->brainStructure->getNumberOfLabelFiles() > 0) {
            dataFileType = DataFileTypeEnum::LABEL;
        }
        else if (this->brainStructure->getNumberOfMetricFiles() > 0) {
            dataFileType = DataFileTypeEnum::METRIC;
        }
    }
    
    const int mapTypeIndex = this->mapFileTypeComboBox->findData((int)dataFileType);
    if (mapTypeIndex >= 0) {
        this->setMapFileTypeComboBoxCurrentIndex(mapTypeIndex);
    }
    
    switch (dataFileType) {
        case DataFileTypeEnum::LABEL:
        {
            if (s->labelFile != NULL) {
                const int fileIndex = this->mapFileComboBox->findData(qVariantFromValue((void*)s->labelFile));
                if (fileIndex >= 0) {
                    this->setMapFileComboBoxCurrentIndex(fileIndex);
                    
                    const int mapIndex = s->labelFile->CaretMappableDataFile::getMapIndexFromName(s->labelMapName);
                    if (mapIndex >= 0) {
                        this->setMapNameComboBoxCurrentIndex(mapIndex);
                    }
                    
                    if (s->labelName.isEmpty() == false) {
                        const int labelIndex = labelSelectionComboBox->findText(s->labelName);
                        if (labelIndex >= 0) {
                            this->labelSelectionComboBox->setCurrentIndex(labelIndex);
                        }
                    }
                }
            }
        }
            break;
        case DataFileTypeEnum::METRIC:
        {
            if (s->metricFile != NULL) {
                const int fileIndex = this->mapFileComboBox->findData(qVariantFromValue((void*)s->metricFile));
                if (fileIndex >= 0) {
                    this->setMapFileComboBoxCurrentIndex(fileIndex);
                    
                    const int mapIndex = s->metricFile->CaretMappableDataFile::getMapIndexFromName(s->metricMapName);
                    if (mapIndex >= 0) {
                        this->setMapNameComboBoxCurrentIndex(mapIndex);
                    }
                    
                    this->metricValueSpinBox->setValue(s->metricValue);
                }
            }
        }
            break;
        default:
            /*
             * If here, previous selections have not been initilized
             */
            break;
    }    
}

/**
 * Save the current selections.  User of this selector
 * MUST call this to save the selections so that they 
 * can be initialized the next time a selector for the
 * brain structure is used.
 */
void 
CaretMappableDataFileAndMapSelector::saveCurrentSelections()
{
    if (this->brainStructure == NULL) {
        return;
    }
    const StructureEnum::Enum structure = this->brainStructure->getStructure();
    Selections* s = this->getPreviousSelection(structure);
    
    const DataFileTypeEnum::Enum dataFileType = this->getSelectedMapFileType();
    s->dataFileType = dataFileType;
    switch (dataFileType) {
        case DataFileTypeEnum::LABEL:
        {
            LabelFile* lf = dynamic_cast<LabelFile*>(this->getSelectedMapFile());
            if (lf != NULL) {
                s->labelFile = lf;
                const int32_t mapIndex = this->getSelectedMapIndex();
                if ((mapIndex >= 0)
                    && (mapIndex < lf->getNumberOfMaps())) {
                    s->labelMapName = lf->getMapName(mapIndex);
                }
                s->labelName = this->labelSelectionComboBox->currentText();
            }
        }
            break;
        case DataFileTypeEnum::METRIC:
        {
            MetricFile* mf = dynamic_cast<MetricFile*>(this->getSelectedMapFile());
            if (mf != NULL) {
                s->metricFile = mf;
                const int32_t mapIndex = this->getSelectedMapIndex();
                if ((mapIndex >= 0)
                    && (mapIndex < mf->getNumberOfMaps())) {
                    s->metricMapName = mf->getMapName(mapIndex);
                }
                s->metricValue = this->metricValueSpinBox->value();
            }
        }
            break;
        default:
            CaretAssertMessage(0,
                               ("File Type "
                                + DataFileTypeEnum::toName(dataFileType)
                                + " not allowed."));
            break;
    }
}


/**
 * Get the previous selections for a structure.  If a previous selections does not
 * exist for the structure it is created and returned.
 * @param structure
 *    Structure for which previous selections are desired.
 * @return
 *    Selections object.
 */ 
CaretMappableDataFileAndMapSelector::Selections* 
CaretMappableDataFileAndMapSelector::getPreviousSelection(const StructureEnum::Enum structure)
{
    std::map<StructureEnum::Enum, Selections*>::iterator iter = previousSelections.find(structure);
    if (iter != previousSelections.end()) {
        Selections* s = iter->second;
        return s;
    }
    
    Selections* s = new Selections();
    previousSelections.insert(std::make_pair(structure, s));
    return s;
}



