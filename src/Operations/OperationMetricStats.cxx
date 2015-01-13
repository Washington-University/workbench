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

#include "OperationMetricStats.h"
#include "OperationException.h"

#include "MetricFile.h"
#include "ReductionOperation.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace caret;
using namespace std;

AString OperationMetricStats::getCommandSwitch()
{
    return "-metric-stats";
}

AString OperationMetricStats::getShortDescription()
{
    return "SPATIAL STATISTICS ON A METRIC FILE";
}

OperationParameters* OperationMetricStats::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addMetricParameter(1, "metric-in", "the input metric");
    
    OptionalParameter* reduceOpt = ret->createOptionalParameter(2, "-reduce", "use a reduction operation");
    reduceOpt->addStringParameter(1, "operation", "the reduction operation");
    
    OptionalParameter* percentileOpt = ret->createOptionalParameter(3, "-percentile", "give the value at a percentile");
    percentileOpt->addDoubleParameter(1, "percent", "the percentile to find");
    
    OptionalParameter* columnOpt = ret->createOptionalParameter(4, "-column", "only display output for one column");
    columnOpt->addStringParameter(1, "column", "the column number or name");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(5, "-roi", "only consider data inside an roi");
    roiOpt->addMetricParameter(1, "roi-metric", "the roi, as a metric file");
    roiOpt->createOptionalParameter(2, "-match-maps", "each column of input uses the corresponding column from the roi file");
    
    ret->createOptionalParameter(6, "-show-map-name", "print map index and name before each output");
    
    ret->setHelpText(
        AString("For each column of the input, a single number is printed, resulting from the specified reduction or percentile operation.  ") +
        "Use -column to only give output for a single column.  " +
        "Use -roi to consider only the data within a region.  " +
        "Exactly one of -reduce or -percentile must be specified.\n\n" +
        "The argument to the -reduce option must be one of the following:\n\n" +
        ReductionOperation::getHelpInfo());
    return ret;
}

namespace
{
    float reduce(const float* data, const int& numNodes, const ReductionEnum::Enum& myop, const float* roiData)
    {
        if (roiData == NULL)
        {
            return ReductionOperation::reduce(data, numNodes, myop);
        } else {
            vector<float> toUse;
            toUse.reserve(numNodes);
            for (int i = 0; i < numNodes; ++i)
            {
                if (roiData[i] > 0.0f)
                {
                    toUse.push_back(data[i]);
                }
            }
            if (toUse.empty()) throw OperationException("roi contains no vertices");
            return ReductionOperation::reduce(toUse.data(), toUse.size(), myop);
        }
    }
    
    float percentile(const float* data, const int& numNodes, const float& percent, const float* roiData)
    {
        CaretAssert(percent >= 0.0f && percent <= 100.0f);
        vector<float> toUse;
        if (roiData == NULL)
        {
            toUse = vector<float>(data, data + numNodes);
        } else {
            toUse.reserve(numNodes);
            for (int i = 0; i < numNodes; ++i)
            {
                if (roiData[i] > 0.0f)
                {
                    toUse.push_back(data[i]);
                }
            }
        }
        if (toUse.empty()) throw OperationException("roi contains no vertices");
        sort(toUse.begin(), toUse.end());
        const float index = percent / 100.0f * (toUse.size() - 1);
        if (index <= 0) return toUse[0];
        if (index >= toUse.size() - 1) return toUse.back();
        float ipart, fpart;
        fpart = modf(index, &ipart);
        return (1.0f - fpart) * toUse[(int)ipart] + fpart * toUse[((int)ipart) + 1];
    }
}

void OperationMetricStats::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    MetricFile* input = myParams->getMetric(1);
    int numNodes = input->getNumberOfNodes();
    int numCols = input->getNumberOfColumns();
    OptionalParameter* reduceOpt = myParams->getOptionalParameter(2);
    OptionalParameter* percentileOpt = myParams->getOptionalParameter(3);
    if (reduceOpt->m_present == percentileOpt->m_present)//use == as logical xor
    {
        throw OperationException("you must use exactly one of -reduce or -percentile");
    }
    ReductionEnum::Enum myop = ReductionEnum::INVALID;
    if (reduceOpt->m_present)
    {
        bool ok = false;
        myop = ReductionEnum::fromName(reduceOpt->getString(1), &ok);
        if (!ok) throw OperationException("unrecognized reduction operation: " + reduceOpt->getString(1));
    }
    float percent = 0.0f;
    if (percentileOpt->m_present)
    {
        percent = (float)percentileOpt->getDouble(1);//use not within range to trap NaNs, just in case
        if (!(percent >= 0.0f && percent <= 100.0f)) throw OperationException("percentile must be between 0 and 100");
    }
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
    bool showMapName = myParams->getOptionalParameter(6)->m_present;
    if (column == -1)
    {
        if (reduceOpt->m_present)
        {
            for (int i = 0; i < numCols; ++i)
            {//store result before printing anything, in case it throws while computing
                if (matchColumnMode)
                {
                    roiData = myRoi->getValuePointerForColumn(i);
                }
                const float result = reduce(input->getValuePointerForColumn(i), numNodes, myop, roiData);
                if (showMapName) cout << AString::number(i + 1) << ": " << input->getMapName(i) << ": ";
                stringstream resultsstr;
                resultsstr << setprecision(7) << result;
                cout << resultsstr.str() << endl;
            }
        } else {
            CaretAssert(percentileOpt->m_present);
            for (int i = 0; i < numCols; ++i)
            {//store result before printing anything, in case it throws while computing
                if (matchColumnMode)
                {
                    roiData = myRoi->getValuePointerForColumn(i);
                }
                const float result = percentile(input->getValuePointerForColumn(i), numNodes, percent, roiData);
                if (showMapName) cout << AString::number(i + 1) << ": " << input->getMapName(i) << ": ";
                stringstream resultsstr;
                resultsstr << setprecision(7) << result;
                cout << resultsstr.str() << endl;
            }
        }
    } else {
        CaretAssert(column >= 0 && column < numCols);
        if (matchColumnMode)
        {
            roiData = myRoi->getValuePointerForColumn(column);
        }
        if (reduceOpt->m_present)
        {
            const float result = reduce(input->getValuePointerForColumn(column), numNodes, myop, roiData);
            if (showMapName) cout << AString::number(column + 1) << ": " << input->getMapName(column) << ": ";
            stringstream resultsstr;
            resultsstr << setprecision(7) << result;
            cout << resultsstr.str() << endl;
        } else {
            CaretAssert(percentileOpt->m_present);
            const float result = percentile(input->getValuePointerForColumn(column), numNodes, percent, roiData);
            if (showMapName) cout << AString::number(column + 1) << ": " << input->getMapName(column) << ": ";
            stringstream resultsstr;
            resultsstr << setprecision(7) << result;
            cout << resultsstr.str() << endl;
        }
    }
}
