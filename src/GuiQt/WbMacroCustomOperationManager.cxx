
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
 * Edit overlay index
 *
 * @param parentWidget
 *     Parent widget for any dialogs
 * @param parameter
 *     Parameter for editing
 * @return
 *     True if the parameter was modified
 */
bool
WbMacroCustomOperationManager::editOverlayIndex(QWidget* parentWidget,
                                                WuQMacroCommandParameter* parameter)
{
    bool okFlag(false);
    const int32_t currentValue(parameter->getValue().toInt());
    const int32_t minimumValue(1);
    const int32_t maximumValue(BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS);
    const int32_t stepValue(1);
    int32_t index = QInputDialog::getInt(parentWidget,
                                         "Overlay Index",
                                         "Overlay Index (1 is at top)",
                                         currentValue,
                                         minimumValue,
                                         maximumValue,
                                         stepValue,
                                         &okFlag);
    if (okFlag) {
        parameter->setValue(index);
    }
    return okFlag;
}

/**
 * Edit screen axis
 *
 * @param parentWidget
 *     Parent widget for any dialogs
 * @param parameter
 *     Parameter for editing
 * @return
 *     True if the parameter was modified
 */
bool
WbMacroCustomOperationManager::editScreenAxis(QWidget* parentWidget,
                                              WuQMacroCommandParameter* parameter)
{
    QStringList axisList;
    axisList.append("X");
    axisList.append("Y");
    axisList.append("Z");
    
    bool okFlag(false);
    const bool editableFlag(false);
    QString selectedName = QInputDialog::getItem(parentWidget,
                                                 "Screen Axis",
                                                 parameter->getName(),
                                                 axisList,
                                                 0,
                                                 editableFlag,
                                                 &okFlag);
    if (okFlag) {
        parameter->setValue(selectedName);
    }
    return okFlag;
}

/**
 * Edit surface selection
 *
 * @param parentWidget
 *     Parent widget for any dialogs
 * @param parameter
 *     Parameter for editing
 * @param errorMessageOut
 *     Output error message if modifying parameter fails
 * @return
 *     True if the parameter was modified
 */
bool
WbMacroCustomOperationManager::editSurface(QWidget* parentWidget,
                                           WuQMacroCommandParameter* parameter,
                                           QString& errorMessageOut)
{
    EventSurfacesGet surfacesEvent;
    EventManager::get()->sendEvent(surfacesEvent.getPointer());
    std::vector<Surface*> surfaces = surfacesEvent.getSurfaces();
    bool okFlag(false);
    if (surfaces.empty()) {
        errorMessageOut = "There are no surfaces available";
    }
    else {
        QStringList surfaceNamesList;
        for (const auto s : surfaces) {
            surfaceNamesList.append(s->getFileNameNoPath());
        }
        
        const bool editableFlag(false);
        QString selectedName = QInputDialog::getItem(parentWidget,
                                                     "Choose Surface",
                                                     parameter->getName(),
                                                     surfaceNamesList,
                                                     0,
                                                     editableFlag,
                                                     &okFlag);
        if (okFlag) {
            parameter->setValue(selectedName);
        }
    }
    
    return okFlag;
}

/**
 * Get the map file in the overlay for this command
 *
 * @param parentWidget
 *     Parent widget for any dialogs
 * @param macroCommand
 *     Macro command containing parameter being edited
 * @param mapFilesOut
 *     Map files in the overlay
 * @param selectedMapFileOut
 *     Selected map file in the overlay
 * @param errorMessageOut
 *     Output error message if finding map files fails
 * @return
 *     True if the parameter was modified
 */
bool
WbMacroCustomOperationManager::getMapFilesInOverlay(QWidget* parentWidget,
                                                    WuQMacroCommand* macroCommand,
                                                    std::vector<CaretMappableDataFile*>& mapFilesOut,
                                                    CaretMappableDataFile* &selectedMapFileOut,
                                                    QString& errorMessageOut)
{
    mapFilesOut.clear();
    selectedMapFileOut = NULL;
    
    /*
     * Find the overlay index parameter that should be before this parameter
     */
    int32_t overlayIndex(-1);
    const int32_t numParams = macroCommand->getNumberOfParameters();
    for (int32_t ip = 0; ip < numParams; ip++) {
        const WuQMacroCommandParameter* p = macroCommand->getParameterAtIndex(ip);
        if (p->getDataType() == WuQMacroDataValueTypeEnum::CUSTOM_DATA) {
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
     * Overlay index is 1..N for user
     */
    if (overlayIndex < 1) {
        errorMessageOut = "Unable to find overlay index";
        return false;
    }
    --overlayIndex;
    
    BrowserTabContent* tabContent = getTabContent(parentWidget,
                                                  "Window containing overlay",
                                                  errorMessageOut);
    if (tabContent == NULL) {
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
    overlay->getSelectionData(mapFilesOut,
                              selectedMapFileOut,
                              selectedMapIndex);
    
    if (mapFilesOut.empty()) {
        errorMessageOut = "The overlay does not contain any files";
        return false;
    }
    return true;
}

/**
 * Edit overlay file
 *
 * @param parentWidget
 *     Parent widget for any dialogs
 * @param macroCommand
 *     Macro command containing parameter being edited
 * @param parameter
 *     Parameter for editing
 * @param errorMessageOut
 *     Output error message if modifying parameter fails
 * @return
 *     True if the parameter was modified
 */
bool
WbMacroCustomOperationManager::editOverlayFile(QWidget* parentWidget,
                                               WuQMacroCommand* macroCommand,
                                               WuQMacroCommandParameter* parameter,
                                               QString& errorMessageOut)
{
    std::vector<CaretMappableDataFile*> mapFiles;
    CaretMappableDataFile* selectedMapFile(NULL);
    if ( ! getMapFilesInOverlay(parentWidget,
                                macroCommand,
                                mapFiles,
                                selectedMapFile,
                                errorMessageOut)) {
        return false;
    }
    
    
    const QString selectedFileName = parameter->getValue().toString();
    int32_t selectedIndex(0);
    QStringList fileNameList;
    for (const auto mf : mapFiles) {
        const QString name(mf->getFileNameNoPath());
        if (selectedFileName == name) {
            selectedIndex = fileNameList.count();
        }
        fileNameList.append(name);
    }
    
    bool okFlag(false);
    const bool editableFlag(false);
    QString selectedName = QInputDialog::getItem(parentWidget,
                                                 "Choose map file",
                                                 "Choose map file",
                                                 fileNameList,
                                                 selectedIndex,
                                                 editableFlag,
                                                 &okFlag);
    if (okFlag) {
        parameter->setValue(selectedName);
        return true;
    }

    return false;
}

/**
 * Edit overlay file
 *
 * @param parentWidget
 *     Parent widget for any dialogs
 * @param macroCommand
 *     Macro command containing parameter being edited
 * @param parameter
 *     Parameter for editing
 * @param errorMessageOut
 *     Output error message if modifying parameter fails
 * @return
 *     True if the parameter was modified
 */
bool
WbMacroCustomOperationManager::editOverlayMap(QWidget* parentWidget,
                                              WuQMacroCommand* macroCommand,
                                              WuQMacroCommandParameter* parameter,
                                              QString& errorMessageOut)
{
    /*
     * Find the overlay file parameter that should be before this parameter
     */
    QString filename;
    const int32_t numParams = macroCommand->getNumberOfParameters();
    for (int32_t ip = 0; ip < numParams; ip++) {
        const WuQMacroCommandParameter* p = macroCommand->getParameterAtIndex(ip);
        if (p->getDataType() == WuQMacroDataValueTypeEnum::CUSTOM_DATA) {
            bool valid(false);
            WbMacroCustomDataTypeEnum::Enum customType = WbMacroCustomDataTypeEnum::fromName(p->getCustomDataType(),
                                                                                             &valid);
            if (valid) {
                if (customType == WbMacroCustomDataTypeEnum::OVERLAY_FILE_NAME_OR_FILE_INDEX) {
                    filename = p->getValue().toString();
                    break;
                }
            }
        }
    }
    
    if (filename.isEmpty()) {
        errorMessageOut = "Name of overlay file is empty, select a file";
        return false;
    }
    
    EventCaretMappableDataFilesGet filesEvent;
    EventManager::get()->sendEvent(filesEvent.getPointer());
    std::vector<CaretMappableDataFile*> allMapFiles;
    filesEvent.getAllFiles(allMapFiles);
    
    CaretMappableDataFile* mapFile(NULL);
    for (auto cmdf : allMapFiles) {
        if (cmdf->getFileName().endsWith(filename)) {
            mapFile = cmdf;
            break;
        }
    }
    if (mapFile == NULL) {
        errorMessageOut = ("Unable to find data file with name \""
                           + filename
                           + "\"");
        return false;
    }
    
    const QString currentMapName = parameter->getValue().toString();
    
    const int32_t numMaps = mapFile->getNumberOfMaps();
    QStringList mapNameList;
    for (int32_t i = 0; i < numMaps; i++) {
        mapNameList.append(mapFile->getMapName(i));
    }
    
    bool okFlag(false);
    const bool editableFlag(false);
    QString selectedName = QInputDialog::getItem(parentWidget,
                                                 "Choose Map Name",
                                                 "Choose map name",
                                                 mapNameList,
                                                 0,
                                                 editableFlag,
                                                 &okFlag);
    if (okFlag) {
        parameter->setValue(selectedName);
        return true;
    }

    return false;
}

/**
 * Is called to edit a macro command parameter with a CUSTOM_DATA data type
 *
 * @param parentWidget
 *     Parent widget for any dialogs
 * @param macroCommand
 *     Macro command that contains the parameter for editing
 * @param parameter
 *     Parameter for editing
 * @return
 *     True if the parameter was modified
 */
bool
WbMacroCustomOperationManager::editCustomDataValueParameter(QWidget* parentWidget,
                                                            WuQMacroCommand* macroCommand,
                                                            WuQMacroCommandParameter* parameter)
{
    bool modFlag(false);
    
    CaretAssert(parentWidget);
    CaretAssert(parameter);
    
    if (parameter->getDataType() != WuQMacroDataValueTypeEnum::CUSTOM_DATA) {
        CaretLogSevere("Attempting to edit non-custom data type as custom data type");
        return false;
    }
    
    const QString typeName(parameter->getCustomDataType());
    bool nameValid(false);
    const WbMacroCustomDataTypeEnum::Enum userType = WbMacroCustomDataTypeEnum::fromName(typeName,
                                                                                         &nameValid);
    if ( ! nameValid) {
        CaretLogSevere("\""
                       + typeName
                       + "\" is not a valid name for a custom parameter");
        return false;
    }
    
    QString errorMessage;
    switch (userType) {
        case WbMacroCustomDataTypeEnum::OVERLAY_INDEX:
            modFlag = editOverlayIndex(parentWidget,
                                       parameter);
            break;
        case WbMacroCustomDataTypeEnum::OVERLAY_FILE_NAME_OR_FILE_INDEX:
            modFlag = editOverlayFile(parentWidget,
                                      macroCommand,
                                      parameter,
                                      errorMessage);
            break;
        case WbMacroCustomDataTypeEnum::OVERLAY_MAP_NAME_OR_MAP_INDEX:
            modFlag = editOverlayMap(parentWidget,
                                     macroCommand,
                                     parameter,
                                     errorMessage);
            break;
        case WbMacroCustomDataTypeEnum::SCREEN_AXIS:
            modFlag = editScreenAxis(parentWidget,
                                     parameter);
            break;
        case WbMacroCustomDataTypeEnum::SURFACE:
            modFlag = editSurface(parentWidget,
                                  parameter,
                                  errorMessage);
            break;
    }
    
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::critical(parentWidget,
                                "Set Value",
                                errorMessage);
    }
    
    return modFlag;

}

/**
 * Run a custom-defined macro command
 *
 * @param parent
 *     Parent widget for any dialogs
 * @param executorMonitor
 *     The executor monitor
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
 * @param parentWidget
 *     Widget for any dialogs
 * @param promptMessage
 *     Message displayed in dialog if more than one window is open
 * @param errorMessageOut
 *     Output with error information if failure
 * @return
 *     Pointer to active tab content or none found
 */
BrowserTabContent*
WbMacroCustomOperationManager::getTabContent(QWidget* parentWidget,
                                             const QString& promptMessage,
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
        QStringList windowNameList;
        for (const auto w : allWindows) {
            windowNameList.append(w->windowTitle());
        }
        
        bool okFlag(false);
        const bool editableFlag(false);
        QString selectedName = QInputDialog::getItem(parentWidget,
                                                     "Choose window",
                                                     promptMessage,
                                                     windowNameList,
                                                     0,
                                                     editableFlag,
                                                     &okFlag);
        if (okFlag) {
            for (auto w : allWindows) {
                if (selectedName == w->windowTitle()) {
                    bbw = w;
                    break;
                }
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

