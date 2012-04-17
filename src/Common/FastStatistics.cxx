
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#include "FastStatistics.h"
#include "CaretPointer.h"
#include <cmath>

using namespace caret;
using namespace std;

const int NUM_BUCKETS_PERCENTILE_HIST = 10000;//10,000 will probably allow us to approximate the percentiles pretty closely, and eats only 80K of memory for each histogram

FastStatistics::FastStatistics()
{
    reset();
}

FastStatistics::FastStatistics(const float* data, const int64_t& dataCount)
{
    update(data, dataCount);
}

void FastStatistics::reset()
{
    m_posCount = 0;
    m_zeroCount = 0;
    m_negCount = 0;
    m_infCount = 0;
    m_negInfCount = 0;
    m_nanCount = 0;
    m_mean = 0.0f;
    m_stdDevPop = 0.0f;
    m_stdDevSample = 0.0f;
    m_mostNeg = 0.0f;
    m_leastNeg = 0.0f;
    m_leastPos = 0.0f;
    m_mostPos = 0.0f;
}

void FastStatistics::update(const float* data, const int64_t& dataCount)
{
    reset();
    CaretArray<float> positives(dataCount), negatives(dataCount);
    double sum = 0.0;//for numerical stability
    for (int64_t i = 0; i < dataCount; ++i)
    {
        if (data[i] != data[i])
        {
            ++m_nanCount;
            continue;//skip NaNs
        }
        if (data[i] == 0.0f)//test exactly zero (negative zero also tests equal), in case someone wants stats on something with miniscule values (percent of surface area per node?)
        {
            ++m_zeroCount;
        } else {
            if (data[i] < 0.0f)
            {
                if (data[i] * 2.0f == data[i])
                {
                    ++m_negInfCount;
                    continue;//skip neg infs
                } else {
                    negatives[m_negCount] = data[i];
                    ++m_negCount;
                    if (data[i] > m_leastNeg) m_leastNeg = data[i];
                    if (data[i] < m_mostNeg) m_mostNeg = data[i];
                }
            } else {
                if (data[i] * 2.0f == data[i])
                {
                    ++m_infCount;
                    continue;//skip infs
                } else {
                    positives[m_posCount] = data[i];
                    ++m_posCount;
                    if (data[i] > m_mostPos) m_mostPos = data[i];
                    if (data[i] < m_leastPos) m_leastPos = data[i];
                }
            }
        }
        if (data[i] > m_max) m_max = data[i];
        if (data[i] < m_min) m_min = data[i];
        sum += data[i];//use a two-pass method for stability, only do mean this pass
    }
    int64_t totalGood = (m_negCount + m_zeroCount + m_posCount);
    m_mean = sum / totalGood;
    float tempf;
    double sum2 = 0.0;
    for (int64_t i = 0; i < dataCount; ++i)
    {
        if (data[i] != data[i]) continue;//skip NaNs
        if (data[i] < -1.0f && (data[i] * 2.0f == data[i])) continue;//exclude -inf
        if (data[i] > 1.0f && (data[i] * 2.0f == data[i])) continue;//exclude inf
        tempf = data[i] - m_mean;
        sum2 += tempf * tempf;
    }
    if (totalGood > 0)
    {
        m_stdDevPop = sqrt(sum2 / totalGood);
        if (totalGood > 1)
        {
            m_stdDevSample = sqrt(sum2 / (totalGood - 1));
        }
    }
    m_negPercentHist.update(NUM_BUCKETS_PERCENTILE_HIST, negatives, m_negCount);
    m_posPercentHist.update(NUM_BUCKETS_PERCENTILE_HIST, positives, m_posCount);
}

void FastStatistics::update(const float* data, const int64_t& dataCount, const float& minThreshInclusive, const float& maxThreshInclusive)
{
    reset();
    CaretArray<float> positives(dataCount), negatives(dataCount);
    double sum = 0.0;//for numerical stability
    for (int64_t i = 0; i < dataCount; ++i)
    {
        if (data[i] != data[i])
        {
            ++m_nanCount;
            continue;//skip NaNs
        }
        if (data[i] < -1.0f && (data[i] * 2.0f == data[i]))
        {
            ++m_negInfCount;
            continue;//skip and count all infs, ignoring the range for now
        }
        if (data[i] > 1.0f && (data[i] * 2.0f == data[i]))
        {
            ++m_infCount;
            continue;//ditto
        }
        if (data[i] < minThreshInclusive || data[i] > maxThreshInclusive)
        {//we now have only numerical values
            continue;//skip them if they are outside the range
        }
        if (data[i] == 0.0f)//test exactly zero (negative zero also tests equal), in case someone wants stats on something with miniscule values (percent of surface area per node?)
        {
            ++m_zeroCount;
        } else {
            if (data[i] < 0.0f)
            {
                negatives[m_negCount] = data[i];
                ++m_negCount;
                if (data[i] > m_leastNeg) m_leastNeg = data[i];
                if (data[i] < m_mostNeg) m_mostNeg = data[i];
            } else {
                positives[m_posCount] = data[i];
                ++m_posCount;
                if (data[i] > m_mostPos) m_mostPos = data[i];
                if (data[i] < m_leastPos) m_leastPos = data[i];
            }
        }
        if (data[i] > m_max) m_max = data[i];
        if (data[i] < m_min) m_min = data[i];
        sum += data[i];//use a two-pass method for stability, only do mean this pass
    }
    int64_t totalGood = (m_negCount + m_zeroCount + m_posCount);
    m_mean = sum / totalGood;
    float tempf;
    double sum2 = 0.0;
    for (int64_t i = 0; i < dataCount; ++i)
    {
        if (data[i] != data[i]) continue;//skip NaNs
        if (data[i] < -1.0f && (data[i] * 2.0f == data[i])) continue;//exclude -inf
        if (data[i] > 1.0f && (data[i] * 2.0f == data[i])) continue;//exclude inf
        tempf = data[i] - m_mean;
        sum2 += tempf * tempf;
    }
    if (totalGood > 0)
    {
        m_stdDevPop = sqrt(sum2 / totalGood);
        if (totalGood > 1)
        {
            m_stdDevSample = sqrt(sum2 / (totalGood - 1));
        }
    }
    m_negPercentHist.update(NUM_BUCKETS_PERCENTILE_HIST, negatives, m_negCount);//10,000 will probably allow us to approximate the percentiles pretty closely, and eats only 80K of memory each
    m_posPercentHist.update(NUM_BUCKETS_PERCENTILE_HIST, positives, m_posCount);
}

float FastStatistics::getApproxNegativePercentile(const float& percent) const
{
    float rank = percent / 100.0f * m_negCount;//translate to rank
    rank = m_negCount - rank;//reverse it because negatives go the other direction, histogram is strictly directional towards positive
    if (rank <= 0) return m_mostNeg;
    if (rank >= m_negCount) return m_leastNeg;
    float histMin, histMax;
    m_negPercentHist.getRange(histMin, histMax);
    const vector<int64_t>& cumulative = m_negPercentHist.getHistogramCumulativeCounts();
    int numBuckets = (int)cumulative.size();
    int lowBound = -1, highBound = numBuckets, guess;//bisection search, "index" -1 is implicitly valued zero
    while (highBound - lowBound > 1)
    {
        guess = (lowBound + highBound) / 2;
        if (cumulative[guess] <= rank)
        {
            lowBound = guess;
        } else {
            highBound = guess;
        }
    }
    if (highBound == numBuckets) return m_mostPos;//the count mismatched the histogram somehow
    float bucketsize = (histMax - histMin) / numBuckets;
    int64_t curLower, curUpper = cumulative[highBound];
    if (lowBound > -1)
    {
        curLower = cumulative[lowBound];
    } else {
        curLower = 0;
    }
    if (highBound > 0 && ((highBound == 1 && cumulative[0] == 0) || cumulative[highBound - 1] == cumulative[highBound - 2]))
    {//tweak the function a bit if there is a bin that collected zero to the immediate left, to reduce discontinuities
        if (rank - curLower >= 1.0f)
        {
            ++curLower;//tweak the low end to start from one higher, to make it continuous with the tweak below
        } else {
            --highBound;//move left, because this interpolated rank doesn't fall within the highBound bucket
            --lowBound;
            curUpper = curLower + 1;//add one to the right end of the flat spot to give it nonzero slope
            while (lowBound > 0 && cumulative[highBound] == cumulative[lowBound - 1])
            {
                --lowBound;//slide left boundary over the flat spot
            }
            if (lowBound == 0 && cumulative[lowBound] == 0)
            {
                --lowBound;//including if first bucket is zero, this shouldn't happen unless all valid values are equal (and a low count of values)
            }
            if (lowBound > -1)
            {
                curLower = cumulative[lowBound];
            } else {
                curLower = 0;
            }
        }
    }
    float lowValue = histMin + (lowBound + 1) * bucketsize, highValue = histMin + (highBound + 1) * bucketsize;
    return lowValue + (highValue - lowValue) * (rank - curLower) / (curUpper - curLower);
}

float FastStatistics::getApproxPositivePercentile(const float& percent) const
{
    float rank = percent / 100.0f * m_posCount;//translate to rank
    if (rank <= 0.0f) return m_leastPos;
    if (rank >= m_posCount) return m_mostPos;
    float histMin, histMax;
    m_posPercentHist.getRange(histMin, histMax);
    const vector<int64_t>& cumulative = m_posPercentHist.getHistogramCumulativeCounts();
    int numBuckets = (int)cumulative.size();
    int lowBound = -1, highBound = numBuckets, guess;//bisection search, "index" -1 is implicitly valued zero
    while (highBound - lowBound > 1)
    {
        guess = (lowBound + highBound) / 2;
        if (cumulative[guess] <= rank)
        {
            lowBound = guess;
        } else {
            highBound = guess;
        }
    }
    if (highBound == numBuckets) return m_mostPos;//the count mismatched the histogram somehow
    float bucketsize = (histMax - histMin) / numBuckets;
    int64_t curLower, curUpper = cumulative[highBound];
    if (lowBound > -1)
    {
        curLower = cumulative[lowBound];
    } else {
        curLower = 0;
    }
    if (highBound > 0 && ((highBound == 1 && cumulative[0] == 0) || (highBound > 1 && cumulative[highBound - 1] == cumulative[highBound - 2])))
    {//tweak the function a bit if there is a bin that collected zero to the immediate left, to reduce discontinuities
        if (rank - curLower >= 1.0f)
        {
            ++curLower;//tweak the low end to start from one higher, to make it continuous with the tweak below
        } else {
            --highBound;//move left, because this interpolated rank doesn't fall within the highBound bucket
            --lowBound;
            curUpper = curLower + 1;//add one to the right end of the flat spot to give it nonzero slope
            while (lowBound > 0 && cumulative[highBound] == cumulative[lowBound - 1])
            {
                --lowBound;//slide left boundary over the flat spot
            }
            if (lowBound == 0 && cumulative[lowBound] == 0)
            {
                --lowBound;//including if first bucket is zero, this shouldn't happen unless all valid values are equal (and a low count of values)
            }
            if (lowBound > -1)
            {
                curLower = cumulative[lowBound];
            } else {
                curLower = 0;
            }
        }
    }
    float lowValue = histMin + (lowBound + 1) * bucketsize, highValue = histMin + (highBound + 1) * bucketsize;
    return lowValue + (highValue - lowValue) * (rank - curLower) / (curUpper - curLower);
}

float FastStatistics::getApproximateMedian() const
{
    int64_t totalGood = m_negCount + m_zeroCount + m_posCount;
    if (m_negCount > m_posCount)
    {
        if (m_zeroCount > (m_negCount - m_posCount))
        {
            return 0.0f;
        } else {
            return getApproxNegativePercentile((m_negCount - m_posCount - m_zeroCount) * 50.0f / totalGood);
        }
    } else {
        if (m_zeroCount > (m_posCount - m_negCount))
        {
            return 0.0f;
        } else {
            return getApproxNegativePercentile((m_posCount - m_negCount - m_zeroCount) * 50.0f / totalGood);
        }
    }
}
