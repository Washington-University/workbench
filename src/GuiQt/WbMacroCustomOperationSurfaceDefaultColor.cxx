
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

#define __WB_MACRO_CUSTOM_OPERATION_SURFACE_DEFAULT_COLOR_DECLARE__
#include "WbMacroCustomOperationSurfaceDefaultColor.h"
#undef __WB_MACRO_CUSTOM_OPERATION_SURFACE_DEFAULT_COLOR_DECLARE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayPropertiesSurface.h"
#include "GuiManager.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"
#include "WuQMacroExecutorMonitor.h"

using namespace caret;


    
/**
 * \class caret::WbMacroCustomOperationSurfaceDefaultColor
 * \brief Macro custom operation for incremental rotation
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WbMacroCustomOperationSurfaceDefaultColor::WbMacroCustomOperationSurfaceDefaultColor()
: WbMacroCustomOperationBase(WbMacroCustomOperationTypeEnum::SURFACE_DEFAULT_COLOR)
{
    
}

/**
 * Destructor.
 */
WbMacroCustomOperationSurfaceDefaultColor::~WbMacroCustomOperationSurfaceDefaultColor()
{
}

/**
 * Get a new instance of the macro command
 *
 * @return
 *     Pointer to command or NULL if not valid
 *     Use getErrorMessage() for error information if NULL returned
 */
WuQMacroCommand*
WbMacroCustomOperationSurfaceDefaultColor::createCommand()
{
    const int32_t versionOne(1);
    
    QString errorMessage;
    WuQMacroCommand* command = WuQMacroCommand::newInstanceCustomCommand(WbMacroCustomOperationTypeEnum::toName(getOperationType()),
                                                                         versionOne,
                                                                         "none",
                                                                         WbMacroCustomOperationTypeEnum::toGuiName(getOperationType()),
                                                                         "Set the surface default vertex color (RGB)",
                                                                         1.0,
                                                                         errorMessage);
    if (command != NULL) {
        command->addParameter(WuQMacroDataValueTypeEnum::INTEGER,
                              "Red (0-255)",
                              "187");
        command->addParameter(WuQMacroDataValueTypeEnum::INTEGER,
                              "Green (0-255)",
                              "187");
        command->addParameter(WuQMacroDataValueTypeEnum::INTEGER,
                              "Blue (0-255)",
                              "187");
    }
    else {
        appendToErrorMessage(errorMessage);
    }
    
    return command;
}

/**
 * Execute the macro command
 *
 * @param parent
 *     Parent widget for any dialogs
 * @param executorMonitor
 *     the macro executor monitor
 * @param executorOptions
 *     Options for executor
 * @param macroCommand
 *     macro command to run
 * @return
 *     True if command executed successfully, else false
 *     Use getErrorMessage() for error information if false returned
 */
bool
WbMacroCustomOperationSurfaceDefaultColor::executeCommand(QWidget* parent,
                                                          const WuQMacroExecutorMonitor* /*executorMonitor*/,
                                                          const WuQMacroExecutorOptions* /*executorOptions*/,
                                                          const WuQMacroCommand* macroCommand)
{
    CaretAssert(parent);
    CaretAssert(macroCommand);
    
    if ( ! validateCorrectNumberOfParameters(macroCommand, 3)) {
        return false;
    }
    
    const int32_t redInt(macroCommand->getParameterAtIndex(0)->getValue().toInt());
    const int32_t greenInt(macroCommand->getParameterAtIndex(1)->getValue().toInt());
    const int32_t blueInt(macroCommand->getParameterAtIndex(2)->getValue().toInt());

    if ((redInt < 0)
        || (redInt > 255)) {
        appendToErrorMessage("Red must be in range [0, 255]");
        return false;
    }
    if ((greenInt < 0)
        || (greenInt > 255)) {
        appendToErrorMessage("Red must be in range [0, 255]");
        return false;
    }
    if ((blueInt < 0)
        || (blueInt > 255)) {
        appendToErrorMessage("Red must be in range [0, 255]");
        return false;
    }
    
    std::array<uint8_t, 3> rgb {
        static_cast<uint8_t>(redInt),
        static_cast<uint8_t>(greenInt),
        static_cast<uint8_t>(blueInt),
    };
    
    DisplayPropertiesSurface* dsp = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
    dsp->setDefaultColorRGB(rgb);

    updateSurfaceColoring();
    updateGraphics();
    updateUserInterface();
    
    return true;
}

