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

#include "OperationMetricEstimateFWHM.h"
#include "OperationException.h"

#include "DescriptiveStatistics.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

#include <cmath>

using namespace caret;
using namespace std;

AString OperationMetricEstimateFWHM::getCommandSwitch()
{
    return "-metric-estimate-fwhm";
}

AString OperationMetricEstimateFWHM::getShortDescription()
{
    return "ESTIMATE FWHM SMOOTHNESS OF A METRIC FILE";
}

OperationParameters* OperationMetricEstimateFWHM::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to use for distance and neighbor information");
    
    ret->addMetricParameter(2, "metric-in", "the input metric");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(3, "-roi", "use only data within an ROI");
    roiOpt->addMetricParameter(1, "roi-metric", "the metric file to use as an ROI");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(4, "-column", "select a single column to estimate smoothness of");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    ret->setHelpText(
        AString("Estimates the smoothness of the metric columns, printing the estimates to standard output.  ") +
        "These estimates ignore variation in vertex spacing."
    );
    return ret;
}

void OperationMetricEstimateFWHM::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
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
            throw OperationException("invalid column specified");
        }
    }
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

float OperationMetricEstimateFWHM::estimateFWHM(const SurfaceFile* mySurf, const MetricFile* input, const MetricFile* roi, const int64_t& column)
{
    CaretAssert(column >= 0 && column < input->getNumberOfColumns());
    int numNodes = input->getNumberOfNodes();
    const float* inCol = input->getValuePointerForColumn(column);
    const float* roiCol = NULL;
    if (roi != NULL)
    {
        if (roi->getNumberOfNodes() != numNodes)
        {
            throw OperationException("roi metric has a different number of nodes than the input metric");
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
