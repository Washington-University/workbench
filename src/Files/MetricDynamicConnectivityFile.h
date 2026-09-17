#ifndef __METRIC_DYNAMIC_CONNECTIVITY_FILE_H__
#define __METRIC_DYNAMIC_CONNECTIVITY_FILE_H__

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

#include "DynamicConnectivityFileInterface.h"
#include "GeneralYokingGroupEnum.h"
#include "MetricFile.h"



namespace caret {

    class ConnectivityCorrelationTwo;
    class ConnectivityCorrelationSettings;
    class ConnectivityDataLoaded;
    
    class MetricDynamicConnectivityFile : public MetricFile, public DynamicConnectivityFileInterface {
        
    public:
        MetricDynamicConnectivityFile(MetricFile* parentMetricFile);
        
        virtual ~MetricDynamicConnectivityFile();
        
        MetricDynamicConnectivityFile(const MetricDynamicConnectivityFile&) = delete;

        MetricDynamicConnectivityFile& operator=(const MetricDynamicConnectivityFile&) = delete;
        
        void initializeFile();
        
        virtual void clear() override;
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) const override;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;
        
        virtual bool supportsWriting() const override;
        
        MetricFile* getParentMetricFile();
        
        const MetricFile* getParentMetricFile() const;
        
        bool isDataValid() const;
        
        virtual bool isEnabledAsLayer() const override;
        
        virtual void setEnabledAsLayer(const bool enabled) override;
        
        virtual GeneralYokingGroupEnum::Enum getDynamicConnectivityYokingGroup() const override;
        
        virtual void setDynamicConnectivityYokingGroup(const GeneralYokingGroupEnum::Enum yokingGroup) override;
        
        virtual bool loadMapDataForVoxelAtCoordinate(const float xyz[3],
                                                     int64_t& rowIndexOut,
                                                     int64_t& columnIndexOut,
                                                     std::vector<float>& correlationDataOut);
        

        virtual bool loadMapAverageDataForVoxelIndices(const int64_t volumeDimensionIJK[3],
                                                       const std::vector<VoxelIJK>& voxelIndices,
                                                       std::vector<float>& correlationDataOut) override;
        
        virtual bool isMapDataLoadingEnabled() const override;
        
        virtual void setMapDataLoadingEnabled(const bool enabled) override;
        
        const ConnectivityDataLoaded* getConnectivityDataLoaded() const;

        virtual int64_t getNumberOfCorrelationDataPoints() const override;
        
        virtual bool loadDataForCorrelationWithDataSet(const ConnectivityCorrelationTwo::DataSet& dataSet,
                                          const AString& dataSetName) override;

        virtual bool loadMapDataForSurfaceNode(const int32_t surfaceNumberOfNodes,
                                            const StructureEnum::Enum structure,
                                            const int32_t nodeIndex,
                                            int64_t& rowIndexOut,
                                               int64_t& columnIndexOut,
                                               std::vector<float>& brainordinateRawDataSeriesOut) override;
        
        virtual bool loadMapAverageDataForSurfaceNodes(const int32_t surfaceNumberOfNodes,
                                                    const StructureEnum::Enum structure,
                                                       const std::vector<int32_t>& nodeIndices,
                                                       std::vector<float>& correlationDataOut) override;

        virtual void loadDataForColumnIndex(const int64_t columnIndex) override;
        
        virtual void loadDataForRowIndex(const int64_t rowIndex) override;
        
        MetricFile* newMetricFileFromLoadedData(const AString& directoryName,
                                                AString& errorMessageOut);
        
        virtual ConnectivityCorrelationSettings* getCorrelationSettings() override;
        
        virtual const ConnectivityCorrelationSettings* getCorrelationSettings() const override;

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass) override;
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass) override;

    private:
        void clearPrivateData();
        
        void clearVertexValues();
        
        bool getConnectivityForVertexIndex(const int32_t vertexIndex,
                                           std::vector<float>& vertexDataOut);
        
        ConnectivityCorrelationTwo* getConnectivityCorrelationTwo() const;
        
        const MetricFile* m_parentMetricFile;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        AString m_dataLoadedName;
        
        int32_t m_numberOfVertices = 0;
        
        bool m_validDataFlag = false;
        
        bool m_enabledAsLayer = true;
        
        GeneralYokingGroupEnum::Enum m_dynamicYokingGroup = GeneralYokingGroupEnum::OFF;
        
        bool m_dataLoadingEnabledFlag = true;
        
        std::unique_ptr<ConnectivityDataLoaded> m_connectivityDataLoaded;
        
        mutable bool m_connectivityCorrelationFailedFlag = false;
        
        mutable std::unique_ptr<ConnectivityCorrelationTwo> m_connectivityCorrelationTwo;
        
        mutable std::unique_ptr<ConnectivityCorrelationSettings> m_correlationSettings;
        
        mutable std::vector<float> m_metricDataCopy;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __METRIC_DYNAMIC_CONNECTIVITY_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __METRIC_DYNAMIC_CONNECTIVITY_FILE_DECLARE__

} // namespace
#endif  //__METRIC_DYNAMIC_CONNECTIVITY_FILE_H__
