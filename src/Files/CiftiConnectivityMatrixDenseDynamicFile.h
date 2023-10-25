#ifndef __CIFTI_CONNECTIVITY_MATRIX_DENSE_DYNAMIC_FILE_H__
#define __CIFTI_CONNECTIVITY_MATRIX_DENSE_DYNAMIC_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include "CaretPointer.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"

namespace caret {
    class CiftiBrainordinateDataSeriesFile;
    class ConnectivityCorrelation;
    class ConnectivityCorrelationSettings;
    class SceneClassAssistant;
    
    class CiftiConnectivityMatrixDenseDynamicFile : public CiftiMappableConnectivityMatrixDataFile {
        
    public:
        CiftiConnectivityMatrixDenseDynamicFile(CiftiBrainordinateDataSeriesFile* parentDataSeriesFile);
        
        virtual ~CiftiConnectivityMatrixDenseDynamicFile();
        
        bool isDataValid() const;

        bool isEnabledAsLayer() const;
        
        void setEnabledAsLayer(const bool enabled);
        
        virtual bool supportsWriting() const;
        
        void updateAfterReading(const CiftiFile* ciftiFile);
        
        CiftiBrainordinateDataSeriesFile* getParentBrainordinateDataSeriesFile();
        
        const CiftiBrainordinateDataSeriesFile* getParentBrainordinateDataSeriesFile() const;
        
        ConnectivityCorrelationSettings* getCorrelationSettings();
        
        const ConnectivityCorrelationSettings* getCorrelationSettings() const;

    private:
        CiftiConnectivityMatrixDenseDynamicFile(const CiftiConnectivityMatrixDenseDynamicFile&);

        CiftiConnectivityMatrixDenseDynamicFile& operator=(const CiftiConnectivityMatrixDenseDynamicFile&);
        
    protected:
        virtual void getDataForColumn(float* dataOut, const int64_t& index) const;
        
        virtual void getDataForRow(float* dataOut, const int64_t& index) const;
                
        virtual void getProcessedDataForColumn(float* dataOut, const int64_t& index) const;
        
        virtual void getProcessedDataForRow(float* dataOut, const int64_t& index) const;
        
        virtual void processRowAverageData(std::vector<float>& rowAverageData);
        
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);
        
        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);
        
    private:
        class RowData {
        public:
            RowData() { }
            
            ~RowData() { }
            
            std::vector<float> m_data;
            float m_mean;
            float m_sqrt_ssxx;
        };
        
        float correlation(const std::vector<float>& data,
                          const float mean,
                          const float sumSquared,
                          const int32_t otherRowIndex,
                          const int32_t numberOfPoints) const;
        
        void preComputeRowMeanAndSumSquared();
        
        void computeDataMeanAndSumSquared(const float* data,
                                          const int32_t dataLength,
                                          float& meanOut,
                                          float& sumSquaredOut) const;
        
        ConnectivityCorrelation* getConnectivityCorrelation() const;
        
        CiftiBrainordinateDataSeriesFile* m_parentDataSeriesFile;
        
        CiftiFile* m_parentDataSeriesCiftiFile;
        
        int32_t m_numberOfBrainordinates;
        
        int32_t m_numberOfTimePoints;
        
        std::vector<RowData> m_rowData;
        
        bool m_validDataFlag;
        
        bool m_enabledAsLayer;
        
        const bool m_cacheDataFlag;
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;
        
        mutable std::unique_ptr<ConnectivityCorrelation> m_connectivityCorrelation;
        
        mutable bool m_connectivityCorrelationFailedFlag = false;
        
        mutable std::vector<float> m_dataSeriesMatrixData;

        mutable std::unique_ptr<ConnectivityCorrelationSettings> m_correlationSettings;
        
        bool m_testConnectivityCorrelationFlag = true;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_CONNECTIVITY_MATRIX_DENSE_DYNAMIC_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_CONNECTIVITY_MATRIX_DENSE_DYNAMIC_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_CONNECTIVITY_MATRIX_DENSE_DYNAMIC_FILE_H__
