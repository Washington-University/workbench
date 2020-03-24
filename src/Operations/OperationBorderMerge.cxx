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
#include "GiftiLabelTable.h"

#include <map>
#include <utility>
#include <vector>

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
    ParameterComponent* selectOpt = borderOpt->createRepeatableParameter(2, "-select", "select a single border to use");
    selectOpt->addStringParameter(1, "border", "the border number or name");
    OptionalParameter* upToOpt = selectOpt->createOptionalParameter(2, "-up-to", "use an inclusive range of borders");
    upToOpt->addStringParameter(1, "last-border", "the number or name of the last column to include");
    upToOpt->createOptionalParameter(2, "-reverse", "use the range in reverse order");
    
    ret->setHelpText(
        AString("Takes one or more border files and makes a new border file from the borders in them.\n\n") +
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
    const vector<ParameterComponent*>& borderInst = myParams->getRepeatableParameterInstances(2);
    int numInputs = (int)borderInst.size();
    if (numInputs < 1) throw OperationException("no inputs specified");
    for (int i = 0; i < numInputs; ++i)
    {
        BorderFile* input = borderInst[i]->getBorder(1);
        if (i != 0)//structure automatically gets set on empty file by added borders
        {
            if (outFile->getStructure() != input->getStructure()) throw OperationException("input files have different structures");
        }
        if (input->getNumberOfNodes() != -1)
        {
            int outNodes = outFile->getNumberOfNodes();
            if (outNodes != -1)
            {
                if (outNodes != input->getNumberOfNodes()) throw OperationException("input files have different number of surface vertices");
            } else {
                outFile->setNumberOfNodes(input->getNumberOfNodes());
            }
        }
        outFile->getClassColorTable()->append(*(input->getClassColorTable()));//let the append logic deal with conflicts
        outFile->getNameColorTable()->append(*(input->getNameColorTable()));//we don't need the return values, as the numbers in the label tables are meaningless
        int numBorderParts = input->getNumberOfBorders();
        const vector<ParameterComponent*>& selectOpts = borderInst[i]->getRepeatableParameterInstances(2);
        int numSelectOpts = (int)selectOpts.size();
        if (numSelectOpts > 0)
        {
            BorderMultiPartHelper myHelp(input);
            for (int j = 0; j < numSelectOpts; ++j)
            {
                int initialBorder = myHelp.fromNumberOrName(selectOpts[j]->getString(1));
                if (initialBorder < 0) throw OperationException("border '" + selectOpts[j]->getString(1) + "' not found in file '" + input->getFileName() + "'");
                OptionalParameter* upToOpt = selectOpts[j]->getOptionalParameter(2);
                if (upToOpt->m_present)
                {
                    int finalBorder = myHelp.fromNumberOrName(upToOpt->getString(1));
                    if (finalBorder < 0) throw OperationException("ending border '" + selectOpts[j]->getString(1) + "' not found in file '" + input->getFileName() + "'");
                    bool reverse = upToOpt->getOptionalParameter(2)->m_present;
                    if (reverse)
                    {
                        for (int b = finalBorder; b >= initialBorder; --b)
                        {
                            for (int k = 0; k < (int)myHelp.borderPieceList[b].size(); ++k)
                            {
                                outFile->addBorder(new Border(*(input->getBorder(myHelp.borderPieceList[b][k]))));
                            }
                        }
                    } else {
                        for (int b = initialBorder; b <= finalBorder; ++b)
                        {
                            for (int k = 0; k < (int)myHelp.borderPieceList[b].size(); ++k)
                            {
                                outFile->addBorder(new Border(*(input->getBorder(myHelp.borderPieceList[b][k]))));
                            }
                        }
                    }
                } else {
                    for (int k = 0; k < (int)myHelp.borderPieceList[initialBorder].size(); ++k)
                    {
                        outFile->addBorder(new Border(*(input->getBorder(myHelp.borderPieceList[initialBorder][k]))));
                    }
                }
            }
        } else {
            for (int j = 0; j < numBorderParts; ++j)
            {
                outFile->addBorder(new Border(*(input->getBorder(j))));
            }
        }
    }
}
