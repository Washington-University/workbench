
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

#include <iostream>

#include "CaretAssert.h"
#include "CaretLogger.h"

#include "OperationSurfaceInformation.h"
#include "OperationException.h"

#include "DataFileException.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * \class caret::OperationSurfaceInformation 
 * \brief Display information about a surface
 */

/**
 * @return Command line switch
 */
AString
OperationSurfaceInformation::getCommandSwitch()
{
    return "-surface-information";
}

/**
 * @return Short description of operation
 */
AString
OperationSurfaceInformation::getShortDescription()
{
    return "DISPLAY INFORMATION ABOUT A SURFACE";
}

/**
 * @return Parameters for operation
 */
OperationParameters*
OperationSurfaceInformation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "Surface File", "Surface for which information is displayed");
    
    AString helpText = ("Information about surface is displayed including vertices, \n"
                        "triangles, bounding box, and spacing.");
    ret->setHelpText(helpText);
    
    return ret;
}

/**
 * Use Parameters and perform operation
 */
void
OperationSurfaceInformation::useParameters(OperationParameters* myParams,
                                          ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    
    try {
        SurfaceFile* surfaceFile = myParams->getSurface(1);
        std::cout << qPrintable(surfaceFile->getInformation()) << std::endl;
    }
    catch (const DataFileException& dfe) {
        throw OperationException(dfe);
    }
}

