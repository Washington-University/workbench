
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include "CaretAssert.h"
#include "CaretLogger.h"

#include "BorderFile.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "OperationBorderFileExportToCaret5.h"
#include "OperationException.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * \class caret::OperationBorderFileExportToCaret5 
 * \brief Export border file to Caret5 file format
 */

/**
 * @return Command line switch
 */
AString
OperationBorderFileExportToCaret5::getCommandSwitch()
{
    return "-border-file-export-to-caret5";
}

/**
 * @return Short description of operation
 */
AString
OperationBorderFileExportToCaret5::getShortDescription()
{
    return "EXPORT BORDER FILE TO CARET5 FILE FORMAT";
}

/**
 * @return Parameters for operation
 */
OperationParameters*
OperationBorderFileExportToCaret5::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "border-file", "workbench border file");
    
    ret->addStringParameter(2, "output-file-prefix", "prefix for name of output caret5 border/borderproj/bordercolor files");
    
    ParameterComponent* surfaceOpt = ret->createRepeatableParameter(3, "-surface", "specify an input surface");
    surfaceOpt->addSurfaceParameter(1, "surface-in", "a surface file for unprojection of borders");
    
    AString helpText("A Workbench border file may contain borders for multiple "
                     "structures and borders that are both projected and "
                     "unprojected.  It also contains a color table for the borders. "
                     "\n"
                     "\n"
                     "Caret5 has both border (unprojected) and border projection "
                     "(projected) files.  In addition, each Caret5 border or border "
                     "projection file typically contains data for a single structure. "
                     "Caret5 also uses a border color file that associates colors with "
                     "the names of the borders. "
                     "\n"
                     "\n"
                     "This command will try to output both Caret5 border and "
                     "border projection files.  Each output border/border projection "
                     "file will contains data for one structure so there may be "
                     "many files created.  The structure name is included in the name of "
                     "each border or border projection file that is created. "
                     "\n"
                     "\n"
                     "One Caret5 border color file will also be produced by this "
                     "command. "
                     "\n"
                     "\n"
                     "Providing surface(s) as input parameters is optional, but recommended. "
                     "Surfaces may be needed to create both projected and/or unprojected coordinates "
                     "of borders.  If there is a failure to produce an output border or "
                     "border projection due to a missing surface with the matching structure, "
                     "an error message will be displayed and some "
                     "output files will not be created. "
                     "\n"
                     "\n"
                     "When writing new files, this command will overwrite a file "
                     "with the same name. "
                     "");
    
    ret->setHelpText(helpText);
    
    return ret;
}

/**
 * Use Parameters and perform operation
 */
void
OperationBorderFileExportToCaret5::useParameters(OperationParameters* myParams,
                                          ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString borderFileName = FileInformation(myParams->getString(1)).getAbsoluteFilePath();
    AString outputCaret5FilePrefix = FileInformation(myParams->getString(2)).getAbsoluteFilePath();

    std::vector<SurfaceFile*> allSurfaces;
    const std::vector<ParameterComponent*>& surfaceInputs = myParams->getRepeatableParameterInstances(3);
    const int32_t numSurfaceInputs = static_cast<int32_t>(surfaceInputs.size());
    for (int32_t iSurf = 0; iSurf < numSurfaceInputs; iSurf++) {
        SurfaceFile* sf = surfaceInputs[iSurf]->getSurface(1);
        allSurfaces.push_back(sf);
    }
    try {
        BorderFile borderFile;
        borderFile.readFile(borderFileName);
        
        borderFile.exportToCaret5Format(allSurfaces,
                                        outputCaret5FilePrefix);
    }
    catch (const DataFileException& dfe) {
        throw OperationException(dfe);
    }
}

