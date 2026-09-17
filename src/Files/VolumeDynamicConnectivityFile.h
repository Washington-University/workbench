#ifndef __VOLUME_DYNN_CONN_FILE_H__
#define __VOLUME_DYNN_CONN_FILE_H__

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

#include "GeneralYokingGroupEnum.h"
#include "DynamicConnectivityFileInterface.h"
#include "VolumeFile.h"

namespace caret {
    class ConnectivityCorrelationTwo;
    class ConnectivityCorrelationSettings;
    class ConnectivityDataLoaded;
    
    class VolumeDynamicConnectivityFile : public VolumeFile, public DynamicConnectivityFileInterface {
        
    public:
        VolumeDynamicConnectivityFile(const VolumeFile* parentVolumeFile);
        
        virtual ~VolumeDynamicConnectivityFile();
        
        VolumeDynamicConnectivityFile(const VolumeDynamicConnectivityFile&) = delete;

        VolumeDynamicConnectivityFile& operator=(const VolumeDynamicConnectivityFile&) = delete;
        
        void initializeFile();
        
        virtual void clear() override;
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) const override;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;

        virtual bool supportsWriting() const override;
        
        VolumeFile* getParentVolumeFile();
        
        const VolumeFile* getParentVolumeFile() const;
        
        bool isDataValid() const;
        
        bool isEnabledAsLayer() const;
        
        void setEnabledAsLayer(const bool enabled);
        
        GeneralYokingGroupEnum::Enum getDynamicConnectivityYokingGroup() const;
        
        void setDynamicConnectivityYokingGroup(const GeneralYokingGroupEnum::Enum yokingGroup);

        virtual bool loadMapDataForVoxelAtCoordinate(const float xyz[3],
                                                     int64_t& rowIndexOut,
                                                     int64_t& columnIndexOut,
                                                     std::vector<float>& correlationDataOut);
        
        
        virtual bool loadMapAverageDataForVoxelIndices(const int64_t volumeDimensionIJK[3],
                                                       const std::vector<VoxelIJK>& voxelIndices,
                                                       std::vector<float>& correlationDataOut) override;
        
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
                                                       std::vector<float>& correlationDataOut)override;
        
        virtual void loadDataForColumnIndex(const int64_t columnIndex) override;
        
        virtual void loadDataForRowIndex(const int64_t rowIndex) override;
        
        bool isMapDataLoadingEnabled() const;
        
        void setMapDataLoadingEnabled(const bool enabled);
        
        const ConnectivityDataLoaded* getConnectivityDataLoaded() const;
        
        bool matchesDimensions(const int64_t dimI,
                               const int64_t dimJ,
                               const int64_t dimK) const;
        
        VolumeFile* newVolumeFileFromLoadedData(const AString& directoryName,
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
        
        void clearVoxels();
        
        bool loadConnectivityForVoxelIndex(const int64_t ijk[3]);
        
        bool getConnectivityForVoxelIndex(const int64_t ijk[3],
                                          std::vector<float>& voxelsOut) ;
        
        ConnectivityCorrelationTwo* getConnectivityCorrelationTwo() const;
        
        void getParentTimepointsForIJK(const int64_t ijk[0],
                                       std::vector<float>& timepointsOut) const;
        
        const VolumeFile* m_parentVolumeFile;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        mutable std::unique_ptr<ConnectivityCorrelationTwo> m_connectivityCorrelationTwo;
        
        mutable bool m_connectivityCorrelationFailedFlag = false;
        
        float* m_voxelData = NULL;
        
        int64_t m_numberOfVoxels = 0;
        
        int64_t m_sliceStride = 0;
        
        int64_t m_timePointIndexStride = 0;
        
        int64_t m_dimI = 0;
        
        int64_t m_dimJ = 0;
        
        int64_t m_dimK = 0;
        
        int64_t m_dimTime = 0;
        
        int64_t m_parentVolumeFileNumberOfTimePoints = 0;
        
        AString m_dataLoadedName;
        
        bool m_validDataFlag = false;
        
        bool m_enabledAsLayer = true;
        
        bool m_dataLoadingEnabledFlag = true;
        
        GeneralYokingGroupEnum::Enum m_dynamicYokingGroup = GeneralYokingGroupEnum::OFF;
        
        std::unique_ptr<ConnectivityDataLoaded> m_connectivityDataLoaded;
        
        mutable std::unique_ptr<ConnectivityCorrelationSettings> m_correlationSettings;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_DYNN_CONN_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_DYNN_CONN_FILE_DECLARE__

} // namespace
#endif  //__VOLUME_DYNN_CONN_FILE_H__
