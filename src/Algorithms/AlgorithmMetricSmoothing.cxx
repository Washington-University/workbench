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
    roiOption->addMetricParameter(1, "roi-metric", "the roi to smooth within, as a metric");
    
    ret->createOptionalParameter(6, "-fix-zeros", "treat zero values as not being data");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(7, "-column", "select a single column to smooth");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    OptionalParameter* methodSelect = ret->createOptionalParameter(8, "-method", "select smoothing method, default GEO_GAUSS_AREA");
    methodSelect->addStringParameter(1, "method", "the name of the smoothing method");
    
    ret->setHelpText(
        AString("Smooth a metric file on a surface.  By default, smooths all input columns on the entire surface, specify -column to smooth ") +
        "only one column, and -roi to smooth only one region, outputting zeros elsewhere.  When using -roi, input data outside the ROI is not used " +
        "to compute the smoothed values.\n\n" + 
        "The -fix-zeros option causes the smoothing to not use an input value if it is zero, but still write a smoothed value to the vertex.  " +
        "This is useful for zeros that indicate lack of information, preventing them from pulling down the intensity of nearby vertices, while " +
        "giving the zero an extrapolated value.\n\n" +
        "Valid values for <method> are:\n\nGEO_GAUSS_AREA - uses a geodesic gaussian kernel, and normalizes based " +
        "on vertex area in order to work more reliably on irregular surfaces\n\nGEO_GAUSS - matches geodesic gaussian smoothing from caret5, but does " +
        "not take vertex areas into account"
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
        myRoi = roiOption->getMetric(1);
    }
    OptionalParameter* fixZerosOpt = myParams->getOptionalParameter(6);
    bool fixZeros = fixZerosOpt->m_present;
    int64_t columnNum = -1;
    OptionalParameter* columnSelect = myParams->getOptionalParameter(7);
    if (columnSelect->m_present)
    {
        columnNum = (int)myMetric->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (columnNum < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    Method myMethod = GEO_GAUSS_AREA;
    OptionalParameter* methodSelect = myParams->getOptionalParameter(8);
    if (methodSelect->m_present)
    {
        AString methodName = methodSelect->getString(1);
        if (methodName == "GEO_GAUSS_AREA")
        {
            myMethod = GEO_GAUSS_AREA;
        } else if (methodName == "GEO_GAUSS") {
            myMethod = GEO_GAUSS;
        } else {
            throw AlgorithmException("unknown smoothing method name");
        }
    }
    AlgorithmMetricSmoothing(myProgObj, mySurf, myMetric, myKernel, myMetricOut, myRoi, fixZeros, columnNum, myMethod);
}

AlgorithmMetricSmoothing::AlgorithmMetricSmoothing(ProgressObject* myProgObj, const SurfaceFile* mySurf, const MetricFile* myMetric, double myKernel, MetricFile* myMetricOut, const MetricFile* myRoi, bool fixZeros, int64_t columnNum, Method myMethod) : AbstractAlgorithm(myProgObj)
{
    const float precomputeWeightWork = 5.0f;//maybe should be a member variable?
    LevelProgress myProgress(myProgObj, 1.0f + precomputeWeightWork);
    int32_t numNodes = mySurf->getNumberOfNodes();
    if (numNodes != myMetric->getNumberOfNodes())
    {
        throw AlgorithmException("metric does not match surface in number of vertices");
    }
    if (myRoi != NULL && myRoi->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("roi metric does not match surface in number of vertices");
    }
    int32_t numCols = myMetric->getNumberOfColumns();
    if (columnNum < -1 || columnNum >= numCols)
    {
        throw AlgorithmException("invalid column number");
    }
    if (myKernel <= 0.0)
    {
        throw AlgorithmException("invalid kernel size");
    }
    myProgress.setTask("Precomputing Smoothing Weights");
    precomputeWeights(mySurf, myKernel, myRoi, myMethod);
    myProgress.reportProgress(precomputeWeightWork);
    if (columnNum == -1)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, myMetric->getNumberOfColumns());
        myMetricOut->setStructure(mySurf->getStructure());
        const float* myRoiColumn;
        if (myRoi != NULL)
        {
            myRoiColumn = myRoi->getValuePointerForColumn(0);
        } else {
            myRoiColumn = NULL;
        }
        float* myScratch = new float[numNodes];
        for (int32_t col = 0; col < numCols; ++col)
        {
            myProgress.setTask("Smoothing Column " + AString::number(col));
            myMetricOut->setColumnName(col, myMetric->getColumnName(col) + ", smooth " + AString::number(myKernel));
            const float* myColumn = myMetric->getValuePointerForColumn(col);
#pragma omp CARET_PARFOR schedule(dynamic)
            for (int32_t i = 0; i < numNodes; ++i)
            {
                if (myRoi == NULL || myRoiColumn[i] > 0.0f)
                {
                    if (fixZeros)
                    {
                        float weightSum = 0.0f, sum = 0.0f;
                        WeightList& myWeightRef = m_weightLists[i];
                        int32_t numWeights = myWeightRef.m_nodes.size();
                        for (int32_t j = 0; j < numWeights; ++j)
                        {
                            if (myColumn[myWeightRef.m_nodes[j]] != 0.0f)
                            {
                                sum += myWeightRef.m_weights[j] * myColumn[myWeightRef.m_nodes[j]];
                                weightSum += myWeightRef.m_weights[j];
                            }
                        }
                        if (weightSum != 0.0f)
                        {
                            myScratch[i] = sum / weightSum;
                        } else {
                            myScratch[i] = 0.0f;
                        }
                    } else {
                        WeightList& myWeightRef = m_weightLists[i];
                        if (myWeightRef.m_weightSum != 0.0f)
                        {
                            float sum = 0.0f;
                            int32_t numWeights = myWeightRef.m_nodes.size();
                            for (int32_t j = 0; j < numWeights; ++j)
                            {
                                sum += myWeightRef.m_weights[j] * myColumn[myWeightRef.m_nodes[j]];
                            }
                            myScratch[i] = sum / myWeightRef.m_weightSum;
                        } else {
                            myScratch[i] = 0.0f;
                        }
                    }
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
        } else {
            myRoiColumn = NULL;
        }
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            if (myRoi == NULL || myRoiColumn[i] > 0.0f)
            {
                if (fixZeros)
                {
                    float weightSum = 0.0f, sum = 0.0f;
                    WeightList& myWeightRef = m_weightLists[i];
                    int32_t numWeights = myWeightRef.m_nodes.size();
                    for (int32_t j = 0; j < numWeights; ++j)
                    {
                        if (myColumn[myWeightRef.m_nodes[j]] != 0.0f)
                        {
                            sum += myWeightRef.m_weights[j] * myColumn[myWeightRef.m_nodes[j]];
                            weightSum += myWeightRef.m_weights[j];
                        }
                    }
                    if (weightSum != 0.0f)
                    {
                        myScratch[i] = sum / weightSum;
                    } else {
                        myScratch[i] = 0.0f;
                    }
                } else {
                    WeightList& myWeightRef = m_weightLists[i];
                    if (myWeightRef.m_weightSum != 0.0f)
                    {
                        float sum = 0.0f;
                        int32_t numWeights = myWeightRef.m_nodes.size();
                        for (int32_t j = 0; j < numWeights; ++j)
                        {
                            sum += myWeightRef.m_weights[j] * myColumn[myWeightRef.m_nodes[j]];
                        }
                        myScratch[i] = sum / myWeightRef.m_weightSum;
                    } else {
                        myScratch[i] = 0.0f;
                    }
                }
            } else {
                myScratch[i] = 0.0f;//zero other stuff
            }
        }//should go incredibly fast, don't worry about progress for one column
        myMetricOut->setValuesForColumn(0, myScratch);
        delete[] myScratch;
    }
}

void AlgorithmMetricSmoothing::precomputeWeightsGeoGauss(const SurfaceFile* mySurf, double myKernel)
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
                m_weightLists[i].m_nodes = myTopoHelp->getNodeNeighbors(i);
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

void AlgorithmMetricSmoothing::precomputeWeightsROIGeoGauss(const SurfaceFile* mySurf, double myKernel, const MetricFile* theRoi)
{
    int32_t numNodes = mySurf->getNumberOfNodes();
    float myKernelF = myKernel;
    float myGeoDist = myKernelF * 3.0f;
    float gaussianDenom = -0.5f / myKernelF / myKernelF;
    m_weightLists.resize(numNodes);
    const float* myRoiColumn = theRoi->getValuePointerForColumn(0);
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
        CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
        vector<float> distances;
        vector<int32_t> nodes;
#pragma omp CARET_FOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            if (myRoiColumn[i] > 0.0f)
            {
                myGeoHelp->getNodesToGeoDist(i, myGeoDist, nodes, distances, true);
                if (distances.size() < 7)
                {
                    nodes = myTopoHelp->getNodeNeighbors(i);
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
}

void AlgorithmMetricSmoothing::precomputeWeightsGeoGaussArea(const SurfaceFile* mySurf, double myKernel)
{//this method is normalized in two ways to provide evenly diffusing smoothing with equivalent sum of areas * values as input
    int32_t numNodes = mySurf->getNumberOfNodes();
    float myKernelF = myKernel;
    float myGeoDist = myKernelF * 3.0f;
    float gaussianDenom = -0.5f / myKernelF / myKernelF;
    vector<float> nodeAreas;
    vector<WeightList> tempList;//this is used to compute scattering kernels because it is easier to normalize scattering kernels correctly, and then convert to gathering kernels
    tempList.resize(numNodes);
    mySurf->computeNodeAreas(nodeAreas);
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();//don't really need one per thread here, but good practice in case we want getNeighborsToDepth
        CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
        vector<float> distances;
#pragma omp CARET_FOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            myGeoHelp->getNodesToGeoDist(i, myGeoDist, tempList[i].m_nodes, distances, true);
            if (distances.size() < 7)
            {
                tempList[i].m_nodes = myTopoHelp->getNodeNeighbors(i);
                tempList[i].m_nodes.push_back(i);
                myGeoHelp->getGeoToTheseNodes(i, tempList[i].m_nodes, distances, true);
            }
            int32_t numNeigh = (int32_t)distances.size();
            tempList[i].m_weights.resize(numNeigh);
            tempList[i].m_weightSum = 0.0f;
            for (int32_t j = 0; j < numNeigh; ++j)
            {
                float weight = exp(distances[j] * distances[j] * gaussianDenom) * nodeAreas[tempList[i].m_nodes[j]];//exp(- dist ^ 2 / (2 * sigma ^ 2)) * area
                tempList[i].m_weights[j] = weight;//we multiply by area so that a node scattering to a dense region on one side and a sparse region on the other
                tempList[i].m_weightSum += weight;//gives similar areal influence to each direction rather than giving a more influence on the dense region (simply because nodes are more numerous)
            }
            float myFactor = nodeAreas[i] / tempList[i].m_weightSum;//make each scattering kernel sum to the area of the node it scatters from
            for (int32_t j = 0; j < numNeigh; ++j)
            {
                tempList[i].m_weights[j] *= myFactor;
            }
            tempList[i].m_weightSum = nodeAreas[i];
        }
    }
    m_weightLists.resize(numNodes);//now convert it to gathering kernels
    for (int32_t i = 0; i < numNodes; ++i)//sadly, this is VERY hard to parallelize in a manner that is efficient, since it needs random access modification
    {
        m_weightLists[i].m_weightSum = 0.0f;//memory initialization may not go much faster in parallel
    }
    for (int32_t i = 0; i < numNodes; ++i)//and this needs to push onto random vectors in the weight list
    {
        int32_t numNeigh = tempList[i].m_nodes.size();
        for (int32_t j = 0; j < numNeigh; ++j)
        {
            int32_t node = tempList[i].m_nodes[j];
            float weight = tempList[i].m_weights[j];
            m_weightLists[node].m_nodes.push_back(i);
            m_weightLists[node].m_weights.push_back(weight);
            m_weightLists[node].m_weightSum += weight;
        }
    }
}

void AlgorithmMetricSmoothing::precomputeWeightsROIGeoGaussArea(const SurfaceFile* mySurf, double myKernel, const MetricFile* theRoi)
{
    int32_t numNodes = mySurf->getNumberOfNodes();
    float myKernelF = myKernel;
    float myGeoDist = myKernelF * 3.0f;
    float gaussianDenom = -0.5f / myKernelF / myKernelF;
    vector<float> nodeAreas;
    vector<WeightList> tempList;//this is used to compute scattering kernels because it is easier to normalize scattering kernels correctly, and then convert to gathering kernels
    tempList.resize(numNodes);
    mySurf->computeNodeAreas(nodeAreas);
    const float* myRoiColumn = theRoi->getValuePointerForColumn(0);
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
        CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
        vector<float> distances;
        vector<int32_t> nodes;
#pragma omp CARET_FOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            if (myRoiColumn[i] > 0.0f)//we don't need to scatter from things outside the ROI
            {
                myGeoHelp->getNodesToGeoDist(i, myGeoDist, nodes, distances, true);
                if (distances.size() < 7)
                {
                    nodes = myTopoHelp->getNodeNeighbors(i);
                    nodes.push_back(i);
                    myGeoHelp->getGeoToTheseNodes(i, nodes, distances, true);
                }
                int32_t numNeigh = (int32_t)distances.size();
                tempList[i].m_weightSum = 0.0f;
                for (int32_t j = 0; j < numNeigh; ++j)
                {//but we DO need to compute scattering TO things outside the ROI, so that our normalization doesn't increase the in-ROI influence of edge nodes
                    float weight = exp(distances[j] * distances[j] * gaussianDenom) * nodeAreas[nodes[j]];//exp(- dist ^ 2 / (2 * sigma ^ 2)) * area
                    tempList[i].m_weightSum += weight;//add it to the total weight in order to normalize correctly
                    if (myRoiColumn[nodes[j]] > 0.0f)
                    {//BUT, don't add it to the list if it is outside the ROI
                        tempList[i].m_nodes.push_back(nodes[j]);
                        tempList[i].m_weights.push_back(weight);
                    }
                }
                float myFactor = nodeAreas[i] / tempList[i].m_weightSum;//make each scattering kernel sum to the area of the node it scatters from
                int32_t numUsed = (int32_t)tempList[i].m_nodes.size();
                for (int32_t j = 0; j < numUsed; ++j)
                {
                    tempList[i].m_weights[j] *= myFactor;
                }
                tempList[i].m_weightSum = 0.0f;//this is never actually used again, but make sure it is wrong in case anything tries to use it
            }
        }
    }
    m_weightLists.resize(numNodes);//now convert it to gathering kernels
    for (int32_t i = 0; i < numNodes; ++i)//sadly, this is VERY hard to parallelize in a manner that is efficient, since it needs random access modification
    {
        m_weightLists[i].m_weightSum = 0.0f;//memory initialization may not go much faster in parallel
    }
    for (int32_t i = 0; i < numNodes; ++i)//and this needs to push onto random vectors in the weight list
    {
        int32_t numNeigh = tempList[i].m_nodes.size();
        for (int32_t j = 0; j < numNeigh; ++j)
        {
            int32_t node = tempList[i].m_nodes[j];
            float weight = tempList[i].m_weights[j];
            m_weightLists[node].m_nodes.push_back(i);
            m_weightLists[node].m_weights.push_back(weight);
            m_weightLists[node].m_weightSum += weight;
        }
    }
}

void AlgorithmMetricSmoothing::precomputeWeights(const SurfaceFile* mySurf, double myKernel, const MetricFile* theRoi, Method myMethod)
{
    if (theRoi != NULL)
    {
        switch (myMethod)
        {
            case GEO_GAUSS_AREA:
                precomputeWeightsROIGeoGaussArea(mySurf, myKernel, theRoi);
                break;
            case GEO_GAUSS:
                precomputeWeightsROIGeoGauss(mySurf, myKernel, theRoi);
                break;
            default:
                throw AlgorithmException("unknown smoothing method specified");
        };
    } else {
        switch (myMethod)
        {
            case GEO_GAUSS_AREA:
                precomputeWeightsGeoGaussArea(mySurf, myKernel);
                break;
            case GEO_GAUSS:
                precomputeWeightsGeoGauss(mySurf, myKernel);
                break;
            default:
                throw AlgorithmException("unknown smoothing method specified");
        };
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
