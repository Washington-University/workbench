
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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

