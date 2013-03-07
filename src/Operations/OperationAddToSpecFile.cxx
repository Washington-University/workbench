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

#include "OperationAddToSpecFile.h"
#include "OperationException.h"
#include "SpecFile.h"
#include "FileInformation.h"
#include <vector>

using namespace caret;
using namespace std;

AString OperationAddToSpecFile::getCommandSwitch()
{
    return "-add-to-spec-file";
}

AString OperationAddToSpecFile::getShortDescription()
{
    return "ADD A FILE TO A SPECIFICATION FILE";
}

OperationParameters* OperationAddToSpecFile::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "specfile", "the specification file to add to");
    ret->addStringParameter(2, "structure", "the structure of the data file");
    ret->addStringParameter(3, "filename", "the path to the file");
    AString myText = AString("The resulting spec file overwrites the existing spec file.  If the spec file doesn't exist, ") +
        "it is created with default metadata.  The structure argument must be one of the following:\n\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        myText += StructureEnum::toName(myStructureEnums[i]) + "\n";
    }
    ret->setHelpText(myText);
    return ret;
}

void OperationAddToSpecFile::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    bool ok = false;
    AString mySpecName = myParams->getString(1);//spec file
    AString myStructureName = myParams->getString(2);//file structure
    StructureEnum::Enum myStrucure = StructureEnum::fromName(myStructureName, &ok);
    if (!ok)
    {
        throw OperationException("unrecognized structure type");
    }
    AString myDataFileName = myParams->getString(3);//file path
    FileInformation myDataFileInfo(myDataFileName);
    if (!myDataFileInfo.exists())
    {
        throw OperationException("data file not found");
    }
    DataFileTypeEnum::Enum myType = DataFileTypeEnum::fromFileExtension(myDataFileName, &ok);
    if (!ok)
    {
        throw OperationException("unrecognized data file type");
    }
    SpecFile mySpec;
    FileInformation mySpecInfo(mySpecName);
    if (mySpecInfo.exists())
    {
        mySpec.readFile(mySpecName);
    } else {
        mySpec.setFileName(mySpecName);
    }
    mySpec.addDataFile(myType, myStrucure, myDataFileName, true, false, true);
    mySpec.writeFile(mySpecName);
}
