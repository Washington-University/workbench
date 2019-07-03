#ifndef __CONNECTIVITY_CORRELATION_H__
#define __CONNECTIVITY_CORRELATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"



namespace caret {

    class ConnectivityCorrelation : public CaretObject {
        
    public:
        ConnectivityCorrelation(const float* data,
                                const int64_t groupCount,
                                const int64_t groupStride,
                                const int64_t groupDataCount,
                                const int64_t groupDataStride);
        
        ConnectivityCorrelation(const float* dataGroups[],
                                const int64_t groupCount,
                                const int64_t groupDataCount,
                                const int64_t groupDataStrides[]);
        
        /**
         * Destructor
         */
        ~ConnectivityCorrelation();
        
        ConnectivityCorrelation(const ConnectivityCorrelation&) = delete;
        
        ConnectivityCorrelation& operator=(const ConnectivityCorrelation&) = delete;
        
        void getCorrelationForGroup(const int64_t groupIndex,
                                    std::vector<float>& dataOut);
        


        // ADD_NEW_METHODS_HERE
        
    private:
        class GroupData {
        public:
            /**
             * Constructor.
             *
             * @param data
             *    Pointer to data
             * @param dataStride
             *    Offset between consecutive elements (1 for contiguous data)
             * @param mean
             *    Mean value of 'data'
             * @param sqrtSumSquared
             *    Square root of sum-squared of data
             */
            GroupData(const float* data,
                      const int64_t dataStride,
                      const float mean,
                      const float sqrtSumSquared)
            : m_data(data),
            m_dataStride(dataStride),
            m_mean(mean),
            m_sqrt_ssxx(sqrtSumSquared) { }
            
            const float* m_data;
            
            const int64_t m_dataStride;
            
            const float m_mean;
            
            const float m_sqrt_ssxx;
        };
        
        void addDataGroup(const float* data,
                          const int64_t dataCount,
                          const int64_t dataStride);
        
        void computeDataMeanAndSumSquared(const float* data,
                                          const int64_t dataCount,
                                          const int64_t dataStride,
                                          float& meanOut,
                                          float& sumSquaredOut) const;
        
        float correlationContiguousData(const int64_t fromGroupIndex,
                                        const int64_t toGroupIndex) const;
        
        float correlationNonContiguousData(const int64_t fromGroupIndex,
                                           const int64_t toGroupIndex) const;
        
        const float m_dataCount;
        
        std::vector<std::unique_ptr<GroupData>> m_groups;
        
        bool m_contiguousDataFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CONNECTIVITY_CORRELATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CONNECTIVITY_CORRELATION_DECLARE__

} // namespace
#endif  //__CONNECTIVITY_CORRELATION_H__
