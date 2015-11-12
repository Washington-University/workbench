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

#include "OperationSceneFileRelocate.h"
#include "OperationException.h"

#include "SceneFile.h"

using namespace caret;
using namespace std;

AString OperationSceneFileRelocate::getCommandSwitch()
{
    return "-scene-file-relocate";
}

AString OperationSceneFileRelocate::getShortDescription()
{
    return "RECREATE SCENE FILE IN NEW LOCATION";
}

OperationParameters* OperationSceneFileRelocate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "input-scene", "the scene file to use");
    
    ret->addStringParameter(2, "output-scene", "output - the new scene file to create");//fake the output formatting
    
    ret->setHelpText(
        AString("Scene files contain internal relative paths, such that moving or copying a scene file will cause it to lose track of the files it refers to.  ") +
        "This command makes a modified copy of the scene file, changing the relative paths to refer to the new relative locations of the files."
    );
    return ret;
}

void OperationSceneFileRelocate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SceneFile myScene;
    myScene.readFile(myParams->getString(1));
    myScene.writeFile(myParams->getString(2));
}
