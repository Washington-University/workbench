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

#include "AlgorithmMetricROIsFromExtrema.h"
#include "AlgorithmException.h"

#include "CaretAssert.h"
#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

#include <cmath>
#include <map>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmMetricROIsFromExtrema::getCommandSwitch()
{
    return "-metric-rois-from-extrema";
}

AString AlgorithmMetricROIsFromExtrema::getShortDescription()
{
    return "CREATE METRIC ROI MAPS FROM EXTREMA MAPS";
}

OperationParameters* AlgorithmMetricROIsFromExtrema::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to use for geodesic distance");
    
    ret->addMetricParameter(2, "metric", "the input metric file");
    
    ret->addDoubleParameter(3, "limit", "geodesic distance limit from vertex, in mm");
    
    ret->addMetricOutputParameter(4, "metric-out", "the output metric file");
    
    OptionalParameter* gaussOpt = ret->createOptionalParameter(5, "-gaussian", "generate a gaussian kernel instead of a flat ROI");
    gaussOpt->addDoubleParameter(1, "sigma", "the sigma for the gaussian kernel, in mm");
    
    OptionalParameter* overlapOpt = ret->createOptionalParameter(6, "-overlap-logic", "how to handle overlapping ROIs, default ALLOW");
    overlapOpt->addStringParameter(1, "method", "the method of resolving overlaps");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(7, "-column", "select a single input column to use");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    ret->setHelpText(
        AString("For each nonzero value in each map, make a map with an ROI around that location.  ") +
        "If the -gaussian option is specified, then normalized gaussian kernels are output instead of ROIs.  " +
        "The <method> argument to -overlap-logic must be one of ALLOW, CLOSEST, or EXCLUDE.  " +
        "ALLOW is the default, and means that ROIs are treated independently and may overlap.  " +
        "CLOSEST means that ROIs may not overlap, and that no ROI contains vertices that are closer to a different seed vertex.  " +
        "EXCLUDE means that ROIs may not overlap, and that any vertex within range of more than one ROI does not belong to any ROI."
    );
    return ret;
}

void AlgorithmMetricROIsFromExtrema::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetric = myParams->getMetric(2);
    float limit = (float)myParams->getDouble(3);
    MetricFile* myMetricOut = myParams->getOutputMetric(4);
    OptionalParameter* gaussOpt = myParams->getOptionalParameter(5);
    float sigma = -1.0f;
    if (gaussOpt->m_present)
    {//set up to use a gaussian function
        sigma = (float)gaussOpt->getDouble(1);
        if (sigma < 0.0f)
        {
            throw AlgorithmException("invalid sigma specified");
        }
    }
    OverlapLogic overlapType = ALLOW;
    OptionalParameter* overlapOpt = myParams->getOptionalParameter(6);
    if (overlapOpt->m_present)
    {
        AString overlapString = overlapOpt->getString(1);
        if (overlapString == "ALLOW")
        {
            overlapType = ALLOW;
        } else if (overlapString == "CLOSEST") {
            overlapType = CLOSEST;
        } else if (overlapString == "EXCLUDE") {
            overlapType = EXCLUDE;
        } else {
            throw AlgorithmException("unrecognized overlap method: " + overlapString);
        }
    }
    int myColumn = -1;
    OptionalParameter* columnSelect = myParams->getOptionalParameter(7);
    if (columnSelect->m_present)
    {
        myColumn = (int)myMetric->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (myColumn < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    AlgorithmMetricROIsFromExtrema(myProgObj, mySurf, myMetric, limit, myMetricOut, sigma, overlapType, myColumn);
}

AlgorithmMetricROIsFromExtrema::AlgorithmMetricROIsFromExtrema(ProgressObject* myProgObj, const SurfaceFile* mySurf, const MetricFile* myMetric, const float& limit,
                                                               MetricFile* myMetricOut, const float& sigma, const OverlapLogic& overlapType, const int& myColumn) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = mySurf->getNumberOfNodes();
    if (myMetric->getNumberOfNodes() != numNodes) throw AlgorithmException("surface and metric files have different number of nodes");
    int numCols = myMetric->getNumberOfColumns();
    if (myColumn < -1 || myColumn >= numCols)
    {
        throw AlgorithmException("invalid column number");
    }
    int64_t extremaCount = 0;
    if (myColumn == -1)
    {
        for (int i = 0; i < numCols; ++i)
        {
            const float* data = myMetric->getValuePointerForColumn(i);
            for (int j = 0; j < numNodes; ++j)
            {
                if (data[j] != 0.0f) ++extremaCount;
            }
        }
    } else {
        const float* data = myMetric->getValuePointerForColumn(0);
        for (int j = 0; j < numNodes; ++j)
        {
            if (data[j] != 0.0f) ++extremaCount;
        }
    }
    if (extremaCount >= (1LL<<31)) throw AlgorithmException("too many output maps for a metric file");//hopefully this is never needed
    int32_t mapsOut = (int32_t)extremaCount;
    vector<float> excludeDists(numNodes, -1.0f);
    vector<int32_t> excludeSouces(numNodes, -1);
    vector<map<int32_t, float> > roiLists(mapsOut);
    CaretPointer<GeodesicHelper> myHelp = mySurf->getGeodesicHelper();
    int64_t mapCounter = 0;
    if (myColumn == -1)
    {
        for (int i = 0; i < numCols; ++i)
        {
            const float* data = myMetric->getValuePointerForColumn(i);
            for (int j = 0; j < numNodes; ++j)
            {
                if (data[j] != 0.0f)
                {
                    vector<int32_t> nodeList;
                    vector<float> distList;
                    myHelp->getNodesToGeoDist(j, limit, nodeList, distList);
                    int listNum = (int)nodeList.size();
                    switch (overlapType)
                    {
                        case ALLOW:
                            for (int k = 0; k < listNum; ++k)
                            {
                                roiLists[mapCounter][nodeList[k]] = distList[k];
                            }
                            break;
                        case CLOSEST:
                            for (int k = 0; k < listNum; ++k)
                            {
                                const int32_t& thisNode = nodeList[k];
                                const float& thisDist = distList[k];
                                if (excludeDists[thisNode] < 0.0f)
                                {
                                    excludeDists[thisNode] = thisDist;
                                    excludeSouces[thisNode] = mapCounter;
                                    roiLists[mapCounter][thisNode] = thisDist;
                                } else {
                                    if (excludeDists[thisNode] > thisDist)
                                    {
                                        roiLists[excludeSouces[thisNode]].erase(thisNode);
                                    }
                                    excludeDists[thisNode] = thisDist;
                                    excludeSouces[thisNode] = mapCounter;
                                    roiLists[mapCounter][thisNode] = thisDist;
                                }
                            }
                            break;
                        case EXCLUDE:
                            for (int k = 0; k < listNum; ++k)
                            {
                                const int32_t& thisNode = nodeList[k];
                                const float& thisDist = distList[k];
                                if (excludeDists[thisNode] < 0.0f)
                                {
                                    excludeDists[thisNode] = thisDist;
                                    excludeSouces[thisNode] = mapCounter;
                                    roiLists[mapCounter][thisNode] = thisDist;
                                } else {
                                    if (excludeSouces[thisNode] != -1)
                                    {
                                        roiLists[excludeSouces[thisNode]].erase(thisNode);
                                        excludeSouces[thisNode] = -1;
                                    }
                                }
                            }
                            break;
                    }
                    ++mapCounter;
                }
            }
        }
    } else {
        const float* data = myMetric->getValuePointerForColumn(0);
        for (int j = 0; j < numNodes; ++j)
        {
            if (data[j] != 0.0f)
            {
                vector<int32_t> nodeList;
                vector<float> distList;
                myHelp->getNodesToGeoDist(j, limit, nodeList, distList);
                int listNum = (int)nodeList.size();
                switch (overlapType)
                {
                    case ALLOW:
                        for (int k = 0; k < listNum; ++k)
                        {
                            roiLists[mapCounter][nodeList[k]] = distList[k];
                        }
                        break;
                    case CLOSEST:
                        for (int k = 0; k < listNum; ++k)
                        {
                            const int32_t& thisNode = nodeList[k];
                            const float& thisDist = distList[k];
                            if (excludeDists[thisNode] < 0.0f)
                            {
                                excludeDists[thisNode] = thisDist;
                                excludeSouces[thisNode] = mapCounter;
                                roiLists[mapCounter][thisNode] = thisDist;
                            } else {
                                if (excludeDists[thisNode] > thisDist)
                                {
                                    roiLists[excludeSouces[thisNode]].erase(thisNode);
                                }
                                excludeDists[thisNode] = thisDist;
                                excludeSouces[thisNode] = mapCounter;
                                roiLists[mapCounter][thisNode] = thisDist;
                            }
                        }
                        break;
                    case EXCLUDE:
                        for (int k = 0; k < listNum; ++k)
                        {
                            const int32_t& thisNode = nodeList[k];
                            const float& thisDist = distList[k];
                            if (excludeDists[thisNode] < 0.0f)
                            {
                                excludeDists[thisNode] = thisDist;
                                excludeSouces[thisNode] = mapCounter;
                                roiLists[mapCounter][thisNode] = thisDist;
                            } else {
                                if (excludeSouces[thisNode] != -1)
                                {
                                    roiLists[excludeSouces[thisNode]].erase(thisNode);
                                    excludeSouces[thisNode] = -1;
                                }
                            }
                        }
                        break;
                }
                ++mapCounter;
            }
        }
    }
    CaretAssert(mapCounter == extremaCount);
    myMetricOut->setNumberOfNodesAndColumns(numNodes, mapsOut);
    myMetricOut->setStructure(mySurf->getStructure());
    vector<float> tempCol(numNodes, 0.0f);
    if (sigma > 0.0f)
    {
        float gaussDenom = -0.5f / sigma / sigma;
        for (int32_t i = 0; i < mapsOut; ++i)
        {
            double accum = 0.0;
            map<int32_t, float>::iterator myEnd = roiLists[i].end();
            for (map<int32_t, float>::iterator iter = roiLists[i].begin(); iter != myEnd; ++iter)
            {
                float gaussVal = exp(iter->second * iter->second * gaussDenom);
                accum += gaussVal;
                tempCol[iter->first] = gaussVal;//initial kernel value
            }
            for (map<int32_t, float>::iterator iter = roiLists[i].begin(); iter != myEnd; ++iter)
            {
                tempCol[iter->first] /= accum;//normalize
            }
            myMetricOut->setValuesForColumn(i, tempCol.data());
            for (map<int32_t, float>::iterator iter = roiLists[i].begin(); iter != myEnd; ++iter)
            {
                tempCol[iter->first] = 0.0f;//rezero changed values for next map
            }
        }
    } else {
        for (int32_t i = 0; i < mapsOut; ++i)
        {
            map<int32_t, float>::iterator myEnd = roiLists[i].end();
            for (map<int32_t, float>::iterator iter = roiLists[i].begin(); iter != myEnd; ++iter)
            {
                tempCol[iter->first] = 1.0f;//make roi
            }
            myMetricOut->setValuesForColumn(i, tempCol.data());
            for (map<int32_t, float>::iterator iter = roiLists[i].begin(); iter != myEnd; ++iter)
            {
                tempCol[iter->first] = 0.0f;//rezero changed values for next map
            }
        }
    }
}

float AlgorithmMetricROIsFromExtrema::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricROIsFromExtrema::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
