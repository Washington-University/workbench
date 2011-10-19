
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

#include "CaretAssert.h"

using namespace caret;

    
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
DescriptiveStatistics::DescriptiveStatistics(const int64_t histogramNumberOfElements)
: CaretObject()
{
    this->histogramNumberOfElements = histogramNumberOfElements;
    CaretAssert(this->histogramNumberOfElements > 0);
}

/**
 * Constructor.
 * 
 * The histogram will contain 100 elements.
 *
 */
DescriptiveStatistics::DescriptiveStatistics()
: CaretObject()
{
    this->histogramNumberOfElements = 100;
}

/**
 * Destructor.
 */
DescriptiveStatistics::~DescriptiveStatistics()
{
    if (this->histogram != NULL) {
        delete this->histogram;
    }
    if (this->histogram96 != NULL) {
        delete this->histogram96;
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
    this->histogram = new int64_t[this->histogramNumberOfElements];
    this->histogram96 = new int64_t[this->histogramNumberOfElements];

    this->containsNegativeValues = false;
    this->containsPositiveValues = false;
    
    this->mean = 0.0;
    this->median = 0.0;
    this->standardDeviationPopulation = 0.0;
    this->standardDeviationSample = 0.0;
    
    this->mean96 = 0.0;
    this->median96 = 0.0;
    this->standardDeviationPopulation96 = 0.0;
    this->standardDeviationSample96 = 0.0;
    
    std::fill(this->histogram,
              this->histogram + this->histogramNumberOfElements,
              0.0);
    std::fill(this->histogram96,
              this->histogram96 + this->histogramNumberOfElements,
              0.0);
    std::fill(this->positivePercentiles,
              this->positivePercentiles + DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS,
              0.0);
    std::fill(this->negativePercentiles,
              this->negativePercentiles + DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS,
              0.0);
    
    if (numberOfValues <= 0) {
        return;
    }
    
    if (numberOfValues == 1) {
        const float v = values[0];
        this->mean = v;
        this->median = v;
        this->histogram[this->histogramNumberOfElements / 2] = v;
        this->histogram96[this->histogramNumberOfElements / 2] = v;
        std::fill(this->positivePercentiles,
                  this->positivePercentiles + DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS,
                  v);
        std::fill(this->negativePercentiles,
                  this->negativePercentiles + DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS,
                  v);
        return;
    }
    

    /*
     * Copy and sort the input data.
     */
    float* sortedValues = new float[numberOfValues];
    std::copy(values, 
              values + numberOfValues, 
              sortedValues);
    std::sort(sortedValues, sortedValues + numberOfValues);
    
    /*
     * Find most/least negative/positive indices in sorted data.
     */
    int64_t mostNegativeIndex  = -1;
    int64_t leastNegativeIndex = -1;
    int64_t leastPositiveIndex = -1;
    int64_t mostPositiveIndex  = -1;
    for (int64_t i = 0; i < numberOfValues; i++) {
        const float v = sortedValues[i];
        if (v < 0.0) {
            if (mostNegativeIndex < 0) {
                mostNegativeIndex = i;
            }
            leastNegativeIndex = i;
        }
        else if (v > 0.0) {
            if (leastPositiveIndex < 0) {
                leastPositiveIndex = i;
            }
            mostPositiveIndex = i;
        }
    }
    
    /*
     * Determine negative percentiles
     * Note: that index 0 is least negative, last index is most negative
     */
    const int64_t numNegativeValues = leastNegativeIndex - mostNegativeIndex + 1;
    if (numNegativeValues > 0) {
        this->containsNegativeValues = true;
        
        const float step = (sortedValues[mostNegativeIndex] - sortedValues[leastNegativeIndex])
        / DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS;
        for (int64_t i = 0; i < DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS; i++) {
            const int64_t indx = static_cast<int64_t>(i * step) + leastNegativeIndex;
            this->negativePercentiles[i] = sortedValues[indx];
        }
        this->negativePercentiles[DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS - 1] =
        sortedValues[mostNegativeIndex];
    }
    
    /*
     * Determine positive percentiles
     */
    const int64_t numPositiveValues = mostPositiveIndex - leastPositiveIndex + 1;
    if (numPositiveValues > 0) {
        this->containsPositiveValues = true;
        
        const float step = (sortedValues[mostPositiveIndex] - sortedValues[leastPositiveIndex]) 
        / DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS;
        for (int64_t i = 0; i < DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS; i++) {
            const int64_t indx = static_cast<int64_t>(i * step) + leastPositiveIndex;
            this->positivePercentiles[i] = sortedValues[indx];
        }
        this->positivePercentiles[DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS - 1] =
        sortedValues[mostPositiveIndex];
    }
    
    /*
     * Indices at 2% and 98%
     */
    const int64_t twoPercentIndex = 
    static_cast<int64_t>(DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS * 0.02);
    const int64_t ninetyEightPercentIndex = 
    static_cast<int64_t>(DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS * 0.98);
    
    /*
     * Prepare for histogram of all data
     */
    const float minValue = sortedValues[0];
    const float maxValue = sortedValues[DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS - 1];
    const float bucketSize = (maxValue - minValue)
        / this->histogramNumberOfElements;

    /*
     * Prepare for histogram of middle 96%
     */
    const float minValue96 = sortedValues[twoPercentIndex];
    const float maxValue96 = sortedValues[ninetyEightPercentIndex];
    const float bucketSize96 = (maxValue96 - minValue96)
        / this->histogramNumberOfElements;
    
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
    for (int64_t i = 0; i < numberOfValues; i++) {
        const float v = sortedValues[i];
        const int64_t indx = (v - minValue) / bucketSize;
        this->histogram[indx]++;
        
        sum += v;
        const float v2 = v * v;
        sumSQ += v2;
        
        if ((i >= twoPercentIndex) && (i <= ninetyEightPercentIndex)) {
            const int64_t indx96 = (v - minValue96) / bucketSize96;
            this->histogram96[indx96]++;
            
            sum96 += v;
            sumSQ96 += v2;
        }
    }    

    /*
     * Compute statistics of all.
     * Pop Variance = (sum(x^2) - [(sum(x))^2] / N) / N
     */
    this->mean = sum / numberOfValues;
    this->median = sortedValues[numberOfValues / 2];
    const double numerator = (sumSQ - ((sum*sum) / numberOfValues));
    this->standardDeviationPopulation = numerator / numberOfValues;
    this->standardDeviationSample = numerator / (numberOfValues - 1);
    
    /*
     * Compute statistics of middle 96%
     */
    const int64_t numberOfValues96 = ninetyEightPercentIndex - twoPercentIndex;
    this->mean96 = sum96 / numberOfValues96;
    this->median96 = sortedValues[(numberOfValues96 / 2) + twoPercentIndex];
    const double numerator96 = (sumSQ96 - ((sum96*sum96) / numberOfValues96));
    if (numberOfValues96 > 0) {
        this->standardDeviationPopulation = numerator96 / numberOfValues96;
        if (numberOfValues96 > 1) {
            this->standardDeviationSample = numerator96 / (numberOfValues96 - 1);
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
    CaretAssert((percent >= 0.0) && (percent <= 100.0));
    const int32_t percentIndex = percent * 10.0;
    CaretAssertArrayIndex(this->positivePercentiles, 
                          DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS, 
                          percentIndex);
    return this->positivePercentiles[percentIndex];
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
    CaretAssert((percent >= 0.0) && (percent <= 100.0));
    const int32_t percentIndex = percent * 10.0;
    CaretAssertArrayIndex(this->negativePercentiles, 
                          DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS, 
                          percentIndex);
    return this->negativePercentiles[percentIndex];
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
