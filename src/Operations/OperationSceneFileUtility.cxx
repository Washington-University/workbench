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

#include <memory>

#include <QDir>

#include "CaretLogger.h"
#include "CaretTemporaryFile.h"
#include "FileInformation.h"
#include "OperationSceneFileUtility.h"
#include "OperationException.h"
#include "Scene.h"
#include "SceneFile.h"
#include "ScenePathName.h"

using namespace caret;
using namespace std;

AString OperationSceneFileUtility::getCommandSwitch()
{
    return "-scene-file-utility";
}

AString OperationSceneFileUtility::getShortDescription()
{
    return "SCENE FILE AND ITS DATA FILES UTILITY";
}

OperationParameters* OperationSceneFileUtility::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "input-scene", "the scene file to use");
    
    ret->addStringParameter(2, "output-directory", "output directory for scene file and its data files");
    
    ret->createOptionalParameter(3, "-preview", "list files that would be created but DO NOT do any copying");
    
    ret->setHelpText("Copy a scene file and its data files to a directory.  The scene file will be in the output directory "
                     "and the data files will be in the subdirectory \"data_files\"");
    return ret;
}

void OperationSceneFileUtility::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SceneFile myScene;
    const AString inputSceneFileName(myParams->getString(1));
    const AString outputDirectoryName(myParams->getString(2));
    const bool previewModeFlag(myParams->getOptionalParameter(3)->m_present);
    
    copySceneFileAndDataFiles(inputSceneFileName,
                              outputDirectoryName,
                              previewModeFlag);
}

/**
 * Copy the scene file and its data files to a new location
 * @param inputSceneFileName
 * @param outputDirectoryName
 * @param previewModeFlag
 * @throws OperationException
 */
void
OperationSceneFileUtility::copySceneFileAndDataFiles(const AString& inputSceneFileName,
                                                     const AString& outputDirectoryNameIn,
                                                     const bool previewModeFlag)
{
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
        throw OperationException("Output directory \""
                                 + outputDirectoryNameIn
                                 + "\" does not exist");
    }
    if ( ! outputDirInfo.isDirectory()) {
        throw OperationException("Output directory \""
                                 + outputDirectoryNameIn
                                 + "\" is not a directory");
    }
    QDir outputDir(outputDirectoryNameIn);
    CaretAssert(outputDir.exists());
    const AString outputDirectoryName(outputDir.absolutePath());
    std::cout << "..." << previewText << "absolute path of output directory: " << outputDirectoryName << std::endl;

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
    
    /*
     * Name of directory for data files
     */
    const AString dataFilesRelativeSubdirectoryName("data_files");
    const AString dataFilesDirectoryName(outputDirectoryName
                                         + "/"
                                         + dataFilesRelativeSubdirectoryName);
    const FileInformation dataFilesDirInfo(dataFilesDirectoryName);
    if (dataFilesDirInfo.exists()) {
        throw OperationException("Data files directory \""
                                 + dataFilesDirectoryName
                                 + "\" exists");
    }
    
    /*
     * Get names of all data files and verify that they exist
     */
    const std::vector<SceneDataFileInfo> sceneDataFileInfo(sceneFile->getAllDataFileInfoFromAllScenes());
    std::vector<AString> allDataFileNames;
    AString dataFileErrors;
    for (const SceneDataFileInfo& sdfi : sceneDataFileInfo) {
        const AString dataFileName(sdfi.getAbsolutePathAndFileName());
        FileInformation dataFileInfo(dataFileName);
        if (dataFileInfo.isRemoteFile()) {
            dataFileErrors.push_back("File on network not supported: "
                                     + dataFileName);
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
            dataFileErrors.appendWithNewLine("Does not exist: "
                                             + dataFileName);
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
        if (outputFileInfo.exists()) {
            dataFileErrors.push_back("Output file exists: "
                                     + outputDataFileName);
        }
        dataFileFromToFileNames.emplace_back(dataFileName,
                                             outputDataFileName);
    }
    
    if ( ! dataFileErrors.isEmpty()) {
        throw OperationException(dataFileErrors);
    }
    
    /*
     * Create directory for data files
     */
    std::cout << "..." << previewText << "creating data file directory: " << dataFilesDirectoryName << std::endl;
    if (executeModeFlag) {
        if ( ! outputDir.mkdir(dataFilesDirectoryName)) {
            throw OperationException("Failed to create data files subdirectory \""
                                     + dataFilesDirectoryName
                                     + "\"");
        }
    }
    
   /*
     * Copy the data files to their new locations
     */
    for (const auto& fromToNames : dataFileFromToFileNames) {
        const AString& fromFileName(fromToNames.first);
        const AString& toFileName(fromToNames.second);
        FileInformation fromFileInfo(fromFileName);
        const int64_t fileSize(fromFileInfo.size());
        const AString sizeString(FileInformation::fileSizeToStandardUnits(fileSize));
        std::cout << "..." << previewText << "copying: " << sizeString << " from " << fromFileName << std::endl;
        std::cout << "      to: " << toFileName << std::endl;
        if (executeModeFlag) {
            try {
                CaretTemporaryFile ctf;
                ctf.readFile(fromFileName);
                ctf.writeFile(toFileName);
            }
            catch (const DataFileException& dfe) {
                throw OperationException(dfe.whatString());
            }
        }
    }

    
    /*
     * Update paths of all data files
     */
    updateDataFilePaths(sceneFile.get(),
                        dataFilesDirectoryName,
                        previewText,
                        executeModeFlag);
    
    /*
     * New path for scene file
     */
    const AString outputSceneFileName(outputDirectoryName
                                      + "/"
                                      + sceneFile->getFileNameNoPath());
    /*
    * Read each scene
    * get all ScenePathName elements and update them
    * find the Spec File in the scene
    * update paths in the spec file
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
    std::cout << "..." << previewText << "writing scene file: " << outputSceneFileName << std::endl;
    if (executeModeFlag) {
        try {
            sceneFile->writeFile(outputSceneFileName);
        }
        catch (const DataFileException& dfe) {
            throw OperationException(dfe.whatString());
        }
    }
    
    if (previewModeFlag) {
        std::cout << "...Preview Mode Complete" << std::endl;
    }
}

/**
 * Update the paths of all data files in all scenes in the given scene file
 * @param sceneFile
 *    The scene file
 * @param dataFilesDirectoryName
 *    New location of all data files
 * @param previewText
 *    Preview text if in preview mode
 * @param executeModeFlag
 *    If true, do not make any changes
 */
void
OperationSceneFileUtility::updateDataFilePaths(SceneFile* sceneFile,
                                               const AString& dataFilesDirectoryName,
                                               const AString& previewText,
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
//                    std::cout << "..." << previewText << "ScenePathName: " << spn->stringValue() << std::endl;
//                    std::cout << "      changed to: " << relocatedFileName << std::endl;
                    
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
OperationSceneFileUtility::getChildDataFiles(const AString& dataFileName)
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
