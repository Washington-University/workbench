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

#include "AlgorithmGiftiLabelAddPrefix.h"
#include "AlgorithmException.h"

#include "GiftiLabelTable.h"
#include "LabelFile.h"

using namespace caret;
using namespace std;

AString AlgorithmGiftiLabelAddPrefix::getCommandSwitch()
{
    return "-gifti-label-add-prefix";
}

AString AlgorithmGiftiLabelAddPrefix::getShortDescription()
{
    return "ADD PREFIX TO ALL LABEL NAMES IN A GIFTI LABEL FILE";
}

OperationParameters* AlgorithmGiftiLabelAddPrefix::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addLabelParameter(1, "label-in", "the input label file");
    
    ret->addStringParameter(2, "prefix", "the prefix string to add");
    
    ret->addLabelOutputParameter(3, "label-out", "the output label file");
    
    ret->setHelpText(
        AString("For each label other than '?\?\?', prepend <prefix> to the label name."));//bleh, trigraphs
    return ret;
}

void AlgorithmGiftiLabelAddPrefix::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LabelFile* labelIn = myParams->getLabel(1);
    AString prefix = myParams->getString(2);
    LabelFile* labelOut = myParams->getOutputLabel(3);
    AlgorithmGiftiLabelAddPrefix(myProgObj, labelIn, prefix, labelOut);
}

AlgorithmGiftiLabelAddPrefix::AlgorithmGiftiLabelAddPrefix(ProgressObject* myProgObj, const LabelFile* labelIn, const AString& prefix, LabelFile* labelOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numColumns = labelIn->getNumberOfColumns();
    GiftiLabelTable outTable = *(labelIn->getLabelTable());
    vector<int32_t> keys;
    outTable.getKeys(keys);
    int numKeys = (int)keys.size();
    int32_t unassigned = GiftiLabelTable(outTable).getUnassignedLabelKey();//make a copy so getUnassignedLabelKey() doesn't modify the one we are using
    for (int i = 0; i < numKeys; ++i)
    {
        if (keys[i] != unassigned)
        {
            outTable.setLabelName(keys[i], prefix + outTable.getLabelName(keys[i]));
        }
    }
    labelOut->setNumberOfNodesAndColumns(labelIn->getNumberOfNodes(), numColumns);
    labelOut->setStructure(labelIn->getStructure());
    *(labelOut->getLabelTable()) = outTable;
    for (int i = 0; i < numColumns; ++i)
    {
        labelOut->setMapName(i, labelIn->getMapName(i));
        labelOut->setLabelKeysForColumn(i, labelIn->getLabelKeyPointerForColumn(i));
    }
}

float AlgorithmGiftiLabelAddPrefix::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmGiftiLabelAddPrefix::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
