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

#include "MetricSmoothingObject.h"

#include "CaretAssert.h"
#include "CaretException.h"
#include "SurfaceFile.h"
#include "MetricFile.h"
#include "GeodesicHelper.h"
#include "TopologyHelper.h"
#include "CaretOMP.h"
#include <cmath>

using namespace std;
using namespace caret;

MetricSmoothingObject::MetricSmoothingObject(const SurfaceFile* mySurf, const float& kernel, const MetricFile* myRoi, Method myMethod, const float* nodeAreas)
{
    CaretAssert(mySurf != NULL);
    if (myRoi != NULL && mySurf->getNumberOfNodes() != myRoi->getNumberOfNodes())
    {
        throw CaretException("roi number of nodes doesn't match the surface");
    }
    precomputeWeights(mySurf, kernel, myRoi, myMethod, nodeAreas);
}

void MetricSmoothingObject::smoothColumn(const MetricFile* metricIn, const int& whichColumn, MetricFile* columnOut, const MetricFile* roi, const bool& fixZeros) const
{
    CaretAssert(metricIn != NULL);
    CaretAssert(columnOut != NULL);
    if (metricIn->getNumberOfNodes() != (int32_t)m_weightLists.size())
    {
        throw CaretException("metric does not match surface number of nodes");
    }
    if (whichColumn < -1 || whichColumn >= metricIn->getNumberOfColumns())
    {
        throw CaretException("invalid column number");
    }
    if (columnOut->getNumberOfNodes() != (int32_t)m_weightLists.size() || columnOut->getNumberOfColumns() != 1)
    {
        columnOut->setNumberOfNodesAndColumns(m_weightLists.size(), 1);
    }
    vector<float> scratch(metricIn->getNumberOfNodes());
    if (roi != NULL)
    {
        if (roi->getNumberOfNodes() != (int32_t)m_weightLists.size())
        {
            throw CaretException("roi does not match surface number of nodes");
        }
        smoothColumnInternal(scratch.data(), metricIn, whichColumn, columnOut, 0, roi, 0, fixZeros);
    } else {
        smoothColumnInternal(scratch.data(), metricIn, whichColumn, columnOut, 0, fixZeros);
    }
}

void MetricSmoothingObject::smoothColumn(const MetricFile* metricIn, const int& whichColumn, MetricFile* metricOut, const int& whichOutColumn, const MetricFile* roi, const int& whichRoiColumn, const bool& fixZeros) const
{
    CaretAssert(metricIn != NULL);
    CaretAssert(metricOut != NULL);
    if (metricIn->getNumberOfNodes() != (int32_t)m_weightLists.size())
    {
        throw CaretException("metric does not match surface number of nodes");
    }
    if (metricOut->getNumberOfNodes() != (int32_t)m_weightLists.size())
    {
        throw CaretException("output metric does not match surface number of nodes");
    }
    if (roi != NULL && (roi->getNumberOfNodes() != (int32_t)m_weightLists.size()))
    {
        throw CaretException("roi does not match surface number of nodes");
    }
    if (whichColumn < -1 || whichColumn >= metricIn->getNumberOfColumns())
    {
        throw CaretException("invalid input column number");
    }
    if (whichOutColumn < -1 || whichOutColumn >= metricOut->getNumberOfColumns())
    {
        throw CaretException("invalid output column number");
    }
    if (roi != NULL && (whichRoiColumn < -1 || whichRoiColumn >= roi->getNumberOfColumns()))
    {
        throw CaretException("invalid input column number");
    }
    vector<float> scratch(metricIn->getNumberOfNodes());
    if (roi != NULL)
    {
        smoothColumnInternal(scratch.data(), metricIn, whichColumn, metricOut, whichOutColumn, roi, whichRoiColumn, fixZeros);
    } else {
        smoothColumnInternal(scratch.data(), metricIn, whichColumn, metricOut, whichOutColumn, fixZeros);
    }
}

void MetricSmoothingObject::smoothMetric(const MetricFile* metricIn, MetricFile* metricOut, const MetricFile* roi, const bool& fixZeros) const
{
    CaretAssert(metricIn != NULL);
    CaretAssert(metricOut != NULL);
    int32_t numCols = metricIn->getNumberOfColumns();
    if (metricIn->getNumberOfNodes() != (int32_t)m_weightLists.size())
    {
        throw CaretException("metric does not match surface number of nodes");
    }
    if (metricOut->getNumberOfNodes() != (int32_t)m_weightLists.size() || metricOut->getNumberOfColumns() != numCols)
    {
        metricOut->setNumberOfNodesAndColumns(m_weightLists.size(), numCols);
    }
    vector<float> scratch(metricIn->getNumberOfNodes());
    if (roi != NULL)
    {
        if (roi->getNumberOfNodes() != (int32_t)m_weightLists.size())
        {
            throw CaretException("roi does not match surface number of nodes");
        }
        for (int32_t i = 0; i < numCols; ++i)
        {
            smoothColumnInternal(scratch.data(), metricIn, i, metricOut, i, roi, 0, fixZeros);
        }
    } else {
        for (int32_t i = 0; i < numCols; ++i)
        {
            smoothColumnInternal(scratch.data(), metricIn, i, metricOut, i, fixZeros);
        }
    }
}

void MetricSmoothingObject::smoothColumnInternal(float* scratch, const MetricFile* metricIn, const int& whichColumn, MetricFile* metricOut, const int& whichOutColumn, const bool& fixZeros) const
{
    CaretAssert(metricIn != NULL);//asserts only, and only basic checks, these functions are private
    CaretAssert(metricOut != NULL);
    CaretAssert(scratch != NULL);
    CaretAssert(whichColumn >= 0 && whichColumn < metricIn->getNumberOfColumns());
    CaretAssert(whichOutColumn >= 0 && whichOutColumn < metricOut->getNumberOfColumns());
    const float* myColumn = metricIn->getValuePointerForColumn(whichColumn);
    int32_t numNodes = metricIn->getNumberOfNodes();
    if (fixZeros)//special case early to keep branching down
    {
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            const WeightList& myWeightRef = m_weightLists[i];
            if (myWeightRef.m_weightSum != 0.0f)//skip nodes with no neighbors quickly
            {
                float sum = 0.0f, weightsum = 0.0f;
                int32_t numWeights = myWeightRef.m_nodes.size();
                for (int32_t j = 0; j < numWeights; ++j)
                {
                    float value = myColumn[myWeightRef.m_nodes[j]];
                    if (value != 0.0f)
                    {
                        float weight = myWeightRef.m_weights[j];
                        sum += weight * value;
                        weightsum += weight;
                    }
                }
                if (weightsum != 0.0f)
                {
                    scratch[i] = sum / weightsum;
                } else {
                    scratch[i] = 0.0f;
                }
            } else {
                scratch[i] = 0.0f;//but we do need to zero what we skip, so a list of nodes to check may not help
            }
        }
    } else {
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            const WeightList& myWeightRef = m_weightLists[i];
            if (myWeightRef.m_weightSum != 0.0f)
            {
                float sum = 0.0f;
                int32_t numWeights = myWeightRef.m_nodes.size();
                for (int32_t j = 0; j < numWeights; ++j)
                {
                    sum += myWeightRef.m_weights[j] * myColumn[myWeightRef.m_nodes[j]];
                }
                scratch[i] = sum / myWeightRef.m_weightSum;
            } else {
                scratch[i] = 0.0f;
            }
        }
    }
    metricOut->setValuesForColumn(whichOutColumn, scratch);
}

void MetricSmoothingObject::smoothColumnInternal(float* scratch, const MetricFile* metricIn, const int& whichColumn, MetricFile* metricOut, const int& whichOutColumn, const MetricFile* roi, const int& whichRoiColumn, const bool& fixZeros) const
{
    CaretAssert(metricIn != NULL);//asserts only, and only basic checks, these functions are private
    CaretAssert(metricOut != NULL);
    CaretAssert(scratch != NULL);
    CaretAssert(roi != NULL);
    CaretAssert(whichColumn >= 0 && whichColumn < metricIn->getNumberOfColumns());
    CaretAssert(whichOutColumn >= 0 && whichOutColumn < metricOut->getNumberOfColumns());
    CaretAssert(whichRoiColumn >= 0 && whichRoiColumn < roi->getNumberOfColumns());
    const float* myColumn = metricIn->getValuePointerForColumn(whichColumn);
    const float* roiColumn = roi->getValuePointerForColumn(whichRoiColumn);
    int32_t numNodes = metricIn->getNumberOfNodes();
    if (fixZeros)//special case early to keep branching down
    {
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            const WeightList& myWeightRef = m_weightLists[i];
            if (roiColumn[i] > 0.0f && myWeightRef.m_weightSum != 0.0f)//skip nodes with no neighbors quickly
            {
                float sum = 0.0f, weightsum = 0.0f;
                int32_t numWeights = myWeightRef.m_nodes.size();
                for (int32_t j = 0; j < numWeights; ++j)
                {
                    int32_t neighbor = myWeightRef.m_nodes[j];
                    float value = myColumn[neighbor];
                    if (roiColumn[neighbor] > 0.0f && value != 0.0f)
                    {
                        float weight = myWeightRef.m_weights[j];
                        sum += weight * value;
                        weightsum += weight;
                    }
                }
                if (weightsum != 0.0f)
                {
                    scratch[i] = sum / weightsum;
                } else {
                    scratch[i] = 0.0f;
                }
            } else {
                scratch[i] = 0.0f;//but we do need to zero what we skip, so a list of nodes to check may not help
            }
        }
    } else {
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            const WeightList& myWeightRef = m_weightLists[i];
            if (roiColumn[i] > 0.0f && myWeightRef.m_weightSum != 0.0f)
            {
                float sum = 0.0f, weightsum = 0.0f;
                int32_t numWeights = myWeightRef.m_nodes.size();
                for (int32_t j = 0; j < numWeights; ++j)
                {
                    int32_t neighbor = myWeightRef.m_nodes[j];
                    if (roiColumn[neighbor] > 0.0f)
                    {
                        float weight = myWeightRef.m_weights[j];
                        sum += weight * myColumn[neighbor];
                        weightsum += weight;
                    }
                }
                if (weightsum != 0.0f)
                {
                    scratch[i] = sum / weightsum;
                } else {
                    scratch[i] = 0.0f;
                }
            } else {
                scratch[i] = 0.0f;
            }
        }
    }
    metricOut->setValuesForColumn(whichOutColumn, scratch);
}

void MetricSmoothingObject::precomputeWeightsGeoGauss(const SurfaceFile* mySurf, float myKernel, const float* nodeAreas)
{
    int32_t numNodes = mySurf->getNumberOfNodes();
    float myGeoDist = myKernel * 3.0f;
    float gaussianDenom = -0.5f / myKernel / myKernel;
    m_weightLists.resize(numNodes);
    CaretPointer<GeodesicHelperBase> myGeoBase(new GeodesicHelperBase(mySurf, nodeAreas));//NOTE: if these are equal to the surface's areas, then it does some extra operations, but gets the same answer
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();//don't really need one per thread here, but good practice in case we want getNeighborsToDepth
        CaretPointer<GeodesicHelper> myGeoHelp(new GeodesicHelper(myGeoBase));
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

void MetricSmoothingObject::precomputeWeightsROIGeoGauss(const SurfaceFile* mySurf, float myKernel, const MetricFile* theRoi, const float* nodeAreas)
{
    int32_t numNodes = mySurf->getNumberOfNodes();
    float myGeoDist = myKernel * 3.0f;
    float gaussianDenom = -0.5f / myKernel / myKernel;
    m_weightLists.resize(numNodes);
    const float* myRoiColumn = theRoi->getValuePointerForColumn(0);
    CaretPointer<GeodesicHelperBase> myGeoBase(new GeodesicHelperBase(mySurf, nodeAreas));//NOTE: if these are equal to the surface's areas, then it does some extra operations, but gets the same answer
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
        CaretPointer<GeodesicHelper> myGeoHelp(new GeodesicHelper(myGeoBase));
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

void MetricSmoothingObject::precomputeWeightsGeoGaussArea(const SurfaceFile* mySurf, float myKernel, const float* nodeAreas)
{//this method is normalized in two ways to provide evenly diffusing smoothing with equivalent sum of areas * values as input
    int32_t numNodes = mySurf->getNumberOfNodes();
    float myGeoDist = myKernel * 3.0f;
    float gaussianDenom = -0.5f / myKernel / myKernel;
    vector<WeightList> tempList;//this is used to compute scattering kernels because it is easier to normalize scattering kernels correctly, and then convert to gathering kernels
    tempList.resize(numNodes);
    CaretPointer<GeodesicHelperBase> myGeoBase(new GeodesicHelperBase(mySurf, nodeAreas));//NOTE: if these are equal to the surface's areas, then it does some extra operations, but gets the same answer
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();//don't really need one per thread here, but good practice in case we want getNeighborsToDepth
        CaretPointer<GeodesicHelper> myGeoHelp(new GeodesicHelper(myGeoBase));
        vector<float> distances;
#pragma omp CARET_FOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            myGeoHelp->getNodesToGeoDist(i, myGeoDist, tempList[i].m_nodes, distances, true);
            const vector<int32_t>& tempneighbors = myTopoHelp->getNodeNeighbors(i);
            if (distances.size() <= tempneighbors.size())//because neighbors doesn't include center, so if they are equal, geo is missing a neighbor
            {
                tempList[i].m_nodes = tempneighbors;
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
        size_t neighborCount = tempList[i].m_nodes.size();
        m_weightLists[i].m_nodes.reserve(neighborCount);//also preallocate the expected number of nodes (geodesic distance should be symmetric except for rounding errors, so it should usually be exact)
        m_weightLists[i].m_weights.reserve(neighborCount);
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

void MetricSmoothingObject::precomputeWeightsROIGeoGaussArea(const SurfaceFile* mySurf, float myKernel, const MetricFile* theRoi, const float* nodeAreas)
{
    int32_t numNodes = mySurf->getNumberOfNodes();
    float myGeoDist = myKernel * 3.0f;
    float gaussianDenom = -0.5f / myKernel / myKernel;
    vector<WeightList> tempList;//this is used to compute scattering kernels because it is easier to normalize scattering kernels correctly, and then convert to gathering kernels
    tempList.resize(numNodes);
    const float* myRoiColumn = theRoi->getValuePointerForColumn(0);
    CaretPointer<GeodesicHelperBase> myGeoBase(new GeodesicHelperBase(mySurf, nodeAreas));//NOTE: if these are equal to the surface's areas, then it does some extra operations, but gets the same answer
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
        CaretPointer<GeodesicHelper> myGeoHelp(new GeodesicHelper(myGeoBase));
        vector<float> distances;
        vector<int32_t> nodes;
#pragma omp CARET_FOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            if (myRoiColumn[i] > 0.0f)//we don't need to scatter from things outside the ROI
            {
                myGeoHelp->getNodesToGeoDist(i, myGeoDist, nodes, distances, true);
                const vector<int32_t>& tempneighbors = myTopoHelp->getNodeNeighbors(i);
                if (distances.size() <= tempneighbors.size())//because neighbors doesn't include center, so if they are equal, geo is missing a neighbor
                {
                    nodes = tempneighbors;
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
        size_t neighborCount = tempList[i].m_nodes.size();
        m_weightLists[i].m_nodes.reserve(neighborCount);//also preallocate the expected number of nodes, again, should be exact except for rounding errors in geodesic distance
        m_weightLists[i].m_weights.reserve(neighborCount);
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

void MetricSmoothingObject::precomputeWeightsGeoGaussEqual(const SurfaceFile* mySurf, float myKernel, const float* nodeAreas)
{//this method is normalized in two ways to provide evenly diffusing smoothing with equivalent sum of values as input - this special purpose smoothing is for things that should not be integrated across the surface
    int32_t numNodes = mySurf->getNumberOfNodes();
    float myGeoDist = myKernel * 3.0f;
    float gaussianDenom = -0.5f / myKernel / myKernel;
    vector<WeightList> tempList;//this is used to compute scattering kernels because it is easier to normalize scattering kernels correctly, and then convert to gathering kernels
    tempList.resize(numNodes);
    CaretPointer<GeodesicHelperBase> myGeoBase(new GeodesicHelperBase(mySurf, nodeAreas));//NOTE: if these are equal to the surface's areas, then it does some extra operations, but gets the same answer
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();//don't really need one per thread here, but good practice in case we want getNeighborsToDepth
        CaretPointer<GeodesicHelper> myGeoHelp(new GeodesicHelper(myGeoBase));
        vector<float> distances;
#pragma omp CARET_FOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            myGeoHelp->getNodesToGeoDist(i, myGeoDist, tempList[i].m_nodes, distances, true);
            const vector<int32_t>& tempneighbors = myTopoHelp->getNodeNeighbors(i);
            if (distances.size() <= tempneighbors.size())//because neighbors doesn't include center, so if they are equal, geo is missing a neighbor
            {
                tempList[i].m_nodes = tempneighbors;
                tempList[i].m_nodes.push_back(i);
                myGeoHelp->getGeoToTheseNodes(i, tempList[i].m_nodes, distances, true);
            }
            int32_t numNeigh = (int32_t)distances.size();
            tempList[i].m_weights.resize(numNeigh);
            tempList[i].m_weightSum = 0.0f;
            for (int32_t j = 0; j < numNeigh; ++j)
            {
                float weight = exp(distances[j] * distances[j] * gaussianDenom);//exp(- dist ^ 2 / (2 * sigma ^ 2))
                tempList[i].m_weights[j] = weight;//we multiply by area so that a node scattering to a dense region on one side and a sparse region on the other
                tempList[i].m_weightSum += weight;//gives similar areal influence to each direction rather than giving a more influence on the dense region (simply because nodes are more numerous)
            }
            float myFactor = 1.0f / tempList[i].m_weightSum;//make each scattering kernel sum to 1
            for (int32_t j = 0; j < numNeigh; ++j)
            {
                tempList[i].m_weights[j] *= myFactor;
            }
            tempList[i].m_weightSum = 1.0f;
        }
    }
    m_weightLists.resize(numNodes);//now convert it to gathering kernels
    for (int32_t i = 0; i < numNodes; ++i)//sadly, this is VERY hard to parallelize in a manner that is efficient, since it needs random access modification
    {
        m_weightLists[i].m_weightSum = 0.0f;//memory initialization may not go much faster in parallel
        size_t neighborCount = tempList[i].m_nodes.size();
        m_weightLists[i].m_nodes.reserve(neighborCount);//also preallocate the expected number of nodes (geodesic distance should be symmetric except for rounding errors, so it should usually be exact)
        m_weightLists[i].m_weights.reserve(neighborCount);
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

void MetricSmoothingObject::precomputeWeightsROIGeoGaussEqual(const SurfaceFile* mySurf, float myKernel, const MetricFile* theRoi, const float* nodeAreas)
{
    int32_t numNodes = mySurf->getNumberOfNodes();
    float myGeoDist = myKernel * 3.0f;
    float gaussianDenom = -0.5f / myKernel / myKernel;
    vector<WeightList> tempList;//this is used to compute scattering kernels because it is easier to normalize scattering kernels correctly, and then convert to gathering kernels
    tempList.resize(numNodes);
    const float* myRoiColumn = theRoi->getValuePointerForColumn(0);
    CaretPointer<GeodesicHelperBase> myGeoBase(new GeodesicHelperBase(mySurf, nodeAreas));//NOTE: if these are equal to the surface's areas, then it does some extra operations, but gets the same answer
#pragma omp CARET_PAR
    {
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
        CaretPointer<GeodesicHelper> myGeoHelp(new GeodesicHelper(myGeoBase));
        vector<float> distances;
        vector<int32_t> nodes;
#pragma omp CARET_FOR schedule(dynamic)
        for (int32_t i = 0; i < numNodes; ++i)
        {
            if (myRoiColumn[i] > 0.0f)//we don't need to scatter from things outside the ROI
            {
                myGeoHelp->getNodesToGeoDist(i, myGeoDist, nodes, distances, true);
                const vector<int32_t>& tempneighbors = myTopoHelp->getNodeNeighbors(i);
                if (distances.size() <= tempneighbors.size())//because neighbors doesn't include center, so if they are equal, geo is missing a neighbor
                {
                    nodes = tempneighbors;
                    nodes.push_back(i);
                    myGeoHelp->getGeoToTheseNodes(i, nodes, distances, true);
                }
                int32_t numNeigh = (int32_t)distances.size();
                tempList[i].m_weightSum = 0.0f;
                for (int32_t j = 0; j < numNeigh; ++j)
                {//but we DO need to compute scattering TO things outside the ROI, so that our normalization doesn't increase the in-ROI influence of edge nodes
                    float weight = exp(distances[j] * distances[j] * gaussianDenom);//exp(- dist ^ 2 / (2 * sigma ^ 2))
                    tempList[i].m_weightSum += weight;//add it to the total weight in order to normalize correctly
                    if (myRoiColumn[nodes[j]] > 0.0f)
                    {//BUT, don't add it to the list if it is outside the ROI
                        tempList[i].m_nodes.push_back(nodes[j]);
                        tempList[i].m_weights.push_back(weight);
                    }
                }
                float myFactor = 1.0f / tempList[i].m_weightSum;//make each scattering kernel sum to 1
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
        size_t neighborCount = tempList[i].m_nodes.size();
        m_weightLists[i].m_nodes.reserve(neighborCount);//also preallocate the expected number of nodes, again, should be exact except for rounding errors in geodesic distance
        m_weightLists[i].m_weights.reserve(neighborCount);
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

void MetricSmoothingObject::precomputeWeights(const SurfaceFile* mySurf, float myKernel, const MetricFile* theRoi, Method myMethod, const float* nodeAreas)
{
    const float* passAreas = nodeAreas;
    vector<float> areasTemp;
    if (passAreas == NULL)
    {//corrected node areas need to be passed into all geodesic smoothings, to generate corrected distances
        mySurf->computeNodeAreas(areasTemp);
        passAreas = areasTemp.data();
    }
    if (theRoi != NULL)
    {
        switch (myMethod)
        {
            case GEO_GAUSS_AREA:
                precomputeWeightsROIGeoGaussArea(mySurf, myKernel, theRoi, passAreas);
                break;
            case GEO_GAUSS_EQUAL:
                precomputeWeightsROIGeoGaussEqual(mySurf, myKernel, theRoi, passAreas);
                break;
            case GEO_GAUSS:
                precomputeWeightsROIGeoGauss(mySurf, myKernel, theRoi, passAreas);
                break;
            default:
                throw CaretException("unknown smoothing method specified");
        };
    } else {
        switch (myMethod)
        {
            case GEO_GAUSS_AREA:
                precomputeWeightsGeoGaussArea(mySurf, myKernel, passAreas);
                break;
            case GEO_GAUSS_EQUAL:
                precomputeWeightsGeoGaussEqual(mySurf, myKernel, passAreas);
                break;
            case GEO_GAUSS:
                precomputeWeightsGeoGauss(mySurf, myKernel, passAreas);
                break;
            default:
                throw CaretException("unknown smoothing method specified");
        };
    }
}
