#ifndef __WU_Q_MACRO_CUSTOM_OPERATION_MANAGER_INTERFACE_H__
#define __WU_Q_MACRO_CUSTOM_OPERATION_MANAGER_INTERFACE_H__

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



class QString;
class QWidget;

namespace caret {
    class WuQMacroCommand;
    class WuQMacroCommandParameter;
    class WuQMacroExecutorMonitor;
    class WuQMacroExecutorOptions;

    class WuQMacroCustomOperationManagerInterface {
        
    public:
        WuQMacroCustomOperationManagerInterface() { }
        
        virtual ~WuQMacroCustomOperationManagerInterface() { }
        
        WuQMacroCustomOperationManagerInterface(const WuQMacroCustomOperationManagerInterface&) = delete;

        WuQMacroCustomOperationManagerInterface& operator=(const WuQMacroCustomOperationManagerInterface&) = delete;
        
        /**
         * Is called to edit a macro command parameter with a CUSTOM_DATA data type
         *
         * @param parent
         *     Parent widget for any dialogs
         * @param macroCommand
         *     Macro command that contains the parameter for editing
         * @param parameter
         *     Parameter for editing
         * @return
         *     True if the parameter was modified
         */
        virtual bool editCustomDataValueParameter(QWidget* parent,
                                                  WuQMacroCommand* macroCommand,
                                                  WuQMacroCommandParameter* parameter) = 0;
        
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
        virtual bool executeCustomOperationMacroCommand(QWidget* parent,
                                                        const WuQMacroExecutorMonitor* executorMonitor,
                                                        const WuQMacroExecutorOptions* executorOptions,
                                                        const WuQMacroCommand* macroCommand,
                                                        QString& errorMessageOut) = 0;
        
        /**
         * @return Names of custom operation defined macro commands
         */
        virtual std::vector<QString> getNamesOfCustomOperationMacroCommands() = 0;
        
        /**
         * @return All custom operation commands.  Caller is responsible for deleting
         * all content of the returned vector.
         */
        virtual std::vector<WuQMacroCommand*> getAllCustomOperationMacroCommands() = 0;
        
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
        virtual WuQMacroCommand* newInstanceOfCustomOperationMacroCommand(const QString& customMacroCommandName,
                                                                          QString& errorMessageOut) = 0;

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_CUSTOM_OPERATION_MANAGER_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_CUSTOM_OPERATION_MANAGER_INTERFACE_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_CUSTOM_OPERATION_MANAGER_INTERFACE_H__
