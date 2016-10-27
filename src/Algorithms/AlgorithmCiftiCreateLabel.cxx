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
#include "StructureEnum.h"
#include "VolumeFile.h"
#include <map>
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
    volumeOpt->addVolumeParameter(1, "label-volume", "volume file containing the label data");
    volumeOpt->addVolumeParameter(2, "parcel-volume", "label volume file with cifti structure names to define the volume parcels");
    
    OptionalParameter* leftLabelOpt = ret->createOptionalParameter(3, "-left-label", "label file for left surface");
    leftLabelOpt->addLabelParameter(1, "label", "the label file");
    OptionalParameter* leftRoiOpt = leftLabelOpt->createOptionalParameter(2, "-roi-left", "roi of vertices to use from left surface");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* rightLabelOpt = ret->createOptionalParameter(4, "-right-label", "label for left surface");
    rightLabelOpt->addLabelParameter(1, "label", "the label file");
    OptionalParameter* rightRoiOpt = rightLabelOpt->createOptionalParameter(2, "-roi-right", "roi of vertices to use from right surface");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* cerebLabelOpt = ret->createOptionalParameter(5, "-cerebellum-label", "label for the cerebellum");
    cerebLabelOpt->addLabelParameter(1, "label", "the label file");
    OptionalParameter* cerebRoiOpt = cerebLabelOpt->createOptionalParameter(2, "-roi-cerebellum", "roi of vertices to use from right surface");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    AString myText = AString("All input files must have the same number of columns/subvolumes.  Only the specified components will be in the output cifti.  ") +
        "At least one component must be specified.\n\n" +
        "The -volume option of -cifti-create-label requires two volume arguments, " +
        "the label-volume argument contains all labels you want to display (e.g. nuclei of the thalamus), " +
        "whereas the parcel-volume argument includes all CIFTI structures you want to include data within (e.g. THALAMUS_LEFT, THALAMUS_RIGHT).  " +
        "If you just want the labels in voxels to be the structure names, you may use the same file for both arguments.  " +
        "The parcel-volume must use some of the label names from this list, all other label names in the parcel-volume will be ignored:\n";
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
    OptionalParameter* leftLabelOpt = myParams->getOptionalParameter(3);
    if (leftLabelOpt->m_present)
    {
        leftData = leftLabelOpt->getLabel(1);
        OptionalParameter* leftRoiOpt = leftLabelOpt->getOptionalParameter(2);
        if (leftRoiOpt->m_present)
        {
            leftRoi = leftRoiOpt->getMetric(1);
        }
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
    }
    AlgorithmCiftiCreateLabel(myProgObj, myCiftiOut, myVol, myVolLabel, leftData, leftRoi, rightData, rightRoi, cerebData, cerebRoi);
}

AlgorithmCiftiCreateLabel::AlgorithmCiftiCreateLabel(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol,
                                                                         const VolumeFile* myVolLabel, const LabelFile* leftData, const MetricFile* leftRoi,
                                                                         const LabelFile* rightData, const MetricFile* rightRoi, const LabelFile* cerebData,
                                                                         const MetricFile* cerebRoi) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(myCiftiOut != NULL);
    LevelProgress myProgress(myProgObj);
    CiftiXMLOld myXML;
    myXML.resetColumnsToBrainModels();
    int numMaps = -1;
    if (leftData != NULL)
    {
        numMaps = leftData->getNumberOfMaps();
        if (leftRoi == NULL)
        {
            myXML.addSurfaceModelToColumns(leftData->getNumberOfNodes(), StructureEnum::CORTEX_LEFT);
        } else {
            if (leftRoi->getNumberOfNodes() != leftData->getNumberOfNodes())
            {
                throw AlgorithmException("left surface ROI and data have different vertex counts");
            }
            myXML.addSurfaceModelToColumns(leftData->getNumberOfNodes(), StructureEnum::CORTEX_LEFT, leftRoi->getValuePointerForColumn(0));
        }
    }
    if (rightData != NULL)
    {
        if (numMaps == -1)
        {
            numMaps = rightData->getNumberOfMaps();
        } else {
            if (numMaps != rightData->getNumberOfMaps())
            {
                throw AlgorithmException("right and left surface data have a different number of maps");
            }
        }
        if (rightRoi == NULL)
        {
            myXML.addSurfaceModelToColumns(rightData->getNumberOfNodes(), StructureEnum::CORTEX_RIGHT);
        } else {
            if (rightRoi->getNumberOfNodes() != rightData->getNumberOfNodes())
            {
                throw AlgorithmException("right surface ROI and data have different vertex counts");
            }
            myXML.addSurfaceModelToColumns(rightRoi->getNumberOfNodes(), StructureEnum::CORTEX_RIGHT, rightRoi->getValuePointerForColumn(0));
        }
    }
    if (cerebData != NULL)
    {
        if (numMaps == -1)
        {
            numMaps = cerebData->getNumberOfMaps();
        } else {
            if (numMaps != cerebData->getNumberOfMaps())
            {
                throw AlgorithmException("cerebellum surface data has a different number of maps");
            }
        }
        if (cerebRoi == NULL)
        {
            myXML.addSurfaceModelToColumns(cerebData->getNumberOfNodes(), StructureEnum::CEREBELLUM);
        } else {
            if (cerebRoi->getNumberOfNodes() != cerebData->getNumberOfNodes())
            {
                throw AlgorithmException("cerebellum surface ROI and data have different vertex counts");
            }
            myXML.addSurfaceModelToColumns(cerebRoi->getNumberOfNodes(), StructureEnum::CEREBELLUM, cerebRoi->getValuePointerForColumn(0));
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
    vector<map<int32_t, int32_t> > surfLeftConvert(numMaps), surfRightConvert(numMaps), surfCerebConvert(numMaps), volConvert(numMaps);//surfLeftConvert could just be a set, but for consistency...
    for (int i = 0; i < numMaps; ++i)
    {
        GiftiLabelTable mapTable;//NOTE: this relies on GiftiLabelTable::append doing the right thing
        bool first = true;
        if (leftData != NULL)
        {
            surfLeftConvert[i] = mapTable.append(*(leftData->getMapLabelTable(i)));//in case label files ever move to one table per map
            if (first)
            {
                first = false;
                myXML.setMapNameForRowIndex(i, leftData->getColumnName(i));
            }
        }
        if (rightData != NULL)
        {
            surfRightConvert[i] = mapTable.append(*(rightData->getMapLabelTable(i)));
            if (first)
            {
                first = false;
                myXML.setMapNameForRowIndex(i, rightData->getColumnName(i));
            }
        }
        if (cerebData != NULL)
        {
            surfCerebConvert[i] = mapTable.append(*(cerebData->getMapLabelTable(i)));
            if (first)
            {
                first = false;
                myXML.setMapNameForRowIndex(i, cerebData->getColumnName(i));
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
    if (myXML.getSurfaceMapForColumns(surfMap, StructureEnum::CORTEX_LEFT))
    {
        for (int64_t i = 0; i < (int)surfMap.size(); ++i)
        {
            for (int t = 0; t < numMaps; ++t)
            {
                map<int32_t, int32_t>::iterator iter = surfLeftConvert[t].find(leftData->getLabelKey(surfMap[i].m_surfaceNode, t));
                if (iter == surfLeftConvert[t].end())
                {
                    temprow[t] = 0;//NOTE: this relies on 0 being the unused label, from the default constructor of the label table, and not being overwritten by append
                } else {
                    temprow[t] = iter->second;
                }
            }
            myCiftiOut->setRow(temprow, surfMap[i].m_ciftiIndex);
        }
    }
    if (myXML.getSurfaceMapForColumns(surfMap, StructureEnum::CORTEX_RIGHT))
    {
        for (int64_t i = 0; i < (int)surfMap.size(); ++i)
        {
            for (int t = 0; t < numMaps; ++t)
            {
                map<int32_t, int32_t>::iterator iter = surfRightConvert[t].find(rightData->getLabelKey(surfMap[i].m_surfaceNode, t));
                if (iter == surfRightConvert[t].end())
                {
                    temprow[t] = 0;
                } else {
                    temprow[t] = iter->second;
                }
            }
            myCiftiOut->setRow(temprow, surfMap[i].m_ciftiIndex);
        }
    }
    if (myXML.getSurfaceMapForColumns(surfMap, StructureEnum::CEREBELLUM))
    {
        for (int64_t i = 0; i < (int)surfMap.size(); ++i)
        {
            for (int t = 0; t < numMaps; ++t)
            {
                map<int32_t, int32_t>::iterator iter = surfCerebConvert[t].find(cerebData->getLabelKey(surfMap[i].m_surfaceNode, t));
                if (iter == surfCerebConvert[t].end())
                {
                    temprow[t] = 0;
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
                map<int32_t, int32_t>::iterator iter = volConvert[t].find(myVol->getValue(volMap[i].m_ijk, t));
                if (iter == volConvert[t].end())
                {
                    temprow[t] = 0;
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
