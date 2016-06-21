/*LICENSE_START*/
/*
 *  Copyright (C) 2016  Washington University School of Medicine
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

#include "OperationSceneFileMerge.h"
#include "OperationException.h"

#include "Scene.h"
#include "SceneFile.h"

using namespace caret;
using namespace std;

AString OperationSceneFileMerge::getCommandSwitch()
{
    return "-scene-file-merge";
}

AString OperationSceneFileMerge::getShortDescription()
{
    return "REARRANGE SCENES INTO A NEW FILE";
}

OperationParameters* OperationSceneFileMerge::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "scene-file-out", "output - the output scene file");//HACK: fake the output formatting
    
    ParameterComponent* sceneFileOpt = ret->createRepeatableParameter(2, "-scene-file", "specify a scene file to use scenes from");
    sceneFileOpt->addStringParameter(1, "scene-file", "the input scene file");
    OptionalParameter* sceneOpt = sceneFileOpt->createOptionalParameter(2, "-scene", "specify a scene to use");//NOTE: can't be repeatable until Scene has a copy constructor
    sceneOpt->addStringParameter(1, "scene", "the scene number or name");
    OptionalParameter* upToOpt = sceneOpt->createOptionalParameter(2, "-up-to", "use an inclusive range of scenes");
    upToOpt->addStringParameter(1, "last-column", "the number or name of the last scene to include");
    upToOpt->createOptionalParameter(2, "-reverse", "use the range in reverse order");
    
    ret->setHelpText(
        AString("Takes one or more scene files and constructs a new scene file by concatenating specified scenes from them.\n\n") +
        "Example: wb_command -scene-file-merge out.scene -scene-file first.scene -scene 1 -scene-file second.scene\n\n" +
        "This example would take the first scene from first.scene, followed by all scenes from second.scene, and write these scenes to out.scene."
    );
    return ret;
}

void OperationSceneFileMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SceneFile outSceneFile;
    const vector<ParameterComponent*>& myInputs = *(myParams->getRepeatableParameterInstances(2));
    int numInputs = (int)myInputs.size();
    if (numInputs == 0) throw OperationException("no inputs specified");
    for (int i = 0; i < numInputs; ++i)
    {
        ParameterComponent* thisInput = myInputs[i];
        SceneFile inputScene;
        inputScene.readFile(thisInput->getString(1));
        OptionalParameter* selectOpt = thisInput->getOptionalParameter(2);
        if (selectOpt->m_present)
        {
            int firstInd = inputScene.getSceneIndexFromNumberOrName(selectOpt->getString(1));
            if (firstInd < 0)
            {
                throw OperationException("scene '" + selectOpt->getString(1) + "' not found in file '" + thisInput->getString(1) + "'");
            }
            OptionalParameter* upToOpt = selectOpt->getOptionalParameter(2);
            if (upToOpt->m_present)
            {
                int lastInd = inputScene.getSceneIndexFromNumberOrName(upToOpt->getString(1));
                if (lastInd < 0)
                {
                    throw OperationException("scene '" + upToOpt->getString(1) + "' not found in file '" + thisInput->getString(1) + "'");
                }
                if (upToOpt->getOptionalParameter(2)->m_present)//reverse
                {
                    for (int s = lastInd; s >= firstInd; --s)
                    {
                        outSceneFile.addScene(inputScene.releaseScene(s));//NOTE: this only works this simply because they are removed in reverse order
                    }
                } else {
                    for (int s = firstInd; s <= lastInd; ++s)
                    {
                        outSceneFile.addScene(inputScene.releaseScene(firstInd));//HACK: because they are removed in order, the next one will always be at the initial index
                    }
                }
            } else {
                outSceneFile.addScene(inputScene.releaseScene(firstInd));
            }
        } else {
            int numScenes = inputScene.getNumberOfScenes();
            for (int s = 0; s < numScenes; ++s)
            {
                outSceneFile.addScene(inputScene.releaseScene(0));//HACK: remove all and add in order, means that the one to remove will always be at first index
            }
        }
    }
    outSceneFile.writeFile(myParams->getString(1));
}
