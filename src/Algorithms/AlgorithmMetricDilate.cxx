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

#include "AlgorithmMetricDilate.h"
#include "AlgorithmException.h"

#include "CaretAssert.h"
#include "CaretOMP.h"
#include "FastStatistics.h"
#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "PaletteColorMapping.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "Vector3D.h"

#include <algorithm>
#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmMetricDilate::getCommandSwitch()
{
    return "-metric-dilate";
}

AString AlgorithmMetricDilate::getShortDescription()
{
    return "DILATE A METRIC FILE";
}

OperationParameters* AlgorithmMetricDilate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addMetricParameter(1, "metric", "the metric to dilate");
    
    ret->addSurfaceParameter(2, "surface", "the surface to compute on");
    
    ret->addDoubleParameter(3, "distance", "distance in mm to dilate");
    
    ret->addMetricOutputParameter(4, "metric-out", "the output metric");
    
    OptionalParameter* badRoiOpt = ret->createOptionalParameter(5, "-bad-vertex-roi", "specify an roi of vertices to overwrite, rather than vertices with value zero");
    badRoiOpt->addMetricParameter(1, "roi-metric", "metric file, positive values denote vertices to have their values replaced");
    
    OptionalParameter* dataRoiOpt = ret->createOptionalParameter(9, "-data-roi", "specify an roi of where there is data");
    dataRoiOpt->addMetricParameter(1, "roi-metric", "metric file, positive values denote vertices that have data");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(6, "-column", "select a single column to dilate");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    ret->createOptionalParameter(7, "-nearest", "use the nearest good value instead of a weighted average");
    
    ret->createOptionalParameter(10, "-linear", "fill in values with linear interpolation along strongest gradient");
    
    OptionalParameter* exponentOpt = ret->createOptionalParameter(8, "-exponent", "use a different exponent in the weighting function");
    exponentOpt->addDoubleParameter(1, "exponent", "exponent 'n' to use in (area / (distance ^ n)) as the weighting function (default 6)");
    
    OptionalParameter* corrAreaOpt = ret->createOptionalParameter(11, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    corrAreaOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    ret->createOptionalParameter(12, "-legacy-cutoff", "use the old method of choosing how many vertices to use when calulating the dilated value with weighted method");
        
    ret->setHelpText(
        AString("For all metric vertices that are designated as bad, if they neighbor a non-bad vertex with data or are within the specified distance of such a vertex, ") +
        "replace the value with a distance-based weighted average of nearby non-bad vertices that have data, otherwise set the value to zero.  " +
        "No matter how small <distance> is, dilation will always use at least the immediate neighbor vertices.  " +
        "If -nearest is specified, it will use the value from the closest non-bad vertex with data within range instead of a weighted average.\n\n" +
        "If -bad-vertex-roi is specified, all vertices with a positive ROI value are bad.  " +
        "If it is not specified, only vertices that have data, with a value of zero, are bad.  " +
        "If -data-roi is not specified, all vertices are assumed to have data.\n\n" +
        "Note that the -corrected-areas option uses an approximate correction for the change in distances along a group average surface.\n\n" +
        "To get the behavior of version 1.3.2 or earlier, use '-legacy-cutoff -exponent 2'."
    );
    return ret;
}

void AlgorithmMetricDilate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    MetricFile* myMetric = myParams->getMetric(1);
    SurfaceFile* mySurf = myParams->getSurface(2);
    float distance = (float)myParams->getDouble(3);
    MetricFile* myMetricOut = myParams->getOutputMetric(4);
    OptionalParameter* badRoiOpt = myParams->getOptionalParameter(5);
    MetricFile* badNodeRoi = NULL;
    if (badRoiOpt->m_present)
    {
        badNodeRoi = badRoiOpt->getMetric(1);
    }
    OptionalParameter* dataRoiOpt = myParams->getOptionalParameter(9);
    MetricFile* dataRoi = NULL;
    if (dataRoiOpt->m_present)
    {
        dataRoi = dataRoiOpt->getMetric(1);
    }
    OptionalParameter* columnSelect = myParams->getOptionalParameter(6);
    int columnNum = -1;
    if (columnSelect->m_present)
    {//set up to use the single column
        columnNum = (int)myMetric->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (columnNum < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    Method myMethod = WEIGHTED;
    bool methodSpecified = false;
    if (myParams->getOptionalParameter(7)->m_present)
    {
        methodSpecified = true;
        myMethod = NEAREST;
    }
    if (myParams->getOptionalParameter(10)->m_present)
    {
        if (methodSpecified) throw AlgorithmException("-nearest and -linear may not be specified together");
        methodSpecified = true;
        myMethod = LINEAR;
    }
    float exponent = 6.0f;
    OptionalParameter* exponentOpt = myParams->getOptionalParameter(8);
    if (exponentOpt->m_present)
    {
        exponent = (float)exponentOpt->getDouble(1);
    }
    OptionalParameter* corrAreaOpt = myParams->getOptionalParameter(11);
    MetricFile* corrAreas = NULL;
    if (corrAreaOpt->m_present)
    {
        corrAreas = corrAreaOpt->getMetric(1);
    }
    bool legacyCutoff = myParams->getOptionalParameter(12)->m_present;
    AlgorithmMetricDilate(myProgObj, myMetric, mySurf, distance, myMetricOut, badNodeRoi, dataRoi, columnNum, myMethod, exponent, corrAreas, legacyCutoff);
}

AlgorithmMetricDilate::AlgorithmMetricDilate(ProgressObject* myProgObj, const MetricFile* myMetric, const SurfaceFile* mySurf, const float& distance, MetricFile* myMetricOut,
                                             const MetricFile* badNodeRoi, const MetricFile* dataRoi, const int& columnNum,
                                             const Method& myMethod, const float& exponent, const MetricFile* corrAreas, const bool legacyCutoff) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = mySurf->getNumberOfNodes();
    if (numNodes != myMetric->getNumberOfNodes())
    {
        throw AlgorithmException("surface and metric number of vertices do not match");
    }
    if (badNodeRoi != NULL && badNodeRoi->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("bad vertex roi number of vertices does not match");
    }
    if (dataRoi != NULL && dataRoi->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("data roi number of vertices does not match");
    }
    if (corrAreas != NULL && corrAreas->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("corrected areas metric number of vertices does not match");
    }
    if (columnNum < -1 || columnNum >= myMetric->getNumberOfColumns())
    {
        throw AlgorithmException("invalid column specified");
    }
    if (distance < 0.0f)
    {
        throw AlgorithmException("distance cannot be negative");
    }
    myMetricOut->setStructure(mySurf->getStructure());
    vector<pair<int, StencilElem> > myStencils;//because we need to iterate over it in parallel
    vector<pair<int, int> > myNearest;
    vector<float> colScratch(numNodes);
    vector<float> myAreasData;
    const float* myAreas = NULL;
    if (corrAreas == NULL)
    {
        mySurf->computeNodeAreas(myAreasData);
        myAreas = myAreasData.data();
    } else {
        myAreas = corrAreas->getValuePointerForColumn(0);
    }
    bool linear = (myMethod == LINEAR), nearest = (myMethod == NEAREST);
    FastStatistics spacingStats;
    if (!linear && badNodeRoi != NULL)//if we know which nodes need to have their values replaced, then we can do the same thing at each vertex for each column
    {
        if (nearest)
        {
            precomputeNearest(myNearest, mySurf, badNodeRoi, dataRoi, corrAreas, distance);
        } else {
            precomputeStencils(myStencils, mySurf, myAreas, badNodeRoi, dataRoi, corrAreas, distance, exponent, legacyCutoff);
        }
    } else {
        mySurf->getNodesSpacingStatistics(spacingStats);//use mean spacing to help set minimum stencil distance, since native surfaces might have a minimum of 0
    }
    if (columnNum == -1)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, myMetric->getNumberOfColumns());
        for (int thisCol = 0; thisCol < myMetric->getNumberOfColumns(); ++thisCol)
        {
            *(myMetricOut->getMapPaletteColorMapping(thisCol)) = *(myMetric->getMapPaletteColorMapping(thisCol));
            const float* myInputData = myMetric->getValuePointerForColumn(thisCol);
            myMetricOut->setColumnName(thisCol, myMetric->getColumnName(thisCol));
            if (badNodeRoi == NULL)
            {
                processColumn(colScratch.data(), myInputData, mySurf, myAreas, badNodeRoi, dataRoi, corrAreas, distance, nearest, linear, exponent, legacyCutoff, spacingStats.getMean());
            } else {
                switch (myMethod)
                {
                    case NEAREST:
                        processColumn(colScratch.data(), numNodes, myInputData, myNearest);
                        break;
                    case WEIGHTED:
                        processColumn(colScratch.data(), numNodes, myInputData, myStencils);
                        break;
                    case LINEAR:
                        processColumn(colScratch.data(), myInputData, mySurf, myAreas, badNodeRoi, dataRoi, corrAreas, distance, nearest, linear, exponent, legacyCutoff, spacingStats.getMean());
                        break;
                }
            }
            myMetricOut->setValuesForColumn(thisCol, colScratch.data());
        }
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        *(myMetricOut->getMapPaletteColorMapping(0)) = *(myMetric->getMapPaletteColorMapping(columnNum));
        const float* myInputData = myMetric->getValuePointerForColumn(columnNum);
        myMetricOut->setColumnName(0, myMetric->getColumnName(columnNum));
        if (badNodeRoi == NULL)
        {
            processColumn(colScratch.data(), myInputData, mySurf, myAreas, badNodeRoi, dataRoi, corrAreas, distance, nearest, linear, exponent, legacyCutoff, spacingStats.getMean());
        } else {
            switch (myMethod)
            {
                case NEAREST:
                    processColumn(colScratch.data(), numNodes, myInputData, myNearest);
                    break;
                case WEIGHTED:
                    processColumn(colScratch.data(), numNodes, myInputData, myStencils);
                    break;
                case LINEAR:
                    processColumn(colScratch.data(), myInputData, mySurf, myAreas, badNodeRoi, dataRoi, corrAreas, distance, nearest, linear, exponent, legacyCutoff, spacingStats.getMean());
                    break;
            }
        }
        myMetricOut->setValuesForColumn(0, colScratch.data());
    }
}

void AlgorithmMetricDilate::processColumn(float* colScratch, const int& numNodes, const float* myInputData, vector<pair<int, int> > myNearest)
{
    for (int i = 0; i < numNodes; ++i)
    {
        colScratch[i] = myInputData[i];//precopy so that the parallel part doesn't have to worry about vertices that don't get dilated to
    }
    int numStencils = (int)myNearest.size();
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int i = 0; i < numStencils; ++i)//parallel may not matter here, but we do other stuff parallel, so...
    {
        const int& node = myNearest[i].first;
        const int& nearest = myNearest[i].second;
        if (nearest != -1)
        {
            colScratch[node] = myInputData[nearest];
        } else {
            colScratch[node] = 0.0f;
        }
    }
}

void AlgorithmMetricDilate::processColumn(float* colScratch, const int& numNodes, const float* myInputData, vector<pair<int, StencilElem> > myStencils)
{
    for (int i = 0; i < numNodes; ++i)
    {
        colScratch[i] = myInputData[i];//precopy so that the parallel part doesn't have to worry about vertices that don't get dilated to
    }
    int numStencils = (int)myStencils.size();
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int i = 0; i < numStencils; ++i)//ditto
    {
        const int& node = myStencils[i].first;
        const StencilElem& stencil = myStencils[i].second;
        int numWeights = (int)stencil.m_weightlist.size();
        if (numWeights > 0)
        {
            double accum = 0.0;
            for (int j = 0; j < numWeights; ++j)
            {
                accum += myInputData[stencil.m_weightlist[j].first] * stencil.m_weightlist[j].second;
            }
            colScratch[node] = accum / stencil.m_weightsum;
        } else {
            colScratch[node] = 0.0f;
        }
    }
}

void AlgorithmMetricDilate::processColumn(float* colScratch, const float* myInputData, const SurfaceFile* mySurf, const float* myAreas,
                                          const MetricFile* badNodeRoi, const MetricFile* dataRoi, const MetricFile* corrAreas,
                                          const float& distance, const bool& nearest, const bool& linear, const float& exponent, const bool legacyCutoff, const float meanSpacing)
{
    float cutoffBase = max(2.0f * distance, 2.0f * meanSpacing), cutoffRatio = max(1.1f, pow(49.0f, 1.0f / (exponent - 2.0f)));//find what ratio from closest vertex corresponds to having 98% of total weight accounted for on a plane, assuming non-adverse ROI
    float minKernel = 1.5f * meanSpacing;//small kernels are cheap for weighted, use similar minimum distance as volume dilate
    float legacyCutoffRatio = 1.5f, test = pow(10.0f, 1.0f / exponent);//old logic: find what cutoff ratio corresponds to a tenth of weight, but don't use more than a 1.5 * nearest cutoff
    if (test > 1.0f && test < legacyCutoffRatio)//if it is less than 1, the exponent is weird, so simply ignore it and use default
    {//this generally cut off early, causing the result to behave like a higher exponent was used
        if (test > 1.1f)
        {
            legacyCutoffRatio = test;
        } else {
            legacyCutoffRatio = 1.1f;
        }
    }
    int numNodes = mySurf->getNumberOfNodes();
    vector<char> charRoi(numNodes, 0);
    const float* badRoiData = NULL;
    if (badNodeRoi != NULL) badRoiData = badNodeRoi->getValuePointerForColumn(0);
    const float* dataRoiVals = NULL;
    if (dataRoi != NULL)
    {
        dataRoiVals = dataRoi->getValuePointerForColumn(0);
    }
    for (int i = 0; i < numNodes; ++i)
    {
        if (badRoiData == NULL)
        {
            if ((dataRoiVals == NULL || dataRoiVals[i] > 0.0f) && myInputData[i] != 0.0f)
            {
                charRoi[i] = 1;
            }
        } else {
            if ((dataRoiVals == NULL || dataRoiVals[i] > 0.0f) && !(badRoiData[i] > 0.0f))//"not greater than" to trap NaNs
            {
                charRoi[i] = 1;
            }
        }
    }
    CaretPointer<GeodesicHelperBase> correctedBase;
    if (corrAreas != NULL)
    {
        correctedBase.grabNew(new GeodesicHelperBase(mySurf, corrAreas->getValuePointerForColumn(0)));//NOTE: myAreas also points to this when applicable
    }
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
        CaretPointer<GeodesicHelper> myGeoHelp;
        if (corrAreas == NULL)
        {
            myGeoHelp = mySurf->getGeodesicHelper();
        } else {
            myGeoHelp.grabNew(new GeodesicHelper(correctedBase));
        }
#pragma omp CARET_FOR schedule(dynamic)
        for (int i = 0; i < numNodes; ++i)
        {
            bool badNode;
            if (badRoiData != NULL)
            {
                badNode = (badRoiData[i] > 0.0f);
            } else {
                badNode = (myInputData[i] == 0.0f);
            }
            if (badNode)
            {
                float closestDist;//NOTE: the only time this function is called with a badRoi is when using linear, which doesn't use the closest distance
                int closestNode = myGeoHelp->getClosestNodeInRoi(i, charRoi.data(), distance, closestDist);
                if (closestNode == -1)//check neighbors, to ensure we dilate by at least one node everywhere
                {
                    const vector<int32_t>& nodeList = myTopoHelp->getNodeNeighbors(i);
                    vector<float> distList;
                    myGeoHelp->getGeoToTheseNodes(i, nodeList, distList);//ok, its a little silly to do this
                    const int numInRange = (int)nodeList.size();
                    for (int j = 0; j < numInRange; ++j)
                    {
                        if (charRoi[nodeList[j]] != 0 && (closestNode == -1 || distList[j] < closestDist))
                        {
                            closestNode = nodeList[j];
                            closestDist = distList[j];
                        }
                    }
                }
                if (closestNode == -1)
                {
                    colScratch[i] = 0.0f;
                } else {
                    if (nearest)
                    {
                        colScratch[i] = myInputData[closestNode];
                    } else {
                        vector<int32_t> nodeList;
                        vector<float> distList;
                        if (linear)
                        {
                            myGeoHelp->getNodesToGeoDist(i, distance, nodeList, distList);
                            int numInRange = (int)nodeList.size();
                            Vector3D center = mySurf->getCoordinate(i);
                            vector<float> blockDists;
                            vector<int32_t> blockPath;
                            vector<int32_t> usableNodes;
                            vector<float> usableDists;
                            for (int j = 0; j < numInRange; ++j)//prescan what is usable, and also exclude things that are through a valid node
                            {
                                if (badRoiData != NULL)
                                {
                                    badNode = (badRoiData[nodeList[j]] > 0.0f);
                                } else {
                                    badNode = (myInputData[nodeList[j]] == 0.0f);
                                }
                                if ((dataRoiVals == NULL || dataRoiVals[nodeList[j]] > 0.0f) && !badNode)
                                {
                                    myGeoHelp->getPathAlongLineSegment(i, nodeList[j], center, mySurf->getCoordinate(nodeList[j]), blockPath, blockDists);
                                    CaretAssert(blockPath.size() > 0 && blockPath[0] == i);//we already know that i is "bad", skip it
                                    bool usable = true;
                                    for (int k = 1; k < (int)blockPath.size() - 1; ++k)//and don't test the endpoint
                                    {
                                        if (dataRoiVals == NULL || dataRoiVals[blockPath[k]] > 0.0f)
                                        {
                                            if (badRoiData != NULL)
                                            {
                                                if (!(badRoiData[blockPath[k]] > 0.0f))//"not greater than" to trap NaNs
                                                {
                                                    usable = false;
                                                    break;
                                                }
                                            } else {
                                                if (myInputData[blockPath[k]] != 0.0f)
                                                {
                                                    usable = false;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    if (usable)
                                    {
                                        usableNodes.push_back(nodeList[j]);
                                        usableDists.push_back(distList[j]);
                                    }
                                }
                            }
                            int numUsable = (int)usableNodes.size();
                            float bestGradient = -1.0f;
                            int bestj = -1, bestk = -1;
                            for (int j = 0; j < numUsable; ++j)
                            {
                                int node1 = usableNodes[j];
                                for (int k = j + 1; k < numUsable; ++k)
                                {
                                    int node2 = usableNodes[k];
                                    float grad = abs(myInputData[node1] - myInputData[node2]) / (usableDists[j] + usableDists[k]);
                                    if (grad > bestGradient)
                                    {
                                        bestGradient = grad;
                                        bestj = j;
                                        bestk = k;
                                    }
                                }
                            }
                            if (bestj == -1)
                            {
                                colScratch[i] = myInputData[closestNode];
                            } else {
                                int node1 = usableNodes[bestj], node2 = usableNodes[bestk];
                                colScratch[i] = myInputData[node1] + (myInputData[node2] - myInputData[node1]) * usableDists[bestj] / (usableDists[bestj] + usableDists[bestk]);
                            }
                        } else {
                            float cutoffDist = cutoffBase;
                            if (legacyCutoff)
                            {
                                cutoffDist = closestDist * legacyCutoffRatio;
                            } else {
                                if (exponent > 2.0f && cutoffRatio < 100.0f && cutoffRatio > 1.0f)//if the ratio is sane, use it, but never exceed cutoffBase
                                {
                                    cutoffDist = max(min(cutoffRatio * closestDist, cutoffDist), minKernel);//but small kernels are rather cheap anyway, so have a minimum size just in case
                                }
                            }
                            myGeoHelp->getNodesToGeoDist(i, cutoffDist, nodeList, distList);
                            int numInRange = (int)nodeList.size();
                            float totalWeight = 0.0f, weightedSum = 0.0f;
                            for (int j = 0; j < numInRange; ++j)
                            {
                                if (charRoi[nodeList[j]] != 0)
                                {
                                    float weight;
                                    const float tolerance = 0.9f;//distances should NEVER be less than closestDist, for obvious reasons
                                    float divdist = distList[j] / closestDist;
                                    if (divdist > tolerance)//tricky: if closestDist is zero, this filters between NaN and inf, resulting in a straight average between nodes with 0 distance
                                    {
                                        weight = myAreas[nodeList[j]] / pow(divdist, exponent);//NOTE: myAreas has already been pointed to the right data with -corrected-areas
                                    } else {
                                        weight = myAreas[nodeList[j]] / pow(tolerance, exponent);
                                    }
                                    totalWeight += weight;
                                    weightedSum += myInputData[nodeList[j]] * weight;
                                }
                            }
                            if (totalWeight != 0.0f)
                            {
                                colScratch[i] = weightedSum / totalWeight;
                            } else {
                                colScratch[i] = 0.0f;
                            }
                        }
                    }
                }
            } else {
                colScratch[i] = myInputData[i];
            }
        }
    }
}

void AlgorithmMetricDilate::precomputeStencils(vector<pair<int, StencilElem> >& myStencils, const SurfaceFile* mySurf, const float* myAreas,
                                               const MetricFile* badNodeRoi, const MetricFile* dataRoi, const MetricFile* corrAreas,
                                               const float& distance, const float& exponent, const bool legacyCutoff)
{
    CaretAssert(badNodeRoi != NULL);//because it should never be called if we don't know exactly what nodes we are replacing
    const float* badNodeData = badNodeRoi->getValuePointerForColumn(0);
    FastStatistics spacingStats;
    mySurf->getNodesSpacingStatistics(spacingStats);//use mean spacing to help set minimum stencil distance, since native surfaces might have a minimum of 0
    float cutoffBase = max(2.0f * distance, 2.0f * spacingStats.getMean()), cutoffRatio = max(1.1f, pow(49.0f, 1.0f / (exponent - 2.0f)));//find what ratio from closest vertex corresponds to having 98% of total weight accounted for on a plane, assuming non-adverse ROI
    float minKernel = 1.5f * spacingStats.getMean();//small kernels are cheap for weighted, use similar minimum distance as volume dilate
    float legacyCutoffRatio = 1.5f, test = pow(10.0f, 1.0f / exponent);//old logic: find what cutoff ratio corresponds to a tenth of weight, but don't use more than a 1.5 * nearest cutoff
    if (test > 1.0f && test < legacyCutoffRatio)//if it is less than 1, the exponent is weird, so simply ignore it and use default
    {//this generally cut off early, causing the result to behave like a higher exponent was used
        if (test > 1.1f)
        {
            legacyCutoffRatio = test;
        } else {
            legacyCutoffRatio = 1.1f;
        }
    }
    int numNodes = mySurf->getNumberOfNodes();
    vector<char> charRoi(numNodes, 0);
    const float* dataRoiVals = NULL;
    int badCount = 0;
    if (dataRoi != NULL)
    {
        dataRoiVals = dataRoi->getValuePointerForColumn(0);
    }
    for (int i = 0; i < numNodes; ++i)
    {
        if (badNodeData[i] > 0.0f)
        {
            ++badCount;
        } else {
            if (dataRoiVals == NULL || dataRoiVals[i] > 0.0f)
            {
                charRoi[i] = 1;
            }
        }
    }
    myStencils.resize(badCount);//initializes all stencils to have empty lists
    badCount = 0;
    CaretPointer<GeodesicHelperBase> correctedBase;
    if (corrAreas != NULL)
    {
        correctedBase.grabNew(new GeodesicHelperBase(mySurf, corrAreas->getValuePointerForColumn(0)));//NOTE: myAreas also points to this when applicable
    }
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
        CaretPointer<GeodesicHelper> myGeoHelp;
        if (corrAreas == NULL)
        {
            myGeoHelp = mySurf->getGeodesicHelper();
        } else {
            myGeoHelp.grabNew(new GeodesicHelper(correctedBase));
        }
#pragma omp CARET_FOR schedule(dynamic)
        for (int i = 0; i < numNodes; ++i)
        {
            if (badNodeData[i] > 0.0f)
            {
                int myIndex;
#pragma omp critical
                {
                    myIndex = badCount;
                    ++badCount;
                }
                myStencils[myIndex].first = i;
                StencilElem& myElem = myStencils[myIndex].second;
                float closestDist;
                int closestNode = myGeoHelp->getClosestNodeInRoi(i, charRoi.data(), distance, closestDist);
                if (closestNode == -1)//check neighbors, to ensure we dilate by at least one node everywhere
                {
                    const vector<int32_t>& nodeList = myTopoHelp->getNodeNeighbors(i);
                    vector<float> distList;
                    myGeoHelp->getGeoToTheseNodes(i, nodeList, distList);//ok, its a little silly to do this
                    const int numInRange = (int)nodeList.size();
                    for (int j = 0; j < numInRange; ++j)
                    {
                        if (charRoi[nodeList[j]] != 0 && (closestNode == -1 || distList[j] < closestDist))
                        {
                            closestNode = nodeList[j];
                            closestDist = distList[j];
                        }
                    }
                }
                if (closestNode != -1)
                {
                    vector<int32_t> nodeList;
                    vector<float> distList;
                    float cutoffDist = cutoffBase;
                    if (legacyCutoff)
                    {
                        cutoffDist = closestDist * legacyCutoffRatio;
                    } else {
                        if (exponent > 2.0f && cutoffRatio < 100.0f && cutoffRatio > 1.0f)//if the ratio is sane, use it, but never exceed cutoffBase
                        {
                            cutoffDist = max(min(cutoffRatio * closestDist, cutoffDist), minKernel);//but small kernels are rather cheap anyway, so have a minimum size just in case
                        }
                    }
                    myGeoHelp->getNodesToGeoDist(i, cutoffDist, nodeList, distList);
                    int numInRange = (int)nodeList.size();
                    myElem.m_weightsum = 0.0f;
                    for (int j = 0; j < numInRange; ++j)
                    {
                        if (charRoi[nodeList[j]] != 0)
                        {
                            float weight;
                            const float tolerance = 0.9f;//distances should NEVER be less than closestDist, for obvious reasons
                            float divdist = distList[j] / closestDist;
                            if (divdist > tolerance)//tricky: if closestDist is zero, this filters between NaN and inf, resulting in a straight average between nodes with 0 distance
                            {
                                weight = myAreas[nodeList[j]] / pow(divdist, exponent);//NOTE: myAreas has already been pointed to the right data with -corrected-areas
                            } else {
                                weight = myAreas[nodeList[j]] / pow(tolerance, exponent);
                            }
                            myElem.m_weightsum += weight;
                            myElem.m_weightlist.push_back(pair<int, float>(nodeList[j], weight));
                        }
                    }
                    if (myElem.m_weightsum == 0.0f)//set list to empty instead of making NaNs
                    {
                        myElem.m_weightlist.clear();
                    }
                }
            }
        }
    }
}

void AlgorithmMetricDilate::precomputeNearest(vector<pair<int, int> >& myNearest, const SurfaceFile* mySurf,
                                              const MetricFile* badNodeRoi, const MetricFile* dataRoi, const MetricFile* corrAreas, const float& distance)
{
    CaretAssert(badNodeRoi != NULL);//because it should never be called if we don't know exactly what nodes we are replacing
    const float* badNodeData = badNodeRoi->getValuePointerForColumn(0);
    int numNodes = mySurf->getNumberOfNodes();
    vector<char> charRoi(numNodes, 0);
    const float* dataRoiVals = NULL;
    int badCount = 0;
    if (dataRoi != NULL)
    {
        dataRoiVals = dataRoi->getValuePointerForColumn(0);
    }
    for (int i = 0; i < numNodes; ++i)
    {
        if (badNodeData[i] > 0.0f)
        {
            ++badCount;
        } else {
            if (dataRoiVals == NULL || dataRoiVals[i] > 0.0f)
            {
                charRoi[i] = 1;
            }
        }
    }
    myNearest.resize(badCount);
    badCount = 0;
    CaretPointer<GeodesicHelperBase> correctedBase;
    if (corrAreas != NULL)
    {
        correctedBase.grabNew(new GeodesicHelperBase(mySurf, corrAreas->getValuePointerForColumn(0)));//NOTE: myAreas also points to this when applicable
    }
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
        CaretPointer<GeodesicHelper> myGeoHelp;
        if (corrAreas == NULL)
        {
            myGeoHelp = mySurf->getGeodesicHelper();
        } else {
            myGeoHelp.grabNew(new GeodesicHelper(correctedBase));
        }
#pragma omp CARET_FOR schedule(dynamic)
        for (int i = 0; i < numNodes; ++i)
        {
            if (badNodeData[i] > 0.0f)
            {
                int myIndex;
#pragma omp critical
                {
                    myIndex = badCount;
                    ++badCount;
                }
                myNearest[myIndex].first = i;
                float closestDist;
                int closestNode = myGeoHelp->getClosestNodeInRoi(i, charRoi.data(), distance, closestDist);
                if (closestNode == -1)//check neighbors, to ensure we dilate by at least one node everywhere
                {
                    const vector<int32_t>& nodeList = myTopoHelp->getNodeNeighbors(i);
                    vector<float> distList;
                    myGeoHelp->getGeoToTheseNodes(i, nodeList, distList);//ok, its a little silly to do this
                    const int numInRange = (int)nodeList.size();
                    for (int j = 0; j < numInRange; ++j)
                    {
                        if (charRoi[nodeList[j]] != 0 && (closestNode == -1 || distList[j] < closestDist))
                        {
                            closestNode = nodeList[j];
                            closestDist = distList[j];
                        }
                    }
                }
                myNearest[myIndex].second = closestNode;
            }
        }
    }
}

float AlgorithmMetricDilate::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricDilate::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
