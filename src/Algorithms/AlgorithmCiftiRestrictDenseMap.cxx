/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

#include "AlgorithmCiftiRestrictDenseMap.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "MultiDimIterator.h"
#include "VolumeFile.h"

#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmCiftiRestrictDenseMap::getCommandSwitch()
{
    return "-cifti-restrict-dense-map";
}

AString AlgorithmCiftiRestrictDenseMap::getShortDescription()
{
    return "EXCLUDE BRAINORDINATES FROM A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiRestrictDenseMap::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-in", "the input cifti");
    
    ret->addStringParameter(2, "direction", "which dimension to change the mapping on (integer, 'ROW', or 'COLUMN')");
    
    ret->addCiftiOutputParameter(3, "cifti-out", "the output cifti");
    
    OptionalParameter* ciftiRoiOpt = ret->createOptionalParameter(4, "-cifti-roi", "cifti file containing combined rois");
    ciftiRoiOpt->addCiftiParameter(1, "roi-cifti", "the rois as a cifti file");
    
    OptionalParameter* leftRoiOpt = ret->createOptionalParameter(5, "-left-roi", "vertices to use from left hemisphere");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the left roi as a metric file");
    
    OptionalParameter* rightRoiOpt = ret->createOptionalParameter(6, "-right-roi", "vertices to use from right hemisphere");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the right roi as a metric file");
    
    OptionalParameter* cerebRoiOpt = ret->createOptionalParameter(7, "-cerebellum-roi", "vertices to use from cerebellum");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the cerebellum roi as a metric file");
    
    OptionalParameter* volRoiOpt = ret->createOptionalParameter(8, "-vol-roi", "voxels to use");
    volRoiOpt->addVolumeParameter(1, "roi-vol", "the roi volume file");
    
    ret->setHelpText(
        AString("Writes a modified version of <cifti-in>, where all brainordinates outside the specified roi(s) are removed from the file.  ") +
        CiftiXML::directionFromStringExplanation() + "  " +
        "If -cifti-roi is specified, no other -*-roi option may be specified.  " +
        "If not using -cifti-roi, any -*-roi options not present will discard the relevant structure, if present in the input file."
    );
    return ret;
}

void AlgorithmCiftiRestrictDenseMap::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    const CiftiFile* ciftiIn = myParams->getCifti(1);
    int direction = CiftiXML::directionFromString(myParams->getString(2));
    CiftiFile* ciftiOut = myParams->getOutputCifti(3);
    OptionalParameter* ciftiRoiOpt = myParams->getOptionalParameter(4);
    OptionalParameter* leftRoiOpt = myParams->getOptionalParameter(5);
    OptionalParameter* rightRoiOpt = myParams->getOptionalParameter(6);
    OptionalParameter* cerebRoiOpt = myParams->getOptionalParameter(7);
    OptionalParameter* volRoiOpt = myParams->getOptionalParameter(8);
    if (ciftiRoiOpt->m_present)
    {
        if (leftRoiOpt->m_present || rightRoiOpt->m_present || cerebRoiOpt->m_present || volRoiOpt->m_present)
        {
            throw AlgorithmException("-cifti-roi cannot be specified with any other roi option");
        }
        const CiftiFile* ciftiRoi = ciftiRoiOpt->getCifti(1);
        AlgorithmCiftiRestrictDenseMap(myProgObj, ciftiIn, direction, ciftiOut, ciftiRoi);
    } else {
        if (!(leftRoiOpt->m_present || rightRoiOpt->m_present || cerebRoiOpt->m_present || volRoiOpt->m_present))
        {
            throw AlgorithmException("you must specify at least one roi option");
        }
        const MetricFile* leftRoi = NULL, *rightRoi = NULL, *cerebRoi = NULL;
        const VolumeFile* volRoi = NULL;
        if (leftRoiOpt->m_present) leftRoi = leftRoiOpt->getMetric(1);
        if (rightRoiOpt->m_present) rightRoi = rightRoiOpt->getMetric(1);
        if (cerebRoiOpt->m_present) cerebRoi = cerebRoiOpt->getMetric(1);
        if (volRoiOpt->m_present) volRoi = volRoiOpt->getVolume(1);
        AlgorithmCiftiRestrictDenseMap(myProgObj, ciftiIn, direction, ciftiOut, leftRoi, rightRoi, cerebRoi, volRoi);
    }
}

AlgorithmCiftiRestrictDenseMap::AlgorithmCiftiRestrictDenseMap(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const int& direction, CiftiFile* ciftiOut, const CiftiFile* ciftiRoi) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CaretAssert(direction >= 0);
    const CiftiXML& inXML = ciftiIn->getCiftiXML(), &roiXML = ciftiRoi->getCiftiXML();
    if (direction >= inXML.getNumberOfDimensions())
    {
        throw AlgorithmException("invalid direction for input file");
    }
    if (inXML.getMappingType(direction) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("input does not have a brain models mapping along specified direction");
    }
    if (roiXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("roi does not have a brain models mapping along column");
    }
    const CiftiBrainModelsMap& inMap = inXML.getBrainModelsMap(direction);
    if (!inMap.approximateMatch(roiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN)))
    {//for now, require that the brain models mappings use the same vertices/voxels, but allow for unusual surface structures not available in the metric/volume version
        throw AlgorithmException("roi cifti brain models mapping doesn't match the input cifti");
    }//alternatively, could separate and call the other version if they don't match, but could be more confusing
    vector<float> roiData(roiXML.getDimensionLength(CiftiXML::ALONG_COLUMN));
    ciftiRoi->getColumn(roiData.data(), 0);//we only need 1 column from the roi
    CiftiBrainModelsMap outMap;
    if (inMap.hasVolumeData())
    {
        outMap.setVolumeSpace(inMap.getVolumeSpace());
    }
    vector<int64_t> translate;
    vector<CiftiBrainModelsMap::ModelInfo> modelInfo = inMap.getModelInfo();
    for (int i = 0; i < (int)modelInfo.size(); ++i)
    {
        switch (modelInfo[i].m_type)
        {
            case CiftiBrainModelsMap::SURFACE:
            {
                vector<CiftiBrainModelsMap::SurfaceMap> surfMap = inMap.getSurfaceMap(modelInfo[i].m_structure);
                vector<int64_t> usedNodes;
                for (size_t j = 0; j < surfMap.size(); ++j)
                {
                    if (roiData[surfMap[j].m_ciftiIndex] > 0.0f)
                    {
                        usedNodes.push_back(surfMap[j].m_surfaceNode);
                        translate.push_back(surfMap[j].m_ciftiIndex);
                    }
                }
                if (!usedNodes.empty())
                {
                    outMap.addSurfaceModel(inMap.getSurfaceNumberOfNodes(modelInfo[i].m_structure), modelInfo[i].m_structure, usedNodes);
                }
                break;
            }
            case CiftiBrainModelsMap::VOXELS:
            {
                vector<CiftiBrainModelsMap::VolumeMap> volMap = inMap.getVolumeStructureMap(modelInfo[i].m_structure);
                vector<int64_t> usedVoxels;
                for (size_t j = 0; j < volMap.size(); ++j)
                {
                    if (roiData[volMap[j].m_ciftiIndex] > 0.0f)
                    {
                        usedVoxels.push_back(volMap[j].m_ijk[0]);
                        usedVoxels.push_back(volMap[j].m_ijk[1]);
                        usedVoxels.push_back(volMap[j].m_ijk[2]);
                        translate.push_back(volMap[j].m_ciftiIndex);
                    }
                }
                if (!usedVoxels.empty())
                {
                    outMap.addVolumeModel(modelInfo[i].m_structure, usedVoxels);
                }
                break;
            }
        }
    }
    if (outMap.getLength() == 0)
    {
        throw AlgorithmException("output mapping would contain no brainordinates");
    }
    CiftiXML outXML = inXML;
    outXML.setMap(direction, outMap);
    ciftiOut->setCiftiXML(outXML);
    vector<int64_t> outDims = outXML.getDimensions();
    vector<float> scratchRow(inXML.getDimensionLength(CiftiXML::ALONG_ROW));
    if (direction == CiftiXML::ALONG_ROW)
    {
        vector<float> outRow(outMap.getLength());
        for (MultiDimIterator<int64_t> iter(vector<int64_t>(outDims.begin() + 1, outDims.end())); !iter.atEnd(); ++iter)
        {
            ciftiIn->getRow(scratchRow.data(), *iter);
            for (size_t i = 0; i < translate.size(); ++i)
            {
                outRow[i] = scratchRow[translate[i]];
            }
            ciftiOut->setRow(outRow.data(), *iter);
        }
    } else {
        for (MultiDimIterator<int64_t> iter(vector<int64_t>(outDims.begin() + 1, outDims.end())); !iter.atEnd(); ++iter)
        {
            vector<int64_t> indices = *iter;
            indices[direction - 1] = translate[(*iter)[direction - 1]];
            ciftiIn->getRow(scratchRow.data(), indices);
            ciftiOut->setRow(scratchRow.data(), *iter);
        }
    }
}

AlgorithmCiftiRestrictDenseMap::AlgorithmCiftiRestrictDenseMap(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const int& direction, CiftiFile* ciftiOut,
                                                               const MetricFile* leftRoi, const MetricFile* rightRoi, const MetricFile* cerebRoi, const VolumeFile* volRoi) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CaretAssert(direction >= 0);
    const CiftiXML& inXML = ciftiIn->getCiftiXML();
    if (direction >= inXML.getNumberOfDimensions())
    {
        throw AlgorithmException("invalid direction for input file");
    }
    if (inXML.getMappingType(direction) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("input does not have a brain models mapping along specified direction");
    }
    const CiftiBrainModelsMap& inMap = inXML.getBrainModelsMap(direction);
    if (leftRoi != NULL)
    {
        if (!inMap.hasSurfaceData(StructureEnum::CORTEX_LEFT))
        {
            throw AlgorithmException("left surface roi provided, but input has no left surface data");
        }
        if (leftRoi->getNumberOfNodes() != inMap.getSurfaceNumberOfNodes(StructureEnum::CORTEX_LEFT))
        {
            throw AlgorithmException("left surface roi has different number of vertices than input cifti");
        }
    }
    if (rightRoi != NULL)
    {
        if (!inMap.hasSurfaceData(StructureEnum::CORTEX_RIGHT))
        {
            throw AlgorithmException("right surface roi provided, but input has no left surface data");
        }
        if (rightRoi->getNumberOfNodes() != inMap.getSurfaceNumberOfNodes(StructureEnum::CORTEX_RIGHT))
        {
            throw AlgorithmException("right surface roi has different number of vertices than input cifti");
        }
    }
    if (cerebRoi != NULL)
    {
        if (!inMap.hasSurfaceData(StructureEnum::CEREBELLUM))
        {
            throw AlgorithmException("cerebellum surface roi provided, but input has no left surface data");
        }
        if (cerebRoi->getNumberOfNodes() != inMap.getSurfaceNumberOfNodes(StructureEnum::CEREBELLUM))
        {
            throw AlgorithmException("cerebellum surface roi has different number of vertices than input cifti");
        }
    }
    if (volRoi != NULL)
    {
        if (!inMap.hasVolumeData()) throw AlgorithmException("volume roi provided, but input has no volume data");
        if (!inMap.getVolumeSpace().matches(volRoi->getVolumeSpace())) throw AlgorithmException("roi volume has different volume space than input cifti");
    }
    CiftiBrainModelsMap outMap;
    if (inMap.hasVolumeData() && volRoi != NULL)
    {
        outMap.setVolumeSpace(inMap.getVolumeSpace());
    }
    vector<int64_t> translate;
    vector<CiftiBrainModelsMap::ModelInfo> modelInfo = inMap.getModelInfo();
    for (int i = 0; i < (int)modelInfo.size(); ++i)
    {
        switch (modelInfo[i].m_type)
        {
            case CiftiBrainModelsMap::SURFACE:
            {
                const MetricFile* useRoi = NULL;
                switch (modelInfo[i].m_structure)
                {
                    case StructureEnum::CORTEX_LEFT:
                        useRoi = leftRoi;
                        break;
                    case StructureEnum::CORTEX_RIGHT:
                        useRoi = rightRoi;
                        break;
                    case StructureEnum::CEREBELLUM:
                        useRoi = cerebRoi;
                        break;
                    default:
                        CaretLogWarning("removing unsupported surface structure: '" + StructureEnum::toName(modelInfo[i].m_structure) + "'");
                        break;//remove other surface structures, but warn
                }
                if (useRoi != NULL)
                {
                    const float* roiData = useRoi->getValuePointerForColumn(0);
                    vector<CiftiBrainModelsMap::SurfaceMap> surfMap = inMap.getSurfaceMap(modelInfo[i].m_structure);
                    vector<int64_t> usedNodes;
                    for (size_t j = 0; j < surfMap.size(); ++j)
                    {
                        if (roiData[surfMap[j].m_surfaceNode] > 0.0f)
                        {
                            usedNodes.push_back(surfMap[j].m_surfaceNode);
                            translate.push_back(surfMap[j].m_ciftiIndex);
                        }
                    }
                    if (!usedNodes.empty())
                    {
                        outMap.addSurfaceModel(inMap.getSurfaceNumberOfNodes(modelInfo[i].m_structure), modelInfo[i].m_structure, usedNodes);
                    }
                }
                break;
            }
            case CiftiBrainModelsMap::VOXELS:
            {
                if (volRoi != NULL)
                {
                    vector<CiftiBrainModelsMap::VolumeMap> volMap = inMap.getVolumeStructureMap(modelInfo[i].m_structure);
                    vector<int64_t> usedVoxels;
                    for (size_t j = 0; j < volMap.size(); ++j)
                    {
                        if (volRoi->getValue(volMap[j].m_ijk) > 0.0f)
                        {
                            usedVoxels.push_back(volMap[j].m_ijk[0]);
                            usedVoxels.push_back(volMap[j].m_ijk[1]);
                            usedVoxels.push_back(volMap[j].m_ijk[2]);
                            translate.push_back(volMap[j].m_ciftiIndex);
                        }
                    }
                    if (!usedVoxels.empty())
                    {
                        outMap.addVolumeModel(modelInfo[i].m_structure, usedVoxels);
                    }
                }
                break;
            }
        }
    }
    if (outMap.getLength() == 0)
    {
        throw AlgorithmException("output mapping would contain no brainordinates");
    }
    CiftiXML outXML = inXML;
    outXML.setMap(direction, outMap);
    ciftiOut->setCiftiXML(outXML);
    vector<int64_t> outDims = outXML.getDimensions();
    vector<float> scratchRow(inXML.getDimensionLength(CiftiXML::ALONG_ROW));
    if (direction == CiftiXML::ALONG_ROW)
    {
        vector<float> outRow(outMap.getLength());
        for (MultiDimIterator<int64_t> iter(vector<int64_t>(outDims.begin() + 1, outDims.end())); !iter.atEnd(); ++iter)
        {
            ciftiIn->getRow(scratchRow.data(), *iter);
            for (size_t i = 0; i < translate.size(); ++i)
            {
                outRow[i] = scratchRow[translate[i]];
            }
            ciftiOut->setRow(outRow.data(), *iter);
        }
    } else {
        for (MultiDimIterator<int64_t> iter(vector<int64_t>(outDims.begin() + 1, outDims.end())); !iter.atEnd(); ++iter)
        {
            vector<int64_t> indices = *iter;
            indices[direction - 1] = translate[(*iter)[direction - 1]];
            ciftiIn->getRow(scratchRow.data(), indices);
            ciftiOut->setRow(scratchRow.data(), *iter);
        }
    }
}

float AlgorithmCiftiRestrictDenseMap::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiRestrictDenseMap::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
