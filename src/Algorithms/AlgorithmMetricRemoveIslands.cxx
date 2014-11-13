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

#include "AlgorithmMetricRemoveIslands.h"
#include "AlgorithmException.h"

#include "MetricFile.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

#include <utility>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmMetricRemoveIslands::getCommandSwitch()
{
    return "-metric-remove-islands";
}

AString AlgorithmMetricRemoveIslands::getShortDescription()
{
    return "REMOVE ISLANDS FROM AN ROI METRIC";
}

OperationParameters* AlgorithmMetricRemoveIslands::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to use for neighbor information");
    
    ret->addMetricParameter(2, "metric-in", "the input ROI metric");
    
    ret->addMetricOutputParameter(3, "metric-out", "the output ROI metric");
    
    OptionalParameter* corrAreaOpt = ret->createOptionalParameter(4, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    corrAreaOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    ret->setHelpText(
        AString("Finds all connected areas in the ROI, and zeros out all but the largest one, in terms of surface area.")
    );
    return ret;
}

void AlgorithmMetricRemoveIslands::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetric = myParams->getMetric(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    MetricFile* corrAreaMetric = NULL;
    OptionalParameter* corrAreaOpt = myParams->getOptionalParameter(4);
    if (corrAreaOpt->m_present)
    {
        corrAreaMetric = corrAreaOpt->getMetric(1);
    }
    AlgorithmMetricRemoveIslands(myProgObj, mySurf, myMetric, myMetricOut, corrAreaMetric);
}

AlgorithmMetricRemoveIslands::AlgorithmMetricRemoveIslands(ProgressObject* myProgObj, const SurfaceFile* mySurf, const MetricFile* myMetric,
                                                           MetricFile* myMetricOut, const MetricFile* corrAreaMetric) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = mySurf->getNumberOfNodes();
    if (myMetric->getNumberOfNodes() != numNodes) throw AlgorithmException("metric file has different number of nodes than the surface");
    vector<float> surfAreaData;
    const float* areaData = NULL;
    if (corrAreaMetric != NULL)
    {
        if (corrAreaMetric->getNumberOfNodes() != numNodes) throw AlgorithmException("corrected vertex area metric file has different number of nodes than the surface");
        areaData = corrAreaMetric->getValuePointerForColumn(0);
    } else {
        mySurf->computeNodeAreas(surfAreaData);
        areaData = surfAreaData.data();
    }
    int numCols = myMetric->getNumberOfColumns();
    myMetricOut->setNumberOfNodesAndColumns(numNodes, numCols);
    myMetricOut->setStructure(myMetric->getStructure());
    for (int col = 0; col < numCols; ++col)
    {
        vector<pair<float, vector<int> > > areas;
        vector<int> used(numNodes, 0);
        CaretPointer<TopologyHelper> myHelp = mySurf->getTopologyHelper();
        const float* roiData = myMetric->getValuePointerForColumn(col);
        myMetricOut->setColumnName(col, myMetric->getColumnName(col));
        for (int i = 0; i < numNodes; ++i)
        {
            if (used[i] == 0 && roiData[i] > 0.0f)
            {
                areas.push_back(make_pair(0.0f, vector<int>()));
                vector<int>& thisAreaMembers = areas.back().second;
                float& thisSurfArea = areas.back().first;
                thisAreaMembers.push_back(i);
                thisSurfArea += areaData[i];
                used[i] = 1;
                vector<int> mystack;
                mystack.push_back(i);
                while (!mystack.empty())
                {
                    int curnode = mystack.back();
                    mystack.pop_back();
                    const vector<int32_t>& neighbors = myHelp->getNodeNeighbors(curnode);
                    int numNeigh = (int)neighbors.size();
                    for (int j = 0; j < numNeigh; ++j)
                    {
                        int thisneigh = neighbors[j];
                        if (used[thisneigh] == 0 && roiData[thisneigh] > 0.0f)
                        {
                            used[thisneigh] = 1;
                            thisAreaMembers.push_back(thisneigh);
                            thisSurfArea += areaData[thisneigh];
                            mystack.push_back(thisneigh);
                        }
                    }
                }
            }
        }
        vector<float> outscratch(numNodes, 0.0f);
        int numAreas = (int)areas.size();
        if (numAreas > 0)
        {
            int bestIndex = 0;
            float bestArea = areas[0].first;
            for (int i = 1; i < numAreas; ++i)
            {
                float thisArea = (int)areas[i].first;
                if (thisArea > bestArea)
                {
                    bestIndex = i;
                    bestArea = thisArea;
                }
            }
            const vector<int>& thisArea = areas[bestIndex].second;
            int numAreaNodes = (int)thisArea.size();
            for (int i = 0; i < numAreaNodes; ++i)
            {
                outscratch[thisArea[i]] = 1.0f;//make it into a simple 0/1 metric, even if it wasn't before
            }
        }
        myMetricOut->setValuesForColumn(col, outscratch.data());
    }
}

float AlgorithmMetricRemoveIslands::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricRemoveIslands::getSubAlgorithmWeight()
{
    return 0.0f;
}
