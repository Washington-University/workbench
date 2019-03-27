
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __WB_MACRO_CUSTOM_OPERATION_MANAGER_DECLARE__
#include "WbMacroCustomOperationManager.h"
#undef __WB_MACRO_CUSTOM_OPERATION_MANAGER_DECLARE__

#include <QInputDialog>

#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "EventSurfacesGet.h"
#include "GuiManager.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "Surface.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WbMacroCustomOperationDelay.h"
#include "WbMacroCustomOperationModelRotation.h"
#include "WbMacroCustomOperationOverlayCrossFade.h"
#include "WbMacroCustomOperationSurfaceInterpolation.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WbMacroCustomOperationVolumeSliceIncrement.h"
#include "WbMacroCustomOperationVolumeToSurfaceCrossFade.h"
#include "WbMacroCustomDataInfo.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"
#include "WuQMessageBox.h"

using namespace caret;

/**
 * \class caret::WbMacroCustomOperationManager 
 * \brief Mananager for macro custom operations
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WbMacroCustomOperationManager::WbMacroCustomOperationManager()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
WbMacroCustomOperationManager::~WbMacroCustomOperationManager()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
WbMacroCustomOperationManager::toString() const
{
    return "WbMacroCustomOperationManager";
}

/**
 * Get names of all surfaces
 *
 * @param surfaceNamesOut
 *     Output with names of surfaces
 * @param errorMessageOut
 *     Ouput with error messages
 * @return
 *     True if surface names are valid, else false
 */
bool
WbMacroCustomOperationManager::getSurfaceNames(std::vector<QString>& surfaceNamesOut,
                                               QString& errorMessageOut)
{
    surfaceNamesOut.clear();
    
    EventSurfacesGet surfacesEvent;
    EventManager::get()->sendEvent(surfacesEvent.getPointer());
    std::vector<Surface*> surfaces = surfacesEvent.getSurfaces();
    bool okFlag(false);
    if (surfaces.empty()) {
        errorMessageOut = "There are no surfaces available";
    }
    else {
        for (const auto s : surfaces) {
            surfaceNamesOut.push_back(s->getFileNameNoPath());
        }
        okFlag = true;
    }
    
    return okFlag;
}

/**
 * Get mappable files selection info
 *
 * @param macroCommand
 *     Macro command containing parameter being edited
 * @param overlayFileParameter
 *     Parameter for overlay file selection
 * @param mapNameParameter
 *     Parameter for map name selection
 * @param mapFileNamesOut
 *     Output with map file names
 * @param selectedFileNameOut
 *     Output with name of selected file
 * @param mapNamesOut
 *     Output with name of maps in selected file
 * @param selectedMapNameOut
 *     Name of selected map
 * @param errorMessageOut
 *     Output error message if finding map files fails
 * @return
 *     True if map file names valid
 */
bool
WbMacroCustomOperationManager::getMappableFilesSelection(const WuQMacroCommand* macroCommand,
                                                         const WuQMacroCommandParameter* overlayFileParameterIn,
                                                         const WuQMacroCommandParameter* mapParameterIn,
                                                         std::vector<QString>& mapFileNamesOut,
                                                         QString& selectedFileNameOut,
                                                         std::vector<QString>& mapNamesOut,
                                                         QString& selectedMapNameOut,
                                                         QString& errorMessageOut)
{
    CaretAssert(macroCommand);
    mapFileNamesOut.clear();
    selectedFileNameOut.clear();
    mapNamesOut.clear();
    selectedMapNameOut.clear();
    errorMessageOut.clear();
    
    const WuQMacroCommandParameter* fileParameter = overlayFileParameterIn;
    const WuQMacroCommandParameter* mapParameter  = mapParameterIn;
    if ((fileParameter != NULL)
        && (mapParameter != NULL)) {
        /* OK, have both */
    }
    else {
        /*
         * Map selection parameter should be immediately after file selection parameter
         */
        int32_t fileParameterIndex(-1);
        int32_t mapParameterIndex(-1);
        if (fileParameter != NULL) {
            fileParameterIndex = macroCommand->getIndexOfParameter(fileParameter);
            mapParameterIndex  = fileParameterIndex + 1;
        }
        else if (mapParameter != NULL) {
            mapParameterIndex  = macroCommand->getIndexOfParameter(mapParameter);
            fileParameterIndex = mapParameterIndex - 1;
        }
        else {
            errorMessageOut = "Both overlay file and map parameter are NULL, one must be valid";
            return false;
        }
        
        if ((fileParameterIndex < 0)
            || (fileParameterIndex >= macroCommand->getNumberOfParameters())) {
            errorMessageOut.append("Unable to find file parameter.  ");
        }
        if ((mapParameterIndex < 0)
            || (mapParameterIndex >= macroCommand->getNumberOfParameters())) {
            errorMessageOut.append("Unable to find map parameter.  ");
        }
        if ( ! errorMessageOut.isEmpty()) {
            return false;
        }
        
        fileParameter = macroCommand->getParameterAtIndex(fileParameterIndex);
        mapParameter  = macroCommand->getParameterAtIndex(mapParameterIndex);
    }
    
    CaretAssert(fileParameter);
    CaretAssert(mapParameter);
    
    bool validFileParamFlag(false);
    const WbMacroCustomDataTypeEnum::Enum overlayParamType = WbMacroCustomDataTypeEnum::fromName(fileParameter->getCustomDataType(),
                                                                                                 &validFileParamFlag);
    if (overlayParamType != WbMacroCustomDataTypeEnum::OVERLAY_FILE_NAME_OR_FILE_INDEX) {
        errorMessageOut.append("Overlay file parameter is not of type OVERLAY_FILE_NAME_OR_FILE_INDEX.  ");
    }
    
    bool validMapNameParamFlag(false);
    const WbMacroCustomDataTypeEnum::Enum mapParamType = WbMacroCustomDataTypeEnum::fromName(mapParameter->getCustomDataType(),
                                                                                             &validMapNameParamFlag);
    if (mapParamType != WbMacroCustomDataTypeEnum::OVERLAY_MAP_NAME_OR_MAP_INDEX) {
        errorMessageOut.append("Overlay map parameter is not of type OVERLAY_MAP_NAME_OR_MAP_INDEX.  ");
    }
    
    if ( ! errorMessageOut.isEmpty()) {
        return false;
    }
    
    const QString selectedFileName = fileParameter->getValue().toString();
    const QString selectedMapName  = mapParameter->getValue().toString();
    
    CaretMappableDataFile* selectedFile(NULL);
    EventCaretMappableDataFilesGet mapFilesEvent;
    EventManager::get()->sendEvent(mapFilesEvent.getPointer());
    std::vector<CaretMappableDataFile*> allFiles;
    mapFilesEvent.getAllFiles(allFiles);
    if ( ! allFiles.empty()) {
        for (auto mf : allFiles) {
            CaretAssert(mf);
            const QString name(mf->getFileNameNoPath());
            if ( ! selectedFileName.isEmpty()) {
                if (name == selectedFileName) {
                    selectedFile = mf;
                }
            }
            mapFileNamesOut.push_back(name);
        }
        
        if (selectedFile == NULL) {
            CaretAssertVectorIndex(allFiles, 0);
            selectedFile = allFiles[0];
        }
        CaretAssert(selectedFile);
        selectedFileNameOut = selectedFile->getFileNameNoPath();
        
        bool selectedMapNameFoundFlag(false);
        const int32_t numMaps = selectedFile->getNumberOfMaps();
        for (int32_t i = 0; i < numMaps; i++) {
            const QString name(selectedFile->getMapName(i));
            if ( ! selectedMapName.isEmpty()) {
                if (selectedMapName == name) {
                    selectedMapNameFoundFlag = true;
                }
            }
            mapNamesOut.push_back(name);
        }
        
        if (selectedMapNameFoundFlag) {
            selectedMapNameOut = selectedMapName;
        }
        else {
            if (selectedFile->getNumberOfMaps() > 0) {
                selectedMapNameOut = selectedFile->getMapName(0);
            }
        }
    }
    
    return true;
}

/**
 * Get the map file in the overlay for this command
 *
 * @param browserWindowIndex
 *     Index of browser window
 * @param macroCommand
 *     Macro command containing parameter being edited
 * @param overlayFileParameter
 *     Parameter for overlay file.  Note that index of the overlay
 *     will be obtained from the most previous parameter that is of
 *     type OVERLAY_INDEX.
 * @param mapFileNamesOut
 *     Output with map file names
 * @param selectedMapFileOut
 *     Map file selected in overlay or NULL if none selected
 * @param selectedMapFileMapNamesOut
 *     Names of maps in selected map file, empty if no map file
 * @param errorMessageOut
 *     Output error message if finding map files fails
 * @return
 *     True if map file names valid
 */
bool
WbMacroCustomOperationManager::getOverlayContents(const int32_t browserWindowIndex,
                                                  const WuQMacroCommand* macroCommand,
                                                  const WuQMacroCommandParameter* overlayFileParameter,
                                                  std::vector<QString>& mapFileNamesOut,
                                                  CaretMappableDataFile* &selectedMapFileOut,
                                                  std::vector<QString>& selectedMapFileMapNamesOut,
                                                  QString& errorMessageOut)
{
    CaretAssert(macroCommand);
    CaretAssert(overlayFileParameter);
    
    mapFileNamesOut.clear();
    selectedMapFileOut = NULL;
    selectedMapFileMapNamesOut.clear();
    
    /*
     * Find the overlay index parameter that should be before this parameter
     */
    const int32_t parameterIndex = macroCommand->getIndexOfParameter(overlayFileParameter);
    if (parameterIndex < 0) {
        errorMessageOut = "Parameter is invalid for command";
        return false;
    }
    
    int32_t overlayIndex(-1);
    for (int32_t ip = (parameterIndex - 1); ip >= 0; --ip) {
        const WuQMacroCommandParameter* p = macroCommand->getParameterAtIndex(ip);
        if (p->getDataType() == WuQMacroDataValueTypeEnum::INTEGER) {
            bool valid(false);
            WbMacroCustomDataTypeEnum::Enum customType = WbMacroCustomDataTypeEnum::fromName(p->getCustomDataType(),
                                                                                             &valid);
            if (valid) {
                if (customType == WbMacroCustomDataTypeEnum::OVERLAY_INDEX) {
                    overlayIndex = p->getValue().toInt();
                    break;
                }
            }
        }
    }
    
    /*
     * Overlay index is 1..N for user so need to decrement
     */
    if (overlayIndex < 1) {
        errorMessageOut = "Unable to find overlay index";
        return false;
    }
    --overlayIndex;
    
    BrowserTabContent* tabContent = getTabContent(browserWindowIndex,
                                                  errorMessageOut);
    if (tabContent == NULL) {
        errorMessageOut = "Unable to find tab content";
        return false;
    }
    
    Overlay* overlay = tabContent->getOverlaySet()->getOverlay(overlayIndex);
    if (overlay == NULL) {
        errorMessageOut = ("Overlay "
                           + AString::number(overlayIndex)
                           + " not found");
        return false;
    }
    
    int32_t selectedMapIndex(-1);
    std::vector<CaretMappableDataFile*> mapFiles;
    CaretMappableDataFile* selectedMapFile(NULL);
    overlay->getSelectionData(mapFiles,
                              selectedMapFile,
                              selectedMapIndex);
    
    if (mapFiles.empty()) {
        errorMessageOut = "The overlay does not contain any files";
        return false;
    }
    
    selectedMapFileOut = selectedMapFile;
    for (const auto mf : mapFiles) {
        mapFileNamesOut.push_back(mf->getFileNameNoPath());
    }
    if (selectedMapFileOut != NULL) {
        const int32_t numMaps = selectedMapFileOut->getNumberOfMaps();
        for (int32_t i = 0; i < numMaps; i++) {
            selectedMapFileMapNamesOut.push_back(selectedMapFileOut->getMapName(i));
        }
    }
    
    return true;
}

/**
 * Get info for data in a custom parameter
 *
 * @param browserWindowIndex
 *     Index of browser window
 * @param macroCommand
 *     Macro command that contains the parameter
 * @param parameter
 *     Parameter for info
 * @param dataInfo
 *     Updated with data info in this method
 * @return
 *     True if the data info is valid
 */
bool
WbMacroCustomOperationManager::getCustomParameterDataInfo(const int32_t browserWindowIndex,
                                                          const WuQMacroCommand* macroCommand,
                                                          const WuQMacroCommandParameter* parameter,
                                                          WbMacroCustomDataInfo& dataInfoOut)
{
    CaretAssert(macroCommand);
    CaretAssert(parameter);
    
    const QString customTypeName(parameter->getCustomDataType());
    bool customTypeNameValid(false);
    const WbMacroCustomDataTypeEnum::Enum userType = WbMacroCustomDataTypeEnum::fromName(customTypeName,
                                                                                         &customTypeNameValid);
    if ( ! customTypeNameValid) {
        CaretLogSevere("\""
                       + customTypeName
                       + "\" is not a valid name for a custom parameter");
        return false;
    }
    
    const QString dataTypeName(WuQMacroDataValueTypeEnum::toName(dataInfoOut.getDataType()));
    
    bool unsupportedFlag(false);
    switch (dataInfoOut.getDataType()) {
        case WuQMacroDataValueTypeEnum::AXIS:
            unsupportedFlag = true;
            break;
        case WuQMacroDataValueTypeEnum::BOOLEAN:
            unsupportedFlag = true;
            break;
        case WuQMacroDataValueTypeEnum::FLOAT:
            break;
        case WuQMacroDataValueTypeEnum::INTEGER:
            break;
        case WuQMacroDataValueTypeEnum::INVALID:
            unsupportedFlag = true;
            break;
        case WuQMacroDataValueTypeEnum::MOUSE:
            unsupportedFlag = true;
            break;
        case WuQMacroDataValueTypeEnum::NONE:
            unsupportedFlag = true;
            break;
        case WuQMacroDataValueTypeEnum::STRING:
            unsupportedFlag = true;
            break;
        case WuQMacroDataValueTypeEnum::STRING_LIST:
            break;
    }
    
    if (unsupportedFlag) {
        CaretLogSevere("Unsupported data type for parameter info "
                        + WuQMacroDataValueTypeEnum::toName(dataInfoOut.getDataType()));
        return false;
    }
    
    bool validFlag(false);
    QString errorMessage("Unknown error");
    QString invalidTypeName;
    switch (userType) {
        case WbMacroCustomDataTypeEnum::OVERLAY_INDEX:
            if (dataInfoOut.getDataType() == WuQMacroDataValueTypeEnum::INTEGER) {
                dataInfoOut.setIntegerRange({ 1, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS });
                validFlag = true;
            }
            else {
                invalidTypeName = WuQMacroDataValueTypeEnum::toName(WuQMacroDataValueTypeEnum::INTEGER);
            }
            break;
        case WbMacroCustomDataTypeEnum::OVERLAY_FILE_NAME_OR_FILE_INDEX:
            if (dataInfoOut.getDataType() == WuQMacroDataValueTypeEnum::STRING_LIST) {
                std::vector<QString> filenames;
                std::vector<QString> mapNames;
                QString selectedFileName;
                QString selectedMapName;
                if (getMappableFilesSelection(macroCommand,
                                              parameter,
                                              NULL,
                                              filenames,
                                              selectedFileName,
                                              mapNames,
                                              selectedMapName,
                                              errorMessage)) {
                    dataInfoOut.setStringListValues(filenames);
                    validFlag = true;
                }
            }
            else {
                invalidTypeName = WuQMacroDataValueTypeEnum::toName(WuQMacroDataValueTypeEnum::STRING_LIST);
            }
            break;
        case WbMacroCustomDataTypeEnum::OVERLAY_MAP_NAME_OR_MAP_INDEX:
            if (dataInfoOut.getDataType() == WuQMacroDataValueTypeEnum::STRING_LIST) {
                std::vector<QString> filenames;
                std::vector<QString> mapNames;
                QString selectedFileName;
                QString selectedMapName;
                if (getMappableFilesSelection(macroCommand,
                                              NULL,
                                              parameter,
                                              filenames,
                                              selectedFileName,
                                              mapNames,
                                              selectedMapName,
                                              errorMessage)) {
                    dataInfoOut.setStringListValues(mapNames);
                    validFlag = true;
                }
            }
            else {
                invalidTypeName = WuQMacroDataValueTypeEnum::toName(WuQMacroDataValueTypeEnum::STRING_LIST);
            }
            break;
        case WbMacroCustomDataTypeEnum::SURFACE:
            if (dataInfoOut.getDataType() == WuQMacroDataValueTypeEnum::STRING_LIST) {
                std::vector<QString> surfaceNames;
                if (getSurfaceNames(surfaceNames, errorMessage)) {
                    dataInfoOut.setStringListValues(surfaceNames);
                    validFlag = true;
                }
            }
            else {
                invalidTypeName = WuQMacroDataValueTypeEnum::toName(WuQMacroDataValueTypeEnum::STRING_LIST);
            }
            break;
    }
    
    if ( ! validFlag) {
        if ( ! invalidTypeName.isEmpty()) {
            errorMessage = ("Custom parameter "
                            + customTypeName
                            + " data type should be "
                            + invalidTypeName
                            + " but is "
                            + dataTypeName);
        }
        
        CaretLogSevere(errorMessage);
    }
    
    return validFlag;
}

/**
 * Run a custom-defined macro command
 *
 * @param parent
 *     Parent widget for any dialogs
 * @param executorMonitor
 *     The executor monitor
 * @param executorOptions
 *     Options for the executor
 * @param customMacroCommand
 *     Custom macro command to run
 * @param errorMessageOut
 *     Contains any error information or empty if no error
 * @return
 *     True if command executed successfully, else false
 */
bool
WbMacroCustomOperationManager::executeCustomOperationMacroCommand(QWidget* parent,
                                                                  const WuQMacroExecutorMonitor* executorMonitor,
                                                                  const WuQMacroExecutorOptions* executorOptions,
                                                                  const WuQMacroCommand* customMacroCommand,
                                                                  QString& errorMessageOut)
{
    CaretAssert(parent);
    CaretAssert(customMacroCommand);
    errorMessageOut.clear();
    
    if (customMacroCommand->getCommandType() != WuQMacroCommandTypeEnum::CUSTOM_OPERATION) {
        errorMessageOut = "Requesting non-custom command execution in user-executor";
        return false;
    }
    
    const QString customCommandName(customMacroCommand->getCustomOperationTypeName());
    bool nameValid(false);
    const WbMacroCustomOperationTypeEnum::Enum commandType = WbMacroCustomOperationTypeEnum::fromName(customCommandName,
                                                                                                      &nameValid);
    if ( ! nameValid) {
        errorMessageOut = ("\""
                           + customCommandName
                           + "\" is not a valid name for a custom macro command");
        return false;
    }
    
    std::unique_ptr<WbMacroCustomOperationBase> customOperation;
    customOperation.reset(createCommand(commandType));
    
    bool successFlag(false);
    if (customOperation) {
        successFlag = customOperation->executeCommand(parent,
                                                      executorMonitor,
                                                      executorOptions,
                                                      customMacroCommand);
        if ( ! successFlag) {
            errorMessageOut = customOperation->getErrorMessage();
        }
    }
    else {
        errorMessageOut = "Custom Operation is missing";
        CaretLogSevere(errorMessageOut);
    }
    
    return successFlag;
}

/**
 * @return Names of custom operation defined macro commands
 */
std::vector<QString>
WbMacroCustomOperationManager::getNamesOfCustomOperationMacroCommands()
{
    std::vector<AString> names;
    
    WbMacroCustomOperationTypeEnum::getAllNames(names,
                                                true);
    
    std::vector<QString> namesOut(names.begin(),
                                  names.end());
    return namesOut;
}

/**
 * @return All custom operation commands.  Caller is responsible for deleting
 * all content of the returned vector.
 */
std::vector<WuQMacroCommand*>
WbMacroCustomOperationManager::getAllCustomOperationMacroCommands()
{
    std::vector<WbMacroCustomOperationTypeEnum::Enum> customCommandTypes;
    WbMacroCustomOperationTypeEnum::getAllEnums(customCommandTypes);
    
    std::vector<WuQMacroCommand*> customCommands;
    for (auto cct : customCommandTypes) {
        if (cct == WbMacroCustomOperationTypeEnum::VOLUME_TO_SURFACE_CROSS_FADE) {
            continue;
        }
        std::unique_ptr<WbMacroCustomOperationBase> customOperation(createCommand(cct));
        customCommands.push_back(customOperation->createCommand());
    }
    
    return customCommands;
}

/**
 * Get a new instance of a custom operation for the given macro command name
 *
 * @param customMacroCommandName
 *     Name of custom macro command
 * @param errorMessageOut
 *     Contains any error information or empty if no error
 * @return
 *     Pointer to command or NULL if not valid
 */
WuQMacroCommand*
WbMacroCustomOperationManager::newInstanceOfCustomOperationMacroCommand(const QString& customMacroCommandName,
                                                                        QString& errorMessageOut)
{
    errorMessageOut.clear();
    
    bool nameValid(false);
    const WbMacroCustomOperationTypeEnum::Enum commandType = WbMacroCustomOperationTypeEnum::fromName(customMacroCommandName,
                                                                                                      &nameValid);
    if ( ! nameValid) {
        errorMessageOut = ("\""
                           + customMacroCommandName
                           + "\" is not a valid name for a custom macro command");
        return NULL;
    }
    
    WuQMacroCommand* command(NULL);
    
    std::unique_ptr<WbMacroCustomOperationBase> customOperation;
    customOperation.reset(createCommand(commandType));

    if (customOperation) {
        command = customOperation->createCommand();
        if (command == NULL) {
            errorMessageOut = customOperation->getErrorMessage();
        }
    }
    else {
        errorMessageOut = "Custom Operation is missing";
        CaretLogSevere(errorMessageOut);
    }
    
    return command;
}

/**
 * Create a custom operation of the given type
 *
 * @param operationType
 *     The operation type
 * @return
 *     New instance of command caller is responsible for destroying
 */
WbMacroCustomOperationBase*
WbMacroCustomOperationManager::createCommand(const WbMacroCustomOperationTypeEnum::Enum operationType)
{
    WbMacroCustomOperationBase* operationOut(NULL);

    switch (operationType) {
        case WbMacroCustomOperationTypeEnum::DELAY:
            operationOut = new WbMacroCustomOperationDelay();
            break;
        case WbMacroCustomOperationTypeEnum::MODEL_ROTATION:
            operationOut = new WbMacroCustomOperationModelRotation();
            break;
        case WbMacroCustomOperationTypeEnum::OVERLAY_CROSS_FADE:
            operationOut = new WbMacroCustomOperationOverlayCrossFade();
            break;
        case WbMacroCustomOperationTypeEnum::SURFACE_INTERPOLATION:
            operationOut = new WbMacroCustomOperationSurfaceInterpolation();
            break;
        case WbMacroCustomOperationTypeEnum::VOLUME_SLICE_INCREMENT:
            operationOut = new WbMacroCustomOperationVolumeSliceIncrement();
            break;
        case WbMacroCustomOperationTypeEnum::VOLUME_TO_SURFACE_CROSS_FADE:
            operationOut = new WbMacroCustomOperationVolumeToSurfaceCrossFade();
            break;
    }

    CaretAssert(operationOut);
    return operationOut;
}

/**
 * Get the active tab content in the active window.  If there is more
 * than one window open, the user is prompted to select a window
 *
 * @param browserWindowIndex
 *     Widget for any dialogs
 * @param errorMessageOut
 *     Output with error information if failure
 * @return
 *     Pointer to active tab content or none found
 */
BrowserTabContent*
WbMacroCustomOperationManager::getTabContent(const int32_t browserWindowIndex,
                                             QString& errorMessageOut)
{
    errorMessageOut.clear();
    
    const std::vector<BrainBrowserWindow*> allWindows = GuiManager::get()->getAllOpenBrainBrowserWindows();
    if (allWindows.empty()) {
        errorMessageOut = "No window are open.  This should never happen.";
        return NULL;
    }
    
    BrowserTabContent* tabContent(NULL);
    BrainBrowserWindow* bbw(NULL);
    
    if (allWindows.size() == 1) {
        CaretAssertVectorIndex(allWindows, 0);
        bbw = allWindows[0];
    }
    else {
        for (auto w : allWindows) {
            if (w->getBrowserWindowIndex() == browserWindowIndex) {
                bbw = w;
                break;
            }
        }
    }
    
    if (bbw != NULL) {
        tabContent = bbw->getBrowserTabContent();
    }
    else {
        errorMessageOut = "Failed to find window selected by user";
    }
    
    return tabContent;
}
