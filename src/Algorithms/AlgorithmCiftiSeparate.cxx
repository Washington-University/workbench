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

#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmException.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "Vector3D.h"
#include "VolumeFile.h"

#include <map>

using namespace caret;
using namespace std;

AString AlgorithmCiftiSeparate::getCommandSwitch()
{
    return "-cifti-separate";
}

AString AlgorithmCiftiSeparate::getShortDescription()
{
    return "SEPARATE A CIFTI MODEL INTO METRIC OR VOLUME";
}

OperationParameters* AlgorithmCiftiSeparate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-in", "the cifti to separate a component of");
    
    ret->addStringParameter(2, "direction", "which direction to separate into components, ROW or COLUMN");
    
    OptionalParameter* metricOpt = ret->createOptionalParameter(3, "-metric", "separate a surface model into a metric file");
    metricOpt->addStringParameter(1, "structure", "the structure to output");
    metricOpt->addMetricOutputParameter(2, "metric-out", "the output metric");
    OptionalParameter* metricRoiOpt = metricOpt->createOptionalParameter(3, "-roi", "also output the roi of which vertices have data, in case the structure isn't the full surface");
    metricRoiOpt->addMetricOutputParameter(1, "roi-out", "the roi output metric");
    
    OptionalParameter* volumeOpt = ret->createOptionalParameter(4, "-volume", "separate a volume model into a volume file");
    volumeOpt->addStringParameter(1, "structure", "the structure to output");
    volumeOpt->addVolumeOutputParameter(2, "volume-out", "the output volume");
    OptionalParameter* volumeRoiOpt = volumeOpt->createOptionalParameter(3, "-roi", "also output the roi of which voxels have data");
    volumeRoiOpt->addVolumeOutputParameter(1, "roi-out", "the roi output volume");
    volumeOpt->createOptionalParameter(4, "-crop", "crop volumes to the size of the parcel rather than using the original volume size");
    
    OptionalParameter* labelOpt = ret->createOptionalParameter(5, "-label", "separate a surface model into a surface label file");
    labelOpt->addStringParameter(1, "structure", "the structure to output");
    labelOpt->addLabelOutputParameter(2, "label-out", "the output label file");
    
    AString helpText = AString("You must specify -metric, -volume, or -label for this command to do anything.  Output volumes will line up with their ") +
        "original positions, whether or not they are cropped.  For dtseries, use COLUMN, and if your matrix is fully symmetric, COLUMN is " +
        "more efficient.  The structure argument must be one of the following:\n";
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
    OptionalParameter* metricOpt = myParams->getOptionalParameter(3);
    if (metricOpt->m_present)
    {
        AString structName = metricOpt->getString(1);
        bool ok = false;
        StructureEnum::Enum myStruct = StructureEnum::fromName(structName, &ok);
        if (!ok)
        {
            throw AlgorithmException("unrecognized structure type");
        }
        MetricFile* metricOut = metricOpt->getOutputMetric(2);
        MetricFile* roiOut = NULL;
        OptionalParameter* metricRoiOpt = metricOpt->getOptionalParameter(3);
        if (metricRoiOpt->m_present)
        {
            roiOut = metricRoiOpt->getOutputMetric(1);
        }
        AlgorithmCiftiSeparate(NULL, ciftiIn, myDir, myStruct, metricOut, roiOut);
    }
    OptionalParameter* volumeOpt = myParams->getOptionalParameter(4);
    if (volumeOpt->m_present)
    {
        AString structName = volumeOpt->getString(1);
        bool ok = false;
        StructureEnum::Enum myStruct = StructureEnum::fromName(structName, &ok);
        if (!ok)
        {
            throw AlgorithmException("unrecognized structure type");
        }
        VolumeFile* volOut = volumeOpt->getOutputVolume(2);
        VolumeFile* roiOut = NULL;
        OptionalParameter* volumeRoiOpt = volumeOpt->getOptionalParameter(3);
        if (volumeRoiOpt->m_present)
        {
            roiOut = volumeRoiOpt->getOutputVolume(1);
        }
        bool cropVol = volumeOpt->getOptionalParameter(4)->m_present;
        int64_t offset[3];
        AlgorithmCiftiSeparate(NULL, ciftiIn, myDir, myStruct, volOut, offset, roiOut, cropVol);
    }
    OptionalParameter* labelOpt = myParams->getOptionalParameter(5);
    if (labelOpt->m_present)
    {
        AString structName = volumeOpt->getString(1);
        bool ok = false;
        StructureEnum::Enum myStruct = StructureEnum::fromName(structName, &ok);
        if (!ok)
        {
            throw AlgorithmException("unrecognized structure type");
        }
        LabelFile* labelOut = labelOpt->getOutputLabel(2);
        AlgorithmCiftiSeparate(NULL, ciftiIn, myDir, myStruct, labelOut);
    }
}

AlgorithmCiftiSeparate::AlgorithmCiftiSeparate(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const int& myDir,
                                               const StructureEnum::Enum& myStruct, MetricFile* metricOut, MetricFile* roiOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<CiftiSurfaceMap> myMap;
    int rowSize = ciftiIn->getNumberOfColumns(), colSize = ciftiIn->getNumberOfRows();
    if (myDir == CiftiXML::ALONG_COLUMN)
    {
        if (!ciftiIn->getSurfaceMapForColumns(myMap, myStruct))
        {
            throw AlgorithmException("structure '" + StructureEnum::toGuiName(myStruct) + "' not found in specified dimension");
        }
        int64_t numNodes = ciftiIn->getColumnSurfaceNumberOfNodes(myStruct);
        metricOut->setNumberOfNodesAndColumns(numNodes, rowSize);
        metricOut->setStructure(myStruct);
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
        if (myDir != CiftiXML::ALONG_ROW) throw AlgorithmException("direction not supported by cifti separate");
        if (!ciftiIn->getSurfaceMapForRows(myMap, myStruct))
        {
            throw AlgorithmException("structure '" + StructureEnum::toGuiName(myStruct) + "' not found in specified dimension");
        }
        int64_t numNodes = ciftiIn->getRowSurfaceNumberOfNodes(myStruct);
        metricOut->setNumberOfNodesAndColumns(numNodes, colSize);
        metricOut->setStructure(myStruct);
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
    if (myDir > 1) throw AlgorithmException("direction not supported in cifti separate");
    vector<CiftiSurfaceMap> myMap;
    int64_t rowSize = ciftiIn->getNumberOfColumns(), colSize = ciftiIn->getNumberOfRows();
    if (myDir == CiftiXML::ALONG_COLUMN)
    {
        if (ciftiIn->getCiftiXML().getRowMappingType() != CIFTI_INDEX_TYPE_LABELS) throw AlgorithmException("label separate requested on non-label cifti");
        if (!ciftiIn->getSurfaceMapForColumns(myMap, myStruct))
        {
            throw AlgorithmException("structure '" + StructureEnum::toGuiName(myStruct) + "' not found in specified dimension");
        }
        int64_t numNodes = ciftiIn->getColumnSurfaceNumberOfNodes(myStruct);
        labelOut->setNumberOfNodesAndColumns(numNodes, rowSize);
        labelOut->setStructure(myStruct);
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
            map<int32_t, int32_t> thisRemap = myTable.append(*(ciftiIn->getCiftiXML().getLabelTableForRowIndex(i)));
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
        if (myDir != CiftiXML::ALONG_ROW) throw AlgorithmException("direction not supported by cifti separate");
        if (ciftiIn->getCiftiXML().getColumnMappingType() != CIFTI_INDEX_TYPE_LABELS) throw AlgorithmException("label separate requested on non-label cifti");
        if (!ciftiIn->getSurfaceMapForRows(myMap, myStruct))
        {
            throw AlgorithmException("structure '" + StructureEnum::toGuiName(myStruct) + "' not found in specified dimension");
        }
        int64_t numNodes = ciftiIn->getRowSurfaceNumberOfNodes(myStruct);
        labelOut->setNumberOfNodesAndColumns(numNodes, colSize);
        labelOut->setStructure(myStruct);
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
            map<int32_t, int32_t> thisRemap = myTable.append(*(ciftiIn->getCiftiXML().getLabelTableForColumnIndex(i)));
            cumulativeRemap.insert(thisRemap.begin(), thisRemap.end());
        }
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
    int64_t myDims[3];
    vector<vector<float> > mySform;
    vector<CiftiVolumeMap> myMap;
    vector<int64_t> newdims;
    int rowSize = ciftiIn->getNumberOfColumns(), colSize = ciftiIn->getNumberOfRows();
    if (!myXML.getVolumeDimsAndSForm(myDims, mySform))
    {
        throw AlgorithmException("input cifti has no volume space information");
    }
    if (myDir == CiftiXML::ALONG_COLUMN)
    {
        if (!myXML.getVolumeStructureMapForColumns(myMap, myStruct))
        {
            throw AlgorithmException("structure '" + StructureEnum::toGuiName(myStruct) + "' not found in specified dimension");
        }
    } else {
        if (myDir != CiftiXML::ALONG_ROW) throw AlgorithmException("direction not supported by cifti separate");
        if (!myXML.getVolumeStructureMapForRows(myMap, myStruct))
        {
            throw AlgorithmException("structure '" + StructureEnum::toGuiName(myStruct) + "' not found in specified dimension");
        }
    }
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
        if (myXML.getRowMappingType() == CIFTI_INDEX_TYPE_LABELS)
        {
            volOut->setType(SubvolumeAttributes::LABEL);
            for (int j = 0; j < rowSize; ++j)
            {
                (*volOut->getMapLabelTable(j)) = (*myXML.getLabelTableForRowIndex(j));
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
        if (myXML.getRowMappingType() == CIFTI_INDEX_TYPE_LABELS)
        {
            volOut->setType(SubvolumeAttributes::LABEL);
            for (int j = 0; j < colSize; ++j)
            {
                (*volOut->getMapLabelTable(j)) = (*myXML.getLabelTableForColumnIndex(j));
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
    vector<CiftiVolumeMap> myMap;
    int64_t myDims[3];
    if (!myXML.getVolumeDimsAndSForm(myDims, sformOut))
    {
        throw AlgorithmException("input cifti has no volume space information");
    }
    if (myDir == CiftiXML::ALONG_COLUMN)
    {
        if (!myXML.getVolumeStructureMapForColumns(myMap, myStruct))
        {
            throw AlgorithmException("structure '" + StructureEnum::toGuiName(myStruct) + "' not found in specified dimension");
        }
    } else {
        if (myDir != CiftiXML::ALONG_ROW) throw AlgorithmException("direction not supported by cifti separate");
        if (!myXML.getVolumeStructureMapForRows(myMap, myStruct))
        {
            throw AlgorithmException("structure '" + StructureEnum::toGuiName(myStruct) + "' not found in specified dimension");
        }
    }
    int64_t numVoxels = (int64_t)myMap.size();
    if (numVoxels > 0)
    {//make a voxel bounding box to minimize memory usage
        int extrema[6] = { myMap[0].m_ijk[0],
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

float AlgorithmCiftiSeparate::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiSeparate::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
