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

#include "AlgorithmMetricTFCE.h"
#include "AlgorithmException.h"

#include "AlgorithmMetricSmoothing.h"
#include "CaretAssert.h"
#include "CaretHeap.h"
#include "CaretOMP.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

#include <cmath>
#include <set>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmMetricTFCE::getCommandSwitch()
{
    return "-metric-tfce";
}

AString AlgorithmMetricTFCE::getShortDescription()
{
    return "DO TFCE ON A METRIC FILE";
}

OperationParameters* AlgorithmMetricTFCE::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface", "the surface to compute on");
    
    ret->addMetricParameter(2, "metric-in", "the metric to run TFCE on");
    
    ret->addMetricOutputParameter(3, "metric-out", "the output metric");
    
    OptionalParameter* presmoothOpt = ret->createOptionalParameter(4, "-presmooth", "smooth the metric before running TFCE");
    presmoothOpt->addDoubleParameter(1, "kernel", "the size of the gaussian smoothing kernel in mm, as sigma by default");
    presmoothOpt->createOptionalParameter(2, "-fwhm", "kernel size is FWHM, not sigma");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(5, "-roi", "select a region of interest to run TFCE on");
    roiOpt->addMetricParameter(1, "roi-metric", "the area to run TFCE on, as a metric");

    OptionalParameter* paramsOpt = ret->createOptionalParameter(6, "-parameters", "set parameters for TFCE integral");
    paramsOpt->addDoubleParameter(1, "E", "exponent for cluster area (default 1.0)");
    paramsOpt->addDoubleParameter(2, "H", "exponent for threshold value (default 2.0)");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(7, "-column", "select a single column");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    OptionalParameter* corrAreaOpt = ret->createOptionalParameter(8, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    corrAreaOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    ret->setHelpText(
        AString("Threshold-free cluster enhancement is a method to increase the relative value of regions that would form clusters in a standard thresholding test.  ") +
        "This is accomplished by evaluating the integral of:\n\n" +
        "e(h, p)^E * h^H * dh\n\n" +
        "at each vertex p, where h ranges from 0 to the maximum value in the data, and e(h, p) is the extent of the cluster containing vertex p at threshold h.  " +
        "Negative values are similarly enhanced by negating the data, running the same process, and negating the result.\n\n" +
        "When using -presmooth with -corrected-areas, note that it is an approximate correction within the smoothing algorithm (the TFCE correction is exact).  " +
        "Doing smoothing on individual surfaces before averaging/TFCE is preferred, when possible, in order to better tie the smoothing kernel size to the original feature size.\n\n" +
        "The TFCE method is explained in: Smith SM, Nichols TE., \"Threshold-free cluster enhancement: addressing problems of smoothing, threshold dependence and localisation in cluster inference.\" Neuroimage. 2009 Jan 1;44(1):83-98. PMID: 18501637"
    );
    return ret;
}

void AlgorithmMetricTFCE::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetric = myParams->getMetric(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    float presmooth = 0.0f;
    OptionalParameter* presmoothOpt = myParams->getOptionalParameter(4);
    if (presmoothOpt->m_present)
    {
        presmooth = (float)presmoothOpt->getDouble(1);
        if (presmooth <= 0.0f) throw AlgorithmException("presmooth kernel size must be positive");
        if (presmoothOpt->getOptionalParameter(2)->m_present)
        {
            presmooth = presmooth / (2.0f * sqrt(2.0f * log(2.0f)));
        }
    }
    MetricFile* myRoi = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(5);
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getMetric(1);
    }
    float param_e = 1.0f, param_h = 2.0;
    OptionalParameter* paramsOpt = myParams->getOptionalParameter(6);
    if (paramsOpt->m_present)
    {
        param_e = (float)paramsOpt->getDouble(1);
        param_h = (float)paramsOpt->getDouble(2);
    }
    OptionalParameter* columnSelect = myParams->getOptionalParameter(7);
    int columnNum = -1;
    if (columnSelect->m_present)
    {//set up to use the single column
        columnNum = (int)myMetric->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (columnNum < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    MetricFile* corrAreaMetric = NULL;
    OptionalParameter* corrAreaOpt = myParams->getOptionalParameter(8);
    if (corrAreaOpt->m_present)
    {
        corrAreaMetric = corrAreaOpt->getMetric(1);
    }
    AlgorithmMetricTFCE(myProgObj, mySurf, myMetric, myMetricOut, presmooth, myRoi, param_e, param_h, columnNum, corrAreaMetric);
}

AlgorithmMetricTFCE::AlgorithmMetricTFCE(ProgressObject* myProgObj, const SurfaceFile* mySurf, const MetricFile* myMetric, MetricFile* myMetricOut, const float& presmooth,
                                         const MetricFile* myRoi, const float& param_e, const float& param_h, const int& columnNum, const MetricFile* corrAreaMetric) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (mySurf->getNumberOfNodes() != myMetric->getNumberOfNodes()) throw AlgorithmException("metric and surface have different number of vertices");
    if (myRoi != NULL && mySurf->getNumberOfNodes() != myRoi->getNumberOfNodes()) throw AlgorithmException("roi metric and surface have different number of vertices");
    if (corrAreaMetric != NULL && mySurf->getNumberOfNodes() != corrAreaMetric->getNumberOfNodes()) throw AlgorithmException("corrected area metric and surface have different number of vertices");
    if (columnNum < -1 || columnNum >= myMetric->getNumberOfColumns()) throw AlgorithmException("invalid column specified");
    const float* roiData = NULL, *areaData = NULL;
    vector<float> surfAreaData;
    if (corrAreaMetric == NULL)
    {
        mySurf->computeNodeAreas(surfAreaData);
        areaData = surfAreaData.data();
    } else {
        areaData = corrAreaMetric->getValuePointerForColumn(0);
    }
    if (myRoi != NULL) roiData = myRoi->getValuePointerForColumn(0);
    if (columnNum == -1)
    {
        const MetricFile* toUse = myMetric;
        MetricFile postSmooth;
        if (presmooth > 0.0f)
        {
            AlgorithmMetricSmoothing(NULL, mySurf, myMetric, presmooth, &postSmooth, myRoi, false, false, -1, corrAreaMetric);
            toUse = &postSmooth;
        }
        int numCols = myMetric->getNumberOfColumns();
        myMetricOut->setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), numCols);
        myMetricOut->setStructure(mySurf->getStructure());
#pragma omp CARET_PAR
        {
            vector<float> outcol(mySurf->getNumberOfNodes(), 0.0f);
#pragma omp CARET_FOR
            for (int col = 0; col < numCols; ++col)
            {
                processColumn(mySurf, toUse->getValuePointerForColumn(col), outcol.data(), roiData, param_e, param_h, areaData);
                myMetricOut->setValuesForColumn(col, outcol.data());
                myMetricOut->setMapName(col, myMetric->getMapName(col));
            }
        }
    } else {
        const MetricFile* toUse = myMetric;
        int useCol = columnNum;
        MetricFile postSmooth;
        if (presmooth > 0.0f)
        {
            AlgorithmMetricSmoothing(NULL, mySurf, myMetric, presmooth, &postSmooth, myRoi, false, false, columnNum, corrAreaMetric);
            toUse = &postSmooth;
            useCol = 0;
        }
        myMetricOut->setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), 1);
        myMetricOut->setStructure(mySurf->getStructure());
        vector<float> outcol(mySurf->getNumberOfNodes(), 0.0f);
        processColumn(mySurf, toUse->getValuePointerForColumn(useCol), outcol.data(), roiData, param_e, param_h, areaData);
        myMetricOut->setValuesForColumn(0, outcol.data());
        myMetricOut->setMapName(0, myMetric->getMapName(columnNum));
    }
}

void AlgorithmMetricTFCE::processColumn(const SurfaceFile* mySurf, const float* colData, float* outData, const float* roiData, const float& param_e, const float& param_h, const float* areaData)
{
    int numNodes = mySurf->getNumberOfNodes();
    vector<double> accum(numNodes, 0.0);
    CaretPointer<TopologyHelper> myHelper = mySurf->getTopologyHelper();
    tfce_pos(myHelper, colData, accum.data(), roiData, param_e, param_h, areaData);
    vector<float> negData(numNodes);
    for (int i = 0; i < numNodes; ++i)
    {
        negData[i] = -colData[i];
    }
    tfce_pos(myHelper, negData.data(), accum.data(), roiData, param_e, param_h, areaData);//negatives and positives don't overlap, so reuse the accum array
    for (int i = 0; i < numNodes; ++i)
    {
        if (roiData == NULL || roiData[i] > 0.0f)
        {
            if (colData[i] < 0.0f)
            {
                outData[i] = (float)-accum[i];
            } else {
                outData[i] = (float)accum[i];
            }
        } else {
            outData[i] = 0.0f;
        }
    }
}

namespace
{//hidden namespace just to make sure things don't collide
    struct Cluster
    {
        double accumVal, totalArea;
        vector<int> members;
        float lastVal;
        bool first;
        Cluster()
        {
            first = true;
            accumVal = 0.0;
            totalArea = 0.0;
            lastVal = 0.0f;
        }
        void addMember(const int& node, const float& val, const float& area, const float& param_e, const float& param_h)
        {
            update(val, param_e, param_h);
            members.push_back(node);
            totalArea += area;
        }
        void update(const float& bottomVal, const float& param_e, const float& param_h)
        {
            if (first)
            {
                lastVal = bottomVal;
                first = false;
            } else {
                if (bottomVal != lastVal)//skip computing if there is no difference
                {
                    CaretAssert(bottomVal < lastVal);
                    double integrated_h = param_h + 1.0f;//integral(x^h) = (x^(h + 1))/(h + 1) + C
                    double newSlice = pow(totalArea, (double)param_e) * (pow((double)lastVal, integrated_h) - pow((double)bottomVal, integrated_h)) / integrated_h;
                    accumVal += newSlice;
                    lastVal = bottomVal;//computing in double precision, with float for inputs, puts the smallest difference between values far greater than the instability of the computation
                }
            }
        }
    };
    
    int allocCluster(vector<Cluster>& clusterList, set<int>& deadClusters)
    {
        if (deadClusters.empty())
        {
            clusterList.push_back(Cluster());
            return (int)(clusterList.size() - 1);
        } else {
            set<int>::iterator iter = deadClusters.begin();
            int ret = *iter;
            deadClusters.erase(iter);
            clusterList[ret] = Cluster();//reinitialize
            return ret;
        }
    }
}

void AlgorithmMetricTFCE::tfce_pos(TopologyHelper* myHelper, const float* colData, double* accumData, const float* roiData, const float& param_e, const float& param_h, const float* areaData)
{
    int numNodes = myHelper->getNumberOfNodes();
    vector<int> membership(numNodes, -1);//int is enough as long as numNodes is fine as an int, for obvious reasons
    vector<Cluster> clusterList;
    set<int> deadClusters;//to allow reallocation without changing indices
    CaretSimpleMaxHeap<int, float> nodeHeap;
    for (int i = 0; i < numNodes; ++i)
    {
        if ((roiData == NULL || roiData[i] > 0.0f) && colData[i] > 0.0f)
        {
            nodeHeap.push(i, colData[i]);
        }
    }
    while (!nodeHeap.isEmpty())
    {
        float value;
        int node = nodeHeap.pop(&value);
        const vector<int32_t>& neighbors = myHelper->getNodeNeighbors(node);
        int numNeigh = (int)neighbors.size();
        set<int> touchingClusters;
        for (int i = 0; i < numNeigh; ++i)
        {
            if (membership[neighbors[i]] != -1)
            {
                touchingClusters.insert(membership[neighbors[i]]);
            }
        }
        int numTouching = (int)touchingClusters.size();
        switch (numTouching)
        {
            case 0://make new cluster
            {
                int newCluster = allocCluster(clusterList, deadClusters);
                clusterList[newCluster].addMember(node, value, areaData[node], param_e, param_h);
                membership[node] = newCluster;
                break;
            }
            case 1://add to cluster
            {
                int whichCluster = *(touchingClusters.begin());
                clusterList[whichCluster].addMember(node, value, areaData[node], param_e, param_h);
                membership[node] = whichCluster;
                accumData[node] -= clusterList[whichCluster].accumVal;//the accum value is the current amount less than the peak value that the edge of the cluster has (this node is on the edge)
                break;//so, when the cluster merges or reaches 0, we add the accum value to every member and zero the accum value of the merged cluster, and we get the correct value in the end (with far fewer flops than integrating everywhere)
            }
            default://merge all touching clusters
            {
                int mergedIndex = -1, biggestSize = 0;//find the biggest cluster (in number of members) and use as merged cluster, for optimization purposes
                for (set<int>::iterator iter = touchingClusters.begin(); iter != touchingClusters.end(); ++iter)
                {
                    if ((int)clusterList[*iter].members.size() > biggestSize)
                    {
                        mergedIndex = *iter;
                        biggestSize = (int)clusterList[*iter].members.size();
                    }
                }
                CaretAssertVectorIndex(clusterList, mergedIndex);
                Cluster& mergedCluster = clusterList[mergedIndex];
                mergedCluster.update(value, param_e, param_h);//recalculate to align cluster bottoms
                for (set<int>::iterator iter = touchingClusters.begin(); iter != touchingClusters.end(); ++iter)
                {
                    if (*iter != mergedIndex)//if we are the largest cluster, don't modify the per-vertex accum for members, so merges between small and large clusters are cheap
                    {
                        Cluster& thisCluster = clusterList[*iter];
                        thisCluster.update(value, param_e, param_h);//recalculate to align cluster bottoms
                        int numMembers = (int)thisCluster.members.size();
                        double correctionVal = thisCluster.accumVal - mergedCluster.accumVal;//fix the accum values in the side cluster so we can add the merged cluster's accum to everything at the end
                        for (int j = 0; j < numMembers; ++j)//add the correction value to every member so that we have the current integrated values correct
                        {
                            accumData[thisCluster.members[j]] += correctionVal;//apply the correction
                            membership[thisCluster.members[j]] = mergedIndex;//change the membership lookup
                        }
                        mergedCluster.members.insert(mergedCluster.members.end(), thisCluster.members.begin(), thisCluster.members.end());//copy all members
                        mergedCluster.totalArea += thisCluster.totalArea;
                        deadClusters.insert(*iter);//kill it
                        vector<int>().swap(clusterList[*iter].members);//also try to deallocate member list
                    }
                }
                mergedCluster.addMember(node, value, areaData[node], param_e, param_h);//will not trigger recomputation, we already recomputed at this value
                accumData[node] -= mergedCluster.accumVal;//the vertex they merge on must not get the peak value of the cluster, obviously, so again, record its difference from peak
                membership[node] = mergedIndex;
                break;//NOTE: do not reset the accum value of the merged cluster, we specifically avoided modifying the per-vertex accum for its members, so the cluster accum is still in play
            }
        }
    }
    int listSize = (int)clusterList.size();//final cleanup of accum values
    for (int i = 0; i < listSize; ++i)
    {
        if (deadClusters.find(i) != deadClusters.end()) continue;//ignore clusters that don't exist
        Cluster& thisCluster = clusterList[i];
        thisCluster.update(0.0f, param_e, param_h);//update to include the to-zero slice
        int numMembers = (int)thisCluster.members.size();
        for (int j = 0; j < numMembers; ++j)
        {
            accumData[thisCluster.members[j]] += thisCluster.accumVal;//add the resulting slice to all members - their stored data contains the offset between the cluster peak and their corect value
        }
    }
}

float AlgorithmMetricTFCE::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricTFCE::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
