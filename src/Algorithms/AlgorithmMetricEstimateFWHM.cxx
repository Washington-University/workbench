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

#include "AlgorithmMetricEstimateFWHM.h"
#include "AlgorithmException.h"

#include "DescriptiveStatistics.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmMetricEstimateFWHM::getCommandSwitch()
{
    return "-metric-estimate-fwhm";
}

AString AlgorithmMetricEstimateFWHM::getShortDescription()
{
    return "ESTIMATE FWHM SMOOTHNESS OF A METRIC FILE";
}

OperationParameters* AlgorithmMetricEstimateFWHM::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to use for distance and neighbor information");
    
    ret->addMetricParameter(2, "metric-in", "the input metric");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(3, "-roi", "use only data within an ROI");
    roiOpt->addMetricParameter(1, "roi-metric", "the metric file to use as an ROI");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(4, "-column", "select a single column to estimate smoothness of");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    OptionalParameter* allColumnsOpt = ret->createOptionalParameter(5, "-whole-file", "estimate for the whole file at once, not each column separately");
    allColumnsOpt->createOptionalParameter(1, "-demean", "subtract the mean image before estimating smoothness");

    ret->setHelpText(
        AString("Estimates the smoothness of the metric columns, printing the estimates to standard output.  ") +
        "These estimates ignore variation in vertex spacing."
    );
    return ret;
}

void AlgorithmMetricEstimateFWHM::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetric = myParams->getMetric(2);
    OptionalParameter* roiOpt = myParams->getOptionalParameter(3);
    MetricFile* roi = NULL;
    if (roiOpt->m_present)
    {
        roi = roiOpt->getMetric(1);
    }
    OptionalParameter* columnSelect = myParams->getOptionalParameter(4);
    int columnNum = -1;
    int numColumns = myMetric->getNumberOfMaps();
    if (columnSelect->m_present)
    {
        columnNum = (int)myMetric->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (columnNum < 0 || columnNum >= numColumns)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    bool allColumns = false, demean = false;
    OptionalParameter* allColumnsOpt = myParams->getOptionalParameter(5);
    if (allColumnsOpt->m_present)
    {
        if (columnSelect->m_present) throw AlgorithmException("specifying both -column and -whole-file is not allowed");
        allColumns = true;
        demean = allColumnsOpt->getOptionalParameter(1)->m_present;
    }
    if (allColumns)
    {
        float result = estimateFWHMAllColumns(mySurf, myMetric, roi, demean);
        cout << "FWHM: " << result << endl;
    } else {
        if (columnNum == -1)
        {
            for (int i = 0; i < numColumns; ++i)
            {
                float result = estimateFWHM(mySurf, myMetric, roi, i);
                if (numColumns > 1) cout << "column " << i + 1 << " ";
                cout << "FWHM: " << result << endl;
            }
        } else {
            float result = estimateFWHM(mySurf, myMetric, roi, columnNum);
            if (numColumns > 1) cout << "column " << columnNum + 1 << " ";
            cout << "FWHM: " << result << endl;
        }
    }
}

float AlgorithmMetricEstimateFWHM::estimateFWHM(const SurfaceFile* mySurf, const MetricFile* input, const MetricFile* roi, const int64_t& column)
{
    CaretAssert(column >= 0 && column < input->getNumberOfColumns());
    int numNodes = input->getNumberOfNodes();
    if (mySurf->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("surface has different number of vertices than the input data");
    }
    const float* inCol = input->getValuePointerForColumn(column);
    const float* roiCol = NULL;
    if (roi != NULL)
    {
        if (roi->getNumberOfNodes() != numNodes)
        {
            throw AlgorithmException("roi metric has a different number of vertices than the input metric");
        }
        roiCol = roi->getValuePointerForColumn(0);
    }
    DescriptiveStatistics nodeSpacingStats;
    mySurf->getNodesSpacingStatistics(nodeSpacingStats);//this will be slow since it recomputes - should change it to returning a const reference, and make it a lazy member
    double globalAccum = 0.0, localAccum = 0.0;
    int64_t globalCount = 0, localCount = 0;
    CaretPointer<TopologyHelper> myHelp = mySurf->getTopologyHelper();
    for (int i = 0; i < numNodes; ++i)//the local difference mean will be zero, as we don't have directionality, so don't bother collecting it
    {
        if (roi == NULL || roiCol[i] > 0.0f)
        {
            float center = inCol[i];
            globalAccum += center;
            ++globalCount;
        }
    }
    float globalMean = globalAccum / globalCount;
    globalAccum = 0.0;
    for (int i = 0; i < numNodes; ++i)
    {
        if (roi == NULL || roiCol[i] > 0.0f)
        {
            float center = inCol[i];
            float tempf = center - globalMean;
            globalAccum += tempf * tempf;//don't need to recalculate count
            const vector<int32_t>& neighbors = myHelp->getNodeNeighbors(i);
            for (int j = 0; j < (int)neighbors.size(); ++j)
            {
                if (neighbors[j] > i && (roi == NULL || roiCol[neighbors[j]] > 0.0f))//collect lopsided to get correct degrees of freedom (if n-1 denom is desired), mean is assumed zero so it works out
                {
                    tempf = center - inCol[neighbors[j]];
                    localAccum += tempf * tempf;
                    ++localCount;
                }
            }
        }
    }
    float globalVariance = globalAccum / globalCount;
    float localVariance = localAccum / localCount;
    float ret = nodeSpacingStats.getMean() * sqrt(-2.0f * log(2.0f) / log(1.0f - localVariance / (2.0f * globalVariance)));
    return ret;
}

float AlgorithmMetricEstimateFWHM::estimateFWHMAllColumns(const SurfaceFile* mySurf, const MetricFile* input, const MetricFile* roi, const bool& demean)
{
    int numNodes = input->getNumberOfNodes();
    if (mySurf->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("surface has different number of vertices than the input metric");
    }
    const float* roiCol = NULL;
    if (roi != NULL)
    {
        if (roi->getNumberOfNodes() != numNodes)
        {
            throw AlgorithmException("roi metric has a different number of vertices than the input metric");
        }
        roiCol = roi->getValuePointerForColumn(0);
    }
    DescriptiveStatistics nodeSpacingStats;
    mySurf->getNodesSpacingStatistics(nodeSpacingStats);//this will be slow since it recomputes - should change it to returning a const reference, and make it a lazy member
    int numCols = input->getNumberOfColumns();
    vector<double> meanImage;
    if (demean)
    {
        meanImage.resize(numNodes, 0.0);
        for (int j = 0; j < numCols; ++j)
        {
            const float* inCol = input->getValuePointerForColumn(j);
            for (int i = 0; i < numNodes; ++i)
            {
                if (roi == NULL || roiCol[i] > 0.0f)//not that it matters, but only computing the mean inside the ROI reduces the working set somewhat
                {
                    meanImage[i] += inCol[i];
                }
            }
        }
        for (int i = 0; i < numNodes; ++i)
        {
            if (roi == NULL || roiCol[i] > 0.0f)
            {
                meanImage[i] /= numCols;
            }
        }
    }
    double globalAccum = 0.0, localAccum = 0.0;
    int64_t globalCount = 0, localCount = 0;
    CaretPointer<TopologyHelper> myHelp = mySurf->getTopologyHelper();
    vector<float> demeaned;
    if (demean) demeaned.resize(numNodes);
    for (int j = 0; j < numCols; ++j)
    {
        const float* inCol = input->getValuePointerForColumn(j);
        if (demean)
        {
            for (int i = 0; i < numNodes; ++i)
            {
                if (roi == NULL || roiCol[i] > 0.0f)
                {
                    demeaned[i] = inCol[i] - meanImage[i];
                }
            }
            inCol = demeaned.data();
        }
        for (int i = 0; i < numNodes; ++i)//the local difference mean will be zero, as we don't have directionality, so don't bother collecting it
        {
            if (roi == NULL || roiCol[i] > 0.0f)
            {
                float center = inCol[i];
                globalAccum += center;
                ++globalCount;
            }
        }
    }
    float globalMean = globalAccum / globalCount;
    globalAccum = 0.0;
    for (int j = 0; j < numCols; ++j)
    {
        const float* inCol = input->getValuePointerForColumn(j);
        if (demean)
        {
            for (int i = 0; i < numNodes; ++i)
            {
                if (roi == NULL || roiCol[i] > 0.0f)
                {
                    demeaned[i] = inCol[i] - meanImage[i];
                }
            }
            inCol = demeaned.data();
        }
        for (int i = 0; i < numNodes; ++i)
        {
            if (roi == NULL || roiCol[i] > 0.0f)
            {
                float center = inCol[i];
                float tempf = center - globalMean;
                globalAccum += tempf * tempf;//don't need to recalculate count
                const vector<int32_t>& neighbors = myHelp->getNodeNeighbors(i);
                for (int j = 0; j < (int)neighbors.size(); ++j)
                {
                    if (neighbors[j] > i && (roi == NULL || roiCol[neighbors[j]] > 0.0f))//collect lopsided to get correct degrees of freedom (if n-1 denom is desired), mean is assumed zero so it works out
                    {
                        tempf = center - inCol[neighbors[j]];
                        localAccum += tempf * tempf;
                        ++localCount;
                    }
                }
            }
        }
    }
    float globalVariance = globalAccum / globalCount;
    float localVariance = localAccum / localCount;
    float ret = nodeSpacingStats.getMean() * sqrt(-2.0f * log(2.0f) / log(1.0f - localVariance / (2.0f * globalVariance)));
    return ret;
}
