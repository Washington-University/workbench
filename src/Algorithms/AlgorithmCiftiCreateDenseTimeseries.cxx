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

#include "AlgorithmCiftiCreateDenseTimeseries.h"
#include "AlgorithmException.h"
#include "StructureEnum.h"
#include "VolumeFile.h"
#include "MetricFile.h"
#include "CiftiFile.h"
#include "CaretAssert.h"
#include "GiftiLabelTable.h"
#include "CaretPointer.h"
#include <map>
#include <vector>
#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmCiftiCreateDenseTimeseries::getCommandSwitch()
{
    return "-cifti-create-dense-timeseries";
}

AString AlgorithmCiftiCreateDenseTimeseries::getShortDescription()
{
    return "CREATE A CIFTI DENSE TIMESERIES";
}

OperationParameters* AlgorithmCiftiCreateDenseTimeseries::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiOutputParameter(1, "cifti-out", "the output cifti file");
    
    OptionalParameter* volumeOpt = ret->createOptionalParameter(2, "-volume", "volume component");
    volumeOpt->addVolumeParameter(1, "volume-data", "volume file containing all voxel data for all volume structures");
    volumeOpt->addVolumeParameter(2, "label-volume", "label volume file containing labels for cifti structures");
    
    OptionalParameter* leftMetricOpt = ret->createOptionalParameter(3, "-left-metric", "metric for left surface");
    leftMetricOpt->addMetricParameter(1, "metric", "the metric file");
    OptionalParameter* leftRoiOpt = leftMetricOpt->createOptionalParameter(2, "-roi-left", "roi of vertices to use from left surface");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* rightMetricOpt = ret->createOptionalParameter(4, "-right-metric", "metric for left surface");
    rightMetricOpt->addMetricParameter(1, "metric", "the metric file");
    OptionalParameter* rightRoiOpt = rightMetricOpt->createOptionalParameter(2, "-roi-right", "roi of vertices to use from right surface");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* cerebMetricOpt = ret->createOptionalParameter(5, "-cerebellum-metric", "metric for the cerebellum");
    cerebMetricOpt->addMetricParameter(1, "metric", "the metric file");
    OptionalParameter* cerebRoiOpt = cerebMetricOpt->createOptionalParameter(2, "-roi-cerebellum", "roi of vertices to use from right surface");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* timestepOpt = ret->createOptionalParameter(6, "-timestep", "set the timestep");
    timestepOpt->addDoubleParameter(1, "interval", "the timestep, in seconds (default 1.0)");
    
    OptionalParameter* timestartOpt = ret->createOptionalParameter(7, "-timestart", "set the start time");
    timestartOpt->addDoubleParameter(1, "start", "the time at the first frame, in seconds (default 0.0)");
    
    AString myText = AString("All input files must have the same number of columns/subvolumes.  Only the specified components will be in the output cifti.  ") +
        "At least one component must be specified.  The label volume should have some of the label names from this list, all other label names will be ignored:\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        myText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    ret->setHelpText(myText);
    return ret;
}

void AlgorithmCiftiCreateDenseTimeseries::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVol = NULL, *myVolLabel = NULL;
    CiftiFile* myCiftiOut = myParams->getOutputCifti(1);
    OptionalParameter* volumeOpt = myParams->getOptionalParameter(2);
    if (volumeOpt->m_present)
    {
        myVol = volumeOpt->getVolume(1);
        myVolLabel = volumeOpt->getVolume(2);
    }
    MetricFile* leftData = NULL, *leftRoi = NULL, *rightData = NULL, *rightRoi = NULL, *cerebData = NULL, *cerebRoi = NULL;
    OptionalParameter* leftMetricOpt = myParams->getOptionalParameter(3);
    if (leftMetricOpt->m_present)
    {
        leftData = leftMetricOpt->getMetric(1);
        OptionalParameter* leftRoiOpt = leftMetricOpt->getOptionalParameter(2);
        if (leftRoiOpt->m_present)
        {
            leftRoi = leftRoiOpt->getMetric(1);
        }
    }
    OptionalParameter* rightMetricOpt = myParams->getOptionalParameter(4);
    if (rightMetricOpt->m_present)
    {
        rightData = rightMetricOpt->getMetric(1);
        OptionalParameter* rightRoiOpt = rightMetricOpt->getOptionalParameter(2);
        if (rightRoiOpt->m_present)
        {
            rightRoi = rightRoiOpt->getMetric(1);
        }
    }
    OptionalParameter* cerebMetricOpt = myParams->getOptionalParameter(5);
    if (cerebMetricOpt->m_present)
    {
        cerebData = cerebMetricOpt->getMetric(1);
        OptionalParameter* cerebRoiOpt = cerebMetricOpt->getOptionalParameter(2);
        if (cerebRoiOpt->m_present)
        {
            cerebRoi = cerebRoiOpt->getMetric(1);
        }
    }
    float timestep = 1.0f;
    OptionalParameter* timestepOpt = myParams->getOptionalParameter(6);
    if (timestepOpt->m_present)
    {
        timestep = (float)timestepOpt->getDouble(1);
    }
    float timestart = 0.0f;
    OptionalParameter* timestartOpt = myParams->getOptionalParameter(7);
    if (timestartOpt->m_present)
    {
        timestart = (float)timestartOpt->getDouble(1);
    }
    AlgorithmCiftiCreateDenseTimeseries(myProgObj, myCiftiOut, myVol, myVolLabel, leftData, leftRoi, rightData, rightRoi, cerebData, cerebRoi, timestep, timestart);
}

AlgorithmCiftiCreateDenseTimeseries::AlgorithmCiftiCreateDenseTimeseries(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol,
                                                                         const VolumeFile* myVolLabel, const MetricFile* leftData, const MetricFile* leftRoi,
                                                                         const MetricFile* rightData, const MetricFile* rightRoi, const MetricFile* cerebData,
                                                                         const MetricFile* cerebRoi, const float& timestep, const float& timestart) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(myCiftiOut != NULL);
    LevelProgress myProgress(myProgObj);
    CiftiXMLOld myXML;
    makeDenseMapping(myXML, CiftiXMLOld::ALONG_COLUMN, myVol, myVolLabel, leftData, leftRoi, rightData, rightRoi, cerebData, cerebRoi);
    int numMaps = -1;
    if (leftData != NULL)
    {
        numMaps = leftData->getNumberOfMaps();
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
    }
    if (myVol != NULL)
    {
        if (numMaps == -1)
        {
            numMaps = myVol->getNumberOfMaps();
        } else {
            if (numMaps != myVol->getNumberOfMaps())
            {
                throw AlgorithmException("volume data has a different number of maps");
            }
        }
    }
    if (numMaps == -1)
    {
        throw AlgorithmException("no models specified");
    }
    myXML.resetRowsToTimepoints(timestep, numMaps, timestart);
    myCiftiOut->setCiftiXML(myXML);
    CaretArray<float> temprow(numMaps);
    vector<CiftiSurfaceMap> surfMap;
    if (myXML.getSurfaceMapForColumns(surfMap, StructureEnum::CORTEX_LEFT))
    {
        for (int64_t i = 0; i < (int)surfMap.size(); ++i)
        {
            for (int t = 0; t < numMaps; ++t)
            {
                temprow[t] = leftData->getValue(surfMap[i].m_surfaceNode, t);
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
                temprow[t] = rightData->getValue(surfMap[i].m_surfaceNode, t);
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
                temprow[t] = cerebData->getValue(surfMap[i].m_surfaceNode, t);
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
                temprow[t] = myVol->getValue(volMap[i].m_ijk, t);
            }
            myCiftiOut->setRow(temprow, volMap[i].m_ciftiIndex);
        }
    }
}

void AlgorithmCiftiCreateDenseTimeseries::makeDenseMapping(CiftiXMLOld& toModify, const int& direction,
                                                           const VolumeFile* myVol, const VolumeFile* myVolLabel,
                                                           const MetricFile* leftData, const MetricFile* leftRoi,
                                                           const MetricFile* rightData, const MetricFile* rightRoi,
                                                           const MetricFile* cerebData, const MetricFile* cerebRoi)
{
    bool noData = true;
    toModify.resetDirectionToBrainModels(direction);
    if (leftData != NULL)
    {
        noData = false;
        if (leftRoi == NULL)
        {
            toModify.addSurfaceModel(direction, leftData->getNumberOfNodes(), StructureEnum::CORTEX_LEFT);
        } else {
            if (leftRoi->getNumberOfNodes() != leftData->getNumberOfNodes())
            {
                throw AlgorithmException("left surface ROI and data have different vertex counts");
            }
            toModify.addSurfaceModel(direction, leftData->getNumberOfNodes(), StructureEnum::CORTEX_LEFT, leftRoi->getValuePointerForColumn(0));
        }
    }
    if (rightData != NULL)
    {
        noData = false;
        if (rightRoi == NULL)
        {
            toModify.addSurfaceModel(direction, rightData->getNumberOfNodes(), StructureEnum::CORTEX_RIGHT);
        } else {
            if (rightRoi->getNumberOfNodes() != rightData->getNumberOfNodes())
            {
                throw AlgorithmException("right surface ROI and data have different vertex counts");
            }
            toModify.addSurfaceModel(direction, rightRoi->getNumberOfNodes(), StructureEnum::CORTEX_RIGHT, rightRoi->getValuePointerForColumn(0));
        }
    }
    if (cerebData != NULL)
    {
        noData = false;
        if (cerebRoi == NULL)
        {
            toModify.addSurfaceModel(direction, cerebData->getNumberOfNodes(), StructureEnum::CEREBELLUM);
        } else {
            if (cerebRoi->getNumberOfNodes() != cerebData->getNumberOfNodes())
            {
                throw AlgorithmException("cerebellum surface ROI and data have different vertex counts");
            }
            toModify.addSurfaceModel(direction, cerebRoi->getNumberOfNodes(), StructureEnum::CEREBELLUM, cerebRoi->getValuePointerForColumn(0));
        }
    }
    if (myVol != NULL)
    {
        CaretAssert(myVolLabel != NULL);
        if (myVolLabel == NULL)
        {
            throw AlgorithmException("making a dense mapping with voxels requires a label volume");
        }
        if (!myVol->matchesVolumeSpace(myVolLabel))
        {
            throw AlgorithmException("label volume has a different volume space than data volume");
        }
        if (myVolLabel->getType() != SubvolumeAttributes::LABEL)
        {
            throw AlgorithmException("parcel volume is not of type label");
        }
        noData = false;
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
                labelMap[labelKeys[i]] = thisStructure;
                if (componentMap.find(thisStructure) == componentMap.end())//make sure we don't already have this structure from another label
                {
                    componentMap[thisStructure] = count;
                    ++count;
                }
            }
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
        toModify.setVolumeDimsAndSForm(ciftiVolDims, myVol->getSform());
        for (map<StructureEnum::Enum, int>::iterator myiter = componentMap.begin(); myiter != componentMap.end(); ++myiter)
        {
            toModify.addVolumeModel(direction, voxelLists[myiter->second], myiter->first);
        }
    }
    if (noData)
    {
        throw AlgorithmException("no models specified");
    }
}


float AlgorithmCiftiCreateDenseTimeseries::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiCreateDenseTimeseries::getSubAlgorithmWeight()
{
    return 0.0f;
}
