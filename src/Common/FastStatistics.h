#ifndef __FAST_STATISTICS_H__
#define __FAST_STATISTICS_H__

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

#include "Histogram.h"

namespace caret
{
    
    ///this class does statistics that are linear in complexity only, NO SORTING, this means its percentiles are approximate, using interpolation from a histogram
    class FastStatistics
    {
        Histogram m_posPercentHist, m_negPercentHist;
        float m_min, m_max, m_mean, m_stdDevPop, m_stdDevSample;
        float m_mostPos, m_leastPos, m_leastNeg, m_mostNeg;
        ///counts of each class of number
        int64_t m_posCount, m_zeroCount, m_negCount, m_infCount, m_negInfCount, m_nanCount;
        
        void reset();
        
    public:
        FastStatistics();
        
        FastStatistics(const float* data, const int64_t& dataCount);
        
        void update(const float* data, const int64_t& dataCount);
        
        ///statistics and display are really not that related, so for now, only include a continuous clipping range, excluding the middle from data will do weird things to standard deviation
        void update(const float* data, const int64_t& dataCount, const float& minThreshInclusive, const float& maxThreshInclusive);
        
        float getApproxPositivePercentile(const float& percent) const;
        
        float getApproxNegativePercentile(const float& percent) const;
        
        void getCounts(int64_t& posCount, int64_t& zeroCount, int64_t& negCount, int64_t& infCount, int64_t& negInfCount, int64_t& nanCount) const
        {
            posCount = m_posCount;
            zeroCount = m_zeroCount;
            negCount = m_negCount;
            infCount = m_infCount;
            negInfCount = m_negInfCount;
            nanCount = m_nanCount;
        }
        
        void getNonzeroRanges(float& mostNegative, float& leastNegative, float& leastPositive, float& mostPositive) const
        {
            mostNegative = m_mostNeg;
            leastNegative = m_leastNeg;
            leastPositive = m_leastPos;
            mostPositive = m_mostPos;
        }
        
        float getMin() const { return m_min; }
        
        float getMax() const { return m_max; }
        
        float getMean() const { return m_mean; }
        
        float getApproximateMedian() const;
        
        float getSampleStdDev() const { return m_stdDevSample; }
        
        float getPopulationStdDev() const { return m_stdDevPop; }
        
    };
    
}

#endif //__FAST_STATISTICS_H__
