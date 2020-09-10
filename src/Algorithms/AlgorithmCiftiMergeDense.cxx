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

#include "AlgorithmCiftiMergeDense.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiReplaceStructure.h"
#include "AlgorithmCiftiSeparate.h"
#include "CiftiFile.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiMergeDense::getCommandSwitch()
{
    return "-cifti-merge-dense";
}

AString AlgorithmCiftiMergeDense::getShortDescription()
{
    return "MERGE CIFTI FILES ALONG DENSE DIMENSION";
}

OperationParameters* AlgorithmCiftiMergeDense::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "direction", "which dimension to merge along, ROW or COLUMN");
    
    ret->addCiftiOutputParameter(2, "cifti-out", "the output cifti file");
    
    ParameterComponent* ciftiOpt = ret->createRepeatableParameter(3, "-cifti", "specify an input cifti file");
    ciftiOpt->addCiftiParameter(1, "cifti-in", "a cifti file to merge");

    OptionalParameter* collideOpt = ret->createOptionalParameter(4, "-label-collision", "how to handle conflicts between label keys");
    collideOpt->addStringParameter(1, "action", "'ERROR', 'FIRST', or 'LEGACY', default 'ERROR', use 'LEGACY' to match v1.4.2 and earlier");
    
    ret->setHelpText(
        AString("The input cifti files must have matching mappings along the direction not specified, and the mapping along the specified direction must be brain models.")
    );
    return ret;
}

void AlgorithmCiftiMergeDense::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    AString directionName = myParams->getString(1);
    int myDir;
    if (directionName == "ROW")
    {
        myDir = CiftiXMLOld::ALONG_ROW;
    } else if (directionName == "COLUMN") {
        myDir = CiftiXMLOld::ALONG_COLUMN;
    } else {
        throw AlgorithmException("incorrect string for direction, use ROW or COLUMN");
    }
    CiftiFile* myCiftiOut = myParams->getOutputCifti(2);
    const vector<ParameterComponent*>& myInstances = myParams->getRepeatableParameterInstances(3);
    vector<const CiftiFile*> ciftiList;
    int numCifti = (int)myInstances.size();
    for (int i = 0; i < numCifti; ++i)
    {
        ciftiList.push_back(myInstances[i]->getCifti(1));
    }
    OptionalParameter* collideOpt = myParams->getOptionalParameter(4);
    LabelConflictLogic conflictLogic = ERROR;
    if (collideOpt->m_present)
    {
        AString collideStr = collideOpt->getString(1);
        if (collideStr == "ERROR")
        {
            conflictLogic = ERROR;
        } else if (collideStr == "FIRST") {
            conflictLogic = FIRST;
        } else if (collideStr == "LEGACY") {
            conflictLogic = LEGACY;
        } else {
            throw AlgorithmException("incorrect string for label collision option");
        }
    }
    AlgorithmCiftiMergeDense(myProgObj, myDir, ciftiList, myCiftiOut, conflictLogic);
}

AlgorithmCiftiMergeDense::AlgorithmCiftiMergeDense(ProgressObject* myProgObj, const int& myDir, const vector<const CiftiFile*>& ciftiList, CiftiFile* myCiftiOut,
                                                   const LabelConflictLogic conflictLogic) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (ciftiList.size() == 0) throw AlgorithmException("no input files specified");
    CaretAssert(ciftiList[0] != NULL);
    if (myDir != CiftiXMLOld::ALONG_ROW && myDir != CiftiXMLOld::ALONG_COLUMN) throw AlgorithmException("direction not supported by cifti merge dense");
    int otherDir = 1 - myDir;//find the other direction
    const CiftiXMLOld& baseXML = ciftiList[0]->getCiftiXMLOld();
    if (baseXML.getMappingType(myDir) != CIFTI_INDEX_TYPE_BRAIN_MODELS) throw AlgorithmException("mapping type along specified dimension is not brain models");
    bool isLabel = (baseXML.getMappingType(otherDir) == CIFTI_INDEX_TYPE_LABELS);
    CiftiXMLOld outXML = baseXML;
    VolumeSpace baseSpace;
    bool haveVolSpace = false;
    if (baseXML.hasVolumeData(myDir))
    {
        haveVolSpace = true;
        baseXML.getVolumeSpace(baseSpace);
    }
    vector<int> sourceCifti(baseXML.getNumberOfBrainModels(myDir), 0);
    for (int i = 1; i < (int)ciftiList.size(); ++i)
    {
        CaretAssert(ciftiList[i] != NULL);
        const CiftiXMLOld& otherXML = ciftiList[i]->getCiftiXMLOld();
        if (!ciftiList[0]->getCiftiXML().getMap(otherDir)->approximateMatch(*(ciftiList[i]->getCiftiXML().getMap(otherDir))))
        {
            throw AlgorithmException("mappings along other dimension do not match");
        }
        if (otherXML.hasVolumeData(myDir))
        {
            if (haveVolSpace)
            {
                VolumeSpace otherSpace;
                otherXML.getVolumeSpace(otherSpace);
                if (!baseSpace.matches(otherSpace)) throw AlgorithmException("input cifti files have non-matching volume spaces");
            } else {
                haveVolSpace = true;
                otherXML.getVolumeSpace(baseSpace);
                outXML.setVolumeDimsAndSForm(baseSpace.getDims(), baseSpace.getSform());//need to set the output vol space if the first input didn't have volume data
            }
        }
        if (otherXML.getMappingType(myDir) != CIFTI_INDEX_TYPE_BRAIN_MODELS) throw AlgorithmException("mapping type along specified dimension is not brain models");
        if ((otherXML.getMappingType(otherDir) == CIFTI_INDEX_TYPE_LABELS) != isLabel) throw AlgorithmException("input files to cifti merge dense mix label and non-label data");
        int numModels = otherXML.getNumberOfBrainModels(myDir);
        for (int j = 0; j < numModels; ++j)
        {
            sourceCifti.push_back(i);
            CiftiBrainModelInfo myInfo = otherXML.getBrainModelInfo(myDir, j);
            switch (myInfo.m_type)
            {
                case CIFTI_MODEL_TYPE_SURFACE:
                {
                    vector<CiftiSurfaceMap> myMap;
                    otherXML.getSurfaceMap(myDir, myMap, myInfo.m_structure);
                    vector<int64_t> nodeList(myMap.size());
                    for (int64_t k = 0; k < (int64_t)myMap.size(); ++k)
                    {
                        nodeList[k] = myMap[k].m_surfaceNode;
                    }
                    if (!outXML.addSurfaceModel(myDir, otherXML.getSurfaceNumberOfNodes(myDir, myInfo.m_structure), myInfo.m_structure, nodeList))
                    {
                        throw AlgorithmException("duplicate surface structure " + StructureEnum::toName(myInfo.m_structure) + " found in input to cifti merge dense");
                    }
                    break;
                }
                case CIFTI_MODEL_TYPE_VOXELS:
                {
                    vector<CiftiVolumeMap> myMap;
                    otherXML.getVolumeStructureMap(myDir, myMap, myInfo.m_structure);
                    vector<voxelIndexType> voxelList(myMap.size() * 3);
                    for (int64_t k = 0; k < (int64_t)myMap.size(); ++k)
                    {
                        int64_t k3 = k * 3;
                        voxelList[k3] = myMap[k].m_ijk[0];
                        voxelList[k3 + 1] = myMap[k].m_ijk[1];
                        voxelList[k3 + 2] = myMap[k].m_ijk[2];
                    }
                    if (!outXML.addVolumeModel(myDir, voxelList, myInfo.m_structure))
                    {
                        throw AlgorithmException("duplicate volume structure " + StructureEnum::toName(myInfo.m_structure) + " found in input to cifti merge dense");
                    }
                    break;
                }
                default:
                    throw AlgorithmException("encountered unknown model type in cifti merge dense");
            }
        }
    }
    CaretAssert((int)sourceCifti.size() == outXML.getNumberOfBrainModels(myDir));
    myCiftiOut->setCiftiXML(outXML);
    for (int i_raw = 0; i_raw < (int)sourceCifti.size(); ++i_raw)
    {
        int i = i_raw;
        if (conflictLogic == FIRST)
        {
            i = int(sourceCifti.size()) - i_raw - 1;//loop through Replaces backwards to give precedence to first model (which comes from the first argument)
        }
        CiftiBrainModelInfo myInfo = outXML.getBrainModelInfo(myDir, i);
        switch (myInfo.m_type)
        {
            case CIFTI_MODEL_TYPE_SURFACE:
            {
                if (isLabel)
                {
                    //is a generic message from ciftireplace/table::append good enough?
                    LabelFile tempFile;
                    AlgorithmCiftiSeparate(NULL, ciftiList[sourceCifti[i]], myDir, myInfo.m_structure, &tempFile);//using this because dealing with label tables is nasty, but doesn't happen on large files
                    AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, myInfo.m_structure, &tempFile, false, (conflictLogic == ERROR));
                } else {//for everything else, just use rows directly, because making large metric files in-memory is problematic
                    vector<CiftiSurfaceMap> inMap, outMap;
                    const CiftiXMLOld& otherXML = ciftiList[sourceCifti[i]]->getCiftiXMLOld();
                    outXML.getSurfaceMap(myDir, outMap, myInfo.m_structure);
                    otherXML.getSurfaceMap(myDir, inMap, myInfo.m_structure);
                    CaretAssert(inMap.size() == outMap.size());
                    vector<float> rowscratch(outXML.getNumberOfColumns()), otherscratch(otherXML.getNumberOfColumns());
                    if (myDir == CiftiXMLOld::ALONG_ROW)
                    {
                        for (int j = 0; j < outXML.getNumberOfRows(); ++j)
                        {
                            myCiftiOut->getRow(rowscratch.data(), j, true);
                            ciftiList[sourceCifti[i]]->getRow(otherscratch.data(), j);
                            for (int k = 0; k < (int)inMap.size(); ++k)
                            {
                                CaretAssert(inMap[k].m_surfaceNode == outMap[k].m_surfaceNode);
                                rowscratch[outMap[k].m_ciftiIndex] = otherscratch[inMap[k].m_ciftiIndex];
                            }
                            myCiftiOut->setRow(rowscratch.data(), j);
                        }
                    } else {
                        for (int k = 0; k < (int)inMap.size(); ++k)
                        {
                            CaretAssert(inMap[k].m_surfaceNode == outMap[k].m_surfaceNode);
                            ciftiList[sourceCifti[i]]->getRow(otherscratch.data(), inMap[k].m_ciftiIndex);
                            myCiftiOut->setRow(otherscratch.data(), outMap[k].m_ciftiIndex);
                        }
                    }
                }
                break;
            }
            case CIFTI_MODEL_TYPE_VOXELS:
            {
                if (isLabel)
                {//cropped volume should be okay on memory for label files - let replace structure handle the label table
                    VolumeFile tempFile;
                    int64_t junk[3];
                    AlgorithmCiftiSeparate(NULL, ciftiList[sourceCifti[i]], myDir, myInfo.m_structure, &tempFile, junk, NULL, true);
                    AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, myInfo.m_structure, &tempFile, true);
                } else {
                    vector<CiftiVolumeMap> inMap, outMap;
                    const CiftiXMLOld& otherXML = ciftiList[sourceCifti[i]]->getCiftiXMLOld();
                    outXML.getVolumeStructureMap(myDir, outMap, myInfo.m_structure);
                    otherXML.getVolumeStructureMap(myDir, inMap, myInfo.m_structure);
                    CaretAssert(inMap.size() == outMap.size());
                    vector<float> rowscratch(outXML.getNumberOfColumns()), otherscratch(otherXML.getNumberOfColumns());
                    if (myDir == CiftiXMLOld::ALONG_ROW)
                    {
                        for (int j = 0; j < outXML.getNumberOfRows(); ++j)
                        {
                            myCiftiOut->getRow(rowscratch.data(), j, true);
                            ciftiList[sourceCifti[i]]->getRow(otherscratch.data(), j);
                            for (int k = 0; k < (int)inMap.size(); ++k)
                            {
                                CaretAssert(inMap[k].m_ijk[0] == outMap[k].m_ijk[0]);
                                CaretAssert(inMap[k].m_ijk[1] == outMap[k].m_ijk[1]);
                                CaretAssert(inMap[k].m_ijk[2] == outMap[k].m_ijk[2]);
                                rowscratch[outMap[k].m_ciftiIndex] = otherscratch[inMap[k].m_ciftiIndex];
                            }
                            myCiftiOut->setRow(rowscratch.data(), j);
                        }
                    } else {
                        for (int k = 0; k < (int)inMap.size(); ++k)
                        {
                            CaretAssert(inMap[k].m_ijk[0] == outMap[k].m_ijk[0]);
                            CaretAssert(inMap[k].m_ijk[1] == outMap[k].m_ijk[1]);
                            CaretAssert(inMap[k].m_ijk[2] == outMap[k].m_ijk[2]);
                            ciftiList[sourceCifti[i]]->getRow(otherscratch.data(), inMap[k].m_ciftiIndex);
                            myCiftiOut->setRow(otherscratch.data(), outMap[k].m_ciftiIndex);
                        }
                    }
                }
                break;
            }
            default:
                throw AlgorithmException("encountered unknown model type in cifti merge dense");
        }
    }
}

float AlgorithmCiftiMergeDense::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiMergeDense::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
