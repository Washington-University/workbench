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

#include "AlgorithmCiftiCreateLabel.h"
#include "AlgorithmException.h"
#include "CaretAssert.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"

#include <vector>
#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmCiftiCreateLabel::getCommandSwitch()
{
    return "-cifti-create-label";
}

AString AlgorithmCiftiCreateLabel::getShortDescription()
{
    return "CREATE A CIFTI LABEL FILE";
}

OperationParameters* AlgorithmCiftiCreateLabel::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiOutputParameter(1, "cifti-out", "the output cifti file");
    
    OptionalParameter* volumeOpt = ret->createOptionalParameter(2, "-volume", "volume component");
    volumeOpt->addVolumeParameter(1, "label-volume", "label volume file containing the data to be copied");
    volumeOpt->addVolumeParameter(2, "structure-label-volume", "label volume file that defines which voxels to use");
    
    OptionalParameter* leftLabelOpt = ret->createOptionalParameter(3, "-left-label", "label file for the left surface");
    leftLabelOpt->addLabelParameter(1, "label", "the label file");
    OptionalParameter* leftRoiOpt = leftLabelOpt->createOptionalParameter(2, "-roi-left", "roi of vertices to use from left surface");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* rightLabelOpt = ret->createOptionalParameter(4, "-right-label", "label file for the right surface");
    rightLabelOpt->addLabelParameter(1, "label", "the label file");
    OptionalParameter* rightRoiOpt = rightLabelOpt->createOptionalParameter(2, "-roi-right", "roi of vertices to use from right surface");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* cerebLabelOpt = ret->createOptionalParameter(5, "-cerebellum-label", "label file for the cerebellum");
    cerebLabelOpt->addLabelParameter(1, "label", "the label file");
    OptionalParameter* cerebRoiOpt = cerebLabelOpt->createOptionalParameter(2, "-roi-cerebellum", "roi of vertices to use from right surface");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    ParameterComponent* genLabelOpt = ret->createRepeatableParameter(6, "-label", "label for a specified surface structure");
    genLabelOpt->addStringParameter(1, "structure", "the structure name");
    genLabelOpt->addLabelParameter(2, "label", "the label file");
    OptionalParameter* genRoiOpt = genLabelOpt->createOptionalParameter(3, "-roi", "roi of vertices to use from this structure");
    genRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    AString myText = AString("All input files must have the same number of columns/subvolumes.  Only the specified components will be in the output cifti.  ") +
        "At least one component must be specified.\n\n" +
        "The -volume option requires two volume arguments, " +
        "the label-volume argument contains all labels you want to display (e.g. nuclei of the thalamus), " +
        "whereas the structure-label-volume argument contains all CIFTI voxel-based structures you want to include data within (e.g. THALAMUS_LEFT, THALAMUS_RIGHT, etc).  " +
        "See -volume-label-import and -volume-help for format details of label volume files.  " +
        "If you just want the labels in voxels to be the structure names, you may use the same file for both arguments.  " +
        "The structure-label-volume must use some of the label names from this list, all other label names in the structure-label-volume will be ignored:\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        myText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    ret->setHelpText(myText);
    return ret;
}

void AlgorithmCiftiCreateLabel::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVol = NULL, *myVolLabel = NULL;
    CiftiFile* myCiftiOut = myParams->getOutputCifti(1);
    OptionalParameter* volumeOpt = myParams->getOptionalParameter(2);
    if (volumeOpt->m_present)
    {
        myVol = volumeOpt->getVolume(1);
        myVolLabel = volumeOpt->getVolume(2);
    }
    LabelFile* leftData = NULL, *rightData = NULL, *cerebData = NULL;
    MetricFile* leftRoi = NULL, *rightRoi = NULL, *cerebRoi = NULL;
    map<StructureEnum::Enum, SurfParam> surfParams;
    OptionalParameter* leftLabelOpt = myParams->getOptionalParameter(3);
    if (leftLabelOpt->m_present)
    {
        leftData = leftLabelOpt->getLabel(1);
        OptionalParameter* leftRoiOpt = leftLabelOpt->getOptionalParameter(2);
        if (leftRoiOpt->m_present)
        {
            leftRoi = leftRoiOpt->getMetric(1);
        }
        surfParams[StructureEnum::CORTEX_LEFT] = SurfParam(leftData, leftRoi);
    }
    OptionalParameter* rightLabelOpt = myParams->getOptionalParameter(4);
    if (rightLabelOpt->m_present)
    {
        rightData = rightLabelOpt->getLabel(1);
        OptionalParameter* rightRoiOpt = rightLabelOpt->getOptionalParameter(2);
        if (rightRoiOpt->m_present)
        {
            rightRoi = rightRoiOpt->getMetric(1);
        }
        surfParams[StructureEnum::CORTEX_RIGHT] = SurfParam(rightData, rightRoi);
    }
    OptionalParameter* cerebLabelOpt = myParams->getOptionalParameter(5);
    if (cerebLabelOpt->m_present)
    {
        cerebData = cerebLabelOpt->getLabel(1);
        OptionalParameter* cerebRoiOpt = cerebLabelOpt->getOptionalParameter(2);
        if (cerebRoiOpt->m_present)
        {
            cerebRoi = cerebRoiOpt->getMetric(1);
        }
        surfParams[StructureEnum::CEREBELLUM] = SurfParam(cerebData, cerebRoi);
    }
    auto genLabelOpts = myParams->getRepeatableParameterInstances(6);
    for (auto instance : genLabelOpts)
    {
        bool ok = false;
        StructureEnum::Enum structure = StructureEnum::fromName(instance->getString(1), &ok);
        if (!ok) throw AlgorithmException("unrecognized structure identifier: " + instance->getString(1));
        if (surfParams.find(structure) != surfParams.end()) throw AlgorithmException(instance->getString(1) + " structure specified more than once");
        OptionalParameter* genRoiOpt = instance->getOptionalParameter(3);
        if (genRoiOpt->m_present)
        {
            surfParams[structure] = SurfParam(instance->getLabel(2), genRoiOpt->getMetric(1));
        } else {
            surfParams[structure] = SurfParam(instance->getLabel(2));
        }
    }
    AlgorithmCiftiCreateLabel(myProgObj, myCiftiOut, myVol, myVolLabel, surfParams);
}

AlgorithmCiftiCreateLabel::AlgorithmCiftiCreateLabel(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol, const VolumeFile* myVolLabel,
                                                     const LabelFile* leftData, const MetricFile* leftRoi,
                                                     const LabelFile* rightData, const MetricFile* rightRoi,
                                                     const LabelFile* cerebData, const MetricFile* cerebRoi) : AbstractAlgorithm(NULL)
{
    map<StructureEnum::Enum, SurfParam> surfParams;
    if (leftData != NULL) surfParams[StructureEnum::CORTEX_LEFT] = SurfParam(leftData, leftRoi);
    if (rightData != NULL) surfParams[StructureEnum::CORTEX_RIGHT] = SurfParam(rightData, rightRoi);
    if (cerebData != NULL) surfParams[StructureEnum::CEREBELLUM] = SurfParam(cerebData, cerebRoi);
    AlgorithmCiftiCreateLabel(myProgObj, myCiftiOut, myVol, myVolLabel, surfParams);
}

AlgorithmCiftiCreateLabel::AlgorithmCiftiCreateLabel(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol, const VolumeFile* myVolLabel,
                                                     const map<StructureEnum::Enum, SurfParam> surfParams) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(myCiftiOut != NULL);
    LevelProgress myProgress(myProgObj);
    CiftiXMLOld myXML;
    myXML.resetColumnsToBrainModels();
    int numMaps = -1;
    StructureEnum::Enum firstSurf = StructureEnum::INVALID;
    for (auto param : surfParams)
    {
        if (numMaps == -1)
        {
            numMaps = param.second.data->getNumberOfMaps();
            firstSurf = param.first;
        } else {
            if (numMaps != param.second.data->getNumberOfMaps())
            {
                throw AlgorithmException(StructureEnum::toName(firstSurf) + " and " + StructureEnum::toName(param.first) + " surface data have a different number of maps");
            }
        }
        if (param.second.roi == NULL)
        {
            myXML.addSurfaceModelToColumns(param.second.data->getNumberOfNodes(), param.first);
        } else {
            if (param.second.roi->getNumberOfNodes() != param.second.data->getNumberOfNodes())
            {
                throw AlgorithmException(StructureEnum::toName(param.first) + " surface ROI and data have different vertex counts");
            }
            myXML.addSurfaceModelToColumns(param.second.data->getNumberOfNodes(), param.first, param.second.roi->getValuePointerForColumn(0));
        }
    }
    if (myVol != NULL)
    {
        CaretAssert(myVolLabel != NULL);
        if (myVol->getType() != SubvolumeAttributes::LABEL)
        {
            throw AlgorithmException("input volume is not a label volume");
        }
        if (!myVol->matchesVolumeSpace(myVolLabel))
        {
            throw AlgorithmException("label volume has a different volume space than data volume");
        }
        if (myVolLabel->getType() != SubvolumeAttributes::LABEL)
        {
            throw AlgorithmException("parcel volume is not a label volume");
        }
        if (numMaps == -1)
        {
            numMaps = myVol->getNumberOfMaps();
        } else {
            if (numMaps != myVol->getNumberOfMaps())
            {
                throw AlgorithmException("volume data has a different number of maps");
            }
        }
        map<int, StructureEnum::Enum> labelMap;//maps label values to structures
        vector<vector<voxelIndexType> > voxelLists;//voxel lists for each volume component
        map<StructureEnum::Enum, int> componentMap;//maps structures to indexes in voxelLists
        const GiftiLabelTable* myLabelTable = myVolLabel->getMapLabelTable(0);
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
        if (count == 0)
        {
            throw AlgorithmException("volume label file does not contain any structure names");
        }
        voxelLists.resize(count);
        vector<int64_t> mydims;
        myVolLabel->getDimensions(mydims);
        for (int64_t k = 0; k < mydims[2]; ++k)
        {
            for (int64_t j = 0; j < mydims[1]; ++j)
            {
                for (int64_t i = 0; i < mydims[0]; ++i)
                {
                    int myval = (int)floor(myVolLabel->getValue(i, j, k) + 0.5f);
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
        myXML.setVolumeDimsAndSForm(ciftiVolDims, myVol->getSform());
        for (map<StructureEnum::Enum, int>::iterator myiter = componentMap.begin(); myiter != componentMap.end(); ++myiter)
        {
            if (!voxelLists[myiter->second].empty())
            {
                myXML.addVolumeModelToColumns(voxelLists[myiter->second], myiter->first);
            }
        }
        if (myXML.getNumberOfRows() == 0)
        {
            throw AlgorithmException("volume label file does not label any voxels as any structure");
        }
    }
    if (numMaps == -1)
    {
        throw AlgorithmException("no models specified");
    }
    myXML.resetRowsToLabels(numMaps);
    vector<map<int32_t, int32_t> > volConvert(numMaps);//surfLeftConvert could just be a set, but for consistency...
    map<StructureEnum::Enum, vector<map<int32_t, int32_t> > > surfConversions;
    for (int i = 0; i < numMaps; ++i)
    {
        GiftiLabelTable mapTable;//NOTE: this relies on GiftiLabelTable::append doing the right thing
        bool first = true;
        for (auto param : surfParams)
        {
            surfConversions[param.first].push_back(mapTable.append(*(param.second.data->getMapLabelTable(i))));//in case label files ever move to one table per map
            if (first)
            {
                first = false;
                myXML.setMapNameForRowIndex(i, param.second.data->getColumnName(i));
            }
        }
        if (myVol != NULL)
        {
            volConvert[i] = mapTable.append(*(myVol->getMapLabelTable(i)));
            if (first)
            {
                first = false;
                myXML.setMapNameForRowIndex(i, myVol->getMapName(i));
            }
        }
        myXML.setLabelTableForRowIndex(i, mapTable);
    }
    myCiftiOut->setCiftiXML(myXML);
    CaretArray<float> temprow(numMaps);
    vector<CiftiSurfaceMap> surfMap;
    for (auto param : surfParams)
    {
        bool ok = myXML.getSurfaceMapForColumns(surfMap, param.first);
        CaretAssert(ok);
        for (int64_t i = 0; i < (int)surfMap.size(); ++i)
        {
            for (int t = 0; t < numMaps; ++t)
            {
                auto iter = surfConversions[param.first][t].find(param.second.data->getLabelKey(surfMap[i].m_surfaceNode, t));
                if (iter == surfConversions[param.first][t].end())
                {//the conversion map also contains the identity mapping of keys that were used as-is, so anything not in it is bogus
                    temprow[t] = myXML.getLabelTableForRowIndex(t)->getUnassignedLabelKey();
                } else {
                    temprow[t] = iter->second;
                }
            }
            myCiftiOut->setRow(temprow, surfMap[i].m_ciftiIndex);
        }
    }
    vector<CiftiVolumeMap> volMap;
    if (myXML.getVolumeMapForColumns(volMap))//we don't need to know which voxel is from which parcel
    {
        for (int64_t i = 0; i < (int)volMap.size(); ++i)
        {
            for (int t = 0; t < numMaps; ++t)
            {
                auto iter = volConvert[t].find(myVol->getValue(volMap[i].m_ijk, t));
                if (iter == volConvert[t].end())
                {
                    temprow[t] = myXML.getLabelTableForRowIndex(t)->getUnassignedLabelKey();
                } else {
                    temprow[t] = iter->second;
                }
            }
            myCiftiOut->setRow(temprow, volMap[i].m_ciftiIndex);
        }
    }
}

float AlgorithmCiftiCreateLabel::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiCreateLabel::getSubAlgorithmWeight()
{
    return 0.0f;
}
