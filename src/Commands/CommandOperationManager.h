#ifndef __COMMAND_OPERATION_MANAGER_H__
#define __COMMAND_OPERATION_MANAGER_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


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
        
        void runCommand(ProgramParameters& parameters) throw (CommandException);
        
        std::vector<CommandOperation*> getCommandOperations();
        
    private:
        CommandOperationManager();
        
        CommandOperationManager(const CommandOperationManager&);

        CommandOperationManager& operator=(const CommandOperationManager&);

    private:
        std::vector<CommandOperation*> commandOperations;
        
        static CommandOperationManager* singletonCommandOperationManager;
    };
    
#ifdef __COMMAND_OPERATION_MANAGER_DEFINE__
    CommandOperationManager* CommandOperationManager::singletonCommandOperationManager = NULL;
#endif // __COMMAND_OPERATION_MANAGER_DEFINE__
    
} // namespace

#endif // __COMMAND_OPERATION_MANAGER_H__
