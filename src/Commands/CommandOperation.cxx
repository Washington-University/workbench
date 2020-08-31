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

#include "CommandOperation.h"

#include "CaretAssert.h"

using namespace caret;

/**
 * Constructor.
 * @param commandLineSwitch
 *   Switch to select this command.
 * @param operationShortDescription
 *   Short description of the command.
 */
CommandOperation::CommandOperation(const AString& commandLineSwitch,
                                   const AString& operationShortDescription)
: CaretObject()
{
    this->commandLineSwitch = commandLineSwitch;
    this->operationShortDescription = operationShortDescription;
    CaretAssert(commandLineSwitch != "-command-switch");//catch failure to change things in the command template
    for (int i = 0; i < commandLineSwitch.length(); ++i)//release build should optimize out empty loops
    {
        CaretAssert(commandLineSwitch[i].unicode() < 128);
    }
}
 
/**
 * Destructor.
 */
CommandOperation::~CommandOperation()
{
    
}

/**
 * Execute the command.
 * 
 * @param parameters
 *   Parameters for the operation.
 * @throws CommandException
 *   If the command failed.
 */
void 
CommandOperation::execute(ProgramParameters& parameters, const bool& preventProvenance)
{
    if (preventProvenance)
    {
        disableProvenance();//let provenance-ignorant commands not need to deal with an unused parameter
    }
    this->executeOperation(parameters);
}

void CommandOperation::disableProvenance()
{
}

void CommandOperation::setCiftiOutputDTypeAndScale(const int16_t&, const double&, const double&)
{
}

void CommandOperation::setCiftiOutputDTypeNoScale(const int16_t&)
{
}

void CommandOperation::setVolumeOutputDTypeAndScale(const int16_t&, const double&, const double&)
{
}

void CommandOperation::setVolumeOutputDTypeNoScale(const int16_t&)
{
}

AString CommandOperation::doCompletion(ProgramParameters&, const bool&)
{
    return "";
}

bool CommandOperation::takesParameters()
{
    return true;
}

void CommandOperation::setCiftiReadMemory(const bool)
{
}

/**
 * Get the short description of the operation.
 */
AString
CommandOperation::getOperationShortDescription() const
{
    return this->operationShortDescription;
}

/**
 * Get the command line switch for selecting the operation.
 */
AString
CommandOperation::getCommandLineSwitch() const
{
    return this->commandLineSwitch;
}

