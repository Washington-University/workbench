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

#include "AlgorithmMetricROIsFromExtrema.h"
#include "AlgorithmException.h"

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

#include <cmath>

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
    
    OptionalParameter* roiOption = ret->createOptionalParameter(6, "-roi", "select a region of interest to use");
    roiOption->addMetricParameter(1, "roi-metric", "the area to use, as a metric");
    
    OptionalParameter* overlapOpt = ret->createOptionalParameter(7, "-overlap-logic", "how to handle overlapping ROIs, default ALLOW");
    overlapOpt->addStringParameter(1, "method", "the method of resolving overlaps");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(8, "-column", "select a single input column to use");
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
        if (sigma <= 0.0f)
        {
            throw AlgorithmException("invalid sigma specified");
        }
    }
    MetricFile* myRoi = NULL;
    OptionalParameter* roiOption = myParams->getOptionalParameter(6);
    if (roiOption->m_present)
    {
        myRoi = roiOption->getMetric(1);
    }
    OverlapLogicEnum::Enum overlapType = OverlapLogicEnum::ALLOW;
    OptionalParameter* overlapOpt = myParams->getOptionalParameter(7);
    if (overlapOpt->m_present)
    {
        bool ok = false;
        overlapType = OverlapLogicEnum::fromName(overlapOpt->getString(1), &ok);
        if (!ok) throw AlgorithmException("unrecognized overlap method: " + overlapOpt->getString(1));
    }
    int myColumn = -1;
    OptionalParameter* columnSelect = myParams->getOptionalParameter(8);
    if (columnSelect->m_present)
    {
        myColumn = (int)myMetric->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (myColumn < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    AlgorithmMetricROIsFromExtrema(myProgObj, mySurf, myMetric, limit, myMetricOut, sigma, myRoi, overlapType, myColumn);
}

AlgorithmMetricROIsFromExtrema::AlgorithmMetricROIsFromExtrema(ProgressObject* myProgObj, const SurfaceFile* mySurf, const MetricFile* myMetric, const float& limit,
                                                               MetricFile* myMetricOut, const float& sigma, const MetricFile* myRoi, const OverlapLogicEnum::Enum& overlapType, const int& myColumn) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = mySurf->getNumberOfNodes();
    if (myMetric->getNumberOfNodes() != numNodes) throw AlgorithmException("surface and metric files have different number of nodes");
    const float* roiData = NULL;
    if (myRoi != NULL)
    {
        if (myRoi->getNumberOfNodes() != numNodes) throw AlgorithmException("roi metric has a different number of nodes");
        roiData = myRoi->getValuePointerForColumn(0);
    }
    int numCols = myMetric->getNumberOfColumns();
    if (myColumn < -1 || myColumn >= numCols)
    {
        throw AlgorithmException("invalid column number");
    }
    int64_t extremaCount = 0;
    if (myColumn == -1)
    {
        if (roiData == NULL)
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
            for (int i = 0; i < numCols; ++i)
            {
                const float* data = myMetric->getValuePointerForColumn(i);
                for (int j = 0; j < numNodes; ++j)
                {
                    if (roiData[j] > 0.0f && data[j] != 0.0f) ++extremaCount;
                }
            }
        }
    } else {
        if (roiData == NULL)
        {
            const float* data = myMetric->getValuePointerForColumn(0);
            for (int j = 0; j < numNodes; ++j)
            {
                if (data[j] != 0.0f) ++extremaCount;
            }
        } else {
            const float* data = myMetric->getValuePointerForColumn(0);
            for (int j = 0; j < numNodes; ++j)
            {
                if (roiData[j] > 0.0f && data[j] != 0.0f) ++extremaCount;
            }
        }
    }
    if (extremaCount == 0) CaretLogWarning("no nonzero values in input metric file, output file will be empty");//if this were an error, the cifti version will error when just one component is zeros
    if (extremaCount >= (1LL<<31)) throw AlgorithmException("too many output maps for a metric file");//hopefully this is never needed
    int32_t mapsOut = (int32_t)extremaCount;
    vector<float> excludeDists(numNodes, -1.0f);
    vector<int32_t> excludeSources(numNodes, -1);
    vector<map<int32_t, float> > roiLists(mapsOut);
    CaretPointer<GeodesicHelper> myHelp = mySurf->getGeodesicHelper();
    int64_t mapCounter = 0;
    if (myColumn == -1)
    {
        for (int i = 0; i < numCols; ++i)
        {
            const float* data = myMetric->getValuePointerForColumn(i);
            processMap(data, excludeDists, excludeSources, roiLists, mapCounter, myHelp, limit, roiData, overlapType, numNodes);
        }
    } else {
        const float* data = myMetric->getValuePointerForColumn(0);
        processMap(data, excludeDists, excludeSources, roiLists, mapCounter, myHelp, limit, roiData, overlapType, numNodes);
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

void AlgorithmMetricROIsFromExtrema::processMap(const float* data, vector<float>& excludeDists, vector<int32_t> excludeSources, vector<map<int32_t, float> >& roiLists,
                                                int64_t& mapCounter, CaretPointer<GeodesicHelper>& myHelp, const float& limit, const float* roiData,
                                                const OverlapLogicEnum::Enum& overlapType, const int& numNodes)
{
    for (int j = 0; j < numNodes; ++j)
    {
        if ((roiData == NULL || roiData[j] > 0.0f) && data[j] != 0.0f)
        {
            vector<int32_t> nodeList;
            vector<float> distList;
            myHelp->getNodesToGeoDist(j, limit, nodeList, distList);
            int listNum = (int)nodeList.size();
            switch (overlapType)
            {
                case OverlapLogicEnum::ALLOW:
                    if (roiData == NULL)
                    {
                        for (int k = 0; k < listNum; ++k)
                        {
                            const int32_t& thisNode = nodeList[k];
                            roiLists[mapCounter][thisNode] = distList[k];
                        }
                    } else {
                        for (int k = 0; k < listNum; ++k)
                        {
                            const int32_t& thisNode = nodeList[k];
                            if (roiData[thisNode] > 0.0f)
                            {
                                roiLists[mapCounter][thisNode] = distList[k];
                            }
                        }
                    }
                    break;
                case OverlapLogicEnum::CLOSEST:
                    for (int k = 0; k < listNum; ++k)
                    {
                        const int32_t& thisNode = nodeList[k];
                        if (roiData == NULL || roiData[thisNode] > 0.0f)
                        {
                            const float& thisDist = distList[k];
                            if (excludeDists[thisNode] < 0.0f)
                            {
                                excludeDists[thisNode] = thisDist;
                                excludeSources[thisNode] = mapCounter;
                                roiLists[mapCounter][thisNode] = thisDist;
                            } else {
                                if (excludeDists[thisNode] > thisDist)
                                {
                                    roiLists[excludeSources[thisNode]].erase(thisNode);
                                }
                                excludeDists[thisNode] = thisDist;
                                excludeSources[thisNode] = mapCounter;
                                roiLists[mapCounter][thisNode] = thisDist;
                            }
                        }
                    }
                    break;
                case OverlapLogicEnum::EXCLUDE:
                    for (int k = 0; k < listNum; ++k)
                    {
                        const int32_t& thisNode = nodeList[k];
                        if (roiData == NULL || roiData[thisNode] > 0.0f)
                        {
                            const float& thisDist = distList[k];
                            if (excludeDists[thisNode] < 0.0f)
                            {
                                excludeDists[thisNode] = thisDist;
                                excludeSources[thisNode] = mapCounter;
                                roiLists[mapCounter][thisNode] = thisDist;
                            } else {
                                if (excludeSources[thisNode] != -1)
                                {
                                    roiLists[excludeSources[thisNode]].erase(thisNode);
                                    excludeSources[thisNode] = -1;
                                }
                            }
                        }
                    }
                    break;
            }
            ++mapCounter;
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
