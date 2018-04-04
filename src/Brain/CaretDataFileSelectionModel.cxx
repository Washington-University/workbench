
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __CARET_DATA_FILE_SELECTION_MODEL_DECLARE__
#include "CaretDataFileSelectionModel.h"
#undef __CARET_DATA_FILE_SELECTION_MODEL_DECLARE__

#include "BorderFile.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretMappableDataFile.h"
#include "ChartableMatrixParcelInterface.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CaretDataFileSelectionModel 
 * \brief Selection model for a CaretDataFile.
 * \ingroup Brain
 *
 * Maintains selection of a type of CaretDataFile.  Used in the GUI
 * by CaretDataFileSelectionComboBox.
 */

/**
 * Constructor.  This constructor is private and one of the static
 * factory methods should be used to create new instances of 
 * this class.
 *
 * @param brain
 *    Brain containing the files.
 * @param structure
 *    Structure for the files.
 * @param fileMode
 *    File mode that indicates how files are chosen from the 'Brain'.
 */
CaretDataFileSelectionModel::CaretDataFileSelectionModel(Brain* brain,
                                                         const StructureEnum::Enum structure,
                                                         const FileMode fileMode)
: CaretObject(),
m_fileMode(fileMode),
m_brain(brain),
m_structure(structure)
{
    m_overrideOfAvailableFilesValid = false;
    
    CaretAssert(brain);
    
    m_sceneAssistant = new SceneClassAssistant();
    m_dataFileTypes.clear();
}

/**
 * Destructor.
 */
CaretDataFileSelectionModel::~CaretDataFileSelectionModel()
{
    delete m_sceneAssistant;
}

/**
 * Create a new instance of a Caret Data File Selection Model that 
 * selects files of the given Data File Type from the given Brain.
 *
 * @param brain
 *    Brain from which files are obtained.
 * @param dataFileType
 *    Type of the data file.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceForCaretDataFileType(Brain* brain,
                                                             const DataFileTypeEnum::Enum dataFileType)
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(brain,
                                                                         StructureEnum::ALL,
                                                                         FILE_MODE_DATA_FILE_TYPE_ENUM);
    model->m_dataFileTypes.push_back(dataFileType);
    
    return model;
}

/**
 * Create a new instance of a Caret Data File Selection Model that
 * selects files of the given Data File Types from the given Brain.
 *
 * @param brain
 *    Brain from which files are obtained.
 * @param dataFileTypes
 *    Types of the data file.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceForCaretDataFileTypes(Brain* brain,
                                                             const std::vector<DataFileTypeEnum::Enum>& dataFileTypes)
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(brain,
                                                                         StructureEnum::ALL,
                                                                         FILE_MODE_DATA_FILE_TYPE_ENUM);
    model->m_dataFileTypes.insert(model->m_dataFileTypes.end(),
                                  dataFileTypes.begin(),
                                  dataFileTypes.end());
    
    return model;
}

/**
 * Create a new instance of a Caret Data File Selection Model that
 * selects files of the given Data File Types for the given structure
 * from the given Brain.
 *
 * @param brain
 *    Brain from which files are obtained.
 * @param structure
 *    Structure for files.   Files with the identical structure are used
 *    as well as those files with structure 'ALL'.
 * @param dataFileTypes
 *    Types of the data file.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceForCaretDataFileTypesInStructure(Brain* brain,
                                                                                const StructureEnum::Enum structure,
                                                                                const std::vector<DataFileTypeEnum::Enum>& dataFileTypes)
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(brain,
                                                                         structure,
                                                                         FILE_MODE_DATA_FILE_TYPE_ENUM);
    model->m_dataFileTypes.insert(model->m_dataFileTypes.end(),
                                  dataFileTypes.begin(),
                                  dataFileTypes.end());
    
    return model;
    
}


/**
 * Create a new instance of a Caret Data File Selection Model that
 * selects files that implement the chartable matrix parcel interface.
 *
 * @param brain
 *    Brain from which files are obtained.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceForChartableMatrixParcelInterface(Brain* brain)
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(brain,
                                                                         StructureEnum::ALL,
                                                                         FILE_MODE_CHARTABLE_MATRIX_PARCEL_INTERFACE);
    
    return model;
}

/**
 * Create a new instance of a Caret Data File Selection Model that
 * selectes multi-structure border files.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceForMultiStructureBorderFiles(Brain* brain)
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(brain,
                                                                         StructureEnum::ALL,
                                                                         FILE_MODE_MULTI_STRUCTURE_BORDER_FILES);
    
    return model;
}



/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
CaretDataFileSelectionModel::CaretDataFileSelectionModel(const CaretDataFileSelectionModel& obj)
: CaretObject(obj),
SceneableInterface(),
m_fileMode(obj.m_fileMode),
m_brain(obj.m_brain)
{
    this->copyHelperCaretDataFileSelectionModel(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
CaretDataFileSelectionModel&
CaretDataFileSelectionModel::operator=(const CaretDataFileSelectionModel& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperCaretDataFileSelectionModel(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
CaretDataFileSelectionModel::copyHelperCaretDataFileSelectionModel(const CaretDataFileSelectionModel& obj)
{
    m_brain         = obj.m_brain;
    m_structure     = obj.m_structure;
    m_dataFileTypes = obj.m_dataFileTypes;
    m_selectedFile  = obj.m_selectedFile;
    m_overrideOfAvailableFilesValid = obj.m_overrideOfAvailableFilesValid;
    m_overrideOfAvailableFiles      = obj.m_overrideOfAvailableFiles;
}

/**
 * @return The selected file or NULL if no file is available.
 */
CaretDataFile*
CaretDataFileSelectionModel::getSelectedFile()
{
    updateSelection();
    return m_selectedFile;
}

/**
 * @return The selected file or NULL if no file is available.
 */
const CaretDataFile*
CaretDataFileSelectionModel::getSelectedFile() const
{
    updateSelection();
    return m_selectedFile;
}

/**
 * Set the selected file or NULL if no file is available.
 *
 * @param selectedFile
 *     Set the selected file.
 */
void
CaretDataFileSelectionModel::setSelectedFile(CaretDataFile* selectedFile)
{
    m_selectedFile = selectedFile;
}

/**
 * @return Files available for selection.
 */
std::vector<CaretDataFile*>
CaretDataFileSelectionModel::getAvailableFiles() const
{
    if (m_overrideOfAvailableFilesValid) {
        return m_overrideOfAvailableFiles;
    }
    
    std::vector<CaretDataFile*> caretDataFiles;
    
    switch (m_fileMode) {
        case FILE_MODE_DATA_FILE_TYPE_ENUM:
        {
            m_brain->getAllDataFilesWithDataFileTypes(m_dataFileTypes,
                                                      caretDataFiles);
        }
            break;
        case FILE_MODE_CHARTABLE_MATRIX_PARCEL_INTERFACE:
        {
            std::vector<ChartableMatrixInterface*> chartFiles;
            m_brain->getAllChartableMatrixDataFiles(chartFiles);
            
            for (std::vector<ChartableMatrixInterface*>::iterator iter = chartFiles.begin();
                 iter != chartFiles.end();
                 iter++) {
                ChartableMatrixInterface* chartFile = *iter;
                ChartableMatrixParcelInterface* chartParcelFile = dynamic_cast<ChartableMatrixParcelInterface*>(chartFile);
                /*
                 * Want files that ARE parcel chartable
                 */
                if (chartParcelFile != NULL) {
                    CaretMappableDataFile* mapFile = chartParcelFile->getMatrixChartCaretMappableDataFile();
                    caretDataFiles.push_back(mapFile);
                }
            }
        }
            break;
        case FILE_MODE_MULTI_STRUCTURE_BORDER_FILES:
        {
            const int numBorderFiles = m_brain->getNumberOfBorderFiles();
            for (int32_t i = 0; i < numBorderFiles; i++) {
                BorderFile* borderFile = m_brain->getBorderFile(i);
                if ( ! borderFile->isSingleStructure()) {
                    caretDataFiles.push_back(borderFile);
                }
            }
        }
            break;
    }

    if (m_structure == StructureEnum::ALL) {
        return caretDataFiles;
    }
    
    
    std::vector<CaretDataFile*> caretDataFilesOut;
    
    for (std::vector<CaretDataFile*>::iterator iter = caretDataFiles.begin();
         iter != caretDataFiles.end();
         iter++) {
        CaretDataFile* cdf = *iter;
        const StructureEnum::Enum fileStructure = cdf->getStructure();
        if ((fileStructure == StructureEnum::ALL)
            || (fileStructure == StructureEnum::INVALID)) {
            caretDataFilesOut.push_back(cdf);
        }
        else if (fileStructure == m_structure) {
            caretDataFilesOut.push_back(cdf);
        }
    }
    
    return caretDataFilesOut;
}

/**
 * Override the available files with the given files.  Once this method
 * is called, these will be the available files until this method is called
 * again.
 *
 * @param availableFiles
 *    Files that will be used in this model.
 */
void
CaretDataFileSelectionModel::overrideAvailableDataFiles(std::vector<CaretDataFile*>& availableFiles)
{
    m_overrideOfAvailableFiles = availableFiles;
    m_overrideOfAvailableFilesValid = true;
}


/**
 * @return Structure used in this model.
 */
StructureEnum::Enum
CaretDataFileSelectionModel::getStructure() const
{
    return m_structure;
}

/**
 * Set the structure used for this model.
 *
 * @param structure
 *     New structure for this model.
 */
void
CaretDataFileSelectionModel::setStructure(const StructureEnum::Enum structure)
{
    m_structure = structure;
    updateSelection();
}


/**
 * Update the selected file.
 */
void
CaretDataFileSelectionModel::updateSelection() const
{
    std::vector<CaretDataFile*> caretDataFiles = getAvailableFiles();
    
    if (caretDataFiles.empty()) {
        m_selectedFile = NULL;
        return;
    }
    
    if (m_selectedFile != NULL) {
        if (std::find(caretDataFiles.begin(),
                      caretDataFiles.end(),
                      m_selectedFile) == caretDataFiles.end()) {
            m_selectedFile = NULL;
        }
    }
    
    if (m_selectedFile == NULL) {
        if (! caretDataFiles.empty()) {
            m_selectedFile = caretDataFiles[0];
        }
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CaretDataFileSelectionModel::toString() const
{
    return "CaretDataFileSelectionModel";
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
CaretDataFileSelectionModel::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "CaretDataFileSelectionModel",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    CaretDataFile* caretDataFile = getSelectedFile();
    if (caretDataFile != NULL) {
        sceneClass->addPathName("selectedFileNameWithPath",
                                caretDataFile->getFileName());
        sceneClass->addString("selectedFileNameNoPath",
                              caretDataFile->getFileNameNoPath());
    }
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
CaretDataFileSelectionModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    setSelectedFile(NULL);
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    bool foundFileFlag = false;
    
    const AString selectedFileNameWithPath = sceneClass->getPathNameValue("selectedFileNameWithPath");
    if ( ! selectedFileNameWithPath.isEmpty()) {
        std::vector<CaretDataFile*> caretDataFiles = getAvailableFiles();
        
        for (std::vector<CaretDataFile*>::iterator iter = caretDataFiles.begin();
             iter != caretDataFiles.end();
             iter++) {
            CaretDataFile* cdf = *iter;
            if (cdf->getFileName() == selectedFileNameWithPath) {
                setSelectedFile(cdf);
                foundFileFlag = true;
                break;
            }
        }
    }
    
    const AString selectedFileNameNoPath = sceneClass->getStringValue("selectedFileNameNoPath",
                                                                      "");
    if ( ! foundFileFlag) {
        if ( ! selectedFileNameNoPath.isEmpty()) {
            std::vector<CaretDataFile*> caretDataFiles = getAvailableFiles();
            
            for (std::vector<CaretDataFile*>::iterator iter = caretDataFiles.begin();
                 iter != caretDataFiles.end();
                 iter++) {
                CaretDataFile* cdf = *iter;
                if (cdf->getFileNameNoPath() == selectedFileNameNoPath) {
                    setSelectedFile(cdf);
                    break;
                }
            }
        }
    }
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

