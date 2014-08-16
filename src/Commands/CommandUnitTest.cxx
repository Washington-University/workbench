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

#include <fstream>
#include <ostream>
#include <iostream>

#include "CaretAssertion.h"
#include "CommandUnitTest.h"
#include "Plane.h"
#include "SystemUtilities.h"

using namespace caret;

/**
 * Constructor.
 */
CommandUnitTest::CommandUnitTest()
: CommandOperation("-unit-test",
                   "UNIT TESTING")
{
    
}

/**
 * Destructor.
 */
CommandUnitTest::~CommandUnitTest()
{
    
}

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
void 
CommandUnitTest::executeOperation(ProgramParameters& /*parameters*/) throw (CommandException,
                                                               ProgramParametersException)
{
    std::ostream* stream = &std::cout;
    
    CaretAssertion::unitTest(*stream, true);
    *stream << std::endl;
    Plane::unitTest(*stream, true);
    *stream << std::endl;
    SystemUtilities::unitTest(*stream, true);
    *stream << std::endl;
}
