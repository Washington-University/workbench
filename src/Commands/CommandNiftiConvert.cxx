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

#include "CommandNiftiConvert.h"
#include "FileInformation.h"
#include "NiftiFile.h"
#include "ProgramParameters.h"

using namespace caret;

/**
 * Constructor.
 */
CommandNiftiConvert::CommandNiftiConvert()
: CommandOperation("-nifti-convert",
                         "Convert a NIFTI 1 file to a NIFTI 2 file")
{
    
}

/**
 * Destructor.
 */
CommandNiftiConvert::~CommandNiftiConvert()
{
    
}

/**
 * @return The help information.
 */
AString
CommandNiftiConvert::getHelpInformation(const AString& /*programName*/)
{
    AString helpInfo = ("Convert NIFTI-1 to NIFTI-2\n"
                        "\n"
                        "Usage:  <input-nifti-file>\n"
                        "        <output-nifti-file>\n"
                        "    \n"
                        "    input-nifti-file\n"
                        "        Required input NIFTI file name.\n"
                        "    \n"
                        "    output-nifti-file\n"
                        "        Required output NIFTI file name.\n"
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
CommandNiftiConvert::executeOperation(ProgramParameters& parameters) throw (CommandException,
                                                               ProgramParametersException)
{
    const AString inputFileName = parameters.nextString("Input NIFTI File Name");
    const AString outputFileName = parameters.nextString("Output NIFTI File Name");
    NiftiFile nf(inputFileName);
    //NiftiFile nfOut;
    Nifti2Header header;
    nf.getHeader(header);
    nf.setHeader(header);
    nf.writeFile(outputFileName);
}