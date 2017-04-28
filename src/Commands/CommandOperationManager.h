#ifndef __COMMAND_OPERATION_MANAGER_H__
#define __COMMAND_OPERATION_MANAGER_H__

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


#include <vector>

#include "CaretObject.h"
#include "CommandException.h"

namespace caret {

    class CommandOperation;
    class ProgramParameters;
    
    /// Manages all command operations.
    class CommandOperationManager : public CaretObject {
        
    public:
        static CommandOperationManager* getCommandOperationManager();
        
        static void deleteCommandOperationManager();
        
        ~CommandOperationManager();
        
        void runCommand(ProgramParameters& parameters);
        
        AString doCompletion(ProgramParameters& parameters, const bool& useExtGlob);
        
        std::vector<CommandOperation*> getCommandOperations();
        
    private:
        CommandOperationManager();
        
        CommandOperationManager(const CommandOperationManager&);

        CommandOperationManager& operator=(const CommandOperationManager&);

        void printAllCommands();
        
        void printDeprecatedCommands();
        
        void printAllCommandsMatching(const AString& partialSwitch);
        
        void printAllCommandsHelpInfo(const AString& programName);
        
        void printHelpInfo();
        
        void printArgumentsHelp(const AString& programName);
        
        void printGlobalOptions();
        
        void printCiftiHelp();
        
        void printGiftiHelp();
        
        void printParallelHelp(const AString& programName);
        
        void printVersionInfo();
        
        bool getGlobalOption(ProgramParameters& parameters, const AString& optionString, const int& numArgs, std::vector<AString>& arguments);
        
        struct OptionInfo
        {
            bool specified;
            bool complete;
            int index;//only valid when complete is false, might not be needed
            OptionInfo() { specified = false; complete = false; index = -1; }
        };
        
        OptionInfo parseGlobalOption(ProgramParameters& parameters, const AString& optionString, const int& numArgs, std::vector<AString>& arguments, const bool& quiet);
        
        static AString fixUnicode(const AString& input, const bool& quiet);
        
    private:
        std::vector<CommandOperation*> commandOperations, deprecatedOperations;
        
        static CommandOperationManager* singletonCommandOperationManager;
    };
    
#ifdef __COMMAND_OPERATION_MANAGER_DEFINE__
    CommandOperationManager* CommandOperationManager::singletonCommandOperationManager = NULL;
#endif // __COMMAND_OPERATION_MANAGER_DEFINE__
    
} // namespace

#endif // __COMMAND_OPERATION_MANAGER_H__
