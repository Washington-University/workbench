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

#include "AlgorithmCiftiAverageDenseROI.h"
#include "AlgorithmException.h"
#include "CaretLogger.h"
#include "CiftiFile.h"
#include "FileInformation.h"
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
    
    ret->addCiftiOutputParameter(1, "cifti-out", "output cifti dscalar file");
    
    OptionalParameter* leftRoiOpt = ret->createOptionalParameter(2, "-left-roi", "vertices to use from left hempsphere");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the left roi as a metric file");
    
    OptionalParameter* rightRoiOpt = ret->createOptionalParameter(3, "-right-roi", "vertices to use from right hempsphere");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the right roi as a metric file");
    
    OptionalParameter* cerebRoiOpt = ret->createOptionalParameter(4, "-cerebellum-roi", "vertices to use from cerebellum");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the cerebellum roi as a metric file");
    
    OptionalParameter* volRoiOpt = ret->createOptionalParameter(5, "-vol-roi", "voxels to use");
    volRoiOpt->addVolumeParameter(1, "roi-vol", "the roi volume file");
    
    ParameterComponent* ciftiOpt = ret->createRepeatableParameter(6, "-cifti", "specify an input cifti file");
    ciftiOpt->addCiftiParameter(1, "cifti-in", "a cifti file to average across");
    
    ret->setHelpText(
        AString("Averages rows within the ROI(s), across all files, equal weight for each matching row.")
    );
    return ret;
}

void AlgorithmCiftiAverageDenseROI::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* ciftiOut = myParams->getOutputCifti(1);
    MetricFile* leftROI = NULL;
    OptionalParameter* leftRoiOpt = myParams->getOptionalParameter(2);
    if (leftRoiOpt->m_present)
    {
        leftROI = leftRoiOpt->getMetric(1);
    }
    MetricFile* rightROI = NULL;
    OptionalParameter* rightRoiOpt = myParams->getOptionalParameter(3);
    if (rightRoiOpt->m_present)
    {
        rightROI = rightRoiOpt->getMetric(1);
    }
    MetricFile* cerebROI = NULL;
    OptionalParameter* cerebRoiOpt = myParams->getOptionalParameter(4);
    if (cerebRoiOpt->m_present)
    {
        cerebROI = cerebRoiOpt->getMetric(1);
    }
    VolumeFile* volROI = NULL;
    OptionalParameter* volRoiOpt = myParams->getOptionalParameter(5);
    if (volRoiOpt->m_present)
    {
        volROI = volRoiOpt->getVolume(1);
    }
    vector<const CiftiInterface*> ciftiList;
    const vector<ParameterComponent*>& ciftiInstances = *(myParams->getRepeatableParameterInstances(6));
    for (int i = 0; i < (int)ciftiInstances.size(); ++i)
    {
        ciftiList.push_back(ciftiInstances[i]->getCifti(1));
    }
    AlgorithmCiftiAverageDenseROI(myProgObj, ciftiList, ciftiOut, leftROI, rightROI, cerebROI, volROI);
}

AlgorithmCiftiAverageDenseROI::AlgorithmCiftiAverageDenseROI(ProgressObject* myProgObj, const vector<const CiftiInterface*>& ciftiList, CiftiFile* ciftiOut,
                                                             const MetricFile* leftROI, const MetricFile* rightROI, const MetricFile* cerebROI, const VolumeFile* volROI) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(ciftiOut != NULL);
    LevelProgress myProgress(myProgObj);
    int numCifti = (int)ciftiList.size();
    if (numCifti < 1) throw AlgorithmException("no cifti files specified to average");
    const CiftiXML& baseXML = ciftiList[0]->getCiftiXML();
    int rowSize = baseXML.getNumberOfColumns();
    vector<double> accum(rowSize, 0.0);
    int64_t accumCount = 0;
    for (int i = 0; i < numCifti; ++i)
    {
        const CiftiXML& thisXML = ciftiList[i]->getCiftiXML();
        if (!thisXML.matchesForRows(baseXML)) throw AlgorithmException("cifti rows do not match between #1 and #" + AString::number(i + 1));
        if (thisXML.getNumberOfColumns() != rowSize) throw AlgorithmException("ERROR: row size doesn't match, but CiftiXML matchesForRows() returned true!");
        if (leftROI != NULL)
        {
            verifySurfaceComponent(i, ciftiList[i], StructureEnum::CORTEX_LEFT, leftROI);
        }
        if (rightROI != NULL)
        {
            verifySurfaceComponent(i, ciftiList[i], StructureEnum::CORTEX_RIGHT, rightROI);
        }
        if (cerebROI != NULL)
        {
            verifySurfaceComponent(i, ciftiList[i], StructureEnum::CEREBELLUM, cerebROI);
        }
        if (volROI != NULL)
        {
            verifyVolumeComponent(i, ciftiList[i], volROI);
        }
    }
    for (int i = 0; i < numCifti; ++i)
    {
        if (leftROI != NULL)
        {
            processSurfaceComponent(accum, accumCount, ciftiList[i], StructureEnum::CORTEX_LEFT, leftROI);
        }
        if (rightROI != NULL)
        {
            processSurfaceComponent(accum, accumCount, ciftiList[i], StructureEnum::CORTEX_RIGHT, rightROI);
        }
        if (cerebROI != NULL)
        {
            processSurfaceComponent(accum, accumCount, ciftiList[i], StructureEnum::CEREBELLUM, cerebROI);
        }
        if (volROI != NULL)
        {
            processVolumeComponent(accum, accumCount, ciftiList[i], volROI);
        }
    }
    if (accumCount == 0) throw AlgorithmException("no data matched the ROI(s)");
    CiftiXML newXml = baseXML;
    newXml.applyRowMapToColumns();
    newXml.resetRowsToScalars(1);
    newXml.setMapNameForRowIndex(0, "row average");
    ciftiOut->setCiftiXML(newXml);
    vector<float> outCol(rowSize);
    for (int i = 0; i < rowSize; ++i)
    {
        outCol[i] = accum[i] / accumCount;
    }
    ciftiOut->setColumn(outCol.data(), 0);
}

void AlgorithmCiftiAverageDenseROI::verifySurfaceComponent(const int& index, const CiftiInterface* myCifti, const StructureEnum::Enum& myStruct, const MetricFile* myRoi)
{
    const CiftiXML& myXml = myCifti->getCiftiXML();
    if (!myXml.hasColumnSurfaceData(myStruct))
    {
        CaretLogWarning("cifti file #" + AString::number(index + 1) + " missing structure " + StructureEnum::toName(myStruct));
        return;
    }
    if (myRoi->getNumberOfNodes() != myXml.getColumnSurfaceNumberOfNodes(myStruct)) throw AlgorithmException("cifti #" + AString::number(index + 1) + " number of vertices does not match roi");
}

void AlgorithmCiftiAverageDenseROI::processSurfaceComponent(vector<double>& accum, int64_t& accumCount, const CiftiInterface* myCifti, const StructureEnum::Enum& myStruct, const MetricFile* myRoi)
{
    const CiftiXML& myXml = myCifti->getCiftiXML();
    if (!myXml.hasColumnSurfaceData(myStruct))
    {
        return;
    }
    if (myRoi->getNumberOfNodes() != myXml.getColumnSurfaceNumberOfNodes(myStruct)) throw AlgorithmException("cifti number of vertices does not match roi");
    int rowSize = myXml.getNumberOfColumns();
    vector<float> rowScratch(rowSize);
    CaretAssert(rowScratch.size() == accum.size());
    vector<CiftiSurfaceMap> myMap;
    myXml.getSurfaceMapForColumns(myMap, myStruct);
    int mapSize = (int)myMap.size();
    const float* roiCol = myRoi->getValuePointerForColumn(0);
    for (int i = 0; i < mapSize; ++i)
    {
        if (roiCol[myMap[i].m_surfaceNode] > 0.0f)
        {
            myCifti->getRow(rowScratch.data(), myMap[i].m_ciftiIndex);
            for (int j = 0; j < rowSize; ++j)
            {
                accum[j] += rowScratch[j];
            }
            ++accumCount;
        }
    }
}

void AlgorithmCiftiAverageDenseROI::verifyVolumeComponent(const int& index, const CiftiInterface* myCifti, const VolumeFile* volROI)
{
    const CiftiXML& myXml = myCifti->getCiftiXML();
    int64_t dims[3];
    vector<vector<float> > sform;
    myXml.getVolumeDimsAndSForm(dims, sform);
    if (!volROI->matchesVolumeSpace(dims, sform)) throw AlgorithmException("cifti file #" + AString::number(index + 1) + " doesn't match the ROI volume's space");
    vector<CiftiVolumeMap> myMap;
    myXml.getVolumeMapForColumns(myMap);
    int mapSize = (int)myMap.size();
    for (int i = 0; i < mapSize; ++i)
    {
        if (!volROI->indexValid(myMap[i].m_ijk)) throw AlgorithmException("cifti file #" + AString::number(index + 1) + " lists invalid voxels");
    }
}

void AlgorithmCiftiAverageDenseROI::processVolumeComponent(vector<double>& accum, int64_t& accumCount, const CiftiInterface* myCifti, const VolumeFile* volROI)
{
    const CiftiXML& myXml = myCifti->getCiftiXML();
    int64_t dims[3];
    vector<vector<float> > sform;
    myXml.getVolumeDimsAndSForm(dims, sform);
    if (!volROI->matchesVolumeSpace(dims, sform)) throw AlgorithmException("cifti file doesn't match the ROI volume's space");
    int rowSize = myXml.getNumberOfColumns();
    vector<float> rowScratch(rowSize);
    CaretAssert(rowScratch.size() == accum.size());
    vector<CiftiVolumeMap> myMap;
    myXml.getVolumeMapForColumns(myMap);
    int mapSize = (int)myMap.size();
    for (int i = 0; i < mapSize; ++i)
    {
        if (!volROI->indexValid(myMap[i].m_ijk)) throw AlgorithmException("cifti file lists invalid voxels");
        if (volROI->getValue(myMap[i].m_ijk) > 0.0f)
        {
            myCifti->getRow(rowScratch.data(), myMap[i].m_ciftiIndex);
            for (int j = 0; j < rowSize; ++j)
            {
                accum[j] += rowScratch[j];
            }
            ++accumCount;
        }
    }
}

float AlgorithmCiftiAverageDenseROI::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiAverageDenseROI::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
