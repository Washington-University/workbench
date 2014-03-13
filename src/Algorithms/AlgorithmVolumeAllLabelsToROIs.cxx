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

#include "AlgorithmVolumeAllLabelsToROIs.h"
#include "AlgorithmException.h"

#include "GiftiLabelTable.h"
#include "VolumeFile.h"

#include <cmath>
#include <map>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmVolumeAllLabelsToROIs::getCommandSwitch()
{
    return "-volume-all-labels-to-rois";
}

AString AlgorithmVolumeAllLabelsToROIs::getShortDescription()
{
    return "MAKE ROIS FROM ALL LABELS IN A VOLUME FRAME";
}

OperationParameters* AlgorithmVolumeAllLabelsToROIs::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "label-in", "the input volume label file");
    
    ret->addStringParameter(2, "map", "the number or name of the label map to use");
    
    ret->addVolumeOutputParameter(3, "volume-out", "the output volume file");
    
    ret->setHelpText(
        AString("The output volume has a frame for each label in the specified input frame, other than the ??? label, ") +
        "each of which contains an ROI of all voxels that are set to the corresponding label."
    );
    return ret;
}

void AlgorithmVolumeAllLabelsToROIs::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myLabel = myParams->getVolume(1);
    AString mapID = myParams->getString(2);
    int whichMap = myLabel->getMapIndexFromNameOrNumber(mapID);
    if (whichMap == -1)
    {
        throw AlgorithmException("invalid map number or name specified");
    }
    VolumeFile* myVolOut = myParams->getOutputVolume(3);
    AlgorithmVolumeAllLabelsToROIs(myProgObj, myLabel, whichMap, myVolOut);
}

AlgorithmVolumeAllLabelsToROIs::AlgorithmVolumeAllLabelsToROIs(ProgressObject* myProgObj, const VolumeFile* myLabel, const int& whichMap, VolumeFile* myVolOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (myLabel->getType() != SubvolumeAttributes::LABEL)
    {
        throw AlgorithmException("input volume must be a label volume");
    }
    if (whichMap < 0 || whichMap >= myLabel->getNumberOfMaps())
    {
        throw AlgorithmException("invalid map index specified");
    }
    const GiftiLabelTable* myTable = myLabel->getMapLabelTable(whichMap);
    int32_t unusedKey = myTable->getUnassignedLabelKey();//WARNING: this actually MODIFIES the label table if the ??? key doesn't exist
    set<int32_t> myKeys = myTable->getKeys();
    int numKeys = (int)myKeys.size();
    if (numKeys < 2)
    {
        throw AlgorithmException("label table doesn't contain any keys besides the ??? key");
    }
    map<int32_t, int> keyToMap;//lookup from keys to subvolume
    vector<int64_t> outDims = myLabel->getOriginalDimensions();
    outDims.resize(4);
    outDims[3] = numKeys - 1;//don't include the ??? key
    myVolOut->reinitialize(outDims, myLabel->getSform());
    myVolOut->setValueAllVoxels(0.0f);
    int counter = 0;
    for (set<int32_t>::iterator iter = myKeys.begin(); iter != myKeys.end(); ++iter)
    {
        if (*iter == unusedKey) continue;//skip the ??? key
        keyToMap[*iter] = counter;
        myVolOut->setMapName(counter, myTable->getLabelName(*iter));
        ++counter;
    }
    for (int64_t k = 0; k < outDims[2]; ++k)//because we need to set voxels in the output, rather than in a temporary frame, for single pass without duplicating the memory
    {
        for (int64_t j = 0; j < outDims[1]; ++j)
        {
            for (int64_t i = 0; i < outDims[0]; ++i)
            {
                int32_t thisKey = (int32_t)floor(myLabel->getValue(i, j, k, whichMap) + 0.5f);
                map<int32_t, int>::iterator iter = keyToMap.find(thisKey);
                if (iter != keyToMap.end())
                {
                    myVolOut->setValue(1.0f, i, j, k, iter->second);
                }
            }
        }
    }
}

float AlgorithmVolumeAllLabelsToROIs::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeAllLabelsToROIs::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
