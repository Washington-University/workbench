
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
#include "WbMacroCustomOperationSurfaceInterpolation.h"
#include "WbMacroCustomOperationTypeEnum.h"
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
    
    bool successFlag(false);
    switch (commandType) {
        case WbMacroCustomOperationTypeEnum::SURFACE_INTERPOLATION:
            successFlag = WbMacroCustomOperationSurfaceInterpolation::executeCommand(parent,
                                                                                     customMacroCommand,
                                                                                     errorMessageOut);
            break;
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
    
    switch (commandType) {
        case WbMacroCustomOperationTypeEnum::SURFACE_INTERPOLATION:
            command = WbMacroCustomOperationSurfaceInterpolation::createCommand(errorMessageOut);
            break;
    }
    
    return command;
}

