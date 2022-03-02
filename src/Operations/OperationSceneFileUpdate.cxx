
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
#include "SessionManager.h"

using namespace caret;

bool OperationSceneFileUpdate::s_fatalErrorFlag = true;
bool OperationSceneFileUpdate::s_enableCopyMapsOptionFlag = false;
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
    
    const QString fixMapPaletteSettingsSwitch("-fix-map-palette-settings");
    ret->createOptionalParameter(PARAM_KEY_OPTION_FIX_MAP_PALETTE_SETTINGS,
                                 fixMapPaletteSettingsSwitch,
                                 "Fix palette settings for files with change in number of maps");

    const QString copyMapOnePaletteToAllMapsSwitch("-copy-map-one-palette");
    if (s_enableCopyMapsOptionFlag) {
        ParameterComponent* copyMapOnePaletteOpt = ret->createRepeatableParameter(PARAM_KEY_OPTION_COPY_MAP_ONE_PALETTE,
                                                                                  copyMapOnePaletteToAllMapsSwitch,
                                                                                  "Copy palettes settings from first map to all maps in a data file");
        copyMapOnePaletteOpt->addStringParameter(1, "Data File Name Suffix", "Name of palette mapped data file (cifti, metric, volume)");
    }

    const QString errorsAsWarningsSwitch("-Werror");
    ret->createOptionalParameter(PARAM_KEY_OPTION_ERROR_AS_WARNING,
                                 errorsAsWarningsSwitch,
                                 "Treat file errors as warnings (unable to find a file or fix a file's palettes)");
    
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
                        "\n\n"
                        "Note: the " + fixMapPaletteSettingsSwitch + " and " + copyMapOnePaletteToAllMapsSwitch
                        + " options are mutually exclusive."
                        "\n\n\"");
    }
    helpText.append(errorsAsWarningsSwitch
                    + "\" will treat file errors (unable to find a file or fix a file's palettes) as warnings and print message(s).  "
                    "If this option is not specified and a file error occurs, the command will terminate without "
                    "creating a new scene file.\n"
                    );

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
    
    int32_t optionsCounter(0);
    
    OptionalParameter* fixMapPaletteSettingsOption(myParams->getOptionalParameter(PARAM_KEY_OPTION_FIX_MAP_PALETTE_SETTINGS));
    const bool fixMapPaletteSettingsFlag(fixMapPaletteSettingsOption->m_present);
    if (fixMapPaletteSettingsFlag) {
        ++optionsCounter;
    }

    std::vector<AString> copyMapOneDataFileNames;
    if (s_enableCopyMapsOptionFlag) {
        const std::vector<ParameterComponent*>& copyMapOneInstances(myParams->getRepeatableParameterInstances(PARAM_KEY_OPTION_COPY_MAP_ONE_PALETTE));
        for (ParameterComponent* copyMapPC : copyMapOneInstances) {
            copyMapOneDataFileNames.push_back(copyMapPC->getString(1));
            ++optionsCounter;
            
            if (fixMapPaletteSettingsFlag) {
                throw OperationException("Cannot have both options for repairing palettes");
            }
        }
    }
    
    if (optionsCounter <= 0) {
        throw OperationException("At least one of the optional parameters must be specified.");
    }

    OptionalParameter* errorsAsWarningsOption(myParams->getOptionalParameter(PARAM_KEY_OPTION_ERROR_AS_WARNING));
    if (errorsAsWarningsOption->m_present) {
        s_fatalErrorFlag = false;
    }
    
    OptionalParameter* verboseOption(myParams->getOptionalParameter(PARAM_KEY_OPTION_VERBOSE));
    if (verboseOption->m_present) {
        s_verboseFlag = true;
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
                throw OperationException("Scene index is invalid");
            }
        }
        else {
            throw OperationException("Scene name is invalid");
        }
    }
    CaretAssert(scene);
    
    /*
     * Enable voxel coloring since it is defaulted off for commands
     */
    VolumeFile::setVoxelColoringEnabled(true);
        
    /*
     * Restore the scene
     */
    const SceneClass* guiManagerClass = scene->getClassWithName("guiManager");
    if (guiManagerClass->getName() != "guiManager") {
        throw OperationException("Top level scene class should be guiManager but it is: "
                                 + guiManagerClass->getName());
    }
    
    /*
     * Ignore unable to find palette messages since the user
     * may have changed the number of maps.
     */
    SceneAttributes sceneAttributes(SceneTypeEnum::SCENE_TYPE_FULL,
                                    scene);
    sceneAttributes.setLogFilesWithPaletteSettingsErrors(true);
    
    SessionManager* sessionManager = SessionManager::get();
    sessionManager->restoreFromScene(&sceneAttributes,
                                     guiManagerClass->getClass("m_sessionManager"));
    
    /*
     * Get the error message but continue processing since the error
     * may not affect the scene.  Print error message later.
     */
    const AString sceneErrorMessage(sceneAttributes.getErrorMessage());
    if ( ! sceneErrorMessage.isEmpty()) {
        throw OperationException(sceneErrorMessage);
    }
    
    if (sessionManager->getNumberOfBrains() <= 0) {
        throw OperationException("Scene loading failure, SessionManager contains no Brains");
    }
    CaretAssert(SessionManager::get()->getBrain(0));


    bool sceneUpdatedFlag(false);
    
    if (fixMapPaletteSettingsFlag) {
        if (fixPalettesInFilesWithMapCountChanged(scene,
                                                  sceneAttributes) > 0) {
            sceneUpdatedFlag = true;
        }
    }
    
    if (! copyMapOneDataFileNames.empty()) {
        if (copyMapOnePalettes(scene,
                           sceneAttributes,
                               copyMapOneDataFileNames) > 0) {
            sceneUpdatedFlag = true;
        }
    }
    
    if (sceneUpdatedFlag) {
        sceneFile.writeFile(outputSceneFileName);
    }
    else {
        throw OperationException("No changes were made to scene file.  Scene file was not written.");
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
        throw OperationException("No files were found with a change in map count compared to map count in palette settings in scene.");
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
                    /*
                     * One palette used for all maps so nothing to fix
                     */
                    foundFlag = true;
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
                        processError("Failed to fix: " + name);
                    }
                }
                else {
                    processError("File not fixed (Apply to All Maps is OFF): "
                                 + name);
                }
                foundFlag = true;
                ++updateCounter;
                
                break;
            }
        }
        
        if ( ! foundFlag) {
            processError("Unable to fix (file may not have been read and is missing: "
                         + name);
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
 * @param copyMapOneDataFileNames
 *   Name of map files
 */
int32_t
OperationSceneFileUpdate::copyMapOnePalettes(Scene* scene,
                                             SceneAttributes& sceneAttributes,
                                             std::vector<AString> copyMapOneDataFileNames)
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
    bool mapPalettesUpdatedFlag(false);
    for (auto& mapFile : allDataFiles) {
        /*
         * Loop through names of files specified by user
         */
        for (AString& copyMapFileName : copyMapOneDataFileNames) {
            bool exitInnerLoopFlag(false);
            if (mapFile->getFileName().endsWith(copyMapFileName)) {
                if (mapFile->isOnePaletteUsedForAllMaps()) {
                    /*
                     * One palette used for all maps so nothing to fix
                     */
                }
                else if (mapFile->isMappedWithPalette()) {
                    const int32_t updateCount(updateScenePaletteXML(scene,
                                                                    &sceneAttributes,
                                                                    mapFile,
                                                                    copyMapFileName,
                                                                    MatchNameMode::MATCH_END_OF_NAME));
                    if (updateCount > 0) {
                        filesUpdatedCounter += updateCount;
                        mapPalettesUpdatedFlag = true;
                        
                        /*
                         * Do not need to process 'mapFile' again so exit names loop
                         */
                        exitInnerLoopFlag = true;
                    }
                    else {
                        processError("FAILED TO UPDATE: "
                                     + mapFile->getFileName()
                                     + " and matching to "
                                     + copyMapFileName);
                    }
                }
                else {
                    processError(mapFile->getFileName()
                                 + " is not mapped with a palette.");
                }
            }
            
            if (exitInnerLoopFlag) {
                break;
            }
        }
    }
    
    return filesUpdatedCounter;
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
                        //mapFile->setApplyPaletteColorMappingToAllMaps(true);
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
                        
                        if (s_verboseFlag) {
                            std::cout << "Palettes updated for: " << mapFile->getFileName() << std::endl;
                        }
                    }
                }
            }

            break;
        }
    }
    
    return updateCounter;
}

/**
 * Process error messages
 * @param message
 *    The error message
 */
void
OperationSceneFileUpdate::processError(const AString& message)
{
    if (s_fatalErrorFlag) {
        throw OperationException(message);
    }
    std::cout << "WARNING: " << message << std::endl;
}
