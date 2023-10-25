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

#include "CaretAssert.h"
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

        void getCorrelationForBrainordinateROI(const std::vector<int64_t>& brainordinateIndices,
                                               std::vector<float>& dataOut);

        void getCorrelationForBrainordinateData(const std::vector<float>& brainordinateData,
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
        
        /**
         * Contains timepoints for one brainordinate
         */
        class BrainordinateData {
        public:
            /**
             * Constructor.
             *
             * @param data
             *    Pointer to data containing a brainordinate's timepoints
             * @param dataStride
             *    Offset between consecutive elements (1 for contiguous data)
             */
            BrainordinateData(const float* data,
                              const int64_t dataStride)
            : m_data(data),
            m_dataStride(dataStride) { }
            
            /*
             * @return Data value for this brainordinate at the given time point index
             *
             * @param timePointIndex
             *     Index of the timepoint
             */
            inline float getTimePointValue(const int64_t timePointIndex) const {
                return m_data[timePointIndex * m_dataStride];
            }
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
        
        /**
         * Contains all brainordinate values for one timepoint
         */
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
            m_dataStride(dataStride) { }
            
            /*
             * @return Data value for this brainordinate at the given time point index
             *
             * @param timePointIndex
             *     Index of the timepoint
             */
            inline float getBrainordinateValue(const int64_t brainordinateIndex) const {
                return m_data[brainordinateIndex * m_dataStride];
            }
            
            const float* m_data;
            
            const int64_t m_dataStride;
        };

        /**
         * Get the data value for the given brainordinate and timepoint indices
         *
         * @param brainordinateIndex
         *     Index of the brainordinate
         * @param timePointIndex
         *     Index of the time point
         * @return
         *     The data value
         */
        inline float getDataValue(const int64_t brainordinateIndex,
                                  const int64_t timePointIndex) const {
            float dataValue(0.0);
            
            switch (m_dataType) {
                case DataTypeEnum::BRAINORDINATES_CONTIGUOUS_DATA:
                case DataTypeEnum::BRAINORDINATES_NON_CONTIGUOUS_DATA:
                    CaretAssertVectorIndex(m_brainordinateData, brainordinateIndex);
                    dataValue = m_brainordinateData[brainordinateIndex]->getTimePointValue(timePointIndex);
                    break;
                case DataTypeEnum::INVALID:
                    CaretAssert(0);
                    break;
                case DataTypeEnum::TIMEPOINTS:
                    CaretAssertVectorIndex(m_timePointData, timePointIndex);
                    dataValue = m_timePointData[timePointIndex]->getBrainordinateValue(brainordinateIndex);
                    break;
            }
            
            return dataValue;
        }
        
        bool initializeWithBrainordinates(const std::vector<const float*>& brainordinateDataPointers,
                                          const int64_t numberOfTimePoints,
                                          const int64_t nextTimePointStride,
                                          AString& errorMessageOut);
        
        bool initializeWithTimePoints(const std::vector<const float*>& timePointDataPointers,
                                      const int64_t numberOfBrainordinates,
                                      const int64_t nextBrainordinateStride,
                                      AString& errorMessageOut);
        
        void computeBrainordinateMeanAndSumSquared();
        
        float correlationBrainordinateContiguousData(const int64_t fromBrainordinateIndex,
                                                     const int64_t toBrainordinateIndex) const;
        
        float correlationBrainordinateContiguousDataAux(const float* fromBrainordinateData,
                                                        const float fromBrainordinateMean,
                                                        const float fromBrainordinateSSXX,
                                                        const int64_t toBrainordinateIndex) const;
        
        float correlationBrainordinateNonContiguousData(const int64_t fromBrainordinateIndex,
                                                        const int64_t toBrainordinateIndex) const;

        float correlationBrainordinateNonContiguousDataAux(const float* fromBrainordinateData,
                                                           const float fromBrainordinateMean,
                                                           const float fromBrainordinateSSXX,
                                                           const int64_t toBrainordinateIndex) const;
        
        float correlationTimePointData(const int64_t fromBrainordinateIndex,
                                      const int64_t toBrainordinateIndex) const;
        
        float correlationTimePointDataAux(const float* fromBrainordinateData,
                                          const float fromBrainordinateMean,
                                          const float fromBrainordinateSSXX,
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
