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

#include "CommandSurfaceMatch.h"
#include "FileInformation.h"
#include "SurfaceFile.h"
#include "ProgramParameters.h"

using namespace caret;

/**
 * Constructor.
 */
CommandSurfaceMatch::CommandSurfaceMatch()
: CommandOperation("-surface-match",
                         "Match surface to bounding box of another surface")
{
    
}

/**
 * Destructor.
 */
CommandSurfaceMatch::~CommandSurfaceMatch()
{
    
}

AString
CommandSurfaceMatch::getHelpInformation(const AString& /*programName*/)
{
    AString helpInfo = ("\n"
                        "Match a surface to fit the bounding box of another surface.\n"
                        "\n"
                        "Usage:  <match-surface-file-name> \\ \n"
                        "        <input-surface-file-name> \\ \n"
                        "        <output-surface-file-name>\n"
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
CommandSurfaceMatch::executeOperation(ProgramParameters& parameters) throw (CommandException,
                                                               ProgramParametersException)
{
    try {
        const AString matchFileName  = parameters.nextString("Match Surface File Name");
        const AString inputFileName  = parameters.nextString("Input Surface File Name");
        const AString outputFileName = parameters.nextString("Output Surface File Name");
        
        SurfaceFile matchSurfaceFile;
        matchSurfaceFile.readFile(matchFileName);
        
        SurfaceFile surfaceFile;
        surfaceFile.readFile(inputFileName);
        
        surfaceFile.matchSurfaceBoundingBox(&matchSurfaceFile);
        
        surfaceFile.writeFile(outputFileName);
    }
    catch (const DataFileException& dfe) {
        throw CommandException(dfe);
    }
}
