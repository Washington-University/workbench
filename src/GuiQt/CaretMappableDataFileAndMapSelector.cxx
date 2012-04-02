
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


#include <QAction>
#include <QComboBox>
#include <QGridLayout>
#include <QInputDialog>
#include <QLabel>
#include <QToolButton>
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

#define __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_DECLARE__
#include "CaretMappableDataFileAndMapSelector.h"
#undef __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_DECLARE__

#include "BrainStructure.h"
#include "CaretAssert.h"
#include "LabelFile.h"
#include "MetricFile.h"
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
: QObject(parent)
{
    CaretAssert(brainStructure);
    this->brainStructure = brainStructure;

    this->supportedMapFileTypes.push_back(DataFileTypeEnum::LABEL);
    this->supportedMapFileTypes.push_back(DataFileTypeEnum::METRIC);
    
    /*
     * Create widgets
     */
    QLabel* mapFileTypeLabel = new QLabel("Type: ");
    QLabel* mapFileLabel     = new QLabel("File: ");
    QLabel* mapNameLabel     = new QLabel("Map: ");
    
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
    
    this->mapFileComboBox = new QComboBox();
    QObject::connect(this->mapFileComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapFileComboBoxSelected(int)));
    
    this->mapNameComboBox     = new QComboBox();
    QObject::connect(this->mapNameComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapNameComboBoxSelected(int)));
    
    QAction* newMapFileAction = WuQtUtilities::createAction("New...", 
                                                            "", 
                                                            this, 
                                                            this, 
                                                            SLOT(newMapFileToolButtonSelected()));
    QToolButton* newMapFileToolButton = new QToolButton();
    newMapFileToolButton->setDefaultAction(newMapFileAction);

    QAction* newMapAction = WuQtUtilities::createAction("New...", 
                                                        "", 
                                                        this, 
                                                        this, 
                                                        SLOT(newMapToolButtonSelected()));
    QToolButton* newMapToolButton     = new QToolButton();
    newMapToolButton->setDefaultAction(newMapAction);
    
    /*
     * Layout widgets
     */
    this->widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(this->widget);
    WuQtUtilities::setLayoutMargins(gridLayout, 
                                    4, 
                                    2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->addWidget(mapFileTypeLabel, 0, 0);
    gridLayout->addWidget(this->mapFileTypeComboBox, 0, 1);
    gridLayout->addWidget(mapFileLabel, 1, 0);
    gridLayout->addWidget(this->mapFileComboBox, 1, 1);
    gridLayout->addWidget(newMapFileToolButton, 1, 2);
    gridLayout->addWidget(mapNameLabel, 2, 0);
    gridLayout->addWidget(this->mapNameComboBox, 2, 1);
    gridLayout->addWidget(newMapToolButton, 2, 2);

    this->setMapFileTypeComboBoxCurrentIndex(0);
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
    std::vector<CaretMappableDataFile*> mapFiles;
    
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
    
    for (std::vector<CaretMappableDataFile*>::iterator iter = mapFiles.begin();
         iter != mapFiles.end();
         iter++) {
        CaretMappableDataFile* cmdf = *iter;
        this->mapFileComboBox->addItem(cmdf->getFileNameNoPath(),
                                       qVariantFromValue((void*)cmdf));
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
    this->loadMapFileComboBox(0); 
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
        const QString mapFileName = mapFileNameLineEdit->text();
        const QString mapName     = mapNameLineEdit->text();
        
        try {
            if (newFileName.endsWith(fileExtension) == false) {
                newFileName += ("."
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
                    labelFile->setFileName(newFileName);
                    this->brainStructure->addLabelFile(labelFile); 
                    fileIndex = this->brainStructure->getNumberOfLabelFiles() - 1;
                }
                    break;
                case DataFileTypeEnum::METRIC:
                {
                    MetricFile* metricFile = new MetricFile();
                    metricFile->setNumberOfNodesAndColumns(numberOfNodes, 1);
                    metricFile->setMapName(0, mapName);
                    metricFile->setStructure(this->brainStructure->getStructure());
                    metricFile->setFileName(newFileName);
                    this->brainStructure->addMetricFile(metricFile);                
                    fileIndex = this->brainStructure->getNumberOfMetricFiles() - 1;
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
    CaretMappableDataFile* cmdf = this->getSelectedMapFile();
    if (cmdf != NULL) {
        bool valid = false;
        const QString newMapName = QInputDialog::getText(this->mapFileComboBox, 
                                                         "Map Name", 
                                                         "Map Name", 
                                                         QLineEdit::Normal, 
                                                         "map", &valid);
        if (valid) {
        }
    }
}

/**
 * @return   The selected map file.  Value will be NULL
 * if no map file is selected.
 */
CaretMappableDataFile* 
CaretMappableDataFileAndMapSelector::getSelectedMapFile()
{
    const int indx = this->mapFileComboBox->currentIndex();
    if ((indx >= 0) 
        && (indx < this->mapFileComboBox->count())) {
        void* pointer = this->mapFileComboBox->itemData(indx).value<void*>();
        CaretMappableDataFile* cmdf = (CaretMappableDataFile*)pointer;
        return cmdf;
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


