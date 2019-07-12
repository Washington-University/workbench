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
        static ConnectivityCorrelation* newInstance(const float* data,
                                                    const int64_t numberOfBrainordinates,
                                                    const int64_t nextBrainordinateStride,
                                                    const int64_t numberOfTimePoints,
                                                    const int64_t nextTimePointStride,
                                                    AString& errorMessageOut);

        static ConnectivityCorrelation* newInstanceBrainordinates(const std::vector<const float*>& brainordinateDataPointers,
                                                                  const int64_t numberOfTimePoints,
                                                                  const int64_t nextTimePointStride,
                                                                  AString& errorMessageOut);

        static ConnectivityCorrelation* newInstanceTimePoints(const std::vector<const float*>& timePointDataPointers,
                                                                  const int64_t numberOfBrainordinates,
                                                                  const int64_t nextBrainordinateStride,
                                                                  AString& errorMessageOut);
        
        /**
         * Destructor
         */
        ~ConnectivityCorrelation();
        
        ConnectivityCorrelation(const ConnectivityCorrelation&) = delete;
        
        ConnectivityCorrelation& operator=(const ConnectivityCorrelation&) = delete;
        
        void getCorrelationForBrainordinate(const int64_t brainordinateIndex,
                                            std::vector<float>& dataOut);



        // ADD_NEW_METHODS_HERE
        
    private:
        enum class DataTypeEnum {
            INVALID,
            BRAINORDINATES_NON_CONTIGUOUS_DATA,
            BRAINORDINATES_CONTIGUOUS_DATA,
            TIMEPOINTS
        };
        
        ConnectivityCorrelation(const DataTypeEnum dataType);
        
        class BrainordinateData {
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
            BrainordinateData(const float* data,
                              const int64_t dataStride)
            : m_data(data),
            m_dataStride(dataStride) { }
            
            const float* m_data;
            
            const int64_t m_dataStride;
        };
        
        class BrainordinateMeanSS {
        public:
            /*
             * Constructor
             *
             * @param mean
             *    Mean value of 'data'
             * @param sqrtSumSquared
             *    Square root of sum-squared of data
             */
            BrainordinateMeanSS(const float mean,
                                const float sqrtSumSquared)
            : m_mean(mean),
            m_sqrt_ssxx(sqrtSumSquared) { }
            
            const float m_mean;
            
            const float m_sqrt_ssxx;
        };
        
        class TimePointData {
        public:
            /**
             * Constructor.
             *
             * @param data
             *    Pointer to data
             * @param dataStride
             *    Offset between consecutive elements (1 for contiguous data)
             */
            TimePointData(const float* data,
                          const int64_t dataStride)
            : m_data(data),
            m_dataStride(dataStride)
            {
            }
            
            const float* m_data;
            
            const int64_t m_dataStride;
        };

        bool initializeWithBrainordinates(const std::vector<const float*>& brainordinateDataPointers,
                                          const int64_t numberOfTimePoints,
                                          const int64_t nextTimePointStride,
                                          AString& errorMessageOut);
        
        bool initializeWithTimePoints(const std::vector<const float*>& timePointDataPointers,
                                      const int64_t numberOfBrainordinates,
                                      const int64_t nextBrainordinateStride,
                                      AString& errorMessageOut);
        
        void computeBrainordinateMeanAndSumSquared();
        
//        void computeDataMeanAndSumSquared(const float* data,
//                                          const int64_t dataCount,
//                                          const int64_t dataStride,
//                                          float& meanOut,
//                                          float& ssxxOut) const;
//
//        void computeDataMeanAndSumSquared(const int64_t brainordinateIndex,
//                                          float& meanOut,
//                                          float& sqrtSquaredOut) const;

        float correlationBrainordinateContiguousData(const int64_t fromBrainordinateIndex,
                                                     const int64_t toBrainordinateIndex) const;
        
        float correlationBrainordinateNonContiguousData(const int64_t fromBrainordinateIndex,
                                                        const int64_t toBrainordinateIndex) const;

        float correlationTimePointData(const int64_t fromBrainordinateIndex,
                                      const int64_t toBrainordinateIndex) const;
        
        DataTypeEnum m_dataType = DataTypeEnum::INVALID;
        
        int64_t m_numberOfBrainordinates = -1;
        
        int64_t m_numberOfTimePoints = -1;
        
        std::vector<std::unique_ptr<BrainordinateData>> m_brainordinateData;
        
        std::vector<std::unique_ptr<TimePointData>> m_timePointData;
        
        std::vector<std::unique_ptr<BrainordinateMeanSS>> m_meanSSData;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CONNECTIVITY_CORRELATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CONNECTIVITY_CORRELATION_DECLARE__

} // namespace
#endif  //__CONNECTIVITY_CORRELATION_H__
