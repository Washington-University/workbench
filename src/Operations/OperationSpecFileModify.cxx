/*LICENSE_START*/
/*
 *  Copyright (C) 2025  Washington University School of Medicine
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

#include "OperationSpecFileModify.h"
#include "OperationException.h"

#include "CaretLogger.h"
#include "FileInformation.h"
#include "SpecFile.h"

#include <QDir>

#include <iostream>
#include <vector>

using namespace caret;
using namespace std;

AString OperationSpecFileModify::getCommandSwitch()
{
    return "-spec-file-modify";
}

AString OperationSpecFileModify::getShortDescription()
{
    return "ADD OR REMOVE FILES FROM SPEC FILE";
}

OperationParameters* OperationSpecFileModify::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "spec-file", "the spec file to modify");
    
    ParameterComponent* addOpt = ret->createRepeatableParameter(2, "-add", "add a file to the spec");
    addOpt->addStringParameter(1, "structure", "the structure of the data file");
    addOpt->addStringParameter(2, "file", "the file to add");
    
    ParameterComponent* removeOpt = ret->createRepeatableParameter(3, "-remove", "remove a file from the spec file");
    removeOpt->addStringParameter(1, "filename", "the filename to remove");
    removeOpt->createOptionalParameter(2, "-recursive", "remove all files that match the string starting from any folder");
    removeOpt->createOptionalParameter(3, "-suffix", "match any file that ends with the given string (if there are multiple matches, error unless -recursive was also specified)");
    
    AString myText = AString("The input spec file is overwritten with the new version.  If the spec file does not exist, it is created with default metadata.\n\n") +
        "Removals are done before additions, so that you can remove files with a pattern, and then add new files even if they match that removal pattern.\n\n" +
        "The <structure> argument of -add must match one of the following strings (you can use INVALID for cifti or volume files):";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        myText += StructureEnum::toName(myStructureEnums[i]) + "\n";
    }
    ret->setHelpText(myText);
    return ret;
}

void OperationSpecFileModify::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString mySpecName = myParams->getString(1);
    SpecFile mySpec;
    FileInformation mySpecInfo(mySpecName);
    if (mySpecInfo.exists())
    {
        mySpec.readFile(mySpecName);
    } else {
        mySpec.setFileName(mySpecName);
    }
    //remove first, then add, we don't want any of this command's adds to get removed
    auto removeInstances = myParams->getRepeatableParameterInstances(3);
    for (auto thisRemove : removeInstances)
    {
        AString pattern = QDir::fromNativeSeparators(thisRemove->getString(1)); //deal with windows native separators
        auto allNames = mySpec.getAllDataFileNames(); //default remove by name behavior looks for exact, but on failure does a dumb "ends with" match, but only removes one
        //so, if someone asks to remove abc.nii.gz but it was already removed, it will happily remove aabc.nii.gz without warning
        //let's do better
        bool found = false;
        AString matchType; //for warning messages
        bool haveMulti = false; //ONLY for erroring with -suffix, -recursive already implies multi is okay - this boolean allows us to report all matches before erroring
        const bool allowMultiWithSuffix = thisRemove->getOptionalParameter(2)->m_present; //double duty, since suffix matches a superset of recursive
        auto patternParts = pattern.split('/');
        for (auto thisName : allNames)
        {
            if (thisRemove->getOptionalParameter(3)->m_present)
            {
                matchType = "suffix";
                if (thisName.endsWith(pattern))
                {
                    if (found)
                    {
                        haveMulti = true; //if we error, it should be after it reports all matches
                    }
                    found = true;
                    cout << "suffix match: " << thisName.toLocal8Bit().constData() << endl;
                    mySpec.removeFileFromSpecByName(thisName);
                }
            } else if (thisRemove->getOptionalParameter(2)->m_present) {
                matchType = "recursive";
                auto thisParts = thisName.split('/');
                bool thisMatch = true;
                for (int i = 0; i < patternParts.size(); ++i)
                {
                    if (thisParts.size() < patternParts.size() || patternParts[i] != thisParts[i + thisParts.size() - patternParts.size()])
                    {
                        thisMatch = false;
                        break;
                    }
                }
                if (thisMatch)
                {
                    found = true;
                    cout << "recursive match: " << thisName.toLocal8Bit().constData() << endl;
                    mySpec.removeFileFromSpecByName(thisName);
                }
            } else {//exact match only, but resolve the absolute path first
                matchType = "exact";
                FileInformation patternInfo(pattern);
                AString fullPattern = patternInfo.getAbsoluteFilePath();
                if (fullPattern == thisName)
                {
                    found = true;
                    mySpec.removeFileFromSpecByName(thisName); //this function also searches, but by doing the test ourselves, we get control of the warning message
                }
            }
        }
        if (haveMulti && !allowMultiWithSuffix)
        {
            throw OperationException("multiple suffix matches found for -remove '" + pattern + "', but -recursive wasn't specified");
        }
        if (!found)
        {
            CaretLogWarning("no " + matchType + " matches found for -remove '" + pattern + "', ignoring");
        }
    }
    //now, the adds
    auto addInstances = myParams->getRepeatableParameterInstances(2);
    for (auto thisAdd : addInstances)
    {
        AString structName = thisAdd->getString(1);
        AString filename = thisAdd->getString(2);
        bool ok = false;
        auto structure = StructureEnum::fromName(structName, &ok);
        if (!ok)
        {
            throw OperationException("unrecognized structure name: " + structName);
        }
        auto myType = DataFileTypeEnum::fromFileExtension(filename, &ok);
        if (!ok)
        {
            throw OperationException("unrecognized data file type");
        }
        FileInformation myDataFileInfo(filename);
        if (!myDataFileInfo.exists())
        {
            throw OperationException("data file not found: " + filename);
        }
        mySpec.addDataFile(myType, structure, filename, true, false, true);
    }
    mySpec.writeFile(mySpecName);
}
