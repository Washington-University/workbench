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

#include "OperationSetStructure.h"
#include "OperationException.h"

#include "BorderFile.h"
#include "CaretLogger.h"
#include "DataFileTypeEnum.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "StructureEnum.h"
#include "SurfaceFile.h"

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
    
    OptionalParameter* surfType = ret->createOptionalParameter(3, "-surface-type", "set the type of a surface (only used if file is a surface file)");
    surfType->addStringParameter(1, "type", "name of surface type");
    
    OptionalParameter* secondaryType = ret->createOptionalParameter(4, "-surface-secondary-type", "set the secondary type of a surface (only used if file is a surface file)");
    secondaryType->addStringParameter(1, "secondary type", "name of surface secondary type");
    
    AString myText = AString("The existing file is modified and rewritten to the same filename.  Valid values for the structure name are:\n\n");
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        myText += StructureEnum::toName(myStructureEnums[i]) + "\n";
    }
    
    myText += "\nValid names for the surface type are:\n\n";
    vector<SurfaceTypeEnum::Enum> mySurfTypeEnums;
    SurfaceTypeEnum::getAllEnums(mySurfTypeEnums);
    for (int i = 0; i < (int)mySurfTypeEnums.size(); ++i)
    {
        myText += SurfaceTypeEnum::toName(mySurfTypeEnums[i]) + "\n";
    }
    
    myText += "\nValid names for the surface secondary type are:\n\n";
    vector<SecondarySurfaceTypeEnum::Enum> mySecondaryTypeEnums;
    SecondarySurfaceTypeEnum::getAllEnums(mySecondaryTypeEnums);
    for (int i = 0; i < (int)mySecondaryTypeEnums.size(); ++i)
    {
        myText += SecondarySurfaceTypeEnum::toName(mySecondaryTypeEnums[i]) + "\n";
    }
    
    ret->setHelpText(myText);
    return ret;
}

void OperationSetStructure::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString fileName = myParams->getString(1);
    AString structureName = myParams->getString(2);
    OptionalParameter* surfType = myParams->getOptionalParameter(3);
    OptionalParameter* secondaryType = myParams->getOptionalParameter(4);
    bool ok = false;
    SurfaceTypeEnum::Enum mySurfType = SurfaceTypeEnum::UNKNOWN;//so compilers won't complain about uninitialized
    if (surfType->m_present)
    {
        AString mySurfTypeName = surfType->getString(1);
        mySurfType = SurfaceTypeEnum::fromName(mySurfTypeName, &ok);
        if (!ok)
        {
            throw OperationException("unrecognized surface type");
        }
    }
    SecondarySurfaceTypeEnum::Enum mySecondType = SecondarySurfaceTypeEnum::INVALID;
    if (secondaryType->m_present)
    {
        AString mySecondaryName = secondaryType->getString(1);
        mySecondType = SecondarySurfaceTypeEnum::fromName(mySecondaryName, &ok);
        if (!ok)
        {
            throw OperationException("unrecognized secondary surface type");
        }
    }
    StructureEnum::Enum myStructure = StructureEnum::fromName(structureName, &ok);
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
                mySurf.setStructure(myStructure);
                if (surfType->m_present)
                {
                    mySurf.setSurfaceType(mySurfType);
                }
                if (secondaryType->m_present)
                {
                    mySurf.setSecondaryType(mySecondType);
                }
                mySurf.writeFile(fileName);
            }
            break;
        case DataFileTypeEnum::LABEL:
            {
                if (surfType->m_present) CaretLogInfo("the -surface-type option is ignored with this file type");
                if (secondaryType->m_present) CaretLogInfo("the -surface-secondary-type option is ignored with this file type");
                LabelFile myLabel;
                myLabel.readFile(fileName);
                myLabel.setStructure(myStructure);
                myLabel.writeFile(fileName);
            }
            break;
        case DataFileTypeEnum::METRIC:
            {
                if (surfType->m_present) CaretLogInfo("the -surface-type option is ignored with this file type");
                if (secondaryType->m_present) CaretLogInfo("the -surface-secondary-type option is ignored with this file type");
                MetricFile myMetric;
                myMetric.readFile(fileName);
                myMetric.setStructure(myStructure);
                myMetric.writeFile(fileName);
            }
            break;
        case DataFileTypeEnum::BORDER:
            {
                if (surfType->m_present) CaretLogInfo("the -surface-type option is ignored with this file type");
                if (secondaryType->m_present) CaretLogInfo("the -surface-secondary-type option is ignored with this file type");
                BorderFile myBorder;
                myBorder.readFile(fileName);
                myBorder.setStructure(myStructure);
                myBorder.writeFile(fileName);
            }
            break;
        default:
            throw OperationException("cannot set the structure of this file type");
    };
}
