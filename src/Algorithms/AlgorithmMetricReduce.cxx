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

#include "AlgorithmMetricReduce.h"
#include "AlgorithmException.h"
#include "MetricFile.h"
#include "ReductionOperation.h"

#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmMetricReduce::getCommandSwitch()
{
    return "-metric-reduce";
}

AString AlgorithmMetricReduce::getShortDescription()
{
    return "PERFORM REDUCTION OPERATION ACROSS METRIC COLUMNS";
}

OperationParameters* AlgorithmMetricReduce::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addMetricParameter(1, "metric-in", "the metric to reduce");
    
    ret->addStringParameter(2, "operation", "the reduction operator to use");
    
    ret->addMetricOutputParameter(3, "metric-out", "the output metric");
    
    ret->setHelpText(
        AString("For each surface vertex, takes the data across columns as a vector, and performs the specified reduction on it, putting the result ") +
        "into the single output column at that vertex.  The reduction operators are as follows:\n\n" + ReductionOperation::getHelpInfo()
    );
    return ret;
}

void AlgorithmMetricReduce::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    MetricFile* metricIn = myParams->getMetric(1);
    AString opString = myParams->getString(2);
    MetricFile* metricOut = myParams->getOutputMetric(3);
    bool ok = false;
    ReductionEnum::Enum myReduce = ReductionEnum::fromName(opString, &ok);
    if (!ok) throw AlgorithmException("unrecognized operation string '" + opString + "'");
    AlgorithmMetricReduce(myProgObj, metricIn, myReduce, metricOut);
}

AlgorithmMetricReduce::AlgorithmMetricReduce(ProgressObject* myProgObj, const MetricFile* metricIn, const ReductionEnum::Enum& myReduce, MetricFile* metricOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = metricIn->getNumberOfNodes();
    int numCols = metricIn->getNumberOfColumns();
    if (numCols < 1 || numNodes < 1) throw AlgorithmException("input must have at least 1 column and 1 vertex");
    metricOut->setNumberOfNodesAndColumns(numNodes, 1);
    metricOut->setStructure(metricIn->getStructure());
    metricOut->setColumnName(0, ReductionEnum::toName(myReduce));
    vector<float> scratch(numCols);
    for (int node = 0; node < numNodes; ++node)
    {
        for (int col = 0; col < numCols; ++col)
        {
            scratch[col] = metricIn->getValue(node, col);
        }
        metricOut->setValue(node, 0, ReductionOperation::reduce(scratch.data(), numCols, myReduce));
    }
}

float AlgorithmMetricReduce::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricReduce::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
