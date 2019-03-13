
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

#define __WB_MACRO_CUSTOM_OPERATION_DELAY_DECLARE__
#include "WbMacroCustomOperationDelay.h"
#undef __WB_MACRO_CUSTOM_OPERATION_DELAY_DECLARE__

#include "CaretAssert.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"

using namespace caret;


    
/**
 * \class caret::WbMacroCustomOperationDelay
 * \brief Custom Macro Command for Delay
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WbMacroCustomOperationDelay::WbMacroCustomOperationDelay()
: WbMacroCustomOperationBase(WbMacroCustomOperationTypeEnum::DELAY)
{
}

/**
 * Destructor.
 */
WbMacroCustomOperationDelay::~WbMacroCustomOperationDelay()
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
WbMacroCustomOperationDelay::createCommand()
{
    const int32_t versionOne(1);
    
    /*
     * No parameters are needed as the base command's delay value is used
     */
    QString errorMessage;
    WuQMacroCommand* command = WuQMacroCommand::newInstanceCustomCommand(WbMacroCustomOperationTypeEnum::toName(WbMacroCustomOperationTypeEnum::DELAY),
                                                                         versionOne,
                                                                         "none",
                                                                         "Delay",
                                                                         "Delay for Seconds",
                                                                         5.0,
                                                                         errorMessage);
    
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
WbMacroCustomOperationDelay::executeCommand(QWidget* /*parent*/,
                                            const WuQMacroExecutorMonitor* /*executorMonitor*/,
                                            const WuQMacroExecutorOptions* /*executorOptions*/,
                                            const WuQMacroCommand* /*macroCommand*/)
{
    /*
     * Nothing to do since the base command's delay time performs the delay
     */
    return true;
}

