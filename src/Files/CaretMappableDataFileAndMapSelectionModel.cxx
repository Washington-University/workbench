
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

#define __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTION_MODEL_DECLARE__
#include "CaretMappableDataFileAndMapSelectionModel.h"
#undef __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTION_MODEL_DECLARE__

#include "CaretAssert.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CaretMappableDataFileAndMapSelectionModel 
 * \brief Model for selection of a CaretMappableDataFile and map index.
 * \ingroup Files
 */

/**
 * Constructor for files that map to the same brainordinates as the given file.
 *
 * @param caretMappableDataFile
 *    Mappable data file.
 * @param excludeSelfFlag
 *    If true exclude self from file selection
 */
CaretMappableDataFileAndMapSelectionModel::CaretMappableDataFileAndMapSelectionModel(const CaretMappableDataFile* caretMappableDataFile,
                                                                                     const bool excludeSelfFlag)
: CaretObject(),
m_mode(excludeSelfFlag
       ? Mode::MAP_TO_SAME_BRAINORDINATES_EXCLUDE_SELF
       : Mode::MAP_TO_SAME_BRAINORDINATES),
m_mappableDataFile(caretMappableDataFile)
{
    std::vector<DataFileTypeEnum::Enum> dataFileTypesVector;
    std::vector<SubvolumeAttributes::VolumeType> volumeTypes;
    performConstruction(dataFileTypesVector,
                        volumeTypes);
}


/**
 * Constructor for instance that will contain files of the given data type.
 *
 * @param dataFileType
 *    Type of data files available for selection.
 */
CaretMappableDataFileAndMapSelectionModel::CaretMappableDataFileAndMapSelectionModel(const DataFileTypeEnum::Enum dataFileType)
: CaretObject(),
m_mode(Mode::MATCH_DATA_FILE_TYPES),
m_mappableDataFile(NULL)
{
    std::vector<DataFileTypeEnum::Enum> dataFileTypesVector;
    dataFileTypesVector.push_back(dataFileType);
    std::vector<SubvolumeAttributes::VolumeType> volumeTypes;
    performConstruction(dataFileTypesVector,
                        volumeTypes);
}

/**
 * Constructor for instance that will contain files of the given data types.
 *
 * @param dataFileTypes
 *    Types of data files available for selection.
 */
CaretMappableDataFileAndMapSelectionModel::CaretMappableDataFileAndMapSelectionModel(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes)
: CaretObject(),
m_mode(Mode::MATCH_DATA_FILE_TYPES),
m_mappableDataFile(NULL)
{
    std::vector<SubvolumeAttributes::VolumeType> volumeTypes;
    performConstruction(dataFileTypes,
                        volumeTypes);
}

/**
 * Constructor for instance that will contain files of the given data types.
 * Volume are limited to the given types
 *
 * @param dataFileTypes
 *    Types of data files available for selection.
 * @param volumeTypes
 *    If not empty, volumes must be one of these types
 */
CaretMappableDataFileAndMapSelectionModel::CaretMappableDataFileAndMapSelectionModel(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes,
                                                                                     const std::vector<SubvolumeAttributes::VolumeType>& volumeTypes)
: CaretObject(),
m_mode(Mode::MATCH_DATA_FILE_TYPES),
m_mappableDataFile(NULL)
{
    performConstruction(dataFileTypes,
                        volumeTypes);
}

/**
 * Destructor.
 */
CaretMappableDataFileAndMapSelectionModel::~CaretMappableDataFileAndMapSelectionModel()
{
    delete m_caretDataFileSelectionModel;
    delete m_sceneAssistant;
}

/**
 * Validate that any data file types are mappable data files
 * and remove any non-mappable data file types.
 */
void
CaretMappableDataFileAndMapSelectionModel::validateDataFileTypes()
{
    std::vector<DataFileTypeEnum::Enum> mappableDataFileTypes;
    
    for (const auto dataFileType : m_dataFileTypes) {
        bool isMappableFile = false;
        switch (dataFileType) {
            case DataFileTypeEnum::ANNOTATION:
                CaretAssert(0);
                break;
            case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                CaretAssert(0);
                break;
            case DataFileTypeEnum::BORDER:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_MAPS:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::CZI_IMAGE_FILE:
                break;
            case DataFileTypeEnum::FOCI:
                break;
            case DataFileTypeEnum::HISTOLOGY_SLICES:
                break;
            case DataFileTypeEnum::IMAGE:
                break;
            case DataFileTypeEnum::LABEL:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::METRIC:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::METRIC_DYNAMIC:
                isMappableFile = true;;
                break;
            case DataFileTypeEnum::OME_ZARR_IMAGE_FILE:
                break;
            case DataFileTypeEnum::PALETTE:
                break;
            case DataFileTypeEnum::RGBA:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::SAMPLES:
                break;
            case DataFileTypeEnum::SCENE:
                break;
            case DataFileTypeEnum::SPECIFICATION:
                break;
            case DataFileTypeEnum::SURFACE:
                break;
            case DataFileTypeEnum::UNKNOWN:
                break;
            case DataFileTypeEnum::VOLUME:
                isMappableFile = true;
                break;
            case DataFileTypeEnum::VOLUME_DYNAMIC:
                isMappableFile = true;
                break;
        }
        
        CaretAssert(isMappableFile);
        if (isMappableFile) {
            mappableDataFileTypes.push_back(dataFileType);
        }
        else {
            CaretLogSevere(DataFileTypeEnum::toGuiName(dataFileType)
                           + " is not a valid mappable data file.");
        }
    }
    
    if (m_dataFileTypes.size() != mappableDataFileTypes.size()) {
        /*
         * Update without non-mappable data file types.
         */
        m_dataFileTypes = mappableDataFileTypes;
    }
}

/**
 * Finish construction for an instance of this class.
 *
 * @param dataFileTypes
 *    Types of data files available for selection.
 * @param volumeTypes
 *    If not empty, volumes must be one of these types
 */
void
CaretMappableDataFileAndMapSelectionModel::performConstruction(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes,
                                                               const std::vector<SubvolumeAttributes::VolumeType>& volumeTypes)
{
    m_dataFileTypes = dataFileTypes;
    m_volumeTypes   = volumeTypes;
    
    switch (m_mode) {
        case Mode::MAP_TO_SAME_BRAINORDINATES:
            CaretAssert(m_mappableDataFile);
            m_caretDataFileSelectionModel = CaretDataFileSelectionModel::newInstanceMapsToSameBrainordinates(m_mappableDataFile);
            break;
        case Mode::MAP_TO_SAME_BRAINORDINATES_EXCLUDE_SELF:
            CaretAssert(m_mappableDataFile);
            m_caretDataFileSelectionModel = CaretDataFileSelectionModel::newInstanceMapsToSameBrainordinatesExcludeSelf(m_mappableDataFile);
            break;
        case Mode::MATCH_DATA_FILE_TYPES:
            validateDataFileTypes();
            m_caretDataFileSelectionModel = CaretDataFileSelectionModel::newInstanceForCaretDataFileTypes(dataFileTypes,
                                                                                                          volumeTypes);
            break;
    }
    
    m_selectedMapIndex = -1;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_caretDataFileSelectionModel",
                          "CaretDataFileSelectionModel",
                          m_caretDataFileSelectionModel);
    m_sceneAssistant->add("m_selectedMapIndex",
                          &m_selectedMapIndex);
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
CaretMappableDataFileAndMapSelectionModel::CaretMappableDataFileAndMapSelectionModel(const CaretMappableDataFileAndMapSelectionModel& obj)
: CaretObject(obj),
SceneableInterface(obj),
m_mode(obj.m_mode),
m_mappableDataFile(obj.m_mappableDataFile)
{
    this->copyHelperCaretMappableDataFileAndMapSelectionModel(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
CaretMappableDataFileAndMapSelectionModel&
CaretMappableDataFileAndMapSelectionModel::operator=(const CaretMappableDataFileAndMapSelectionModel& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperCaretMappableDataFileAndMapSelectionModel(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
CaretMappableDataFileAndMapSelectionModel::copyHelperCaretMappableDataFileAndMapSelectionModel(const CaretMappableDataFileAndMapSelectionModel& obj)
{
    *m_caretDataFileSelectionModel = *obj.m_caretDataFileSelectionModel;
    m_selectedMapIndex = obj.m_selectedMapIndex;
}

/**
 * @return The data file selection model.
 */
CaretDataFileSelectionModel*
CaretMappableDataFileAndMapSelectionModel::getCaretDataFileSelectionModel()
{
    return m_caretDataFileSelectionModel;
}


/**
 * @return the selected file.
 */
CaretMappableDataFile*
CaretMappableDataFileAndMapSelectionModel::getSelectedFile()
{
    return dynamic_cast<CaretMappableDataFile*>(m_caretDataFileSelectionModel->getSelectedFile());
}

/**
 * @return the selected file.
 */
const CaretMappableDataFile*
CaretMappableDataFileAndMapSelectionModel::getSelectedFile() const
{
    return dynamic_cast<CaretMappableDataFile*>(m_caretDataFileSelectionModel->getSelectedFile());
}

/**
 * @return the selected map index (-1 if no map available).
 */
int32_t
CaretMappableDataFileAndMapSelectionModel::getSelectedMapIndex() const
{
    const CaretMappableDataFile* cmdf = getSelectedFile();
    if (cmdf != NULL) {
        const int32_t numMaps = cmdf->getNumberOfMaps();
        if (numMaps > 0) {
            if (m_selectedMapIndex < 0) {
                m_selectedMapIndex = 0;
            }
            else if (m_selectedMapIndex >= numMaps) {
                m_selectedMapIndex = numMaps - 1;
            }
        }
        else {
            m_selectedMapIndex = -1;
        }
    }
    else {
        m_selectedMapIndex = -1;
    }
    
    return m_selectedMapIndex;
}

/**
 * @return the files in this model.
 */
std::vector<CaretMappableDataFile*>
CaretMappableDataFileAndMapSelectionModel::getAvailableFiles() const
{
    switch (m_mode) {
        case Mode::MAP_TO_SAME_BRAINORDINATES:
            break;
        case Mode::MAP_TO_SAME_BRAINORDINATES_EXCLUDE_SELF:
            break;
        case Mode::MATCH_DATA_FILE_TYPES:
            break;
    }
    
    std::vector<CaretDataFile*> caretDataFiles = m_caretDataFileSelectionModel->getAvailableFiles();
    std::vector<CaretMappableDataFile*> mappableFiles;
    for (std::vector<CaretDataFile*>::iterator iter = caretDataFiles.begin();
         iter != caretDataFiles.end();
         iter++) {
        CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(*iter);
        if (cmdf != NULL) {
            mappableFiles.push_back(cmdf);
        }
    }
    return mappableFiles;
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
CaretMappableDataFileAndMapSelectionModel::overrideAvailableDataFiles(std::vector<CaretMappableDataFile*>& availableFiles)
{
    std::vector<CaretDataFile*> files(availableFiles.begin(),
                                      availableFiles.end());
    
    m_caretDataFileSelectionModel->overrideAvailableDataFiles(files);
}


/**
 * Set the selected file
 *
 * @param selectedFile
 *    New selected file.
 */
void
CaretMappableDataFileAndMapSelectionModel::setSelectedFile(CaretMappableDataFile* selectedFile)
{
    if (selectedFile != NULL) {
        switch (m_mode) {
            case Mode::MAP_TO_SAME_BRAINORDINATES:
                break;
            case Mode::MAP_TO_SAME_BRAINORDINATES_EXCLUDE_SELF:
                break;
            case Mode::MATCH_DATA_FILE_TYPES:
            {
                const DataFileTypeEnum::Enum fileType = selectedFile->getDataFileType();
                if (std::find(m_dataFileTypes.begin(),
                              m_dataFileTypes.end(),
                              fileType) == m_dataFileTypes.end()) {
                    AString validFileTypeNames;
                    for (std::vector<DataFileTypeEnum::Enum>::const_iterator typeIter = m_dataFileTypes.begin();
                         typeIter != m_dataFileTypes.end();
                         typeIter++) {
                        const DataFileTypeEnum::Enum dataFileType = *typeIter;
                        validFileTypeNames.append(DataFileTypeEnum::toName(dataFileType) + " ");
                    }
                    
                    
                    const AString msg("Attempting to set file that is of type "
                                      + DataFileTypeEnum::toGuiName(fileType)
                                      + " but model is for type(s) "
                                      + validFileTypeNames);
                    CaretAssertMessage(0, msg);
                    CaretLogSevere(msg);
                    return;
                }
            }
                break;
        }
    }
    
    m_caretDataFileSelectionModel->setSelectedFile(selectedFile);
}

/**
 * Set the selected file by name
 *
 * @param filename
 *    Name of file.
 */
void
CaretMappableDataFileAndMapSelectionModel::setSelectedFileName(const AString& filename)
{
    std::vector<CaretDataFile*> files(m_caretDataFileSelectionModel->getAvailableFiles());
    for (CaretDataFile* cdf : files) {
        CaretAssert(cdf);
        if (filename == cdf->getFileName()) {
            m_caretDataFileSelectionModel->setSelectedFile(cdf);
            return;
        }
    }
    for (CaretDataFile* cdf : files) {
        CaretAssert(cdf);
        if (cdf->getFileName().endsWith(filename)) {
            m_caretDataFileSelectionModel->setSelectedFile(cdf);
            return;
        }
    }
}


/**
 * Set the selected map index
 *
 * @param mapIndex
 *    New selected map index.
 */
void
CaretMappableDataFileAndMapSelectionModel::setSelectedMapIndex(const int32_t mapIndex)
{
    const CaretMappableDataFile* cdf(getSelectedFile());
    if (cdf != NULL) {
        if ((mapIndex >= 0)
            && (mapIndex < cdf->getNumberOfMaps())) {
            m_selectedMapIndex = mapIndex;
        }
    }
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
CaretMappableDataFileAndMapSelectionModel::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "CaretMappableDataFileAndMapSelectionModel",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
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
CaretMappableDataFileAndMapSelectionModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

