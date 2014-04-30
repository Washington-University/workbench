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

#include "FileInformation.h"
#include "OperationSpecFileMerge.h"
#include "OperationException.h"
#include "SpecFile.h"

using namespace caret;
using namespace std;

AString OperationSpecFileMerge::getCommandSwitch()
{
    return "-spec-file-merge";
}

AString OperationSpecFileMerge::getShortDescription()
{
    return "MERGE TWO SPEC FILES INTO ONE";
}

OperationParameters* OperationSpecFileMerge::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "spec-1", "first spec file to merge");
    ret->addStringParameter(2, "spec-2", "second spec file to merge");
    ret->addStringParameter(3, "out-spec", "output - output spec file");//fake the "output" formatting, could make a spec file parameter type
    ret->setHelpText(AString("The output spec file contains every file that is in either of the input spec files."));
    return ret;
}

void OperationSpecFileMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString spec1Name = FileInformation(myParams->getString(1)).getAbsoluteFilePath();//since opening a spec file may change the current directory, we must convert all paths to absolute first
    AString spec2Name = FileInformation(myParams->getString(2)).getAbsoluteFilePath();
    AString outSpecName = FileInformation(myParams->getString(3)).getAbsoluteFilePath();
    SpecFile spec1, spec2;
    spec1.readFile(spec1Name);
    spec2.readFile(spec2Name);
    spec1.appendSpecFile(spec2);
    spec1.writeFile(outSpecName);
}
