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

#include "AlgorithmMetricDilate.h"
#include "AlgorithmException.h"
#include "SurfaceFile.h"
#include "MetricFile.h"

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
    
    ret->addMetricOutputParameter(3, "metric-out", "the output metric");
    
    OptionalParameter* maxIterOpt = ret->createOptionalParameter(4, "-max-iter", "maximum iterations (default iterate until nothing changes)");
    maxIterOpt->addIntegerParameter(1, "iterations", "number of iterations");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(5, "-bad-node-roi", "specify an roi of nodes to overwrite, rather than nodes with value zero");
    roiOpt->addMetricParameter(1, "roi-metric", "metric file, all positive values denote nodes to have their values replaced");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(6, "-column", "select a single column to dilate");
    columnSelect->addIntegerParameter(1, "column-number", "the column number");
    
    ret->setHelpText(
        AString("For all metric nodes that are designated as bad and that have neighbors designated as good, replace their values with the average ") +
        "of their good neighbors, and mark them as good.  Repeat until either no change happens, or the maximum specified iterations is reached."
    );
    return ret;
}

void AlgorithmMetricDilate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    MetricFile* myMetric = myParams->getMetric(1);
    SurfaceFile* mySurf = myParams->getSurface(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    OptionalParameter* maxIterOpt = myParams->getOptionalParameter(4);
    int iterMax = -1;
    if (maxIterOpt->m_present)
    {
        iterMax = (int)maxIterOpt->getInteger(1);
    }
    OptionalParameter* roiOpt = myParams->getOptionalParameter(5);
    MetricFile* badNodeRoi = NULL;
    if (roiOpt->m_present)
    {
        badNodeRoi = roiOpt->getMetric(1);
    }
    OptionalParameter* columnSelect = myParams->getOptionalParameter(6);
    int columnNum = -1;
    if (columnSelect->m_present)
    {//set up to use the single column
        columnNum = (int)columnSelect->getInteger(1);//get the integer argument to the option
        if (columnNum < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    AlgorithmMetricDilate(myProgObj, myMetric, mySurf, myMetricOut, iterMax, badNodeRoi, columnNum);//executes the algorithm
}

AlgorithmMetricDilate::AlgorithmMetricDilate(ProgressObject* myProgObj, const MetricFile* myMetric, const SurfaceFile* mySurf, MetricFile* myMetricOut, int iterMax, const MetricFile* badNodeRoi, int columnNum) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = mySurf->getNumberOfNodes();
    if (numNodes != myMetric->getNumberOfNodes())
    {
        throw AlgorithmException("surface and metric number of nodes do not match");
    }
    if (badNodeRoi != NULL && badNodeRoi->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("bad node roi number of nodes does not match");
    }
    if (columnNum < -1 || columnNum >= myMetric->getNumberOfColumns())
    {
        throw AlgorithmException("invalid column specified");
    }
    if (iterMax < -1 || iterMax == 0)
    {
        throw AlgorithmException("invalid number of iterations specified");
    }
    vector<int> nodelistA, nodelistB;
    vector<int>* curList = &nodelistA, *nextList = &nodelistB, *tempList;
    CaretArray<int> markArray(numNodes);
    CaretArray<float> colScratch(numNodes);
    vector<float> myAreas;
    mySurf->computeNodeAreas(myAreas);
    if (columnNum == -1)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, myMetric->getNumberOfColumns());
        for (int thisCol = 0; thisCol < myMetric->getNumberOfColumns(); ++thisCol)
        {
            const float* myInputData = myMetric->getValuePointerForColumn(thisCol);
            myMetricOut->setColumnName(thisCol, myMetric->getColumnName(thisCol) + " dilated");
            for (int i = 0; i < numNodes; ++i)
            {
                colScratch[i] = myInputData[i];//copy everything over, including what we may overwrite
            }
            curList->clear();
            if (badNodeRoi == NULL)
            {
                for (int i = 0; i < numNodes; ++i)
                {
                    if (myInputData[i] == 0.0f)
                    {
                        curList->push_back(i);
                        markArray[i] = 1;
                    } else {
                        markArray[i] = 0;
                    }
                }
            } else {
                const float* myRoiData = badNodeRoi->getValuePointerForColumn(thisCol);
                for (int i = 0; i < numNodes; ++i)
                {
                    if (myRoiData[i] > 0.0f)
                    {
                        curList->push_back(i);
                        markArray[i] = 1;
                    } else {
                        markArray[i] = 0;
                    }
                }
            }
            int iterCount = 0;
            bool changed = true;
            while ((iterMax == -1 || iterCount < iterMax) && changed)
            {
                changed = false;
                ++iterCount;
                int nextIter = iterCount + 1;
                CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
                int listSize = curList->size();
                nextList->clear();
                for (int i = 0; i < listSize; ++i)
                {
                    int thisNode = (*curList)[i];
                    const vector<int>& myNeighbors = myTopoHelp->getNodeNeighbors(thisNode);
                    float totalWeight = 0.0f;
                    float totalWeightedSum = 0.0f;
                    int numNeigh = (int)myNeighbors.size();
                    for (int j = 0; j < numNeigh; ++j)
                    {
                        int thisNeigh = myNeighbors[j];
                        if (markArray[thisNeigh] < iterCount)//optimization: markArray contains the iteration that gave it a value, or the current iteration otherwise
                        {
                            float weight = myAreas[thisNeigh];
                            totalWeight += weight;
                            totalWeightedSum += weight * colScratch[thisNeigh];
                        }
                    }
                    if (totalWeight != 0.0f)
                    {
                        colScratch[thisNode] = totalWeightedSum / totalWeight;
                        changed = true;
                    } else {
                        markArray[thisNode] = nextIter;
                        nextList->push_back(thisNode);
                    }
                }
                tempList = curList;//pointer swap rather than vector copy
                curList = nextList;
                nextList = tempList;
            }
            myMetricOut->setValuesForColumn(thisCol, colScratch.getArray());
        }
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        const float* myInputData = myMetric->getValuePointerForColumn(columnNum);
        myMetricOut->setColumnName(0, myMetric->getColumnName(columnNum) + " dilated");
        for (int i = 0; i < numNodes; ++i)
        {
            colScratch[i] = myInputData[i];//copy everything over, including what we may overwrite
        }
        if (badNodeRoi == NULL)
        {
            curList->clear();
            for (int i = 0; i < numNodes; ++i)
            {
                if (myInputData[i] == 0.0f)
                {
                    curList->push_back(i);
                    markArray[i] = 1;
                } else {
                    markArray[i] = 0;
                }
            }
        } else {
            const float* myRoiData = badNodeRoi->getValuePointerForColumn(columnNum);
            for (int i = 0; i < numNodes; ++i)
            {
                if (myRoiData[i] > 0.0f)
                {
                    curList->push_back(i);
                    markArray[i] = 1;
                } else {
                    markArray[i] = 0;
                }
            }
        }
        int iterCount = 0;
        bool changed = true;
        while ((iterMax == -1 || iterCount < iterMax) && changed)
        {
            changed = false;
            ++iterCount;
            int nextIter = iterCount + 1;
            CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
            int listSize = curList->size();
            nextList->clear();
            for (int i = 0; i < listSize; ++i)
            {
                int thisNode = (*curList)[i];
                const vector<int>& myNeighbors = myTopoHelp->getNodeNeighbors(thisNode);
                float totalWeight = 0.0f;
                float totalWeightedSum = 0.0f;
                int numNeigh = (int)myNeighbors.size();
                for (int j = 0; j < numNeigh; ++j)
                {
                    int thisNeigh = myNeighbors[j];
                    if (markArray[thisNeigh] < iterCount)//optimization: markArray contains the iteration that gave it a value, or the current iteration otherwise
                    {
                        float weight = myAreas[thisNeigh];
                        totalWeight += weight;
                        totalWeightedSum += weight * colScratch[thisNeigh];
                    }
                }
                if (totalWeight != 0.0f)
                {
                    colScratch[thisNode] = totalWeightedSum / totalWeight;
                    changed = true;
                } else {
                    markArray[thisNode] = nextIter;
                    nextList->push_back(thisNode);
                }
            }
            tempList = curList;//pointer swap rather than vector copy
            curList = nextList;
            nextList = tempList;
        }
        myMetricOut->setValuesForColumn(columnNum, colScratch.getArray());
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
