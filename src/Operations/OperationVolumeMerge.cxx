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

#include "OperationVolumeMerge.h"
#include "OperationException.h"

#include "CaretAssert.h"
#include "GiftiLabelTable.h"
#include "PaletteColorMapping.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString OperationVolumeMerge::getCommandSwitch()
{
    return "-volume-merge";
}

AString OperationVolumeMerge::getShortDescription()
{
    return "MERGE VOLUME FILES INTO A NEW FILE";
}

OperationParameters* OperationVolumeMerge::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeOutputParameter(1, "volume-out", "the output volume file");
    
    ParameterComponent* volumeOpt = ret->createRepeatableParameter(2, "-volume", "specify an input volume file");
    volumeOpt->addVolumeParameter(1, "volume-in", "a volume file to use subvolumes from");
    ParameterComponent* subvolOpt = volumeOpt->createRepeatableParameter(2, "-subvolume", "select a single subvolume to use");
    subvolOpt->addStringParameter(1, "subvol", "the subvolume number or name");
    OptionalParameter* upToOpt = subvolOpt->createOptionalParameter(2, "-up-to", "use an inclusive range of subvolumes");
    upToOpt->addStringParameter(1, "last-subvol", "the number or name of the last subvolume to include");
    upToOpt->createOptionalParameter(2, "-reverse", "use the range in reverse order");
    
    ret->setHelpText(
        AString("Takes one or more volume files and constructs a new volume file by concatenating subvolumes from them.  ") +
        "The input volume files must have the same volume space.\n\n" +
        "Example: wb_command -volume-merge out.nii -volume first.nii -subvolume 1 -volume second.nii\n\n" +
        "This example would take the first subvolume from first.nii, followed by all subvolumes from second.nii, " +
        "and write these to out.nii."
    );
    return ret;
}

void OperationVolumeMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    VolumeFile* volumeOut = myParams->getOutputVolume(1);
    const vector<ParameterComponent*>& myInputs = myParams->getRepeatableParameterInstances(2);
    int numInputs = (int)myInputs.size();
    if (numInputs < 1) throw OperationException("no inputs specified");
    int64_t subvolCount = 0;
    const VolumeFile* firstVol = myInputs[0]->getVolume(1);
    vector<int64_t> firstDims = firstVol->getDimensions();
    bool isLabel = (firstVol->getType() == SubvolumeAttributes::LABEL);
    for (int i = 0; i < numInputs; ++i)
    {
        const VolumeFile* myVol = myInputs[i]->getVolume(1);
        if (!myVol->matchesVolumeSpace(firstVol))
        {
            throw OperationException("volume file '" + myVol->getFileName() + "' has a different volume space");
        }
        if (isLabel != (myVol->getType() == SubvolumeAttributes::LABEL))
        {
            throw OperationException("only some volumes are label volumes, first mismatch is '" + myVol->getFileName() + "'");
        }
        vector<int64_t> thisDims = myVol->getDimensions();
        if (thisDims[4] != firstDims[4]) throw ("volume file '" + myVol->getFileName() + "' has a different number of components");
        const vector<ParameterComponent*>& subvolOpts = myInputs[i]->getRepeatableParameterInstances(2);
        int numSubvolOpts = (int)subvolOpts.size();
        if (numSubvolOpts > 0)
        {
            for (int j = 0; j < numSubvolOpts; ++j)
            {
                int64_t initialFrame = myVol->getMapIndexFromNameOrNumber(subvolOpts[j]->getString(1));
                if (initialFrame < 0) throw OperationException("subvolume '" + subvolOpts[j]->getString(1) + "' not found in file '" + myVol->getFileName() + "'");
                OptionalParameter* upToOpt = subvolOpts[j]->getOptionalParameter(2);
                if (upToOpt->m_present)
                {
                    int64_t finalFrame = myVol->getMapIndexFromNameOrNumber(upToOpt->getString(1));
                    if (finalFrame < 0) throw OperationException("ending subvolume '" + upToOpt->getString(1) + "' not found in file '" + myVol->getFileName() + "'");
                    if (finalFrame < initialFrame) throw OperationException("ending subvolume '" + upToOpt->getString(1) + "' occurs before starting subvolume '"
                                                                            + subvolOpts[j]->getString(1) + "' in file '" + myVol->getFileName() + "'");
                    subvolCount += finalFrame - initialFrame + 1;//inclusive - we don't need to worry about reversing for counting, though
                } else {
                    subvolCount += 1;
                }
            }
        } else {
            subvolCount += myVol->getNumberOfMaps();
        }
    }
    vector<int64_t> outDims = firstVol->getOriginalDimensions();
    outDims.resize(4);
    outDims[3] = subvolCount;
    volumeOut->reinitialize(outDims, firstVol->getSform(), firstDims[4], firstVol->getType(), firstVol->m_header);
    int64_t curOutVol = 0;
    for (int i = 0; i < numInputs; ++i)
    {
        const VolumeFile* myVol = myInputs[i]->getVolume(1);
        const vector<ParameterComponent*>& subvolOpts = myInputs[i]->getRepeatableParameterInstances(2);
        int numSubvolOpts = (int)subvolOpts.size();
        if (numSubvolOpts > 0)
        {
            for (int j = 0; j < numSubvolOpts; ++j)
            {
                int64_t initialFrame = myVol->getMapIndexFromNameOrNumber(subvolOpts[j]->getString(1));
                OptionalParameter* upToOpt = subvolOpts[j]->getOptionalParameter(2);
                if (upToOpt->m_present)
                {
                    int64_t finalFrame = myVol->getMapIndexFromNameOrNumber(upToOpt->getString(1));
                    bool reverse = upToOpt->getOptionalParameter(2)->m_present;
                    if (reverse)
                    {
                        for (int64_t b = finalFrame; b >= initialFrame; --b)
                        {
                            for (int64_t c = 0; c < firstDims[4]; ++c)
                            {
                                volumeOut->setFrame(myVol->getFrame(b, c), curOutVol, c);
                            }
                            volumeOut->setMapName(curOutVol, myVol->getMapName(b));
                            if (isLabel)
                            {
                                *(volumeOut->getMapLabelTable(curOutVol)) = *(myVol->getMapLabelTable(b));
                            } else {
                                *(volumeOut->getMapPaletteColorMapping(curOutVol)) = *(myVol->getMapPaletteColorMapping(b));
                            }
                            ++curOutVol;
                        }
                    } else {
                        for (int64_t b = initialFrame; b <= finalFrame; ++b)
                        {
                            for (int64_t c = 0; c < firstDims[4]; ++c)
                            {
                                volumeOut->setFrame(myVol->getFrame(b, c), curOutVol, c);
                            }
                            volumeOut->setMapName(curOutVol, myVol->getMapName(b));
                            if (isLabel)
                            {
                                *(volumeOut->getMapLabelTable(curOutVol)) = *(myVol->getMapLabelTable(b));
                            } else {
                                *(volumeOut->getMapPaletteColorMapping(curOutVol)) = *(myVol->getMapPaletteColorMapping(b));
                            }
                            ++curOutVol;
                        }
                    }
                } else {
                    for (int64_t c = 0; c < firstDims[4]; ++c)
                    {
                        volumeOut->setFrame(myVol->getFrame(initialFrame, c), curOutVol, c);
                    }
                    volumeOut->setMapName(curOutVol, myVol->getMapName(initialFrame));
                    if (isLabel)
                    {
                        *(volumeOut->getMapLabelTable(curOutVol)) = *(myVol->getMapLabelTable(initialFrame));
                    } else {
                        *(volumeOut->getMapPaletteColorMapping(curOutVol)) = *(myVol->getMapPaletteColorMapping(initialFrame));
                    }
                    ++curOutVol;
                }
            }
        } else {
            vector<int64_t> myDims = myVol->getDimensions();
            for (int64_t b = 0; b < myDims[3]; ++b)
            {
                for (int64_t c = 0; c < firstDims[4]; ++c)
                {
                    volumeOut->setFrame(myVol->getFrame(b, c), curOutVol, c);
                }
                volumeOut->setMapName(curOutVol, myVol->getMapName(b));
                if (isLabel)
                {
                    *(volumeOut->getMapLabelTable(curOutVol)) = *(myVol->getMapLabelTable(b));
                } else {
                    *(volumeOut->getMapPaletteColorMapping(curOutVol)) = *(myVol->getMapPaletteColorMapping(b));
                }
                ++curOutVol;
            }
        }
    }
    CaretAssert(curOutVol == subvolCount);
}
