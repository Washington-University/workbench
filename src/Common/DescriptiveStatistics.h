#ifndef __DESCRIPTIVE_STATISTICS__H_
#define __DESCRIPTIVE_STATISTICS__H_

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

#include <vector>

#include "CaretObject.h"

namespace caret {

    class DescriptiveStatistics : public CaretObject {
        
    public:
        
        DescriptiveStatistics(const int64_t histogramNumberOfElements = 100, const int64_t percentileDivisions = 1001);
        
        virtual ~DescriptiveStatistics();
        
        void update(const float* values,
                    const int64_t numberOfValues);
        
        void update(const float* values,
                    const int64_t numberOfValues,
                    const float mostPositiveValueInclusive,
                    const float leastPositiveValueInclusive,
                    const float leastNegativeValueInclusive,
                    const float mostNegativeValueInclusive,
                    const bool includeZeroValues);
        
        void update(const std::vector<float>& values);
        
        void update(const std::vector<float>& values,
                    const float mostPositiveValueInclusive,
                    const float leastPositiveValueInclusive,
                    const float leastNegativeValueInclusive,
                    const float mostNegativeValueInclusive,
                    const bool includeZeroValues);
        
        float getPositivePercentile(const float percent) const;
        
        float getNegativePercentile(const float percent) const;
        
        void invalidateData();
        
        /**
         * @return Does the data contains positive values.
         */
        bool hasPositiveValues() const { return this->m_containsPositiveValues; }
        
        /**
         * @return Does the data contains negative values.
         */
        bool hasNegativeValues() const { return this->m_containsNegativeValues; }
        
        /**
         * @return The number of elements in the histogram.
         */
        int64_t getHistogramNumberOfElements() const { return this->m_histogramNumberOfElements; }
        
        /**
         * @return Get the histogram for all values.  The number of elements
         * is HISTOGRAM_NUMBER_OF_ELEMENTS.
         */
        const int64_t* getHistogram() const { return this->m_histogram; }
        
        /**
         * @return The most positive value.
         */
        float getMostPositiveValue() const { return this->m_positivePercentiles[m_percentileDivisions - 1]; }
        
        /**
         * @return The least positive value.
         */
        float getLeastPositiveValue() const { return this->m_positivePercentiles[0]; }
        
        /**
         * @return The most negative value.
         */
        float getMostNegativeValue() const { return this->m_negativePercentiles[m_percentileDivisions - 1]; }
        
        /**
         * @return The least negative value.
         */
        float getLeastNegativeValue() const { return this->m_negativePercentiles[0]; }
        
        /**
         * @return The minimum value.
         */
        float getMinimumValue() const { return this->m_minimumValue; }
        
        /**
         * @return The maximum value.
         */
        float getMaximumValue() const { return this->m_maximumValue; }
        
        /**
         * @return The mean (average) value.
         */
        float getMean() const { return this->m_mean; }
        
        /**
         * @return The median value.
         */
        float getMedian() const { return this->m_median; }
        
        /**
         * @return The population standard deviation (divide by N).
         */
        float getPopulationStandardDeviation() const { return this->m_standardDeviationPopulation; }
        
        /**
         * @return The sample standard deviation (divide by N - 1).
         */
        float getStandardDeviationSample() const { return this->m_standardDeviationSample; }
        
    private:
        DescriptiveStatistics(const DescriptiveStatistics&);

        DescriptiveStatistics& operator=(const DescriptiveStatistics&);
        
    public:
        virtual AString toString() const;
        
    private:
        /**
         * Contains the histogram which provides the
         * distribution of the data.
         */
        int64_t* m_histogram;
        
        /**
         * Contains the number of elements in the histograms.
         */
        int64_t m_histogramNumberOfElements;
        
        ///contains number of divisions in the percentiles
        int64_t m_percentileDivisions;
        
        /**
         * Contains the negative percentiles.
         *
         * Index 0 contains the least negative value less than zero.
         *
         * Index 'X' contains the value that is less than
         * 'X' percent of values.
         *
         * The last index contains the most negative value.
         */
        float* m_negativePercentiles;
        
        
        /** Indicates that negative data is present. */
        bool m_containsNegativeValues;
        
        /**
         * Contains the positive percentiles.
         *
         * Index 0 contains the least positive value greater than zero.
         *
         * Index 'X' contains the value that is greater than
         * 'X' percent of values.
         *
         * The last index contains the greatest positive value.
         */
        float* m_positivePercentiles;
        
        /** Indicates that positive data is present. */
        bool m_containsPositiveValues;
        
        /** minimum value regardless of sign */
        float m_minimumValue;

        /** maximum value regardless of sign */
        float m_maximumValue;
        
        /** The mean (average) value. */
        float m_mean;
        
        /** The population standard deviation of all values (divide by N). */
        float m_standardDeviationPopulation;
        
        /** The sample standard deviation of all values (divide by N - 1). */
        float m_standardDeviationSample;
        
        /** The median (middle) value. */
        float m_median;
        
        ///counts of each class of number
        int64_t m_validCount, m_infCount, m_negInfCount, m_nanCount;

        /// last input value for number of (prevents unnecessary updates)
        int64_t m_lastInputNumberOfValues;
        
        /// last input value for most positive (prevents unnecessary updates)
        float m_lastInputMostPositiveValueInclusive;
        
        /// last input value for least positive (prevents unnecessary updates)
        float m_lastInputLeastPositiveValueInclusive;
        
        /// last input value for least negative (prevents unnecessary updates)
        float m_lastInputLeastNegativeValueInclusive;
        
        /// last input value for most negative (prevents unnecessary updates)
        float m_lastInputMostNegativeValueInclusive;
        
        /// last input value for include zeros (prevents unnecessary updates)
        bool m_lastInputIncludeZeroValues;

    };
    
#ifdef __DESCRIPTIVE_STATISTICS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DESCRIPTIVE_STATISTICS_DECLARE__

} // namespace
#endif  //__DESCRIPTIVE_STATISTICS__H_
