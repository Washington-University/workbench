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

#include <memory>
#include <set>

#include <QTextStream>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPointer.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "EventGetDisplayedDataFiles.h"
#include "EventManager.h"
#include "FileAdapter.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#define __SPEC_FILE_DEFINE__
#include "SpecFile.h"
#undef __SPEC_FILE_DEFINE__
#include "SpecFileDataFile.h"
#include "SpecFileDataFileTypeGroup.h"
#include "SpecFileSaxReader.h"
#include "StringTableModel.h"
#include "SystemUtilities.h"
#include "XmlSaxParser.h"
#include "XmlWriter.h"

using namespace caret;

/**
 * \class caret::SpecFile
 * \brief A spec file groups caret data files.
 * \ingroup Files
 */


/**
 * Constructor.
 */
SpecFile::SpecFile()
: CaretDataFile(DataFileTypeEnum::SPECIFICATION)
{
    this->initializeSpecFile();
}

/**
 * Destructor.
 */
SpecFile::~SpecFile()
{
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        delete *iter;
    }
    this->dataFileTypeGroups.clear();
    
    delete this->metadata;
    this->metadata = NULL;
}

/**
 * Copy constructor.
 * @param sf
 *    Spec file whose data is copied.
 */
SpecFile::SpecFile(const SpecFile& sf)
: CaretDataFile(sf)
{
    this->initializeSpecFile();
    this->copyHelperSpecFile(sf);
}

/**
 * Assignment operator.
 * @param sf
 *    Spec file whose data is copied.
 * @return
 *    Reference to this file.
 */
SpecFile& 
SpecFile::operator=(const SpecFile& sf)
{
    if (this != &sf) {
        CaretDataFile::operator=(sf);
        this->copyHelperSpecFile(sf);
    }
    
    return *this;
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
SpecFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
SpecFile::setStructure(const StructureEnum::Enum /* structure */)
{
    /* nothing since spec file not structure type file */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
SpecFile::getFileMetaData()
{
    return metadata;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
SpecFile::getFileMetaData() const
{
    return metadata;
}

/**
 * Initialize this spec file.
 */
void
SpecFile::initializeSpecFile()
{
    this->metadata = new GiftiMetaData();
    
    std::vector<DataFileTypeEnum::Enum> allEnums;
    DataFileTypeEnum::getAllEnums(allEnums,
                                  DataFileTypeEnum::OPTIONS_NONE);
    
    /*
     * Do surface files first since they need to be loaded before other files
     */
    for (std::vector<DataFileTypeEnum::Enum>::iterator iter = allEnums.begin();
         iter != allEnums.end();
         iter++) {
        DataFileTypeEnum::Enum dataFileType = *iter;
        const AString typeName = DataFileTypeEnum::toName(dataFileType);
        if (typeName.startsWith("SURFACE")) {
            SpecFileDataFileTypeGroup* dftg = new SpecFileDataFileTypeGroup(dataFileType);
            this->dataFileTypeGroups.push_back(dftg);
        }
    }
    
    /*
     * Do remaining file types excluding surfaces
     */
    for (std::vector<DataFileTypeEnum::Enum>::iterator iter = allEnums.begin();
         iter != allEnums.end();
         iter++) {
        DataFileTypeEnum::Enum dataFileType = *iter;
        if (dataFileType == DataFileTypeEnum::UNKNOWN) {
            // ignore
        }
        else {
            const AString typeName = DataFileTypeEnum::toName(dataFileType);
            if (typeName.startsWith("SURFACE") == false) {
                SpecFileDataFileTypeGroup* dftg = new SpecFileDataFileTypeGroup(dataFileType);
                this->dataFileTypeGroups.push_back(dftg);
            }
        }
    }
}

/**
 * Copy helper.
 * @param sf
 *    Spec file whose data is copied.
 */
void 
SpecFile::copyHelperSpecFile(const SpecFile& sf)
{
    this->clearData();
    
    if (this->metadata != NULL) {
        delete this->metadata;
    }
    this->metadata = new GiftiMetaData(*sf.metadata);
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = sf.dataFileTypeGroups.begin();
         iter != sf.dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* group = *iter;
        const int numFiles = group->getNumberOfFiles();
        for (int32_t i = 0; i < numFiles; i++) {
            SpecFileDataFile* file = group->getFileInformation(i);
            this->addDataFile(group->getDataFileType(), 
                              file->getStructure(), 
                              file->getFileName(),
                              file->isLoadingSelected(),
                              file->isSavingSelected(),
                              file->isSpecFileMember());
        }
    }
    
    this->setFileName(sf.getFileName());
    this->clearModified();
}

/**
 * Change the name of a file.  The existing SpecFileDataFile remains with 
 * its CaretDataFile removed and a new SpecFileDataFile is created with 
 * the CaretDataFile.
 *
 * @param specFileDataFile
 *    The SpecFileDataFile that has its name changed.
 * @param newFileName
 *    New name for file.
 * @return New SpecFileDataFile that is created or NULL if filename did not
 * change of there is an error.
 */
SpecFileDataFile*
SpecFile::changeFileName(SpecFileDataFile* specFileDataFile,
                         const AString& newFileName)
{
    CaretAssert(specFileDataFile);
    
    /*
     * Make sure name changed.
     */
    if (specFileDataFile->getFileName() == newFileName) {
        return NULL;
    }
    
    /*
     * Create a new SpecFileDataFile
     */
    SpecFileDataFile* newSpecFileDataFile = new SpecFileDataFile(*specFileDataFile);
    newSpecFileDataFile->setFileName(newFileName);
    newSpecFileDataFile->setCaretDataFile(specFileDataFile->getCaretDataFile());

    /*
     * Remove CaretDataFile from previous SpecFileDataFile
     */
    specFileDataFile->setCaretDataFile(NULL);
    specFileDataFile->setSavingSelected(false);
    
    /*
     * Add new SpecFileDataFile to appropriate group.
     */
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* group = *iter;
        if (group->getDataFileType() == newSpecFileDataFile->getDataFileType()) {
            group->addFileInformation(newSpecFileDataFile);
            return newSpecFileDataFile;
        }
    }
    
    CaretAssert(0);
    CaretLogSevere("PROGRAM ERROR: Failed to match DataFileType");
    return NULL;
}

/**
 * Add a Caret Data File.
 *
 * @param If there is a a spec file entry with the same name as the Caret
 * Data File, the caret data file is added to the spec file entry.  
 * Otherwise, a new Spec File Entry is created with the given Caret
 * Data File.
 *
 * @param caretDataFile
 *    Caret data file that is added to, or creates, a spec file entry.
 */
void
SpecFile::addCaretDataFile(CaretDataFile* caretDataFile)
{
    CaretAssert(caretDataFile);
    
    if ( ! SpecFile::isDataFileTypeAllowedInSpecFile(caretDataFile->getDataFileType())) {
        return;
    }
    
    /*
     * Matches to first file found that has matching name and data file type
     */
    SpecFileDataFile* matchedSpecFileDataFile = NULL;
    
    /*
     * Matches to first file found that has matching name and data file type
     * AND has its caret data file with a NULL value.
     */
    SpecFileDataFile* matchedSpecFileDataFileWithNULL = NULL;
    
    /*
     * Group that matches data file type
     */
    SpecFileDataFileTypeGroup* matchedDataFileTypeGroup = NULL;
    
    /*
     * Find matches
     */
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        if (dataFileTypeGroup->getDataFileType() == caretDataFile->getDataFileType()) {
            matchedDataFileTypeGroup = dataFileTypeGroup;

            const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
            for (int32_t i = 0; i < numFiles; i++) {
                SpecFileDataFile* sfdf = dataFileTypeGroup->getFileInformation(i);
                if (sfdf->getCaretDataFile() == caretDataFile) {
                    return;
                }
                if (sfdf->getFileName() == caretDataFile->getFileName()) {
                    matchedSpecFileDataFile = sfdf;
                    if (sfdf->getCaretDataFile() == NULL) {
                        matchedSpecFileDataFileWithNULL = sfdf;
                        break;
                    }
                }
            }
        }
        if (matchedSpecFileDataFileWithNULL != NULL) {
            break;
        }
    }
    
    CaretAssert(matchedDataFileTypeGroup);
    
    SpecFileDataFile* specFileDataFileToUpdate = NULL;
    if (matchedSpecFileDataFileWithNULL != NULL) {
        /*
         * Found item that matched with a NULL value for caret data file
         */
        specFileDataFileToUpdate = matchedSpecFileDataFileWithNULL;
    }
    else if (matchedSpecFileDataFile != NULL) {
        /*
         * Found item that matched but had non-NULL value for caret data file.
         * This means that there is a copy of a file loaded (two files same name)
         */
        specFileDataFileToUpdate = new SpecFileDataFile(*matchedSpecFileDataFile);
        matchedDataFileTypeGroup->addFileInformation(specFileDataFileToUpdate);
    }
    else {
        /*
         * No matches found, file is not in spec file
         */
        specFileDataFileToUpdate = new SpecFileDataFile(caretDataFile->getFileName(),
                                                        caretDataFile->getDataFileType(),
                                                        caretDataFile->getStructure(),
                                                        false);
        matchedDataFileTypeGroup->addFileInformation(specFileDataFileToUpdate);
    }
    
    specFileDataFileToUpdate->setStructure(caretDataFile->getStructure());
    specFileDataFileToUpdate->setCaretDataFile(caretDataFile);
}

/**
 * Remove a Caret Data File.
 *
 * @param If there is a a spec file entry with the given caret data file
 * remove it.  Note: file has likely already been deleted so use only the
 * the caret data file pointer but to not deference it.
 *
 * @param caretDataFile
 *    Caret data file that is removed from a spec file entry.
 * @param logSevereIfFailureToRemoveFileFlag
 *   If true, log a message if failure to remove file
 */
bool
SpecFile::removeCaretDataFile(const CaretDataFile* caretDataFile,
                              const bool logSevereIfFailureToRemoveFileFlag)
{
    CaretAssert(caretDataFile);
    
    /*
     * Get the entry
     */
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
        for (int32_t i = 0; i < numFiles; i++) {
            SpecFileDataFile* sfdf = dataFileTypeGroup->getFileInformation(i);
            if (sfdf->getCaretDataFile() == caretDataFile) {
                sfdf->setCaretDataFile(NULL);
                return true;
            }
        }
    }
    
    if (logSevereIfFailureToRemoveFileFlag) {
        CaretLogSevere("Failed to remove CaretDataFile at address "
                       + AString::number((qulonglong)caretDataFile)
                       + " from SpecFile: "
                       + getFileName());
    }
    
    return false;
}


/**
 * Add a data file to this spec file.
 *
 * @param dataFileType
 *   Type of data file.
 * @param structure
 *   Structure of data file (not all files use structure).
 * @param filename
 *   Name of the file.
 * @param fileLoadingSelectionStatus
 *   Selection status for loading of the file.
 * @param fileSavingSelectionStatus
 *   Selection status for saving of the file.
 * @param specFileMemberStatus
 *   True if the file is a member of the spec file and is written
 *   into the spec file.
 *
 * @throws DataFileException
 *   If data file type is UNKNOWN.
 */
void 
SpecFile::addDataFile(const DataFileTypeEnum::Enum dataFileType,
                      const StructureEnum::Enum structure,
                      const AString& filename,
                      const bool fileLoadingSelectionStatus,
                      const bool fileSavingSelectionStatus,
                      const bool specFileMemberStatus)
{
    addDataFilePrivate(dataFileType,
                       structure,
                       filename,
                       fileLoadingSelectionStatus,
                       fileSavingSelectionStatus,
                       specFileMemberStatus);
}

/**
 * Add a data file to this spec file.
 *
 * @param dataFileType
 *   Type of data file.
 * @param structure
 *   Structure of data file (not all files use structure).
 * @param filename
 *   Name of the file.
 * @param fileLoadingSelectionStatus
 *   Selection status for loading of the file.
 * @param fileSavingSelectionStatus
 *   Selection status for saving of the file.
 * @param specFileMemberStatus
 *   True if the file is a member of the spec file and is written
 *   into the spec file.
 * @return
 *   SpecFileDataFile that was created or matched.  NULL if error or file type not allowed.
 *
 * @throws DataFileException
 *   If data file type is UNKNOWN.
 */
SpecFileDataFile*
SpecFile::addDataFilePrivate(const DataFileTypeEnum::Enum dataFileType,
                             const StructureEnum::Enum structure,
                             const AString& filename,
                             const bool fileLoadingSelectionStatus,
                             const bool fileSavingSelectionStatus,
                             const bool specFileMemberStatus)
{
    if ( ! SpecFile::isDataFileTypeAllowedInSpecFile(dataFileType)) {
        return NULL;
    }
    
    AString name = filename;

    const bool dataFileOnNetwork = DataFile::isFileOnNetwork(name);

    //NOTE: the spec file's location is completely irrelevant to this operation!
    if (!dataFileOnNetwork) {
        FileInformation fileInfo(name);
        if (fileInfo.isRelative()) {
            name = fileInfo.getAbsoluteFilePath();
        }
    }//if it is on the network, don't modify it
    
//    const AString message = ("After adding, " 
//                             + filename
//                             + " becomes " 
//                             + name);
//    CaretLogFine(message);
    
//    if (this->getFileName().isEmpty() == false) {
//        name = SystemUtilities::relativePath(name, FileInformation(this->getFileName()).getPathName());
//    }
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        if (dataFileTypeGroup->getDataFileType() == dataFileType) {
            /*
             * If already in file, no need to add it a second time but do update
             * its selection status if new entry has file selected
             */
            const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
            for (int32_t i = 0; i < numFiles; i++) {
                SpecFileDataFile* sfdf = dataFileTypeGroup->getFileInformation(i);
                if (sfdf->getFileName() == name) {
                    if (fileLoadingSelectionStatus) {
                        sfdf->setLoadingSelected(fileLoadingSelectionStatus);
                    }
                    if (fileSavingSelectionStatus) {
                        sfdf->setSavingSelected(fileSavingSelectionStatus);
                    }
                    return sfdf;
                }
            }
            
            SpecFileDataFile* sfdf = new SpecFileDataFile(name,
                                                          dataFileType,
                                                          structure,
                                                          specFileMemberStatus);
            sfdf->setLoadingSelected(fileLoadingSelectionStatus);
            sfdf->setSavingSelected(fileSavingSelectionStatus);
            dataFileTypeGroup->addFileInformation(sfdf);
            return sfdf;
        }
    }
                        
    DataFileException e(getFileName(),
                        "Data File Type: "
                        + DataFileTypeEnum::toName(dataFileType)
                        + " not allowed "
                        + " for file "
                        + filename);
    CaretLogThrowing(e);
    throw e;
    
    return NULL; // will never get here since exception thrown
}

///**
// * @return ALL of the connectivity file types (NEVER delete contents of returned vector.
// */
//void 
//SpecFile::getAllConnectivityFileTypes(std::vector<SpecFileDataFile*>& connectivityDataFilesOut)
//{
//    connectivityDataFilesOut.clear();
//    
//    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
//         iter != dataFileTypeGroups.end();
//         iter++) {
//        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
//        if (DataFileTypeEnum::isConnectivityDataType(dataFileTypeGroup->getDataFileType())) {
//            const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
//            for (int32_t i = 0; i < numFiles; i++) {
//                connectivityDataFilesOut.push_back(dataFileTypeGroup->getFileInformation(i));
//            }
//        }
//    }
//}

/**
 * Set the selection status of a data file.
 * @param dataFileTypeName
 *   Name of type of data file.
 * @param structure
 *   Name of Structure of data file (not all files use structure).
 * @param filename
 *   Name of the file.
 * @param fileSelectionStatus
 *   Selection status of file.
 */
void 
SpecFile::setFileLoadingSelectionStatus(const DataFileTypeEnum::Enum dataFileType,
                                 const StructureEnum::Enum structure,
                                 const AString& filename,
                                 const bool fileSelectionStatus)
{    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        if (dataFileTypeGroup->getDataFileType() == dataFileType) {
            const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
            for (int32_t i = 0; i < numFiles; i++) {
                SpecFileDataFile* sfdf = dataFileTypeGroup->getFileInformation(i);
                if (sfdf->getStructure() == structure) {
                    if (sfdf->getFileName().endsWith(filename)) {
                        sfdf->setLoadingSelected(fileSelectionStatus);
                    }
                }
            }
        }
    }
}


/**
 * Add a data file to this spec file.
 * @param dataFileTypeName
 *   Name of type of data file.
 * @param structure
 *   Name of Structure of data file (not all files use structure).
 * @param filename
 *   Name of the file.
 * @param fileLoadingSelectionStatus
 *   Selection status for loading of file.
 * @param fileSavingSelectionStatus
 *   Selection status for saving of file.
 * @param specFileMemberStatus
 *   True if the file is a member of the spec file and is written
 *   into the spec file.
 *
 * @throws DataFileException
 *   If data file type is UNKNOWN.
 */
void 
SpecFile::addDataFile(const AString& dataFileTypeName,
                      const AString& structureName,
                      const AString& filename,
                      const bool fileLoadingSelectionStatus,
                      const bool fileSavingSelectionStatus,
                      const bool specFileMemberStatus)
{
    bool validType = false;
    DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromName(dataFileTypeName, &validType);
    bool validStructure = false;
    StructureEnum::Enum structure = StructureEnum::fromGuiName(structureName, &validStructure);
    this->addDataFilePrivate(dataFileType,
                             structure,
                             filename,
                             fileLoadingSelectionStatus,
                             fileSavingSelectionStatus,
                             specFileMemberStatus);
}


/**
 * Clear the file..
 */
void 
SpecFile::clear()
{
    DataFile::clear();
    
    this->clearData();
}

/**
 * Clear the spec file's data as if there were no files loaded.
 */
void 
SpecFile::clearData()
{
    this->metadata->clear();
    
    /*
     * Do not clear the vector, just remove file information from all types
     */
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        dataFileTypeGroup->removeAllFileInformation();
    }
}
        
/**
 * Is this file empty?
 *
 * @return true if file is empty, else false.
 */
bool 
SpecFile::isEmpty() const
{
    return (this->getNumberOfFiles() <= 0);
}

/**
 * @return The number of files.
 */
int32_t 
SpecFile::getNumberOfFiles() const
{
    int count = 0;
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        count += dataFileTypeGroup->getNumberOfFiles();
    }
    
    return count;
}

/**
 * @return The number of files selected for loading.
 */
int32_t 
SpecFile::getNumberOfFilesSelectedForLoading() const
{
    int count = 0;
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        count += dataFileTypeGroup->getNumberOfFilesSelectedForLoading();
    }
    
    return count;
}

/**
 * @return The number of files selected for saving.
 */
int32_t
SpecFile::getNumberOfFilesSelectedForSaving() const
{
    int count = 0;
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        count += dataFileTypeGroup->getNumberOfFilesSelectedForSaving();
    }
    
    return count;
}

/**
 * @return True if there is at least one file with a remote path 
 * (http...) AND the file is selected for loading.
 */
bool
SpecFile::hasFilesWithRemotePathSelectedForLoading() const
{
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        const SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
        for (int32_t i = 0; i < numFiles; i++) {
            const SpecFileDataFile* sfdf = dataFileTypeGroup->getFileInformation(i);
            if (sfdf->isLoadingSelected()) {
                if (DataFile::isFileOnNetwork(sfdf->getFileName())) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

/**
 * @return A vector containing all file names selected for loading.
 */
std::vector<AString>
SpecFile::getAllDataFileNamesSelectedForLoading() const
{
    std::vector<AString> allFileNames;
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        
        const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
        for (int32_t i = 0; i < numFiles; i++) {
            if (dataFileTypeGroup->getFileInformation(i)->isLoadingSelected()) {
                const AString filename = dataFileTypeGroup->getFileInformation(i)->getFileName();
                allFileNames.push_back(filename);
            }
        }
    }
    
    return allFileNames;
}


/**
 * @return A vector containing all file names.
 */
std::vector<AString>
SpecFile::getAllDataFileNames() const
{
    std::vector<AString> allFileNames;
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        
        const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
        for (int32_t i = 0; i < numFiles; i++) {
            const AString filename = dataFileTypeGroup->getFileInformation(i)->getFileName();
            allFileNames.push_back(filename);
        }
    }
    
    return allFileNames;
}

/**
 * @return True if the only files selected for loading are scene files.
 */
bool
SpecFile::areAllFilesSelectedForLoadingSceneFiles() const
{
    int32_t sceneFileCount = 0;
    int32_t allFilesCount  = 0;
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        allFilesCount += dataFileTypeGroup->getNumberOfFilesSelectedForLoading();
        
        if (dataFileTypeGroup->getDataFileType() == DataFileTypeEnum::SCENE) {
            sceneFileCount += dataFileTypeGroup->getNumberOfFilesSelectedForLoading();
        }
    }

    if (sceneFileCount > 0) {
        if (sceneFileCount == allFilesCount) {
            return true;
        }
    }
    
    return false;
}

/**
 * Remove any files that are not "in spec" and do not have an
 * associated caret data file.
 */
void
SpecFile::removeAnyFileInformationIfNotInSpecAndNoCaretDataFile()
{
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        dataFileTypeGroup->removeFileInformationIfNotInSpecAndNoCaretDataFile();
    }
    setModified();
}


/**
 * Read the file.
 *
 * @param filenameIn
 *    Name of file to read.
 *
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
SpecFile::readFile(const AString& filenameIn)
{
    clear();
    
    AString filename = filenameIn;
    if (DataFile::isFileOnNetwork(filename) == false) {
        FileInformation specInfo(filename);
        filename = specInfo.getAbsoluteFilePath();
    }
    this->setFileName(filename);
    
    checkFileReadability(filename);
    
    SpecFileSaxReader saxReader(this);
    std::unique_ptr<XmlSaxParser> parser(XmlSaxParser::createXmlParser());
    try {
        parser->parseFile(filename, &saxReader);
    }
    catch (const XmlSaxParserException& e) {
        clear();
        this->setFileName("");
        
        int lineNum = e.getLineNumber();
        int colNum  = e.getColumnNumber();
        
        AString msg = "Parse Error while reading:";
        
        if ((lineNum >= 0) && (colNum >= 0)) {
            msg += (" line/col ("
                    + AString::number(e.getLineNumber())
                    + "/"
                    + AString::number(e.getColumnNumber())
                    + ")");
        }
        
        msg += (": " + e.whatString());
        
        DataFileException dfe(filename,
                              msg);
        CaretLogThrowing(dfe);
        throw dfe;
    }

    this->setFileName(filename);
    this->setAllFilesSelectedForLoading(true);
    this->setAllFilesSelectedForSaving(false);
    
    this->clearModified();
}

/**
 * Read the spec file from a string containing the files content.
 * @param string
 *    String containing the file's content.
 * @throws DataFileException
 *    If there is an error reading the file from the string.
 */
void 
SpecFile::readFileFromString(const AString& string)
{
    SpecFileSaxReader saxReader(this);
    std::unique_ptr<XmlSaxParser> parser(XmlSaxParser::createXmlParser());
    try {
        parser->parseString(string, &saxReader);
    }
    catch (const XmlSaxParserException& e) {
        clear();
        this->setFileName("");
        
        int lineNum = e.getLineNumber();
        int colNum  = e.getColumnNumber();
        
        AString msg = "Parse Error while reading Spec File from string.";
        
        if ((lineNum >= 0) && (colNum >= 0)) {
            msg += (" line/col ("
                    + AString::number(e.getLineNumber())
                    + "/"
                    + AString::number(e.getColumnNumber())
                    + ")");
        }
        
        msg += (": " + e.whatString());
        
        DataFileException dfe(getFileName(),
                              msg);
        CaretLogThrowing(dfe);
        throw dfe;
    }
    
    this->clearModified();
}

/**
 * Write the file.
 *
 * @param filename
 *    Name of file to read.
 *
 * @throws DataFileException
 *    If there is an error writing the file.
 */
void 
SpecFile::writeFile(const AString& filename)
{
    if (!(filename.endsWith(".spec") || filename.endsWith(".wb_spec")))
    {
        CaretLogWarning("spec file '" + filename + "' should be saved ending in .spec");
    }
    checkFileWritability(filename);
    
    FileInformation specInfo(filename);
    AString absFileName = specInfo.getAbsoluteFilePath();
    this->setFileName(absFileName);

    try {
        //
        // Format the version string so that it ends with at most one zero
        //
        const AString versionString = AString::number(1.0);
        
        //
        // Open the file
        //
        FileAdapter file;
        AString errorMessage;
        QTextStream* textStream = file.openQTextStreamForWritingFile(this->getFileName(),
                                                                     errorMessage);
        if (textStream == NULL) {
            throw DataFileException(getFileName(),
                                    errorMessage);
        }

        //
        // Create the xml writer
        //
        XmlWriter xmlWriter(*textStream);
        
        /*
         * Write the XML and include metadata
         */
        this->writeFileContentToXML(xmlWriter, 
                                    WRITE_META_DATA_YES,
                                    WRITE_IN_SPEC_FILES);
                
        file.close();
        
        this->clearModified();
    }
    catch (const GiftiException& e) {
        throw DataFileException(getFileName(),
                                e.whatString());
    }
    catch (const XmlException& e) {
        throw DataFileException(getFileName(),
                                e.whatString());
    }
}

/**
 * Write the file's content to the XML Writer.
 * @param xmlWriter
 *    XML Writer to which file content is written.
 * @param writeMetaDataStatus
 *    Yes of no to write metadata.
 * @throws DataFileException
 *    If there is an error writing to the XML writer.
 */
void 
SpecFile::writeFileContentToXML(XmlWriter& xmlWriter,
                                const WriteMetaDataType writeMetaDataStatus,
                                const WriteFilesSelectedType writeFilesSelectedStatus)
{    
    //
    // Write header info
    //
    xmlWriter.writeStartDocument("1.0");
    
    //
    // Write GIFTI root element
    //
    XmlAttributes attributes;
    
    //attributes.addAttribute("xmlns:xsi",
    //                        "http://www.w3.org/2001/XMLSchema-instance");
    //attributes.addAttribute("xsi:noNamespaceSchemaLocation",
    //                        "http://brainvis.wustl.edu/caret6/xml_schemas/GIFTI_Caret.xsd");
    attributes.addAttribute(SpecFile::XML_ATTRIBUTE_VERSION,
                            SpecFile::getFileVersionAsString());
    xmlWriter.writeStartElement(SpecFile::XML_TAG_SPEC_FILE,
                                attributes);
    
    //
    // Write Metadata
    //
    if (writeMetaDataStatus == WRITE_META_DATA_YES) {
        if (metadata != NULL) {
            metadata->writeAsXML(xmlWriter);
        }
    }
    
    //
    // Write files
    //
    const int32_t numGroups = this->getNumberOfDataFileTypeGroups();
    for (int32_t i = 0; i < numGroups; i++) {
        SpecFileDataFileTypeGroup* group = this->getDataFileTypeGroupByIndex(i);
        const int32_t numFiles = group->getNumberOfFiles();
        for (int32_t j = 0; j < numFiles; j++) {
            SpecFileDataFile* file = group->getFileInformation(j);
            
                bool writeIt = true;
                switch (writeFilesSelectedStatus) {
                    case WRITE_ALL_FILES:
                        break;
                    case WRITE_IN_SPEC_FILES:
                        writeIt = file->isSpecFileMember();
                        break;
                }
                
                if (writeIt) {
                    const AString name = updateFileNameAndPathForWriting(file->getFileName());
                    
                    XmlAttributes atts;
                    atts.addAttribute(SpecFile::XML_ATTRIBUTE_STRUCTURE, 
                                      StructureEnum::toGuiName(file->getStructure()));
                    atts.addAttribute(SpecFile::XML_ATTRIBUTE_DATA_FILE_TYPE, 
                                      DataFileTypeEnum::toName(group->getDataFileType()));
                    atts.addAttribute(SpecFile::XML_ATTRIBUTE_SELECTED, 
                                      file->isLoadingSelected());
                    xmlWriter.writeStartElement(SpecFile::XML_TAG_DATA_FILE, 
                                                atts);
                    xmlWriter.writeCharacters("      " 
                                              + name 
                                              + "\n");
                    xmlWriter.writeEndElement();
                }
        }
    }
    
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
}

/**
 * Update the file name for writing to a spec file 
 * (makes file relative to spec file location).
 */
AString 
SpecFile::updateFileNameAndPathForWriting(const AString& dataFileNameIn)
{
    AString dataFileName = dataFileNameIn;
    
    FileInformation fileInfo(dataFileName);
    if (fileInfo.isAbsolute()) {
        const AString specFileName = getFileName();
        FileInformation specFileInfo(specFileName);
        if (specFileInfo.isAbsolute()) {
            const AString newPath = SystemUtilities::relativePath(fileInfo.getPathName(),
                                                                  specFileInfo.getPathName());
            if (newPath.isEmpty()) {
                dataFileName = fileInfo.getFileName();
            }
            else {
                dataFileName = (newPath
                                + "/"
                                + fileInfo.getFileName());
            }
        }
    }
    
    AString message = ("When writing, " 
                       + dataFileNameIn
                       + " becomes " 
                       + dataFileName);
    CaretLogFine(message);
    
    return dataFileName;
}


/**
 * Write the file to a XML string.
 * @param writeMetaDataStatus
 *    Write the metadata to the file.
 * @return 
 *    String containing XML.
 * @throws DataFileException
 *    If error writing to XML.
 */
//AString 
//SpecFile::writeFileToString(const WriteMetaDataType writeMetaDataStatus,
//                            const WriteFilesSelectedType writeFilesSelectedStatus)
//{    
//    /*
//     * Create a TextStream that writes to a string.
//     */
//    AString xmlString;
//    QTextStream textStream(&xmlString);
//    
//    /*
//     * Create the xml writer
//     */
//    XmlWriter xmlWriter(textStream);
//
//    /*
//     * Write file to XML.
//     */
//    this->writeFileContentToXML(xmlWriter,
//                                writeMetaDataStatus,
//                                writeFilesSelectedStatus);
//    
//    return xmlString;
//}

/**
 * Get information about this file's contents.
 * @return
 *    Information about the file's contents.
 */
AString 
SpecFile::toString() const
{
    AString info = "name=" + this->getFileName() + "\n";
    info += this->metadata->toString() + "\n";
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        info += (dataFileTypeGroup->toString() + "\n");
    }
    
    return info;
}

/**
 * @return The number of data file type groups.
 */
int32_t 
SpecFile::getNumberOfDataFileTypeGroups() const
{
    return this->dataFileTypeGroups.size();
}

/**
 * Get the data file type group for the given index.
 * @param dataFileTypeGroupIndex
 *    Index of data file type group.
 * @return Data file type group at given index.
 */
SpecFileDataFileTypeGroup* 
SpecFile::getDataFileTypeGroupByIndex(const int32_t dataFileTypeGroupIndex)
{
    CaretAssertVectorIndex(this->dataFileTypeGroups, dataFileTypeGroupIndex);
    return this->dataFileTypeGroups[dataFileTypeGroupIndex];
}

/**
 * Get the data file type group for the given index.
 * @param dataFileTypeGroupIndex
 *    Index of data file type group.
 * @return Data file type group at given index.
 */
const SpecFileDataFileTypeGroup* 
SpecFile::getDataFileTypeGroupByIndex(const int32_t dataFileTypeGroupIndex) const
{
    CaretAssertVectorIndex(this->dataFileTypeGroups, dataFileTypeGroupIndex);
    return this->dataFileTypeGroups[dataFileTypeGroupIndex];
}

/**
 * Get the data file type group for the given data file type.
 * @param dataFileType
 *    Data file type requested.
 * @return Data file type group for requested data file type or 
 *    NULL if no matching item found.
 */
SpecFileDataFileTypeGroup* 
SpecFile::getDataFileTypeGroupByType(const DataFileTypeEnum::Enum dataFileType) const
{
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
     iter != dataFileTypeGroups.end();
     iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        if (dataFileTypeGroup->getDataFileType() == dataFileType) {
            return dataFileTypeGroup;
        }
    }
    
    return NULL;
}

/**
 * Set all file's selection status for loading.
 * @param selected
 *    New selection status for loading.
 */   
void 
SpecFile::setAllFilesSelectedForLoading(bool selectionStatus)
{
    for (std::vector<SpecFileDataFileTypeGroup*>::iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        dataFileTypeGroup->setAllFilesSelectedForLoading(selectionStatus);
    }    
}

/**
 * Set all file's selection status for saving.
 * @param selected
 *    New selection status for saving.
 */
void
SpecFile::setAllFilesSelectedForSaving(bool selectionStatus)
{
    for (std::vector<SpecFileDataFileTypeGroup*>::iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        dataFileTypeGroup->setAllFilesSelectedForSaving(selectionStatus);
    }
}

/**
 * Set all scene files selected and all other files not selected.
 */
void 
SpecFile::setAllSceneFilesSelectedForLoadingAndAllOtherFilesNotSelected()
{
    for (std::vector<SpecFileDataFileTypeGroup*>::iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        const bool selectionStatus = (dataFileTypeGroup->getDataFileType() == DataFileTypeEnum::SCENE);
        dataFileTypeGroup->setAllFilesSelectedForLoading(selectionStatus);
    }    
}

/**
 * Set the save status to on for any files that are modified.
 */
void
SpecFile::setModifiedFilesSelectedForSaving()
{
    for (std::vector<SpecFileDataFileTypeGroup*>::iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        dataFileTypeGroup->setModifiedFilesSelectedForSaving();
    }    
}

/**
 * Get the file information for the file with the given name.
 * 
 * @param fileName
 *     Name of the data file.
 * @return
 *     File information for the file or NULL if not found.
 */

const SpecFileDataFile*
SpecFile::getFileInfoFromFileName(const AString& fileName) const
{
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
        for (int32_t i = 0; i < numFiles; i++) {
            SpecFileDataFile* dataFileInfo = dataFileTypeGroup->getFileInformation(i);
            if (fileName == dataFileInfo->getFileName()) {
                return dataFileInfo;
            }
        }
    }
    
    return NULL;
}

/**
 * Transfer the "in spec" status for all data files from the
 * given spec file to this spec file.  Files are matched by
 * data file type and absolute path file name.
 *
 * The spec file names MUST match, else no action is taken.
 *
 * @param specFile
 *     Spec file from which in spec statuses are copied.
 */
void
SpecFile::transferDataFilesInSpecStatus(const SpecFile& specFile)
{
    if (getFileName() != specFile.getFileName()) {
        return;
    }
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
        for (int32_t i = 0; i < numFiles; i++) {
            SpecFileDataFile* dataFileInfo = dataFileTypeGroup->getFileInformation(i);
            const AString fileName = dataFileInfo->getFileName();
            FileInformation fileInfo(fileName);
            if (fileInfo.exists()
                && (fileInfo.isAbsolute())) {
                const SpecFileDataFile* copyFromDataFileInfo = specFile.getFileInfoFromFileName(fileName);
                if (copyFromDataFileInfo != NULL) {
                    dataFileInfo->setSpecFileMember(copyFromDataFileInfo->isSpecFileMember());
                }
            }
        }
    }
}

/**
 * @return The version of the file as a number.
 */
float 
SpecFile::getFileVersion()
{
    return SpecFile::specFileVersion;
}

/**
 * @return The version of the file as a string.
 */
AString 
SpecFile::getFileVersionAsString()
{
    return AString::number(SpecFile::specFileVersion, 'f', 1);
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of the class' instance.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
SpecFile::saveToScene(const SceneAttributes* sceneAttributes,
                      const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "SpecFile",
                                            1);
    
    AString specFileNameForScene;
    if (sceneAttributes->isSpecFileNameSavedToScene()) {
        specFileNameForScene = getFileName();
    }
    
    const bool allLoadedFilesFlag = sceneAttributes->isAllLoadedFilesSavedToScene();
    

    
    std::set<const CaretDataFile*> displayedDataFiles;
    if ( ! allLoadedFilesFlag) {
        const std::vector<int32_t> tabIndicesForScene = sceneAttributes->getIndicesOfTabsForSavingToScene();
        const std::vector<int32_t> windowIndicesForScene = sceneAttributes->getIndicesOfWindowsForSavingToScene();
        EventGetDisplayedDataFiles displayedFilesEvent(windowIndicesForScene,
                                                       tabIndicesForScene);
        EventManager::get()->sendEvent(displayedFilesEvent.getPointer());
        displayedDataFiles = displayedFilesEvent.getDisplayedDataFiles();        
    }
    
    sceneClass->addPathName("specFileName",
                            specFileNameForScene);
    
    std::vector<SceneClass*> dataFileClasses;
    
    //
    // Write files (except Scene and Palette files)
    //
    const int32_t numGroups = this->getNumberOfDataFileTypeGroups();
    for (int32_t i = 0; i < numGroups; i++) {
        SpecFileDataFileTypeGroup* group = this->getDataFileTypeGroupByIndex(i);
        const DataFileTypeEnum::Enum dataFileType = group->getDataFileType();
        if (dataFileType == DataFileTypeEnum::SCENE) {
            //CaretLogInfo("Note: Scene files not added to scene at this time");
        }
        else if (dataFileType == DataFileTypeEnum::PALETTE) {
            CaretLogInfo("Note: Palette files not added to scene at this time");
        }
        else {
            const int32_t numFiles = group->getNumberOfFiles();
            for (int32_t j = 0; j < numFiles; j++) {
                SpecFileDataFile* specFileDataFile = group->getFileInformation(j);
                
                bool addFileToSceneFlag = false;
                
                
                if (sceneAttributes->isFilenameForceWriteToScene(specFileDataFile->getFileName())) {
                    /*
                     * The scene file update command adds file to the scene that are not loaded
                     * so we must force adding of the filename to the scene's spec file
                     */
                    addFileToSceneFlag = true;
                }
                else {
                    /*
                     * Only write files that are loaded (indicated by its
                     * "caretDataFile" not NULL.
                     */
                    const CaretDataFile* caretDataFile = specFileDataFile->getCaretDataFile();
                    if (caretDataFile != NULL) {
                        if (allLoadedFilesFlag) {
                            addFileToSceneFlag = true;
                        }
                        else {
                            if (displayedDataFiles.find(caretDataFile) != displayedDataFiles.end()) {
                                addFileToSceneFlag = true;
                            }
                        }
                    }
                }

                if (addFileToSceneFlag) {
                    SceneClass* fileClass = new SceneClass("specFileDataFile",
                                                           "SpecFileDataFile",
                                                           1);
                    fileClass->addEnumeratedType<DataFileTypeEnum, DataFileTypeEnum::Enum>("dataFileType",
                                                                                           dataFileType);
                    fileClass->addEnumeratedType<StructureEnum, StructureEnum::Enum>("structure",
                                                                                     specFileDataFile->getStructure());
                    const AString name = updateFileNameAndPathForWriting(specFileDataFile->getFileName());
                    fileClass->addPathName("fileName",
                                           specFileDataFile->getFileName());
                    fileClass->addBoolean("selected",
                                          specFileDataFile->isLoadingSelected());
                    
                    dataFileClasses.push_back(fileClass);
                }
            }
        }
    }

    sceneClass->addChild(new SceneClassArray("dataFilesArray",
                                             dataFileClasses));
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 * 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously 
 *     saved and should be restored.
 */
void 
SpecFile::restoreFromScene(const SceneAttributes* sceneAttributes,
                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    this->clear();
    
    const AString specFileName = sceneClass->getPathNameValue("specFileName",
                                                              "");
    this->setFileName(specFileName);
    
    /*
     * If spec file name is path to a valid file,
     * load the spec file and then deselect all 
     * of the files in the spec file.  Since the
     * scene may contain a subset of the files in
     * the spec file, not doing this would result
     * in the spec file missing data file if the
     * user saves files after loading the scene.
     */
    if (specFileName.isEmpty() == false) {
        FileInformation specFileInfo(specFileName);
        if (specFileInfo.exists()) {
            try {
                readFile(specFileName);
            }
            catch (const DataFileException& e) {
                sceneAttributes->addToErrorMessage("Error reading spec file "
                                                   + specFileName
                                                   + " for displaying scene: "
                                                   + e.whatString());
            }
            setAllFilesSelectedForLoading(false);
            setAllFilesSelectedForSaving(false);
        }
    }
    
    const SceneClassArray* dataFileClassArray = sceneClass->getClassArray("dataFilesArray");
    if (dataFileClassArray != NULL) {
        const int32_t numberOfFiles = dataFileClassArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numberOfFiles; i++) {
            const SceneClass* dataFileClass = dataFileClassArray->getClassAtIndex(i);
            const bool selectedForLoading = dataFileClass->getBooleanValue("selected");
            const AString dataFileName = dataFileClass->getPathNameValue("fileName");
            const DataFileTypeEnum::Enum dataFileType = dataFileClass->getEnumeratedTypeValue<DataFileTypeEnum, 
                                                                                              DataFileTypeEnum::Enum>("dataFileType",
                                                                                                                      DataFileTypeEnum::UNKNOWN);
            const StructureEnum::Enum structure = dataFileClass->getEnumeratedTypeValue<StructureEnum, 
            StructureEnum::Enum>("structure",
                                    StructureEnum::INVALID);
            
            this->addDataFile(dataFileType, 
                              structure, 
                              dataFileName, 
                              selectedForLoading,
                              false, // not selected for saving
                              true);
        }
    }
}

/**
 * Append content of a spec file to this spec file.
 * @param toAppend
 *    Spec file that is appended.
 */
void
SpecFile::appendSpecFile(const SpecFile& toAppend)
{
    int numOtherGroups = (int)toAppend.dataFileTypeGroups.size();
    AString otherDirectory = FileInformation(toAppend.getFileName()).getAbsolutePath();//hopefully the filename is already absolute, if it isn't and we changed directory, we can't recover the correct path
    if (!otherDirectory.endsWith('/'))//deal with the root directory
    {
        otherDirectory += "/";
    }
    for (int i = 0; i < numOtherGroups; ++i)
    {
        const SpecFileDataFileTypeGroup* thisGroup = toAppend.dataFileTypeGroups[i];
        int numOtherFiles = thisGroup->getNumberOfFiles();
        for (int j = 0; j < numOtherFiles; ++j)
        {
            const SpecFileDataFile* fileData = thisGroup->getFileInformation(j);
            AString fileName = fileData->getFileName();
            FileInformation fileInfo(fileName);//do not trust exists, we don't have the right working directory, this is ONLY to check whether the path is absolute
            if (fileInfo.isRelative())
            {
                fileName = otherDirectory + fileName;//don't trust the file to exist from current directory, use string manipulation only
            }
            addDataFile(thisGroup->getDataFileType(),
                        fileData->getStructure(),
                        fileName,
                        fileData->isLoadingSelected(),
                        fileData->isSavingSelected(),
                        fileData->isSpecFileMember());//absolute paths should get converted to relative on writing
        }
    }
}

/**
 * Set this file modified.
 */
void
SpecFile::setModified()
{
    CaretDataFile::setModified();
}

/**
 * @return true if this file has been modified.
 */
bool
SpecFile::isModified() const
{
    if (CaretDataFile::isModified()) {
        return true;
    }
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        const SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        if (dataFileTypeGroup->isModified()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Clear the modification status.
 */
void
SpecFile::clearModified()
{
    CaretDataFile::clearModified();

    for (std::vector<SpecFileDataFileTypeGroup*>::iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        dataFileTypeGroup->clearModified();
    }
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
SpecFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
    
    const int32_t numberOfFiles = getNumberOfFiles();
    if (numberOfFiles <= 0) {
        return;
    }
    
    int32_t columnCounter = 0;
    const int32_t COL_TYPE      = columnCounter++;
    const int32_t COL_STRUCTURE = columnCounter++;
    const int32_t COL_NAME      = columnCounter++;
    const int32_t COL_PATH      = columnCounter++;
    
    StringTableModel table((numberOfFiles + 1),
                           columnCounter);
    
    int32_t rowIndex = 0;
    table.setElement(rowIndex,
                     COL_TYPE,
                     "TYPE");
    table.setElement(rowIndex,
                     COL_STRUCTURE,
                     "STRUCTURE");
    table.setElement(rowIndex,
                     COL_NAME,
                     "NAME");
    table.setElement(rowIndex,
                     COL_PATH,
                     "PATH");
    
    rowIndex++;
    
    table.setColumnAlignment(COL_TYPE, StringTableModel::ALIGN_LEFT);
    table.setColumnAlignment(COL_STRUCTURE, StringTableModel::ALIGN_LEFT);
    table.setColumnAlignment(COL_NAME, StringTableModel::ALIGN_LEFT);
    table.setColumnAlignment(COL_PATH, StringTableModel::ALIGN_LEFT);
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        const DataFileTypeEnum::Enum dataFileType = dataFileTypeGroup->getDataFileType();
        
        const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
        for (int32_t i = 0; i < numFiles; i++) {
            SpecFileDataFile* sfdf = dataFileTypeGroup->getFileInformation(i);
            FileInformation fileInfo(sfdf->getFileName());
            
            table.setElement(rowIndex,
                             COL_TYPE,
                             DataFileTypeEnum::toGuiName(dataFileType));
            table.setElement(rowIndex,
                             COL_STRUCTURE,
                             StructureEnum::toGuiName(sfdf->getStructure()));
            table.setElement(rowIndex,
                             COL_NAME,
                             fileInfo.getFileName());
            table.setElement(rowIndex,
                             COL_PATH,
                             fileInfo.getPathName());
            
            rowIndex++;
        }
    }
    
    dataFileInformation.addText("\n"
                                + table.getInString());
}

/**
 * Is the given file type allowed in the spec file?
 *
 * @param dataFileType
 *     Type of data file.
 * @return
 *     True if allowed in spec file, else false.
 */
bool
SpecFile::isDataFileTypeAllowedInSpecFile(const DataFileTypeEnum::Enum dataFileType)
{
    bool allowedFlag(true);
    
    switch (dataFileType) {
        case DataFileTypeEnum::ANNOTATION:
            break;
        case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
            break;
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            allowedFlag = false;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            break;
        case DataFileTypeEnum::CZI_IMAGE_FILE:
            break;
        case DataFileTypeEnum::FOCI:
            break;
        case DataFileTypeEnum::IMAGE:
            break;
        case DataFileTypeEnum::LABEL:
            break;
        case DataFileTypeEnum::METRIC:
            break;
        case DataFileTypeEnum::METRIC_DYNAMIC:
            allowedFlag = false;
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
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
            break;
        case DataFileTypeEnum::VOLUME_DYNAMIC:
            allowedFlag = false;
            break;
    }
    
    return allowedFlag;
}



