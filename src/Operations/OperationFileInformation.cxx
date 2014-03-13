
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

#include "CaretAssert.h"
#include "CaretLogger.h"

#include "CaretDataFile.h"
#include "CaretDataFileHelper.h"
#include "DataFileContentInformation.h"
#include "OperationFileInformation.h"
#include "OperationException.h"

using namespace caret;

/**
 * \class caret::OperationFileInformation 
 * \brief List information about a file's content
 */

/**
 * @return Command line switch
 */
AString
OperationFileInformation::getCommandSwitch()
{
    return "-file-information";
}

/**
 * @return Short description of operation
 */
AString
OperationFileInformation::getShortDescription()
{
    return "LIST INFORMATION ABOUT A FILE'S CONTENT";
}

/**
 * @return Parameters for operation
 */
OperationParameters*
OperationFileInformation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "data-file", "data file");
    
    AString helpText("List information about the content of a data file.  "
                     "The information listed is dependent upon the type of "
                     "data file.");
    
    ret->setHelpText(helpText);
    
    return ret;
}

/**
 * Use Parameters and perform operation
 */
void
OperationFileInformation::useParameters(OperationParameters* myParams,
                                          ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    
    const AString dataFileName = myParams->getString(1);
    
    try {
        CaretDataFile* caretDataFile = CaretDataFileHelper::readAnyCaretDataFile(dataFileName);
        DataFileContentInformation dataFileContentInformation;
        caretDataFile->addToDataFileContentInformation(dataFileContentInformation);
        std::cout << qPrintable(dataFileContentInformation.getInformationInString()) << std::endl;
        delete caretDataFile;
    }
    catch (const DataFileException& dfe) {
        throw OperationException(dfe.whatString());
    }
}

