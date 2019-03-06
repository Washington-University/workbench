
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

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "EventSurfacesGet.h"
#include "GuiManager.h"
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
 * Is called to edit a macro command parameter with a CUSTOM_DATA data type
 *
 * @param parent
 *     Parent widget for any dialogs
 * @param parameter
 *     Parameter for editing
 * @return
 *     True if the parameter was modified
 */
bool
WbMacroCustomOperationManager::editCustomDataValueParameter(QWidget* parent,
                                                            WuQMacroCommandParameter* parameter)
{
    bool modFlag(false);
    
    CaretAssert(parent);
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
        case WbMacroCustomDataTypeEnum::SCREEN_AXIS:
        {
            QStringList axisList;
            axisList.append("X");
            axisList.append("Y");
            axisList.append("Z");

            bool okFlag(false);
            QString selectedName = QInputDialog::getItem(parent,
                                                         "Choose Surface",
                                                         parameter->getName(),
                                                         axisList,
                                                         0,
                                                         false,
                                                         &okFlag);
            if (okFlag) {
                parameter->setValue(selectedName);
                modFlag = true;
            }
        }
            break;
        case WbMacroCustomDataTypeEnum::SURFACE:
        {
            EventSurfacesGet surfacesEvent;
            EventManager::get()->sendEvent(surfacesEvent.getPointer());
            std::vector<Surface*> surfaces = surfacesEvent.getSurfaces();
            if (surfaces.empty()) {
                errorMessage = "There are no surfaces available";
            }
            else {
                QStringList surfaceNamesList;
                for (const auto s : surfaces) {
                    surfaceNamesList.append(s->getFileNameNoPath());
                }
                
                bool okFlag(false);
                QString selectedName = QInputDialog::getItem(parent,
                                                             "Choose Surface",
                                                             parameter->getName(),
                                                             surfaceNamesList,
                                                             0,
                                                             false,
                                                             &okFlag);
                if (okFlag) {
                    parameter->setValue(selectedName);
                    modFlag = true;
                }
            }
        }
            break;
    }
    
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::critical(parent,
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
 * @param customMacroCommand
 *     Custom macro command to run
 * @param errorMessageOut
 *     Contains any error information or empty if no error
 * @return
 *     True if command executed successfully, else false
 */
bool
WbMacroCustomOperationManager::executeCustomOperationMacroCommand(QWidget* parent,
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

