/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

//#ifdef CARET_OS_MACOSX
//#define HAVE_STD_FILESYSTEM_LIBRARY 1
//#endif

#ifdef HAVE_STD_FILESYSTEM_LIBRARY
#include <filesystem>
#else
#include <QFile>
#endif

#include <map>
#include <memory>
#include <set>
#include <vector>

#include <QDir>

#include "CaretLogger.h"
#include "FileInformation.h"
#include "OperationException.h"
#include "OperationSceneFileRestructure.h"
#include "OperationShowSceneTwo.h"
#include "Scene.h"
#include "SceneFile.h"
#include "ScenePathName.h"
#include "SpecFile.h"
#include "SpecFileDataFile.h"
#include "SpecFileDataFileTypeGroup.h"

using namespace caret;
using namespace std;

AString OperationSceneFileRestructure::getCommandSwitch()
{
    return "-scene-file-restructure";
}

AString OperationSceneFileRestructure::getShortDescription()
{
    return "SCENE FILE AND ITS DATA FILES RESTRUCTURE";
}

OperationParameters* OperationSceneFileRestructure::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "input-scene", "Path to the input scene file");
    
    ret->addStringParameter(2, "output-directory", "Directory where the restructured scene and data files will be placed");
    
    ret->createOptionalParameter(3, "-preview", "Lists the files that would be created without performing any copying");
    
    ret->createOptionalParameter(4, "-skip-missing-files", "Issues warnings for missing or network files instead of fatal errors");
    
    ret->createOptionalParameter(5, "-test-scenes", "After copying, tests loading of files for each scene in the output scene file");
    
    OptionalParameter* overwriteOpt = ret->createOptionalParameter(6, "-overwrite-files-mode", "Controls behavior when files already exist in the output directory");
    overwriteOpt->addStringParameter(1, "overwrite-files-mode", "the overwrite files mode (ERROR, YES, or SKIP)");

    ret->setHelpText(getInstructionsInPlainText());
    
    return ret;
}

void OperationSceneFileRestructure::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SceneFile myScene;
    const AString inputSceneFileName(myParams->getString(1));
    const AString outputDirectoryName(myParams->getString(2));
    const bool previewModeFlag(myParams->getOptionalParameter(3)->m_present);
    const bool skipMissingFilesFlag(myParams->getOptionalParameter(4)->m_present);
    const bool testScenesFlag(myParams->getOptionalParameter(5)->m_present);

    OverwriteFilesMode overwriteFilesMode(OverwriteFilesMode::OVERWRITE_ERROR);
    OptionalParameter* overwriteOpt = myParams->getOptionalParameter(6);
    if (overwriteOpt->m_present) {
        const AString overwriteModeName(overwriteOpt->getString(1));
        if (overwriteModeName == "ERROR") {
            overwriteFilesMode = OverwriteFilesMode::OVERWRITE_ERROR;
        }
        else if (overwriteModeName == "YES") {
            overwriteFilesMode = OverwriteFilesMode::OVERWRITE_YES;
        }
        else if (overwriteModeName == "SKIP") {
            overwriteFilesMode = OverwriteFilesMode::OVERWRITE_SKIP;
        }
        else {
            throw OperationException("Unrecognized values for \"-overwrite-files\" option: "
                                     + overwriteModeName);
        }
    }
    else {
        overwriteFilesMode = OverwriteFilesMode::OVERWRITE_ERROR;
    }

    AString messagesDummy;
    copySceneFileAndDataFiles(inputSceneFileName,
                              outputDirectoryName,
                              MessageMode::COMMAND_LINE,
                              overwriteFilesMode,
                              previewModeFlag,
                              skipMissingFilesFlag,
                              testScenesFlag,
                              messagesDummy);
}

/**
 * Copy the scene file and its data files to a new location
 * @param inputSceneFileName
 *    The input scene file name
 * @param outputDirectoryName
 *    Output directory to which files are written
 * @param messageMode
 *    Mode for messages
 * @param overwriteFilesMode
 *    The mode for overwriting existing files
 * @param previewModeFlag
 *    If true do not copy any files but list the file operations that would have been performed
 * @param skipMissingFilesFlag
 *    Missing files cause a warning, not an error
 * @param testScenesFlag
 *    If true, test loading of files in each scene
 * @param messagesOut
 *    Contains messages if message mode is GUI
 * @throws OperationException
 */
void
OperationSceneFileRestructure::copySceneFileAndDataFiles(const AString& inputSceneFileName,
                                                         const AString& outputDirectoryNameIn,
                                                         const MessageMode messageMode,
                                                         const OverwriteFilesMode overwriteFilesMode,
                                                         const bool previewModeFlag,
                                                         const bool skipMissingFilesFlag,
                                                         const bool testScenesFlag,
                                                         AString& messagesOut)
{
    messagesOut.clear();
    
    const bool executeModeFlag( ! previewModeFlag);
    const AString previewText(previewModeFlag
                              ? "preview "
                              : "");
    
    /*
     * Validate output directory and get output directory's
     * absolute path.
     */
    const FileInformation outputDirInfo(outputDirectoryNameIn);
    if ( ! outputDirInfo.exists()) {
        throw OperationException("Output directory does not exist: "
                                 + outputDirectoryNameIn);
    }
    if ( ! outputDirInfo.isDirectory()) {
        throw OperationException("Output directory is not a directory: "
                                 + outputDirectoryNameIn);
    }
    QDir outputDir(outputDirectoryNameIn);
    CaretAssert(outputDir.exists());
    const AString outputDirectoryName(outputDir.absolutePath());
    const AString outputDirMessage("..."
                                   + previewText
                                   + "Absolute path of output directory: "
                                   + outputDirectoryName);
    switch (messageMode) {
        case MessageMode::GUI:
            messagesOut.appendWithNewLine(outputDirMessage);
            break;
        case MessageMode::COMMAND_LINE:
            std::cout << outputDirMessage << std::endl;
            break;
    }

    /*
     * Read scene file
     */
    std::unique_ptr<SceneFile> sceneFile;
    try {
        sceneFile.reset(new SceneFile());
        CaretAssert(sceneFile.get());
        sceneFile->readFile(inputSceneFileName);
    }
    catch (const DataFileException& dfe) {
        throw OperationException(dfe.whatString());
    }
    
    AString dataFileErrors;

    /*
     * Name of directory for data files
     */
    const AString dataFilesRelativeSubdirectoryName("data_files");
    const AString dataFilesDirectoryName(outputDirectoryName
                                         + "/"
                                         + dataFilesRelativeSubdirectoryName);
    const FileInformation dataFilesDirInfo(dataFilesDirectoryName);
    bool createDataFilesDirectoryFlag(true);
    if (dataFilesDirInfo.exists()) {
        switch (overwriteFilesMode) {
            case OverwriteFilesMode::OVERWRITE_YES:
                createDataFilesDirectoryFlag = false; /* directory already exists */
                break;
            case OverwriteFilesMode::OVERWRITE_ERROR:
                dataFileErrors.appendWithNewLine("Output data files directory exists: "
                                                 + dataFilesDirectoryName);
                break;
            case OverwriteFilesMode::OVERWRITE_SKIP:
                createDataFilesDirectoryFlag = false; /* directory already exists */
                break;
        }
    }
    
    /*
     * Get names of all data files and verify that they exist
     */
    const std::vector<SceneDataFileInfo> sceneDataFileInfo(sceneFile->getAllDataFileInfoFromAllScenes());
    std::vector<AString> allDataFileNames; /* use 'set' as files may be in more than one scene */
    for (const SceneDataFileInfo& sdfi : sceneDataFileInfo) {
        const AString dataFileName(sdfi.getAbsolutePathAndFileName());
        FileInformation dataFileInfo(dataFileName);
        if (dataFileInfo.isRemoteFile()) {
            if (skipMissingFilesFlag) {
                const AString skipMessage("...skipping file on network: "
                                          + dataFileName);
                switch (messageMode) {
                    case MessageMode::GUI:
                        messagesOut.appendWithNewLine(skipMessage);
                        break;
                    case MessageMode::COMMAND_LINE:
                        std::cout << skipMessage << std::endl;
                        break;
                }
            }
            else {
                dataFileErrors.appendWithNewLine("File on network not supported: "
                                                 + dataFileName);
            }
        }
        else if (dataFileInfo.exists()) {
            if (dataFileInfo.isFile()) {
                if (dataFileInfo.isReadable()) {
                    allDataFileNames.push_back(dataFileName);
                    
                    std::vector<AString> childDataFiles(getChildDataFiles(dataFileName));
                    allDataFileNames.insert(allDataFileNames.end(),
                                            childDataFiles.begin(),
                                            childDataFiles.end());
                }
                else {
                    dataFileErrors.appendWithNewLine("Does not have read permission: "
                                                     + dataFileName);
                }
            }
            else {
                dataFileErrors.appendWithNewLine("Is not a file: "
                                                 + dataFileName);
            }
        }
        else {
            if (skipMissingFilesFlag) {
                const AString skipMessage("...skipping missing file: "
                                          + dataFileName);
                switch (messageMode) {
                    case MessageMode::GUI:
                        messagesOut.appendWithNewLine(skipMessage);
                        break;
                    case MessageMode::COMMAND_LINE:
                        std::cout << skipMessage << std::endl;
                        break;
                }
            }
            else {
                dataFileErrors.appendWithNewLine("Does not exist: "
                                                 + dataFileName);
            }
        }
    }
    
    /*
     * Detect files with same name but different paths
     */
    std::map<AString, AString> nameAndPath;
    for (const auto& name : allDataFileNames) {
        const FileInformation fileInfo(name);
        AString filename(fileInfo.getFileName());
        AString pathname(fileInfo.getAbsolutePath());
        const auto iter(nameAndPath.find(filename));
        if (iter != nameAndPath.end()) {
            dataFileErrors.appendWithNewLine("Files with same name but different paths is not supported:"
                                             "\n   " + filename
                                             + "\n      " + pathname
                                             + "\n      " + iter->second);
        }
        else {
            nameAndPath.insert(std::make_pair(filename,
                                              pathname));
        }
    }
    
    /*
     * Create output file names and verify that they do not exist
     */
    std::vector<std::pair<AString, AString>> dataFileFromToFileNames;
    for (const AString& dataFileName : allDataFileNames) {
        const FileInformation fileInfo(dataFileName);
        const AString outputDataFileName(dataFilesDirectoryName
                                         + "/"
                                         + fileInfo.getFileName());
        FileInformation outputFileInfo(outputDataFileName);
        bool copyFileFlag(true);
        if (outputFileInfo.exists()) {
            switch (overwriteFilesMode) {
                case OverwriteFilesMode::OVERWRITE_YES:
                    copyFileFlag = true;
                    break;
                case OverwriteFilesMode::OVERWRITE_ERROR:
                    copyFileFlag = false;
                    dataFileErrors.appendWithNewLine("Output file exists: "
                                                     + outputDataFileName);
                    break;
                case OverwriteFilesMode::OVERWRITE_SKIP:
                    copyFileFlag = false;
                    break;
            }
        }
        if (copyFileFlag) {
            dataFileFromToFileNames.emplace_back(dataFileName,
                                                 outputDataFileName);
        }
    }
    
    if ( ! dataFileErrors.isEmpty()) {
        throw OperationException(dataFileErrors);
    }
    
    /*
     * Create directory for data files
     */
    if (createDataFilesDirectoryFlag) {
        const AString createDirMsg("..."
                                   + previewText
                                   + "creating data file directory: "
                                   + dataFilesDirectoryName);
        switch (messageMode) {
            case MessageMode::GUI:
                messagesOut.appendWithNewLine(createDirMsg);
                break;
            case MessageMode::COMMAND_LINE:
                std::cout << createDirMsg << std::endl;
                break;
        }
        if (executeModeFlag) {
            if ( ! outputDir.mkdir(dataFilesDirectoryName)) {
                throw OperationException("Failed to create data files subdirectory: "
                                         + dataFilesDirectoryName);
            }
        }
    }
    
    if ( ! previewModeFlag) {
        CaretAssert(FileInformation(dataFilesDirectoryName).exists());
    }
    
    /*
     * Copy the data files to their new locations
     */
    bool allowOverwriteFilesFlag(false);
    switch (overwriteFilesMode) {
        case OverwriteFilesMode::OVERWRITE_YES:
            allowOverwriteFilesFlag = true;
            break;
        case OverwriteFilesMode::OVERWRITE_ERROR:
            break;
        case OverwriteFilesMode::OVERWRITE_SKIP:
            break;
    }
    copyDataFiles(dataFileFromToFileNames,
                  previewText,
                  executeModeFlag,
                  allowOverwriteFilesFlag,
                  messageMode,
                  messagesOut);
    
    /*
     * Update paths of all data files
     */
    updateDataFilePaths(sceneFile.get(),
                        dataFilesDirectoryName,
                        executeModeFlag);
    
    /*
     * New path for scene file
     */
    const AString outputSceneFileName(outputDirectoryName
                                      + "/"
                                      + sceneFile->getFileNameNoPath());
    /*
    * there are any spec file on disk used by the scene, update their paths
     * Test loading of scenes
     * Need to handle spec file
     * overwrite mode
     * ignore missing files
     * ignore network files
     * meta files (metavolume, histology slices)
     */
    
    /*
     * Write scene file
     */
    const AString writeSceneMsg("\n..."
                                + previewText
                                + "writing scene file: "
                                + outputSceneFileName);
    switch (messageMode) {
        case MessageMode::GUI:
            messagesOut.appendWithNewLine(writeSceneMsg);
            break;
        case MessageMode::COMMAND_LINE:
            std::cout << writeSceneMsg << std::endl;
            break;
    }
    if (executeModeFlag) {
        try {
            sceneFile->writeFile(outputSceneFileName);
            
            if (testScenesFlag) {
                const int32_t numberOfScenes(sceneFile->getNumberOfScenes());
                if (numberOfScenes > 0) {
                    testScenes(outputSceneFileName,
                               numberOfScenes,
                               messageMode,
                               messagesOut);
                }
            }
            
        }
        catch (const DataFileException& dfe) {
            throw OperationException(dfe.whatString());
        }
    }
    
    if (previewModeFlag) {
        const AString previewMsg("...Preview Mode Complete");
        switch (messageMode) {
            case MessageMode::GUI:
                messagesOut.appendWithNewLine(previewMsg);
                break;
            case MessageMode::COMMAND_LINE:
                std::cout << previewMsg << std::endl;
                break;
        }
    }
}

/**
 * Copy the data files
 * @param dataFileFromToFileNames
 *    Pairs of old file path name and new file path name
 * @param previewText
 *    Preview text if in preview mode
 * @param executeModeFlag
 *    If true, do not make any changes
 * @param allowOverwriteFilesFlag
 *    If true, allow overwriting of existing files
 * @param messageMode
 *    Mode for messages
 * @param messagesOut
 *    Contains messages if message mode is GUI
 */
void
OperationSceneFileRestructure::copyDataFiles(const std::vector<std::pair<AString, AString>> dataFileFromToFileNames,
                                         const AString& previewText,
                                         const bool executeModeFlag,
                                         const bool allowOverwriteFilesFlag,
                                         const MessageMode messageMode,
                                         AString& messagesOut)
{
    for (const auto& fromToNames : dataFileFromToFileNames) {
        const AString& fromFileName(fromToNames.first);
        const AString& toFileName(fromToNames.second);
        FileInformation fromFileInfo(fromFileName);
        const int64_t fileSize(fromFileInfo.size());
        const AString sizeString(FileInformation::fileSizeToStandardUnits(fileSize));
        const AString copyMsg("\n..." + previewText + "copying: " + sizeString + " from " + fromFileName
                              + "\n      to: " + toFileName);
        switch (messageMode) {
            case MessageMode::GUI:
                messagesOut.appendWithNewLine(copyMsg);
                break;
            case MessageMode::COMMAND_LINE:
                std::cout << copyMsg << std::endl;
                break;
        }
        if (executeModeFlag) {
            try {
                if (copySpecialFileTypes(fromFileName,
                                         toFileName)) {
                    /* file was copied */
                }
                else {
#ifdef HAVE_STD_FILESYSTEM_LIBRARY
                    QFileInfo fromFileInfo(fromFileName);
                    QFileInfo toFileInfo(toFileName);
                    try {
                        if (allowOverwriteFilesFlag) {
                            std::filesystem::copy(fromFileInfo.filesystemAbsoluteFilePath(),
                                                  toFileInfo.filesystemAbsoluteFilePath(),
                                                  std::filesystem::copy_options::overwrite_existing);
                        }
                        else {
                            /* Will throw exception of "to" file exists */
                            std::filesystem::copy(fromFileInfo.filesystemAbsoluteFilePath(),
                                                  toFileInfo.filesystemAbsoluteFilePath(),
                                                  std::filesystem::copy_options::none);
                        }
                    }
                    catch (const std::filesystem::filesystem_error& e) {
                        throw DataFileException("Failed to copy \""
                                                + fromFileName
                                                + "\" to \""
                                                + toFileName
                                                + "\": "
                                                + AString(e.what()));
                    }
#else // HAVE_STD_FILESYSTEM_LIBRARY
                    if (QFileInfo::exists(toFileName)) {
                        if (allowOverwriteFilesFlag) {
                            /*
                             * QFile::copy() will not overwrite the target filename
                             * so we need to remove the target file
                             */
                            const bool successFlag(QFile::remove(toFileName));
                            if ( ! successFlag) {
                                throw DataFileException("Unable to remove existing output file prior to overwriting it: "
                                                        + toFileName);
                            }
                        }
                        else {
                            throw DataFileException("Overwriting of existing files disabled for "
                                                    + toFileName);
                        }
                    }
                    const bool successFlag(QFile::copy(fromFileName,
                                                       toFileName));
                    if ( ! successFlag) {
                        throw DataFileException("Failed to copy \""
                                                + fromFileName
                                                + "\" to \""
                                                + toFileName
                                                + "\"");
                    }
#endif // HAVE_STD_FILESYSTEM_LIBRARY
                }
            }
            catch (const DataFileException& dfe) {
                throw OperationException(dfe.whatString());
            }
        }
    }
}

/**
 * Some file types need special handling when they are copied
 * @param fromFileName
 *    The 'from' file name
 * @param toFileName
 *    The 'to' file name
 */
bool
OperationSceneFileRestructure::copySpecialFileTypes(const AString& fromFileName,
                                                    const AString& toFileName)
{
    bool validFlag(false);
    const DataFileTypeEnum::Enum dataFileType(DataFileTypeEnum::fromFileExtension(fromFileName,
                                                                                  &validFlag));
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
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_MAPS:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
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
            break;
        case DataFileTypeEnum::META_VOLUME:
            break;
        case DataFileTypeEnum::METRIC:
            break;
        case DataFileTypeEnum::METRIC_DYNAMIC:
            break;
        case DataFileTypeEnum::OME_ZARR_IMAGE_FILE:
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
            break;
        case DataFileTypeEnum::SAMPLES:
            break;
        case DataFileTypeEnum::SCENE:
            break;
        case DataFileTypeEnum::SPECIFICATION:
        {
            /*
             * Spec file is in same directory as data files
             * so need to update file locations within the
             * spec file.
             */
            SpecFile sf;
            sf.readFile(fromFileName);
            const int32_t numGroups(sf.getNumberOfDataFileTypeGroups());
            for (int32_t iGroup = 0; iGroup < numGroups; iGroup++) {
                SpecFileDataFileTypeGroup* group(sf.getDataFileTypeGroupByIndex(iGroup));
                const int32_t numFiles(group->getNumberOfFiles());
                for (int32_t iFile = 0; iFile < numFiles; iFile++) {
                    SpecFileDataFile* file(group->getFileInformation(iFile));
                    const AString filename(file->getFileName());
                    FileInformation fileInfo(filename);
                    file->setFileName(fileInfo.getFileName());
                }
            }
            sf.writeFile(toFileName);
        }
            break;
        case DataFileTypeEnum::SURFACE:
            break;
        case DataFileTypeEnum::UNKNOWN:
            break;
        case DataFileTypeEnum::VOLUME:
            break;
        case DataFileTypeEnum::VOLUME_DYNAMIC:
            break;
    }

    return false;
}


/**
 * Update the paths of all data files in all scenes in the given scene file
 * @param sceneFile
 *    The scene file
 * @param dataFilesDirectoryName
 *    New location of all data files
 * @param executeModeFlag
 *    If true, do not make any changes
 */
void
OperationSceneFileRestructure::updateDataFilePaths(SceneFile* sceneFile,
                                                   const AString& dataFilesDirectoryName,
                                                   const bool executeModeFlag)
{
    CaretAssert(sceneFile);
    
    const int32_t numScenes(sceneFile->getNumberOfScenes());
    for (int32_t i = 0; i < numScenes; i++) {
        Scene* scene(sceneFile->getSceneAtIndex(i));
        CaretAssert(scene);
        std::vector<ScenePathName*> allScenePathNames(scene->getAllScenePathNameDescendants());
        for (ScenePathName* spn : allScenePathNames) {
            const AString& dataFileName(spn->stringValue().trimmed());
            if ( ! dataFileName.isEmpty()) {
                FileInformation fileInfo(dataFileName);
                const AString filename(fileInfo.getFileName());
                if ( ! filename.isEmpty()) {
                    const AString relocatedFileName(dataFilesDirectoryName
                                                 + "/"
                                                 + filename);
                    
                    if (executeModeFlag) {
                        spn->setValue(relocatedFileName);
                    }
                }
            }
        }
    }
}

/**
 * Some data files may be a "file of files".  For these types of files get
 * all of the child data files.
 * @param dataFileName
 *    Name of data file
 * @return
 *    Names of child data files
 */
std::vector<AString>
OperationSceneFileRestructure::getChildDataFiles(const AString& dataFileName)
{
    std::vector<AString> childDataFileNames;
    
    bool validFlag(false);
    const DataFileTypeEnum::Enum dataFileType(DataFileTypeEnum::fromFileExtension(dataFileName, &validFlag));
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
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_MAPS:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            break;
        case DataFileTypeEnum::CZI_IMAGE_FILE:
            break;
        case DataFileTypeEnum::FOCI:
            break;
        case DataFileTypeEnum::HISTOLOGY_SLICES:
            throw OperationException("Program not finished: Histology Slices File needs to read its child volume files");
            break;
        case DataFileTypeEnum::IMAGE:
            break;
        case DataFileTypeEnum::LABEL:
            break;
        case DataFileTypeEnum::META_VOLUME:
            throw OperationException("Program not finished: Meta-Volume File needs to read its child volume files");
            break;
        case DataFileTypeEnum::METRIC:
            break;
        case DataFileTypeEnum::METRIC_DYNAMIC:
            break;
        case DataFileTypeEnum::OME_ZARR_IMAGE_FILE:
            throw OperationException("Program not finished: OME-ZARR File needs to read its child volume files");
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
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
            break;
        case DataFileTypeEnum::VOLUME_DYNAMIC:
            break;
    }

    return childDataFileNames;
}

/**
 * Test the scenes to see if files load successfully
 * @param sceneFileName
 *    Name of scene file
 * @param numberOfScenes
 *    Number of scenes in the scene file
 * @param messageMode
 *    Mode for messages
 * @param messagesOut
 *    Contains messages if message mode is GUI
 */
void
OperationSceneFileRestructure::testScenes(const AString& sceneFileName,
                                          const int32_t numberOfScenes,
                                          const MessageMode messageMode,
                                          AString& messagesOut)
{
    for (int32_t i = 0; i < numberOfScenes; i++) {
        const AString sceneNumberText(AString::number(i + 1));
        
        messagesOut.appendWithNewLine(" ");
        const AString testSceneMessage("...Testing scene "
                                       + sceneNumberText);
        switch (messageMode) {
            case MessageMode::GUI:
                messagesOut.appendWithNewLine(testSceneMessage);
                break;
            case MessageMode::COMMAND_LINE:
                std::cout << testSceneMessage << std::endl;
                break;
        }

        const bool doNotUseSceneColorsFlag(false);
        AString errorMessage;
        OperationShowSceneTwo::loadSceneFileAndRestoreScene(sceneFileName,
                                                            sceneNumberText,
                                                            doNotUseSceneColorsFlag,
                                                            errorMessage);
        if (errorMessage.isEmpty()) {
            errorMessage = ("...Files in scene "
                            + sceneNumberText
                            + " were successfully loaded.");
        }
        
        switch (messageMode) {
            case MessageMode::GUI:
                messagesOut.appendWithNewLine(errorMessage);
                break;
            case MessageMode::COMMAND_LINE:
                std::cout << errorMessage << std::endl;
                break;
        }
    }
}

/**
 * @return Instructions in Plain Text
 */
AString
OperationSceneFileRestructure::getInstructionsInPlainText()
{
    return AString::convertHtmlToPlainText(getInstructionsPrivate(MessageMode::COMMAND_LINE));
}

/**
 * @return Instructions in HTML
 */
AString
OperationSceneFileRestructure::getInstructionsInHtml()
{
    return getInstructionsPrivate(MessageMode::GUI);
}

/**
 * @return Instructions in HTML
 */
AString
OperationSceneFileRestructure::getInstructionsPrivate(const MessageMode messageMode)
{
    const AString dataFileTypesText("GIFTI External Binary Files; "
                                    + DataFileTypeEnum::toQFileDialogFilterForReading(DataFileTypeEnum::HISTOLOGY_SLICES)
                                    + "; "
                                    + DataFileTypeEnum::toQFileDialogFilterForReading(DataFileTypeEnum::META_VOLUME)
                                    + "; "
                                    + DataFileTypeEnum::toQFileDialogFilterForReading(DataFileTypeEnum::OME_ZARR_IMAGE_FILE));
    
    std::string overwriteModesText =
R"a(
Overwrite Modes:
<ul>
<li> YES: Overwrites existing files.
<li> ERROR: Fails if a file would be overwritten (default).
<li> SKIP: Skips copying if the file already exists.
</ul>
)a";
    
    std::string ts =
R"a(
<html><body>
Restructure a scene file and its associated data files by copying them into a new directory structure 
located in the specified output directory. The original scene file and data files are NOT modified.
<p>
After execution, the output directory will contain:
<ul>
<li>The restructured scene file.
<li>A subdirectory named data_files containing all associated data files.
</ul>
<p>
OVERWRITE_MODES
<p>
Notes:
<ul>
<li> For best results, ensure the output directory is empty before running this command.
<li> Missing files will cause a fatal error unless the skip missing files option is used.  Note that missing files may cause scenes to display incorrectly.
<li> Use the preview option to simulate the file copying operations and review the expected output (No output files are created).
<li> Some datasets may contain multiple scene files that share data files.  Run this command on each scene file and:
(1) Use the same output directory; 
 (2) Set the Overwrite Mode to SKIP to preserve shared files.
</ul>
<p>

Known Limitations:
<ul>
<li> Files with the same name in different directories are not supported.
<li> Unsupported file types: DATA_FILE_TYPES
<li> Remote files (e.g., hosted on web servers) are not supported.
</ul>

</body></html>
)a";
    
    AString textOut(AString::fromStdString(ts));
    textOut.replace("DATA_FILE_TYPES",
                    dataFileTypesText);
    switch (messageMode) {
        case MessageMode::GUI:
            textOut.replace("OVERWRITE_MODES",
                            "");
            break;
        case MessageMode::COMMAND_LINE:
            textOut.replace("OVERWRITE_MODES",
                            AString::fromStdString(overwriteModesText));
            break;
    }

    return textOut;
}
