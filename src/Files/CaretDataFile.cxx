
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

#define __CARET_DATA_FILE_DECLARE__
#include "CaretDataFile.h"
#undef __CARET_DATA_FILE_DECLARE__

#include "CaretMappableDataFile.h"
#include "DataFileContentInformation.h"
#include "FileIdentificationAttributes.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::CaretDataFile 
 * \brief A data file with abstract methods for caret data
 *
 * This class is essentially an interface that defines methods
 * that are supported by most Caret Data Files.
 */
/**
 * Constructor.
 */
CaretDataFile::CaretDataFile(const DataFileTypeEnum::Enum dataFileType)
: DataFile(),
SceneableInterface()
{
    m_dataFileType = dataFileType;
    
    /*
     * As of 10jun2022 any instances of CaretMappableDataFile
     * and MediaFile support identification attributes
     * (files that are in layers)
     */
    bool supportsIdentificationAttributesFlag(false);
    switch (dataFileType) {
        case DataFileTypeEnum::ANNOTATION:
            break;
        case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
            break;
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::CZI_IMAGE_FILE:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::FOCI:
            break;
        case DataFileTypeEnum::HISTOLOGY_SLICES:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::IMAGE:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::LABEL:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::METRIC:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::METRIC_DYNAMIC:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::OME_ZARR_IMAGE_FILE:
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
            supportsIdentificationAttributesFlag = true;
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
            supportsIdentificationAttributesFlag = true;
            break;
        case DataFileTypeEnum::VOLUME_DYNAMIC:
            supportsIdentificationAttributesFlag = true;
            break;
    }

    m_fileIdentificationAttributes.reset(new FileIdentificationAttributes(supportsIdentificationAttributesFlag));

    AString name = ("untitled_"
                    + AString::number(s_defaultFileNameCounter)
                    + "."
                    + DataFileTypeEnum::toFileExtension(m_dataFileType));
    s_defaultFileNameCounter++;
    
    setFileNameProtected(name);
}

/**
 * Destructor.
 */
CaretDataFile::~CaretDataFile()
{
    
}

/**
 * @return The type of this data file.
 */
DataFileTypeEnum::Enum 
CaretDataFile::getDataFileType() const
{
    return m_dataFileType; 
}

/**
 * @return True if this file supports file metadata, else false.
 * Subclasses should override this method if the subclass
 * DOES NOT support file metadata.
 */
bool
CaretDataFile::supportsFileMetaData() const
{
    return true;
}


/**
 * Override the default data type for the file.
 * Use this with extreme caution as using a type invalid
 * with the file may cause disaster.
 * 
 * @param dataFileType
 *    New value for file's data type.
 */
void 
CaretDataFile::setDataFileType(const DataFileTypeEnum::Enum dataFileType)
{
    m_dataFileType = dataFileType;
}

/**
 * Copy constructor.
 * @param cdf
 *    Instance that is copied to this.
 */
CaretDataFile::CaretDataFile(const CaretDataFile& cdf)
: DataFile(cdf),
SceneableInterface(cdf)
{
    copyDataCaretDataFile(cdf);
}

/**
 * Assignment operator.
 * @param cdf
 *    Instance that is assigned to this.
 */
CaretDataFile& 
CaretDataFile::operator=(const CaretDataFile& cdf)
{
    if (this != &cdf) {
        DataFile::operator=(cdf);
        copyDataCaretDataFile(cdf);
    }
    return *this;
}

/**
 * Assists with copying instances of this class.
 * @param cdf
 *    Instance that is copied to this.
 */
void 
CaretDataFile::copyDataCaretDataFile(const CaretDataFile& cdf)
{
    m_dataFileType = cdf.m_dataFileType;
}

/**
 * @return Is this file mapped to a valid single structure ?
 */
bool
CaretDataFile::isSingleStructure() const
{
    return StructureEnum::isSingleStructure(getStructure());
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
CaretDataFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    DataFile::addToDataFileContentInformation(dataFileInformation);
    
    const DataFileTypeEnum::Enum dataFileType = getDataFileType();
    bool validExtensionFlag = false;
    const DataFileTypeEnum::Enum extensionFileType = DataFileTypeEnum::fromFileExtension(getFileName(),
                                                                                         &validExtensionFlag);
    
    if (dataFileType != extensionFileType) {
        const AString msg("Incorrect filename extension.  It should be \""
                          + DataFileTypeEnum::toFileExtension(dataFileType)
                          + "\"");
        dataFileInformation.addNameAndValue("WARNING EXTENSION",
                                            msg);
    }
    
    dataFileInformation.addNameAndValue("Type",
                                        DataFileTypeEnum::toGuiName(m_dataFileType));
    
    std::vector<StructureEnum::Enum> allStructures;
    StructureEnum::getAllEnums(allStructures);
    
    CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(this);
    if (cmdf != NULL) {
        AString structureNames;
        
        for (std::vector<StructureEnum::Enum>::iterator iter = allStructures.begin();
             iter != allStructures.end();
             iter++) {
            if (cmdf->isMappableToSurfaceStructure(*iter)) {
                structureNames.append(StructureEnum::toGuiName(*iter) + " ");
            }
        }
        dataFileInformation.addNameAndValue("Structure",
                                           structureNames);
    }
    else {
        dataFileInformation.addNameAndValue("Structure",
                                            StructureEnum::toGuiName(getStructure()));
    }
    
    const GiftiMetaData* fileMetaData = getFileMetaData();
    const auto namesAndValues(fileMetaData->getAsMap());
    if ( ! namesAndValues.empty()) {
        const AString dashes("-----------");
        dataFileInformation.addNameAndValue("Metadata Start", dashes);
        for (const auto& nv : namesAndValues) {
            dataFileInformation.addNameAndValue(nv.first, nv.second);
        }
        dataFileInformation.addNameAndValue("Metadata End", dashes);
    }
}

/**
 * Set the username and password for reading files, typically from
 * a database or website.
 *
 * @param username
 *     Account's username.
 * @param password
 *     Account's password.
 */
void
CaretDataFile::setFileReadingUsernameAndPassword(const AString& username,
                                                 const AString& password)
{
    s_fileReadingUsername = username;
    s_fileReadingPassword = password;
}

/**
 * @return The username for file reading from database or website.
 */
AString
CaretDataFile::getFileReadingUsername()
{
    return s_fileReadingUsername;
}

/**
 * @return The password for file reading from database or website.
 */
AString
CaretDataFile::getFileReadingPassword()
{
    return s_fileReadingPassword;
}

/**
 * Create a scene for an instance of a class.
 *
 * NOTE: In most cases, subclasses should not override this method but
 * instead override  saveFileDataToScene() use it to add data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  If there is no data for the scene, a NULL pointer
 *    will be returned.
 */
SceneClass*
CaretDataFile::saveToScene(const SceneAttributes* sceneAttributes,
                                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "CaretDataFile",
                                            1);
    
    sceneClass->addClass(m_fileIdentificationAttributes->saveToScene(sceneAttributes,
                                                                     "m_fileIdentificationAttributes"));
    
    saveFileDataToScene(sceneAttributes,
                        sceneClass);
    
    if (sceneClass->getNumberOfObjects() <= 0) {
        delete sceneClass;
        sceneClass = NULL;
    }
    //    const int32_t numMaps = getNumberOfMaps();
    //    if (numMaps > 0) {
    //        bool* mapEnabledArray = new bool[numMaps];
    //        for (int32_t i = 0; i < numMaps; i++) {
    //            mapEnabledArray[i] = m_mapContent[i]->m_dataLoadingEnabled;
    //        }
    //
    //        sceneClass->addBooleanArray("mapEnabled",
    //                                    mapEnabledArray,
    //                                    numMaps);
    //        delete[] mapEnabledArray;
    //    }
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 *
 * NOTE: In most cases, subclasses should not override this method but
 * instead override restoreFileDataFromScene() use it to access 
 * data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
CaretDataFile::restoreFromScene(const SceneAttributes* sceneAttributes,
                                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_fileIdentificationAttributes->restoreFromScene(sceneAttributes,
                                                     sceneClass->getClass("m_fileIdentificationAttributes"));
    /*
     * Resoration of file data from the scene may cause
     * a modified palette status if the palette color
     * mapping was saved to the scene.  We want to
     * keep this modified status so that if the scene
     * is resaved, the modified palette information
     * is added to the scene.
     */
    restoreFileDataFromScene(sceneAttributes,
                             sceneClass);
}

/**
 * Save file data from the scene.  For subclasses that need to
 * save to a scene, this method should be overriden.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
CaretDataFile::saveFileDataToScene(const SceneAttributes* /*sceneAttributes*/,
                                           SceneClass* /*sceneClass*/)
{
    /* Nothing as subclasses needing to save to scenes will override. */
}

/**
 * Restore file data from the scene.  For subclasses that need to
 * restore from a scene, this method should be overridden. The scene class
 * will be valid and any scene data may be obtained from it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
CaretDataFile::restoreFileDataFromScene(const SceneAttributes* /*sceneAttributes*/,
                                                const SceneClass* /*sceneClass*/)
{
    /* Nothing as subclasses needing to restore from scenes will override. */
}

/**
 * @return True if this file type supports writing, else false.
 *
 * By default, this method returns true.  Files that do not support
 * writing should override this method and return false.
 */
bool
CaretDataFile::supportsWriting() const
{
    return true;
}

/**
 * @return The name of the file with both the file's path and
 * filename's extension removed.
 *
 * First, the filename is tested to see if it ends with any of the valid
 * file extensions for the DataFileTypeEnum.  If any of the extensions match,
 * the filename, with the extension removed, is returned.  The valud
 * file extensions are used since many GIFTI and CIFTI files contain
 * a "." in their extensions (eg: .surf.gii   .dconn.nii).
 *
 *  /mnt/path/anatomical.surf.gii "returns" anatomical
 *
 * Second, the last "/" (directory separator) is found to locate where
 * the name of the file, excluding the path, is located.  Using just
 * the name of the file, anything before the last "." is returned.
 * removed.
 *
 * Third, if there is not "." in the name of the file
 * the equivalent of getFileName() is returned.
 */
AString
CaretDataFile::getFileNameNoPathNoExtension() const
{
    AString nameNoExt = getFileNameNoExtension();
    const int lastSlashIndex = std::max(nameNoExt.lastIndexOf("/"),
                                        nameNoExt.lastIndexOf("\\"));
    if (lastSlashIndex >= 0) {
        if ((lastSlashIndex + 1) < nameNoExt.length()) {
            nameNoExt = nameNoExt.mid(lastSlashIndex + 1);
        }
    }
    
    return nameNoExt;
}


/**
 * @return The name (and path if present) of the file with the
 * filename's extension removed.  
 *
 * First, the filename is tested to see if it ends with any of the valid
 * file extensions for the DataFileTypeEnum.  If any of the extensions match,
 * the filename, with the extension removed, is returned.  The valud
 * file extensions are used since many GIFTI and CIFTI files contain
 * a "." in their extensions (eg: .surf.gii   .dconn.nii).
 *
 *  /mnt/path/anatomical.surf.gii "returns" /mnt/path/anatomical
 *
 * Second, the last "/" (directory separator) is found to locate where
 * the name of the file, excluding the path, is located.  Using just
 * the name of the file, anything before the last "." is returned.
 * removed.  
 *
 * Third, if there is not "." in the name of the file
 * the equivalent of getFileName() is returned.
 */
AString
CaretDataFile::getFileNameNoExtension() const
{
    AString name = getFileName();
    
    std::vector<AString> dataFileTypeExtensions = DataFileTypeEnum::getAllFileExtensions(getDataFileType());
    
    /*
     * Test using file type extensions
     */
    for (std::vector<AString>::iterator iter = dataFileTypeExtensions.begin();
         iter != dataFileTypeExtensions.end();
         iter++) {
        const AString ext = *iter;
        const int offset = name.lastIndexOf(ext);
        if (offset > 0) {
            name.resize(offset - 1);
            return name;
        }
    }
    
    /*
     * Look for the last "." and the last forward slash or back slash
     *
     * Dont' was to chop off a "." in the path (eg: /mnt/back.up/file)
     */
    const int lastSlashIndex = std::max(name.lastIndexOf("/"),
                                        name.lastIndexOf("\\"));
    
    const int dotIndex = name.lastIndexOf(".");
    if (lastSlashIndex > 0) {
        if (dotIndex > lastSlashIndex) {
            name.resize(dotIndex);
        }
    }
    else {
        if (dotIndex > 0) {
            name.resize(dotIndex);
        }
    }
    
    return name;
}

/**
 * @return File casted to a CZI image file (avoids use of dynamic_cast that can be slow)
 */
CziImageFile*
CaretDataFile::castToCziImageFile()
{
    return NULL;
}

/**
 * @return File casted to a CZI image file (avoids use of dynamic_cast that can be slow)
 * Overidden in ImageFile
 */
const CziImageFile*
CaretDataFile::castToCziImageFile() const
{
    return NULL;
}

/**
 * @return File casted to an image file (avoids use of dynamic_cast that can be slow)
 */
ImageFile*
CaretDataFile::castToImageFile()
{
    return NULL;
}

/**
 * @return File casted to an image file (avoids use of dynamic_cast that can be slow)
 * Overidden in ImageFile
 */
const ImageFile*
CaretDataFile::castToImageFile() const
{
    return NULL;
}


/**
 * @return File casted to an media file (avoids use of dynamic_cast that can be slow)
 * Overidden in MediaFile
 */
MediaFile*
CaretDataFile::castToMediaFile()
{
    return NULL;
}

/**
 * @return File casted to an media file (avoids use of dynamic_cast that can be slow)
 * Overidden in ImageFile
 */
const MediaFile*
CaretDataFile::castToMediaFile() const
{
    return NULL;
}

/**
 * @return File casted to caret mappable data  file (avoids use of dynamic_cast that can be slow)
 * Overidden in MediaFile
 */
CaretMappableDataFile*
CaretDataFile::castToCaretMappableDataFile()
{
    return NULL;
}

/**
 * @return File casted to an caret mappable data file (avoids use of dynamic_cast that can be slow)
 * Overidden in ImageFile
 */
const CaretMappableDataFile*
CaretDataFile::castToCaretMappableDataFile() const
{
    return NULL;
}

/**
 * @return File casted to a histology slices file (avoids use of dynamic_cast that can be slow)
 * Overidden in HistologySlicesFile
 */
const HistologySlicesFile*
CaretDataFile::castToHistologySlicesFile() const
{
    return NULL;
}

/**
 * @return File casted to histology slices  file (avoids use of dynamic_cast that can be slow)
 * Overidden in HistologySlicesFile
 */
HistologySlicesFile*
CaretDataFile::castToHistologySlicesFile()
{
    return NULL;
}

/**
 * @return File casted to a OME-ZARR Image File (avoids use of dynamic_cast that can be slow)
 * Overidden in OmeZarrImageFile
 */
const OmeZarrImageFile*
CaretDataFile::castToOmeZarrImageFile() const
{
    return NULL;
}

/**
 * @return File casted to a OME-ZARR Image File (avoids use of dynamic_cast that can be slow)
 * Overidden in OmeZarrImageFile
 */
OmeZarrImageFile*
CaretDataFile::castToOmeZarrImageFile()
{
    return NULL;
}

/**
 * @return The file identification attributes
 */
FileIdentificationAttributes*
CaretDataFile::getFileIdentificationAttributes()
{
    return m_fileIdentificationAttributes.get();
}

/**
 * @return The file identification attributes (const method)
 */
const FileIdentificationAttributes*
CaretDataFile::getFileIdentificationAttributes() const
{
    return m_fileIdentificationAttributes.get();
}


