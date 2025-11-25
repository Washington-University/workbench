
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __BORDER_OPTIMIZE_EXECUTOR_DECLARE__
#include "BorderOptimizeExecutor.h"
#undef __BORDER_OPTIMIZE_EXECUTOR_DECLARE__

#include "Border.h"
#include "CaretAssert.h"
#include "Surface.h"

#include "AlgorithmBorderResample.h"
#include "AlgorithmBorderToVertices.h"
#include "AlgorithmCiftiCorrelationGradient.h"
#include "AlgorithmCiftiRestrictDenseMap.h"
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmMetricDilate.h"
#include "AlgorithmMetricGradient.h"
#include "AlgorithmMetricFindClusters.h"
#include "AlgorithmNodesInsideBorder.h"
#include "AlgorithmSurfaceCreateSphere.h"
#include "AlgorithmSurfaceResample.h"
#include "BorderFile.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CiftiDenseSparseFile.h"
#include "CiftiFile.h"
#include "CiftiMappableDataFile.h"
#include "EventProgressUpdate.h"
#include "FileInformation.h"
#include "GeodesicHelper.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceResamplingHelper.h"
#include "TextFile.h"
#include "TopologyHelper.h"

#include <cmath>

using namespace caret;
using namespace std;

    
/**
 * \class caret::BorderOptimizeExecutor 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 * \ingroup GuiQt
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
BorderOptimizeExecutor::BorderOptimizeExecutor()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BorderOptimizeExecutor::~BorderOptimizeExecutor()
{
}

namespace
{
    void doCombination(const MetricFile& gradient, const vector<int32_t>& roiNodes, const bool& invert, const float& mapStrength, vector<float>& combinedGradData)
    {
        const float* gradVals = gradient.getValuePointerForColumn(0);
        float myMin = 0.0f, myMax = 0.0f;
        bool first = true;
        int numSelected = (int)roiNodes.size();
        for (int i = 0; i < numSelected; ++i)//for normalizing, gather min/max
        {
            float tempVal = gradVals[roiNodes[i]];
            if (MathFunctions::isNumeric(tempVal))
            {
                if (first)
                {
                    first = false;
                    myMin = tempVal;
                    myMax = tempVal;
                } else {
                    if (tempVal < myMin) myMin = tempVal;
                    if (tempVal > myMax) myMax = tempVal;
                }
            }
        }
        if (myMin == myMax)//if no contrast, map it all to (post-inversion) minimum - also trips if no numeric data
        {
            if (invert)
            {
                myMin -= 1.0f;
            } else {
                myMax += 1.0f;
            }
        }
        float myRange = myMax - myMin;
        for (int i = 0; i < numSelected; ++i)//for normalizing, gather min/max
        {
            float tempVal = gradVals[roiNodes[i]];
            if (MathFunctions::isNumeric(tempVal))
            {
                float toCombine;
                if (invert)
                {
                    toCombine = (myMax - tempVal) / myRange;
                } else {
                    toCombine = (tempVal - myMin) / myRange;
                }
                combinedGradData[roiNodes[i]] *= 1.0f + mapStrength * (toCombine - 1.0f);//equals toCombine * mapStrength + 1.0f - mapStrength
            } else {
                combinedGradData[roiNodes[i]] *= 1.0f - mapStrength;
            }
        }
    }
    
    bool extractGradientData(const CaretMappableDataFile* dataFile, const int32_t& mapIndex, SurfaceFile* surface, const MetricFile* gradRoi,
                             const float& smoothing, const MetricFile* correctedAreasMetric, MetricFile& gradientOut, const bool& skipGradient, const float& excludeDist)
    {
        int numNodes = surface->getNumberOfNodes();
        MetricFile tempData, tempRoi;
        const MetricFile* useData = &tempData, *useRoi = gradRoi;
        switch (dataFile->getDataFileType())
        {
            case DataFileTypeEnum::METRIC:
            {
                const MetricFile* metricFile = dynamic_cast<const MetricFile*>(dataFile);
                CaretAssert(metricFile != NULL);
                CaretAssert(metricFile->getNumberOfNodes() == numNodes);
                useData = metricFile;
                break;
            }
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SPARSE:
            {
                tempData.setNumberOfNodesAndColumns(numNodes, 1);
                tempData.setStructure(surface->getStructure());
                tempRoi.setNumberOfNodesAndColumns(numNodes, 1);
                const CiftiDenseSparseFile* ciftiDenseSparseFile = dynamic_cast<const CiftiDenseSparseFile*>(dataFile);
                CaretAssert(ciftiDenseSparseFile != NULL);
                CaretAssert(ciftiDenseSparseFile->getMappingSurfaceNumberOfNodes(surface->getStructure()) == numNodes);
                vector<float> surfData, ciftiRoi;
                bool result = ciftiDenseSparseFile->getMapDataForSurface(mapIndex, surface->getStructure(), surfData, &ciftiRoi);
                CaretAssert(result);
                if (!result)
                {
                    CaretLogSevere("failed to get map data for map " + AString::number(mapIndex) + " of data file of type " + DataFileTypeEnum::toName(dataFile->getDataFileType()));
                    return false;
                }
                tempData.setValuesForColumn(0, surfData.data());
                vector<float> maskedROI(numNodes);
                const float* gradRoiData = gradRoi->getValuePointerForColumn(0);
                for (int i = 0; i < numNodes; ++i)
                {
                    if (gradRoiData[i] > 0.0f)
                    {
                        maskedROI[i] = ciftiRoi[i];
                    } else {
                        maskedROI[i] = 0.0f;
                    }
                }
                tempRoi.setValuesForColumn(0, maskedROI.data());
                useRoi = &tempRoi;
                break;
            }
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            {
                tempData.setNumberOfNodesAndColumns(numNodes, 1);
                tempData.setStructure(surface->getStructure());
                tempRoi.setNumberOfNodesAndColumns(numNodes, 1);
                const CiftiMappableDataFile* ciftiMappableFile = dynamic_cast<const CiftiMappableDataFile*>(dataFile);
                CaretAssert(ciftiMappableFile != NULL);
                CaretAssert(ciftiMappableFile->getMappingSurfaceNumberOfNodes(surface->getStructure()) == numNodes);
                vector<float> surfData, ciftiRoi;
                bool result = ciftiMappableFile->getMapDataForSurface(mapIndex, surface->getStructure(), surfData, &ciftiRoi);
                CaretAssert(result);
                if (!result)
                {
                    CaretLogSevere("failed to get map data for map " + AString::number(mapIndex) + " of data file of type " + DataFileTypeEnum::toName(dataFile->getDataFileType()));
                    return false;
                }
                tempData.setValuesForColumn(0, surfData.data());
                vector<float> maskedROI(numNodes);
                const float* gradRoiData = gradRoi->getValuePointerForColumn(0);
                for (int i = 0; i < numNodes; ++i)
                {
                    if (gradRoiData[i] > 0.0f)
                    {
                        maskedROI[i] = ciftiRoi[i];
                    } else {
                        maskedROI[i] = 0.0f;
                    }
                }
                tempRoi.setValuesForColumn(0, maskedROI.data());
                useRoi = &tempRoi;
                break;
            }
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
            {
                const CiftiMappableDataFile* ciftiMappableFile = dynamic_cast<const CiftiMappableDataFile*>(dataFile);
                CaretAssert(ciftiMappableFile != NULL);
                CaretAssert(ciftiMappableFile->getMappingSurfaceNumberOfNodes(surface->getStructure()) == numNodes);
                const CiftiFile* dataCifti = ciftiMappableFile->getCiftiFile();
                const MetricFile* leftRoi = NULL, *rightRoi = NULL, *cerebRoi = NULL;
                const MetricFile* leftCorrAreas = NULL, *rightCorrAreas = NULL, *cerebCorrAreas = NULL;
                SurfaceFile* leftSurf = NULL, *rightSurf = NULL, *cerebSurf = NULL;
                switch (surface->getStructure())
                {
                    case StructureEnum::CORTEX_LEFT:
                        leftRoi = gradRoi;
                        leftCorrAreas = correctedAreasMetric;
                        leftSurf = surface;
                        break;
                    case StructureEnum::CORTEX_RIGHT:
                        rightRoi = gradRoi;
                        rightCorrAreas = correctedAreasMetric;
                        rightSurf = surface;
                        break;
                    case StructureEnum::CEREBELLUM:
                        cerebRoi = gradRoi;
                        cerebCorrAreas = correctedAreasMetric;
                        cerebSurf = surface;
                        break;
                    default:
                        CaretAssert(false);
                        break;
                }
                CiftiFile restrictCifti, corrGradCifti;
                AlgorithmCiftiRestrictDenseMap(NULL, dataCifti, CiftiXML::ALONG_COLUMN, &restrictCifti, leftRoi, rightRoi, cerebRoi, NULL);
                AlgorithmCiftiCorrelationGradient(NULL, &restrictCifti, &corrGradCifti, leftSurf, rightSurf, cerebSurf,
                                                  leftCorrAreas, rightCorrAreas, cerebCorrAreas, smoothing, 0.0f, false, false, excludeDist, -1.0f);
                AlgorithmCiftiSeparate(NULL, &corrGradCifti, CiftiXML::ALONG_COLUMN, surface->getStructure(), &gradientOut);
                return true;
            }
            default:
                CaretLogWarning("ignoring map " + AString::number(mapIndex) + " of data file of type " + DataFileTypeEnum::toName(dataFile->getDataFileType()));
                return false;
        }
        if (skipGradient)
        {
            gradientOut.setNumberOfNodesAndColumns(numNodes, 1);
            gradientOut.setStructure(surface->getStructure());
            gradientOut.setValuesForColumn(0, useData->getValuePointerForColumn(0));
        } else {
            AlgorithmMetricGradient(NULL, surface, useData, &gradientOut, NULL, smoothing, useRoi, false, 0, correctedAreasMetric);
        }
        return true;
    }
    
    bool getStatisticsString(const CaretMappableDataFile* dataFile, const int32_t& mapIndex, const vector<int32_t> nodeLists[2],
                             const SurfaceFile& surface, const MetricFile* correctedAreasMetric, const float& excludeDist, AString& statsOut)
    {
        vector<float> tempStatsStore, roiData;
        StructureEnum::Enum structure = surface.getStructure();
        int numNodes = surface.getNumberOfNodes();
        const float* statsData = NULL;
        switch (dataFile->getDataFileType())
        {
            case DataFileTypeEnum::METRIC:
            {
                const MetricFile* metricFile = dynamic_cast<const MetricFile*>(dataFile);
                CaretAssert(metricFile != NULL);
                statsData = metricFile->getValuePointerForColumn(mapIndex);
                break;
            }
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SPARSE:
            {
                const CiftiDenseSparseFile* ciftiDenseSparseFile = dynamic_cast<const CiftiDenseSparseFile*>(dataFile);
                CaretAssert(ciftiDenseSparseFile != NULL);
                bool result = ciftiDenseSparseFile->getMapDataForSurface(mapIndex, structure, tempStatsStore, &roiData);
                CaretAssert(result);
                if (!result)
                {
                    CaretLogSevere("failed to get map data for map " + AString::number(mapIndex) + " of data file of type " + DataFileTypeEnum::toName(dataFile->getDataFileType()));
                    return false;
                }
                statsData = tempStatsStore.data();
                break;
            }
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            {
                const CiftiMappableDataFile* ciftiMappableFile = dynamic_cast<const CiftiMappableDataFile*>(dataFile);
                CaretAssert(ciftiMappableFile != NULL);
                bool result = ciftiMappableFile->getMapDataForSurface(mapIndex, structure, tempStatsStore, &roiData);
                CaretAssert(result);
                if (!result)
                {
                    CaretLogSevere("failed to get map data for map " + AString::number(mapIndex) + " of data file of type " + DataFileTypeEnum::toName(dataFile->getDataFileType()));
                    return false;
                }
                statsData = tempStatsStore.data();
                break;
            }
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
            {
                const CiftiMappableDataFile* ciftiMappableFile = dynamic_cast<const CiftiMappableDataFile*>(dataFile);
                CaretAssert(ciftiMappableFile != NULL);
                const CiftiFile* dataCifti = ciftiMappableFile->getCiftiFile();
                const CiftiXML& myXML = dataCifti->getCiftiXML();
                const CiftiBrainModelsMap& myDenseMap = myXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
                CaretAssert(myDenseMap.hasSurfaceData(structure));//the GUI should filter by structure, right?
                int64_t rowLength = myXML.getDimensionLength(CiftiXML::ALONG_ROW);
                vector<vector<float> > data[2];
                for (int i = 0; i < 2; ++i)
                {
                    data[i].resize(nodeLists[i].size());
                    for (int j = 0; j < (int)nodeLists[i].size(); ++j)
                    {
                        int64_t index = myDenseMap.getIndexForNode(nodeLists[i][j], structure);
                        if (index != -1)//roi can go outside the cifti ROI, ignore such vertices
                        {
                            data[i][j].resize(rowLength);//only allocate the ones inside the cifti ROI
                            dataCifti->getRow(data[i][j].data(), index);
                        }
                    }
                }
                vector<float> samples[2][2];
                CaretPointer<GeodesicHelperBase> myGeoBase;
                if (correctedAreasMetric != NULL)
                {
                    myGeoBase.grabNew(new GeodesicHelperBase(&surface, correctedAreasMetric->getValuePointerForColumn(0)));
                }
                for (int posSide = 0; posSide < 2; ++posSide)
                {
                    int listSize = (int)nodeLists[posSide].size();
#pragma omp CARET_PAR
                    {
                        CaretPointer<GeodesicHelper> myGeoHelp;
                        if (correctedAreasMetric != NULL)
                        {
                            myGeoHelp.grabNew(new GeodesicHelper(myGeoBase));
                        } else {
                            myGeoHelp = surface.getGeodesicHelper();
                        }
#pragma omp CARET_FOR schedule(dynamic)
                        for (int i = 0; i < listSize; ++i)
                        {
                            if (!data[posSide][i].empty())
                            {
                                vector<int32_t> excludeNodes;
                                vector<float> excludeDists;
                                myGeoHelp->getNodesToGeoDist(nodeLists[posSide][i], excludeDist, excludeNodes, excludeDists);
                                vector<char> excludeLookup(numNodes, 0);
                                for (int j = 0; j < (int)excludeNodes.size(); ++j)
                                {
                                    excludeLookup[excludeNodes[j]] = 1;
                                }
                                for (int side = 0; side < 2; ++side)
                                {
                                    vector<double> averagerow(rowLength, 0.0);
                                    int count = 0;
                                    for (int j = 0; j < (int)nodeLists[side].size(); ++j)
                                    {
                                        if (excludeLookup[nodeLists[side][j]] == 0 && !data[side][j].empty())
                                        {
                                            ++count;
                                            float* dataRef = data[side][j].data();
                                            for (int k = 0; k < rowLength; ++k)
                                            {
                                                averagerow[k] += dataRef[k];
                                            }
                                        }
                                    }
                                    if (count != 0)
                                    {
                                        double accum1 = 0.0, accum2 = 0.0;
                                        for (int k = 0; k < rowLength; ++k)
                                        {
                                            averagerow[k] /= count;
                                            accum1 += averagerow[k];
                                            accum2 += data[posSide][i][k];
                                        }
                                        float mean1 = accum1 / rowLength, mean2 = accum2 / rowLength;
                                        accum1 = 0.0, accum2 = 0.0;
                                        double accum3 = 0.0;
                                        for (int k = 0; k < rowLength; ++k)
                                        {
                                            float val1 = averagerow[k] - mean1, val2 = data[posSide][i][k] - mean2;
                                            accum1 += val1 * val2;
                                            accum2 += val1 * val1;
                                            accum3 += val2 * val2;
                                        }
                                        double corrval = accum1 / sqrt(accum2 * accum3);
                                        if (corrval >= 0.999999)  corrval = 0.999999;//prevent inf
                                        if (corrval <= -0.999999)  corrval = -0.999999;//prevent -inf
#pragma omp critical
                                        {
                                            samples[posSide][side].push_back(0.5f * log((1 + corrval)/(1 - corrval)));//fisher z transform
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                float tstats[2], cohen_ds[2], pvals[2];
                for (int piece = 0; piece < 2; ++piece)
                {
                    int count[2];
                    float mean[2], variance[2];
                    for (int posSide = 0; posSide < 2; ++posSide)
                    {
                        count[posSide] = (int)samples[posSide][piece].size();
                        if (count[posSide] < 2) return false;
                        double accum = 0.0;
                        for (int i = 0; i < count[posSide]; ++i)
                        {
                            accum += samples[posSide][piece][i];
                        }
                        mean[posSide] = accum / count[posSide];
                        accum = 0.0;
                        for (int i = 0; i < count[posSide]; ++i)
                        {
                            float tempf = samples[posSide][piece][i] - mean[posSide];
                            accum += tempf * tempf;
                        }
                        variance[posSide] = accum / (count[posSide] - 1);
                    }
                    tstats[piece] = (mean[0] - mean[1]) / sqrt(variance[0] / count[0] + variance[1] / count[1]);//welch's t-test
                    cohen_ds[piece] = (mean[0] - mean[1]) / sqrt(((count[0] - 1) * variance[0] + (count[1] - 1) * variance[1]) / (count[0] + count[1] - 2));
                    pvals[piece] = 2.0f * MathFunctions::q_func(abs(tstats[piece]));//treat as 2-tailed z-stat, this isn't meant to be rigorous, and the t-test cdf is a pain
                }
                statsOut = "p1=" + AString::number(pvals[0], 'g', 3) + ", p2=" + AString::number(pvals[1], 'g', 3) +
                           ", t1=" + AString::number(tstats[0], 'g', 3) + ", t2=" + AString::number(tstats[1], 'g', 3) +
                           ", d1=" + AString::number(cohen_ds[0], 'g', 3) + ", d2=" + AString::number(cohen_ds[1], 'g', 3);
                return true;
            }
            default:
                CaretLogWarning("statistics: ignoring data file of type " + DataFileTypeEnum::toName(dataFile->getDataFileType()));
                return false;
        }
        float mean[2], variance[2];
        int count[2] = {0, 0};
        for (int piece = 0; piece < 2; ++piece)
        {
            double accum = 0.0;
            for (int i = 0; i < (int)nodeLists[piece].size(); ++i)
            {
                if (roiData.empty() || roiData[nodeLists[piece][i]] > 0.0f)
                {
                    accum += statsData[nodeLists[piece][i]];//weight by vertex area?  weighted welch's t-test, oh joy
                    ++count[piece];
                }
            }
            mean[piece] = accum / count[piece];
            accum = 0.0;
            for (int i = 0; i < (int)nodeLists[piece].size(); ++i)
            {
                if (roiData.empty() || roiData[nodeLists[piece][i]] > 0.0f)
                {
                    float tempf = (statsData[nodeLists[piece][i]] - mean[piece]);
                    accum += tempf * tempf;
                }
            }
            variance[piece] = accum / (count[piece] - 1);
        }
        float tstat = (mean[0] - mean[1]) / sqrt(variance[0] / count[0] + variance[1] / count[1]);//welch's t-test
        float cohen_d = (mean[0] - mean[1]) / sqrt(((count[0] - 1) * variance[0] + (count[1] - 1) * variance[1]) / (count[0] + count[1] - 2));
        float pval = 2.0f * MathFunctions::q_func(abs(tstat));//treat as 2-tailed z-stat, this isn't meant to be rigorous, and the t-test cdf is a pain
        statsOut = "p=" + AString::number(pval, 'g', 3) + ", t=" + AString::number(tstat, 'g', 3) + ", d=" + AString::number(cohen_d, 'g', 3);
        return true;
    }
    
    int getBorderPointNode(const Border* theBorder, const int& index)
    {
        CaretAssert(index >= 0 && index < theBorder->getNumberOfPoints());
        const SurfaceProjectionBarycentric* thisBary = theBorder->getPoint(index)->getBarycentricProjection();
        CaretAssert(thisBary != NULL && thisBary->isValid());
        int thisNode = thisBary->getNodeWithLargestWeight();
        CaretAssert(thisNode >= 0);
        return thisNode;
    }
    
    struct BorderRedrawInfo
    {
        int startpoint, endpoint;
    };
}

/**
 * Run the border optimization algorithm.
 *
 * @param inputData
 *     Input data for the algorithm.
 * @param statisticsInformationOut
 *     Output containing statistics information.
 * @param errorMessageOut
 *     Output containing error information when false returned.
 * @return 
 *     True if successful, else false.
 */
bool
BorderOptimizeExecutor::run(const InputData& inputData,
                            AString& statisticsInformationOut,
                            AString& errorMessageOut)
{
    AString stageString = "initializing";
    try
    {
        statisticsInformationOut.clear();
        errorMessageOut.clear();
        
        printInputs(inputData);
        
        SurfaceFile* computeSurf = inputData.m_surface;
        const MetricFile* correctedAreasMetric = inputData.m_vertexAreasMetricFile;
        int32_t numNodes = computeSurf->getNumberOfNodes();
        int numSelected = (int)inputData.m_nodesInsideROI.size();
        if (numSelected < 1)
        {
            errorMessageOut = "no nodes selected";
            return false;
        }
        vector<float> roiData(numNodes, 0.0f);
        vector<float> combinedGradData(numNodes, 0.0f);
        for (int i = 0; i < numSelected; ++i)
        {
            roiData[inputData.m_nodesInsideROI[i]] = 1.0f;
            combinedGradData[inputData.m_nodesInsideROI[i]] = 1.0f;//also initialize only the inside-roi parts of the gradient combining function, leaving the outside 0
        }
        int numBorders = (int)inputData.m_borders.size();
        vector<BorderRedrawInfo> myRedrawInfo(numBorders);
        const int PROGRESS_MAX = 100;
        const int SEGMENT_PROGRESS = 10;
        const int COMPUTE_PROGRESS = 55;
        const int HELPER_PROGRESS = 5;
        const int DRAW_PROGRESS = 10;
        const int STATISTICS_PROGRESS = 20;
        CaretAssert(SEGMENT_PROGRESS + COMPUTE_PROGRESS + HELPER_PROGRESS + DRAW_PROGRESS + STATISTICS_PROGRESS == PROGRESS_MAX);
        stageString = "border segment locating";
        for (int i = 0; i < numBorders; ++i)
        {//find pieces of border to redraw, before doing gradient, so it can error early
            Border* thisBorder = inputData.m_borders[i];
            EventProgressUpdate tempEvent(0, PROGRESS_MAX, (SEGMENT_PROGRESS * i) / numBorders,
                                        "finding in-roi segment of border '" + thisBorder->getName() + "'");
            EventManager::get()->sendEvent(&tempEvent);
            if (tempEvent.isCancelled())
            {
                errorMessageOut = "cancelled by user";
                return false;
            }
            int numPoints = thisBorder->getNumberOfPoints();
            int start, end;
            if (thisBorder->isClosed())
            {
                if (numPoints < 3)//one outside, two inside will work
                {
                    errorMessageOut = "Border '" + thisBorder->getName() + "' has too few points to adjust";
                    return false;
                }
                for (start = numPoints - 1; start >= 0; --start)//search backwards for a point outside the roi
                {
                    if (!(roiData[getBorderPointNode(thisBorder, start)] > 0.0f)) break;
                }
                if (start == -1)//no points outside ROI
                {
                    errorMessageOut = "Border '" + thisBorder->getName() + "' has no points outside the ROI";
                    return false;
                }
                int added;
                for (added = 1; added < numPoints; ++added)//search forward from it to find the point inside the roi - if the above loop had to loop, this will end on the first iteration
                {
                    int pointIndex = (start + added) % numPoints;//closed border requires mod arithmetic
                    if (roiData[getBorderPointNode(thisBorder, pointIndex)] > 0.0f) break;
                }
                if (added == numPoints)//no points inside ROI
                {//NOTE: if multipart borders are used, and passed borders don't check the parts for being in/out, then this name is not unique
                    errorMessageOut = "Border '" + thisBorder->getName() + "' has no points inside the ROI";
                    return false;
                }
                start = (start + added) % numPoints;
                for (added = 1; added < numPoints; ++added)
                {
                    int pointIndex = (start + added) % numPoints;
                    if (!(roiData[getBorderPointNode(thisBorder, pointIndex)] > 0.0f)) break;
                }
                CaretAssert(added != numPoints);//we have points inside and outside roi, this search should have stopped
                end = (start + added - 1) % numPoints;//we will check this for being equal to start later, first check for multiple sections
                for (; added < numPoints; ++added)
                {
                    int pointIndex = (start + added) % numPoints;
                    if (roiData[getBorderPointNode(thisBorder, pointIndex)] > 0.0f)
                    {
                        errorMessageOut = "Border '" + thisBorder->getName() + "' has multiple sections inside the ROI";
                        return false;
                    }
                }
                if (end == start)
                {
                    errorMessageOut = "Border '" + thisBorder->getName() + "' has only one point inside the ROI";
                    return false;
                }
                if (getBorderPointNode(thisBorder, start) == getBorderPointNode(thisBorder, end))
                {
                    errorMessageOut = "Border '" + thisBorder->getName() + "' enters and exits the ROI too close to the same vertex";
                    return false;
                }
            } else {//open border
                if (numPoints < 2)//two, both inside, will work
                {
                    errorMessageOut = "Border '" + thisBorder->getName() + "' has too few points to adjust";
                    return false;
                }
                for (start = 0; start < numPoints; ++start)//search for first point inside
                {
                    if (roiData[getBorderPointNode(thisBorder, start)] > 0.0f) break;
                }
                if (start == numPoints)
                {//NOTE: if multipart borders are used, and passed borders don't check the parts for being in/out, then this name is not unique
                    errorMessageOut = "Border '" + thisBorder->getName() + "' has no points inside the ROI";
                    return false;
                }
                for (end = start + 1; end < numPoints; ++end)//search for first point outside
                {
                    if (!(roiData[getBorderPointNode(thisBorder, end)] > 0.0f)) break;
                }
                --end;//and subtract one to get inclusive range, also deals with == numPoints
                for (int check = end + 2; check < numPoints; ++check)//look for a second piece inside
                {
                    if (roiData[getBorderPointNode(thisBorder, check)] > 0.0f)
                    {
                        errorMessageOut = "Border '" + thisBorder->getName() + "' has multiple sections inside the ROI";
                        return false;
                    }
                }
            }
            myRedrawInfo[i].startpoint = start;
            myRedrawInfo[i].endpoint = end;
        }
        stageString = "data processing";
        int numInputs = (int)inputData.m_dataFileInfo.size();
        vector<float> inputRoiData(numNodes, 0.0f);
        for (int i = 0; i < (int)inputData.m_nodesInsideROI.size(); ++i)
        {
            inputRoiData[inputData.m_nodesInsideROI[i]] = 1.0f;
        }
        MetricFile inputRoi, dilatedRoi;
        inputRoi.setNumberOfNodesAndColumns(numNodes, 1);
        inputRoi.setValuesForColumn(0, inputRoiData.data());
        AlgorithmMetricDilate(NULL, &inputRoi, computeSurf, 0.0001f, &dilatedRoi);//dilate roi by 1 neighbor
        for (int i = 0; i < numInputs; ++i)
        {
            EventProgressUpdate tempEvent(0, PROGRESS_MAX, SEGMENT_PROGRESS + (COMPUTE_PROGRESS * i) / numInputs,
                                        "processing data file '" + inputData.m_dataFileInfo[i].m_mapFile->getFileNameNoPath() + "'");
            EventManager::get()->sendEvent(&tempEvent);
            if (tempEvent.isCancelled())
            {
                errorMessageOut = "cancelled by user";
                return false;
            }
            MetricFile tempGradient;
            if (inputData.m_dataFileInfo[i].m_allMapsFlag)
            {
                for (int j = 0; j < inputData.m_dataFileInfo[i].m_mapFile->getNumberOfMaps(); ++j)
                {
                    if (extractGradientData(inputData.m_dataFileInfo[i].m_mapFile, j, computeSurf, &dilatedRoi,
                                            inputData.m_dataFileInfo[i].m_smoothing, correctedAreasMetric, tempGradient,
                                            inputData.m_dataFileInfo[i].m_skipGradient, inputData.m_dataFileInfo[i].m_corrGradExcludeDist))
                    {
                        doCombination(tempGradient, inputData.m_nodesInsideROI, inputData.m_dataFileInfo[i].m_invertGradientFlag,
                                    inputData.m_dataFileInfo[i].m_weight, combinedGradData);
                    }
                }
            } else {
                if (extractGradientData(inputData.m_dataFileInfo[i].m_mapFile, inputData.m_dataFileInfo[i].m_mapIndex, computeSurf,
                                        &dilatedRoi, inputData.m_dataFileInfo[i].m_smoothing, correctedAreasMetric, tempGradient,
                                        inputData.m_dataFileInfo[i].m_skipGradient, inputData.m_dataFileInfo[i].m_corrGradExcludeDist))
                {
                    doCombination(tempGradient, inputData.m_nodesInsideROI, inputData.m_dataFileInfo[i].m_invertGradientFlag,
                                inputData.m_dataFileInfo[i].m_weight, combinedGradData);
                }
            }
        }
        stageString = "border drawing";
        if (inputData.m_combinedGradientDataOut != NULL)
        {
            inputData.m_combinedGradientDataOut->setNumberOfNodesAndColumns(numNodes, 1);
            inputData.m_combinedGradientDataOut->setStructure(computeSurf->getStructure());
            inputData.m_combinedGradientDataOut->setValuesForColumn(0, combinedGradData.data());
        }
        SurfaceFile* drawSurf = computeSurf, *origSphere = inputData.m_upsamplingSphericalSurface, highresSphere, highresMidthick;
        vector<float> drawGrad = combinedGradData, drawRoi = roiData;
        vector<float> origAreasStore, highresAreasStore;
        const float* origAreas = NULL;
        if (correctedAreasMetric != NULL)
        {
            origAreas = correctedAreasMetric->getValuePointerForColumn(0);
        }
        const float* drawAreas = origAreas;
        if (origSphere != NULL)
        {
            if (inputData.m_upsamplingSphericalSurface->getNumberOfNodes() >= inputData.m_upsamplingResolution)
            {
                errorMessageOut = "upsampling number of vertices must be greater than current vertex count";
                return false;
            }
            AlgorithmSurfaceCreateSphere(NULL, inputData.m_upsamplingResolution, &highresSphere);
            int highresNumNodes = highresSphere.getNumberOfNodes();
            highresSphere.setStructure(origSphere->getStructure());
            AlgorithmSurfaceResample(NULL, computeSurf, origSphere, &highresSphere, SurfaceResamplingMethodEnum::BARYCENTRIC, &highresMidthick);
            if (origAreas == NULL)
            {
                computeSurf->computeNodeAreas(origAreasStore);
                origAreas = origAreasStore.data();
                highresMidthick.computeNodeAreas(highresAreasStore);
            } else {
                vector<float> origRatioStore(origAreas, origAreas + numNodes), highresRatioStore(highresNumNodes), wrongAreas, highresWrongAreas;
                computeSurf->computeNodeAreas(wrongAreas);//to get high res corrected areas, convert to expansion ratio,
                highresMidthick.computeNodeAreas(highresWrongAreas);//then resample and multiply by the high res surface areas
                for (int i = 0; i < numNodes; ++i)
                {
                    origRatioStore[i] /= wrongAreas[i];
                }//we don't have anything high res but the wrong areas yet, so use like area measures - expansion ratio should be fairly smooth anyway, so not as important
                SurfaceResamplingHelper initialUpsampler(SurfaceResamplingMethodEnum::ADAP_BARY_AREA, origSphere, &highresSphere, wrongAreas.data(), highresWrongAreas.data());
                initialUpsampler.resampleNormal(origRatioStore.data(), highresRatioStore.data());
                highresAreasStore.resize(highresNumNodes);
                for (int i = 0; i < highresNumNodes; ++i)
                {
                    highresAreasStore[i] = highresRatioStore[i] * highresWrongAreas[i];
                }
            }
            drawSurf = &highresMidthick;
            drawAreas = highresAreasStore.data();
            drawGrad.resize(highresNumNodes);
            drawRoi.resize(highresNumNodes);
            SurfaceResamplingHelper finalUpsampler(SurfaceResamplingMethodEnum::ADAP_BARY_AREA, origSphere, &highresSphere, origAreas, highresAreasStore.data(), roiData.data());
            finalUpsampler.resampleNormal(combinedGradData.data(), drawGrad.data());
            finalUpsampler.getResampleValidROI(drawRoi.data());
        }
        {
            EventProgressUpdate tempEvent(0, PROGRESS_MAX, SEGMENT_PROGRESS + COMPUTE_PROGRESS,
                                        "generating geodesic helper");
            EventManager::get()->sendEvent(&tempEvent);
            if (tempEvent.isCancelled())
            {
                errorMessageOut = "cancelled by user";
                return false;
            }
        }
        CaretPointer<GeodesicHelper> myGeoHelp;
        CaretPointer<GeodesicHelperBase> myGeoBase;
        if (correctedAreasMetric != NULL)
        {
            myGeoBase.grabNew(new GeodesicHelperBase(drawSurf, drawAreas));
            myGeoHelp.grabNew(new GeodesicHelper(myGeoBase));
        } else {
            myGeoHelp = drawSurf->getGeodesicHelper();
        }
        CaretPointer<TopologyHelper> myTopoHelp = drawSurf->getTopologyHelper();
        vector<Border*> drawOrigBorders = inputData.m_borders;
        BorderFile highresOrigBorders;
        if (origSphere != NULL)
        {//first, copy all borders and resample to get endpoint positions on high res mesh, then draw new segments as borders and downsample just the segments
            BorderFile origBorders;
            for (int i = 0; i < numBorders; ++i)
            {
                origBorders.addBorder(new Border(*(inputData.m_borders[i])));
            }
            AlgorithmBorderResample(NULL, &origBorders, origSphere, &highresSphere, &highresOrigBorders);//NOTE: this must keep each border and point intact and in the same order, just on the new sphere
            for (int i = 0; i < numBorders; ++i)
            {
                drawOrigBorders[i] = highresOrigBorders.getBorder(i);
            }
        }
        vector<float> roiMinusTracesData = drawRoi;
        BorderFile redrawnSegments;
        for (int i = 0; i < numBorders; ++i)
        {
            EventProgressUpdate tempEvent(0, PROGRESS_MAX, SEGMENT_PROGRESS + COMPUTE_PROGRESS + HELPER_PROGRESS + (DRAW_PROGRESS * i) / numBorders,
                                        "redrawing segment of border '" + inputData.m_borders[i]->getName() + "'");//use non-resampled borders for name, just in case
            EventManager::get()->sendEvent(&tempEvent);
            if (tempEvent.isCancelled())
            {
                errorMessageOut = "cancelled by user";
                return false;
            }
            vector<int32_t> nodes;
            vector<float> dists;
            myGeoHelp->getPathFollowingData(getBorderPointNode(drawOrigBorders[i], myRedrawInfo[i].startpoint), getBorderPointNode(drawOrigBorders[i], myRedrawInfo[i].endpoint),
                                            drawGrad.data(), nodes, dists, inputData.m_gradientFollowingStrength, drawRoi.data(), true, true);
            if (nodes.size() < 3)//require at least 1 surviving point after removing endpoints
            {
                errorMessageOut = "Unable to redraw border segment for border '" + inputData.m_borders[i]->getName() + "'";
                return false;
            }
            CaretPointer<Border> redrawnSegment(new Border());
            for (int j = 1; j < (int)nodes.size() - 1; ++j)//drop the closest node to the start and end points from the redrawn segment
            {
                const vector<int32_t>& nodeTiles = myTopoHelp->getNodeTiles(nodes[j]);
                CaretAssert(!nodeTiles.empty());
                const int32_t* tileNodes = drawSurf->getTriangle(nodeTiles[0]);
                int whichNode;
                for (whichNode = 0; whichNode < 3; ++whichNode)
                {
                    if (tileNodes[whichNode] == nodes[j]) break;
                }
                CaretAssert(whichNode < 3);//it should always find it
                float weights[3] = { 0.0f, 0.0f, 0.0f };
                weights[whichNode] = 1.0f;
                SurfaceProjectedItem* myItem = new SurfaceProjectedItem();
                myItem->getBarycentricProjection()->setTriangleNodes(tileNodes);//none of these should throw
                myItem->getBarycentricProjection()->setTriangleAreas(weights);
                myItem->getBarycentricProjection()->setValid(true);
                myItem->setStructure(computeSurf->getStructure());
                redrawnSegment->addPoint(myItem);
            }
            redrawnSegments.addBorder(redrawnSegment.releasePointer());
            int numOrigPoints = inputData.m_borders[i]->getNumberOfPoints();
            Border fullRedrawn = *(drawOrigBorders[i]);//use the potentially upsampled version for the ROI splitting, but don't re-downsample it as the modified border on the current mesh
            fullRedrawn.removeAllPoints();
            if (!(inputData.m_borders[i]->isClosed()))//if it is a closed border, start with the newly drawn section, for simplicity
            {
                for (int j = 0; j <= myRedrawInfo[i].startpoint; ++j)//include the original startpoint
                {
                    fullRedrawn.addPoint(new SurfaceProjectedItem(*(drawOrigBorders[i]->getPoint(j))));
                }
            }
            fullRedrawn.addPoints(redrawnSegment);
            if (inputData.m_borders[i]->isClosed())
            {//mod arithmetic for closed borders
                int numKeep = ((numOrigPoints + myRedrawInfo[i].startpoint - myRedrawInfo[i].endpoint) % numOrigPoints) + 1;//inclusive
                for (int j = 0; j < numKeep; ++j)
                {
                    fullRedrawn.addPoint(new SurfaceProjectedItem(*(drawOrigBorders[i]->getPoint((j + myRedrawInfo[i].endpoint) % numOrigPoints))));
                }
            } else {
                for (int j = myRedrawInfo[i].endpoint; j < numOrigPoints; ++j)//include original endpoint
                {
                    fullRedrawn.addPoint(new SurfaceProjectedItem(*(drawOrigBorders[i]->getPoint(j))));
                }
            }
            MetricFile borderTrace;
            BorderFile tempBorderFile;
            tempBorderFile.addBorder(new Border(fullRedrawn));//because it takes ownership of a pointer
            AlgorithmBorderToVertices(NULL, drawSurf, &tempBorderFile, &borderTrace);
            const float* traceData = borderTrace.getValuePointerForColumn(0);
            int drawNumNodes = drawSurf->getNumberOfNodes();
            for (int j = 0; j < drawNumNodes; ++j)
            {
                if (traceData[j] > 0.0f)
                {
                    roiMinusTracesData[j] = 0.0f;
                }
            }
        }
        BorderFile* segmentsToUse = &redrawnSegments;
        BorderFile downsampledSegments;
        if (origSphere != NULL)
        {
            AlgorithmBorderResample(NULL, &redrawnSegments, &highresSphere, origSphere, &downsampledSegments);
            segmentsToUse = &downsampledSegments;
        }
        vector<Border> modifiedBorders(numBorders);//modify all without replacing so we can error before changing any
        for (int i = 0; i < numBorders; ++i)
        {
            Border& modifiedBorder = modifiedBorders[i];
            modifiedBorder = *(inputData.m_borders[i]);
            int numOrigPoints = inputData.m_borders[i]->getNumberOfPoints();
            modifiedBorder.removeAllPoints();//keep name, color, class, etc
            if (!(inputData.m_borders[i]->isClosed()))//if it is a closed border, start with the newly drawn section, for simplicity
            {
                for (int j = 0; j <= myRedrawInfo[i].startpoint; ++j)//include the original startpoint
                {
                    modifiedBorder.addPoint(new SurfaceProjectedItem(*(inputData.m_borders[i]->getPoint(j))));
                }
            }
            modifiedBorder.addPoints(segmentsToUse->getBorder(i));
            if (inputData.m_borders[i]->isClosed())
            {//mod arithmetic for closed borders
                int numKeep = ((numOrigPoints + myRedrawInfo[i].startpoint - myRedrawInfo[i].endpoint) % numOrigPoints) + 1;//inclusive
                for (int j = 0; j < numKeep; ++j)
                {
                    modifiedBorder.addPoint(new SurfaceProjectedItem(*(inputData.m_borders[i]->getPoint((j + myRedrawInfo[i].endpoint) % numOrigPoints))));
                }
            } else {
                for (int j = myRedrawInfo[i].endpoint; j < numOrigPoints; ++j)//include original endpoint
                {
                    modifiedBorder.addPoint(new SurfaceProjectedItem(*(inputData.m_borders[i]->getPoint(j))));
                }
            }
        }
        stageString = "roi splitting";
        MetricFile roiMinusBorderTraces, roiMetric, drawAreasMetric;
        roiMinusBorderTraces.setNumberOfNodesAndColumns(roiMinusTracesData.size(), 1);
        roiMinusBorderTraces.setValuesForColumn(0, roiMinusTracesData.data());
        roiMetric.setNumberOfNodesAndColumns(drawRoi.size(), 1);
        roiMetric.setValuesForColumn(0, drawRoi.data());
        drawAreasMetric.setNumberOfNodesAndColumns(drawSurf->getNumberOfNodes(), 1);
        drawAreasMetric.setValuesForColumn(0, drawAreas);
        MetricFile clustersMetric;
        int endVal = 0;
        AlgorithmMetricFindClusters(NULL, drawSurf, &roiMinusBorderTraces, 0.5f, 10.0f, &clustersMetric, false, &roiMetric, &drawAreasMetric, 0, 1, &endVal);
        if (endVal != 3)
        {
            statisticsInformationOut = AString::number(endVal - 1) + " cluster(s) found after splitting the roi with the redrawn borders, skipping statistics";
        } else {
            const float* clusterData = clustersMetric.getValuePointerForColumn(0);
            vector<float> downsampledClusters;
            if (origSphere != NULL)
            {
                SurfaceResamplingHelper downsampler(SurfaceResamplingMethodEnum::ADAP_BARY_AREA, &highresSphere, origSphere, highresAreasStore.data(), origAreas, drawRoi.data());
                vector<int32_t> highresData(highresSphere.getNumberOfNodes()), downsamledData(numNodes);
                const float* highresClusters = clustersMetric.getValuePointerForColumn(0);
                for (int i = 0; i < (int)highresData.size(); ++i)
                {
                    highresData[i] = floor(highresClusters[i] + 0.5f);
                }
                downsampler.resamplePopular(highresData.data(), downsamledData.data());
                downsampledClusters.resize(numNodes);
                for (int i = 0; i < (int)downsamledData.size(); ++i)
                {
                    downsampledClusters[i] = downsamledData[i];
                }
                clusterData = downsampledClusters.data();
            }
            stageString = "statistics";
            vector<int32_t> nodeLists[2];
            for (int i = 0; i < numNodes; ++i)
            {
                int label = (int)floor(clusterData[i] + 0.5f);
                CaretAssert(label < 3);
                if (label > 0) nodeLists[label - 1].push_back(i);
            }
            vector<int32_t> orderedNodeLists[2];
            if (nodeLists[0].size() >= nodeLists[1].size())
            {
                orderedNodeLists[0] = nodeLists[0];
                orderedNodeLists[1] = nodeLists[1];
            } else {
                orderedNodeLists[0] = nodeLists[1];
                orderedNodeLists[1] = nodeLists[0];
            }
            if (inputData.m_borderPair.size() == 2)
            {
                vector<int32_t> insideBorderNodes;
                AlgorithmNodesInsideBorder(NULL, computeSurf, inputData.m_borderPair[0], false, insideBorderNodes);
                vector<char> insideLookup(numNodes, 0);
                for (int i = 0; i < (int)insideBorderNodes.size(); ++i)
                {
                    insideLookup[insideBorderNodes[i]] = 1;
                }
                int counts[2] = {0, 0};
                for (int whichList = 0; whichList < 2; ++whichList)
                {
                    for (int i = 0; i < (int)orderedNodeLists[whichList].size(); ++i)
                    {
                        if (insideLookup[orderedNodeLists[whichList][i]] != 0)
                        {
                            ++counts[whichList];
                        }
                    }
                }
                if (counts[0] >= counts[1])
                {
                    statisticsInformationOut += inputData.m_borderPair[0]->getName() + " n=" + AString::number(orderedNodeLists[0].size()) + ", " +
                                                inputData.m_borderPair[1]->getName() + " n=" + AString::number(orderedNodeLists[1].size()) + "\n\n";
                } else {
                    statisticsInformationOut += inputData.m_borderPair[1]->getName() + " n=" + AString::number(orderedNodeLists[0].size()) + ", " +
                                                inputData.m_borderPair[0]->getName() + " n=" + AString::number(orderedNodeLists[1].size()) + "\n\n";
                }
                if (orderedNodeLists[0].size() < 2 || orderedNodeLists[1].size() < 2)
                {
                    statisticsInformationOut += "roi pieces are too small for statistics";
                } else {
                    for (int i = 0; i < numInputs; ++i)
                    {
                        EventProgressUpdate tempEvent(0, PROGRESS_MAX, SEGMENT_PROGRESS + COMPUTE_PROGRESS + HELPER_PROGRESS + DRAW_PROGRESS + (STATISTICS_PROGRESS * i) / numInputs,
                                                    "computing statistics on file '" + inputData.m_dataFileInfo[i].m_mapFile->getFileNameNoPath() + "'");
                        EventManager::get()->sendEvent(&tempEvent);
                        if (tempEvent.isCancelled())
                        {
                            errorMessageOut = "cancelled by user";
                            return false;
                        }
                        if (inputData.m_dataFileInfo[i].m_allMapsFlag)
                        {
                            for (int j = 0; j < inputData.m_dataFileInfo[i].m_mapFile->getNumberOfMaps(); ++j)
                            {
                                AString statsOut;
                                if(getStatisticsString(inputData.m_dataFileInfo[i].m_mapFile, j, orderedNodeLists, *computeSurf, correctedAreasMetric, inputData.m_dataFileInfo[i].m_corrGradExcludeDist, statsOut))
                                {
                                    statisticsInformationOut += statsOut + ": " +
                                                                inputData.m_dataFileInfo[i].m_mapFile->getMapName(inputData.m_dataFileInfo[i].m_mapIndex) + ", " +
                                                                inputData.m_dataFileInfo[i].m_mapFile->getFileNameNoPath() + ", " +
                                                                FileInformation(inputData.m_dataFileInfo[i].m_mapFile->getFileName()).getLastDirectory() + "\n";
                                }
                            }
                            statisticsInformationOut += "\n";
                        } else {
                            AString statsOut;
                            if(getStatisticsString(inputData.m_dataFileInfo[i].m_mapFile, inputData.m_dataFileInfo[i].m_mapIndex, orderedNodeLists, *computeSurf, correctedAreasMetric, inputData.m_dataFileInfo[i].m_corrGradExcludeDist, statsOut))
                            {
                                statisticsInformationOut += statsOut + ": " +
                                                            inputData.m_dataFileInfo[i].m_mapFile->getMapName(inputData.m_dataFileInfo[i].m_mapIndex) + ", " +
                                                            inputData.m_dataFileInfo[i].m_mapFile->getFileNameNoPath() + ", " +
                                                            FileInformation(inputData.m_dataFileInfo[i].m_mapFile->getFileName()).getLastDirectory() + "\n\n";
                            }
                        }
                    }
                }
            }
        }
        stageString = "border replacing";
        for (int i = 0; i < numBorders; ++i)//now replace the borders with the modified ones
        {
            inputData.m_borders[i]->replacePointsWithUndoSaving(&modifiedBorders[i]);
        }

        if (inputData.m_saveResults)
        {
            saveResults(inputData, statisticsInformationOut);
        }
        
        /*
        * Modifying a border:
        *
        * (1) Make a copy of the border
        *
        *     Border* borderCopy = new Border(*border)
        *
        * (2) Modify the 'copied' border
        *
        * (3) When modification is complete, calling this method
        *     will first make an 'undo' copy of 'border' that is stored
        *     inside of border and then replace the points in 'border'
        *     with those from 'borderCopy'.   This will allow the
        *     user to press the Border ToolBar's 'Undo Finish' button
        *     if the changes are not acceptable.
        *
        *     border->replacePointsWithUndoSaving(borderCopy)
        */
        
    } catch (CaretException& e) {//all exceptions are errors
        errorMessageOut = "Caught exception during " + stageString + " stage: " + e.whatString();
        return false;
    }
    return true;
}

void BorderOptimizeExecutor::saveResults(const BorderOptimizeExecutor::InputData& inputData, const AString& statisticsInformation)
{
    AString completeBase = FileInformation(inputData.m_savingPath, inputData.m_savingBaseName).getAbsoluteFilePath();
    BorderFile tempOutBorder;
    tempOutBorder.setNumberOfNodes(inputData.m_surface->getNumberOfNodes());
    tempOutBorder.addBorder(new Border(*(inputData.m_borderEnclosingROI)));
    tempOutBorder.writeFile(completeBase + ".border");
    if (inputData.m_combinedGradientDataOut != NULL)
    {
        inputData.m_combinedGradientDataOut->writeFile(completeBase + ".func.gii");
    }
    TextFile textOut;//because QT's paths use non-native separators on windows
    textOut.replaceText(statisticsInformation);
    textOut.addLine("\n\nBorders chosen to optimize:");
    for (std::vector<Border*>::const_iterator bi = inputData.m_borders.begin(); bi != inputData.m_borders.end(); bi++)
    {
        textOut.addLine((*bi)->getName());
    }
    textOut.addLine("\nBorder pair for statistics:");
    for (std::vector<Border*>::const_iterator bi = inputData.m_borderPair.begin(); bi != inputData.m_borderPair.end(); bi++)
    {
        textOut.addLine((*bi)->getName());
    }
    textOut.addLine("\nOptimizing Surface: " + inputData.m_surface->getFileName());
    textOut.addLine("\nData Files used:");
    for (std::vector<DataFileInfo>::const_iterator fi = inputData.m_dataFileInfo.begin(); fi != inputData.m_dataFileInfo.end(); fi++)
    {
        const DataFileInfo& dfi = *fi;
        textOut.addLine("\n" + dfi.m_mapFile->getFileName());
        if (dfi.m_allMapsFlag) {
            textOut.addLine("    Map: All Maps");
        }
        else {
            textOut.addLine("    Map: " + AString::number(dfi.m_mapIndex) + ", " + dfi.m_mapFile->getMapName(dfi.m_mapIndex));
        }
        
        textOut.addLine("    Strength: " + AString::number(dfi.m_weight));
        textOut.addLine("    Smoothing: " + AString::number(dfi.m_smoothing));
        textOut.addLine("    Invert Gradient: " + AString::fromBool(dfi.m_invertGradientFlag));
    }
    if (inputData.m_vertexAreasMetricFile != NULL) textOut.addLine("\nVertex Areas Metric File:\n" + inputData.m_vertexAreasMetricFile->getFileName());
    if (inputData.m_upsamplingSphericalSurface != NULL) {
        textOut.addLine("\nUpsampling Sphere: " + inputData.m_upsamplingSphericalSurface->getFileName());
        textOut.addLine("Upsampling Resolution: " + AString::number(inputData.m_upsamplingResolution));
    }
    textOut.addLine("\nGradient Following Strength: " + AString::number(inputData.m_gradientFollowingStrength));
    textOut.writeFile(completeBase + ".txt");
}

/**
 * Run the border optimization algorithm.
 *
 * @param inputData
 *     Input data for the algorithm.
 */
void
BorderOptimizeExecutor::printInputs(const InputData& inputData)
{
    std::cout << "Optimizing borders: " << std::endl;
    for (std::vector<Border*>::const_iterator bi = inputData.m_borders.begin();
         bi != inputData.m_borders.end();
         bi++) {
        std::cout << "    " << qPrintable((*bi)->getName()) << std::endl;
    }
   
    std::cout << "Border pair: " << std::endl;
    for (std::vector<Border*>::const_iterator bi = inputData.m_borderPair.begin();
         bi != inputData.m_borderPair.end();
         bi++) {
        std::cout << "    " << qPrintable((*bi)->getName()) << std::endl;
    }
    
    std::cout << "Optimizing Surface: " << qPrintable(inputData.m_surface->getFileNameNoPath()) << std::endl;
    std::cout << "Number of nodes in ROI: " << qPrintable(AString::number(inputData.m_nodesInsideROI.size())) << std::endl;
    std::cout << "Optimizing Data Files: " << std::endl;
    for (std::vector<DataFileInfo>::const_iterator fi = inputData.m_dataFileInfo.begin();
         fi != inputData.m_dataFileInfo.end();
         fi++) {
        const DataFileInfo& dfi = *fi;
        std::cout << "    Name: " << qPrintable(dfi.m_mapFile->getFileNameNoPath()) << std::endl;
        if (dfi.m_allMapsFlag) {
            std::cout << "    Map: All Maps" << std::endl;
        }
        else {
            std::cout << "    Map: " << qPrintable(AString::number(dfi.m_mapIndex))
                  << "     " << qPrintable(dfi.m_mapFile->getMapName(dfi.m_mapIndex)) << std::endl;
        }
        
        std::cout << "    Strength: " << dfi.m_weight << std::endl;
        std::cout << "    Smoothing: " << dfi.m_smoothing << std::endl;
        std::cout << "    Invert Gradient: " << AString::fromBool(dfi.m_invertGradientFlag) << std::endl;
        std::cout << "    Skip Gradient: " << AString::fromBool(dfi.m_skipGradient) << std::endl;
    }
    std::cout << "Vertex Areas Metric File: " << ((inputData.m_vertexAreasMetricFile != NULL)
                                                  ? qPrintable(inputData.m_vertexAreasMetricFile->getFileNameNoPath())
                                                  : "NULL") << std::endl;
    if (inputData.m_upsamplingSphericalSurface != NULL) {
        std::cout << "Upsampling Sphere: " << qPrintable(inputData.m_upsamplingSphericalSurface->getFileNameNoPath()) << std::endl;
        std::cout << "Upsampling Resolution: " << qPrintable(QString::number(inputData.m_upsamplingResolution)) << std::endl;
    }
    std::cout << "Gradient Following Strength: " << inputData.m_gradientFollowingStrength << std::endl;
}
