#ifndef __COMMAND_OPERATION_H__
#define __COMMAND_OPERATION_H__

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


#include "CaretObject.h"
#include "CommandException.h"
#include "ProgramParametersException.h"

namespace caret {

    class ProgramParameters;
    
    /**
     * Abstract class for a command operation.
     */
    class CommandOperation : public CaretObject {

    public:
        virtual ~CommandOperation();
        
        void execute(ProgramParameters& parameters) 
            throw (CommandException);
        
    protected:
        /**
         * Execute the operation.
         * 
         * @param parameters
         *   Parameters for the operation.
         * @throws CommandException
         *   If the command failed.
         * @throws ProgramParametersException
         *   If there is an error in the parameters.
         */
        virtual void executeOperation(ProgramParameters& parameters) 
           throw (CommandException,
                ProgramParametersException) = 0;
        
        CommandOperation(const QString& commandLineSwitch,
                         const QString& operationShortDescription);
        
    private:
        CommandOperation();
        
        CommandOperation(const CommandOperation&);

        CommandOperation& operator=(const CommandOperation&);
        
    public:
        
        QString getOperationShortDescription() const;
        
        QString getCommandLineSwitch() const;
        
    private:
        /** Short description listing commands purpose */
        QString operationShortDescription;
        
        /** Switch on command line */
        QString commandLineSwitch;
    };
    
} // namespace

#endif // __COMMAND_OPERATION_H__
