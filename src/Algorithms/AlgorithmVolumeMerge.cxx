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

#include "AlgorithmVolumeMerge.h"
#include "AlgorithmException.h"
#include "CaretAssert.h"
#include "GiftiLabelTable.h"
#include "PaletteColorMapping.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString AlgorithmVolumeMerge::getCommandSwitch()
{
    return "-volume-merge";
}

AString AlgorithmVolumeMerge::getShortDescription()
{
    return "MERGE VOLUME FILES INTO A NEW FILE";
}

OperationParameters* AlgorithmVolumeMerge::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeOutputParameter(1, "volume-out", "the output volume file");
    
    ParameterComponent* volumeOpt = ret->createRepeatableParameter(2, "-volume", "specify an input volume file");
    volumeOpt->addVolumeParameter(1, "volume-in", "a volume file to use subvolumes from");
    OptionalParameter* subvolOpt = volumeOpt->createOptionalParameter(2, "-subvolume", "select a single subvolume to use");
    subvolOpt->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Takes one or more volume files and constructs a new volume file by concatenating subvolumes from them.  ") +
        "The input volume files must have the same volume space.\n\n" +
        "Example: wb_command -volume-merge out.nii -volume first.nii -subvolume 1 -volume second.nii\n\n" +
        "This example would take the first subvolume from first.nii, followed by all subvolumes from second.nii, " +
        "and write these to out.nii."
    );
    return ret;
}

void AlgorithmVolumeMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* volumeOut = myParams->getOutputVolume(1);
    const vector<ParameterComponent*>& myInputs = *(myParams->getRepeatableParameterInstances(2));
    int numInputs = (int)myInputs.size();
    vector<const VolumeFile*> volumeList;
    vector<int64_t> indexList;
    for (int i = 0; i < numInputs; ++i)
    {
        VolumeFile* myVol = myInputs[i]->getVolume(1);
        volumeList.push_back(myVol);
        OptionalParameter* subvolOpt = myInputs[i]->getOptionalParameter(2);
        if (subvolOpt->m_present)
        {
            int index = myVol->getMapIndexFromNameOrNumber(subvolOpt->getString(1));
            if (index == -1) throw AlgorithmException("subvolume '" + subvolOpt->getString(1) + "' not found in file '" + myVol->getFileName() + "'");
            indexList.push_back(index);
        } else {
            indexList.push_back(-1);
        }
    }
    AlgorithmVolumeMerge(myProgObj, volumeList, indexList, volumeOut);
}

AlgorithmVolumeMerge::AlgorithmVolumeMerge(ProgressObject* myProgObj, const vector<const VolumeFile*>& volumeList, const vector<int64_t>& indexList, VolumeFile* volumeOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (volumeList.size() == 0)
    {
        throw AlgorithmException("no files specified");
    }
    CaretAssert(volumeList.size() == indexList.size());
    int numInputs = (int)volumeList.size();
    int64_t numOutVols = 0;
    vector<int64_t> firstDims;
    volumeList[0]->getDimensions(firstDims);
    bool isLabel = (volumeList[0]->getType() == SubvolumeAttributes::LABEL);
    for (int i = 0; i < numInputs; ++i)
    {
        if (!volumeList[i]->matchesVolumeSpace(volumeList[0]))
        {
            throw AlgorithmException("volume file '" + volumeList[i]->getFileName() + "' has a different volume space");
        }
        if (isLabel != (volumeList[i]->getType() == SubvolumeAttributes::LABEL))
        {
            throw AlgorithmException("only some volumes are label volumes, first mismatch is '" + volumeList[i]->getFileName() + "'");
        }
        vector<int64_t> thisDims;
        volumeList[i]->getDimensions(thisDims);
        if (thisDims[4] != firstDims[4]) throw ("volume file '" + volumeList[i]->getFileName() + "' has a different number of components");
        if (indexList[i] < -1) throw AlgorithmException("found invalid (less than -1) index in indexList in AlgorithmVolumeMerge");
        if (indexList[i] == -1)
        {
            numOutVols += thisDims[3];
        } else {
            if (indexList[i] >= thisDims[3]) throw AlgorithmException("index too large for input volume #" + AString::number(i + 1));
            numOutVols += 1;
        }
    }
    vector<int64_t> outDims = volumeList[0]->getOriginalDimensions();
    outDims.resize(4);
    outDims[3] = numOutVols;
    volumeOut->reinitialize(outDims, volumeList[0]->getSform(), firstDims[4], volumeList[0]->getType());
    int64_t curSubvol = 0;
    for (int i = 0; i < numInputs; ++i)
    {
        if (indexList[i] == -1)
        {
            vector<int64_t> thisDims;
            volumeList[i]->getDimensions(thisDims);
            for (int b = 0; b < thisDims[3]; ++b)
            {
                for (int c = 0; c < firstDims[4]; ++c)
                {
                    volumeOut->setFrame(volumeList[i]->getFrame(b, c), curSubvol, c);
                }
                volumeOut->setMapName(curSubvol, volumeList[i]->getMapName(b));
                if (isLabel)
                {
                    *(volumeOut->getMapLabelTable(curSubvol)) = *(volumeList[i]->getMapLabelTable(b));
                } else {
                    *(volumeOut->getMapPaletteColorMapping(curSubvol)) = *(volumeList[i]->getMapPaletteColorMapping(b));
                }
                ++curSubvol;
            }
        } else {
            for (int c = 0; c < firstDims[4]; ++c)
            {
                volumeOut->setFrame(volumeList[i]->getFrame(indexList[i], c), curSubvol, c);
            }
            volumeOut->setMapName(curSubvol, volumeList[i]->getMapName(indexList[i]));
            if (isLabel)
            {
                *(volumeOut->getMapLabelTable(curSubvol)) = *(volumeList[i]->getMapLabelTable(indexList[i]));
            } else {
                *(volumeOut->getMapPaletteColorMapping(curSubvol)) = *(volumeList[i]->getMapPaletteColorMapping(indexList[i]));
            }
            ++curSubvol;
        }
    }
}

float AlgorithmVolumeMerge::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeMerge::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
