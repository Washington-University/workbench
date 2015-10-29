
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

#include "FastStatistics.h"
#include "CaretPointer.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace caret;
using namespace std;

const int64_t NUM_BUCKETS_PERCENTILE_HIST = 10000;//10,000 maximum to deal with some outliers outliers until I think of a better fix

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
    m_absCount = 0;
    m_mean = 0.0f;
    m_stdDevPop = 0.0f;
    m_stdDevSample = 0.0f;
    m_mostNeg = 0.0f;
    m_leastNeg = -numeric_limits<float>::max();
    m_leastPos = numeric_limits<float>::max();
    m_mostPos = 0.0f;
    m_leastAbs = numeric_limits<float>::max();
    m_mostAbs = 0.0;
    m_min = 0.0f;
    m_max = 0.0f;
}

void FastStatistics::update(const float* data, const int64_t& dataCount)
{
    reset();
    CaretArray<float> positives(dataCount), negatives(dataCount), absolutes(dataCount);
    double sum = 0.0;//for numerical stability
    bool first = true;//so min can be positive and max can be negative
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
                    
                    absolutes[m_absCount] = -data[i];
                    if (absolutes[m_absCount] > m_mostAbs)  m_mostAbs  = absolutes[m_absCount];
                    if (absolutes[m_absCount] < m_leastAbs) m_leastAbs = absolutes[m_absCount];
                    ++m_absCount;
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
                    
                    absolutes[m_absCount] = data[i];
                    if (absolutes[m_absCount] > m_mostAbs)  m_mostAbs  = absolutes[m_absCount];
                    if (absolutes[m_absCount] < m_leastAbs) m_leastAbs = absolutes[m_absCount];
                    ++m_absCount;
                }
            }
        }
        if (data[i] > m_max || first) m_max = data[i];
        if (data[i] < m_min || first) m_min = data[i];
        sum += data[i];//use a two-pass method for stability, only do mean this pass
        first = false;
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
    int usebuckets = min(NUM_BUCKETS_PERCENTILE_HIST, dataCount);
    m_negPercentHist.update(usebuckets, negatives, m_negCount);
    m_posPercentHist.update(usebuckets, positives, m_posCount);
    m_absPercentHist.update(usebuckets, absolutes, m_absCount);
    
    if (m_negCount <= 0)
    {
        m_leastNeg = 0.0;
        m_mostNeg  = 0.0;
    }
    if (m_posCount <= 0)
    {
        m_leastPos = 0.0;
        m_mostPos  = 0.0;
    }
    if (m_absCount <= 0)
    {
        m_leastAbs = 0.0;
        m_mostAbs  = 0.0;
    }
}

void FastStatistics::update(const float* data, const int64_t& dataCount, const float& minThreshInclusive, const float& maxThreshInclusive)
{
    reset();
    CaretArray<float> positives(dataCount), negatives(dataCount), absolutes(dataCount);
    double sum = 0.0;//for numerical stability
    bool first = true;//so min can be positive and max can be negative
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
                
                absolutes[m_absCount] = -data[i];
                ++m_absCount;
            } else {
                positives[m_posCount] = data[i];
                ++m_posCount;
                if (data[i] > m_mostPos) m_mostPos = data[i];
                if (data[i] < m_leastPos) m_leastPos = data[i];
                
                absolutes[m_absCount] = data[i];
                ++m_absCount;
            }
        }
        if (data[i] > m_max || first) m_max = data[i];
        if (data[i] < m_min || first) m_min = data[i];
        sum += data[i];//use a two-pass method for stability, only do mean this pass
        first = false;
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
    int usebuckets = min(NUM_BUCKETS_PERCENTILE_HIST, dataCount);
    m_negPercentHist.update(usebuckets, negatives, m_negCount);//10,000 will probably allow us to approximate the percentiles pretty closely, and eats only 80K of memory each
    m_posPercentHist.update(usebuckets, positives, m_posCount);
    m_absPercentHist.update(usebuckets, absolutes, m_absCount);
    
    if (m_negCount <= 0)
    {
        m_leastNeg = 0.0;
        m_mostNeg  = 0.0;
    }
    if (m_posCount <= 0)
    {
        m_leastPos = 0.0;
        m_mostPos  = 0.0;
    }
    if (m_absCount <= 0)
    {
        m_leastAbs = 0.0;
        m_mostAbs  = 0.0;
    }
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

float FastStatistics::getApproxAbsolutePercentile(const float& percent) const
{
    float rank = percent / 100.0f * m_absCount;//translate to rank
    if (rank <= 0.0f) return m_leastAbs;
    if (rank >= m_absCount) return m_mostAbs;
    float histMin, histMax;
    m_absPercentHist.getRange(histMin, histMax);
    const vector<int64_t>& cumulative = m_absPercentHist.getHistogramCumulativeCounts();
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
    if (highBound == numBuckets) return m_mostAbs;//the count mismatched the histogram somehow
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

float
FastStatistics::getValuePercentileHelper(const Histogram& histogram, const float numberOfDataValues, const bool negativeDataFlag, const float value)
{
    float percentile = 0.0;
    
    const std::vector<int64_t>& cumulativeBuckets = histogram.getHistogramCumulativeCounts();
    if ((numberOfDataValues > 0)
        && (! cumulativeBuckets.empty())) {
        float minValue = 0.0;
        float maxValue = 0.0;
        histogram.getRange(minValue,
                                  maxValue);
        if (value < minValue) {
            if (negativeDataFlag) {
                percentile = 100.0;
            }
            else {
                percentile = 0.0;
            }
        }
        else if (value > maxValue) {
            if (negativeDataFlag) {
                percentile = 0;
            }
            else {
                percentile = 100.0;
            }
        }
        else {
            const float histoRange = maxValue - minValue;
            if (histoRange > 0.0) {
                const int64_t numBuckets = cumulativeBuckets.size();
                int64_t bucketIndex = static_cast<int>(((value - minValue) / histoRange) * numBuckets);
                if (bucketIndex < 0) {
                    bucketIndex = 0;
                }
                else if (bucketIndex >= numBuckets) {
                    bucketIndex = numBuckets - 1;
                }
                
                CaretAssertVectorIndex(cumulativeBuckets,
                                       bucketIndex);
                float cumulativeValue = cumulativeBuckets[bucketIndex];
                
                if (negativeDataFlag) {
                    CaretAssertVectorIndex(cumulativeBuckets, numBuckets - 1);
                    cumulativeValue = cumulativeBuckets[numBuckets - 1] - cumulativeBuckets[bucketIndex];
                }
                
                percentile = (cumulativeValue / numberOfDataValues) * 100.0;
            }
        }
    }
    
    return percentile;
}

float
FastStatistics::getNegativeValuePercentile(const float value) const
{
    return getValuePercentileHelper(m_negPercentHist, m_negCount, true, value);
}

float
FastStatistics::getAbsoluteValuePercentile(const float value) const
{
    float dataValue = value;
    if (dataValue < 0.0) {
        dataValue = -dataValue;
    }
    return getValuePercentileHelper(m_absPercentHist, m_absCount, false, dataValue);
}

float
FastStatistics::getPositiveValuePercentile(const float value) const
{
    return getValuePercentileHelper(m_posPercentHist, m_posCount, false, value);
    
//    float percentile = 0.0;
//    
//    const std::vector<int64_t>& cumulativeBuckets = m_posPercentHist.getHistogramCumulativeCounts();
//    if ( ! cumulativeBuckets.empty()) {
//        float minValue = 0.0;
//        float maxValue = 0.0;
//        m_posPercentHist.getRange(minValue,
//                                  maxValue);
//        if (value < minValue) {
//            percentile = 0.0;
//        }
//        else if (value > maxValue) {
//            percentile = 100.0;
//        }
//        else {
//            const float histoRange = maxValue - minValue;
//            if (histoRange > 0.0) {
//                const int64_t numBuckets = cumulativeBuckets.size();
//                int64_t bucketIndex = static_cast<int>(((value - minValue) / histoRange) * numBuckets);
//                if (bucketIndex < 0) {
//                    bucketIndex = 0;
//                }
//                else if (bucketIndex >= numBuckets) {
//                    bucketIndex = numBuckets - 1;
//                }
//                
//                CaretAssertVectorIndex(cumulativeBuckets,
//                                       bucketIndex);
//                const float cumulativeValue = cumulativeBuckets[bucketIndex];
//                percentile = (cumulativeValue / m_posCount) * 100.0;
//            }
//        }
//    }
//    
//    return percentile;
}


