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

#include "AlgorithmCiftiAverageROICorrelation.h"
#include "AlgorithmException.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CiftiFile.h"
#include "FileInformation.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

#include <cmath>
#include <fstream>
#include <string>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmCiftiAverageROICorrelation::getCommandSwitch()
{
    return "-cifti-average-roi-correlation";
}

AString AlgorithmCiftiAverageROICorrelation::getShortDescription()
{
    return "CORRELATE ROI AVERAGE WITH ALL ROWS THEN AVERAGE ACROSS SUBJECTS";
}

OperationParameters* AlgorithmCiftiAverageROICorrelation::getParameters()
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
        AString("Averages rows for each map of the ROI(s), takes the correlation of each ROI average to the rest of the rows in the same file, applies the fisher small z transform, then averages the results across all files.  ") +
        "ROIs are always treated as weighting functions, including negative values.  " +
        "For efficiency, ensure that everything that is not intended to be used is zero in the ROI map.  " +
        "If -cifti-roi is specified, -left-roi, -right-roi, -cerebellum-roi, and -vol-roi must not be specified.  " +
        "If multiple non-cifti ROI files are specified, they must have the same number of columns."
    );
    return ret;
}

void AlgorithmCiftiAverageROICorrelation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
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
    const vector<ParameterComponent*>& ciftiInputs = *(myParams->getRepeatableParameterInstances(10));
    if (ciftiInputs.size() == 0) throw AlgorithmException("at least one -cifti input is required");
    for (int i = 0; i < (int)ciftiInputs.size(); ++i)
    {
        ciftiList.push_back(ciftiInputs[i]->getCifti(1));
    }
    if (ciftiROI != NULL)
    {
        AlgorithmCiftiAverageROICorrelation(myProgObj, ciftiList, ciftiOut, ciftiROI, leftAreaSurf, rightAreaSurf, cerebAreaSurf);
    } else {
        AlgorithmCiftiAverageROICorrelation(myProgObj, ciftiList, ciftiOut, leftROI, rightROI, cerebROI, volROI, leftAreaSurf, rightAreaSurf, cerebAreaSurf);
    }
}

AlgorithmCiftiAverageROICorrelation::AlgorithmCiftiAverageROICorrelation(ProgressObject* myProgObj, const vector<const CiftiFile*>& ciftiList, CiftiFile* ciftiOut,
                                                             const MetricFile* leftROI, const MetricFile* rightROI, const MetricFile* cerebROI, const VolumeFile* volROI,
                                                             const SurfaceFile* leftAreaSurf, const SurfaceFile* rightAreaSurf, const SurfaceFile* cerebAreaSurf) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(ciftiOut != NULL);
    LevelProgress myProgress(myProgObj);
    int numCifti = (int)ciftiList.size();
    if (numCifti < 1) throw AlgorithmException("no cifti files specified to average");
    const CiftiXMLOld& baseXML = ciftiList[0]->getCiftiXMLOld();
    int rowSize = baseXML.getNumberOfColumns();
    int colSize = baseXML.getNumberOfRows();
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
        const CiftiXMLOld& thisXML = ciftiList[i]->getCiftiXMLOld();
        if (!thisXML.mappingMatches(CiftiXMLOld::ALONG_COLUMN, baseXML, CiftiXMLOld::ALONG_COLUMN)) throw AlgorithmException("cifti space does not match between cifti #1 and #" + AString::number(i + 1));
        if (thisXML.getNumberOfColumns() != rowSize) throw AlgorithmException("row length doesn't match between cifti #1 and #" + AString::number(i + 1));
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
    vector<vector<float> > tempresult(colSize, vector<float>(numMaps));
    CiftiXMLOld newXml = baseXML;
    newXml.resetRowsToScalars(numMaps);
    for (int i = 0; i < numMaps; ++i)
    {
        newXml.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, i, nameFile->getMapName(i));
    }
    ciftiOut->setCiftiXML(newXml);
    if (numCifti > 1)//skip averaging in single subject case
    {
        vector<vector<double> > accum(colSize, vector<double>(numMaps, 0.0));
        for (int i = 0; i < numCifti; ++i)
        {
            processCifti(ciftiList[i], tempresult, leftROI, rightROI, cerebROI, volROI, numMaps, leftAreaPointer, rightAreaPointer, cerebAreaPointer);
            for (int j = 0; j < colSize; ++j)
            {
                for (int myMap = 0; myMap < numMaps; ++myMap)
                {
                    accum[j][myMap] += tempresult[j][myMap];
                }
            }
        }
        for (int i = 0; i < colSize; ++i)
        {
            for (int myMap = 0; myMap < numMaps; ++myMap)
            {
                tempresult[i][myMap] = accum[i][myMap] / numCifti;
            }
            ciftiOut->setRow(tempresult[i].data(), i);
        }
    } else {
        processCifti(ciftiList[0], tempresult, leftROI, rightROI, cerebROI, volROI, numMaps, leftAreaPointer, rightAreaPointer, cerebAreaPointer);
        for (int i = 0; i < colSize; ++i)
        {
            ciftiOut->setRow(tempresult[i].data(), i);
        }
    }
}

AlgorithmCiftiAverageROICorrelation::AlgorithmCiftiAverageROICorrelation(ProgressObject* myProgObj, const vector<const CiftiFile*>& ciftiList, CiftiFile* ciftiOut, const CiftiFile* ciftiROI,
                                                                         const SurfaceFile* leftAreaSurf, const SurfaceFile* rightAreaSurf, const SurfaceFile* cerebAreaSurf): AbstractAlgorithm(myProgObj)
{
    CaretAssert(ciftiOut != NULL);
    LevelProgress myProgress(myProgObj);
    int numCifti = (int)ciftiList.size();
    if (numCifti < 1) throw AlgorithmException("no cifti files specified to average");
    const CiftiXMLOld baseXML = ciftiList[0]->getCiftiXMLOld(), roiXML = ciftiROI->getCiftiXMLOld();
    int rowSize = baseXML.getNumberOfColumns();
    int colSize = baseXML.getNumberOfRows();
    int numMaps = ciftiROI->getNumberOfColumns();
    if (!baseXML.mappingMatches(CiftiXMLOld::ALONG_COLUMN, roiXML, CiftiXMLOld::ALONG_COLUMN)) throw AlgorithmException("cifti roi doesn't match cifti space of data");
    for (int i = 1; i < numCifti; ++i)
    {
        if (!baseXML.mappingMatches(CiftiXMLOld::ALONG_COLUMN, ciftiList[i]->getCiftiXMLOld(), CiftiXMLOld::ALONG_COLUMN)) throw AlgorithmException("cifti space does not match between cifti #1 and #" + AString::number(i + 1));
        if (ciftiList[i]->getNumberOfColumns() != rowSize) throw AlgorithmException("row length doesn't match between cifti #1 and #" + AString::number(i + 1));
    }
    vector<float> leftAreaData, rightAreaData, cerebAreaData;
    float* leftAreaPointer = NULL, *rightAreaPointer = NULL, *cerebAreaPointer = NULL;
    if (leftAreaSurf != NULL)
    {
        if (baseXML.getSurfaceNumberOfNodes(CiftiXMLOld::ALONG_COLUMN, StructureEnum::CORTEX_LEFT) != leftAreaSurf->getNumberOfNodes())
        {
            throw AlgorithmException("left area surface and left cortex cifti structure have different number of nodes");
        }
        leftAreaSurf->computeNodeAreas(leftAreaData);
        leftAreaPointer = leftAreaData.data();
    }
    if (rightAreaSurf != NULL)
    {
        if (baseXML.getSurfaceNumberOfNodes(CiftiXMLOld::ALONG_COLUMN, StructureEnum::CORTEX_RIGHT) != rightAreaSurf->getNumberOfNodes())
        {
            throw AlgorithmException("right area surface and right cortex cifti structure have different number of nodes");
        }
        rightAreaSurf->computeNodeAreas(rightAreaData);
        rightAreaPointer = rightAreaData.data();
    }
    if (cerebAreaSurf != NULL)
    {
        if (baseXML.getSurfaceNumberOfNodes(CiftiXMLOld::ALONG_COLUMN, StructureEnum::CEREBELLUM) != cerebAreaSurf->getNumberOfNodes())
        {
            throw AlgorithmException("cerebellum area surface and cerebellum cortex cifti structure have different number of nodes");
        }
        cerebAreaSurf->computeNodeAreas(cerebAreaData);
        cerebAreaPointer = cerebAreaData.data();
    }
    vector<vector<float> > tempresult(colSize, vector<float>(numMaps));
    CiftiXMLOld newXml = baseXML;
    newXml.resetRowsToScalars(numMaps);
    for (int i = 0; i < numMaps; ++i)
    {
        newXml.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, i, roiXML.getMapNameForRowIndex(i));
    }
    ciftiOut->setCiftiXML(newXml);
    if (numCifti > 1)//skip averaging in single subject case
    {
        vector<vector<double> > accum(colSize, vector<double>(numMaps, 0.0));
        for (int i = 0; i < numCifti; ++i)
        {
            processCifti(ciftiList[i], tempresult, ciftiROI, numMaps, leftAreaPointer, rightAreaPointer, cerebAreaPointer);
            for (int j = 0; j < colSize; ++j)
            {
                for (int myMap = 0; myMap < numMaps; ++myMap)
                {
                    accum[j][myMap] += tempresult[j][myMap];
                }
            }
        }
        for (int i = 0; i < colSize; ++i)
        {
            for (int myMap = 0; myMap < numMaps; ++myMap)
            {
                tempresult[i][myMap] = accum[i][myMap] / numCifti;
            }
            ciftiOut->setRow(tempresult[i].data(), i);
        }
    } else {
        processCifti(ciftiList[0], tempresult, ciftiROI, numMaps, leftAreaPointer, rightAreaPointer, cerebAreaPointer);
        for (int i = 0; i < colSize; ++i)
        {
            ciftiOut->setRow(tempresult[i].data(), i);
        }
    }
}

void AlgorithmCiftiAverageROICorrelation::verifySurfaceComponent(const int& index, const CiftiFile* myCifti, const StructureEnum::Enum& myStruct, const MetricFile* myRoi)
{
    const CiftiXMLOld& myXml = myCifti->getCiftiXMLOld();
    if (!myXml.hasColumnSurfaceData(myStruct))
    {
        CaretLogWarning("cifti file #" + AString::number(index + 1) + " missing structure " + StructureEnum::toName(myStruct));
        return;
    }
    if (myRoi->getNumberOfNodes() != myXml.getColumnSurfaceNumberOfNodes(myStruct)) throw AlgorithmException("cifti #" + AString::number(index + 1) + " number of vertices does not match roi");
}

void AlgorithmCiftiAverageROICorrelation::verifyVolumeComponent(const int& index, const CiftiFile* myCifti, const VolumeFile* volROI)
{
    const CiftiXMLOld& myXml = myCifti->getCiftiXMLOld();
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

void AlgorithmCiftiAverageROICorrelation::processCifti(const CiftiFile* myCifti, vector<vector<float> >& output,
                                                       const MetricFile* leftROI, const MetricFile* rightROI,const MetricFile* cerebROI, const VolumeFile* volROI,
                                                       const int& numMaps, const float* leftAreas, const float* rightAreas, const float* cerebAreas)
{
    int rowSize = myCifti->getNumberOfColumns();
    int colSize = myCifti->getNumberOfRows();
    vector<vector<float> > average(numMaps, vector<float>(rowSize));
    vector<float> rrs(numMaps);
    for (int myMap = 0; myMap < numMaps; ++myMap)
    {
        vector<double> accumarray(rowSize, 0.0);
        addSurface(myCifti, StructureEnum::CORTEX_LEFT, accumarray, leftROI, myMap, leftAreas);//we don't need to keep track of the kernel sums because we are correlating
        addSurface(myCifti, StructureEnum::CORTEX_RIGHT, accumarray, rightROI, myMap, rightAreas);
        addSurface(myCifti, StructureEnum::CEREBELLUM, accumarray, cerebROI, myMap, cerebAreas);
        addVolume(myCifti, accumarray, volROI, myMap);
        double accum = 0.0;
        for (int i = 0; i < rowSize; ++i)
        {
            accum += accumarray[i];
        }
        double mean = accum / rowSize;
        accum = 0.0;
        for (int i = 0; i < rowSize; ++i)
        {
            average[myMap][i] = accumarray[i] - mean;//remove the mean from the average timeseries to optimize the correlation, and change back to float for possible speed improvement
            accum += average[myMap][i] * average[myMap][i];
        }
        rrs[myMap] = sqrt(accum);//compute this only once
    }
    int curRow = 0;
#pragma omp CARET_PAR
    {
        vector<float> rowscratch(rowSize);
#pragma omp CARET_FOR schedule(dynamic)
        for (int i = 0; i < colSize; ++i)
        {
            int myRow;
#pragma omp critical
            {
                myRow = curRow;//force sequential reading
                ++curRow;
                myCifti->getRow(rowscratch.data(), myRow);//and never read multiple rows at once from the same file
            }
            double tempaccum = 0.0;//compute mean of new row
            for (int j = 0; j < rowSize; ++j)
            {
                tempaccum += rowscratch[j];
            }
            float thismean = tempaccum / rowSize;
            tempaccum = 0.0;
            for (int j = 0; j < rowSize; ++j)
            {
                rowscratch[j] -= thismean;//demean
                tempaccum += rowscratch[j] * rowscratch[j];//precompute rrs
            }
            float thisrrs = sqrt(tempaccum);
            for (int myMap = 0; myMap < numMaps; ++myMap)
            {
                double corraccum = 0.0;//correlate
                for (int j = 0; j < rowSize; ++j)
                {
                    corraccum += rowscratch[j] * average[myMap][j];//gather the correlation
                }
                corraccum /= rrs[myMap] * thisrrs;
                if (corraccum > 0.999999) corraccum = 0.999999;
                if (corraccum < -0.999999) corraccum = -0.999999;
                output[myRow][myMap] = 0.5 * log((1 + corraccum) / (1 - corraccum));//fisher z transform, needed for averaging
            }
        }
    }
}

void AlgorithmCiftiAverageROICorrelation::processCifti(const CiftiFile* myCifti, vector<vector<float> >& output, const CiftiFile* ciftiROI, const int& numMaps,
                                                       const float* leftAreas, const float* rightAreas, const float* cerebAreas)
{
    int rowSize = myCifti->getNumberOfColumns();
    int colSize = myCifti->getNumberOfRows();
    vector<vector<float> > average(numMaps, vector<float>(rowSize));
    vector<float> rrs(numMaps);
    const CiftiXMLOld& roiXML = ciftiROI->getCiftiXMLOld();
    vector<StructureEnum::Enum> surfStructures, ignored;
    roiXML.getStructureLists(CiftiXMLOld::ALONG_COLUMN, surfStructures, ignored);
    vector<float> roiScratch(numMaps), dataScratch(rowSize);
    {
        vector<vector<double> > accumarray(numMaps, vector<double>(rowSize, 0.0));
        for (int whichStruct = 0; whichStruct < (int)surfStructures.size(); ++whichStruct)
        {
            const float* areaPtr = NULL;
            switch (surfStructures[whichStruct])
            {
                case StructureEnum::CORTEX_LEFT:
                    areaPtr = leftAreas;
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    areaPtr = rightAreas;
                    break;
                case StructureEnum::CEREBELLUM:
                    areaPtr = cerebAreas;
                    break;
                default:
                    break;
            }
            vector<CiftiSurfaceMap> myMap;
            roiXML.getSurfaceMap(CiftiXMLOld::ALONG_COLUMN, myMap, surfStructures[whichStruct]);
            for (int i = 0; i < (int)myMap.size(); ++i)
            {
                bool dataLoaded = false;
                ciftiROI->getRow(roiScratch.data(), myMap[i].m_ciftiIndex);
                for (int j = 0; j < numMaps; ++j)
                {
                    if (roiScratch[j] != 0.0f)
                    {
                        if (!dataLoaded)
                        {
                            myCifti->getRow(dataScratch.data(), myMap[i].m_ciftiIndex);
                            dataLoaded = true;
                        }
                        if (areaPtr != NULL)
                        {
                            for (int k = 0; k < rowSize; ++k)
                            {
                                accumarray[j][k] += dataScratch[k] * roiScratch[j] * areaPtr[myMap[i].m_surfaceNode];
                            }
                        } else {
                            for (int k = 0; k < rowSize; ++k)
                            {
                                accumarray[j][k] += dataScratch[k] * roiScratch[j];
                            }
                        }
                    }
                }
            }
        }
        vector<CiftiVolumeMap> myMap;
        roiXML.getVolumeMap(CiftiXMLOld::ALONG_COLUMN, myMap);
        for (int i = 0; i < (int)myMap.size(); ++i)
        {
            bool dataLoaded = false;
            ciftiROI->getRow(roiScratch.data(), myMap[i].m_ciftiIndex);
            for (int j = 0; j < numMaps; ++j)
            {
                if (roiScratch[j] != 0.0f)
                {
                    if (!dataLoaded)
                    {
                        myCifti->getRow(dataScratch.data(), myMap[i].m_ciftiIndex);
                        dataLoaded = true;
                    }
                    for (int k = 0; k < rowSize; ++k)
                    {
                        accumarray[j][k] += dataScratch[k] * roiScratch[j];
                    }
                }
            }
        }
        for (int i = 0; i < numMaps; ++i)
        {
            double accum = 0.0;
            for (int j = 0; j < rowSize; ++j)
            {
                accum += accumarray[i][j];
            }
            float mean = accum / rowSize;
            accum = 0.0;
            for (int j = 0; j < rowSize; ++j)
            {
                average[i][j] = accumarray[i][j] - mean;
                accum += average[i][j] * average[i][j];
            }
            vector<double>().swap(accumarray[i]);//hack to free memory before it goes out of scope
            rrs[i] = sqrt(accum);
        }
    }
    int curRow = 0;
#pragma omp CARET_PAR
    {
        vector<float> rowscratch(rowSize);
#pragma omp CARET_FOR schedule(dynamic)
        for (int i = 0; i < colSize; ++i)
        {
            int myRow;
#pragma omp critical
            {
                myRow = curRow;//force sequential reading
                ++curRow;
                myCifti->getRow(rowscratch.data(), myRow);//and never read multiple rows at once from the same file
            }
            double tempaccum = 0.0;//compute mean of new row
            for (int j = 0; j < rowSize; ++j)
            {
                tempaccum += rowscratch[j];
            }
            float thismean = tempaccum / rowSize;
            tempaccum = 0.0;
            for (int j = 0; j < rowSize; ++j)
            {
                rowscratch[j] -= thismean;//demean
                tempaccum += rowscratch[j] * rowscratch[j];//precompute rrs
            }
            float thisrrs = sqrt(tempaccum);
            for (int myMap = 0; myMap < numMaps; ++myMap)
            {
                double corraccum = 0.0;//correlate
                for (int j = 0; j < rowSize; ++j)
                {
                    corraccum += rowscratch[j] * average[myMap][j];//gather the correlation
                }
                corraccum /= rrs[myMap] * thisrrs;
                if (corraccum > 0.999999) corraccum = 0.999999;
                if (corraccum < -0.999999) corraccum = -0.999999;
                output[myRow][myMap] = 0.5 * log((1 + corraccum) / (1 - corraccum));//fisher z transform, needed for averaging
            }
        }
    }
}

void AlgorithmCiftiAverageROICorrelation::addSurface(const CiftiFile* myCifti, StructureEnum::Enum myStruct, vector<double>& accum, const MetricFile* myRoi, const int& myMap, const float* myAreas)
{
    if (myRoi == NULL) return;
    vector<CiftiBrainModelsMap::SurfaceMap> surfaceMap = myCifti->getCiftiXML().getBrainModelsMap(CiftiXML::ALONG_COLUMN).getSurfaceMap(myStruct);
    int mapSize = (int)surfaceMap.size();
    int rowSize = myCifti->getNumberOfColumns();
    vector<float> rowscratch(rowSize);
    if (myAreas != NULL)
    {
        for (int i = 0; i < mapSize; ++i)
        {
            float value = myRoi->getValue(surfaceMap[i].m_surfaceNode, myMap);
            if (value != 0.0f)
            {
                float thisArea = myAreas[surfaceMap[i].m_surfaceNode];
                myCifti->getRow(rowscratch.data(), surfaceMap[i].m_ciftiIndex);
                for (int j = 0; j < rowSize; ++j)
                {
                    accum[j] += rowscratch[j] * value * thisArea;
                }
            }
        }
    } else {
        for (int i = 0; i < mapSize; ++i)
        {
            float value = myRoi->getValue(surfaceMap[i].m_surfaceNode, myMap);
            if (value != 0.0f)
            {
                myCifti->getRow(rowscratch.data(), surfaceMap[i].m_ciftiIndex);
                for (int j = 0; j < rowSize; ++j)
                {
                    accum[j] += rowscratch[j] * value;
                }
            }
        }
    }
}

void AlgorithmCiftiAverageROICorrelation::addVolume(const CiftiFile* myCifti, vector<double>& accum, const VolumeFile* myRoi, const int& myMap)
{
    if (myRoi == NULL) return;
    vector<CiftiBrainModelsMap::VolumeMap> volMap = myCifti->getCiftiXML().getBrainModelsMap(CiftiXML::ALONG_COLUMN).getFullVolumeMap();
    int mapSize = (int)volMap.size();
    int rowSize = myCifti->getNumberOfColumns();
    vector<float> rowscratch(rowSize);
    for (int i = 0; i < mapSize; ++i)
    {
        if (myRoi->getValue(volMap[i].m_ijk, myMap) > 0.0f)
        {
            myCifti->getRow(rowscratch.data(), volMap[i].m_ciftiIndex);
            for (int j = 0; j < rowSize; ++j)
            {
                accum[j] += rowscratch[j];
            }
        }
    }
}

float AlgorithmCiftiAverageROICorrelation::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiAverageROICorrelation::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
