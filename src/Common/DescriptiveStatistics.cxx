
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

#define __DESCRIPTIVE_STATISTICS_DECLARE__
#include "DescriptiveStatistics.h"
#undef __DESCRIPTIVE_STATISTICS_DECLARE__

#include <algorithm>
#include <cmath>

#include "CaretAssert.h"

using namespace caret;
using namespace std;

    
/**
 * \class DescriptiveStatistics 
 * \brief Contains descriptive statics for some group of data.
 *
 * Provides descriptive statistics for a group of data
 * that includes mininmum and maximum values, percentiles,
 * a histogram, and statistical measurements.
 */

/**
 * Constructor that allows setting the number of elements in the histogram.
 *
 * @param histogramNumberOfElements
 *    Number of elemnents for the histogram.  Must be positive!!
 */
DescriptiveStatistics::DescriptiveStatistics(const int64_t histogramNumberOfElements, const int64_t percentileDivisions)
: CaretObject()
{
    m_histogramNumberOfElements = histogramNumberOfElements;
    m_percentileDivisions = percentileDivisions;
    m_validCount = 0;
    m_infCount = 0;
    m_negInfCount = 0;
    m_nanCount = 0;
    CaretAssert(m_histogramNumberOfElements > 2);
    CaretAssert(m_percentileDivisions > 2);
    m_histogram = new int64_t[m_histogramNumberOfElements];
    m_histogram96 = new int64_t[m_histogramNumberOfElements];
    m_positivePercentiles = new float[m_percentileDivisions];
    m_negativePercentiles = new float[m_percentileDivisions];
}

/**
 * Destructor.
 */
DescriptiveStatistics::~DescriptiveStatistics()
{
    if (m_histogram != NULL)
    {
        delete[] m_histogram;
    }
    if (m_histogram96 != NULL)
    {
        delete[] m_histogram96;
    }
    if (m_positivePercentiles != NULL)
    {
        delete[] m_positivePercentiles;
    }
    if (m_negativePercentiles != NULL)
    {
        delete[] m_negativePercentiles;
    }
}

/**
 * Update the statistics with the given data.
 * @param values
 *    Values for which statistics are calculated.
 * @param numberOfValues
 *    Number of elements in values array.
 */
void 
DescriptiveStatistics::update(const float* values,
                              const int64_t numberOfValues)
{    
    m_containsNegativeValues = false;
    m_containsPositiveValues = false;
    
    m_mean = 0.0;
    m_median = 0.0;
    m_standardDeviationPopulation = 0.0;
    m_standardDeviationSample = 0.0;
    
    m_mean96 = 0.0;
    m_median96 = 0.0;
    m_standardDeviationPopulation96 = 0.0;
    m_standardDeviationSample96 = 0.0;
    
    std::fill(m_histogram,
              m_histogram + m_histogramNumberOfElements,
              0.0);
    std::fill(m_histogram96,
              m_histogram96 + m_histogramNumberOfElements,
              0.0);
    std::fill(m_positivePercentiles,
              m_positivePercentiles + m_percentileDivisions,
              0.0);
    std::fill(m_negativePercentiles,
              m_negativePercentiles + m_percentileDivisions,
              0.0);
    
    if (numberOfValues <= 0) {
        return;
    }
    
    if (numberOfValues == 1) {
        const float v = values[0];
        m_mean = v;
        m_median = v;
        m_histogram[m_histogramNumberOfElements / 2] = v;
        m_histogram96[m_histogramNumberOfElements / 2] = v;
        fill(m_positivePercentiles,
                  m_positivePercentiles + m_percentileDivisions,
                  v);
        fill(m_negativePercentiles,
                  m_negativePercentiles + m_percentileDivisions,
                  v);
        return;
    }
    

    /*
     * Copy and sort the input data.
     */
    float* sortedValues = new float[numberOfValues];
    m_validCount = 0;
    m_infCount = 0;
    m_negInfCount = 0;
    m_nanCount = 0;
    for (int64_t i = 0; i < numberOfValues; ++i)
    {//remove and count non-numerical values
        if (values[i] != values[i])
        {
            ++m_nanCount;
            continue;
        }
        if (values[i] < -1.0f && (values[i] * 2.0f == values[i]))
        {
            ++m_negInfCount;
            continue;
        }
        if (values[i] > 1.0f && (values[i] * 2.0f == values[i]))
        {
            ++m_infCount;
            continue;
        }
        sortedValues[m_validCount] = values[i];
        ++m_validCount;
    }
    sort(sortedValues, sortedValues + m_validCount);
    
    /*
     * Find most/least negative/positive indices in sorted data.
     */
    int64_t mostNegativeIndex  = -1;
    int64_t leastNegativeIndex = -1;
    int64_t leastPositiveIndex = -1;
    int64_t mostPositiveIndex  = -1;
    if (sortedValues[0] < 0.0f)
    {
        mostNegativeIndex = 0;
    }
    if (sortedValues[0] > 0.0f)
    {
        leastPositiveIndex = 0;
    }
    if (sortedValues[m_validCount - 1] > 0.0f)
    {
        mostPositiveIndex = m_validCount - 1;
    }
    if (sortedValues[m_validCount - 1] < 0.0f)
    {
        leastNegativeIndex = m_validCount - 1;
    }
    if (leastNegativeIndex == -1 && leastPositiveIndex == -1)
    {//need to find where the zeros start and end
        int64_t start = -1, end = m_validCount, guess, nextEnd = m_validCount;
        while (end - start > 1)
        {//bisection search for last negative
            guess = (start + end) / 2;
            CaretAssertArrayIndex(sortedValues, m_validCount, guess);
            if (sortedValues[guess] < 0.0f)
            {
                start = guess;
            } else {
                end = guess;
                if (sortedValues[guess] > 0.0f)
                {
                    nextEnd = guess;//save some time on the next search
                }
            }
        }
        leastNegativeIndex = start;
        end = nextEnd;//don't reinitialize start, it is just before the first nonnegative already
        while (end - start > 1)
        {//bisection search for first positive
            guess = (start + end) / 2;
            CaretAssertArrayIndex(sortedValues, m_validCount, guess);
            if (sortedValues[guess] > 0.0f)
            {
                end = guess;
            } else {
                start = guess;
            }
        }
        leastPositiveIndex = end;
    }
    
    /*
     * Determine negative percentiles
     * Note: that index 0 is least negative, last index is most negative
     */
    const int64_t numNegativeValues = leastNegativeIndex - mostNegativeIndex + 1;
    if (numNegativeValues > 0) {
        m_containsNegativeValues = true;
        
        m_negativePercentiles[0] = sortedValues[leastNegativeIndex];
        for (int64_t i = 1; i < m_percentileDivisions - 1; i++)
        {
            int64_t indx = leastNegativeIndex - (int64_t)(((double)i * numNegativeValues) / m_percentileDivisions + 0.5);
            if (indx < 0) indx = 0;
            if (indx >= m_validCount) indx = m_validCount - 1;
            CaretAssertArrayIndex(sortedValues, m_validCount, indx);
            m_negativePercentiles[i] = sortedValues[indx];
        }
        m_negativePercentiles[m_percentileDivisions - 1] = sortedValues[mostNegativeIndex];
    }
    
    /*
     * Determine positive percentiles
     */
    const int64_t numPositiveValues = mostPositiveIndex - leastPositiveIndex + 1;
    if (numPositiveValues > 0) {
        this->m_containsPositiveValues = true;
        
        m_positivePercentiles[0] = sortedValues[leastPositiveIndex];
        for (int64_t i = 1; i < m_percentileDivisions - 1; i++) {
            int64_t indx = (int64_t)(((double)i * numPositiveValues) / m_percentileDivisions + 0.5) + leastPositiveIndex;
            if (indx < 0) indx = 0;
            if (indx >= m_validCount) indx = m_validCount - 1;
            CaretAssertArrayIndex(sortedValues, m_validCount, indx);
            m_positivePercentiles[i] = sortedValues[indx];
        }
        m_positivePercentiles[m_percentileDivisions - 1] = sortedValues[mostPositiveIndex];
    }
    
    /*
     * Indices at 2% and 98%
     */
    const int64_t twoPercentIndex = (int64_t)(m_validCount * 0.02 + 0.5);
    const int64_t ninetyEightPercentIndex = (int64_t)(m_validCount * 0.98 + 0.5);
    
    /*
     * Prepare for histogram of all data
     */
    const float minValue = sortedValues[0];
    const float maxValue = sortedValues[m_validCount - 1];
    const float bucketSize = (maxValue - minValue) / m_histogramNumberOfElements;

    /*
     * Prepare for histogram of middle 96%
     */
    CaretAssertArrayIndex(sortedValues, m_validCount, twoPercentIndex);
    const float minValue96 = sortedValues[twoPercentIndex];
    CaretAssertArrayIndex(sortedValues, m_validCount, ninetyEightPercentIndex);
    const float maxValue96 = sortedValues[ninetyEightPercentIndex];
    const float bucketSize96 = (maxValue96 - minValue96) / m_histogramNumberOfElements;
    
    /*
     * Prepare for statistics
     */
    double sum = 0.0;
    double sumSQ = 0.0;
    double sum96 = 0.0;
    double sumSQ96 = 0.0;
    
    /*
     * Create histogram and statistics.
     */
    for (int64_t i = 0; i < m_validCount; i++) {
        const float v = sortedValues[i];
        int64_t indx = (v - minValue) / bucketSize;
        if (indx >= m_histogramNumberOfElements) indx = m_histogramNumberOfElements - 1;//NEVER trust floats to not have rounding errors when nonzero
        if (indx < 0) indx = 0;//probably not needed, involves subtracting equals
        CaretAssertArrayIndex(m_histogram, m_histogramNumberOfElements, indx);
        m_histogram[indx]++;
        
        sum += v;
        const float v2 = v * v;
        sumSQ += v2;
        
        if ((i >= twoPercentIndex) && (i <= ninetyEightPercentIndex)) {
            int64_t indx96 = (v - minValue96) / bucketSize96;
            if (indx96 >= m_histogramNumberOfElements) indx96 = m_histogramNumberOfElements - 1;
            if (indx96 < 0) indx96 = 0;
            CaretAssertArrayIndex(m_histogram, m_histogramNumberOfElements, indx96);
            m_histogram96[indx96]++;
            
            sum96 += v;
            sumSQ96 += v2;
        }
    }    

    /*
     * Compute statistics of all.
     * Pop Variance = (sum(x^2) - [(sum(x))^2] / N) / N
     */
    m_mean = sum / m_validCount;
    m_median = sortedValues[m_validCount / 2];
    const double numerator = (sumSQ - ((sum*sum) / m_validCount));
    m_standardDeviationPopulation = -1.0;
    m_standardDeviationSample = -1.0;
    if (m_validCount > 0)
    {
        m_standardDeviationPopulation = sqrt(numerator / m_validCount);
        if (m_validCount > 1)
        {
            m_standardDeviationSample = sqrt(numerator / (m_validCount - 1));
        }
    }
    
    /*
     * Compute statistics of middle 96%
     */
    const int64_t numberOfValues96 = ninetyEightPercentIndex - twoPercentIndex;
    m_mean96 = sum96 / numberOfValues96;
    m_median96 = sortedValues[(numberOfValues96 / 2) + twoPercentIndex];
    const double numerator96 = (sumSQ96 - ((sum96*sum96) / numberOfValues96));
    m_standardDeviationPopulation96 = -1.0;
    m_standardDeviationSample96 = -1.0;
    if (numberOfValues96 > 0)
    {
        m_standardDeviationPopulation = sqrt(numerator96 / numberOfValues96);
        if (numberOfValues96 > 1) {
            m_standardDeviationSample = sqrt(numerator96 / (numberOfValues96 - 1));
        }
    }
}

/**
 * Update the statistics with the given data.
 * @param values
 *    Vector of values for which statistics are calculated.
 */
void 
DescriptiveStatistics::update(const std::vector<float>& values)
{
    this->update(&values[0], values.size());
}

/**
 * Get the value that is greater than 'percent' of positive values.
 *
 * @param percent
 *    The percent which ranges inclusively from 0 to 100.
 * @return
 *    Value that is greater than 'percent' of the positive values.
 */
float 
DescriptiveStatistics::getPositivePercentile(const float percent) const
{
    const float myIndex = (percent / 100 * (m_percentileDivisions - 1));//noninteger index to interpolate at
    int64_t lowIndex = (int64_t)floor(myIndex);
    int64_t highIndex = (int64_t)ceil(myIndex);
    if (highIndex <= 0) return m_positivePercentiles[0];
    if (lowIndex >= m_percentileDivisions - 1) return m_positivePercentiles[m_percentileDivisions - 1];
    if (lowIndex == highIndex) return m_positivePercentiles[lowIndex];
    float lowWeight = highIndex - myIndex;
    float highWeight = myIndex - lowIndex;
    return (lowWeight * m_positivePercentiles[lowIndex] + highWeight * m_positivePercentiles[highIndex]) / (lowWeight + highWeight);
}

/**
 * Get the value that is more negative than 'percent' of negative values.
 *
 * @param percent
 *    The percent which ranges inclusively from 0 to 100.
 * @return
 *    Value that is more negative than 'percent' of the negative values.
 */
float 
DescriptiveStatistics::getNegativePercentile(const float percent) const
{
    const float myIndex = (percent / 100 * (m_percentileDivisions - 1));//noninteger index to interpolate at
    int64_t lowIndex = (int64_t)floor(myIndex);
    int64_t highIndex = (int64_t)ceil(myIndex);
    if (highIndex <= 0) return m_negativePercentiles[0];
    if (lowIndex >= m_percentileDivisions - 1) return m_negativePercentiles[m_percentileDivisions - 1];
    if (lowIndex == highIndex) return m_negativePercentiles[lowIndex];
    float lowWeight = highIndex - myIndex;
    float highWeight = myIndex - lowIndex;
    return (lowWeight * m_negativePercentiles[lowIndex] + highWeight * m_negativePercentiles[highIndex]) / (lowWeight + highWeight);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
DescriptiveStatistics::toString() const
{
    return "DescriptiveStatistics";
}
