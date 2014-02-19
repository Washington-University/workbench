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

#include "AlgorithmMetricFindClusters.h"
#include "AlgorithmException.h"

#include "MetricFile.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmMetricFindClusters::getCommandSwitch()
{
    return "-metric-find-clusters";
}

AString AlgorithmMetricFindClusters::getShortDescription()
{
    return "FILTER CLUSTERS BY SURFACE AREA";
}

OperationParameters* AlgorithmMetricFindClusters::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to compute on");
    
    ret->addMetricParameter(2, "metric-in", "the input metric");
    
    ret->addDoubleParameter(3, "minimum-value", "threshold for data values");
    
    ret->addDoubleParameter(4, "minimum-area", "threshold for cluster area, in mm^2");
    
    ret->addMetricOutputParameter(5, "metric-out", "the output metric");
    
    OptionalParameter* roiOption = ret->createOptionalParameter(6, "-roi", "select a region of interest");
    roiOption->addMetricParameter(1, "roi-metric", "the roi, as a metric");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(7, "-column", "select a single column");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    ret->setHelpText(
        AString("Outputs a metric with ones for all vertices within a large enough cluster, and zeros elsewhere.  ") +
        "To apply this as a mask to the data, or to do more complicated thresholding, see -metric-math."
    );
    return ret;
}

void AlgorithmMetricFindClusters::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetric = myParams->getMetric(2);
    float minVal = (float)myParams->getDouble(3);
    float minArea = (float)myParams->getDouble(4);
    MetricFile* myMetricOut = myParams->getOutputMetric(5);
    MetricFile* myRoi = NULL;
    OptionalParameter* roiOption = myParams->getOptionalParameter(6);
    if (roiOption->m_present)
    {
        myRoi = roiOption->getMetric(1);
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
    AlgorithmMetricFindClusters(myProgObj, mySurf, myMetric, minVal, minArea, myMetricOut, myRoi, columnNum);
}

AlgorithmMetricFindClusters::AlgorithmMetricFindClusters(ProgressObject* myProgObj, const SurfaceFile* mySurf, const MetricFile* myMetric, const float& minVal,
                                                         const float& minArea, MetricFile* myMetricOut, const MetricFile* myRoi, const int& columnNum) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = mySurf->getNumberOfNodes();
    if (myMetric->getNumberOfNodes() != numNodes) throw AlgorithmException("metric does not match surface in number of vertices");
    const float* roiData = NULL;
    if (myRoi != NULL)
    {
        if (myRoi->getNumberOfNodes() != numNodes) throw AlgorithmException("roi metric does not match surface in number of vertices");
        roiData = myRoi->getValuePointerForColumn(0);
    }
    int numCols = myMetric->getNumberOfColumns();
    if (columnNum < -1 || columnNum >= numCols)
    {
        throw AlgorithmException("invalid column number");
    }
    vector<float> nodeAreas;
    mySurf->computeNodeAreas(nodeAreas);
    CaretPointer<TopologyHelper> myHelp = mySurf->getTopologyHelper();
    vector<int> toSearch;
    if (columnNum == -1)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, numCols);
        myMetricOut->setStructure(mySurf->getStructure());
        for (int c = 0; c < numCols; ++c)
        {
            myMetricOut->setColumnName(c, myMetric->getColumnName(c));
            vector<float> outData(numNodes, 0.0f);
            const float* data = myMetric->getValuePointerForColumn(c);
            vector<int> marked(numNodes, 0);
            for (int i = 0; i < numNodes; ++i)
            {
                if ((roiData == NULL || roiData[i] > 0.0f) && marked[i] == 0 && data[i] >= minVal)
                {
                    float area = 0.0f;
                    toSearch.push_back(i);
                    marked[i] = 1;//mark it when added to list to prevent multiples
                    for (int index = 0; index < (int)toSearch.size(); ++index)//NOTE: vector grows inside loop
                    {
                        int node = toSearch[index];//keep list around so we can put it into the output immediately
                        area += nodeAreas[node];
                        const vector<int32_t>& neighbors = myHelp->getNodeNeighbors(node);
                        int numNeigh = (int)neighbors.size();
                        for (int n = 0; n < numNeigh; ++n)
                        {
                            const int32_t& neighbor = neighbors[n];
                            if ((roiData == NULL || roiData[neighbor] > 0.0f) && marked[neighbor] == 0 && data[neighbor] >= minVal)
                            {
                                toSearch.push_back(neighbor);
                                marked[neighbor] = 1;
                            }
                        }
                    }
                    if (area > minArea)
                    {
                        int clusterCount = (int)toSearch.size();
                        for (int index = 0; index < clusterCount; ++index)
                        {
                            outData[toSearch[index]] = 1.0f;
                        }
                    }
                    toSearch.clear();
                }
            }
            myMetricOut->setValuesForColumn(c, outData.data());
        }
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        myMetricOut->setStructure(mySurf->getStructure());
        myMetricOut->setColumnName(0, myMetric->getColumnName(columnNum));
        vector<float> outData(numNodes, 0.0f);
        const float* data = myMetric->getValuePointerForColumn(columnNum);
        vector<int> marked(numNodes, 0);
        for (int i = 0; i < numNodes; ++i)
        {
            if ((roiData == NULL || roiData[i] > 0.0f) && marked[i] == 0 && data[i] >= minVal)
            {
                float area = 0.0f;
                toSearch.push_back(i);
                marked[i] = 1;//mark it when added to list to prevent multiples
                for (int index = 0; index < (int)toSearch.size(); ++index)//NOTE: vector grows inside loop
                {
                    int node = toSearch[index];//keep list around so we can put it into the output immediately
                    area += nodeAreas[node];
                    const vector<int32_t>& neighbors = myHelp->getNodeNeighbors(node);
                    int numNeigh = (int)neighbors.size();
                    for (int n = 0; n < numNeigh; ++n)
                    {
                        const int32_t& neighbor = neighbors[n];
                        if ((roiData == NULL || roiData[neighbor] > 0.0f) && marked[neighbor] == 0 && data[neighbor] >= minVal)
                        {
                            toSearch.push_back(neighbor);
                            marked[neighbor] = 1;
                        }
                    }
                }
                if (area > minArea)
                {
                    int clusterCount = (int)toSearch.size();
                    for (int index = 0; index < clusterCount; ++index)
                    {
                        outData[toSearch[index]] = 1.0f;
                    }
                }
                toSearch.clear();
            }
        }
        myMetricOut->setValuesForColumn(0, outData.data());
    }
}

float AlgorithmMetricFindClusters::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricFindClusters::getSubAlgorithmWeight()
{
    return 0.0f;
}
