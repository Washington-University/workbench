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

#include "VolumeFile.h"



namespace caret {
    class ConnectivityCorrelation;
    class ConnectivityCorrelationSettings;
    class ConnectivityDataLoaded;
    
    class VolumeDynamicConnectivityFile : public VolumeFile {
        
    public:
        VolumeDynamicConnectivityFile(const VolumeFile* parentVolumeFile);
        
        virtual ~VolumeDynamicConnectivityFile();
        
        VolumeDynamicConnectivityFile(const VolumeDynamicConnectivityFile&) = delete;

        VolumeDynamicConnectivityFile& operator=(const VolumeDynamicConnectivityFile&) = delete;
        
        void initializeFile();
        
        virtual void clear() override;
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) override;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;

        virtual bool supportsWriting() const override;
        
        VolumeFile* getParentVolumeFile();
        
        const VolumeFile* getParentVolumeFile() const;
        
        bool isDataValid() const;
        
        bool isEnabledAsLayer() const;
        
        void setEnabledAsLayer(const bool enabled);
        
        bool loadConnectivityForVoxelXYZ(const float xyz[3]);
        
        bool loadMapAverageDataForVoxelIndices(const int64_t volumeDimensionIJK[3],
                                               const std::vector<VoxelIJK>& voxelIndices);
        
        bool isDataLoadingEnabled() const;
        
        void setDataLoadingEnabled(const bool enabled);
        
        const ConnectivityDataLoaded* getConnectivityDataLoaded() const;
        
        bool matchesDimensions(const int64_t dimI,
                               const int64_t dimJ,
                               const int64_t dimK) const;
        
        VolumeFile* newVolumeFileFromLoadedData(const AString& directoryName,
                                                AString& errorMessageOut);
        
        ConnectivityCorrelationSettings* getCorrelationSettings();
        
        const ConnectivityCorrelationSettings* getCorrelationSettings() const;
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass) override;

        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass) override;

    private:
        void clearPrivateData();
        
        void clearVoxels();
        
        void getTimePointsForVoxel(const int64_t i,
                                   const int64_t j,
                                   const int64_t k,
                                   std::vector<float>& dataOut) const;

        inline int64_t getVoxelOffset(const int64_t i,
                                      const int64_t j,
                                      const int64_t k,
                                      const int64_t timePointIndex) const {
            const int64_t offset = (i
                                    + (j * m_dimI)
                                    + (k * m_sliceStride)
                                    + (timePointIndex * m_timePointIndexStride));
            return offset;
        }
        
        bool loadConnectivityForVoxelIndex(const int64_t ijk[3]);
        
        bool getConnectivityForVoxelIndex(const int64_t ijk[3],
                                          std::vector<float>& voxelsOut) ;
        
        ConnectivityCorrelation* getConnectivityCorrelation();
        

        const VolumeFile* m_parentVolumeFile;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        std::unique_ptr<ConnectivityCorrelation> m_connectivityCorrelation;

        bool m_connectivityCorrelationFailedFlag = false;
        
        float* m_voxelData = NULL;
        
        int64_t m_numberOfVoxels = 0;
        
        int64_t m_sliceStride = 0;
        
        int64_t m_timePointIndexStride = 0;
        
        int64_t m_dimI = 0;
        
        int64_t m_dimJ = 0;
        
        int64_t m_dimK = 0;
        
        int64_t m_dimTime = 0;
        
        AString m_dataLoadedName;
        
        bool m_validDataFlag = false;
        
        bool m_enabledAsLayer = true;
        
        bool m_dataLoadingEnabledFlag = true;
        
        std::unique_ptr<ConnectivityDataLoaded> m_connectivityDataLoaded;
        
        mutable std::unique_ptr<ConnectivityCorrelationSettings> m_correlationSettings;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_DYNN_CONN_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_DYNN_CONN_FILE_DECLARE__

} // namespace
#endif  //__VOLUME_DYNN_CONN_FILE_H__
