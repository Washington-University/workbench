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

#include <fstream>
#include <ostream>
#include <iostream>

#include "CommandSurfaceInformation.h"
#include "FileInformation.h"
#include "SurfaceFile.h"
#include "ProgramParameters.h"

using namespace caret;

/**
 * Constructor.
 */
CommandSurfaceInformation::CommandSurfaceInformation()
: CommandOperation("-surface-information",
                         "Print information about a surface")
{
    
}

/**
 * Destructor.
 */
CommandSurfaceInformation::~CommandSurfaceInformation()
{
    
}

AString
CommandSurfaceInformation::getHelpInformation(const AString& /*programName*/)
{
    AString helpInfo = ("\n"
                        "Print information about a surface.\n"
                        "\n"
                        "Usage:  <surface-file-name>  \n"
                        "    \n"
                        );
    return helpInfo;
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
CommandSurfaceInformation::executeOperation(ProgramParameters& parameters) throw (CommandException,
                                                               ProgramParametersException)
{
    const AString surfaceFileName  = parameters.nextString("Surface File Name");
    
    SurfaceFile surfaceFile;
    surfaceFile.readFile(surfaceFileName);
    
    std::cout << qPrintable(surfaceFile.getInformation()) << std::endl;
}