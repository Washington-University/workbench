#ifndef __CONNECTIVITY_CORRELATION_TWO_H__
#define __CONNECTIVITY_CORRELATION_TWO_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#include <vector>

#include "CaretAssert.h"
#include "CaretObject.h"
#include "ConnectivityCorrelationSettings.h"


namespace caret {

    class ConnectivityCorrelationTwo : public CaretObject {
        
    public:
        class DataSet {
        public:
            DataSet(const int64_t dataSetIndex,
                    const float* dataElements,
                    const int64_t numDataElements,
                    const int64_t dataStride,
                    const float mean,
                    const float sqrtSumSquared,
                    const bool allZerosFlag)
            : m_dataSetIndex(dataSetIndex),
            m_dataElements(dataElements),
            m_numDataElements(numDataElements),
            m_dataStride(dataStride),
            m_mean(mean),
            m_sqrtSumSquared(sqrtSumSquared),
            m_allZerosFlag(allZerosFlag)
            { }
            
            /** @return data element at given index */
            const float& get(const int64_t index) const {
                CaretAssert((index >= 0) && (index < m_numDataElements));
                return m_dataElements[index * m_dataStride];
            }
            
            const int64_t m_dataSetIndex;
            const float*  m_dataElements;
            const int64_t m_numDataElements;
            const int64_t m_dataStride;
            const float   m_mean;
            const float   m_sqrtSumSquared;
            const bool    m_allZerosFlag;
        };
        
        class DataSetGroup {
        public:
            DataSetGroup() { }
            
            void addDataSet(DataSet* dataSet) {
                if (m_dataSets.empty()) {
                    m_dataSets.emplace_back(dataSet);
                }
                else {
                    if (m_dataSets[0]->m_numDataElements == dataSet->m_numDataElements) {
                        m_dataSets.emplace_back(dataSet);
                    }
                }
            }
            
            int32_t getNumberOfDataSets() const { return m_dataSets.size(); }
            
            int64_t getNumberOfElements() const {
                if ( ! m_dataSets.empty()) {
                    return m_dataSets[0]->m_numDataElements;
                }
                return 0;
            }
            
            void setNumberOfDataSets(const int32_t numDataSets) {
                m_dataSets.resize(numDataSets);
            }
            
            void setDataSet(const int32_t index,
                            DataSet* dataSet) {
                m_dataSets[index].reset(dataSet);
            }
            
            DataSet* getDataSetPtr(const int32_t index) const { return m_dataSets[index].get(); }
            
            DataSet& getDataSetRef(const int32_t index) const { return *m_dataSets[index].get(); }
            
            std::vector<std::unique_ptr<DataSet>> m_dataSets;
        };
        
        static DataSet createDataSet(const float* dataElements,
                                     const int64_t numberOfDataElements,
                                     const int64_t dataStride,
                                     const bool correlationNoDemeanEnabled);
        
        static DataSet* createDataSetPtr(const float* dataElements,
                                         const int64_t numberOfDataElements,
                                         const int64_t dataStride,
                                         const bool correlationNoDemeanEnabled);
        
        static ConnectivityCorrelationTwo* newInstance(const AString& ownerName,
                                                       const ConnectivityCorrelationSettings& settings,
                                                       const std::vector<const float*>& dataSetPointers,
                                                       const int64_t numberOfDataElements,
                                                       const int64_t dataStride,
                                                       AString& errorMessageOut);

        virtual ~ConnectivityCorrelationTwo();
        
        ConnectivityCorrelationTwo(const ConnectivityCorrelationTwo&) = delete;

        ConnectivityCorrelationTwo& operator=(const ConnectivityCorrelationTwo&) = delete;
        
        const ConnectivityCorrelationSettings* getSettings() const;
        
        void computeAverageForDataSetIndices(const std::vector<int64_t> dataSetIndices,
                                             std::vector<float>& dataOut) const;
        
        void computeForDataSetGroup(const DataSetGroup& dataSetGroup,
                                    std::vector<float>& dataOut) const;
        
        void computeForDataSet(const DataSet& dataSet,
                               std::vector<float>& dataOut) const;
        
        void computeForDataSetIndex(const int64_t dataSetIndex,
                                    std::vector<float>& dataOut) const;
        
        int64_t getNumberOfDataElements() const;
        
        static constexpr bool s_useAppleAccelerateFlag = true;
        
        // ADD_NEW_METHODS_HERE

    private:
        
        ConnectivityCorrelationTwo(const AString& ownerName,
                                   const ConnectivityCorrelationSettings& settings,
                                   const std::vector<const float*>& dataSetPointers,
                                   const int64_t numberOfDataElements,
                                   const int64_t dataStride);
        
        float computeForDataSets(const DataSet& a,
                                 const DataSet& b) const;
        
        static void computeMeanAndSumSquared(const float* dataPtr,
                                             const int64_t numberOfDataElements,
                                             const int64_t dataStride,
                                             const bool correlationNoDemeanEnabled,
                                             float& meanOut,
                                             float& sqrtSumSquaredOut,
                                             bool& allZerosOutFlag);

        void printDebugData();
        
        const AString m_ownerName;
        
        const ConnectivityCorrelationSettings m_settings;
        
        const int64_t m_numberOfDataSets;
        
        const int64_t m_numberOfDataElements;
        
        const int64_t m_dataStride;
        
        std::vector<DataSet*> m_dataSets;
        
        bool m_debugFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CONNECTIVITY_CORRELATION_TWO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CONNECTIVITY_CORRELATION_TWO_DECLARE__

} // namespace
#endif  //__CONNECTIVITY_CORRELATION_TWO_H__
