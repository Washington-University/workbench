
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

#define __WB_MACRO_CUSTOM_OPERATION_SURFACE_INTERPOLATION_DECLARE__
#include "WbMacroCustomOperationSurfaceInterpolation.h"
#undef __WB_MACRO_CUSTOM_OPERATION_SURFACE_INTERPOLATION_DECLARE__

#include "CaretAssert.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"

using namespace caret;


    
/**
 * \class caret::WbMacroCustomOperationSurfaceInterpolation 
 * \brief Custom Macro Command for Surface Interpolation
 * \ingroup GuiQt
 */

/**
 * Run the command
 *
 * @param parent
 *     Parent widget for any dialogs
 * @param macroCommand
 *     macro command to run
 * @param errorMessageOut
 *     Contains any error information or empty if no error
 * @return
 *     True if command executed successfully, else false
 */
bool
WbMacroCustomOperationSurfaceInterpolation::executeCommand(QWidget* parent,
                                                           const WuQMacroCommand* macroCommand,
                                                           QString& errorMessageOut)
{
    bool successFlag(false);
    
    return successFlag;
}

/**
 * Get a new instance of the command
 *
 * @param errorMessageOut
 *     Contains any error information or empty if no error
 * @return
 *     Pointer to command or NULL if not valid
 */
WuQMacroCommand*
WbMacroCustomOperationSurfaceInterpolation::createCommand(QString& errorMessageOut)
{
    errorMessageOut.clear();
    
    const int32_t versionOne(1);
    
    WuQMacroCommandParameter* paramSurfaceOne = new WuQMacroCommandParameter(WuQMacroDataValueTypeEnum::CUSTOM_DATA,
                                                                             "Starting Surface",
                                                                             "");
    paramSurfaceOne->setCustomDataType(WbMacroCustomDataTypeEnum::toName(WbMacroCustomDataTypeEnum::SURFACE));
    
    WuQMacroCommandParameter* paramSurfaceTwo = new WuQMacroCommandParameter(WuQMacroDataValueTypeEnum::CUSTOM_DATA,
                                                                             "Ending Surface",
                                                                             "");
    paramSurfaceTwo->setCustomDataType(WbMacroCustomDataTypeEnum::toName(WbMacroCustomDataTypeEnum::SURFACE));
    
    WuQMacroCommand* command = WuQMacroCommand::newInstanceCustomCommand("SurfaceInterpolation",
                                                                         versionOne,
                                                                         "none",
                                                                         "Surface Interpolation",
                                                                         "Interpolate Between Two Surface",
                                                                         1.0,
                                                                         errorMessageOut);
    if (command != NULL) {
        command->addParameter(paramSurfaceOne);
        command->addParameter(paramSurfaceTwo);
        command->addParameter(WuQMacroDataValueTypeEnum::FLOAT,
                              "Duration (secs)",
                              (float)5.0);
    }
    
    return command;
}

/**
 * Destructor.
 */
WbMacroCustomOperationSurfaceInterpolation::~WbMacroCustomOperationSurfaceInterpolation()
{
}

