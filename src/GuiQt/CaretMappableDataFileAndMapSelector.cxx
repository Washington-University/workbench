
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
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "DataFileException.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "GiftiLabel.h"
#include "GiftiLabelTableEditor.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "GuiManager.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "SpecFile.h"
#include "StructureEnumComboBox.h"
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
 * @param defaultName
 *    Default name for file.
 * @param brain
 *    Brain structure that contains map files.
 * @param structure
 *    Type of map file.
 * @param parent
 *    The parent.
 */
CaretMappableDataFileAndMapSelector::CaretMappableDataFileAndMapSelector(const AString defaultName,
                                                                         Brain* brain,
                                                                         const std::vector<DataFileTypeEnum::Enum>& supportedMapFileTypes,
                                                                         const std::vector<StructureEnum::Enum>& supportedStructures,
                                                                         QObject* parent)
: WuQWidget(parent)
{
    m_brain = brain;
    m_defaultName = defaultName;
    m_newMapAction = NULL;

    m_supportedMapFileTypes = supportedMapFileTypes;
    m_supportedStructures   = supportedStructures;
    
    m_mapFileTypesThatAllowAddingMaps.push_back(DataFileTypeEnum::LABEL);
    m_mapFileTypesThatAllowAddingMaps.push_back(DataFileTypeEnum::METRIC);
    
    /*
     * File Type
     */
    QLabel* mapFileTypeLabel = new QLabel("File Type: ");
    this->mapFileTypeComboBox = new QComboBox();
    for (std::vector<DataFileTypeEnum::Enum>::iterator dataFileTypeIterator = m_supportedMapFileTypes.begin();
         dataFileTypeIterator != m_supportedMapFileTypes.end();
         dataFileTypeIterator++) {
        const DataFileTypeEnum::Enum dataFileType = *dataFileTypeIterator;
        const AString name = DataFileTypeEnum::toGuiName(dataFileType);
        this->mapFileTypeComboBox->addItem(name,
                                           (int)DataFileTypeEnum::toIntegerCode(dataFileType));
    }
    QObject::connect(this->mapFileTypeComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapFileTypeComboBoxSelected(int)));
    
    /*
     * Structure
     */
    QLabel* mapFileStructureLabel = new QLabel("Structure: ");
    m_mapFileStructureComboBox = new StructureEnumComboBox(this);
    m_mapFileStructureComboBox->listOnlyTheseStructures(supportedStructures);
    if ( ! supportedStructures.empty()) {
        m_mapFileStructureComboBox->setSelectedStructure(supportedStructures[0]);
    }
    QObject::connect(m_mapFileStructureComboBox, SIGNAL(structureSelected(const StructureEnum::Enum)),
                     this, SLOT(mapFileStructureComboBoxSelected(const StructureEnum::Enum)));
    
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
    m_newMapAction = WuQtUtilities::createAction("New...", 
                                                        "", 
                                                        this, 
                                                        this, 
                                                        SLOT(newMapToolButtonSelected()));
    QToolButton* newMapToolButton     = new QToolButton();
    newMapToolButton->setDefaultAction(m_newMapAction);
    
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
     * Float Value Entry
     */
    QLabel* floatValueLabel = new QLabel("Value: ");
    m_floatValueSpinBox = WuQFactory::newDoubleSpinBox();
    m_floatValueSpinBox->setMaximumWidth(100);
    m_floatValueSpinBox->setRange(-std::numeric_limits<float>::max(), 
                                       std::numeric_limits<float>::max());
    m_floatValueSpinBox->setValue(1.0);
    QObject::connect(m_floatValueSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(floatValueChanged(double)));
    m_floatValueControlsGroup = new WuQWidgetObjectGroup(this);
    m_floatValueControlsGroup->add(floatValueLabel);
    m_floatValueControlsGroup->add(m_floatValueSpinBox);
    
    /*
     * Layout widgets
     */
    this->widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(this->widget);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 
                                    4, 
                                    2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->setColumnStretch(2, 0);
    int32_t rowIndex = gridLayout->rowCount();
    gridLayout->addWidget(mapFileTypeLabel, rowIndex, 0);
    gridLayout->addWidget(this->mapFileTypeComboBox, rowIndex, 1);
    rowIndex++;
    gridLayout->addWidget(mapFileStructureLabel, rowIndex, 0);
    gridLayout->addWidget(m_mapFileStructureComboBox->getWidget(), rowIndex, 1);
    rowIndex++;
    gridLayout->addWidget(mapFileLabel, rowIndex, 0);
    gridLayout->addWidget(this->mapFileComboBox, rowIndex, 1);
    gridLayout->addWidget(newMapFileToolButton, rowIndex, 2);
    rowIndex++;
    gridLayout->addWidget(mapNameLabel, rowIndex, 0);
    gridLayout->addWidget(this->mapNameComboBox, rowIndex, 1);
    gridLayout->addWidget(newMapToolButton, rowIndex, 2);
    rowIndex++;
    gridLayout->addWidget(labelNameLabel, rowIndex, 0);
    gridLayout->addWidget(this->labelSelectionComboBox, rowIndex, 1);
    gridLayout->addWidget(editLabelsToolButton, rowIndex, 2);
    rowIndex++;
    gridLayout->addWidget(floatValueLabel, rowIndex, 0, Qt::AlignLeft);
    gridLayout->addWidget(m_floatValueSpinBox, rowIndex, 1);
    rowIndex++;

    this->setMapFileTypeComboBoxCurrentIndex(0);
    
    loadLastSelectionsForFileType(DataFileTypeEnum::UNKNOWN);
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
    std::vector<CaretMappableDataFile*> matchingMapFiles;
    
    const DataFileTypeEnum::Enum selectedDataFileType = this->getSelectedMapFileType();
    const StructureEnum::Enum selectedStructure = this->getSelectedMapFileStructure();
    
    std::vector<CaretMappableDataFile*> caretMappableDataFiles;
    m_brain->getAllMappableDataFiles(caretMappableDataFiles);
    
    for (std::vector<CaretMappableDataFile*>::iterator iter = caretMappableDataFiles.begin();
         iter != caretMappableDataFiles.end();
         iter++) {
        CaretMappableDataFile* caretMapFile = *iter;
        const DataFileTypeEnum::Enum fileType = caretMapFile->getDataFileType();
        
        if (selectedDataFileType == fileType) {
            if (caretMapFile->isMappableToSurfaceStructure(selectedStructure)) {
                matchingMapFiles.push_back(caretMapFile);
            }
        }
    }
    
    this->mapFileComboBox->clear();
    
    for (std::vector<CaretMappableDataFile*>::iterator iter = matchingMapFiles.begin();
         iter != matchingMapFiles.end();
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
    loadLastSelectionsForFileType(this->getSelectedMapFileType());
    enableDisableNewMapAction();
    emit selectionChanged(this);
}

/**
 * Enable/disable the new map action.
 */
void
CaretMappableDataFileAndMapSelector::enableDisableNewMapAction()
{
    if (m_newMapAction != NULL) {
        const DataFileTypeEnum::Enum dataFileType = this->getSelectedMapFileType();
        
        if (std::find(m_mapFileTypesThatAllowAddingMaps.begin(),
                      m_mapFileTypesThatAllowAddingMaps.end(),
                      dataFileType) != m_mapFileTypesThatAllowAddingMaps.end()) {
            m_newMapAction->setEnabled(true);
        }
        else {
            m_newMapAction->setEnabled(false);
        }
    }
}

/**
 * Called when the structure is changed.
 *
 * @param structure
 *    Selected structure.
 */
void
CaretMappableDataFileAndMapSelector::mapFileStructureComboBoxSelected(const StructureEnum::Enum /*structure*/)
{
    loadLastSelectionsForFileType(getSelectedMapFileType());
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
    bool showLabelSelectionWidgets = false;
    bool showScalarSelectionWidgets = false;
    
    switch (dataFileType) {
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            showLabelSelectionWidgets = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            showScalarSelectionWidgets = true;
            break;
        case DataFileTypeEnum::LABEL:
            showLabelSelectionWidgets = true;
            break;
        case DataFileTypeEnum::METRIC:
            showScalarSelectionWidgets = true;
            break;
        default:
            break;
    }
    
    labelValueControlsGroup->setVisible(showLabelSelectionWidgets);
    m_floatValueControlsGroup->setVisible(showScalarSelectionWidgets);
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
        
        if ((selectedMapIndex >= 0)
            && (selectedMapIndex < numMaps)) {
            this->mapNameComboBox->setCurrentIndex(selectedMapIndex);
        }
    }
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
    QString fileExtension = DataFileTypeEnum::toFileExtension(dataFileType);
    
    QString newFileName = "NewFile";
    QString newMapName  = "Map Name";
    if (m_defaultName.isEmpty() == false) {
        newFileName = m_defaultName;
        newMapName  = m_defaultName;
    }
    newFileName += ("."
                    + fileExtension);

    WuQDataEntryDialog newFileMapDialog("New Map File and Map",
                                        this->widget);
    QLineEdit* mapFileNameLineEdit = newFileMapDialog.addLineEditWidget("New Map File Name", 
                                                                        newFileName);
    QLineEdit* mapNameLineEdit     = newFileMapDialog.addLineEditWidget("New Map Name", 
                                                                        newMapName);
    
    if (newFileMapDialog.exec() == WuQDataEntryDialog::Accepted) {
        QString mapFileName   = mapFileNameLineEdit->text();
        const QString mapName = mapNameLineEdit->text();
        
        try {
            if (mapFileName.endsWith(fileExtension) == false) {
                mapFileName += ("."
                                + fileExtension);
            }

            const StructureEnum::Enum structure = getSelectedMapFileStructure();
            BrainStructure* brainStructure = GuiManager::get()->getBrain()->getBrainStructure(structure,
                                                                                              false);
            if (brainStructure == NULL) {
                throw DataFileException(newFileName,
                                        "Invalid brain structure (not loaded): "
                                        + StructureEnum::toGuiName(structure));
            }
            const int32_t numberOfNodes = brainStructure->getNumberOfNodes();
            if (numberOfNodes <= 0) {
                throw DataFileException(newFileName,
                                        "Invalid number of nodes (0) in brain structure: "
                                        + StructureEnum::toGuiName(structure));
            }

            switch (dataFileType) {
                case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                {
                    AString errorMessage;
                    CiftiMappableDataFile* ciftiMappableDataFile =
                    CiftiMappableDataFile::newInstanceForCiftiFileTypeAndSurface(DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL,
                                                                                 structure,
                                                                                 numberOfNodes,
                                                                                 errorMessage);
                    if (ciftiMappableDataFile == NULL) {
                        throw DataFileException(mapFileName,
                                                errorMessage);
                    }
                    
                    CaretAssert(dynamic_cast<CiftiBrainordinateLabelFile*>(ciftiMappableDataFile) != NULL);
                    
                    if (ciftiMappableDataFile != NULL) {
                        ciftiMappableDataFile->setMapName(0,
                                                          mapName);
                        ciftiMappableDataFile->setFileName(mapFileName);
                        EventManager::get()->sendEvent(EventDataFileAdd(ciftiMappableDataFile).getPointer());
                    }
                    else {
                        throw DataFileException(mapFileName,
                                                errorMessage);
                    }
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                {
                    AString errorMessage;
                    CiftiMappableDataFile* ciftiMappableDataFile =
                        CiftiMappableDataFile::newInstanceForCiftiFileTypeAndSurface(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR,
                                                                                     structure,
                                                                        numberOfNodes,
                                                                        errorMessage);
                    if (ciftiMappableDataFile == NULL) {
                        throw DataFileException(mapFileName,
                                                errorMessage);
                    }
                    
                    CaretAssert(dynamic_cast<CiftiBrainordinateScalarFile*>(ciftiMappableDataFile) != NULL);
                    
                    if (ciftiMappableDataFile != NULL) {
                        ciftiMappableDataFile->setMapName(0,
                                                    mapName);
                        ciftiMappableDataFile->setFileName(mapFileName);
                        EventManager::get()->sendEvent(EventDataFileAdd(ciftiMappableDataFile).getPointer());
                    }
                    else {
                        throw DataFileException(mapFileName,
                                                errorMessage);
                    }
                }
                    break;
                case DataFileTypeEnum::LABEL:
                {
                        LabelFile* labelFile = new LabelFile();
                        labelFile->setNumberOfNodesAndColumns(numberOfNodes, 1);
                        labelFile->setMapName(0, mapName);
                        labelFile->setStructure(structure);
                        labelFile->setFileName(mapFileName);
                    EventManager::get()->sendEvent(EventDataFileAdd(labelFile).getPointer());
                }
                    break;
                case DataFileTypeEnum::METRIC:
                {
                        MetricFile* metricFile = new MetricFile();
                        metricFile->setNumberOfNodesAndColumns(numberOfNodes, 1);
                        metricFile->setMapName(0, mapName);
                        metricFile->setStructure((structure));
                        metricFile->setFileName(mapFileName);
                    EventManager::get()->sendEvent(EventDataFileAdd(metricFile).getPointer());
                }
                    break;
                default:
                    CaretAssertMessage(0,
                                       ("File Type "
                                        + DataFileTypeEnum::toName(dataFileType)
                                        + " not allowed."));
                    break;
            }
            
            
            this->loadMapFileComboBox(0);
            const int numMapFiles = this->mapFileComboBox->count();
            if (numMapFiles > 0) {
                this->setMapFileComboBoxCurrentIndex(numMapFiles - 1);
            }
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
    CaretMappableDataFile* mapFile = this->getSelectedMapFile();
    if (mapFile != NULL) {
        const StructureEnum::Enum structure = getSelectedMapFileStructure();
        if ( ! mapFile->isMappableToSurfaceStructure(structure)) {
            WuQMessageBox::errorOk(this->getWidget(), 
                                   "The selected file cannot be mapped to the given structure, use the New button to create a new file.");
            return;
        }
        
        QString presetMapName  = "Map Name";
        if (m_defaultName.isEmpty() == false) {
            presetMapName  = m_defaultName;
        }
        
        bool valid = false;
        const QString newMapName = QInputDialog::getText(this->mapFileComboBox, 
                                                         "Map Name", 
                                                         "Map Name", 
                                                         QLineEdit::Normal, 
                                                         presetMapName,
                                                         &valid);
        if (valid) {
            try {
                GiftiTypeFile* gtf = dynamic_cast<GiftiTypeFile*>(mapFile);
                int32_t mapIndex = 0;
                if (gtf != NULL ) {
                    gtf->addMaps(gtf->getNumberOfNodes(),
                                 1);
                    
                    mapIndex = gtf->getNumberOfMaps() - 1;
                    gtf->setMapName(mapIndex, newMapName);
                    
                    this->loadMapNameComboBox(mapIndex);
                }
                else {
                    CaretAssertMessage(0, "Add support for non-GIFTI files !!!!");
                }

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
    int32_t indx = this->mapFileTypeComboBox->currentIndex();
    if (indx < 0) {
        indx = 0;
    }
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
 * @return The selected structure.
 */
StructureEnum::Enum
CaretMappableDataFileAndMapSelector::getSelectedMapFileStructure() const
{
    return m_mapFileStructureComboBox->getSelectedStructure();
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
CaretMappableDataFileAndMapSelector::floatValueChanged(double /*value*/)
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
    
    CaretMappableDataFile* cmdf = this->getSelectedMapFile();
    if (cmdf != NULL) {
        if (cmdf->isMappedWithLabelTable()) {
            const int32_t mapIndex = getSelectedMapIndex();
            if (mapIndex >= 0) {
                GiftiLabelTable* labelTable = cmdf->getMapLabelTable(mapIndex);
                
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
}

/**
 * Display the label editor.
 */
void 
CaretMappableDataFileAndMapSelector::showLabelsEditor()
{
    CaretMappableDataFile* cmdf = this->getSelectedMapFile();
    if (cmdf != NULL) {
        if (cmdf->isMappedWithLabelTable()) {
            const int32_t mapIndex = getSelectedMapIndex();
            if (mapIndex >= 0) {
                GiftiLabelTable* labelTable = cmdf->getMapLabelTable(mapIndex);
                GiftiLabelTableEditor labelsEditor(cmdf,
                                                   mapIndex,
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
}

/**
 * @return The metric value from the metric value spin box.
 */
float 
CaretMappableDataFileAndMapSelector::getSelectedMetricValue() const
{
    const float value = m_floatValueSpinBox->value();
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
 * Load the last selection that matches the given data file type.  If the file type is unknown,
 * then just find the last selection of any kind.
 *
 * @param dataFileTypeIn
 *    Last selected data file type.
 */
void
CaretMappableDataFileAndMapSelector::loadLastSelectionsForFileType(const DataFileTypeEnum::Enum dataFileTypeIn)
{
    DataFileTypeEnum::Enum dataFileType = dataFileTypeIn;
    StructureEnum::Enum structure = StructureEnum::INVALID;
    
    if (dataFileType != DataFileTypeEnum::UNKNOWN) {
        structure = getSelectedMapFileStructure();
    }
    PreviousSelection* ps = getPreviousSelection(dataFileType,
                                                 structure);
    
    
    //    if (dataFileType == DataFileTypeEnum::UNKNOWN) {
    //        dataFileType = DataFileTypeEnum::LABEL;
    //
    ////        if (this->brainStructure->getNumberOfLabelFiles() > 0) {
    ////            dataFileType = DataFileTypeEnum::LABEL;
    ////        }
    ////        else if (this->brainStructure->getNumberOfMetricFiles() > 0) {
    ////            dataFileType = DataFileTypeEnum::METRIC;
    ////        }
    //    }
    
    //    const int mapTypeIndex = this->mapFileTypeComboBox->findData((int)dataFileType);
    //    if (mapTypeIndex >= 0) {
    //        this->setMapFileTypeComboBoxCurrentIndex(mapTypeIndex);
    //    }
    
    if (ps != NULL) {
        const int dataFileTypeInt = DataFileTypeEnum::toIntegerCode(ps->m_dataFileType);
        const int mapFileTypeIndex = mapFileTypeComboBox->findData(dataFileTypeInt);
        if (mapFileTypeIndex >= 0) {
            this->setMapFileTypeComboBoxCurrentIndex(mapFileTypeIndex);
        }
        m_mapFileStructureComboBox->setSelectedStructure(ps->m_structure);
        
        const int fileIndex = this->mapFileComboBox->findData(qVariantFromValue((void*)ps->m_mapFile));
        if (fileIndex >= 0) {
            this->setMapFileComboBoxCurrentIndex(fileIndex);
            
            const int mapIndex = ps->m_mapFile->getMapIndexFromName(ps->m_mapName);
            if (mapIndex >= 0) {
                this->setMapNameComboBoxCurrentIndex(mapIndex);
            }
            
            if ( ! ps->m_labelName.isEmpty()) {
                this->labelSelectionComboBox->clear();
                loadLabelNameComboBox();
                const int labelIndex = labelSelectionComboBox->findText(ps->m_labelName);
                if (labelIndex >= 0) {
                    this->labelSelectionComboBox->setCurrentIndex(labelIndex);
                }
            }
            
            m_floatValueSpinBox->blockSignals(true);
            m_floatValueSpinBox->setValue(ps->m_scalarValue);
            m_floatValueSpinBox->blockSignals(false);
        }
    }
    
    enableDisableNewMapAction();
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
    DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::UNKNOWN;
    StructureEnum::Enum structure       = StructureEnum::INVALID;
    
    CaretMappableDataFile* cmdf = getSelectedMapFile();
    if (cmdf != NULL) {
        dataFileType = cmdf->getDataFileType();
        structure    = m_mapFileStructureComboBox->getSelectedStructure();
        
        AString mapName = cmdf->getMapName(getSelectedMapIndex());
        PreviousSelection* ps = new PreviousSelection(structure,
                                                      dataFileType,
                                                      cmdf,
                                                      mapName,
                                                      labelSelectionComboBox->currentText(),
                                                      m_floatValueSpinBox->value());
        previousSelections.push_back(ps);
    }
}


/**
 * Get the previous selections for a data file type and structure.
 *
 * @param dataFileType
 *    Data file type.  If UNKNOWN, match to any file type.
 * @param structure
 *    Structure for which previous selections are desired.
 * @return
 *    Pointer to matching previous selection or NULL if no match
 *    with a valid data file.
 */
CaretMappableDataFileAndMapSelector::PreviousSelection*
CaretMappableDataFileAndMapSelector::getPreviousSelection(const DataFileTypeEnum::Enum dataFileType,
                                                          const StructureEnum::Enum structure)
{
    std::vector<CaretMappableDataFile*> allMapFiles;
    GuiManager::get()->getBrain()->getAllMappableDataFiles(allMapFiles);
    
    for (std::vector<PreviousSelection*>::reverse_iterator iter = previousSelections.rbegin();
         iter != previousSelections.rend();
         iter++) {
        PreviousSelection* ps = *iter;
        
        bool matchFlag = false;
        if (dataFileType == DataFileTypeEnum::UNKNOWN) {
            matchFlag = true;
        }
        else if ((ps->m_dataFileType == dataFileType)
                 && (ps->m_structure == structure)) {
            matchFlag = true;
        }
        
        if (matchFlag) {
            if (std::find(allMapFiles.begin(),
                          allMapFiles.end(),
                          ps->m_mapFile) != allMapFiles.end()) {
                return ps;
            }
        }
    }
    
    return NULL;
}

