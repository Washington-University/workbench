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

#include "AlgorithmCiftiAverageDenseROI.h"
#include "AlgorithmException.h"
#include "CaretLogger.h"
#include "CiftiFile.h"
#include "FileInformation.h"
#include "SurfaceFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"

#include <fstream>
#include <string>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmCiftiAverageDenseROI::getCommandSwitch()
{
    return "-cifti-average-dense-roi";
}

AString AlgorithmCiftiAverageDenseROI::getShortDescription()
{
    return "AVERAGE CIFTI ROWS ACROSS SUBJECTS BY ROI";
}

OperationParameters* AlgorithmCiftiAverageDenseROI::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiOutputParameter(1, "cifti-out", "output cifti file");
    
    OptionalParameter* ciftiRoiOpt = ret->createOptionalParameter(2, "-cifti-roi", "cifti file containing combined weights");
    ciftiRoiOpt->addCiftiParameter(1, "roi-cifti", "the roi cifti file");
    ciftiRoiOpt->createOptionalParameter(2, "-in-memory", "cache the roi in memory so that it isn't re-read for each input cifti");
    
    OptionalParameter* leftRoiOpt = ret->createOptionalParameter(3, "-left-roi", "weights to use for left hempsphere");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the left roi as a metric file");
    
    OptionalParameter* rightRoiOpt = ret->createOptionalParameter(4, "-right-roi", "weights to use for right hempsphere");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the right roi as a metric file");
    
    OptionalParameter* cerebRoiOpt = ret->createOptionalParameter(5, "-cerebellum-roi", "weights to use for cerebellum surface");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the cerebellum roi as a metric file");
    
    OptionalParameter* volRoiOpt = ret->createOptionalParameter(6, "-vol-roi", "voxel weights to use");
    volRoiOpt->addVolumeParameter(1, "roi-vol", "the roi volume file");
    
    OptionalParameter* leftAreaSurfOpt = ret->createOptionalParameter(7, "-left-area-surf", "specify the left surface for vertex area correction");
    leftAreaSurfOpt->addSurfaceParameter(1, "left-surf", "the left surface file");
    
    OptionalParameter* rightAreaSurfOpt = ret->createOptionalParameter(8, "-right-area-surf", "specify the right surface for vertex area correction");
    rightAreaSurfOpt->addSurfaceParameter(1, "right-surf", "the right surface file");
    
    OptionalParameter* cerebAreaSurfOpt = ret->createOptionalParameter(9, "-cerebellum-area-surf", "specify the cerebellum surface for vertex area correction");
    cerebAreaSurfOpt->addSurfaceParameter(1, "cerebellum-surf", "the cerebellum surface file");
    
    ParameterComponent* ciftiOpt = ret->createRepeatableParameter(10, "-cifti", "specify an input cifti file");
    ciftiOpt->addCiftiParameter(1, "cifti-in", "a cifti file to average across");
    
    ret->setHelpText(
        AString("Averages rows for each map of the ROI(s), across all files.  ") +
        "ROI maps are treated as weighting functions, including negative values.  " +
        "For efficiency, ensure that everything that is not intended to be used is zero in the ROI map.  " +
        "If -cifti-roi is specified, -left-roi, -right-roi, -cerebellum-roi, and -vol-roi must not be specified.  " +
        "If multiple non-cifti ROI files are specified, they must have the same number of columns."
    );
    return ret;
}

void AlgorithmCiftiAverageDenseROI::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* ciftiOut = myParams->getOutputCifti(1);
    CiftiFile* ciftiROI = NULL;
    OptionalParameter* ciftiRoiOpt = myParams->getOptionalParameter(2);
    if (ciftiRoiOpt->m_present)
    {
        ciftiROI = ciftiRoiOpt->getCifti(1);
        if (ciftiRoiOpt->getOptionalParameter(2)->m_present) ciftiROI->convertToInMemory();
    }
    MetricFile* leftROI = NULL;
    OptionalParameter* leftRoiOpt = myParams->getOptionalParameter(3);
    if (leftRoiOpt->m_present)
    {
        if (ciftiROI != NULL) throw AlgorithmException("-cifti-roi cannot be used with any other ROI option");
        leftROI = leftRoiOpt->getMetric(1);
    }
    MetricFile* rightROI = NULL;
    OptionalParameter* rightRoiOpt = myParams->getOptionalParameter(4);
    if (rightRoiOpt->m_present)
    {
        if (ciftiROI != NULL) throw AlgorithmException("-cifti-roi cannot be used with any other ROI option");
        rightROI = rightRoiOpt->getMetric(1);
    }
    MetricFile* cerebROI = NULL;
    OptionalParameter* cerebRoiOpt = myParams->getOptionalParameter(5);
    if (cerebRoiOpt->m_present)
    {
        if (ciftiROI != NULL) throw AlgorithmException("-cifti-roi cannot be used with any other ROI option");
        cerebROI = cerebRoiOpt->getMetric(1);
    }
    VolumeFile* volROI = NULL;
    OptionalParameter* volRoiOpt = myParams->getOptionalParameter(6);
    if (volRoiOpt->m_present)
    {
        if (ciftiROI != NULL) throw AlgorithmException("-cifti-roi cannot be used with any other ROI option");
        volROI = volRoiOpt->getVolume(1);
    }
    SurfaceFile* leftAreaSurf = NULL;
    OptionalParameter* leftAreaSurfOpt = myParams->getOptionalParameter(7);
    if (leftAreaSurfOpt->m_present)
    {
        leftAreaSurf = leftAreaSurfOpt->getSurface(1);
    }
    SurfaceFile* rightAreaSurf = NULL;
    OptionalParameter* rightAreaSurfOpt = myParams->getOptionalParameter(8);
    if (rightAreaSurfOpt->m_present)
    {
        rightAreaSurf = rightAreaSurfOpt->getSurface(1);
    }
    SurfaceFile* cerebAreaSurf = NULL;
    OptionalParameter* cerebAreaSurfOpt = myParams->getOptionalParameter(9);
    if (cerebAreaSurfOpt->m_present)
    {
        cerebAreaSurf = cerebAreaSurfOpt->getSurface(1);
    }
    vector<const CiftiFile*> ciftiList;
    const vector<ParameterComponent*>& ciftiInstances = myParams->getRepeatableParameterInstances(10);
    for (int i = 0; i < (int)ciftiInstances.size(); ++i)
    {
        ciftiList.push_back(ciftiInstances[i]->getCifti(1));
    }
    if (ciftiROI != NULL)
    {
        AlgorithmCiftiAverageDenseROI(myProgObj, ciftiList, ciftiOut, ciftiROI, leftAreaSurf, rightAreaSurf, cerebAreaSurf);
    } else {
        AlgorithmCiftiAverageDenseROI(myProgObj, ciftiList, ciftiOut, leftROI, rightROI, cerebROI, volROI, leftAreaSurf, rightAreaSurf, cerebAreaSurf);
    }
}

AlgorithmCiftiAverageDenseROI::AlgorithmCiftiAverageDenseROI(ProgressObject* myProgObj, const vector<const CiftiFile*>& ciftiList, CiftiFile* ciftiOut,
                                                             const MetricFile* leftROI, const MetricFile* rightROI, const MetricFile* cerebROI, const VolumeFile* volROI,
                                                             const SurfaceFile* leftAreaSurf, const SurfaceFile* rightAreaSurf, const SurfaceFile* cerebAreaSurf) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(ciftiOut != NULL);
    LevelProgress myProgress(myProgObj);
    int numCifti = (int)ciftiList.size();
    if (numCifti < 1) throw AlgorithmException("no cifti files specified to average");
    const CiftiXML& baseXML = ciftiList[0]->getCiftiXML();
    if (baseXML.getNumberOfDimensions() != 2) throw AlgorithmException("this operation currently only supports 2D cifti");
    if (baseXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) throw AlgorithmException("inputs must have brain models along columns");
    int rowSize = baseXML.getDimensionLength(CiftiXML::ALONG_ROW);
    bool first = true;
    const CaretMappableDataFile* nameFile = NULL;
    int numMaps = -1;
    vector<float> leftAreaData, rightAreaData, cerebAreaData;
    float* leftAreaPointer = NULL, *rightAreaPointer = NULL, *cerebAreaPointer = NULL;
    if (leftROI != NULL)
    {
        if (leftAreaSurf != NULL)
        {
            if (leftROI->getNumberOfNodes() != leftAreaSurf->getNumberOfNodes()) throw AlgorithmException("left area surface and left roi have different number of nodes");
            leftAreaSurf->computeNodeAreas(leftAreaData);
            leftAreaPointer = leftAreaData.data();
        }
        first = false;
        numMaps = leftROI->getNumberOfMaps();
        nameFile = leftROI;
    }
    if (rightROI != NULL)
    {
        if (rightAreaSurf != NULL)
        {
            if (rightROI->getNumberOfNodes() != rightAreaSurf->getNumberOfNodes()) throw AlgorithmException("right area surface and right roi have different number of nodes");
            rightAreaSurf->computeNodeAreas(rightAreaData);
            rightAreaPointer = rightAreaData.data();
        }
        if (first)
        {
            first = false;
            numMaps = rightROI->getNumberOfMaps();
            nameFile = rightROI;
        } else {
            if (rightROI->getNumberOfMaps() != numMaps) throw AlgorithmException("right roi has a different number of maps");
        }
    }
    if (cerebROI != NULL)
    {
        if (cerebAreaSurf != NULL)
        {
            if (cerebROI->getNumberOfNodes() != cerebAreaSurf->getNumberOfNodes()) throw AlgorithmException("cerebellum area surface and cerebellum roi have different number of nodes");
            cerebAreaSurf->computeNodeAreas(cerebAreaData);
            cerebAreaPointer = cerebAreaData.data();
        }
        if (first)
        {
            first = false;
            numMaps = cerebROI->getNumberOfMaps();
            nameFile = cerebROI;
        } else {
            if (cerebROI->getNumberOfMaps() != numMaps) throw AlgorithmException("cerebellum roi has a different number of maps");
        }
    }
    if (volROI != NULL)
    {
        if (first)
        {
            first = false;
            numMaps = volROI->getNumberOfMaps();
            nameFile = volROI;
        } else {
            if (volROI->getNumberOfMaps() != numMaps) throw AlgorithmException("volume roi has a different number of maps");
        }
    }
    if (first) throw AlgorithmException("no roi files provided");
    for (int i = 0; i < numCifti; ++i)
    {
        const CiftiXML& thisXML = ciftiList[i]->getCiftiXML();
        if (!thisXML.approximateMatch(baseXML)) throw AlgorithmException("cifti files do not match between #1 and #" + AString::number(i + 1));
    }
    if (leftROI != NULL)
    {
        verifySurfaceComponent(ciftiList[0], StructureEnum::CORTEX_LEFT, leftROI);
    }
    if (rightROI != NULL)
    {
        verifySurfaceComponent(ciftiList[0], StructureEnum::CORTEX_RIGHT, rightROI);
    }
    if (cerebROI != NULL)
    {
        verifySurfaceComponent(ciftiList[0], StructureEnum::CEREBELLUM, cerebROI);
    }
    if (volROI != NULL)
    {
        verifyVolumeComponent(ciftiList[0], volROI);
    }
    vector<vector<double> > accum(numMaps, vector<double>(rowSize, 0.0));
    vector<double> denom(numMaps, 0.0);
    for (int i = 0; i < numCifti; ++i)
    {
        if (leftROI != NULL)
        {
            processSurfaceComponent(accum, denom, ciftiList[i], StructureEnum::CORTEX_LEFT, leftROI, leftAreaPointer);
        }
        if (rightROI != NULL)
        {
            processSurfaceComponent(accum, denom, ciftiList[i], StructureEnum::CORTEX_RIGHT, rightROI, rightAreaPointer);
        }
        if (cerebROI != NULL)
        {
            processSurfaceComponent(accum, denom, ciftiList[i], StructureEnum::CEREBELLUM, cerebROI, cerebAreaPointer);
        }
        if (volROI != NULL)
        {
            processVolumeComponent(accum, denom, ciftiList[i], volROI);
        }
    }
    CiftiXML newXml;
    newXml.setNumberOfDimensions(2);
    newXml.setMap(CiftiXML::ALONG_COLUMN, *(baseXML.getMap(CiftiXML::ALONG_ROW)));
    CiftiScalarsMap rowMap;
    rowMap.setLength(numMaps);
    for (int i = 0; i < numMaps; ++i)
    {
        rowMap.setMapName(i, nameFile->getMapName(i));
    }
    newXml.setMap(CiftiXML::ALONG_ROW, rowMap);
    ciftiOut->setCiftiXML(newXml);
    vector<float> outRow(numMaps);
    for (int j = 0; j < numMaps; ++j)
    {
        if (denom[j] == 0.0) throw AlgorithmException("no data matched one of the ROI(s)");
    }
    for (int i = 0; i < rowSize; ++i)
    {
        for (int j = 0; j < numMaps; ++j)
        {
            outRow[j] = accum[j][i] / denom[j];
        }
        ciftiOut->setRow(outRow.data(), i);
    }
}

AlgorithmCiftiAverageDenseROI::AlgorithmCiftiAverageDenseROI(ProgressObject* myProgObj, const vector<const CiftiFile*>& ciftiList, CiftiFile* ciftiOut, const CiftiFile* ciftiROI,
                                                             const SurfaceFile* leftAreaSurf, const SurfaceFile* rightAreaSurf, const SurfaceFile* cerebAreaSurf): AbstractAlgorithm(myProgObj)
{
    CaretAssert(ciftiOut != NULL);
    LevelProgress myProgress(myProgObj);
    int numCifti = (int)ciftiList.size();
    if (numCifti < 1) throw AlgorithmException("no cifti files specified to average");
    const CiftiXML& baseXML = ciftiList[0]->getCiftiXML(), &roiXML = ciftiROI->getCiftiXML();
    if (baseXML.getNumberOfDimensions() != 2 || roiXML.getNumberOfDimensions() != 2) throw AlgorithmException("this operation currently only supports 2D cifti");
    if (baseXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) throw AlgorithmException("inputs must have brain models along columns");
    const CiftiBrainModelsMap& baseBrainModels = baseXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if (baseBrainModels != *(roiXML.getMap(CiftiXML::ALONG_COLUMN))) throw AlgorithmException("cifti roi mapping along columns doesn't match input cifti");
    int rowSize = baseXML.getDimensionLength(CiftiXML::ALONG_ROW);
    vector<float> leftAreaData, rightAreaData, cerebAreaData;
    float* leftAreaPointer = NULL, *rightAreaPointer = NULL, *cerebAreaPointer = NULL;
    if (leftAreaSurf != NULL)
    {
        if (baseBrainModels.getSurfaceNumberOfNodes(StructureEnum::CORTEX_LEFT) != leftAreaSurf->getNumberOfNodes())
        {
            throw AlgorithmException("left area surface and left cortex cifti structure have different number of nodes");
        }
        leftAreaSurf->computeNodeAreas(leftAreaData);
        leftAreaPointer = leftAreaData.data();
    }
    if (rightAreaSurf != NULL)
    {
        if (baseBrainModels.getSurfaceNumberOfNodes(StructureEnum::CORTEX_RIGHT) != rightAreaSurf->getNumberOfNodes())
        {
            throw AlgorithmException("right area surface and right cortex cifti structure have different number of nodes");
        }
        rightAreaSurf->computeNodeAreas(rightAreaData);
        rightAreaPointer = rightAreaData.data();
    }
    if (cerebAreaSurf != NULL)
    {
        if (baseBrainModels.getSurfaceNumberOfNodes(StructureEnum::CEREBELLUM) != cerebAreaSurf->getNumberOfNodes())
        {
            throw AlgorithmException("cerebellum area surface and cerebellum cortex cifti structure have different number of nodes");
        }
        cerebAreaSurf->computeNodeAreas(cerebAreaData);
        cerebAreaPointer = cerebAreaData.data();
    }
    for (int i = 1; i < numCifti; ++i)
    {
        const CiftiXML& thisXML = ciftiList[i]->getCiftiXML();
        if (!thisXML.approximateMatch(baseXML)) throw AlgorithmException("cifti files do not match between '" + ciftiList[0]->getFileName() + "' and '" + ciftiList[i]->getFileName() + "'");
    }
    int numMaps = roiXML.getDimensionLength(CiftiXML::ALONG_ROW);
    vector<vector<double> > accum(numMaps, vector<double>(rowSize, 0.0));
    vector<double> denom(numMaps, 0.0);
    for (int i = 0; i < numCifti; ++i)
    {
        processCifti(accum, denom, ciftiList[i], ciftiROI, leftAreaPointer, rightAreaPointer, cerebAreaPointer);
    }
    CiftiXML newXml;
    newXml.setNumberOfDimensions(2);
    newXml.setMap(CiftiXML::ALONG_COLUMN, *(baseXML.getMap(CiftiXML::ALONG_ROW)));
    CiftiScalarsMap rowMap;
    rowMap.setLength(numMaps);
    const CiftiMappingType& nameMap = *(roiXML.getMap(CiftiXML::ALONG_ROW));
    for (int i = 0; i < numMaps; ++i)
    {
        rowMap.setMapName(i, nameMap.getIndexName(i));
    }
    newXml.setMap(CiftiXML::ALONG_ROW, rowMap);
    ciftiOut->setCiftiXML(newXml);
    vector<float> outRow(numMaps);
    for (int j = 0; j < numMaps; ++j)
    {
        if (denom[j] == 0.0) throw AlgorithmException("no data matched one of the ROI(s)");
    }
    for (int i = 0; i < rowSize; ++i)
    {
        for (int j = 0; j < numMaps; ++j)
        {
            outRow[j] = accum[j][i] / denom[j];
        }
        ciftiOut->setRow(outRow.data(), i);
    }
}

void AlgorithmCiftiAverageDenseROI::verifySurfaceComponent(const CiftiFile* myCifti, const StructureEnum::Enum& myStruct, const MetricFile* myRoi)
{
    const CiftiXML& myXml = myCifti->getCiftiXML();
    CaretAssert(myXml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS);//should be checked in the algorithm constructor
    const CiftiBrainModelsMap& brainModelsMap = myXml.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if (!brainModelsMap.hasSurfaceData(myStruct))
    {
        CaretLogWarning("cifti files are missing structure " + StructureEnum::toName(myStruct));
        return;
    }
    if (myRoi->getNumberOfNodes() != brainModelsMap.getSurfaceNumberOfNodes(myStruct))
    {
        throw AlgorithmException("cifti number of vertices does not match roi for structure " + StructureEnum::toName(myStruct));
    }
}

void AlgorithmCiftiAverageDenseROI::processSurfaceComponent(vector<vector<double> >& accum, vector<double>& denom, const CiftiFile* myCifti, const StructureEnum::Enum& myStruct, const MetricFile* myRoi, const float* myAreas)
{
    const CiftiXML& myXml = myCifti->getCiftiXML();
    CaretAssert(myXml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS);//should be checked in the algorithm constructor
    const CiftiBrainModelsMap& brainModelsMap = myXml.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if (!brainModelsMap.hasSurfaceData(myStruct))
    {
        return;
    }
    if (myRoi->getNumberOfNodes() != brainModelsMap.getSurfaceNumberOfNodes(myStruct)) throw AlgorithmException("cifti number of vertices does not match roi");
    int rowSize = myXml.getDimensionLength(CiftiXML::ALONG_ROW);
    vector<float> rowScratch(rowSize);
    CaretAssert(rowScratch.size() == accum[0].size());
    vector<CiftiBrainModelsMap::SurfaceMap> myMap = brainModelsMap.getSurfaceMap(myStruct);
    int mapSize = (int)myMap.size();
    int numMaps = myRoi->getNumberOfMaps();
    if (myAreas == NULL)
    {
        for (int i = 0; i < mapSize; ++i)
        {
            const int& myNode = myMap[i].m_surfaceNode;
            bool dataLoaded = false;
            for (int m = 0; m < numMaps; ++m)
            {
                const float roiVal = myRoi->getValue(myNode, m);
                if (roiVal != 0.0f)
                {
                    if (!dataLoaded)
                    {
                        myCifti->getRow(rowScratch.data(), myMap[i].m_ciftiIndex);
                        dataLoaded = true;
                    }
                    for (int j = 0; j < rowSize; ++j)
                    {
                        accum[m][j] += rowScratch[j] * roiVal;
                    }
                    denom[m] += roiVal;
                }
            }
        }
    } else {
        for (int i = 0; i < mapSize; ++i)
        {
            const int& myNode = myMap[i].m_surfaceNode;
            bool dataLoaded = false;
            for (int m = 0; m < numMaps; ++m)
            {
                const float& roiVal = myRoi->getValue(myNode, m);
                if (roiVal != 0.0f)
                {
                    const float weight = roiVal * myAreas[myMap[i].m_surfaceNode];
                    if (!dataLoaded)
                    {
                        myCifti->getRow(rowScratch.data(), myMap[i].m_ciftiIndex);
                        dataLoaded = true;
                    }
                    for (int j = 0; j < rowSize; ++j)
                    {
                        accum[m][j] += rowScratch[j] * weight;
                    }
                    denom[m] += weight;
                }
            }
        }
    }
}

void AlgorithmCiftiAverageDenseROI::verifyVolumeComponent(const CiftiFile* myCifti, const VolumeFile* volROI)
{
    const CiftiXML& myXml = myCifti->getCiftiXML();
    CaretAssert(myXml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS);//should be checked in the algorithm constructor
    const CiftiBrainModelsMap& brainModelsMap = myXml.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if (!volROI->matchesVolumeSpace(brainModelsMap.getVolumeSpace())) throw AlgorithmException("cifti files don't match the ROI volume's space");
}

void AlgorithmCiftiAverageDenseROI::processVolumeComponent(vector<vector<double> >& accum, vector<double>& denom, const CiftiFile* myCifti, const VolumeFile* volROI)
{
    const CiftiXML& myXml = myCifti->getCiftiXML();
    CaretAssert(myXml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS);//should be checked in the algorithm constructor
    const CiftiBrainModelsMap& brainModelsMap = myXml.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if (!volROI->matchesVolumeSpace(brainModelsMap.getVolumeSpace())) throw AlgorithmException("cifti files don't match the ROI volume's space");
    int rowSize = myXml.getDimensionLength(CiftiXML::ALONG_ROW);
    vector<float> rowScratch(rowSize);
    CaretAssert(rowScratch.size() == accum[0].size());
    vector<CiftiBrainModelsMap::VolumeMap> myMap = brainModelsMap.getFullVolumeMap();
    int mapSize = (int)myMap.size();
    int numMaps = volROI->getNumberOfMaps();
    for (int i = 0; i < mapSize; ++i)
    {
        bool dataLoaded = false;
        if (!volROI->indexValid(myMap[i].m_ijk)) throw AlgorithmException("cifti file lists invalid voxels");
        for (int m = 0; m < numMaps; ++m)
        {
            const float& roiVal = volROI->getValue(myMap[i].m_ijk, m);
            if (roiVal != 0.0f)
            {
                if (!dataLoaded)
                {
                    myCifti->getRow(rowScratch.data(), myMap[i].m_ciftiIndex);
                    dataLoaded = true;
                }
                for (int j = 0; j < rowSize; ++j)
                {
                    accum[m][j] += rowScratch[j] * roiVal;
                }
                denom[m] += roiVal;
            }
        }
    }
}

void AlgorithmCiftiAverageDenseROI::processCifti(vector<vector<double> >& accum, vector<double>& denom, const CiftiFile* myCifti, const CiftiFile* ciftiROI,
                                                 const float* leftAreas, const float* rightAreas, const float* cerebAreas)
{
    const CiftiXML& myXml = myCifti->getCiftiXML();//same along columns for data and roi, we already checked
    CaretAssert(myXml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS);//should be checked in the algorithm constructor
    const CiftiBrainModelsMap& brainModelsMap = myXml.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    int rowSize = myXml.getDimensionLength(CiftiXML::ALONG_ROW);
    vector<StructureEnum::Enum> surfList = brainModelsMap.getSurfaceStructureList();
    int numMaps = ciftiROI->getNumberOfColumns();
    vector<float> dataScratch(rowSize), roiScratch(numMaps);
    for (int s = 0; s < (int)surfList.size(); ++s)
    {
        const float* myAreas = NULL;
        switch (surfList[s])
        {
            case StructureEnum::CORTEX_LEFT:
                myAreas = leftAreas;
                break;
            case StructureEnum::CORTEX_RIGHT:
                myAreas = rightAreas;
                break;
            case StructureEnum::CEREBELLUM:
                myAreas = cerebAreas;
                break;
            default:
                throw AlgorithmException("found unexpected surface structure in cifti files: " + StructureEnum::toName(surfList[s]));
        }
        vector<CiftiBrainModelsMap::SurfaceMap> myMap = brainModelsMap.getSurfaceMap(surfList[s]);
        int mapSize = (int)myMap.size();
        if (myAreas != NULL)
        {
            for (int i = 0; i < mapSize; ++i)
            {
                ciftiROI->getRow(roiScratch.data(), myMap[i].m_ciftiIndex);
                const float& thisArea = myAreas[myMap[i].m_surfaceNode];
                bool dataLoaded = false;
                for (int m = 0; m < numMaps; ++m)//ROI maps, not cifti mapping
                {
                    const float& roiValue = roiScratch[m];
                    if (roiValue != 0.0f)
                    {
                        if (!dataLoaded)
                        {
                            myCifti->getRow(dataScratch.data(), i);
                            dataLoaded = true;
                        }
                        const float weight = roiValue * thisArea;
                        for (int j = 0; j < rowSize; ++j)
                        {
                            accum[m][j] += dataScratch[j] * weight;
                        }
                        denom[m] += weight;
                    }
                }
            }
        } else {
            for (int i = 0; i < mapSize; ++i)
            {
                ciftiROI->getRow(roiScratch.data(), myMap[i].m_ciftiIndex);
                bool dataLoaded = false;
                for (int m = 0; m < numMaps; ++m)//ROI maps, not cifti mapping
                {
                    const float& roiValue = roiScratch[m];
                    if (roiValue != 0.0f)
                    {
                        if (!dataLoaded)
                        {
                            myCifti->getRow(dataScratch.data(), i);
                            dataLoaded = true;
                        }
                        for (int j = 0; j < rowSize; ++j)
                        {
                            accum[m][j] += dataScratch[j] * roiValue;
                        }
                        denom[m] += roiValue;
                    }
                }
            }
        }
    }
    vector<CiftiBrainModelsMap::VolumeMap> myMap = brainModelsMap.getFullVolumeMap();//again, we know columns match between ROI and data
    int mapSize = (int)myMap.size();
    for (int i = 0; i < mapSize; ++i)
    {
        ciftiROI->getRow(roiScratch.data(), myMap[i].m_ciftiIndex);
        bool dataLoaded = false;
        for (int m = 0; m < numMaps; ++m)//ROI maps, not cifti mapping
        {
            const float& roiValue = roiScratch[m];
            if (roiValue != 0.0f)
            {
                if (!dataLoaded)
                {
                    myCifti->getRow(dataScratch.data(), i);
                    dataLoaded = true;
                }
                for (int j = 0; j < rowSize; ++j)
                {
                    accum[m][j] += dataScratch[j] * roiValue;
                }
                denom[m] += roiValue;
            }
        }
    }
}

float AlgorithmCiftiAverageDenseROI::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiAverageDenseROI::getSubAlgorithmWeight()
{
    return 0.0f;
}
