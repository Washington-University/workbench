
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

#include "Histogram.h"
#include "CaretAssert.h"
#include <cmath>

using namespace caret;
using namespace std;

Histogram::Histogram(const int& numBuckets)
{
    resize(numBuckets);
    reset();
}

Histogram::Histogram(const float* data, const int64_t& dataCount)
{
    CaretAssert(dataCount > 0);
    const int MIN_BUCKET_COUNT = 10;
    const int MAX_BUCKET_COUNT = 10000;//hits this max at 100 million
    int numBuckets = (int)sqrt((float)dataCount);
    if (numBuckets < MIN_BUCKET_COUNT) numBuckets = MIN_BUCKET_COUNT;
    if (numBuckets > MAX_BUCKET_COUNT) numBuckets = MAX_BUCKET_COUNT;
    resize(numBuckets);
    update(data, dataCount);
}

Histogram::Histogram(const int& numBuckets, const float* data, const int64_t& dataCount)
{
    resize(numBuckets);
    update(data, dataCount);
}

void Histogram::resize(const int& buckets)
{
    CaretAssert(buckets > 0);
    m_buckets.resize(buckets);
    m_cumulative.resize(buckets);
    m_display.resize(buckets);
}

void Histogram::reset()
{
    m_posCount = 0;
    m_zeroCount = 0;
    m_negCount = 0;
    m_infCount = 0;
    m_negInfCount = 0;
    m_nanCount = 0;
    int numBuckets = (int)m_buckets.size();
    for (int i = 0; i < numBuckets; ++i)
    {
        m_buckets[i] = 0;
        m_cumulative[i] = 0;
        m_display[i] = 0.0f;
    }
    m_displayHeightMax = 0.0;
    m_bucketMin = 0.0;
    m_bucketMax = 0.0;
}

void Histogram::update(const int& numBuckets, const float* data, const int64_t& dataCount)
{
    resize(numBuckets);
    update(data, dataCount);
}

void Histogram::update(const float* data, const int64_t& dataCount)
{
    int numBuckets = (int)m_buckets.size();
    reset();
    bool first = true;
    for (int64_t i = 0; i < dataCount; ++i)
    {//count value classes
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
                    ++m_negCount;
                }
            } else {
                if (data[i] * 2.0f == data[i])
                {
                    ++m_infCount;
                    continue;//skip infs
                } else {
                    ++m_posCount;
                }
            }
        }
        if (first)
        {
            first = false;
            m_bucketMin = data[i];
            m_bucketMax = data[i];
        } else {
            if (data[i] > m_bucketMax)
            {
                m_bucketMax = data[i];
            } else if (data[i] < m_bucketMin) {//skip testing for new minimum if we found a new maximum
                m_bucketMin = data[i];
            }
        }
    }
    if (first)
    {
        m_bucketMin = m_bucketMax = 0.0f;
        return;//our arrays are already zeroed, so just return if no valid data
    }
    if (m_bucketMin == m_bucketMax)
    {
        int64_t totalValid = m_negCount + m_posCount + m_zeroCount;
        for (int i = 0; i < numBuckets - 1; ++i)
        {
            m_cumulative[i] = (i + 1) * totalValid / numBuckets;//so, its not particularly useful if our range is zero, but split them evenly among buckets just for kicks
            if (i == 0)
            {
                m_buckets[i] = m_cumulative[i];
            } else {
                m_buckets[i] = m_cumulative[i] - m_cumulative[i - 1];
            }
        }//display is already zeroed, so just return
        m_cumulative[numBuckets - 1] = totalValid;//make sure the last one has all of them
        if (numBuckets > 1)
        {
            m_buckets[numBuckets - 1] = m_cumulative[numBuckets - 1] - m_cumulative[numBuckets - 2];
        } else {
            m_buckets[numBuckets - 1] = m_cumulative[numBuckets - 1];
        }
        return;
    }
    float bucketsize = (m_bucketMax - m_bucketMin) / numBuckets;
    for (int64_t i = 0; i < dataCount; ++i)
    {//determine histogram
        if (data[i] != data[i]) continue;//exclude NaN
        if (data[i] < -1.0f && (data[i] * 2.0f == data[i])) continue;//exclude -inf
        if (data[i] > 1.0f && (data[i] * 2.0f == data[i])) continue;//exclude inf
        int bucket = (int)((data[i] - m_bucketMin) / bucketsize);//doesn't really matter whether small negative floats truncate to a 0 integer
        if (bucket < 0) bucket = 0;//because of this
        if (bucket >= numBuckets) bucket = numBuckets - 1;
        CaretAssertVectorIndex(m_buckets, bucket);
        ++m_buckets[bucket];
    }
    computeCumulative();
    m_displayHeightMax = 0.0;
    for (int i = 0; i < numBuckets; ++i)
    {//compute display values by normalizing by bucket size
        m_display[i] = m_buckets[i] / bucketsize;
        if (m_display[i] > m_displayHeightMax) {
            m_displayHeightMax = m_display[i];
        }
    }
}

void Histogram::update(const int32_t& numBuckets,
                       const float* data, const int64_t& dataCount, float mostPositiveValueInclusive,
                       float leastPositiveValueInclusive, float leastNegativeValueInclusive,
                       float mostNegativeValueInclusive, const bool& includeZeroValues)
{
    resize(numBuckets);
    update(data, dataCount, mostPositiveValueInclusive,
           leastPositiveValueInclusive, leastNegativeValueInclusive,
           mostNegativeValueInclusive, includeZeroValues);
}

void Histogram::update(const float* data, const int64_t& dataCount, float mostPositiveValueInclusive,
                       float leastPositiveValueInclusive, float leastNegativeValueInclusive,
                       float mostNegativeValueInclusive, const bool& includeZeroValues)
{
    int numBuckets = (int)m_buckets.size();
    reset();
    if (mostNegativeValueInclusive > 0.0f) mostNegativeValueInclusive = 0.0f;//sanity check the inputs without asserting
    if (mostPositiveValueInclusive < 0.0f) mostPositiveValueInclusive = 0.0f;
    if (leastNegativeValueInclusive > 0.0f) leastNegativeValueInclusive = 0.0f;
    if (leastPositiveValueInclusive < 0.0f) leastPositiveValueInclusive = 0.0f;
    if ((mostPositiveValueInclusive >= leastPositiveValueInclusive && mostPositiveValueInclusive != 0.0f) || includeZeroValues)
    {
        m_bucketMax = mostPositiveValueInclusive;
    } else {
        m_bucketMax = leastNegativeValueInclusive;
    }
    if (mostNegativeValueInclusive != 0.0f || includeZeroValues)
    {
        m_bucketMin = mostNegativeValueInclusive;
    } else {
        m_bucketMin = leastPositiveValueInclusive;
    }
    float sanity = m_bucketMax + m_bucketMin;
    if (m_bucketMax <= m_bucketMin || sanity != sanity)
    {//bad input ranges, so collect counts, make a mock histogram if equal, and return (display values will be zeros)
        int64_t equalCount = 0;
        for (int64_t i = 0; i < dataCount; ++i)
        {
            if (data[i] != data[i])
            {
                ++m_nanCount;
                continue;
            }
            if (data[i] < -1.0f && (data[i] * 2.0f == data[i]))
            {
                ++m_negInfCount;
                continue;
            }
            if (data[i] > 1.0f && (data[i] * 2.0f == data[i]))
            {
                ++m_infCount;
                continue;
            }
            if (data[i] == m_bucketMax)
            {
                ++equalCount;
            }
        }
        if (m_bucketMax == m_bucketMin)
        {
            if (m_bucketMax == 0.0f)
            {
                m_zeroCount = equalCount;
            } else {
                if (m_bucketMax < 0.0f)
                {
                    m_negCount = equalCount;
                } else {
                    m_posCount = equalCount;
                }
            }
            for (int i = 0; i < numBuckets - 1; ++i)
            {
                m_cumulative[i] = (i + 1) * equalCount / numBuckets;//so, its not particularly useful if our range is zero, but split them evenly among buckets just for kicks
                if (i == 0)
                {
                    m_buckets[i] = m_cumulative[i];
                } else {
                    m_buckets[i] = m_cumulative[i] - m_cumulative[i - 1];
                }
            }//display is already zeroed, so just return
            m_cumulative[numBuckets - 1] = equalCount;//make sure the last one has all of them
            if (numBuckets > 1)
            {
                m_buckets[numBuckets - 1] = m_cumulative[numBuckets - 1] - m_cumulative[numBuckets - 2];
            } else {
                m_buckets[numBuckets - 1] = m_cumulative[numBuckets - 1];
            }
        }
        return;
    }
    float bucketsize = (m_bucketMax - m_bucketMin) / numBuckets;
    for (int64_t i = 0; i < dataCount; ++i)//do the histogram
    {//count value classes
        if (data[i] != data[i])
        {
            ++m_nanCount;
            continue;//skip NaNs
        }
        if (data[i] == 0.0f)//test exactly zero (negative zero also tests equal), in case someone wants stats on something with miniscule values (percent of surface area per node?)
        {
            if (!includeZeroValues) continue;//don't count what is excluded
            ++m_zeroCount;
        } else {
            if (data[i] < 0.0f)
            {
                if (data[i] * 2.0f == data[i])
                {
                    ++m_negInfCount;
                    continue;//skip neg infs
                } else {
                    if (data[i] > leastNegativeValueInclusive || data[i] < mostNegativeValueInclusive) continue;//exclude negatives outside range
                    ++m_negCount;
                }
            } else {
                if (data[i] * 2.0f == data[i])
                {
                    ++m_infCount;
                    continue;//skip infs
                } else {
                    if (data[i] > mostPositiveValueInclusive || data[i] < leastPositiveValueInclusive) continue;//exclude negatives outside range
                    ++m_posCount;
                }
            }
        }
        int bucket = (int)((data[i] - m_bucketMin) / bucketsize);//doesn't really matter whether small negative floats truncate to a 0 integer
        if (bucket < 0) bucket = 0;//because of this
        if (bucket >= numBuckets) bucket = numBuckets - 1;
        CaretAssertVectorIndex(m_buckets, bucket);
        ++m_buckets[bucket];
    }
    computeCumulative();
    m_displayHeightMax = 0.0;
    for (int i = 0; i < numBuckets; ++i)
    {//compute display values by normalizing by bucket size
        m_display[i] = m_buckets[i] / bucketsize;
        if (m_display[i] > m_displayHeightMax) {
            m_displayHeightMax = m_display[i];
        }
    }
}

void Histogram::computeCumulative()
{
    int numBuckets = (int)m_buckets.size();
    int64_t accum = 0;
    for (int i = 0; i < numBuckets; ++i)
    {
        accum += m_buckets[i];
        m_cumulative[i] = accum;
    }
}

/**
 * Get the data value and height for the histogram's bucket index.
 *
 * @param bucketIndex
 *     The bucket index.
 * @param bucketDataValueOut
 *     Output with data value at the bucket (x-axis)
 * @param bucketHeightOut
 *     Output with height of bucket (y-axis)
 * @return
 *     True if output values are positive, else false.
 */
bool
Histogram::getHistogramDisplayBucketDataValueAndHeight( const int32_t bucketIndex,
                                                       float& bucketDataValueOut,
                                                       float& bucketHeightOut) const
{
    bucketDataValueOut = 0.0;
    bucketHeightOut    = 0.0;
    
    const std::vector<float>& buckets = getHistogramDisplay();
    const int32_t numberOfBuckets = static_cast<int32_t>(buckets.size());
    if ((bucketIndex >= 0)
        && (bucketIndex < numberOfBuckets)) {
        CaretAssertVectorIndex(buckets,
                               bucketIndex);
        
        float rangeMin = 0.0;
        float rangeMax = 0.0;
        getRange(rangeMin, rangeMax);
        bucketDataValueOut = rangeMin;
        if (rangeMax > rangeMin) {
            const float range = rangeMax - rangeMin;
            const float bucketWidth = range / numberOfBuckets;
            bucketDataValueOut = (rangeMin
                                  + (bucketIndex * bucketWidth));
        }
        
        bucketHeightOut = buckets[bucketIndex];
        
        return true;
    }
    
    return false;
}
