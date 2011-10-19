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
        enum { 
            /**
             * Number of values in the positive and negative percentiles.
             */
            PERCENTILE_NUMBER_OF_ELEMENTS = 1001
        };
        
        DescriptiveStatistics();
        
        DescriptiveStatistics(const int64_t histogramNumberOfElements);
        
        virtual ~DescriptiveStatistics();
        
        void update(const float* values,
                    const int64_t numberOfValues);
        
        void update(const std::vector<float>& values);
        
        float getPositivePercentile(const float percent) const;
        
        float getNegativePercentile(const float percent) const;
        
        /**
         * @return Does the data contains positive values.
         */
        bool hasPositiveValues() const { return this->containsPositiveValues; }
        
        /**
         * @return Does the data contains negative values.
         */
        bool hasNegativeValues() const { return this->containsNegativeValues; }
        
        /**
         * @return The number of elements in the histogram.
         */
        int64_t getHistogramNumberOfElements() { return this->histogramNumberOfElements; }
        
        /**
         * @return Get the histogram for all values.  The number of elements
         * is HISTOGRAM_NUMBER_OF_ELEMENTS.
         */
        const int64_t* getHistogram() const { return this->histogram; }
        
        /**
         * @return Get the histogram for the middle 96% of values.  
         * The number of elements is HISTOGRAM_NUMBER_OF_ELEMENTS.
         */
        const int64_t* getHistogram96() const { return this->histogram96; }
        
        /**
         * @return The most positive value.
         */
        float getMostPositiveValue() const { return this->positivePercentiles[PERCENTILE_NUMBER_OF_ELEMENTS - 1]; }
        
        /**
         * @return The least positive value.
         */
        float getLeastPositiveValue() const { return this->positivePercentiles[0]; }
        
        /**
         * @return The most negative value.
         */
        float getMostNegativeValue() const { return this->negativePercentiles[PERCENTILE_NUMBER_OF_ELEMENTS - 1]; }
        
        /**
         * @return The least negative value.
         */
        float getLeastNegativeValue() const { return this->negativePercentiles[0]; }
        
        /**
         * @return The mean (average) value.
         */
        float getMean() const { return this->mean; }
        
        /**
         * @return The median value.
         */
        float getMedian() const { return this->median; }
        
        /**
         * @return The population standard deviation (divide by N).
         */
        float getPopulationStandardDeviation() const { return this->standardDeviationPopulation; }
        
        /**
         * @return The sample standard deviation (divide by N - 1).
         */
        float getStandardDeviationSample() const { return this->standardDeviationSample; }
        
        /**
         * @return The mean (average) of middle 96% of elements.
         */
        float getMean96() const { return this->mean96; }
        
        /**
         * @return The median of middle 96% of elements.
         */
        float getMedian96() const { return this->median96; }
        
        /**
         * @return The population standard deviation of middle 96% of elements (divide by N).
         */
        float getPopulationStandardDeviation96() const { return this->standardDeviationPopulation96; }
        
        /**
         * @return The sample standard deviation of middle 96% of elements (divide by N - 1).
         */
        float getStandardDeviationSample96() const { return this->standardDeviationSample96; }
        
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
        int64_t* histogram;
        
        /**
         * Contains the histogram which provides the
         * distribution of the data EXCLUDING the smallest
         * two percent and the largest two percent of
         * data values.
         */
        int64_t* histogram96;
        
        /**
         * Contains the number of elements in the histograms.
         */
        int64_t histogramNumberOfElements;
        
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
        float negativePercentiles[PERCENTILE_NUMBER_OF_ELEMENTS];
        
        
        /** Indicates that negative data is present. */
        bool containsNegativeValues;
        
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
        float positivePercentiles[PERCENTILE_NUMBER_OF_ELEMENTS];
        
        /** Indicates that positive data is present. */
        bool containsPositiveValues;
        
        /** The mean (average) value. */
        float mean;
        
        /** The population standard deviation of all values (divide by N). */
        float standardDeviationPopulation;
        
        /** The sample standard deviation of all values (divide by N - 1). */
        float standardDeviationSample;
        
        /** The median (middle) value. */
        float median;
        
        /** The mean (average) value of the middle 96% of elements. */
        float mean96;
        
        /** The population standard deviation of middle 96% elements (divide by N). */
        float standardDeviationPopulation96;
        
        /** The sample standard deviation of middle 96% elements (divide by N - 1). */
        float standardDeviationSample96;
        
        /** The median (middle) value of middle 96% of elements*/
        float median96;
        
    };
    
#ifdef __DESCRIPTIVE_STATISTICS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DESCRIPTIVE_STATISTICS_DECLARE__

} // namespace
#endif  //__DESCRIPTIVE_STATISTICS__H_
