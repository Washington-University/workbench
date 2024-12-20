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

#include "AlgorithmCiftiCreateDenseScalar.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiCreateDenseTimeseries.h" //for making the dense mapping from metric files
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CiftiFile.h"
#include "CaretAssert.h"
#include "GiftiLabelTable.h"
#include "MetricFile.h"
#include "PaletteColorMapping.h"
#include "StructureEnum.h"
#include "VolumeFile.h"

#include <cmath>
#include <fstream>
#include <map>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmCiftiCreateDenseScalar::getCommandSwitch()
{
    return "-cifti-create-dense-scalar";
}

AString AlgorithmCiftiCreateDenseScalar::getShortDescription()
{
    return "CREATE A CIFTI DENSE SCALAR FILE";
}

OperationParameters* AlgorithmCiftiCreateDenseScalar::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiOutputParameter(1, "cifti-out", "the output cifti file");
    
    OptionalParameter* volumeOpt = ret->createOptionalParameter(2, "-volume", "volume component");
    volumeOpt->addVolumeParameter(1, "volume-data", "volume file containing all voxel data for all volume structures");
    volumeOpt->addVolumeParameter(2, "structure-label-volume", "label volume file containing labels for cifti structures");
    
    OptionalParameter* leftMetricOpt = ret->createOptionalParameter(3, "-left-metric", "metric for left surface");
    leftMetricOpt->addMetricParameter(1, "metric", "the metric file");
    OptionalParameter* leftRoiOpt = leftMetricOpt->createOptionalParameter(2, "-roi-left", "roi of vertices to use from left surface");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* rightMetricOpt = ret->createOptionalParameter(4, "-right-metric", "metric for right surface");
    rightMetricOpt->addMetricParameter(1, "metric", "the metric file");
    OptionalParameter* rightRoiOpt = rightMetricOpt->createOptionalParameter(2, "-roi-right", "roi of vertices to use from right surface");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* cerebMetricOpt = ret->createOptionalParameter(5, "-cerebellum-metric", "metric for the cerebellum");
    cerebMetricOpt->addMetricParameter(1, "metric", "the metric file");
    OptionalParameter* cerebRoiOpt = cerebMetricOpt->createOptionalParameter(2, "-roi-cerebellum", "roi of vertices to use from right surface");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    ParameterComponent* genMetricOpt = ret->createRepeatableParameter(7, "-metric", "metric for a specified structure");
    genMetricOpt->addStringParameter(1, "structure", "the structure name");
    genMetricOpt->addMetricParameter(2, "metric", "the metric file");
    OptionalParameter* genRoiopt = genMetricOpt->createOptionalParameter(3, "-roi", "roi of vertices to use from this structure");
    genRoiopt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");

    OptionalParameter* nameFileOpt = ret->createOptionalParameter(6, "-name-file", "use a text file to set all map names");
    nameFileOpt->addStringParameter(1, "file", "text file containing map names, one per line");
    
    AString myText = AString("All input files must have the same number of columns/subvolumes.  ") +
        "Only the specified components will be in the output cifti file.  " +
        "Map names will be taken from one of the input files.  " +
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
    ret->setHelpText(myText);
    return ret;
}

void AlgorithmCiftiCreateDenseScalar::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
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
    auto genMetricOpts = myParams->getRepeatableParameterInstances(7);
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
    vector<AString> nameStore;
    vector<AString>* namePtr = NULL;
    OptionalParameter* nameFileOpt = myParams->getOptionalParameter(6);
    if (nameFileOpt->m_present)
    {
        AString listfileName = nameFileOpt->getString(1);
        ifstream nameListFile(listfileName.toLocal8Bit().constData());
        if (!nameListFile.good())
        {
            throw AlgorithmException("error reading name list file");
        }
        string mapName;
        while (getline(nameListFile, mapName))
        {
            nameStore.push_back(mapName.c_str());
        }
        namePtr = &nameStore;
    }
    AlgorithmCiftiCreateDenseScalar(myProgObj, myCiftiOut, myVol, myVolLabel, surfParams, namePtr);
}

AlgorithmCiftiCreateDenseScalar::AlgorithmCiftiCreateDenseScalar(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol, const VolumeFile* myVolLabel,
                                                                 const MetricFile* leftData, const MetricFile* leftRoi,
                                                                 const MetricFile* rightData, const MetricFile* rightRoi,
                                                                 const MetricFile* cerebData, const MetricFile* cerebRoi,
                                                                 const vector<AString>* namePtr) : AbstractAlgorithm(NULL)
{
    map<StructureEnum::Enum, SurfParam> surfParams;
    if (leftData != NULL) surfParams[StructureEnum::CORTEX_LEFT] = SurfParam(leftData, leftRoi);
    if (rightData != NULL) surfParams[StructureEnum::CORTEX_RIGHT] = SurfParam(rightData, rightRoi);
    if (cerebData != NULL) surfParams[StructureEnum::CEREBELLUM] = SurfParam(cerebData, cerebRoi);
    AlgorithmCiftiCreateDenseScalar(myProgObj, myCiftiOut, myVol, myVolLabel, surfParams, namePtr);
}

AlgorithmCiftiCreateDenseScalar::AlgorithmCiftiCreateDenseScalar(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol, const VolumeFile* myVolLabel,
                                                                 const map<StructureEnum::Enum, SurfParam> surfParams,
                                                                 const vector<AString>* namePtr) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(myCiftiOut != NULL);
    LevelProgress myProgress(myProgObj);
    CiftiBrainModelsMap denseMap = AlgorithmCiftiCreateDenseTimeseries::makeDenseMapping(myVol, myVolLabel, surfParams);
    CiftiXML myXML;
    myXML.setNumberOfDimensions(2);
    myXML.setMap(CiftiXML::ALONG_COLUMN, denseMap);
    int numMaps = -1;
    //Always choose a dominant input, to try to copy palette settings from, even if we have a name list
    const CaretMappableDataFile* nameFile = NULL;
    StructureEnum::Enum firstSurf = StructureEnum::INVALID;
    for (auto param : surfParams)
    {
        if (numMaps == -1)
        {
            numMaps = param.second.data->getNumberOfMaps();
            nameFile = param.second.data;
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
            if (nameFile == NULL) nameFile = myVol;
        } else {
            if (numMaps != myVol->getNumberOfMaps())
            {
                throw AlgorithmException("volume data has a different number of maps");
            }
        }
    }
    if (numMaps == -1)//doubles as checking nameFile for being NULL
    {
        throw AlgorithmException("no models specified");
    }
    if (namePtr != NULL)
    {
        if ((int)(namePtr->size()) != numMaps)
        {
            if ((int)(namePtr->size()) < numMaps)
            {
                throw AlgorithmException("not enough map names provided");
            } else {
                if ((int)(namePtr->size()) > numMaps + 1 || namePtr->back() != "")
                {//allow an extra newline on the end of the input file - could do strict checking here and fix things up in useParameters, but then it would also need to figure out number of output maps
                    throw AlgorithmException("more map names were provided than output file would use");
                }
            }
            CaretLogFine("provided map names list has an extra empty string on the end");
        }
    }
    CiftiScalarsMap scalarMap;
    scalarMap.setLength(numMaps);
    for (int i = 0; i < numMaps; ++i)
    {
        if (namePtr != NULL)
        {
            scalarMap.setMapName(i, namePtr->at(i));
        } else {
            scalarMap.setMapName(i, nameFile->getMapName(i));//copy map names
        }
        const PaletteColorMapping* thisPalette = nameFile->getMapPaletteColorMapping(i);
        if (thisPalette != NULL)
        {
            *(scalarMap.getMapPalette(i)) = *thisPalette;
        }
    }
    myXML.setMap(CiftiXML::ALONG_ROW, scalarMap);
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

float AlgorithmCiftiCreateDenseScalar::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiCreateDenseScalar::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
