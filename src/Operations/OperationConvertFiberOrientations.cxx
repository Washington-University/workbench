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

#include "OperationConvertFiberOrientations.h"
#include "OperationException.h"
#include "CiftiFile.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString OperationConvertFiberOrientations::getCommandSwitch()
{
    return "-convert-fiber-orientations";
}

AString OperationConvertFiberOrientations::getShortDescription()
{
    return "CONVERT BINGHAM PARAMETER VOLUMES TO FIBER ORIENTATION FILE";
}

OperationParameters* OperationConvertFiberOrientations::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "label-volume", "volume of cifti structure labels");

    ret->addCiftiOutputParameter(2, "fiber-out", "the output fiber orientation file");
    
    ParameterComponent* fiberOpt = ret->createRepeatableParameter(3, "-fiber", "specify the parameter volumes for a fiber");
    fiberOpt->addVolumeParameter(1, "mean-f", "mean fiber strength");
    fiberOpt->addVolumeParameter(2, "stdev-f", "standard deviation of fiber strength");
    fiberOpt->addVolumeParameter(3, "theta", "theta angle");
    fiberOpt->addVolumeParameter(4, "phi", "phi angle");
    fiberOpt->addVolumeParameter(5, "psi", "psi angle");
    fiberOpt->addVolumeParameter(6, "ka", "ka bingham parameter");
    fiberOpt->addVolumeParameter(7, "kb", "kb bingham parameter");
    AString myText = AString("Takes precomputed bingham parameters from volume files and converts them to the format workbench uses for display.  ") +
        "The <label-volume> argument must be a label volume, where the labels use these strings:\n\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        myText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    ret->setHelpText(myText);
    return ret;
}

void OperationConvertFiberOrientations::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    VolumeFile* labelVol = myParams->getVolume(1);
    if (labelVol->getType() != SubvolumeAttributes::LABEL)
    {
        throw OperationException("<label-volume> must have a label table, see -volume-label-import");
    }
    CiftiFile* ciftiOut = myParams->getOutputCifti(2);
    const vector<ParameterComponent*>& myInstances = myParams->getRepeatableParameterInstances(3);
    int numFibers = (int)myInstances.size();
    if (numFibers < 1) throw OperationException("must specify -fiber at least once");
    if (numFibers > 3) throw OperationException("only three fibers are supported at this time");
    for (int i = 0; i < numFibers; ++i)
    {
        VolumeFile* meanfvol = myInstances[i]->getVolume(1);
        VolumeFile* stdevfvol = myInstances[i]->getVolume(2);
        VolumeFile* thetavol = myInstances[i]->getVolume(3);
        VolumeFile* phivol = myInstances[i]->getVolume(4);
        VolumeFile* psivol = myInstances[i]->getVolume(5);
        VolumeFile* kavol = myInstances[i]->getVolume(6);
        VolumeFile* kbvol = myInstances[i]->getVolume(7);
        if (!labelVol->matchesVolumeSpace(meanfvol) ||
            !labelVol->matchesVolumeSpace(stdevfvol) ||
            !labelVol->matchesVolumeSpace(thetavol) ||
            !labelVol->matchesVolumeSpace(phivol) ||
            !labelVol->matchesVolumeSpace(psivol) ||
            !labelVol->matchesVolumeSpace(kavol) ||
            !labelVol->matchesVolumeSpace(kbvol))
        {
            throw OperationException("all inputs must be in the same volume space");
        }
    }
    map<int, StructureEnum::Enum> labelMap;//maps label values to structures
    vector<vector<voxelIndexType> > voxelLists;//voxel lists for each volume component
    map<StructureEnum::Enum, int> componentMap;//maps structures to indexes in voxelLists
    const GiftiLabelTable* myLabelTable = labelVol->getMapLabelTable(0);
    vector<int32_t> labelKeys;
    myLabelTable->getKeys(labelKeys);
    int count = 0;
    for (int i = 0; i < (int)labelKeys.size(); ++i)
    {
        bool ok = false;
        StructureEnum::Enum thisStructure = StructureEnum::fromName(myLabelTable->getLabelName(labelKeys[i]), &ok);
        if (ok)
        {
            if (componentMap.find(thisStructure) == componentMap.end())//make sure we don't already have this structure from another label
            {
                labelMap[labelKeys[i]] = thisStructure;
                componentMap[thisStructure] = count;
                ++count;
            }
        }
    }
    voxelLists.resize(count);
    vector<int64_t> mydims;
    labelVol->getDimensions(mydims);
    for (int64_t k = 0; k < mydims[2]; ++k)
    {
        for (int64_t j = 0; j < mydims[1]; ++j)
        {
            for (int64_t i = 0; i < mydims[0]; ++i)
            {
                int myval = (int)floor(labelVol->getValue(i, j, k) + 0.5f);
                map<int, StructureEnum::Enum>::iterator myiter = labelMap.find(myval);
                if (myiter != labelMap.end())
                {
                    int whichList = componentMap.find(myiter->second)->second;//this should always find one, so we don't need to check for being end
                    voxelLists[whichList].push_back(i);
                    voxelLists[whichList].push_back(j);
                    voxelLists[whichList].push_back(k);
                }
            }
        }
    }
    int64_t ciftiVolDims[3];
    ciftiVolDims[0] = mydims[0];
    ciftiVolDims[1] = mydims[1];
    ciftiVolDims[2] = mydims[2];
    CiftiXMLOld myXML;
    myXML.resetColumnsToBrainModels();
    myXML.setVolumeDimsAndSForm(ciftiVolDims, labelVol->getSform());
    for (map<StructureEnum::Enum, int>::iterator myiter = componentMap.begin(); myiter != componentMap.end(); ++myiter)
    {
        myXML.addVolumeModelToColumns(voxelLists[myiter->second], myiter->first);
    }
    myXML.resetRowsToScalars(24);
    myXML.setMapNameForRowIndex(0, "x coord");
    myXML.setMapNameForRowIndex(1, "y coord");
    myXML.setMapNameForRowIndex(2, "z coord");
    myXML.setMapNameForRowIndex(3, "mean f1");
    myXML.setMapNameForRowIndex(4, "stdev f1");
    myXML.setMapNameForRowIndex(5, "theta1");
    myXML.setMapNameForRowIndex(6, "phi1");
    myXML.setMapNameForRowIndex(7, "ka1");
    myXML.setMapNameForRowIndex(8, "kb1");
    myXML.setMapNameForRowIndex(9, "psi1");
    myXML.setMapNameForRowIndex(10, "mean f2");
    myXML.setMapNameForRowIndex(11, "stdev f2");
    myXML.setMapNameForRowIndex(12, "theta2");
    myXML.setMapNameForRowIndex(13, "phi2");
    myXML.setMapNameForRowIndex(14, "ka2");
    myXML.setMapNameForRowIndex(15, "kb2");
    myXML.setMapNameForRowIndex(16, "psi2");
    myXML.setMapNameForRowIndex(17, "mean f3");
    myXML.setMapNameForRowIndex(18, "stdev f3");
    myXML.setMapNameForRowIndex(19, "theta3");
    myXML.setMapNameForRowIndex(20, "phi3");
    myXML.setMapNameForRowIndex(21, "ka3");
    myXML.setMapNameForRowIndex(22, "kb3");
    myXML.setMapNameForRowIndex(23, "psi3");
    ciftiOut->setCiftiXML(myXML);
    vector<CiftiVolumeMap> volMap;
    CaretArray<float> temprow(24, 0.0f);
    temprow[14] = 1.0f;//do not put zeros in ka and kb, ever
    temprow[15] = 1.0f;
    temprow[21] = 1.0f;
    temprow[22] = 1.0f;
    myXML.getVolumeMapForColumns(volMap);//we don't need to know which voxel is from which parcel
    int64_t end = (int64_t)volMap.size();
    for (int64_t i = 0; i < end; ++i)
    {
        labelVol->indexToSpace(volMap[i].m_ijk, temprow);//first three elements are the coordinates
        for (int j = 0; j < numFibers; ++j)
        {
            int base = 7 * j + 3;
            VolumeFile* meanfvol = myInstances[j]->getVolume(1);
            VolumeFile* stdevfvol = myInstances[j]->getVolume(2);
            VolumeFile* thetavol = myInstances[j]->getVolume(3);
            VolumeFile* phivol = myInstances[j]->getVolume(4);
            VolumeFile* psivol = myInstances[j]->getVolume(5);
            VolumeFile* kavol = myInstances[j]->getVolume(6);
            VolumeFile* kbvol = myInstances[j]->getVolume(7);
            temprow[base] = meanfvol->getValue(volMap[i].m_ijk);
            temprow[base + 1] = stdevfvol->getValue(volMap[i].m_ijk);
            temprow[base + 2] = thetavol->getValue(volMap[i].m_ijk);
            temprow[base + 3] = phivol->getValue(volMap[i].m_ijk);
            temprow[base + 4] = kavol->getValue(volMap[i].m_ijk);
            temprow[base + 5] = kbvol->getValue(volMap[i].m_ijk);
            temprow[base + 6] = psivol->getValue(volMap[i].m_ijk);
        }
        ciftiOut->setRow(temprow, volMap[i].m_ciftiIndex);
    }
}
