
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

#include <cstdio>


#include "Brain.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "DataFileException.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventDataFileAdd.h"
#include "EventDataFileDelete.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "OperationSceneFileUpdate.h"
#include "OperationException.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneFile.h"
#include "SceneObject.h"
#include "SceneObjectArray.h"
#include "SpecFile.h"
#include "SessionManager.h"
#include "TextFile.h"

using namespace caret;

AString OperationSceneFileUpdate::s_errorMessages;
bool OperationSceneFileUpdate::s_fatalErrorFlag = false;
bool OperationSceneFileUpdate::s_enableCopyMapsOptionFlag = true;
bool OperationSceneFileUpdate::s_verboseFlag = false;

/**
 * \class caret::OperationSceneFileUpdate
 * \brief Fix palettes in a scene file after the number of maps in a file changes
 *
 * Fix palettes in a scene file after the number of maps in a file changes
 */

/**
 * @return Command line switch
 */
AString
OperationSceneFileUpdate::getCommandSwitch()
{
    return "-scene-file-update";
}

/**
 * @return Short description of operation
 */
AString
OperationSceneFileUpdate::getShortDescription()
{
    return "UPDATE SCENE FILE";
}

/**
 * @return Parameters for operation
 */
OperationParameters*
OperationSceneFileUpdate::getParameters()
{
    OperationParameters* ret(new OperationParameters());
    
    ret->addStringParameter(PARAM_KEY_INPUT_SCENE_FILE, "input-scene-file", "the input scene file");
    
    ret->addStringParameter(PARAM_KEY_OUTPUT_SCENE_FILE, "output-scene-file", "the new scene file to create");
    
    ret->addStringParameter(PARAM_KEY_SCENE_NAME_NUMBER, "scene-name-or-number", "name or number (starting at one) of the scene in the scene file");
    
    const QString copyMapOnePaletteToAllMapsSwitch("-copy-map-one-palette");
    if (s_enableCopyMapsOptionFlag) {
        ParameterComponent* copyMapOnePaletteOpt = ret->createRepeatableParameter(PARAM_KEY_OPTION_COPY_MAP_ONE_PALETTE,
                                                                                  copyMapOnePaletteToAllMapsSwitch,
                                                                                  "Copy palettes settings from first map to all maps in a data file");
        copyMapOnePaletteOpt->addStringParameter(1, "Data File Name Suffix", "Name of palette mapped data file (cifti, metric, volume)");
    }
    
    const QString addRemoveFileDescription("Name of data file.  If a data file not in the current directory, it is best to use "
                                           "an absolute path (a relative path may work).  Instead of a data file, this value may "
                                           "be the name of a text file (must end in \".txt\") that contains the names of "
                                           "one or more data files, one per line.\n"
                                           "Example on UNIX to create a text file containing all CIFTI scalar files in the current directory "
                                           "with absolute paths:\n"
                                           "   ls -d $PWD/*dscalar.nii  > file.txt");
    const QString dataFileAddSwitch("-data-file-add");
    ParameterComponent* dataFileAddOpt(ret->createRepeatableParameter(PARAM_KEY_OPTION_DATA_FILE_ADD,
                                                                      dataFileAddSwitch,
                                                                      "Add a data file to scene's loaded files"));
    dataFileAddOpt->addStringParameter(1, "Name of data file", addRemoveFileDescription);
    
    const QString dataFileRemoveSwitch("-data-file-remove");
    ParameterComponent* dataFileRemoveOpt(ret->createRepeatableParameter(PARAM_KEY_OPTION_DATA_FILE_REMOVE,
                                                                      dataFileRemoveSwitch,
                                                                      "Remove a data file from scene's loaded files"));
    dataFileRemoveOpt->addStringParameter(1, "Name of data file", addRemoveFileDescription);
    
    const QString fixMapPaletteSettingsSwitch("-fix-map-palette-settings");
    ret->createOptionalParameter(PARAM_KEY_OPTION_FIX_MAP_PALETTE_SETTINGS,
                                 fixMapPaletteSettingsSwitch,
                                 "Fix palette settings for files with change in number of maps");

    const QString removeMissingFilesSwitch("-remove-missing-files");
    ret->createOptionalParameter(PARAM_KEY_OPTION_REMOVE_MISSING_FILES,
                                 removeMissingFilesSwitch,
                                 "Remove missing files from SpecFile");
    
    const QString errorSwitch("-error");
    ret->createOptionalParameter(PARAM_KEY_OPTION_ERROR,
                                 errorSwitch,
                                 "Abort command if there is an error performing any of the operations on the scene file");
    
    const QString verboseSwitch("-verbose");
    ret->createOptionalParameter(PARAM_KEY_OPTION_VERBOSE,
                                 verboseSwitch,
                                 "Print names of files that have palettes updated");
    
    AString helpText("This command will update a scene for specific changes in data files.\n"
                     "\n\""
                     + fixMapPaletteSettingsSwitch
                     + "\" will find all data files that have had a change in the number of maps since the scene "
                     "scene was created.  If the file has its \"Apply to All Maps\" property enabled, the palette "
                     "setting in the first map is copied to all maps in the file.  Note: This modifies the palette "
                     "settings for the file in the scene (data file is NOT modified)."
                     "\n\n\"");
    if (s_enableCopyMapsOptionFlag) {
        helpText.append(copyMapOnePaletteToAllMapsSwitch
                        + "\" will copy the palette settings from the first map to all other maps in a data file.  This option "
                        "is typically used when the number of maps in a data file changes.  It "
                        "changes the palette settings in the scene that are applied to the data file when the scene "
                        "is loaded (the data file is not modified).  The name of the data file specified on the "
                        "command line is matched to the end of file names in the scene.  This allows matching multiple "
                        "files if their names end with the same characters.  It also allows including a relative path "
                        "when there is more than one file with the same name but in different paths and only one of the "
                        "files to be updated."
                        "\n\n\"");
    }
    helpText.append(removeMissingFilesSwitch
                    + "\" Any files that fail to load when the scene is read will be removed from the scene.  Thus, if "
                    "one deletes files prior to running with this option, the deleted files are removed from the scene.  "
                    "\n\n\"");
    
    helpText.append(errorSwitch
                    + "\" If this option is provided and there is an error while performing any of the scene "
                    "operations, the command will immediately cease processing and the output scene file will not "
                    "be created.   Otherwise any errors will be listed after the command finishes.\n");

    ret->setHelpText(helpText);
    
    return ret;
}

/**
 * Use Parameters and perform operation
 */
void
OperationSceneFileUpdate::useParameters(OperationParameters* myParams,
                                        ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const AString inputSceneFileName(FileInformation(myParams->getString(PARAM_KEY_INPUT_SCENE_FILE)).getAbsoluteFilePath());
    const AString outputSceneFileName(FileInformation(myParams->getString(PARAM_KEY_OUTPUT_SCENE_FILE)).getAbsoluteFilePath());
    AString sceneNameOrNumber = myParams->getString(PARAM_KEY_SCENE_NAME_NUMBER);
    
    std::vector<SceneOperation> sceneOperations;
    
    const std::vector<ParameterComponent*>& dataFileAddInstances(myParams->getRepeatableParameterInstances(PARAM_KEY_OPTION_DATA_FILE_ADD));
    for (ParameterComponent* dataFileAddComp : dataFileAddInstances) {
        sceneOperations.push_back(SceneOperation(SceneOperationType::DATA_FILE_ADD,
                                                 dataFileAddComp->getString(1)));
    }
    
    const std::vector<ParameterComponent*>& dataFileRemoveInstances(myParams->getRepeatableParameterInstances(PARAM_KEY_OPTION_DATA_FILE_REMOVE));
    for (ParameterComponent* dataFileRemoveComp : dataFileRemoveInstances) {
        sceneOperations.push_back(SceneOperation(SceneOperationType::DATA_FILE_REMOVE,
                                                 dataFileRemoveComp->getString(1)));
    }
    
    bool haveFixMapPaletteSettingsFlag(false);
    if (myParams->getOptionalParameter(PARAM_KEY_OPTION_FIX_MAP_PALETTE_SETTINGS)->m_present) {
        sceneOperations.push_back(SceneOperation(SceneOperationType::FIX_MAP_PALETTE_SETTINGS,
                                                 ""));
        haveFixMapPaletteSettingsFlag = true;
    }

    if (s_enableCopyMapsOptionFlag) {
        const std::vector<ParameterComponent*>& copyMapOneInstances(myParams->getRepeatableParameterInstances(PARAM_KEY_OPTION_COPY_MAP_ONE_PALETTE));
        for (ParameterComponent* copyMapPC : copyMapOneInstances) {
            sceneOperations.push_back(SceneOperation(SceneOperationType::COPY_MAP_ONE_PALETTE,
                                                     copyMapPC->getString(1)));
        }
    }
    
    OptionalParameter* removeMissingFilesOption(myParams->getOptionalParameter(PARAM_KEY_OPTION_REMOVE_MISSING_FILES));
    const bool removeMissingFilesFlag(removeMissingFilesOption->m_present);
    if (removeMissingFilesFlag) {
        /* prevents need "at least one operation error" */
        sceneOperations.push_back(SceneOperation(SceneOperationType::REMOVE_MISSING_FILES, ""));
    }
    
    OptionalParameter* errorOption(myParams->getOptionalParameter(PARAM_KEY_OPTION_ERROR));
    if (errorOption->m_present) {
        s_fatalErrorFlag = true;
    }
    
    OptionalParameter* verboseOption(myParams->getOptionalParameter(PARAM_KEY_OPTION_VERBOSE));
    if (verboseOption->m_present) {
        s_verboseFlag = true;
    }
    
    /*
     * Did user provide an operation?
     */
    if (sceneOperations.empty()) {
        throw OperationException("At least one of the optional parameters must be provided.");
    }
    
    /*
     * Read the scene file and load the scene
     */
    SceneFile sceneFile;
    sceneFile.readFile(inputSceneFileName);
    Scene* scene = sceneFile.getSceneWithName(sceneNameOrNumber);
    if (scene == NULL) {
        bool valid = false;
        const int32_t sceneIndexStartAtOne = sceneNameOrNumber.toInt(&valid);
        if (valid) {
            const int32_t sceneIndex = sceneIndexStartAtOne - 1;
            if ((sceneIndex >= 0)
                && (sceneIndex < sceneFile.getNumberOfScenes())) {
                scene = sceneFile.getSceneAtIndex(sceneIndex);
            }
            else {
                throw OperationException("Scene index is invalid: "
                                         + sceneNameOrNumber);
            }
        }
        else {
            throw OperationException("Scene name is invalid: "
                                     + sceneNameOrNumber);
        }
    }
    CaretAssert(scene);
    
    /*
     * Enable voxel coloring since it is defaulted off for commands
     * Probably do not need this
     */
    VolumeFile::setVoxelColoringEnabled(true);
        
    /*
     * Restore the scene
     */
    const SceneClass* guiManagerClass = scene->getClassWithName("guiManager");
    if (guiManagerClass->getName() != "guiManager") {
        throw OperationException("PROGRAM ERROR: Top level scene class should be guiManager but it is: "
                                 + guiManagerClass->getName());
    }
    
    /*
     * Ignore unable to find palette messages since the user
     * may have changed the number of maps.
     */
    SceneAttributes sceneAttributes(SceneTypeEnum::SCENE_TYPE_FULL,
                                    scene);
    if (haveFixMapPaletteSettingsFlag) {
        sceneAttributes.setLogFilesWithPaletteSettingsErrors(true);
    }
    
    /*
     * Keep all files in the scene when writing the scene,
     * event those files that were not or failed to load.
     */
    sceneAttributes.setKeepAllFilesInScene(true);
    
    SessionManager* sessionManager = SessionManager::get();
    sessionManager->restoreFromScene(&sceneAttributes,
                                     guiManagerClass->getClass("m_sessionManager"));
    
    /*
     * Get the error message but continue processing since the error
     * may not affect the scene.  Print error message later.
     */
    const AString sceneErrorMessage(sceneAttributes.getErrorMessage());
    if ( ! sceneErrorMessage.isEmpty()) {
        std::cout << ("Begin errors loading scene (command will continue):\n"
                      + sceneErrorMessage
                      + "\nEnd errors loading scene (command will continue)\n");
        if (removeMissingFilesFlag) {
            std::cout << ("\nWhen running with the \""
                          + removeMissingFilesOption->m_optionSwitch
                          + "\" option, the error\n"
                          "messages above will include \"File was not found\" messages.\n"
                          "These \"File was not found\" messages can be ignored as they are\n"
                          "listing the files that will be removed by this command.") << std::endl << std::endl;
        }
    }
    
    if (sessionManager->getNumberOfBrains() <= 0) {
        throw OperationException("Scene loading failure, SessionManager contains no Brains; scenee is invalid or corrupt");
    }
    Brain* brain(SessionManager::get()->getBrain(0));
    if (brain == NULL) {
        throw OperationException("Unable to find \"brain\"; scene is invalid or corrupt");
    }


    bool sceneUpdatedFlag(false);
    
    /*
     * Sort the operations on the scene
     * Note: the SceneOperationType enums are integers
     * used for sorting
     */
    std::sort(sceneOperations.begin(),
              sceneOperations.end());

    bool specFileModified(false);
    
    for (auto& sc : sceneOperations) {
        addToVerboseMessages("Performing operation: "
                             + SceneOperation::typeToName(sc.m_sceneOperationType));

        switch (sc.m_sceneOperationType) {
            case SceneOperationType::COPY_MAP_ONE_PALETTE:
                if (copyMapOnePalettes(scene,
                                       sceneAttributes,
                                       sc.m_parameter) > 0) {
                    sceneUpdatedFlag = true;
                }
                break;
            case SceneOperationType::DATA_FILE_ADD:
                if (dataFileAddRemove(scene,
                                      sc.getFileNames(),
                                      brain,
                                      SceneOperationType::DATA_FILE_ADD) > 0) {
                    sceneUpdatedFlag = true;
                    specFileModified = true;
                }
                break;
            case SceneOperationType::DATA_FILE_REMOVE:
                if (dataFileAddRemove(scene,
                                      sc.getFileNames(),
                                      brain,
                                      SceneOperationType::DATA_FILE_REMOVE) > 0) {
                    sceneUpdatedFlag = true;
                    specFileModified = true;
                }
                break;
            case SceneOperationType::FIX_MAP_PALETTE_SETTINGS:
                if (fixPalettesInFilesWithMapCountChanged(scene,
                                                          sceneAttributes) > 0) {
                    sceneUpdatedFlag = true;
                }
                break;
            case SceneOperationType::REMOVE_MISSING_FILES:
            {
                SpecFile* specFile(brain->getSpecFile());
                CaretAssert(specFile);
                const int32_t removedCount(specFile->removeAllNonLoadedFiles());
                if (removedCount > 0) {
                    addToVerboseMessages("Removed "
                                         + AString::number(removedCount)
                                         + " missing files.");
                    sceneUpdatedFlag = true;
                    specFileModified = true;
                }
            }
                /* Nothing here, using the option sets a SceneAttribute above */
                break;
        }
    }
    
    if (specFileModified) {
        SpecFile* specFile(brain->getSpecFile());
        SceneClass* newSpecFileClass(specFile->saveToScene(&sceneAttributes, "specFile"));
        CaretAssert(newSpecFileClass);
        
        /*
         * <ObjectArray Type="class" Name="m_brains" Length="1">
         *    <Element Index="0">
         *       <Object Type="class" Class="Brain" Name="m_brains" Version="1">
         *
         *  lots of stuff
         *
         * <Object Type="class" Class="SpecFile" Name="specFile" Version="1">
         *    <Object Type="pathName" Name="specFileName">../../../fsaverage_LR32k/S1200_MSMAll3T535V.MSMAll.32k_fs_LR.wb.spec</Object>
         *    <ObjectArray Type="class" Name="dataFilesArray" Length="35">
         *       <Element Index="0">
         */
        SceneObject* brainArrayObject(scene->getDescendantWithName("m_brains"));
        if (brainArrayObject == NULL) {
            throw OperationException("Unable to find object named \"m_brains\" in the scene.  Is the scene corrupt?");
        }
        
        SceneClassArray* brainArrayClass(dynamic_cast<SceneClassArray*>(brainArrayObject));
        if (brainArrayClass->getNumberOfArrayElements() != 1) {
            throw OperationException("PROGRAM ERROR: m_brains has child brain count (should be 1): "
                                     + AString::number(brainArrayClass->getNumberOfArrayElements()));
        }
        
        SceneClass* brainClass(brainArrayClass->getClassAtIndex(0));
        if (brainClass == NULL) {
            throw OperationException("PROGRAM ERROR: brain class at index 0 is invalid.");
        }
        
        const SceneObject* specFileObject(brainClass->getObjectWithName("specFile"));
        if (specFileObject == NULL) {
            throw OperationException("PROGRAM ERROR: Unable to find \"specFile\" child of \"m_brains\"");
        }
        
        if ( ! brainClass->replaceChild(specFileObject,
                                        newSpecFileClass)) {
            throw OperationException("PROGRAM ERROR: Failed to replace spec file object in scene with new spec file object");
        }
    }
    
    if ( ! sceneUpdatedFlag) {
        addToErrorMessages("No changes were made to scene file.  New scene file was created.");
    }
    
    sceneFile.writeFile(outputSceneFileName);
    
    if ( ! s_errorMessages.isEmpty()) {
        std::cout << std::endl << "Scene file has been created.  These errors occurred during processing:" << std::endl;
        std::cout << s_errorMessages << std::endl << std::endl;
    }
}

/**
 * Fix palette settings for files with a change in the number of maps
 * @param sceneAttributes
 *    The scene attributes containing data files with changed map counts
 * @return
 *    Number of files that had palettes settings updated in the scene.
 */
int32_t
OperationSceneFileUpdate::fixPalettesInFilesWithMapCountChanged(Scene* scene,
                                                                SceneAttributes& sceneAttributes)
{
    int32_t updateCounter(0);

    const std::vector<std::pair<CaretMappableDataFile*, AString>> filesAndNames(sceneAttributes.getMapFilesWithPaletteSettingsErrors());
    if (filesAndNames.empty()) {
        addToErrorMessages("Fix Palettes: No files were found with a change in map count compared to map count in palette settings in scene.");
        return updateCounter;
    }
    
    std::vector<CaretMappableDataFile*> allDataFiles;
    EventCaretMappableDataFilesGet mapFilesEvent;
    EventManager::get()->sendEvent(mapFilesEvent.getPointer());
    mapFilesEvent.getAllFiles(allDataFiles);

    /* ensure apply to all selected */
    for (auto& fn : filesAndNames) {
        CaretMappableDataFile* file(fn.first);
        const AString name(fn.second);
        
        bool foundFlag(false);
        for (auto& mapFile : allDataFiles) {
            if (mapFile == file) {
                if (mapFile->isOnePaletteUsedForAllMaps()) {
                    if (mapFile->isOnePaletteUsedForAllMaps()) {
                        /*
                         * One palette used for all maps so nothing to fix
                         */
                        foundFlag = true;
                        addToErrorMessages("File not fixed, one palette is used for all maps: "
                                           + name);
                    }
                    else if (mapFile->isApplyPaletteColorMappingToAllMaps()) {
                        const int32_t changedCount(updateScenePaletteXML(scene,
                                                                         &sceneAttributes,
                                                                         mapFile,
                                                                         mapFile->getFileNameNoPath(),
                                                                         MatchNameMode::MATCH_EXACT));
                        if (changedCount > 0) {
                            updateCounter += changedCount;
                        }
                        else {
                            addToErrorMessages("Failed to fix palettes: "
                                               + name);
                        }
                        foundFlag = true;
                    }
                    else {
                        addToErrorMessages("File not fixed, Apply to All Maps is OFF): "
                                           + mapFile->getFileName());
                    }
                }
                else {
                    addToErrorMessages("Map one palettes not copied, file is not mapped with palettes: "
                                       + mapFile->getFileName());
                }
                
                break;
            }
        }
        
        if ( ! foundFlag) {
            addToErrorMessages("Unable to fix (file may not have been read and is missing): "
                               + file->getFileName());
        }
    }
    
    return updateCounter;
}

/**
 * Copy the map one palette to all other palettes for files
 * @param scene
 *   Scene that is being updated
 * @param sceneAttributes
 *   The scene attributes
 * @param copyMapOneDataFileName
 *   Name of map file
 */
int32_t
OperationSceneFileUpdate::copyMapOnePalettes(Scene* scene,
                                             SceneAttributes& sceneAttributes,
                                             const AString& copyMapOneDataFileName)
{
    int32_t filesUpdatedCounter(0);
    
    /*
     * Get all mappable data files
     */
    std::vector<CaretMappableDataFile*> allDataFiles;
    EventCaretMappableDataFilesGet mapFilesEvent;
    EventManager::get()->sendEvent(mapFilesEvent.getPointer());
    mapFilesEvent.getAllFiles(allDataFiles);
    
    /*
     * Loop through map files in memory loaded by scene
     */
    for (auto& mapFile : allDataFiles) {
        if (mapFile->getFileName().endsWith(copyMapOneDataFileName)) {
            if (mapFile->isMappedWithPalette()) {
                if (mapFile->isOnePaletteUsedForAllMaps()) {
                    /*
                     * One palette used for all maps so nothing to fix
                     */
                    addToErrorMessages("Map one palettes not copied, one palette is used for all maps: "
                                       + mapFile->getFileName());
                }
                else {
                    const int32_t updateCount(updateScenePaletteXML(scene,
                                                                &sceneAttributes,
                                                                mapFile,
                                                                copyMapOneDataFileName,
                                                                MatchNameMode::MATCH_END_OF_NAME));
                    if (updateCount > 0) {
                        filesUpdatedCounter += updateCount;
                    }
                    else {
                        addToErrorMessages("Failed to copy map one palettes: "
                                           + mapFile->getFileName());
                    }
                }
            }
            else {
                addToErrorMessages("Map one palettes not copied, file is not mapped with palettes: "
                                   + mapFile->getFileName());
            }
        }
    }
    
    if (filesUpdatedCounter == 0) {
        addToErrorMessages("No files found for: "
                           + copyMapOneDataFileName);
    }
    
    return filesUpdatedCounter;
}

/**
 * Add or remove data files in the scene
 * @param scene
 *    The scene that needs to be updated
 * @param filenames
 *    Names of data files
 * @param brain
 *    The 'brain'
 * @param addRemoveOperation
 *    Operation to add or remove
 *
 */
int32_t
OperationSceneFileUpdate::dataFileAddRemove(Scene* /*scene*/,
                                            const std::vector<AString>& filenames,
                                            Brain* brain,
                                            const SceneOperationType addRemoveOperation)
{
    CaretAssert(brain);

    bool addFlag(false);
    if (addRemoveOperation == SceneOperationType::DATA_FILE_ADD) {
        addFlag = true;
    }
    else if (addRemoveOperation == SceneOperationType::DATA_FILE_REMOVE) {
        addFlag = false;
    }
    else {
        CaretAssert(0);
    }
    
    if (filenames.empty()) {
        addToErrorMessages("No files provided for "
                           + AString(addFlag ? "Data File Add " : "Data File Remove ")
                           + " operation");
        return 0;
    }
    
    int32_t updateCounter(0);
    
    SpecFile* specFile(brain->getSpecFile());
    for (auto& name : filenames) {
        if (addFlag) {
            FileInformation fileInfo(name);
            const AString fullPathName(fileInfo.getAbsoluteFilePath());
            if (QFile::exists(fullPathName)) {
                bool validFlag(false);
                DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromFileExtension(fullPathName, &validFlag);
                if (validFlag) {
                    bool fileLoadingSelectionStatus(true);
                    bool fileSavingSelectionStatus(false);
                    bool specFileMemberStatus(true);
                    try {
                        specFile->addDataFile(dataFileType,
                                              StructureEnum::ALL,
                                              fullPathName,
                                              fileLoadingSelectionStatus,
                                              fileSavingSelectionStatus,
                                              specFileMemberStatus);
                        
                        ++updateCounter;
                        addToVerboseMessages("Added to Spec File: "
                                             + fullPathName);
                    }
                    catch (const DataFileException& dfe) {
                        addToErrorMessages("Failed to add "
                                           + fullPathName
                                           + " to SpecFile.  "
                                           + dfe.whatString());
                    }
                }
                else {
                    addToErrorMessages("Invalid file type: "
                                       + fullPathName);
                }
            }
            else {
                addToErrorMessages("File not added because file does not exist: "
                                   + fullPathName);
            }
        }
        else {
            const bool logSevereIfFailureToRemoveFileFlag(false);

            FileInformation fileInfo(name);
            const AString fullPathName(fileInfo.getAbsoluteFilePath());
            
            if (QFile::exists(fullPathName)) {
                std::vector<CaretDataFile*> allDataFiles;
                brain->getAllDataFiles(allDataFiles);
                for (auto dataFile : allDataFiles) {
                    if (dataFile->getFileName().endsWith(name)) {
                        if (specFile->removeCaretDataFile(dataFile,
                                                          logSevereIfFailureToRemoveFileFlag)) {
                            ++updateCounter;
                            addToVerboseMessages("Removed from Spec File: "
                                                 + dataFile->getFileName());
                        }
                        else {
                            addToErrorMessages("Failed to remove file: "
                                               + fullPathName);
                        }
                    }
                }
            }
            else {
                SpecFile* specFile(brain->getSpecFile());
                if (specFile->removeCaretDataFileByName(fullPathName,
                                                        logSevereIfFailureToRemoveFileFlag)) {
                    ++updateCounter;
                    if (s_verboseFlag) {
                        addToVerboseMessages("Removed from Spec File (by name): "
                                             + fullPathName);
                    }
                }
                else {
                    addToErrorMessages("Failed to remove file (by name): "
                                       + fullPathName);
                }
            }
        }
    }

    return updateCounter;
}


/**
 * Find the file in the Scene XML tree and update its palette content
 * @param scene
 *    The scene that needs to be updated
 * @param sceneAttributes
 *    The scene attributes
 * @param mapFile
 *    The map file with updated palette color mapping
 * @param dataFileNameOrSuffix
 *    Name of data file (note mapFile->getFileName() will have absolute path)
 * @param matchMode
 *    If EXACT, dataFileNameOrSuffix is the name of file without any path
 *    If END_OF_NAME dataFileNameOrSuffix matches to end of file name in scene.
 *
 */
int32_t
OperationSceneFileUpdate::updateScenePaletteXML(Scene* scene,
                                                SceneAttributes* sceneAttributes,
                                                CaretMappableDataFile* mapFile,
                                                const AString& dataFileNameOrSuffix,
                                                const MatchNameMode matchMode)
{
    CaretAssert(scene);
    CaretAssert(sceneAttributes);
    CaretAssert(mapFile);
    
    /*
     * We are looking for XML that looks something like this:
     *
     * <ObjectArray Type="class" Name="m_brains" Length="1">
     *    <Element Index="0">
     *       <Object Type="class" Class="Brain" Name="m_brains" Version="1">
     *           <ObjectArray Type="class" Name="allCaretDataFiles_V2" Length="3">
     *           <Element Index="0">
     *               <Object Type="class" Class="CaretDataFile" Name="Q1-Q6_R440.L.midthickness.32k_fs_LR.surf.gii" Version="1">
     *
     * We then look for the array element where "Name" is the name of the data file.  When found, we save the files data
     * to a SceneClass and then replace the element at the "Element Index"
     */
    int32_t updateCounter(0);
    
    /*
     * getDescendants() returns ALL XML elements in the scene.  We are looking for
     * the element named "allCaretDataFiles_V2" scene data for each file loaded
     * in the scene.   "allCaretDataFiles_V2" contains an array of elements,
     * one each for each data file in the scene.  We then look at the element
     * "Names", that are filenames, find the matching one and then replace
     * that file's scene data.
     */
    std::vector<SceneObject*> sceneDescendants(scene->getDescendants());
    for (auto& sd : sceneDescendants) {
        if (sd->getName() == "allCaretDataFiles_V2") {
            
            SceneClassArray* filesArray(dynamic_cast<SceneClassArray*>(sd));
            if (filesArray != NULL) {
                const int32_t numFiles(filesArray->getNumberOfArrayElements());
                for (int32_t iFile = 0; iFile < numFiles; iFile++) {
                    const SceneClass* fileClass(filesArray->getClassAtIndex(iFile));
                    
                    bool matchFlag(false);
                    switch (matchMode) {
                        case MatchNameMode::MATCH_EXACT:
                            matchFlag = fileClass->getName().endsWith(dataFileNameOrSuffix);
                            break;
                        case MatchNameMode::MATCH_END_OF_NAME:
                            matchFlag = fileClass->getName().endsWith(dataFileNameOrSuffix);
                            break;
                    }
                    
                    if (matchFlag) {
                        /*
                         * We located the file in the scene.  Now 'apply to all maps'
                         * for the data file in memory.
                         */
                        const int32_t mapIndex(0);
                        mapFile->applyPaletteColorMappingToAllMaps(mapIndex);

                        /*
                         * Duplicates code from Brain::saveToScene for 'allCaretDataFileScenes"
                         * Save's file's data to the scene
                         */
                        SceneClass* newSceneClassForFile(mapFile->saveToScene(sceneAttributes,
                                                                              mapFile->getFileNameNoPath()));
                        CaretAssert(newSceneClassForFile);
                        newSceneClassForFile->addPathName("dataFileName_V2",
                                                          mapFile->getFileName());

                        /*
                         * Replace the file's data in the scene.
                         */
                        filesArray->setClassAtIndex(iFile,
                                                    newSceneClassForFile);
                        
                        /*
                         * Delete the file's old scene data since it has been
                         * removed from the scene.
                         */
                        delete fileClass;
                        
                        ++updateCounter;
                        
                        addToVerboseMessages("Palettes updated for: "
                                             + mapFile->getFileName());
                    }
                }
            }

            break;
        }
    }
    
    return updateCounter;
}

/**
 * Add to error  messages
 * @param message
 *    The error message
 */
void
OperationSceneFileUpdate::addToErrorMessages(const AString& message)
{
    if (s_fatalErrorFlag) {
        throw OperationException(message);
    }
    s_errorMessages.appendWithNewLine(message);
}

/**
 * Add to verbose  messages
 * @param message
 *    The error message
 */
void
OperationSceneFileUpdate::addToVerboseMessages(const AString& message)
{
    if (s_verboseFlag) {
        std::cout << "Verbose: " << message << std::endl;
    }
}

/**
 * @return The parameter in a vector.  If the parameter supplied by the users
 *  is the name of a text file, return each line from the file in one of the vector
 *  parameters.  Otherwise, just return a single element vector containing
 *  the parameter.
 */
std::vector<AString>
OperationSceneFileUpdate::SceneOperation::getFileNames() const
{
    std::vector<AString> namesOut;
    
    if (m_parameter.endsWith(".txt")) {
        try {
            TextFile textFile;
            textFile.readFile(m_parameter);
            namesOut = textFile.getTextLines();
            if (namesOut.empty()) {
                addToErrorMessages("No filenames read from "
                                   + m_parameter);
            }
        }
        catch (DataFileException& dfe) {
            addToErrorMessages("Failed to read: "
                               + m_parameter
                               + " "
                               + dfe.whatString());
        }
    }
    else {
        namesOut.push_back(m_parameter);
    }
    
    return namesOut;
}

AString
OperationSceneFileUpdate::SceneOperation::typeToName(const SceneOperationType sceneOperationType)
{
    AString s;
    switch (sceneOperationType) {
        case SceneOperationType::REMOVE_MISSING_FILES:
            s = "Remove Missing Files";
            break;
        case SceneOperationType::DATA_FILE_REMOVE:
            s = "Data File - Remove";
            break;
        case SceneOperationType::DATA_FILE_ADD:
            s = "Data File - Add";
            break;
        case SceneOperationType::COPY_MAP_ONE_PALETTE:
            s = "Copy Map One Palette to Other Maps";
            break;
        case SceneOperationType::FIX_MAP_PALETTE_SETTINGS:
            s = "Fix map palette settings";
            break;
    }
    return s;
}
