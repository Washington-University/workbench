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
        
        void loadConnectivityForVoxelIndex(const int64_t ijk[3]);
        
        void loadConnectivityForVoxelXYZ(const float xyz[3]);
        
        bool isDataLoadingEnabled() const;
        
        void setDataLoadingEnabled(const bool enabled);
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
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
        
        const VolumeFile* m_parentVolumeFile;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        std::unique_ptr<ConnectivityCorrelation> m_connectivityCorrelation;
        
        float* m_voxelData = NULL;
        
        int64_t m_numberOfVoxels = 0;
        
        int64_t m_sliceStride = 0;
        
        int64_t m_timePointIndexStride = 0;
        
        int64_t m_dimI = 0;
        
        int64_t m_dimJ = 0;
        
        int64_t m_dimK = 0;
        
        int64_t m_dimTime = 0;
        
        bool m_validDataFlag = false;
        
        bool m_enabledAsLayer = true;
        
        bool m_dataLoadingEnabledFlag = true;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_DYNN_CONN_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_DYNN_CONN_FILE_DECLARE__

} // namespace
#endif  //__VOLUME_DYNN_CONN_FILE_H__
