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

#include "OperationWbsparseMergeDense.h"
#include "OperationException.h"

#include "CaretSparseFile.h"

using namespace caret;
using namespace std;

AString OperationWbsparseMergeDense::getCommandSwitch()
{
    return "-wbsparse-merge-dense";
}

AString OperationWbsparseMergeDense::getShortDescription()
{
    return "MERGE WBSPARSE FILES ALONG DENSE DIMENSION";
}

OperationParameters* OperationWbsparseMergeDense::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "direction", "which dimension to merge along, ROW or COLUMN");
    
    ret->addStringParameter(2, "wbsparse-out", "output - the output wbsparse file");//HACK: fake the output format since we don't have a wbsparse parameter type (or file type, really)
    
    ParameterComponent* wbsparseOpt = ret->createRepeatableParameter(3, "-wbsparse", "specify an input wbsparse file");
    wbsparseOpt->addStringParameter(1, "wbsparse-in", "a wbsparse file to merge");
    
    ret->setHelpText(
        AString("The input wbsparse files must have matching mappings along the direction not specified, and the mapping along the specified direction must be brain models.")
    );
    return ret;
}

void OperationWbsparseMergeDense::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString directionName = myParams->getString(1);
    int myDir;
    if (directionName == "ROW")
    {
        myDir = CiftiXML::ALONG_ROW;
    } else if (directionName == "COLUMN") {
        myDir = CiftiXML::ALONG_COLUMN;
    } else {
        throw OperationException("incorrect string for direction, use ROW or COLUMN");
    }
    AString outputName = myParams->getString(2);
    const vector<ParameterComponent*>& myInstances = myParams->getRepeatableParameterInstances(3);
    vector<CaretPointer<CaretSparseFile> > wbsparseList;
    int numCifti = (int)myInstances.size();
    for (int i = 0; i < numCifti; ++i)
    {
        wbsparseList.push_back(CaretPointer<CaretSparseFile>(new CaretSparseFile(myInstances[i]->getString(1))));
    }
    if (wbsparseList.size() == 0) throw OperationException("no files specified");
    if (myDir != CiftiXML::ALONG_ROW && myDir != CiftiXML::ALONG_COLUMN) throw OperationException("direction not supported by wbsparse merge dense");
    int otherDir = 1 - myDir;//find the other direction
    const CiftiXML& baseXML = wbsparseList[0]->getCiftiXML();
    if (baseXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS) throw OperationException("mapping type along specified dimension is not brain models");
    if (baseXML.getMappingType(otherDir) == CiftiMappingType::LABELS)throw OperationException("labels not supported in wbsparse merge dense");
    CiftiXML outXML = baseXML;
    VolumeSpace baseSpace;
    const CiftiBrainModelsMap& baseDenseMap = baseXML.getBrainModelsMap(myDir);
    CiftiBrainModelsMap newDenseMap = baseDenseMap;
    bool haveVolSpace = false;
    if (baseDenseMap.hasVolumeData())
    {
        haveVolSpace = true;
        baseSpace = baseDenseMap.getVolumeSpace();
    }
    vector<int> sourceWbsparse(baseDenseMap.getModelInfo().size(), 0);
    for (int i = 1; i < (int)wbsparseList.size(); ++i)
    {
        const CiftiXML& otherXML = wbsparseList[i]->getCiftiXML();
        if (*(baseXML.getMap(otherDir)) != *(otherXML.getMap(otherDir)))
        {
            throw OperationException("mappings along other dimension do not match");
        }
        if (otherXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS) throw OperationException("input files do not have brain models mapping on merge dimension");
        const CiftiBrainModelsMap& otherDenseMap = otherXML.getBrainModelsMap(myDir);
        if (otherDenseMap.hasVolumeData())
        {
            if (haveVolSpace)
            {
                if (!baseSpace.matches(otherDenseMap.getVolumeSpace())) throw OperationException("input files have non-matching volume spaces");
            } else {
                haveVolSpace = true;
                baseSpace = otherDenseMap.getVolumeSpace();
                newDenseMap.setVolumeSpace(baseSpace);//need to set the output vol space if the first input didn't have volume data
            }
        }
        vector<CiftiBrainModelsMap::ModelInfo> otherModels = otherDenseMap.getModelInfo();
        int numModels = (int)otherModels.size();
        for (int j = 0; j < numModels; ++j)
        {
            sourceWbsparse.push_back(i);
            const CiftiBrainModelsMap::ModelInfo& myInfo = otherModels[j];
            switch (myInfo.m_type)
            {
                case CiftiBrainModelsMap::SURFACE:
                {
                    vector<CiftiBrainModelsMap::SurfaceMap> myMap = otherDenseMap.getSurfaceMap(myInfo.m_structure);
                    vector<int64_t> nodeList(myMap.size());
                    for (int64_t k = 0; k < (int64_t)myMap.size(); ++k)
                    {
                        nodeList[k] = myMap[k].m_surfaceNode;
                    }
                    newDenseMap.addSurfaceModel(otherDenseMap.getSurfaceNumberOfNodes(myInfo.m_structure), myInfo.m_structure, nodeList);
                    break;
                }
                case CiftiBrainModelsMap::VOXELS:
                {
                    vector<CiftiBrainModelsMap::VolumeMap> myMap = otherDenseMap.getVolumeStructureMap(myInfo.m_structure);
                    vector<int64_t> voxelList(myMap.size() * 3);
                    for (int64_t k = 0; k < (int64_t)myMap.size(); ++k)
                    {
                        int64_t k3 = k * 3;
                        voxelList[k3] = myMap[k].m_ijk[0];
                        voxelList[k3 + 1] = myMap[k].m_ijk[1];
                        voxelList[k3 + 2] = myMap[k].m_ijk[2];
                    }
                    newDenseMap.addVolumeModel(myInfo.m_structure, voxelList);
                    break;
                }
                default:
                    throw OperationException("encountered unknown model type in cifti merge dense");
            }
        }
    }
    outXML.setMap(myDir, newDenseMap);
    int numOutModels = (int)sourceWbsparse.size();
    CaretAssert(numOutModels == (int)newDenseMap.getModelInfo().size());
    int64_t outColSize = outXML.getDimensionLength(CiftiXML::ALONG_COLUMN);
    CaretSparseFileWriter myWriter(outputName, outXML);
    vector<CiftiBrainModelsMap::ModelInfo> outModelInfo = newDenseMap.getModelInfo();
    switch (myDir)
    {
        case CiftiXML::ALONG_ROW:
        {
            for (int64_t i = 0; i < outColSize; ++i)
            {
                int64_t curOffset = 0;
                vector<int64_t> outIndices, outValues, inIndices, inValues;
                int loaded = -1;
                for (int j = 0; j < numOutModels; ++j)//we could just do the entire row for each file, but doing it by structure could allow structure selection in the future
                {
                    const CiftiBrainModelsMap::ModelInfo& myInfo = outModelInfo[j];
                    const CiftiXML& thisXML = wbsparseList[sourceWbsparse[j]]->getCiftiXML();
                    const CiftiBrainModelsMap& thisDenseMap = thisXML.getBrainModelsMap(myDir);
                    int64_t startIndex = -1, endIndex = -1;
                    switch (myInfo.m_type)
                    {
                        case CiftiBrainModelsMap::SURFACE:
                        {
                            vector<CiftiBrainModelsMap::SurfaceMap> tempMap = thisDenseMap.getSurfaceMap(myInfo.m_structure);
                            if (tempMap.size() > 0)
                            {
                                startIndex = tempMap[0].m_ciftiIndex;//NOTE: CiftiXML guarantees these are ordered by cifti index and contiguous
                                endIndex = startIndex + tempMap.size();
                            } else {
                                startIndex = 0;
                                endIndex = 0;
                            }
                            break;
                        }
                        case CiftiBrainModelsMap::VOXELS:
                        {
                            vector<CiftiBrainModelsMap::VolumeMap> tempMap = thisDenseMap.getVolumeStructureMap(myInfo.m_structure);
                            if (tempMap.size() > 0)
                            {
                                startIndex = tempMap[0].m_ciftiIndex;//NOTE: CiftiXML guarantees these are ordered by cifti index and contiguous
                                endIndex = startIndex + tempMap.size();
                            } else {
                                startIndex = 0;
                                endIndex = 0;
                            }
                            break;
                        }
                        default:
                            CaretAssert(false);
                            break;
                    }
                    if (endIndex > startIndex)
                    {
                        if (loaded != sourceWbsparse[j])
                        {
                            wbsparseList[sourceWbsparse[j]]->getRowSparse(i, inIndices, inValues);
                            loaded = sourceWbsparse[j];
                        }
                        int64_t numSparse = (int64_t)inIndices.size();
                        for (int64_t k = 0; k < numSparse; ++k)
                        {
                            if (inIndices[k] >= startIndex && inIndices[k] < endIndex)
                            {
                                outIndices.push_back(inIndices[k] + curOffset);
                                outValues.push_back(inValues[k]);
                            }
                        }
                        curOffset += endIndex - startIndex;
                    }
                }
                myWriter.writeRowSparse(i, outIndices, outValues);
                outIndices.clear();//reset for next row
                outValues.clear();
            }
            break;
        }
        case CiftiXML::ALONG_COLUMN:
        {
            vector<int64_t> inIndices, inValues;
            for (int j = 0; j < numOutModels; ++j)
            {
                const CiftiBrainModelsMap::ModelInfo& myInfo = outModelInfo[j];
                const CiftiXML& thisXML = wbsparseList[sourceWbsparse[j]]->getCiftiXML();
                const CiftiBrainModelsMap& thisDenseMap = thisXML.getBrainModelsMap(myDir);
                switch (myInfo.m_type)
                {
                    case CiftiBrainModelsMap::SURFACE:
                    {
                        vector<CiftiBrainModelsMap::SurfaceMap> tempMap = thisDenseMap.getSurfaceMap(myInfo.m_structure), outMap = newDenseMap.getSurfaceMap(myInfo.m_structure);
                        int64_t mapSize = (int64_t)tempMap.size();
                        CaretAssert(mapSize == (int64_t)outMap.size());
                        for (int64_t k = 0; k < mapSize; ++k)
                        {
                            CaretAssert(tempMap[k].m_surfaceNode == outMap[k].m_surfaceNode);
                            wbsparseList[sourceWbsparse[j]]->getRowSparse(tempMap[k].m_ciftiIndex, inIndices, inValues);
                            myWriter.writeRowSparse(outMap[k].m_ciftiIndex, inIndices, inValues);
                        }
                        break;
                    }
                    case CiftiBrainModelsMap::VOXELS:
                    {
                        vector<CiftiBrainModelsMap::VolumeMap> tempMap = thisDenseMap.getVolumeStructureMap(myInfo.m_structure), outMap = newDenseMap.getVolumeStructureMap(myInfo.m_structure);
                        int64_t mapSize = (int64_t)tempMap.size();
                        CaretAssert(mapSize == (int64_t)outMap.size());
                        for (int64_t k = 0; k < mapSize; ++k)
                        {
                            CaretAssert(tempMap[k].m_ijk[0] == outMap[k].m_ijk[0]);
                            CaretAssert(tempMap[k].m_ijk[1] == outMap[k].m_ijk[1]);
                            CaretAssert(tempMap[k].m_ijk[2] == outMap[k].m_ijk[2]);
                            wbsparseList[sourceWbsparse[j]]->getRowSparse(tempMap[k].m_ciftiIndex, inIndices, inValues);
                            myWriter.writeRowSparse(outMap[k].m_ciftiIndex, inIndices, inValues);
                        }
                        break;
                    }
                    default:
                        CaretAssert(false);
                        break;
                }
            }
            break;
        }
        default:
            CaretAssert(false);
            break;
    }
    myWriter.finish();
}
