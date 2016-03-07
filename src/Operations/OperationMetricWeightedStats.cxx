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

#include "OperationMetricWeightedStats.h"
#include "OperationException.h"

#include "CaretHeap.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace caret;
using namespace std;

AString OperationMetricWeightedStats::getCommandSwitch()
{
    return "-metric-weighted-stats";
}

AString OperationMetricWeightedStats::getShortDescription()
{
    return "WEIGHTED SPATIAL STATISTICS ON A METRIC FILE";
}

OperationParameters* OperationMetricWeightedStats::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addMetricParameter(1, "metric-in", "the input metric");
    
    OptionalParameter* areaSurfOpt = ret->createOptionalParameter(2, "-area-surface", "use vertex areas as weights");
    areaSurfOpt->addSurfaceParameter(1, "area-surface", "the surface to use for vertex areas");
    
    OptionalParameter* weightMetricOpt = ret->createOptionalParameter(3, "-weight-metric", "use weights from a metric file");
    weightMetricOpt->addMetricParameter(1, "weight-metric", "metric file containing the weights");
    
    OptionalParameter* columnOpt = ret->createOptionalParameter(4, "-column", "only display output for one column");
    columnOpt->addStringParameter(1, "column", "the column number or name");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(5, "-roi", "only consider data inside an roi");
    roiOpt->addMetricParameter(1, "roi-metric", "the roi, as a metric file");
    roiOpt->createOptionalParameter(2, "-match-maps", "each column of input uses the corresponding column from the roi file");
    
    ret->createOptionalParameter(6, "-mean", "compute weighted mean");
    
    OptionalParameter* stdevOpt = ret->createOptionalParameter(7, "-stdev", "compute weighted standard deviation");
    stdevOpt->createOptionalParameter(1, "-sample", "estimate population stdev from the sample");
    
    OptionalParameter* percentileOpt = ret->createOptionalParameter(8, "-percentile", "compute weighted percentile");
    percentileOpt->addDoubleParameter(1, "percent", "the percentile to find");
    
    ret->createOptionalParameter(9, "-sum", "compute weighted sum");
    
    ret->createOptionalParameter(10, "-show-map-name", "print map index and name before each output");
    
    ret->setHelpText(
        AString("For each column of the input, a single number is printed, resulting from the specified operation.  ") +
        "You must specify exactly one of -area-surface or -weight-metric.  " +
        "Use -column to only give output for a single column.  " +
        "Use -roi to consider only the data within a region.  " +
        "Exactly one of -reduce or -percentile must be specified, and exactly one of -mean, -stdev, -percentile or -sum must be specified.\n\n" +
        "Using -sum with -area-surface (or -weight-metric with a metric containing similar data) is equivalent to integrating with respect to surface area.  " +
        "For example, if you want to find the surface area within an roi, do this:\n\n" +
        "$ wb_command -metric-weighted-stats roi.func.gii -sum -area-surface midthickness.surf.gii"
    );
    return ret;
}

namespace
{
    enum OperationType
    {
        MEAN,
        STDEV,
        SAMPSTDEV,
        PERCENTILE,
        SUM
    };
    
    float doOperation(const float* data, const float* weights, const int& numNodes, const OperationType& myop, const float* roiData, const float& argument)
    {//argument is only used for percentile currently
        const float* useData = data, *useWeights = weights;
        int numUse = numNodes;
        vector<float> dataScratch, weightScratch;//for when we have an ROI
        if (roiData != NULL)
        {
            dataScratch.reserve(numNodes);
            weightScratch.reserve(numNodes);
            for (int i = 0; i < numNodes; ++i)
            {
                if (roiData[i] > 0.0f)
                {
                    dataScratch.push_back(data[i]);
                    weightScratch.push_back(weights[i]);
                }
            }
            if (dataScratch.size() < 1) throw OperationException("roi contains no vertices");
            useData = dataScratch.data();
            useWeights = weightScratch.data();
            numUse = (int)dataScratch.size();
        }
        switch(myop)
        {
            case SUM:
            case MEAN:
            case STDEV:
            case SAMPSTDEV://these all start the same way
            {
                double accum = 0.0, weightsum = 0.0;
                for (int i = 0; i < numUse; ++i)
                {
                    accum += useData[i] * useWeights[i];
                    weightsum += useWeights[i];
                }
                if (myop == SUM) return accum;
                const float mean = accum / weightsum;
                if (myop == MEAN) return mean;
                accum = 0.0;
                double weightsum2 = 0.0;//for weighted sample stdev
                for (int i = 0; i < numUse; ++i)
                {
                    float tempf = useData[i] - mean;
                    accum += useWeights[i] * tempf * tempf;
                    weightsum2 += useWeights[i] * useWeights[i];
                }
                if (myop == STDEV) return sqrt(accum / weightsum);
                CaretAssert(myop == SAMPSTDEV);
                if (numUse < 2) throw OperationException("sample standard deviation requires at least 2 elements in the roi");
                return sqrt(accum / (weightsum - weightsum2 / weightsum));//http://en.wikipedia.org/wiki/Weighted_arithmetic_mean#Weighted_sample_variance
            }
            case PERCENTILE:
            {
                CaretAssert(argument >= 0.0f && argument <= 100.0f);
                if (numUse == 1) return useData[0];//would need special handling anyway, so get it early
                CaretSimpleMinHeap<float, float> sorter;
                double weightaccum = 0.0;//double will usually prevent adding weights in a different order from getting a different answer
                for (int i = 0; i < numUse; ++i)
                {
                    if (useWeights[i] < 0.0f) throw OperationException("negative weights not allowed in weighted percentile");
                    weightaccum += useWeights[i];
                    sorter.push(useWeights[i], useData[i]);//sort by value
                }
                float targetWeight = argument / 100.0f * weightaccum;
                float lastData, nextData;
                float lastWeight = sorter.pop(&lastData);
                weightaccum = lastWeight;
                float nextWeight = sorter.top(&nextData);
                int position = 1;//because the first and last sections get special treatment to not have flat ends on the function
                while (weightaccum + nextWeight * 0.5f < targetWeight && sorter.size() > 1)
                {
                    ++position;
                    sorter.pop();
                    weightaccum += nextWeight;
                    lastWeight = nextWeight;
                    lastData = nextData;
                    nextWeight = sorter.top(&nextData);
                }
                if (targetWeight < weightaccum)
                {
                    if (position == 1)
                    {//stretch interpolation at first position to the edge
                        return lastData + (nextData - lastData) * 0.5f * ((targetWeight - weightaccum) / lastWeight + 1.0f);
                    } else {
                        return lastData + (nextData - lastData) * 0.5f * ((targetWeight - weightaccum) / (lastWeight * 0.5f) + 1.0f);
                    }
                } else {
                    if (position == numUse - 1)
                    {//ditto
                        return (lastData + nextData) * 0.5f + (nextData - lastData) * 0.5f * (targetWeight - weightaccum) / nextWeight;
                    } else {
                        return (lastData + nextData) * 0.5f + (nextData - lastData) * 0.5f * (targetWeight - weightaccum) / (nextWeight * 0.5f);
                    }
                }
            }
        }
        CaretAssert(false);//make sure execution never actually reaches end of function
        throw OperationException("internal error in weighted stats");
    }
}

void OperationMetricWeightedStats::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    MetricFile* input = myParams->getMetric(1);
    int numNodes = input->getNumberOfNodes();
    int numCols = input->getNumberOfColumns();
    vector<float> areaData;
    const float* useWeights = NULL;
    OptionalParameter* areaSurfOpt = myParams->getOptionalParameter(2);
    if (areaSurfOpt->m_present)
    {
        SurfaceFile* mySurf = areaSurfOpt->getSurface(1);
        if (mySurf->getNumberOfNodes() != numNodes) throw OperationException("area surface has different number of vertices than input metric");
        mySurf->computeNodeAreas(areaData);
        useWeights = areaData.data();
    }
    OptionalParameter* weightMetricOpt = myParams->getOptionalParameter(3);
    if (weightMetricOpt->m_present)
    {
        if (useWeights != NULL) throw OperationException("you may not specify both -area-surface and -weight-metric");
        MetricFile* myWeights = weightMetricOpt->getMetric(1);
        if (myWeights->getNumberOfNodes() != numNodes) throw OperationException("weight metric has different number of vertices than input metric");
        useWeights = myWeights->getValuePointerForColumn(0);
    }
    if (useWeights == NULL) throw OperationException("you must specify either -area-surface or -weight-metric");
    int column = -1;
    OptionalParameter* columnOpt = myParams->getOptionalParameter(4);
    if (columnOpt->m_present)
    {
        column = input->getMapIndexFromNameOrNumber(columnOpt->getString(1));
        if (column < 0) throw OperationException("invalid column specified");
    }
    bool matchColumnMode = false;
    MetricFile* myRoi = NULL;
    const float* roiData = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(5);
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getMetric(1);
        if (myRoi->getNumberOfNodes() != numNodes) throw OperationException("roi doesn't match input in number of vertices");
        if (roiOpt->getOptionalParameter(2)->m_present)
        {
            if (myRoi->getNumberOfColumns() != numCols)
            {
                throw OperationException("-match-maps specified, but roi has different number of columns than input");
            }
            matchColumnMode = true;
        } else {
            roiData = myRoi->getValuePointerForColumn(0);
        }
    }
    bool haveOp = false;
    OperationType myop;
    if (myParams->getOptionalParameter(6)->m_present)
    {
        haveOp = true;
        myop = MEAN;
    }
    OptionalParameter* stdevOpt = myParams->getOptionalParameter(7);
    if (stdevOpt->m_present)
    {
        if (haveOp) throw OperationException("you may only specify one operation");
        haveOp = true;
        if (stdevOpt->getOptionalParameter(1)->m_present)
        {
            myop = SAMPSTDEV;
        } else {
            myop = STDEV;
        }
    }
    float argument = -1.0f;
    OptionalParameter* percentileOpt = myParams->getOptionalParameter(8);
    if (percentileOpt->m_present)
    {
        if (haveOp) throw OperationException("you may only specify one operation");
        haveOp = true;
        myop = PERCENTILE;
        argument = percentileOpt->getDouble(1);
        if (!(argument >= 0.0f && argument <= 100.0f)) throw OperationException("percentile must be between 0 and 100");
    }
    if (myParams->getOptionalParameter(9)->m_present)
    {
        if (haveOp) throw OperationException("you may only specify one operation");
        haveOp = true;
        myop = SUM;
    }
    if (!haveOp) throw OperationException("you must specify an operation");
    bool showMapName = myParams->getOptionalParameter(10)->m_present;
    if (column == -1)
    {
        for (int i = 0; i < numCols; ++i)
        {//store result before printing anything, in case it throws while computing
            if (matchColumnMode)
            {
                roiData = myRoi->getValuePointerForColumn(i);
            }
            const float result = doOperation(input->getValuePointerForColumn(i), useWeights, numNodes, myop, roiData, argument);
            if (showMapName) cout << AString::number(i + 1) << ": " << input->getMapName(i) << ": ";
            stringstream resultsstr;
            resultsstr << setprecision(7) << result;
            cout << resultsstr.str() << endl;
        }
    } else {
        if (matchColumnMode)
        {
            roiData = myRoi->getValuePointerForColumn(column);
        }
        const float result = doOperation(input->getValuePointerForColumn(column), useWeights, numNodes, myop, roiData, argument);
        if (showMapName) cout << AString::number(column + 1) << ": " << input->getMapName(column) << ": ";
        stringstream resultsstr;
        resultsstr << setprecision(7) << result;
        cout << resultsstr.str() << endl;
    }
}
