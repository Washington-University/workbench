
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

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class DescriptiveStatistics 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */
/**
 * Constructor.
 */
DescriptiveStatistics::DescriptiveStatistics()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
DescriptiveStatistics::~DescriptiveStatistics()
{
    
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
DescriptiveStatistics::getPositivePercentile(const int64_t percent) const
{
    CaretAssertArrayIndex(this->positivePercentiles, 
                          DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS, 
                          percent);
    return this->positivePercentiles[percent];
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
DescriptiveStatistics::getNegativePercentile(const int64_t percent) const
{
    CaretAssertArrayIndex(this->negativePercentiles, 
                          DescriptiveStatistics::PERCENTILE_NUMBER_OF_ELEMENTS, 
                          percent);
    return this->negativePercentiles[percent];
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
