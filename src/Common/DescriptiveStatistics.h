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


#include "CaretObject.h"

namespace caret {

    class DescriptiveStatistics : public CaretObject {
        
    public:
        enum { 
            /**
             * Number of values in the histogram.
             */
            HISTOGRAM_NUMBER_OF_ELEMENTS = 100,
            
            /**
             * Number of values in the positive and negative percentiles.
             */
            PERCENTILE_NUMBER_OF_ELEMENTS = 101
        };
        
        DescriptiveStatistics();
        
        virtual ~DescriptiveStatistics();
        
        void update(const float* values,
                    const int64_t numberOfValues);
        
        float getPositivePercentile(const int64_t percent) const;
        
        float getNegativePercentile(const int64_t percent) const;
        
        /**
         * @return Does the data contains positive values.
         */
        bool hasPositiveValues() const { return this->containsPositiveValues; }
        
        /**
         * @return Does the data contains negative values.
         */
        bool hasNegativeValues() const { return this->containsNegativeValues; }
        
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
        int64_t histogram[HISTOGRAM_NUMBER_OF_ELEMENTS];
        
        /**
         * Contains the histogram which provides the
         * distribution of the data EXCLUDING the smallest
         * two percent and the largest two percent of
         * data values.
         */
        int64_t histogram96[HISTOGRAM_NUMBER_OF_ELEMENTS];
        
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
    };
    
#ifdef __DESCRIPTIVE_STATISTICS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DESCRIPTIVE_STATISTICS_DECLARE__

} // namespace
#endif  //__DESCRIPTIVE_STATISTICS__H_
