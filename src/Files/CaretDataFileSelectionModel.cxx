
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

#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretMappableDataFile.h"
#include "ChartableMatrixParcelInterface.h"
#include "EventCaretDataFilesGet.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::CaretDataFileSelectionModel 
 * \brief Selection model for a CaretDataFile.
 * \ingroup Files
 *
 * Maintains selection of a type of CaretDataFile.  Used in the GUI
 * by CaretDataFileSelectionComboBox.
 */

/**
 * Constructor.  This constructor is private and one of the static
 * factory methods should be used to create new instances of 
 * this class.
 *
 * @param mappableDataFile
 *    Mappable data file for matching brainordinates.
 * @param structure
 *    Structure for the files.
 * @param fileMode
 *    File mode that indicates how files are chosen.
 */
CaretDataFileSelectionModel::CaretDataFileSelectionModel(const CaretMappableDataFile* mappableDataFile,
                                                         const StructureEnum::Enum structure,
                                                         const FileMode fileMode)
: CaretObject(),
m_mappableDataFile(mappableDataFile),
m_fileMode(fileMode),
m_structure(structure)
{
    switch (m_fileMode) {
        case FILE_MODE_DATA_FILE_TYPE_ENUM:
            break;
        case FILE_MODE_CHARTABLE_MATRIX_PARCEL_INTERFACE:
            break;
        case FILE_MODE_MAPS_TO_SAME_BRAINORDINATES:
            CaretAssert(m_mappableDataFile);
            break;
        case FILE_MODE_MAPS_TO_SAME_BRAINORDINATES_EXCLUDE_SELF:
            CaretAssert(m_mappableDataFile);
            break;
        case FILE_MODE_MULTI_STRUCTURE_BORDER_FILES:
            break;
    }
    
    m_overrideOfAvailableFilesValid = false;
    
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
 * matches a mappable data file's brainordinate mapping.
 *
 * @param dataFileType
 *    Type of the data file.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceMapsToSameBrainordinates(const CaretMappableDataFile* mappableDataFile)
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(mappableDataFile,
                                                                         StructureEnum::ALL,
                                                                         FILE_MODE_MAPS_TO_SAME_BRAINORDINATES);
    return model;
}

/**
 * Create a new instance of a Caret Data File Selection Model that
 * matches a mappable data file's brainordinate mapping.
 *
 * @param dataFileType
 *    Type of the data file.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceMapsToSameBrainordinatesExcludeSelf(const CaretMappableDataFile* mappableDataFile)
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(mappableDataFile,
                                                                         StructureEnum::ALL,
                                                                         FILE_MODE_MAPS_TO_SAME_BRAINORDINATES_EXCLUDE_SELF);
    return model;
}


/**
 * Create a new instance of a Caret Data File Selection Model that
 * selects files of the given Data File Type.
 *
 * @param dataFileType
 *    Type of the data file.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceForCaretDataFileType(const DataFileTypeEnum::Enum dataFileType)
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(NULL,
                                                                         StructureEnum::ALL,
                                                                         FILE_MODE_DATA_FILE_TYPE_ENUM);
    model->m_dataFileTypes.push_back(dataFileType);
    
    return model;
}

/**
 * Create a new instance of a Caret Data File Selection Model that
 * selects files of the given Data File Types.
 *
 * @param dataFileTypes
 *    Types of the data file.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceForCaretDataFileTypes(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes)
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(NULL,
                                                                         StructureEnum::ALL,
                                                                         FILE_MODE_DATA_FILE_TYPE_ENUM);
    model->m_dataFileTypes.insert(model->m_dataFileTypes.end(),
                                  dataFileTypes.begin(),
                                  dataFileTypes.end());
    
    return model;
}

/**
 * Create a new instance of a Caret Data File Selection Model that
 * selects files of the given Data File Types for the given structure.
 *
 * @param structure
 *    Structure for files.   Files with the identical structure are used
 *    as well as those files with structure 'ALL'.
 * @param dataFileTypes
 *    Types of the data file.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceForCaretDataFileTypes(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes,
                                                              const std::vector<SubvolumeAttributes::VolumeType>& volumeTypes)
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(NULL,
                                                                         StructureEnum::ALL,
                                                                         FILE_MODE_DATA_FILE_TYPE_ENUM);
    model->m_dataFileTypes.insert(model->m_dataFileTypes.end(),
                                  dataFileTypes.begin(),
                                  dataFileTypes.end());
    model->m_volumeTypes.insert(model->m_volumeTypes.end(),
                                volumeTypes.begin(),
                                volumeTypes.end());
    
    return model;
}


/**
 * Create a new instance of a Caret Data File Selection Model that
 * selects files of the given Data File Types for the given structure.
 *
 * @param structure
 *    Structure for files.   Files with the identical structure are used
 *    as well as those files with structure 'ALL'.
 * @param dataFileTypes
 *    Types of the data file.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceForCaretDataFileTypesInStructure(const StructureEnum::Enum structure,
                                                                         const std::vector<DataFileTypeEnum::Enum>& dataFileTypes)
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(NULL,
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
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceForChartableMatrixParcelInterface()
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(NULL,
                                                                         StructureEnum::ALL,
                                                                         FILE_MODE_CHARTABLE_MATRIX_PARCEL_INTERFACE);
    
    return model;
}

/**
 * Create a new instance of a Caret Data File Selection Model that
 * selectes multi-structure border files.
 */
CaretDataFileSelectionModel*
CaretDataFileSelectionModel::newInstanceForMultiStructureBorderFiles()
{
    CaretDataFileSelectionModel* model = new CaretDataFileSelectionModel(NULL,
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
m_mappableDataFile(obj.m_mappableDataFile),
m_fileMode(obj.m_fileMode)
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
        CaretAssert(m_fileMode == obj.m_fileMode);
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
    m_structure     = obj.m_structure;
    m_dataFileTypes = obj.m_dataFileTypes;
    m_volumeTypes   = obj.m_volumeTypes;
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
 * Set the selected file best matching filename or first file if no match.
 * The names of the files (without paths) must match
 *
 * @param filename
 *     name of file
 */
void
CaretDataFileSelectionModel::setSelectedFileByFilename(const AString& filename)
{
    std::vector<CaretDataFile*> allFiles(getAvailableFiles());
    CaretDataFile* longestNameMatchFile = NULL;
    int32_t longestNameMatchLength = -1;
        
    for (CaretDataFile* cdf : allFiles) {
        if (cdf->getFileName() == filename) {
            longestNameMatchFile = cdf;
            break;
        }
        
        if (filename.endsWith(cdf->getFileNameNoPath())) {
            const int32_t matchLen(filename.countMatchingCharactersFromEnd(cdf->getFileName()));
            if (matchLen > longestNameMatchLength) {
                longestNameMatchFile = cdf;
                longestNameMatchLength = matchLen;
            }
        }
    }
    
    if (longestNameMatchFile != NULL) {
        setSelectedFile(longestNameMatchFile);
    }
    else if ( ! allFiles.empty()) {
        CaretAssertVectorIndex(allFiles, 0);
        setSelectedFile(allFiles[0]);
    }
    else {
        setSelectedFile(NULL);
    }
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
            std::vector<CaretDataFile*> unfilteredFiles = EventCaretDataFilesGet::getCaretDataFilesForTypes(m_dataFileTypes);
            if (m_volumeTypes.empty()) {
                caretDataFiles = unfilteredFiles;
            }
            else {
                /*
                 * Filter volume volume files based upon volume type
                 */
                for (auto& cdf :unfilteredFiles) {
                    if (cdf->getDataFileType() == DataFileTypeEnum::VOLUME) {
                        VolumeFile* vf(dynamic_cast<VolumeFile*>(cdf));
                        CaretAssert(vf);
                        if (std::find(m_volumeTypes.begin(),
                                      m_volumeTypes.end(),
                                      vf->getType()) != m_volumeTypes.end()) {
                            caretDataFiles.push_back(cdf);
                        }
                    }
                    else {
                        caretDataFiles.push_back(cdf);
                    }
                }
            }
        }
            break;
        case FILE_MODE_CHARTABLE_MATRIX_PARCEL_INTERFACE:
        {
            std::vector<CaretDataFile*> allFiles = EventCaretDataFilesGet::getAllCaretDataFiles();
            for (auto dataFile : allFiles) {
                ChartableMatrixParcelInterface* chartParcelFile = dynamic_cast<ChartableMatrixParcelInterface*>(dataFile);
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
        case FILE_MODE_MAPS_TO_SAME_BRAINORDINATES:
        case FILE_MODE_MAPS_TO_SAME_BRAINORDINATES_EXCLUDE_SELF:
        {
            const bool excludeSelfFlag(m_fileMode == FILE_MODE_MAPS_TO_SAME_BRAINORDINATES_EXCLUDE_SELF);
            CaretAssert(m_mappableDataFile);
            EventCaretMappableDataFilesGet mapFilesGetEvent;
            EventManager::get()->sendEvent(mapFilesGetEvent.getPointer());
            std::vector<CaretMappableDataFile*> mapFiles;
            mapFilesGetEvent.getAllFiles(mapFiles);
            
            /*
             * Allow the same file to be included since this is used
             * by thresholding and may want to threshold with a map
             * in the same file.
             */
            for (auto mf : mapFiles) {
                bool doFlag(true);
                if (mf == m_mappableDataFile) {
                    if (excludeSelfFlag) {
                        doFlag = false;
                    }
                }
                if (doFlag) {
                    switch (m_mappableDataFile->getBrainordinateMappingMatch(mf)) {
                        case CaretMappableDataFile::BrainordinateMappingMatch::EQUAL:
                            caretDataFiles.push_back(mf);
                            break;
                        case CaretMappableDataFile::BrainordinateMappingMatch::NO:
                            break;
                        case CaretMappableDataFile::BrainordinateMappingMatch::SUBSET:
                            caretDataFiles.push_back(mf);
                            break;
                    }
                }
            }
        }
            break;
        case FILE_MODE_MULTI_STRUCTURE_BORDER_FILES:
        {
            std::vector<CaretDataFile*> borderFiles = EventCaretDataFilesGet::getCaretDataFilesForType(DataFileTypeEnum::BORDER);
            for (auto file : borderFiles) {
                if ( ! file->isSingleStructure()) {
                    caretDataFiles.push_back(file);
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

