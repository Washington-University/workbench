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

#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmException.h"
#include "CaretLogger.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "Vector3D.h"
#include "VolumeFile.h"

#include <cstdlib>
#include <map>

using namespace caret;
using namespace std;

AString AlgorithmCiftiSeparate::getCommandSwitch()
{
    return "-cifti-separate";
}

AString AlgorithmCiftiSeparate::getShortDescription()
{
    return "WRITE A CIFTI STRUCTURE AS METRIC, LABEL OR VOLUME";
}

OperationParameters* AlgorithmCiftiSeparate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-in", "the cifti to separate a component of");
    
    ret->addStringParameter(2, "direction", "which direction to separate into components, ROW or COLUMN");
    
    ParameterComponent* labelOpt = ret->createRepeatableParameter(3, "-label", "separate a surface model into a surface label file");
    labelOpt->addStringParameter(1, "structure", "the structure to output");
    labelOpt->addLabelOutputParameter(2, "label-out", "the output label file");
    OptionalParameter* labelRoiOpt = labelOpt->createOptionalParameter(3, "-roi", "also output the roi of which vertices have data");
    labelRoiOpt->addMetricOutputParameter(1, "roi-out", "the roi output metric");
    
    ParameterComponent* metricOpt = ret->createRepeatableParameter(4, "-metric", "separate a surface model into a metric file");
    metricOpt->addStringParameter(1, "structure", "the structure to output");
    metricOpt->addMetricOutputParameter(2, "metric-out", "the output metric");
    OptionalParameter* metricRoiOpt = metricOpt->createOptionalParameter(3, "-roi", "also output the roi of which vertices have data");
    metricRoiOpt->addMetricOutputParameter(1, "roi-out", "the roi output metric");
    
    ParameterComponent* volumeOpt = ret->createRepeatableParameter(5, "-volume", "separate a volume structure into a volume file");
    volumeOpt->addStringParameter(1, "structure", "the structure to output");
    volumeOpt->addVolumeOutputParameter(2, "volume-out", "the output volume");
    OptionalParameter* volumeRoiOpt = volumeOpt->createOptionalParameter(3, "-roi", "also output the roi of which voxels have data");
    volumeRoiOpt->addVolumeOutputParameter(1, "roi-out", "the roi output volume");
    volumeOpt->createOptionalParameter(4, "-crop", "crop volume to the size of the component rather than using the original volume size");
    
    OptionalParameter* volumeAllOpt = ret->createOptionalParameter(6, "-volume-all", "separate all volume structures into a volume file");
    volumeAllOpt->addVolumeOutputParameter(1, "volume-out", "the output volume");
    OptionalParameter* volumeAllRoiOpt = volumeAllOpt->createOptionalParameter(2, "-roi", "also output the roi of which voxels have data");
    volumeAllRoiOpt->addVolumeOutputParameter(1, "roi-out", "the roi output volume");
    OptionalParameter* volumeAllLabelOpt = volumeAllOpt->createOptionalParameter(4, "-label", "output a volume label file indicating the location of structures");
    volumeAllLabelOpt->addVolumeOutputParameter(1, "label-out", "the label output volume");
    volumeAllOpt->createOptionalParameter(3, "-crop", "crop volume to the size of the data rather than using the original volume size");
    
    AString helpText = AString("For dtseries, dscalar, and dlabel, use COLUMN for <direction>, and if you have a symmetric dconn, COLUMN is more efficient.\n\n") +
        "You must specify at least one of -metric, -volume-all, -volume, or -label for this command to do anything.  " +
        "Output volumes will spatially line up with their original positions, whether or not they are cropped.  " +
        "Volume files produced by separating a dlabel file, or from the -label suboption of -volume-all, will be label volumes, see -volume-help.\n\n" +
        "For each <structure> argument, use one of the following strings:\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        helpText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    ret->setHelpText(helpText);
    return ret;
}

void AlgorithmCiftiSeparate::useParameters(OperationParameters* myParams, ProgressObject* /*myProgObj*/)
{//ignore the progress object for now, and allow specifying multiple options at once
    CiftiFile* ciftiIn = myParams->getCifti(1);
    AString dirName = myParams->getString(2);
    int myDir;
    if (dirName == "ROW")
    {
        myDir = CiftiXML::ALONG_ROW;
    } else if (dirName == "COLUMN") {
        myDir = CiftiXML::ALONG_COLUMN;
    } else {
        throw AlgorithmException("incorrect string for direction, use ROW or COLUMN");
    }
    bool outputRequested = false;
    const vector<ParameterComponent*>& labelInstances = myParams->getRepeatableParameterInstances(3);
    for (int i = 0; i < (int)labelInstances.size(); ++i)
    {
        outputRequested = true;
        AString structName = labelInstances[i]->getString(1);
        bool ok = false;
        StructureEnum::Enum myStruct = StructureEnum::fromName(structName, &ok);
        if (!ok)
        {
            throw AlgorithmException("unrecognized structure type: '" + structName + "'");
        }
        LabelFile* labelOut = labelInstances[i]->getOutputLabel(2);
        MetricFile* roiOut = NULL;
        OptionalParameter* labelRoiOpt = labelInstances[i]->getOptionalParameter(3);
        if (labelRoiOpt->m_present)
        {
            roiOut = labelRoiOpt->getOutputMetric(1);
        }
        AlgorithmCiftiSeparate(NULL, ciftiIn, myDir, myStruct, labelOut, roiOut);
    }
    const vector<ParameterComponent*>& metricInstances = myParams->getRepeatableParameterInstances(4);
    for (int i = 0; i < (int)metricInstances.size(); ++i)
    {
        outputRequested = true;
        AString structName = metricInstances[i]->getString(1);
        bool ok = false;
        StructureEnum::Enum myStruct = StructureEnum::fromName(structName, &ok);
        if (!ok)
        {
            throw AlgorithmException("unrecognized structure type: '" + structName + "'");
        }
        MetricFile* metricOut = metricInstances[i]->getOutputMetric(2);
        MetricFile* roiOut = NULL;
        OptionalParameter* metricRoiOpt = metricInstances[i]->getOptionalParameter(3);
        if (metricRoiOpt->m_present)
        {
            roiOut = metricRoiOpt->getOutputMetric(1);
        }
        AlgorithmCiftiSeparate(NULL, ciftiIn, myDir, myStruct, metricOut, roiOut);
    }
    const vector<ParameterComponent*>& volumeInstances = myParams->getRepeatableParameterInstances(5);
    for (int i = 0; i < (int)volumeInstances.size(); ++i)
    {
        outputRequested = true;
        AString structName = volumeInstances[i]->getString(1);
        bool ok = false;
        StructureEnum::Enum myStruct = StructureEnum::fromName(structName, &ok);
        if (!ok)
        {
            throw AlgorithmException("unrecognized structure type: '" + structName + "'");
        }
        VolumeFile* volOut = volumeInstances[i]->getOutputVolume(2);
        VolumeFile* roiOut = NULL;
        OptionalParameter* volumeRoiOpt = volumeInstances[i]->getOptionalParameter(3);
        if (volumeRoiOpt->m_present)
        {
            roiOut = volumeRoiOpt->getOutputVolume(1);
        }
        bool cropVol = volumeInstances[i]->getOptionalParameter(4)->m_present;
        int64_t offset[3];
        AlgorithmCiftiSeparate(NULL, ciftiIn, myDir, myStruct, volOut, offset, roiOut, cropVol);
    }
    OptionalParameter* volumeAllOpt = myParams->getOptionalParameter(6);
    if (volumeAllOpt->m_present)
    {
        outputRequested = true;
        VolumeFile* volOut = volumeAllOpt->getOutputVolume(1);
        VolumeFile* roiOut = NULL;
        OptionalParameter* volumeAllRoiOpt = volumeAllOpt->getOptionalParameter(2);
        if (volumeAllRoiOpt->m_present)
        {
            roiOut = volumeAllRoiOpt->getOutputVolume(1);
        }
        bool cropVol = volumeAllOpt->getOptionalParameter(3)->m_present;
        VolumeFile* labelOut = NULL;
        OptionalParameter* volumeAllLabelOpt = volumeAllOpt->getOptionalParameter(4);
        if (volumeAllLabelOpt->m_present)
        {
            labelOut = volumeAllLabelOpt->getOutputVolume(1);
        }
        int64_t offset[3];
        AlgorithmCiftiSeparate(NULL, ciftiIn, myDir, volOut, offset, roiOut, cropVol, labelOut);
    }
    if (!outputRequested)
    {
        CaretLogWarning("no output requested from -cifti-separate, command will do nothing");
    }
}

AlgorithmCiftiSeparate::AlgorithmCiftiSeparate(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const int& myDir,
                                               const StructureEnum::Enum& myStruct, MetricFile* metricOut, MetricFile* roiOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& myXML = ciftiIn->getCiftiXML();
    if (myXML.getNumberOfDimensions() != 2) throw AlgorithmException("cifti separate only supported on 2D cifti");
    if (myDir >= myXML.getNumberOfDimensions() || myDir < 0) throw AlgorithmException("direction invalid for input cifti");
    if (myXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS) throw AlgorithmException("specified direction does not contain brain models");
    if (myXML.getMappingType(1 - myDir) == CiftiMappingType::LABELS) CaretLogWarning("creating a metric file from cifti label data");
    const CiftiBrainModelsMap& myBrainModelsMap = myXML.getBrainModelsMap(myDir);
    if (!myBrainModelsMap.hasSurfaceData(myStruct)) throw AlgorithmException("specified file and direction does not contain the requested surface structure '" + StructureEnum::toName(myStruct) + "'");
    vector<CiftiBrainModelsMap::SurfaceMap> myMap = myBrainModelsMap.getSurfaceMap(myStruct);
    int rowSize = ciftiIn->getNumberOfColumns(), colSize = ciftiIn->getNumberOfRows();
    if (myDir == CiftiXML::ALONG_COLUMN)
    {
        int64_t numNodes = myBrainModelsMap.getSurfaceNumberOfNodes(myStruct);
        metricOut->setNumberOfNodesAndColumns(numNodes, rowSize);
        metricOut->setStructure(myStruct);
        const CiftiMappingType& myNamesMap = *(myXML.getMap(1 - myDir));
        for (int j = 0; j < rowSize; ++j)
        {
            metricOut->setMapName(j, myNamesMap.getIndexName(j));
        }
        if (roiOut != NULL)
        {
            roiOut->setNumberOfNodesAndColumns(numNodes, 1);
            roiOut->setStructure(myStruct);
        }
        int mapSize = (int)myMap.size();
        CaretArray<float> rowScratch(rowSize);
        CaretArray<float> nodeUsed(numNodes, 0.0f);
        for (int i = 0; i < mapSize; ++i)
        {
            ciftiIn->getRow(rowScratch, myMap[i].m_ciftiIndex);
            nodeUsed[myMap[i].m_surfaceNode] = 1.0f;
            for (int j = 0; j < rowSize; ++j)
            {
                metricOut->setValue(myMap[i].m_surfaceNode, j, rowScratch[j]);
            }
        }
        if (roiOut != NULL)
        {
            roiOut->setValuesForColumn(0, nodeUsed);
        }
        for (int i = 0; i < numNodes; ++i)//zero unused columns
        {
            if (nodeUsed[i] == 0.0f)
            {
                for (int j = 0; j < rowSize; ++j)
                {
                    metricOut->setValue(i, j, 0.0f);
                }
            }
        }
    } else {
        int64_t numNodes = myBrainModelsMap.getSurfaceNumberOfNodes(myStruct);
        metricOut->setNumberOfNodesAndColumns(numNodes, colSize);
        metricOut->setStructure(myStruct);
        const CiftiMappingType& myNamesMap = *(myXML.getMap(1 - myDir));
        for (int j = 0; j < colSize; ++j)
        {
            metricOut->setMapName(j, myNamesMap.getIndexName(j));
        }
        if (roiOut != NULL)
        {
            roiOut->setNumberOfNodesAndColumns(numNodes, 1);
            roiOut->setStructure(myStruct);
        }
        int mapSize = (int)myMap.size();
        CaretArray<float> rowScratch(rowSize), metricScratch(numNodes, 0.0f);
        if (roiOut != NULL)
        {
            CaretArray<float> nodeUsed(numNodes, 0.0f);
            for (int i = 0; i < mapSize; ++i)
            {
                nodeUsed[myMap[i].m_surfaceNode] = 1.0f;
            }
            roiOut->setValuesForColumn(0, nodeUsed);
        }
        for (int i = 0; i < colSize; ++i)
        {
            ciftiIn->getRow(rowScratch, i);
            for (int j = 0; j < mapSize; ++j)
            {
                metricScratch[myMap[j].m_surfaceNode] = rowScratch[myMap[j].m_ciftiIndex];
            }
            metricOut->setValuesForColumn(i, metricScratch);
        }
    }
}

AlgorithmCiftiSeparate::AlgorithmCiftiSeparate(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const int& myDir,
                                               const StructureEnum::Enum& myStruct, LabelFile* labelOut, MetricFile* roiOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& myXML = ciftiIn->getCiftiXML();
    if (myXML.getNumberOfDimensions() != 2) throw AlgorithmException("cifti separate only supported on 2D cifti");
    if (myDir >= myXML.getNumberOfDimensions() || myDir < 0) throw AlgorithmException("direction invalid for input cifti");
    if (myXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS) throw AlgorithmException("specified direction does not contain brain models");
    if (myXML.getMappingType(1 - myDir) != CiftiMappingType::LABELS) throw AlgorithmException("label separate requested on non-label cifti");
    const CiftiBrainModelsMap& myBrainModelsMap = myXML.getBrainModelsMap(myDir);
    const CiftiLabelsMap& myLabelsMap = myXML.getLabelsMap(1 - myDir);
    if (!myBrainModelsMap.hasSurfaceData(myStruct)) throw AlgorithmException("specified file and direction does not contain the requested surface structure '" + StructureEnum::toName(myStruct) + "'");
    vector<CiftiBrainModelsMap::SurfaceMap> myMap = myBrainModelsMap.getSurfaceMap(myStruct);
    int64_t rowSize = ciftiIn->getNumberOfColumns(), colSize = ciftiIn->getNumberOfRows();
    if (myDir == CiftiXML::ALONG_COLUMN)
    {
        int64_t numNodes = myBrainModelsMap.getSurfaceNumberOfNodes(myStruct);
        labelOut->setNumberOfNodesAndColumns(numNodes, rowSize);
        labelOut->setStructure(myStruct);
        const CiftiMappingType& myNamesMap = *(myXML.getMap(1 - myDir));
        for (int j = 0; j < rowSize; ++j)
        {
            labelOut->setMapName(j, myNamesMap.getIndexName(j));
        }
        if (roiOut != NULL)
        {
            roiOut->setNumberOfNodesAndColumns(numNodes, 1);
            roiOut->setStructure(myStruct);
        }
        int64_t mapSize = (int64_t)myMap.size();
        CaretArray<float> rowScratch(rowSize);
        CaretArray<float> nodeUsed(numNodes, 0.0f);
        GiftiLabelTable myTable;
        map<int32_t, int32_t> cumulativeRemap;
        for (int64_t i = 0; i < rowSize; ++i)
        {
            map<int32_t, int32_t> thisRemap = myTable.append(*(myLabelsMap.getMapLabelTable(i)));
            cumulativeRemap.insert(thisRemap.begin(), thisRemap.end());
        }
        for (int64_t i = 0; i < mapSize; ++i)
        {
            ciftiIn->getRow(rowScratch, myMap[i].m_ciftiIndex);
            nodeUsed[myMap[i].m_surfaceNode] = 1.0f;
            for (int j = 0; j < rowSize; ++j)
            {
                int32_t inVal = (int32_t)floor(rowScratch[j] + 0.5f);
                map<int32_t, int32_t>::const_iterator iter = cumulativeRemap.find(inVal);
                if (iter == cumulativeRemap.end())
                {
                    labelOut->setLabelKey(myMap[i].m_surfaceNode, j, inVal);
                } else {
                    labelOut->setLabelKey(myMap[i].m_surfaceNode, j, iter->second);
                }
            }
        }
        myTable.setHierarchy(myLabelsMap.getMapLabelTable(0)->getHierarchy()); //grab the first one for now, maybe figure out how to try to merge hierarchies later, if it matters
        *(labelOut->getLabelTable()) = myTable;
        int32_t unusedLabel = myTable.getUnassignedLabelKey();
        if (roiOut != NULL)
        {
            roiOut->setValuesForColumn(0, nodeUsed);
        }
        for (int i = 0; i < numNodes; ++i)//set unused columns to unassigned
        {
            if (nodeUsed[i] == 0.0f)
            {
                for (int j = 0; j < rowSize; ++j)
                {
                    labelOut->setLabelKey(i, j, unusedLabel);
                }
            }
        }
    } else {
        int64_t numNodes = myBrainModelsMap.getSurfaceNumberOfNodes(myStruct);
        labelOut->setNumberOfNodesAndColumns(numNodes, colSize);
        labelOut->setStructure(myStruct);
        const CiftiMappingType& myNamesMap = *(myXML.getMap(1 - myDir));
        for (int j = 0; j < colSize; ++j)
        {
            labelOut->setMapName(j, myNamesMap.getIndexName(j));
        }
        if (roiOut != NULL)
        {
            roiOut->setNumberOfNodesAndColumns(numNodes, 1);
            roiOut->setStructure(myStruct);
        }
        int64_t mapSize = (int)myMap.size();
        if (roiOut != NULL)
        {
            CaretArray<float> nodeUsed(numNodes, 0.0f);
            for (int i = 0; i < mapSize; ++i)
            {
                nodeUsed[myMap[i].m_surfaceNode] = 1.0f;
            }
            roiOut->setValuesForColumn(0, nodeUsed);
        }
        CaretArray<float> rowScratch(rowSize);
        CaretArray<int> nodeUsed(numNodes, 0);
        for (int64_t j = 0; j < mapSize; ++j)
        {
            nodeUsed[myMap[j].m_surfaceNode] = 1;
        }
        GiftiLabelTable myTable;
        map<int32_t, int32_t> cumulativeRemap;
        for (int64_t i = 0; i < colSize; ++i)
        {
            map<int32_t, int32_t> thisRemap = myTable.append(*(myLabelsMap.getMapLabelTable(i)));
            cumulativeRemap.insert(thisRemap.begin(), thisRemap.end());
        }
        myTable.setHierarchy(myLabelsMap.getMapLabelTable(0)->getHierarchy()); //ditto
        *(labelOut->getLabelTable()) = myTable;
        int32_t unusedLabel = myTable.getUnassignedLabelKey();
        for (int64_t i = 0; i < colSize; ++i)
        {
            ciftiIn->getRow(rowScratch, i);
            for (int64_t j = 0; j < mapSize; ++j)
            {
                int32_t inVal = (int32_t)floor(rowScratch[j] + 0.5f);
                map<int32_t, int32_t>::const_iterator iter = cumulativeRemap.find(inVal);
                if (iter == cumulativeRemap.end())
                {
                    labelOut->setLabelKey(myMap[j].m_surfaceNode, i, inVal);
                } else {
                    labelOut->setLabelKey(myMap[j].m_surfaceNode, i, iter->second);
                }
            }
            for (int64_t j = 0; j < numNodes; ++j)//set unused columns to unassigned
            {
                if (nodeUsed[j] == 0)
                {
                    labelOut->setLabelKey(j, i, unusedLabel);
                }
            }
        }
    }
}

AlgorithmCiftiSeparate::AlgorithmCiftiSeparate(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const int& myDir,
                                               const StructureEnum::Enum& myStruct, VolumeFile* volOut, int64_t offsetOut[3],
                                               VolumeFile* roiOut, const bool& cropVol) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& myXML = ciftiIn->getCiftiXML();
    if (myXML.getNumberOfDimensions() != 2) throw AlgorithmException("cifti separate only supported on 2D cifti");
    if (myDir >= ciftiIn->getCiftiXML().getNumberOfDimensions() || myDir < 0) throw AlgorithmException("direction invalid for input cifti");
    if (myXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS) throw AlgorithmException("specified direction does not contain brain models");
    int rowSize = ciftiIn->getNumberOfColumns(), colSize = ciftiIn->getNumberOfRows();
    const CiftiBrainModelsMap& myBrainMap = myXML.getBrainModelsMap(myDir);
    const int64_t* myDims = myBrainMap.getVolumeSpace().getDims();
    vector<vector<float> > mySform = myBrainMap.getVolumeSpace().getSform();
    if (!myBrainMap.hasVolumeData(myStruct)) throw AlgorithmException("specified file and direction does not contain the requested volume structure");
    vector<CiftiBrainModelsMap::VolumeMap> myMap = myBrainMap.getVolumeStructureMap(myStruct);
    vector<int64_t> newdims;
    int64_t numVoxels = (int64_t)myMap.size();
    if (cropVol)
    {
        newdims.resize(3);
        getCroppedVolSpace(ciftiIn, myDir, myStruct, newdims.data(), mySform, offsetOut);
    } else {
        newdims.push_back(myDims[0]);
        newdims.push_back(myDims[1]);
        newdims.push_back(myDims[2]);
        offsetOut[0] = 0;
        offsetOut[1] = 0;
        offsetOut[2] = 0;
    }
    if (roiOut != NULL)
    {
        roiOut->reinitialize(newdims, mySform);
        roiOut->setValueAllVoxels(0.0f);
    }
    CaretArray<float> rowScratch(rowSize);
    if (myDir == CiftiXML::ALONG_COLUMN)
    {
        if (rowSize > 1) newdims.push_back(rowSize);
        volOut->reinitialize(newdims, mySform);
        volOut->setValueAllVoxels(0.0f);
        const CiftiMappingType& myNamesMap = *(myXML.getMap(1 - myDir));
        for (int j = 0; j < rowSize; ++j)
        {
            volOut->setMapName(j, myNamesMap.getIndexName(j));
        }
        if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::LABELS)
        {
            const CiftiLabelsMap& myLabelsMap = myXML.getLabelsMap(CiftiXML::ALONG_ROW);
            volOut->setType(SubvolumeAttributes::LABEL);
            for (int j = 0; j < rowSize; ++j)
            {
                *(volOut->getMapLabelTable(j)) = *(myLabelsMap.getMapLabelTable(j));
            }
        }
        for (int64_t i = 0; i < numVoxels; ++i)
        {
            int64_t thisvoxel[3] = { myMap[i].m_ijk[0] - offsetOut[0], myMap[i].m_ijk[1] - offsetOut[1], myMap[i].m_ijk[2] - offsetOut[2] };
            if (roiOut != NULL)
            {
                roiOut->setValue(1.0f, thisvoxel);
            }
            ciftiIn->getRow(rowScratch, myMap[i].m_ciftiIndex);
            for (int j = 0; j < rowSize; ++j)
            {
                volOut->setValue(rowScratch[j], thisvoxel, j);
            }
        }
    } else {
        if (colSize > 1) newdims.push_back(colSize);
        volOut->reinitialize(newdims, mySform);
        volOut->setValueAllVoxels(0.0f);
        const CiftiMappingType& myNamesMap = *(myXML.getMap(1 - myDir));
        for (int j = 0; j < colSize; ++j)
        {
            volOut->setMapName(j, myNamesMap.getIndexName(j));
        }
        if (myXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::LABELS)
        {
            const CiftiLabelsMap& myLabelsMap = myXML.getLabelsMap(CiftiXML::ALONG_COLUMN);
            volOut->setType(SubvolumeAttributes::LABEL);
            for (int j = 0; j < colSize; ++j)
            {
                *(volOut->getMapLabelTable(j)) = *(myLabelsMap.getMapLabelTable(j));
            }
        }
        for (int64_t i = 0; i < colSize; ++i)
        {
            ciftiIn->getRow(rowScratch, i);
            for (int64_t j = 0; j < numVoxels; ++j)
            {
                int64_t thisvoxel[3] = { myMap[j].m_ijk[0] - offsetOut[0], myMap[j].m_ijk[1] - offsetOut[1], myMap[j].m_ijk[2] - offsetOut[2] };
                if (i == 0 && roiOut != NULL)
                {
                    roiOut->setValue(1.0f, thisvoxel);
                }
                volOut->setValue(rowScratch[myMap[j].m_ciftiIndex], thisvoxel, i);
            }
        }
    }
}

AlgorithmCiftiSeparate::AlgorithmCiftiSeparate(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const int& myDir, VolumeFile* volOut, int64_t offsetOut[3],
                                               VolumeFile* roiOut, const bool& cropVol, VolumeFile* labelOut): AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& myXML = ciftiIn->getCiftiXML();
    if (myXML.getNumberOfDimensions() != 2) throw AlgorithmException("cifti separate only supported on 2D cifti");
    if (myDir >= ciftiIn->getCiftiXML().getNumberOfDimensions() || myDir < 0) throw AlgorithmException("direction invalid for input cifti");
    if (myXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS) throw AlgorithmException("specified direction does not contain brain models");
    int rowSize = ciftiIn->getNumberOfColumns(), colSize = ciftiIn->getNumberOfRows();
    const CiftiBrainModelsMap& myBrainMap = myXML.getBrainModelsMap(myDir);
    if (!myBrainMap.hasVolumeData()) throw AlgorithmException("specified file and direction does not contain any volume data");
    const int64_t* myDims = myBrainMap.getVolumeSpace().getDims();
    vector<vector<float> > mySform = myBrainMap.getVolumeSpace().getSform();
    vector<int64_t> newdims;
    if (cropVol)
    {
        newdims.resize(3);
        getCroppedVolSpaceAll(ciftiIn, myDir, newdims.data(), mySform, offsetOut);
    } else {
        newdims.push_back(myDims[0]);
        newdims.push_back(myDims[1]);
        newdims.push_back(myDims[2]);
        offsetOut[0] = 0;
        offsetOut[1] = 0;
        offsetOut[2] = 0;
    }
    if (labelOut != NULL)
    {
        labelOut->reinitialize(newdims, mySform, 1, SubvolumeAttributes::LABEL);
        labelOut->setValueAllVoxels(0.0f);//unlabeled key defaults to 0
        vector<StructureEnum::Enum> volStructs = myBrainMap.getVolumeStructureList();
        GiftiLabelTable structureTable;
        for (int i = 0; i < (int)volStructs.size(); ++i)
        {
            const int32_t structKey = structureTable.addLabel(StructureEnum::toName(volStructs[i]), rand() & 255, rand() & 255, rand() & 255, 255);
            const vector<int64_t>& voxelList = myBrainMap.getVoxelList(volStructs[i]);
            int64_t structVoxels = (int64_t)voxelList.size();
            for (int64_t j = 0; j < structVoxels; j += 3)
            {
                labelOut->setValue(structKey, voxelList[j] - offsetOut[0], voxelList[j + 1] - offsetOut[1], voxelList[j + 2] - offsetOut[2]);
            }
        }
        *(labelOut->getMapLabelTable(0)) = structureTable;
    }
    if (roiOut != NULL)
    {
        roiOut->reinitialize(newdims, mySform);
        roiOut->setValueAllVoxels(0.0f);
    }
    vector<CiftiBrainModelsMap::VolumeMap> myMap = myBrainMap.getFullVolumeMap();
    int64_t numVoxels = (int64_t)myMap.size();
    CaretArray<float> rowScratch(rowSize);
    if (myDir == CiftiXML::ALONG_COLUMN)
    {
        if (rowSize > 1) newdims.push_back(rowSize);
        volOut->reinitialize(newdims, mySform);
        volOut->setValueAllVoxels(0.0f);
        const CiftiMappingType& myNamesMap = *(myXML.getMap(1 - myDir));
        for (int j = 0; j < rowSize; ++j)
        {
            volOut->setMapName(j, myNamesMap.getIndexName(j));
        }
        if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::LABELS)
        {
            const CiftiLabelsMap& myLabelsMap = myXML.getLabelsMap(CiftiXML::ALONG_ROW);
            volOut->setType(SubvolumeAttributes::LABEL);
            for (int j = 0; j < rowSize; ++j)
            {
                *(volOut->getMapLabelTable(j)) = *(myLabelsMap.getMapLabelTable(j));
            }
        }
        for (int64_t i = 0; i < numVoxels; ++i)
        {
            int64_t thisvoxel[3] = { myMap[i].m_ijk[0] - offsetOut[0], myMap[i].m_ijk[1] - offsetOut[1], myMap[i].m_ijk[2] - offsetOut[2] };
            if (roiOut != NULL)
            {
                roiOut->setValue(1.0f, thisvoxel);
            }
            ciftiIn->getRow(rowScratch, myMap[i].m_ciftiIndex);
            for (int j = 0; j < rowSize; ++j)
            {
                volOut->setValue(rowScratch[j], thisvoxel, j);
            }
        }
    } else {
        if (colSize > 1) newdims.push_back(colSize);
        volOut->reinitialize(newdims, mySform);
        volOut->setValueAllVoxels(0.0f);
        const CiftiMappingType& myNamesMap = *(myXML.getMap(1 - myDir));
        for (int j = 0; j < colSize; ++j)
        {
            volOut->setMapName(j, myNamesMap.getIndexName(j));
        }
        if (myXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::LABELS)
        {
            const CiftiLabelsMap& myLabelsMap = myXML.getLabelsMap(CiftiXML::ALONG_COLUMN);
            volOut->setType(SubvolumeAttributes::LABEL);
            for (int j = 0; j < colSize; ++j)
            {
                *(volOut->getMapLabelTable(j)) = *(myLabelsMap.getMapLabelTable(j));
            }
        }
        for (int64_t i = 0; i < colSize; ++i)
        {
            ciftiIn->getRow(rowScratch, i);
            for (int64_t j = 0; j < numVoxels; ++j)
            {
                int64_t thisvoxel[3] = { myMap[j].m_ijk[0] - offsetOut[0], myMap[j].m_ijk[1] - offsetOut[1], myMap[j].m_ijk[2] - offsetOut[2] };
                if (i == 0 && roiOut != NULL)
                {
                    roiOut->setValue(1.0f, thisvoxel);
                }
                volOut->setValue(rowScratch[myMap[j].m_ciftiIndex], thisvoxel, i);
            }
        }
    }
}

void AlgorithmCiftiSeparate::getCroppedVolSpace(const CiftiFile* ciftiIn, const int& myDir, const StructureEnum::Enum& myStruct, int64_t dimsOut[3],
                                                vector<vector<float> >& sformOut, int64_t offsetOut[3])
{
    const CiftiXML& myXML = ciftiIn->getCiftiXML();
    if (myXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS) throw AlgorithmException("specified direction does not contain brain models");
    const CiftiBrainModelsMap& myBrainMap = myXML.getBrainModelsMap(myDir);
    sformOut = myBrainMap.getVolumeSpace().getSform();
    vector<CiftiBrainModelsMap::VolumeMap> myMap = myBrainMap.getVolumeStructureMap(myStruct);
    int64_t numVoxels = (int64_t)myMap.size();
    if (numVoxels > 0)
    {//make a voxel bounding box to minimize memory usage
        int64_t extrema[6] = { myMap[0].m_ijk[0],
            myMap[0].m_ijk[0],
            myMap[0].m_ijk[1],
            myMap[0].m_ijk[1],
            myMap[0].m_ijk[2],
            myMap[0].m_ijk[2]
        };
        for (int64_t i = 1; i < numVoxels; ++i)
        {
            if (myMap[i].m_ijk[0] < extrema[0]) extrema[0] = myMap[i].m_ijk[0];
            if (myMap[i].m_ijk[0] > extrema[1]) extrema[1] = myMap[i].m_ijk[0];
            if (myMap[i].m_ijk[1] < extrema[2]) extrema[2] = myMap[i].m_ijk[1];
            if (myMap[i].m_ijk[1] > extrema[3]) extrema[3] = myMap[i].m_ijk[1];
            if (myMap[i].m_ijk[2] < extrema[4]) extrema[4] = myMap[i].m_ijk[2];
            if (myMap[i].m_ijk[2] > extrema[5]) extrema[5] = myMap[i].m_ijk[2];
        }
        dimsOut[0] = extrema[1] - extrema[0] + 1;
        dimsOut[1] = extrema[3] - extrema[2] + 1;
        dimsOut[2] = extrema[5] - extrema[4] + 1;
        offsetOut[0] = extrema[0];
        offsetOut[1] = extrema[2];
        offsetOut[2] = extrema[4];
        Vector3D ivec, jvec, kvec, shift;
        ivec[0] = sformOut[0][0]; ivec[1] = sformOut[1][0]; ivec[2] = sformOut[2][0];
        jvec[0] = sformOut[0][1]; jvec[1] = sformOut[1][1]; jvec[2] = sformOut[2][1];
        kvec[0] = sformOut[0][2]; kvec[1] = sformOut[1][2]; kvec[2] = sformOut[2][2];
        shift = offsetOut[0] * ivec + offsetOut[1] * jvec + offsetOut[2] * kvec;
        sformOut[0][3] += shift[0];//fix the sform to align to the old position with the new dimensions
        sformOut[1][3] += shift[1];
        sformOut[2][3] += shift[2];
    } else {
        throw AlgorithmException("cropped volume requested, but no voxels exist in this structure");
    }
}

void AlgorithmCiftiSeparate::getCroppedVolSpaceAll(const CiftiFile* ciftiIn, const int& myDir, int64_t dimsOut[3], vector<vector<float> >& sformOut, int64_t offsetOut[3])
{
    const CiftiXML& myXML = ciftiIn->getCiftiXML();
    if (myXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS) throw AlgorithmException("specified direction does not contain brain models");
    const CiftiBrainModelsMap& myBrainMap = myXML.getBrainModelsMap(myDir);
    sformOut = myBrainMap.getVolumeSpace().getSform();
    vector<CiftiBrainModelsMap::VolumeMap> myMap = myBrainMap.getFullVolumeMap();
    int64_t numVoxels = (int64_t)myMap.size();
    if (numVoxels > 0)
    {//make a voxel bounding box to minimize memory usage
        int64_t extrema[6] = { myMap[0].m_ijk[0],
            myMap[0].m_ijk[0],
            myMap[0].m_ijk[1],
            myMap[0].m_ijk[1],
            myMap[0].m_ijk[2],
            myMap[0].m_ijk[2]
        };
        for (int64_t i = 1; i < numVoxels; ++i)
        {
            if (myMap[i].m_ijk[0] < extrema[0]) extrema[0] = myMap[i].m_ijk[0];
            if (myMap[i].m_ijk[0] > extrema[1]) extrema[1] = myMap[i].m_ijk[0];
            if (myMap[i].m_ijk[1] < extrema[2]) extrema[2] = myMap[i].m_ijk[1];
            if (myMap[i].m_ijk[1] > extrema[3]) extrema[3] = myMap[i].m_ijk[1];
            if (myMap[i].m_ijk[2] < extrema[4]) extrema[4] = myMap[i].m_ijk[2];
            if (myMap[i].m_ijk[2] > extrema[5]) extrema[5] = myMap[i].m_ijk[2];
        }
        dimsOut[0] = extrema[1] - extrema[0] + 1;
        dimsOut[1] = extrema[3] - extrema[2] + 1;
        dimsOut[2] = extrema[5] - extrema[4] + 1;
        offsetOut[0] = extrema[0];
        offsetOut[1] = extrema[2];
        offsetOut[2] = extrema[4];
        Vector3D ivec, jvec, kvec, shift;
        ivec[0] = sformOut[0][0]; ivec[1] = sformOut[1][0]; ivec[2] = sformOut[2][0];
        jvec[0] = sformOut[0][1]; jvec[1] = sformOut[1][1]; jvec[2] = sformOut[2][1];
        kvec[0] = sformOut[0][2]; kvec[1] = sformOut[1][2]; kvec[2] = sformOut[2][2];
        shift = offsetOut[0] * ivec + offsetOut[1] * jvec + offsetOut[2] * kvec;
        sformOut[0][3] += shift[0];//fix the sform to align to the old position with the new dimensions
        sformOut[1][3] += shift[1];
        sformOut[2][3] += shift[2];
    } else {
        throw AlgorithmException("cropped volume requested, but no voxels exist in the specified direction");
    }
}

float AlgorithmCiftiSeparate::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiSeparate::getSubAlgorithmWeight()
{
    return 0.0f;
}
