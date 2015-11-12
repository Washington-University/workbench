/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

#include "OperationSpecFileRelocate.h"
#include "OperationException.h"

#include "SpecFile.h"

using namespace caret;
using namespace std;

AString OperationSpecFileRelocate::getCommandSwitch()
{
    return "-spec-file-relocate";
}

AString OperationSpecFileRelocate::getShortDescription()
{
    return "RECREATE SPEC FILE IN NEW LOCATION";
}

OperationParameters* OperationSpecFileRelocate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "input-spec", "the spec file to use");
    
    ret->addStringParameter(2, "output-spec", "output - the new spec file to create");//fake the output formatting
    
    ret->setHelpText(
        AString("Spec files contain internal relative paths, such that moving or copying a spec file will cause it to lose track of the files it refers to.  ") +
        "This command makes a modified copy of the spec file, changing the relative paths to refer to the new relative locations of the files."
    );
    return ret;
}

void OperationSpecFileRelocate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SpecFile mySpec;
    mySpec.readFile(myParams->getString(1));
    mySpec.writeFile(myParams->getString(2));
}
