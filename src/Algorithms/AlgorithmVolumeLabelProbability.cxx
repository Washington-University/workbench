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

#include "AlgorithmVolumeLabelProbability.h"
#include "AlgorithmException.h"

#include "GiftiLabelTable.h"
#include "VolumeFile.h"

#include <cmath>
#include <map>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmVolumeLabelProbability::getCommandSwitch()
{
    return "-volume-label-probability";
}

AString AlgorithmVolumeLabelProbability::getShortDescription()
{
    return "FIND FREQUENCY OF VOLUME LABELS";
}

OperationParameters* AlgorithmVolumeLabelProbability::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "label-maps", "volume label file containing individual label maps from many subjects");
    
    ret->addVolumeOutputParameter(2, "probability-out", "the relative frequencies of each label at each voxel");
    
    ret->createOptionalParameter(3, "-exclude-unlabeled", "don't make a probability map of the unlabeled key");
    
    ret->setHelpText(
        AString("This command outputs a set of soft ROIs, one for each label in the input, ") +
        "where the value is how many of the input maps had that label at that voxel, divided by the number of input maps."
    );
    return ret;
}

void AlgorithmVolumeLabelProbability::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    const VolumeFile* inputVol = myParams->getVolume(1);
    VolumeFile* outputVol = myParams->getOutputVolume(2);
    bool excludeUnlabeled = myParams->getOptionalParameter(3)->m_present;
    AlgorithmVolumeLabelProbability(myProgObj, inputVol, outputVol, excludeUnlabeled);
}

AlgorithmVolumeLabelProbability::AlgorithmVolumeLabelProbability(ProgressObject* myProgObj, const VolumeFile* inputVol, VolumeFile* outputVol, const bool& excludeUnlabeled) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (inputVol->getType() != SubvolumeAttributes::LABEL) throw AlgorithmException("input volume must be a label volume");
    if (inputVol->getNumberOfComponents() != 1) throw AlgorithmException("label volumes must not have multiple components per map");
    int numInMaps = inputVol->getNumberOfMaps();
    vector<map<int, int> > outMapToKeyLookup(numInMaps);//we match labels by name, not by key - the lookup is this direction so we can compute one output frame at a time
    map<AString, int> nameToOutMap;//yes, that involves a lot of rescanning memory, but it takes half the additional memory as one pass (because VolumeFile has internal memory)
    for (int i = 0; i < numInMaps; ++i)//testing shows it doesn't take that much longer anyway, for smallish frame sizes at least
    {
        const GiftiLabelTable* thisTable = inputVol->getMapLabelTable(i);
        set<int32_t> thisKeys = thisTable->getKeys();
        int32_t unlabeledKey = -1;//don't request it from the table if we aren't going to skip it, because requesting it can add it to the table
        if (excludeUnlabeled)
        {
            unlabeledKey = thisTable->getUnassignedLabelKey();
        }
        for (set<int32_t>::iterator iter = thisKeys.begin(); iter != thisKeys.end(); ++iter)//order by key value
        {
            if (excludeUnlabeled && *iter == unlabeledKey) continue;//skip to next key
            const AString thisName = thisTable->getLabelName(*iter);
            map<AString, int>::iterator search = nameToOutMap.find(thisName);
            int outMap = -1;
            if (search == nameToOutMap.end())
            {
                outMap = nameToOutMap.size();//sequential integers starting from 0
                nameToOutMap[thisName] = outMap;
            } else {
                outMap = search->second;
            }
            outMapToKeyLookup[i][outMap] = *iter;
        }
    }
    const vector<int64_t> inDims = inputVol->getDimensions();
    vector<int64_t> outDims = inDims;
    outDims.resize(4);
    int64_t numOutMaps = (int64_t)nameToOutMap.size();
    outDims[3] = numOutMaps;
    outputVol->reinitialize(outDims, inputVol->getSform());
    for (map<AString, int>::iterator iter = nameToOutMap.begin(); iter != nameToOutMap.end(); ++iter)
    {
        outputVol->setMapName(iter->second, iter->first);
    }
    int64_t frameSize = inDims[0] * inDims[1] * inDims[2];
    for (int outMap = 0; outMap < numOutMaps; ++outMap)
    {
        vector<int> scratchCount(frameSize, 0);
        for (int inMap = 0; inMap < numInMaps; ++inMap)
        {
            map<int, int>::iterator search = outMapToKeyLookup[inMap].find(outMap);
            if (search == outMapToKeyLookup[inMap].end())
            {
                continue;//this map doesn't contain a label name that matches, skip to next input map
            }
            int matchKey = search->second;
            const float* inFrame = inputVol->getFrame(inMap);
            for (int64_t i = 0; i < frameSize; ++i)
            {
                int thisKey = (int)floor(inFrame[i] + 0.5f);
                if (thisKey == matchKey)
                {
                    ++scratchCount[i];
                }
            }
        }
        vector<float> scratchFrameOut(frameSize);
        for (int64_t i = 0; i < frameSize; ++i)
        {
            scratchFrameOut[i] = ((float)scratchCount[i]) / numInMaps;
        }
        outputVol->setFrame(scratchFrameOut.data(), outMap);
    }
}

float AlgorithmVolumeLabelProbability::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeLabelProbability::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
