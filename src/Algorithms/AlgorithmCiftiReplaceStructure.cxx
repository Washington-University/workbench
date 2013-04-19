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

#include "AlgorithmCiftiReplaceStructure.h"
//for computing the cropped volume space
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmException.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "Vector3D.h"
#include "VolumeFile.h"

#include <cmath>
#include <set>

using namespace caret;
using namespace std;

AString AlgorithmCiftiReplaceStructure::getCommandSwitch()
{
    return "-cifti-replace-structure";
}

AString AlgorithmCiftiReplaceStructure::getShortDescription()
{
    return "REPLACE DATA IN A STRUCTURE IN A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiReplaceStructure::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "cifti", "the cifti to modify");//in place read/write is not a case in OperationParameters currently, could hack it by using a CiftiParameter...
    
    ret->addStringParameter(2, "direction", "which dimension to interpret as a single map, ROW or COLUMN");
    
    OptionalParameter* metricOpt = ret->createOptionalParameter(3, "-metric", "replace the data in a surface component");
    metricOpt->addStringParameter(1, "structure", "the structure to replace the data of");
    metricOpt->addMetricParameter(2, "metric", "the input metric");

    OptionalParameter* volumeOpt = ret->createOptionalParameter(4, "-volume", "replace the data in a volume component");
    volumeOpt->addStringParameter(1, "structure", "the structure to replace the data of");
    volumeOpt->addVolumeOutputParameter(2, "volume", "the input volume");
    volumeOpt->createOptionalParameter(3, "-from-cropped", "interpret the input as a volume cropped to the size of the parcel");

    OptionalParameter* labelOpt = ret->createOptionalParameter(5, "-label", "replace the data in a surface label component");
    labelOpt->addStringParameter(1, "structure", "the structure to replace the data of");
    labelOpt->addLabelParameter(2, "label", "the input label file");

    AString helpText = AString("You must specify -metric, -volume, or -label for this command to do anything.  Input volumes must line up with the ") +
        "output of -cifti-separate.  For dtseries, use COLUMN, and if your matrix will be fully symmetric, COLUMN is " +
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

void AlgorithmCiftiReplaceStructure::useParameters(OperationParameters* myParams, ProgressObject* /*myProgObj*/)
{
    AString ciftiName = myParams->getString(1);
    CiftiFile myCifti(ciftiName, ON_DISK);
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
        MetricFile* metricIn = metricOpt->getMetric(2);
        AlgorithmCiftiReplaceStructure(NULL, &myCifti, myDir, myStruct, metricIn);
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
        VolumeFile* volIn = volumeOpt->getOutputVolume(2);
        bool fromCropVol = volumeOpt->getOptionalParameter(3)->m_present;
        AlgorithmCiftiReplaceStructure(NULL, &myCifti, myDir, myStruct, volIn, fromCropVol);
    }
    OptionalParameter* labelOpt = myParams->getOptionalParameter(5);
    if (labelOpt->m_present)
    {
        AString structName = labelOpt->getString(1);
        bool ok = false;
        StructureEnum::Enum myStruct = StructureEnum::fromName(structName, &ok);
        if (!ok)
        {
            throw AlgorithmException("unrecognized structure type");
        }
        LabelFile* labelIn = labelOpt->getLabel(2);
        AlgorithmCiftiReplaceStructure(NULL, &myCifti, myDir, myStruct, labelIn);
    }
}

AlgorithmCiftiReplaceStructure::AlgorithmCiftiReplaceStructure(ProgressObject* myProgObj, CiftiFile* ciftiInOut, const int& myDir,
                                                               const StructureEnum::Enum& myStruct, const MetricFile* metricIn) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<CiftiSurfaceMap> myMap;
    int rowSize = ciftiInOut->getNumberOfColumns(), colSize = ciftiInOut->getNumberOfRows();
    if (myDir == CiftiXML::ALONG_COLUMN)
    {
        if (!ciftiInOut->getSurfaceMapForColumns(myMap, myStruct))
        {
            throw AlgorithmException("structure not found in specified dimension");
        }
        int64_t numNodes = ciftiInOut->getColumnSurfaceNumberOfNodes(myStruct);
        if (metricIn->getNumberOfNodes() != numNodes)
        {
            throw AlgorithmException("input metric has the wrong number of vertices");
        }
        if (metricIn->getNumberOfColumns() != rowSize)
        {
            throw AlgorithmException("input metric has the wrong number of columns");
        }
        int mapSize = (int)myMap.size();
        CaretArray<float> rowScratch(rowSize);
        for (int i = 0; i < mapSize; ++i)
        {
            for (int j = 0; j < rowSize; ++j)
            {
                rowScratch[j] = metricIn->getValue(myMap[i].m_surfaceNode, j);
            }
            ciftiInOut->setRow(rowScratch, myMap[i].m_ciftiIndex);
        }
    } else {
        if (myDir != CiftiXML::ALONG_ROW) throw AlgorithmException("unsupported cifti direction");
        if (!ciftiInOut->getSurfaceMapForRows(myMap, myStruct))
        {
            throw AlgorithmException("structure not found in specified dimension");
        }
        int64_t numNodes = ciftiInOut->getRowSurfaceNumberOfNodes(myStruct);
        if (metricIn->getNumberOfNodes() != numNodes)
        {
            throw AlgorithmException("input metric has the wrong number of vertices");
        }
        if (metricIn->getNumberOfColumns() != colSize)
        {
            throw AlgorithmException("input metric has the wrong number of columns");
        }
        int mapSize = (int)myMap.size();
        CaretArray<float> rowScratch(rowSize);
        for (int i = 0; i < colSize; ++i)
        {
            ciftiInOut->getRow(rowScratch, i, true);
            for (int j = 0; j < mapSize; ++j)
            {
                rowScratch[myMap[j].m_ciftiIndex] = metricIn->getValue(myMap[j].m_surfaceNode, i);
            }
            ciftiInOut->setRow(rowScratch, myMap[i].m_ciftiIndex);
        }
    }
}

AlgorithmCiftiReplaceStructure::AlgorithmCiftiReplaceStructure(ProgressObject* myProgObj, CiftiFile* ciftiInOut, const int& myDir,
                                                               const StructureEnum::Enum& myStruct, const LabelFile* labelIn) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    ciftiInOut->convertToInMemory();
    vector<CiftiSurfaceMap> myMap;
    int64_t rowSize = ciftiInOut->getNumberOfColumns(), colSize = ciftiInOut->getNumberOfRows();
    if (myDir == CiftiXML::ALONG_COLUMN)
    {
        if (ciftiInOut->getCiftiXML().getRowMappingType() != CIFTI_INDEX_TYPE_LABELS) throw AlgorithmException("label separate requested on non-label cifti");
        if (!ciftiInOut->getSurfaceMapForColumns(myMap, myStruct))
        {
            throw AlgorithmException("structure not found in specified dimension");
        }
        int64_t numNodes = ciftiInOut->getColumnSurfaceNumberOfNodes(myStruct);
        if (labelIn->getNumberOfNodes() != numNodes)
        {
            throw AlgorithmException("input label file has the wrong number of vertices");
        }
        if (labelIn->getNumberOfColumns() != rowSize)
        {
            throw AlgorithmException("input label file has the wrong number of columns");
        }
        int64_t mapSize = (int64_t)myMap.size();
        CaretArray<float> rowScratch(rowSize, 0.0f);//initialize to the default "unused" value in case we get short reads due to unallocated on-disk cifti
        vector<set<int32_t> > usedArray(rowSize);
        vector<map<int32_t, int32_t> > remapArray(rowSize);
        for (int64_t j = 0; j < rowSize; ++j)
        {
            GiftiLabelTable myTable = *(labelIn->getLabelTable());//we remap the old label table values so that the new label table keys are unmolested
            remapArray[j] = myTable.append(*(ciftiInOut->getCiftiXML().getLabelTableForRowIndex(j)));
            *(ciftiInOut->getCiftiXML().getLabelTableForRowIndex(j)) = myTable;
        }
        set<int64_t> writeRows;
        for (int64_t i = 0; i < mapSize; ++i)
        {
            writeRows.insert(myMap[i].m_ciftiIndex);
        }
        for (int64_t i = 0; i < colSize; ++i)//we have to remap all old data since we are changing he keys of the old label table
        {
            if (writeRows.find(i) == writeRows.end())
            {
                ciftiInOut->getRow(rowScratch, i, true);
                bool rowChanged = false;
                for (int64_t j = 0; j < rowSize; ++j)
                {
                    int32_t tempKey = (int32_t)floor(rowScratch[j] + 0.5f);
                    map<int32_t, int32_t>::iterator iter = remapArray[j].find(tempKey);
                    if (iter != remapArray[j].end())
                    {
                        if (iter->first != iter->second)
                        {
                            rowChanged = true;
                            rowScratch[j] = iter->second;//remap the key if its value changes
                            usedArray[j].insert(iter->second);
                        } else {
                            usedArray[j].insert(tempKey);
                        }
                    } else {
                        rowChanged = true;
                        int32_t unusedLabel = ciftiInOut->getCiftiXML().getLabelTableForRowIndex(j)->getUnassignedLabelKey();
                        rowScratch[j] = unusedLabel;
                        usedArray[j].insert(unusedLabel);
                    }
                }
                if (rowChanged) ciftiInOut->setRow(rowScratch, i);
            }
        }
        for (int64_t i = 0; i < mapSize; ++i)//set the new rows
        {
            for (int64_t j = 0; j < rowSize; ++j)
            {
                int32_t tempKey = labelIn->getLabelKey(myMap[i].m_surfaceNode, j);
                usedArray[j].insert(tempKey);
                rowScratch[j] = tempKey;
            }
            ciftiInOut->setRow(rowScratch, myMap[i].m_ciftiIndex);
        }
        for (int64_t i = 0; i < rowSize; ++i)//delete unused labels
        {
            ciftiInOut->getCiftiXML().getLabelTableForRowIndex(i)->deleteUnusedLabels(usedArray[i]);
        }
    } else {
        if (myDir != CiftiXML::ALONG_ROW) throw AlgorithmException("unsupported cifti direction");
        if (ciftiInOut->getCiftiXML().getColumnMappingType() != CIFTI_INDEX_TYPE_LABELS) throw AlgorithmException("label separate requested on non-label cifti");
        if (!ciftiInOut->getSurfaceMapForRows(myMap, myStruct))
        {
            throw AlgorithmException("structure not found in specified dimension");
        }
        int64_t numNodes = ciftiInOut->getRowSurfaceNumberOfNodes(myStruct);
        if (labelIn->getNumberOfNodes() != numNodes)
        {
            throw AlgorithmException("input label file has the wrong number of vertices");
        }
        if (labelIn->getNumberOfColumns() != colSize)
        {
            throw AlgorithmException("input label file has the wrong number of columns");
        }
        int64_t mapSize = (int64_t)myMap.size();
        CaretArray<float> rowScratch(rowSize, 0.0f);
        set<int64_t> writeCols;
        for (int64_t i = 0; i < mapSize; ++i)
        {
            writeCols.insert(myMap[i].m_ciftiIndex);
        }
        for (int64_t i = 0; i < colSize; ++i)
        {
            GiftiLabelTable myTable = *(labelIn->getLabelTable());//we remap the old label table values so that the new label table keys are unmolested
            map<int32_t, int32_t> remap = myTable.append(*(ciftiInOut->getCiftiXML().getLabelTableForColumnIndex(i)));
            *(ciftiInOut->getCiftiXML().getLabelTableForColumnIndex(i)) = myTable;
            ciftiInOut->getRow(rowScratch, i, true);
            set<int32_t> used;
            int32_t unusedKey = myTable.getUnassignedLabelKey();
            for(int64_t j = 0; j < rowSize; ++j)
            {
                if (writeCols.find(j) == writeCols.end())
                {
                    int32_t tempKey = (int32_t)floor(rowScratch[j] + 0.5f);
                    map<int32_t, int32_t>::iterator iter = remap.find(tempKey);
                    if (iter != remap.end())
                    {
                        rowScratch[j] = iter->second;
                        used.insert(iter->second);
                    } else {
                        rowScratch[j] = unusedKey;
                        used.insert(unusedKey);
                    }
                }
            }
            for (int64_t j = 0; j < mapSize; ++j)
            {
                int32_t tempKey = labelIn->getLabelKey(myMap[j].m_surfaceNode, i);
                rowScratch[myMap[j].m_ciftiIndex] = tempKey;
                used.insert(tempKey);
            }
            ciftiInOut->getCiftiXML().getLabelTableForColumnIndex(i)->deleteUnusedLabels(used);
            ciftiInOut->setRow(rowScratch, myMap[i].m_ciftiIndex);
        }
    }
}

AlgorithmCiftiReplaceStructure::AlgorithmCiftiReplaceStructure(ProgressObject* myProgObj, CiftiFile* ciftiInOut, const int& myDir,
                                                               const StructureEnum::Enum& myStruct, const VolumeFile* volIn, const bool& fromCropped) : AbstractAlgorithm(myProgObj)
{
    const CiftiXML& myXML = ciftiInOut->getCiftiXML();
    LevelProgress myProgress(myProgObj);
    int64_t myDims[3], offset[3];
    vector<vector<float> > mySform;
    vector<CiftiVolumeMap> myMap;
    vector<int64_t> newdims;
    int64_t rowSize = ciftiInOut->getNumberOfColumns(), colSize = ciftiInOut->getNumberOfRows();
    if (!myXML.getVolumeDimsAndSForm(myDims, mySform))
    {
        throw AlgorithmException("input cifti has no volume space information");
    }
    if (myDir == CiftiXML::ALONG_COLUMN)
    {
        if (!myXML.getVolumeStructureMapForColumns(myMap, myStruct))
        {
            throw AlgorithmException("structure not found in specified dimension");
        }
    } else {
        if (myDir != CiftiXML::ALONG_ROW) throw AlgorithmException("direction not supported in cifti replace structure");
        if (!myXML.getVolumeStructureMapForRows(myMap, myStruct))
        {
            throw AlgorithmException("structure not found in specified dimension");
        }
    }
    int64_t numVoxels = (int64_t)myMap.size();
    if (fromCropped)
    {
        newdims.resize(3);
        AlgorithmCiftiSeparate::getCroppedVolSpace(ciftiInOut, myDir, myStruct, newdims.data(), mySform, offset);
    } else {
        newdims.push_back(myDims[0]);
        newdims.push_back(myDims[1]);
        newdims.push_back(myDims[2]);
        offset[0] = 0;
        offset[1] = 0;
        offset[2] = 0;
    }
    if (!volIn->matchesVolumeSpace(newdims.data(), mySform))
    {
        throw AlgorithmException("input volume doesn't match volume space and dimensions in CIFTI");
    }
    CaretArray<float> rowScratch(rowSize, 0.0f);//initialize with default unused label in case dlabel and short reads due to uninitialized on-disk cifti
    vector<int64_t> volDims;
    volIn->getDimensions(volDims);
    if (myDir == CiftiXML::ALONG_COLUMN)
    {
        if (volDims[3] != rowSize)
        {
            throw AlgorithmException("volume has the wrong number of subvolumes");
        }
        if (myXML.getRowMappingType() == CIFTI_INDEX_TYPE_LABELS)
        {
            if (volIn->getType() != SubvolumeAttributes::LABEL) throw ("replace structure called on cifti label file with non-label volume");
            ciftiInOut->convertToInMemory();
            vector<map<int32_t, int32_t> > remapArray(rowSize);
            vector<set<int32_t> > usedArray(rowSize);
            for (int64_t i = 0; i < rowSize; ++i)
            {
                GiftiLabelTable myTable = *(volIn->getMapLabelTable(i));//we remap the old label table values so that the new label table keys are unmolested
                remapArray[i] = myTable.append(*(myXML.getLabelTableForRowIndex(i)));
                *(myXML.getLabelTableForRowIndex(i)) = myTable;
            }
            set<int64_t> writeRows;
            for (int64_t i = 0; i < numVoxels; ++i)
            {
                writeRows.insert(myMap[i].m_ciftiIndex);
            }
            for (int64_t i = 0; i < colSize; ++i)
            {
                if (writeRows.find(i) == writeRows.end())
                {
                    ciftiInOut->getRow(rowScratch, i, true);
                    bool rowChanged = false;
                    for (int64_t j = 0; j < rowSize; ++j)
                    {
                        int32_t tempKey = (int32_t)floor(rowScratch[j] + 0.5f);
                        map<int32_t, int32_t>::iterator iter = remapArray[j].find(tempKey);
                        if (iter != remapArray[j].end())
                        {
                            if (iter->first != iter->second)
                            {
                                rowChanged = true;
                                rowScratch[j] = iter->second;//remap the key if its value changes
                                usedArray[j].insert(iter->second);
                            } else {
                                usedArray[j].insert(tempKey);
                            }
                        } else {
                            rowChanged = true;
                            int32_t unusedLabel = ciftiInOut->getCiftiXML().getLabelTableForRowIndex(j)->getUnassignedLabelKey();
                            rowScratch[j] = unusedLabel;
                            usedArray[j].insert(unusedLabel);
                        }
                    }
                    if (rowChanged) ciftiInOut->setRow(rowScratch, i);
                }
            }
            for (int64_t i = 0; i < numVoxels; ++i)
            {
                int64_t thisvoxel[3] = { myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2] };
                for (int j = 0; j < rowSize; ++j)
                {
                    int32_t tempKey = (int32_t)floor(volIn->getValue(thisvoxel, j) + 0.5f);
                    usedArray[j].insert(tempKey);
                    rowScratch[j] = tempKey;
                }
                ciftiInOut->setRow(rowScratch, myMap[i].m_ciftiIndex);
            }
            for (int64_t i = 0; i < rowSize; ++i)//delete unused labels
            {
                myXML.getLabelTableForRowIndex(i)->deleteUnusedLabels(usedArray[i]);
            }
        } else {
            for (int64_t i = 0; i < numVoxels; ++i)
            {
                int64_t thisvoxel[3] = { myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2] };
                for (int j = 0; j < rowSize; ++j)
                {
                    rowScratch[j] = volIn->getValue(thisvoxel, j);
                }
                ciftiInOut->setRow(rowScratch, myMap[i].m_ciftiIndex);
            }
        }
    } else {
        if (volDims[3] != colSize)
        {
            throw AlgorithmException("volume has the wrong number of subvolumes");
        }
        if (myXML.getColumnMappingType() == CIFTI_INDEX_TYPE_LABELS)
        {
            if (volIn->getType() != SubvolumeAttributes::LABEL) throw ("replace structure called on cifti label file with non-label volume");
            ciftiInOut->convertToInMemory();
            set<int64_t> writeCols;
            for (int64_t i = 0; i < numVoxels; ++i)
            {
                writeCols.insert(myMap[i].m_ciftiIndex);
            }
            for (int64_t i = 0; i < colSize; ++i)
            {
                GiftiLabelTable myTable = *(volIn->getMapLabelTable(i));//we remap the old label table values so that the new label table keys are unmolested
                map<int32_t, int32_t> remap = myTable.append(*(ciftiInOut->getCiftiXML().getLabelTableForColumnIndex(i)));
                *(ciftiInOut->getCiftiXML().getLabelTableForColumnIndex(i)) = myTable;
                ciftiInOut->getRow(rowScratch, i, true);
                set<int32_t> used;
                int32_t unusedKey = myTable.getUnassignedLabelKey();
                for(int64_t j = 0; j < rowSize; ++j)
                {
                    if (writeCols.find(j) == writeCols.end())
                    {
                        int32_t tempKey = (int32_t)floor(rowScratch[j] + 0.5f);
                        map<int32_t, int32_t>::iterator iter = remap.find(tempKey);
                        if (iter != remap.end())
                        {
                            rowScratch[j] = iter->second;
                            used.insert(iter->second);
                        } else {
                            rowScratch[j] = unusedKey;
                            used.insert(unusedKey);
                        }
                    }
                }
                for (int64_t j = 0; j < numVoxels; ++j)
                {
                    int64_t thisvoxel[3] = { myMap[j].m_ijk[0] - offset[0], myMap[j].m_ijk[1] - offset[1], myMap[j].m_ijk[2] - offset[2] };
                    int32_t tempKey = (int32_t)floor(volIn->getValue(thisvoxel, i) + 0.5f);
                    rowScratch[myMap[j].m_ciftiIndex] = tempKey;
                    used.insert(tempKey);
                }
                ciftiInOut->getCiftiXML().getLabelTableForColumnIndex(i)->deleteUnusedLabels(used);
                ciftiInOut->setRow(rowScratch, myMap[i].m_ciftiIndex);
            }
        } else {
            for (int64_t i = 0; i < colSize; ++i)
            {
                ciftiInOut->getRow(rowScratch, i, true);//the on-disk cifti file may not have been allocated yet, so short reads are okay
                for (int64_t j = 0; j < numVoxels; ++j)
                {
                    int64_t thisvoxel[3] = { myMap[j].m_ijk[0] - offset[0], myMap[j].m_ijk[1] - offset[1], myMap[j].m_ijk[2] - offset[2] };
                    rowScratch[myMap[j].m_ciftiIndex] = volIn->getValue(thisvoxel, i);
                }
                ciftiInOut->setRow(rowScratch, i);
            }
        }
    }
}

float AlgorithmCiftiReplaceStructure::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiReplaceStructure::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
