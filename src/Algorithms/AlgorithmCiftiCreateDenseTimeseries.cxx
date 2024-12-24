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

#include "AlgorithmCiftiCreateDenseTimeseries.h"
#include "AlgorithmException.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "GiftiLabelTable.h"
#include "MetricFile.h"
#include "PaletteColorMapping.h"
#include "StructureEnum.h"
#include "VolumeFile.h"

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
    volumeOpt->addVolumeParameter(2, "structure-label-volume", "label volume file containing labels for cifti structures");
    
    OptionalParameter* leftMetricOpt = ret->createOptionalParameter(3, "-left-metric", "metric for the left cortical surface");
    leftMetricOpt->addMetricParameter(1, "metric", "the metric file");
    OptionalParameter* leftRoiOpt = leftMetricOpt->createOptionalParameter(2, "-roi-left", "roi of vertices to use from left surface");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* rightMetricOpt = ret->createOptionalParameter(4, "-right-metric", "metric for the right cortical surface");
    rightMetricOpt->addMetricParameter(1, "metric", "the metric file");
    OptionalParameter* rightRoiOpt = rightMetricOpt->createOptionalParameter(2, "-roi-right", "roi of vertices to use from right surface");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* cerebMetricOpt = ret->createOptionalParameter(5, "-cerebellum-metric", "metric for the cerebellum");
    cerebMetricOpt->addMetricParameter(1, "metric", "the metric file");
    OptionalParameter* cerebRoiOpt = cerebMetricOpt->createOptionalParameter(2, "-roi-cerebellum", "roi of vertices to use from right surface");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    ParameterComponent* genMetricOpt = ret->createRepeatableParameter(9, "-metric", "metric for a specified surface structure");
    genMetricOpt->addStringParameter(1, "structure", "the structure name");
    genMetricOpt->addMetricParameter(2, "metric", "the metric file");
    OptionalParameter* genRoiopt = genMetricOpt->createOptionalParameter(3, "-roi", "roi of vertices to use from this structure");
    genRoiopt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* timestepOpt = ret->createOptionalParameter(6, "-timestep", "set the timestep");
    timestepOpt->addDoubleParameter(1, "interval", "the timestep, in seconds (default 1.0)");
    
    OptionalParameter* timestartOpt = ret->createOptionalParameter(7, "-timestart", "set the start time");
    timestartOpt->addDoubleParameter(1, "start", "the time at the first frame, in seconds (default 0.0)");
    
    OptionalParameter* unitOpt = ret->createOptionalParameter(8, "-unit", "use a unit other than time");
    unitOpt->addStringParameter(1, "unit", "unit identifier (default SECOND)");
    
    AString myText = AString("All input files must have the same number of columns/subvolumes.  ") +
        "Only the specified components will be in the output cifti.  " +
        "At least one component must be specified.\n\n" +
        "See -volume-label-import and -volume-help for format details of label volume files.  " +
        "The -metric structure argument and labels in the structure-label-volume must use names from the below list " +
        "(labels with other names are ignored, -metric arguments with other strings are an error):\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        myText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    myText += "\n\nThe -unit option accepts these values:\n";
    vector<CiftiSeriesMap::Unit> units = CiftiSeriesMap::getAllUnits();
    for (int i = 0; i < (int)units.size(); ++i)
    {
        myText += "\n" + CiftiSeriesMap::unitToString(units[i]);
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
    map<StructureEnum::Enum, SurfParam> surfParams;
    OptionalParameter* leftMetricOpt = myParams->getOptionalParameter(3);
    if (leftMetricOpt->m_present)
    {
        leftData = leftMetricOpt->getMetric(1);
        OptionalParameter* leftRoiOpt = leftMetricOpt->getOptionalParameter(2);
        if (leftRoiOpt->m_present)
        {
            leftRoi = leftRoiOpt->getMetric(1);
        }
        surfParams[StructureEnum::CORTEX_LEFT] = SurfParam(leftData, leftRoi);
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
        surfParams[StructureEnum::CORTEX_RIGHT] = SurfParam(rightData, rightRoi);
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
        surfParams[StructureEnum::CEREBELLUM] = SurfParam(cerebData, cerebRoi);
    }
    auto genMetricOpts = myParams->getRepeatableParameterInstances(9);
    for (auto instance : genMetricOpts)
    {
        bool ok = false;
        StructureEnum::Enum structure = StructureEnum::fromName(instance->getString(1), &ok);
        if (!ok) throw AlgorithmException("unrecognized structure identifier: " + instance->getString(1));
        if (surfParams.find(structure) != surfParams.end()) throw AlgorithmException(instance->getString(1) + " structure specified more than once");
        OptionalParameter* genRoiOpt = instance->getOptionalParameter(3);
        if (genRoiOpt->m_present)
        {
            surfParams[structure] = SurfParam(instance->getMetric(2), genRoiOpt->getMetric(1));
        } else {
            surfParams[structure] = SurfParam(instance->getMetric(2));
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
    CiftiSeriesMap::Unit myUnit = CiftiSeriesMap::SECOND;
    OptionalParameter* unitOpt = myParams->getOptionalParameter(8);
    if (unitOpt->m_present)
    {
        AString unitName = unitOpt->getString(1);
        bool ok = false;
        myUnit = CiftiSeriesMap::stringToUnit(unitName, ok);
        if (!ok)
        {
            throw AlgorithmException("unrecognized unit name: '" + unitName + "'");
        }
    }
    AlgorithmCiftiCreateDenseTimeseries(myProgObj, myCiftiOut, myVol, myVolLabel, surfParams, timestep, timestart, myUnit);
}

AlgorithmCiftiCreateDenseTimeseries::AlgorithmCiftiCreateDenseTimeseries(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol, const VolumeFile* myVolLabel,
                                                                         const MetricFile* leftData, const MetricFile* leftRoi,
                                                                         const MetricFile* rightData, const MetricFile* rightRoi,
                                                                         const MetricFile* cerebData, const MetricFile* cerebRoi,
                                                                         const float& timestep, const float& timestart, const CiftiSeriesMap::Unit& myUnit) : AbstractAlgorithm(NULL)
{
    map<StructureEnum::Enum, SurfParam> surfParams;
    if (leftData != NULL) surfParams[StructureEnum::CORTEX_LEFT] = SurfParam(leftData, leftRoi);
    if (rightData != NULL) surfParams[StructureEnum::CORTEX_RIGHT] = SurfParam(rightData, rightRoi);
    if (cerebData != NULL) surfParams[StructureEnum::CEREBELLUM] = SurfParam(cerebData, cerebRoi);
    AlgorithmCiftiCreateDenseTimeseries(myProgObj, myCiftiOut, myVol, myVolLabel, surfParams, timestep, timestart, myUnit);
}

AlgorithmCiftiCreateDenseTimeseries::AlgorithmCiftiCreateDenseTimeseries(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol, const VolumeFile* myVolLabel,
                                                                         const map<StructureEnum::Enum, SurfParam> surfParams,
                                                                         const float& timestep, const float& timestart, const CiftiSeriesMap::Unit& myUnit) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(myCiftiOut != NULL);
    LevelProgress myProgress(myProgObj);
    CiftiBrainModelsMap denseMap = makeDenseMapping(myVol, myVolLabel, surfParams);
    CiftiXML myXML;
    myXML.setNumberOfDimensions(2);
    myXML.setMap(CiftiXML::ALONG_COLUMN, denseMap);
    int numMaps = -1;
    const PaletteColorMapping* myPalette = NULL;
    StructureEnum::Enum firstSurf = StructureEnum::INVALID;
    for (auto param : surfParams)
    {
        if (numMaps == -1)
        {
            numMaps = param.second.data->getNumberOfMaps();
            myPalette = param.second.data->getMapPaletteColorMapping(0);
            firstSurf = param.first;
        } else {
            if (numMaps != param.second.data->getNumberOfMaps())
            {
                throw AlgorithmException(StructureEnum::toName(firstSurf) + " and " + StructureEnum::toName(param.first) + " surface data have a different number of maps");
            }
        }
    }
    if (myVol != NULL)
    {
        if (numMaps == -1)
        {
            numMaps = myVol->getNumberOfMaps();
            myPalette = myVol->getMapPaletteColorMapping(0);
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
    CiftiSeriesMap seriesMap;
    seriesMap.setUnit(myUnit);
    seriesMap.setStart(timestart);
    seriesMap.setStep(timestep);
    seriesMap.setLength(numMaps);
    myXML.setMap(CiftiXML::ALONG_ROW, seriesMap);
    if (myPalette != NULL)
    {
        *(myXML.getFilePalette()) = *myPalette;
    }
    myCiftiOut->setCiftiXML(myXML);
    CaretArray<float> temprow(numMaps);
    const CiftiBrainModelsMap& myDenseMap = myXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    vector<StructureEnum::Enum> surfStructs = myDenseMap.getSurfaceStructureList();
    for (int whichStruct = 0; whichStruct < (int)surfStructs.size(); ++whichStruct)
    {
        vector<CiftiBrainModelsMap::SurfaceMap> surfMap = myDenseMap.getSurfaceMap(surfStructs[whichStruct]);
        const MetricFile* dataMetric = surfParams.find(surfStructs[whichStruct])->second.data; //we built the map from these inputs, so it should be in there
        for (int64_t i = 0; i < (int)surfMap.size(); ++i)
        {
            for (int t = 0; t < numMaps; ++t)
            {
                temprow[t] = dataMetric->getValue(surfMap[i].m_surfaceNode, t);
            }
            myCiftiOut->setRow(temprow, surfMap[i].m_ciftiIndex);
        }
    }
    vector<CiftiBrainModelsMap::VolumeMap> volMap = myDenseMap.getFullVolumeMap();//we don't need to know which voxel is from which structure
    for (int64_t i = 0; i < (int)volMap.size(); ++i)
    {
        for (int t = 0; t < numMaps; ++t)
        {
            temprow[t] = myVol->getValue(volMap[i].m_ijk, t);
        }
        myCiftiOut->setRow(temprow, volMap[i].m_ciftiIndex);
    }
}

CiftiBrainModelsMap AlgorithmCiftiCreateDenseTimeseries::makeDenseMapping(const VolumeFile* myVol, const VolumeFile* myVolLabel,
                                                           const MetricFile* leftData, const MetricFile* leftRoi,
                                                           const MetricFile* rightData, const MetricFile* rightRoi,
                                                           const MetricFile* cerebData, const MetricFile* cerebRoi)
{
    map<StructureEnum::Enum, SurfParam> surfParams;
    if (leftData != NULL) surfParams[StructureEnum::CORTEX_LEFT] = SurfParam(leftData, leftRoi);
    if (rightData != NULL) surfParams[StructureEnum::CORTEX_RIGHT] = SurfParam(rightData, rightRoi);
    if (cerebData != NULL) surfParams[StructureEnum::CEREBELLUM] = SurfParam(cerebData, cerebRoi);
    return makeDenseMapping(myVol, myVolLabel, surfParams);
}

CiftiBrainModelsMap AlgorithmCiftiCreateDenseTimeseries::makeDenseMapping(const VolumeFile* myVol, const VolumeFile* myVolLabel,
                                                           const std::map<StructureEnum::Enum, SurfParam> surfParams)
{
    bool noData = true;
    CiftiBrainModelsMap denseMap;
    for (auto param : surfParams)
    {
        if (param.second.data != NULL)
        {//silently ignore NULL surface parameters, I guess?
            noData = false;
            if (param.second.roi == NULL)
            {
                denseMap.addSurfaceModel(param.second.data->getNumberOfNodes(), param.first);
            } else {
                if (param.second.data->getNumberOfNodes() != param.second.roi->getNumberOfNodes())
                {
                    throw AlgorithmException(StructureEnum::toName(param.first) + " surface ROI and data have different vertex counts");
                }
                denseMap.addSurfaceModel(param.second.data->getNumberOfNodes(), param.first, param.second.roi->getValuePointerForColumn(0));
            }
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
        vector<vector<int64_t> > voxelLists;//voxel lists for each volume component
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
        denseMap.setVolumeSpace(VolumeSpace(mydims.data(), myVol->getSform()));
        for (map<StructureEnum::Enum, int>::iterator myiter = componentMap.begin(); myiter != componentMap.end(); ++myiter)
        {
            if (voxelLists[myiter->second].empty())
            {
                CaretLogWarning("volume label file has empty definition of '" + StructureEnum::toName(myiter->first) + "', skipping");
            } else {
                denseMap.addVolumeModel(myiter->first, voxelLists[myiter->second]);
            }
        }
    }
    if (noData)
    {
        throw AlgorithmException("no models specified");
    }
    return denseMap;
}


float AlgorithmCiftiCreateDenseTimeseries::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiCreateDenseTimeseries::getSubAlgorithmWeight()
{
    return 0.0f;
}
