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

#include "CommandOperation.h"

using namespace caret;

/**
 * Constructor.
 * @param commandLineSwitch
 *   Switch to select this command.
 * @param operationShortDescription
 *   Short description of the command.
 */
CommandOperation::CommandOperation(const std::string& commandLineSwitch,
                                   const std::string& operationShortDescription)
: CaretObject()
{
    
}
 
/**
 * Destructor.
 */
CommandOperation::~CommandOperation()
{
    
}


/**
 * Get the short description of the operation.
 */
std::string 
CommandOperation::getOperationShortDescription() const
{
    return this->operationShortDescription;
}

/**
 * Get the command line switch for selecting the operation.
 */
std::string 
CommandOperation::getCommandLineSwitch() const
{
    return this->commandLineSwitch;
}

/**
 * Set the parameters for the operation.
 * @param parameters
 *    Parameters for the operation.
 */
void 
CommandOperation::setParameters(ProgramParameters* parameters)
{
    this->parameters = parameters;
}
