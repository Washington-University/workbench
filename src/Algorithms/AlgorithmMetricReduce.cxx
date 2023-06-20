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

#include "AlgorithmMetricReduce.h"
#include "AlgorithmException.h"
#include "CaretLogger.h"
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
    
    OptionalParameter* excludeOpt = ret->createOptionalParameter(4, "-exclude-outliers", "exclude non-numeric values and outliers by standard deviation");
    excludeOpt->addDoubleParameter(1, "sigma-below", "number of standard deviations below the mean to include");
    excludeOpt->addDoubleParameter(2, "sigma-above", "number of standard deviations above the mean to include");
    
    ret->createOptionalParameter(5, "-only-numeric", "exclude non-numeric values");
    
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
    OptionalParameter* excludeOpt = myParams->getOptionalParameter(4);
    bool onlyNumeric = myParams->getOptionalParameter(5)->m_present;
    bool ok = false;
    ReductionEnum::Enum myReduce = ReductionEnum::fromName(opString, &ok);
    if (!ok) throw AlgorithmException("unrecognized operation string '" + opString + "'");
    if (excludeOpt->m_present)
    {
        if (onlyNumeric) CaretLogWarning("-only-numeric is redundant when -exclude-outliers is specified");
        AlgorithmMetricReduce(myProgObj, metricIn, myReduce, metricOut, excludeOpt->getDouble(1), excludeOpt->getDouble(2));
    } else {
        AlgorithmMetricReduce(myProgObj, metricIn, myReduce, metricOut, onlyNumeric);
    }
}

AlgorithmMetricReduce::AlgorithmMetricReduce(ProgressObject* myProgObj, const MetricFile* metricIn, const ReductionEnum::Enum& myReduce, MetricFile* metricOut, const bool& onlyNumeric) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = metricIn->getNumberOfNodes();
    int numCols = metricIn->getNumberOfColumns();
    if (numCols < 1 || numNodes < 1) throw AlgorithmException("input must have at least 1 column and 1 vertex");
    if (numCols == 1 && ! ReductionOperation::isLengthOneReasonable(myReduce))
    {
        CaretLogWarning("-metric-reduce is being used for a length=1 reduction on file '" + metricIn->getFileName() + "'");
    }
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
        if (onlyNumeric)
        {
            metricOut->setValue(node, 0, ReductionOperation::reduceOnlyNumeric(scratch.data(), numCols, myReduce));
        } else {
            metricOut->setValue(node, 0, ReductionOperation::reduce(scratch.data(), numCols, myReduce));
        }
    }
}

AlgorithmMetricReduce::AlgorithmMetricReduce(ProgressObject* myProgObj, const MetricFile* metricIn, const ReductionEnum::Enum& myReduce, MetricFile* metricOut, const float& sigmaBelow, const float& sigmaAbove) : AbstractAlgorithm(myProgObj)
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
        metricOut->setValue(node, 0, ReductionOperation::reduceExcludeDev(scratch.data(), numCols, myReduce, sigmaBelow, sigmaAbove));
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
