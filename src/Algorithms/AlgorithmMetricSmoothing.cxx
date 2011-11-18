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

#include "AlgorithmMetricSmoothing.h"
#include "AlgorithmException.h"
#include "SurfaceFile.h"
#include "MetricFile.h"
#include "GeodesicHelper.h"
#include "TopologyHelper.h"
#include "CaretOMP.h"
#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmMetricSmoothing::getCommandSwitch()
{
    return "-metric-smoothing";
}

AString AlgorithmMetricSmoothing::getShortDescription()
{
    return "SMOOTH A METRIC FILE";
}

OperationParameters* AlgorithmMetricSmoothing::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to smooth on");
    ret->addMetricParameter(2, "metric-in", "the metric to smooth");
    ret->addDoubleParameter(3, "smoothing-kernel", "the sigma for the gaussian kernel function, in mm");
    ret->addMetricOutputParameter(4, "metric-out", "the output metric");
    OptionalParameter* roiOption = ret->createOptionalParameter(5, "-roi", "select a region of interest to smooth");
    roiOption->addMetricParameter(6, "roi-metric", "the roi to smooth within, as a metric");
    OptionalParameter* columnSelect = ret->createOptionalParameter(7, "-column", "select a single column to smooth");
    columnSelect->addIntegerParameter(8, "column-number", "the column number to smooth");
    ret->setHelpText(
        AString("Smooth a metric file on a surface.  By default, smooths all input columns on the entire surface, specify -column to smooth ") +
        "only one column, and -roi to smooth only one region, outputting zeros elsewhere.  When using -roi, input data outside the ROI is not used " +
        "to compute the smoothed values."
    );
    return ret;
}

void AlgorithmMetricSmoothing::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetric = myParams->getMetric(2);
    double myKernel = myParams->getDouble(3);
    MetricFile* myMetricOut = myParams->getOutputMetric(4);
    MetricFile* myRoi = NULL;
    OptionalParameter* roiOption = myParams->getOptionalParameter(5);
    if (roiOption->m_present)
    {
        myRoi = roiOption->getMetric(6);
    }
    int64_t columnNum = -1;
    OptionalParameter* columnSelect = myParams->getOptionalParameter(7);
    if (columnSelect->m_present)
    {
        columnNum = columnSelect->getInteger(8);//todo: subtract one for 1-based conventions?
    }
    AlgorithmMetricSmoothing(myProgObj, mySurf, myMetric, myMetricOut, myKernel, myRoi, columnNum);
}

AlgorithmMetricSmoothing::AlgorithmMetricSmoothing(ProgressObject* myProgObj, SurfaceFile* mySurf, MetricFile* myMetric, MetricFile* myMetricOut, double myKernel, MetricFile* myRoi, int64_t columnNum) : AbstractAlgorithm(myProgObj)
{
    const float precomputeWeightWork = 0.1f;//maybe should be a member variable?
    LevelProgress myProgress(myProgObj, 1.0f + precomputeWeightWork);
    int32_t numNodes = mySurf->getNumberOfNodes();
    if (numNodes != myMetric->getNumberOfNodes())
    {
        throw AlgorithmException("metric does not match surface in number of nodes");
    }
    if (myRoi != NULL && myRoi->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("roi metric does not match surface in number of nodes");
    }
    int32_t numCols = myMetric->getNumberOfColumns();
    if (columnNum < -1 || columnNum >= numCols)
    {
        throw AlgorithmException("invalid column number");
    }
    myProgress.setTask("Precomputing Smoothing Weights");
    if (myRoi == NULL)
    {
        precomputeWeights(mySurf, myKernel);
    } else {
        precomputeWeightsROI(mySurf, myKernel, myRoi);
    }
    myProgress.reportProgress(precomputeWeightWork);
    if (columnNum == -1)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, myMetric->getNumberOfColumns());
        myMetricOut->setStructure(mySurf->getStructure());
        const float* myRoiColumn;
        if (myRoi != NULL)
        {
            myRoiColumn = myRoi->getValuePointerForColumn(0);
        }
        float* myScratch = new float[numNodes];
        for (int32_t col = 0; col < numCols; ++col)
        {
            myProgress.setTask("Smoothing Column " + AString::number(col));
            myMetricOut->setColumnName(col, myMetric->getColumnName(col) + ", smooth " + AString::number(myKernel));
            const float* myColumn = myMetric->getValuePointerForColumn(col);
#pragma omp CARET_PARFOR
            for (int32_t i = 0; i < numNodes; ++i)
            {
                if (myRoi == NULL || myRoiColumn[i] > 0.0f)
                {
                    float sum = 0.0f;
                    WeightList& myWeightRef = m_weightLists[i];
                    int32_t numWeights = myWeightRef.m_nodes.size();
                    for (int32_t j = 0; j < numWeights; ++j)
                    {
                        sum += myWeightRef.m_weights[j] * myColumn[myWeightRef.m_nodes[j]];
                    }
                    myScratch[i] = sum / myWeightRef.m_weightSum;
                } else {
                    myScratch[i] = 0.0f;//zero other stuff
                }
            }
            myMetricOut->setValuesForColumn(col, myScratch);
            myProgress.reportProgress(precomputeWeightWork + ((float)col + 1) / numCols);
        }
        delete[] myScratch;
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        myMetricOut->setStructure(mySurf->getStructure());
        myMetricOut->setColumnName(0, myMetric->getColumnName(columnNum) + ", smooth " + AString::number(myKernel));
        myProgress.setTask("Smoothing Column " + AString::number(columnNum));
        const float* myColumn = myMetric->getValuePointerForColumn(columnNum);
        const float* myRoiColumn;
        float* myScratch = new float[numNodes];
        if (myRoi != NULL)
        {
            myRoiColumn = myRoi->getValuePointerForColumn(0);
        }
#pragma omp CARET_PARFOR
        for (int32_t i = 0; i < numNodes; ++i)
        {
            if (myRoi == NULL || myRoiColumn[i] > 0.0f)
            {
                float sum = 0.0f;
                WeightList& myWeightRef = m_weightLists[i];
                int32_t numWeights = myWeightRef.m_nodes.size();
                for (int32_t j = 0; j < numWeights; ++j)
                {
                    sum += myWeightRef.m_weights[j] * myColumn[myWeightRef.m_nodes[j]];
                }
                myScratch[i] = sum / myWeightRef.m_weightSum;
            } else {
                myScratch[i] = 0.0f;//zero other stuff
            }
        }//should go incredibly fast, don't worry about progress for one column
        myMetricOut->setValuesForColumn(columnNum, myScratch);
        delete[] myScratch;
    }
}

void AlgorithmMetricSmoothing::precomputeWeights(SurfaceFile* mySurf, double myKernel)
{
    int32_t numNodes = mySurf->getNumberOfNodes();
    float myKernelF = myKernel;
    float myGeoDist = myKernelF * 3.0f;
    float gaussianDenom = -0.5f / myKernelF / myKernelF;
    m_weightLists.resize(numNodes);
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();//don't really need one per thread here, but good practice in case we want getNeighborsToDepth
        CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
        vector<float> distances;
#pragma omp CARET_FOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            myGeoHelp->getNodesToGeoDist(i, myGeoDist, m_weightLists[i].m_nodes, distances, true);
            if (distances.size() < 7)
            {
                myTopoHelp->getNodeNeighbors(i, m_weightLists[i].m_nodes);
                m_weightLists[i].m_nodes.push_back(i);
                myGeoHelp->getGeoToTheseNodes(i, m_weightLists[i].m_nodes, distances, true);
            }
            int32_t numNeigh = (int32_t)distances.size();
            m_weightLists[i].m_weights.resize(numNeigh);
            m_weightLists[i].m_weightSum = 0.0f;
            for (int32_t j = 0; j < numNeigh; ++j)
            {
                float weight = exp(distances[j] * distances[j] * gaussianDenom);//exp(- dist ^ 2 / (2 * sigma ^ 2))
                m_weightLists[i].m_weights[j] = weight;
                m_weightLists[i].m_weightSum += weight;
            }
        }
    }
}

void AlgorithmMetricSmoothing::precomputeWeightsROI(SurfaceFile* mySurf, double myKernel, MetricFile* theRoi)
{
    int32_t numNodes = mySurf->getNumberOfNodes();
    float myKernelF = myKernel;
    float myGeoDist = myKernelF * 3.0f;
    float gaussianDenom = -0.5f / myKernelF / myKernelF;
    m_weightLists.resize(numNodes);
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
        CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
        vector<float> distances;
        vector<int32_t> nodes;
        const float* myRoiColumn = theRoi->getValuePointerForColumn(0);
#pragma omp CARET_FOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            myGeoHelp->getNodesToGeoDist(i, myGeoDist, nodes, distances, true);
            if (distances.size() < 7)
            {
                myTopoHelp->getNodeNeighbors(i, nodes);
                nodes.push_back(i);
                myGeoHelp->getGeoToTheseNodes(i, nodes, distances, true);
            }
            int32_t numNeigh = (int32_t)distances.size();
            m_weightLists[i].m_weights.reserve(numNeigh);
            m_weightLists[i].m_nodes.reserve(numNeigh);
            m_weightLists[i].m_weightSum = 0.0f;
            for (int32_t j = 0; j < numNeigh; ++j)
            {
                if (myRoiColumn[nodes[j]] > 0.0f)
                {
                    float weight = exp(distances[j] * distances[j] * gaussianDenom);//exp(- dist ^ 2 / (2 * sigma ^ 2))
                    m_weightLists[i].m_weights.push_back(weight);
                    m_weightLists[i].m_nodes.push_back(nodes[j]);
                    m_weightLists[i].m_weightSum += weight;
                    
                }
            }
        }
    }
}

float AlgorithmMetricSmoothing::getAlgorithmInternalWeight()
{
    return 1.0f;
}

float AlgorithmMetricSmoothing::getSubAlgorithmWeight()
{
    return 0.0f;
}
