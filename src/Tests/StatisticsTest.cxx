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
#include "StatisticsTest.h"
#include <cstdlib>
#include <cmath>

#include "FastStatistics.h"
#include "DescriptiveStatistics.h"

using namespace caret;
using namespace std;

StatisticsTest::StatisticsTest(const AString& identifier) : TestInterface(identifier)
{
}

void StatisticsTest::execute()
{
    const int NUM_ELEMENTS = 1 << 20;
    vector<float> myData(NUM_ELEMENTS);//dynamically allocate to not take lots of stack space
    for (int i = 0; i < NUM_ELEMENTS; ++i)
    {
        myData[i] = (rand() * 100.0f / RAND_MAX) - 50.0f;
    }
    DescriptiveStatistics myFullStats;
    myFullStats.update(myData.data(), NUM_ELEMENTS);
    FastStatistics myFastStats(myData.data(), NUM_ELEMENTS);
    float exacttolerance = myFullStats.getPopulationStandardDeviation() * 0.000001f;
    float approxtolerance = myFullStats.getPopulationStandardDeviation() * 0.01f;
    if (abs(myFullStats.getMinimumValue() - myFastStats.getMin()) > exacttolerance)
    {
        setFailed(AString("mismatch in min, full: ") + AString::number(myFullStats.getMinimumValue()) + ", fast: " + AString::number(myFastStats.getMin()));
    }
    if (abs(myFullStats.getMaximumValue() - myFastStats.getMax()) > exacttolerance)
    {
        setFailed(AString("mismatch in max, full: ") + AString::number(myFullStats.getMaximumValue()) + ", fast: " + AString::number(myFastStats.getMax()));
    }
    if (abs(myFullStats.getMean() - myFastStats.getMean()) > exacttolerance)
    {
        setFailed(AString("mismatch in mean, full: ") + AString::number(myFullStats.getMean()) + ", fast: " + AString::number(myFastStats.getMean()));
    }
    if (abs(myFullStats.getStandardDeviationSample() - myFastStats.getSampleStdDev()) > exacttolerance)
    {
        setFailed(AString("mismatch in sample stddev, full: ") + AString::number(myFullStats.getStandardDeviationSample()) + ", fast: " + AString::number(myFastStats.getSampleStdDev()));
    }
    if (abs(myFullStats.getPopulationStandardDeviation() - myFastStats.getPopulationStdDev()) > exacttolerance)
    {
        setFailed(AString("mismatch in population stddev, full: ") + AString::number(myFullStats.getPopulationStandardDeviation()) + ", fast: " + AString::number(myFastStats.getPopulationStdDev()));
    }
    if (abs(myFullStats.getMedian() - myFastStats.getApproximateMedian()) > approxtolerance)
    {
        setFailed(AString("mismatch in median, full: ") + AString::number(myFullStats.getMedian()) + ", fast: " + AString::number(myFastStats.getApproximateMedian()));
    }
    if (abs(myFullStats.getPositivePercentile(90.0f) - myFastStats.getApproxPositivePercentile(90.0f)) > approxtolerance)
    {
        setFailed(AString("mismatch in 90% positive percentile, full: ") + AString::number(myFullStats.getPositivePercentile(90.0f)) + ", fast: " + AString::number(myFastStats.getApproxPositivePercentile(90.0f)));
    }
    if (abs(myFullStats.getNegativePercentile(90.0f) - myFastStats.getApproxNegativePercentile(90.0f)) > approxtolerance)
    {
        setFailed(AString("mismatch in 90% negative percentile, full: ") + AString::number(myFullStats.getNegativePercentile(90.0f)) + ", fast: " + AString::number(myFastStats.getApproxNegativePercentile(90.0f)));
    }
}
