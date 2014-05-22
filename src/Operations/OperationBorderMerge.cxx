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

#include "OperationBorderMerge.h"
#include "OperationException.h"

#include "Border.h"
#include "BorderFile.h"

using namespace caret;
using namespace std;

AString OperationBorderMerge::getCommandSwitch()
{
    return "-border-merge";
}

AString OperationBorderMerge::getShortDescription()
{
    return "MERGE BORDER FILES INTO A NEW FILE";
}

OperationParameters* OperationBorderMerge::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addBorderOutputParameter(1, "border-file-out", "the output border file");
    
    ParameterComponent* borderOpt = ret->createRepeatableParameter(2, "-border", "specify an input border file");
    borderOpt->addBorderParameter(1, "border-file-in", "a border file to use borders from");
    OptionalParameter* selectOpt = borderOpt->createOptionalParameter(2, "-select", "select a single border to use");
    selectOpt->addStringParameter(1, "border", "the border number or name");
    
    ret->setHelpText(
        AString("Takes one or more border files and makes a new border file from the borders in them.  ") +
        "Beware, the borders are NOT checked for same structure or number of vertices in the surface they apply to.\n\n" +
        "Example: wb_command -border-merge out.border -border first.border -select 1 -border second.border\n\n" +
        "This example would take the first border from first.border, followed by all borders from second.border, " +
        "and write these to out.border."
    );
    return ret;
}

void OperationBorderMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    BorderFile* outFile = myParams->getOutputBorder(1);
    const vector<ParameterComponent*>& borderInst = *(myParams->getRepeatableParameterInstances(2));
    int numInputs = (int)borderInst.size();
    if (numInputs < 1) throw OperationException("no inputs specified");
    for (int i = 0; i < numInputs; ++i)
    {
        BorderFile* input = borderInst[i]->getBorder(1);
        int numBorders = input->getNumberOfBorders();
        OptionalParameter* selectOpt = borderInst[i]->getOptionalParameter(2);
        if (selectOpt->m_present)
        {
            AString borderID = selectOpt->getString(1);
            bool ok = false;
            int whichBorder = borderID.toInt(&ok) - 1;//first border is "1"
            if (!ok)//only search for name if the string isn't a number, to prevent surprises
            {
                for (int j = 0; j < numBorders; ++j)
                {
                    if (input->getBorder(j)->getName() == borderID)
                    {
                        ok = true;
                        whichBorder = j;
                        break;
                    }
                }
            }
            if (!ok || whichBorder < 0 || whichBorder >= numBorders)
            {
                throw OperationException("border '" + borderID + "' not found in file '" + input->getFileName() + "'");
            }
            outFile->addBorder(new Border(*(input->getBorder(whichBorder))));
        } else {
            for (int j = 0; j < numBorders; ++j)
            {
                outFile->addBorder(new Border(*(input->getBorder(j))));
            }
        }
    }
}
