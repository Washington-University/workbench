#ifndef __CONNECTIVITY_DATA_LOADED_H__
#define __CONNECTIVITY_DATA_LOADED_H__

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


#include "CaretObject.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"
#include "VoxelIJK.h"

namespace caret {

    class SceneClassAssistant;
    class SurfaceFile;
    
    class ConnectivityDataLoaded : public CaretObject, public SceneableInterface {
    
    public:
        ConnectivityDataLoaded();
        
        virtual ~ConnectivityDataLoaded();
        
    private:
        ConnectivityDataLoaded(const ConnectivityDataLoaded&);

        ConnectivityDataLoaded& operator=(const ConnectivityDataLoaded&);
        
    public:
        enum Mode {
            MODE_NONE,
            MODE_ROW,
            MODE_COLUMN,
            MODE_SURFACE_NODE,
            MODE_SURFACE_NODE_AVERAGE,
            MODE_VOXEL_XYZ,
            MODE_VOXEL_IJK_AVERAGE
        };
        
        void reset();
        
        Mode getMode() const;
        
        void getRowColumnLoading(int64_t& rowIndex,
                                 int64_t& columnIndex) const;
        
        void setRowColumnLoading(const int64_t rowIndex,
                                 const int64_t columnIndex);
        
        void getSurfaceNodeLoading(StructureEnum::Enum& structure,
                                   int32_t& surfaceNumberOfNodes,
                                   int32_t& surfaceNodeIndex,
                                   int64_t& rowIndex,
                                   int64_t& columnIndex) const;
        
        void setSurfaceNodeLoading(const StructureEnum::Enum structure,
                                   const int32_t surfaceNumberOfNodes,
                                   const int32_t surfaceNodeIndex,
                                   const int64_t rowIndex,
                                   const int64_t columnIndex);
        
        void getSurfaceAverageNodeLoading(StructureEnum::Enum& structure,
                                   int32_t& surfaceNumberOfNode,
                                          std::vector<int32_t>& surfaceNodeIndices) const;
        
        void setSurfaceAverageNodeLoading(const StructureEnum::Enum structure,
                                          const int32_t surfaceNumberOfNodes,
                                          const std::vector<int32_t>& surfaceNodeIndices);
        
        void getVolumeXYZLoading(float volumeXYZ[3],
                                 int64_t& rowIndex,
                                 int64_t& columnIndex) const;
        
        void setVolumeXYZLoading(const float volumeXYZ[3],
                                 const int64_t rowIndex,
                                 const int64_t columnIndex);
        
        void getVolumeAverageVoxelLoading(int64_t volumeDimensionsIJK[3],
                                          std::vector<VoxelIJK>& voxelIndicesIJK) const;
        
        void setVolumeAverageVoxelLoading(const int64_t volumeDimensionsIJK[3],
                                          const std::vector<VoxelIJK>& voxelIndicesIJK);
                
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName);
        
    private:
        // ADD_NEW_MEMBERS_HERE

        SceneClassAssistant* m_sceneAssistant;
        
        Mode m_mode;
        
        int32_t m_rowIndex;
        
        int32_t m_columnIndex;
        
        StructureEnum::Enum m_surfaceStructure;
        
        int32_t m_surfaceNumberOfNodes;
        
        std::vector<int32_t> m_surfaceNodeIndices;
        
        std::vector<VoxelIJK> m_voxelIndices;
        
        int32_t m_volumeDimensionsIJK[3];
        
        float m_volumeXYZ[3];
    };
    
#ifdef __CONNECTIVITY_DATA_LOADED_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CONNECTIVITY_DATA_LOADED_DECLARE__

} // namespace
#endif  // __CONNECTIVITY_DATA_LOADED_H__

