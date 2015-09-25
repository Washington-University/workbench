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

#include "AlgorithmCiftiAllLabelsToROIs.h"
#include "AlgorithmException.h"

#include "CiftiFile.h"
#include "GiftiLabelTable.h"

#include <cmath>
#include <map>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmCiftiAllLabelsToROIs::getCommandSwitch()
{
    return "-cifti-all-labels-to-rois";
}

AString AlgorithmCiftiAllLabelsToROIs::getShortDescription()
{
    return "MAKE ROIS FROM ALL LABELS IN A CIFTI LABEL MAP";
}

OperationParameters* AlgorithmCiftiAllLabelsToROIs::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "label-in", "the input cifti label file");
    
    ret->addStringParameter(2, "map", "the number or name of the label map to use");
    
    ret->addCiftiOutputParameter(3, "cifti-out", "the output cifti file");
        
    ret->setHelpText(
        AString("The output cifti file is a dscalar file with a column (map) for each label in the specified input map, other than the ??? label, ") +
        "each of which contains a binary ROI of all brainordinates that are set to the corresponding label.\n\n" +
        "Most of the time, specifying '1' for the <map> argument will do what is desired."
    );
    return ret;
}

void AlgorithmCiftiAllLabelsToROIs::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myLabel = myParams->getCifti(1);
    AString mapID = myParams->getString(2);
    if (myLabel->getCiftiXMLOld().getMappingType(CiftiXMLOld::ALONG_ROW) != CIFTI_INDEX_TYPE_LABELS)
    {
        throw AlgorithmException("mapping type along row must be labels");//because we need to translate map ID to index before algorithm, but want a more useful error if the reason it fails is mapping type
    }
    int whichMap = myLabel->getCiftiXMLOld().getMapIndexFromNameOrNumber(CiftiXMLOld::ALONG_ROW, mapID);
    if (whichMap == -1)
    {
        throw AlgorithmException("invalid map number or name specified");
    }
    CiftiFile* myCiftiOut = myParams->getOutputCifti(3);
    AlgorithmCiftiAllLabelsToROIs(myProgObj, myLabel, whichMap, myCiftiOut);
}

AlgorithmCiftiAllLabelsToROIs::AlgorithmCiftiAllLabelsToROIs(ProgressObject* myProgObj, const CiftiFile* myLabel, const int& whichMap, CiftiFile* myCiftiOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXMLOld& myXML = myLabel->getCiftiXMLOld();
    if (myXML.getMappingType(CiftiXMLOld::ALONG_ROW) != CIFTI_INDEX_TYPE_LABELS)
    {
        throw AlgorithmException("mapping type along row must be labels");//check again, in case it is used from somewhere other than useParameters()
    }
    const GiftiLabelTable* myTable = myXML.getLabelTableForRowIndex(whichMap);
    int32_t unusedKey = myTable->getUnassignedLabelKey();//WARNING: this actually MODIFIES the label table if the ??? key doesn't exist
    set<int32_t> myKeys = myTable->getKeys();
    int numKeys = (int)myKeys.size();
    if (numKeys < 2)
    {
        throw AlgorithmException("label table doesn't contain any keys besides the ??? key");
    }
    map<int32_t, int> keyToMap;//lookup from keys to column
    CiftiXMLOld outXML = myXML;
    outXML.resetDirectionToScalars(CiftiXMLOld::ALONG_ROW, numKeys - 1);
    int counter = 0;
    for (set<int32_t>::iterator iter = myKeys.begin(); iter != myKeys.end(); ++iter)
    {
        if (*iter == unusedKey) continue;//skip the ??? key
        keyToMap[*iter] = counter;
        outXML.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, counter, myTable->getLabelName(*iter));
        ++counter;
    }
    myCiftiOut->setCiftiXML(outXML);
    vector<float> outRowScratch(numKeys - 1, 0.0f), inRowScratch(myXML.getNumberOfColumns());
    int64_t numRows = myXML.getNumberOfRows();
    for (int64_t i = 0; i < numRows; ++i)
    {
        myLabel->getRow(inRowScratch.data(), i);
        int32_t thisKey = (int32_t)floor(inRowScratch[whichMap] + 0.5f);
        map<int32_t, int>::iterator iter = keyToMap.find(thisKey);
        if (iter != keyToMap.end())
        {
            outRowScratch[iter->second] = 1.0f;//set the single element for the correct map
        }
        myCiftiOut->setRow(outRowScratch.data(), i);
        if (iter != keyToMap.end())
        {
            outRowScratch[iter->second] = 0.0f;//and rezero it to get ready for the next row
        }
    }
}

float AlgorithmCiftiAllLabelsToROIs::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiAllLabelsToROIs::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
