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

#include "OperationSetStructure.h"
#include "OperationException.h"
#include "StructureEnum.h"
#include "DataFileTypeEnum.h"
#include "SurfaceFile.h"
#include "LabelFile.h"
#include "MetricFile.h"

using namespace caret;
using namespace std;

AString OperationSetStructure::getCommandSwitch()
{
    return "-set-structure";
}

AString OperationSetStructure::getShortDescription()
{
    return "SET STRUCTURE OF A DATA FILE";
}

OperationParameters* OperationSetStructure::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "data-file", "the file to set the structure of");
    ret->addStringParameter(2, "structure", "the structure to set the file to");
    AString myText = AString("The existing file is modified and rewritten to the same filename.  Valid values for the structure argument are:\n\n");
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        myText += StructureEnum::toName(myStructureEnums[i]) + "\n";
    }
    ret->setHelpText(myText);
    return ret;
}

void OperationSetStructure::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString fileName = myParams->getString(1);
    AString structureName = myParams->getString(2);
    bool ok = false;
    StructureEnum::Enum myStrucure = StructureEnum::fromName(structureName, &ok);
    if (!ok)
    {
        throw OperationException("unrecognized structure type");
    }
    DataFileTypeEnum::Enum myType = DataFileTypeEnum::fromFileExtension(fileName, &ok);
    if (!ok)
    {
        throw OperationException("unrecognized data file type");
    }
    switch (myType)
    {
        case DataFileTypeEnum::SURFACE:
            {
                SurfaceFile mySurf;
                mySurf.readFile(fileName);
                mySurf.setStructure(myStrucure);
                mySurf.writeFile(fileName);
            }
            break;
        case DataFileTypeEnum::LABEL:
            {
                LabelFile myLabel;
                myLabel.readFile(fileName);
                myLabel.setStructure(myStrucure);
                myLabel.writeFile(fileName);
            }
            break;
        case DataFileTypeEnum::METRIC:
            {
                MetricFile myMetric;
                myMetric.readFile(fileName);
                myMetric.setStructure(myStrucure);
                myMetric.writeFile(fileName);
            }
            break;
        default:
            throw OperationException("cannot set the structure of this file type");
    };
}
