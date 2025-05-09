#ifndef __CARET_MAPPABLE_DATA_FILE_CLUSTER_FINDER_H__
#define __CARET_MAPPABLE_DATA_FILE_CLUSTER_FINDER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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


#include <cstdint>
#include <memory>
#include <vector>

#include "CaretObject.h"
#include "CaretResult.h"
#include "StructureEnum.h"
#include "Vector3D.h"
#include "VoxelIJK.h"

namespace caret {

    class CaretMappableDataFile;
    class CiftiBrainordinateLabelFile;
    class GiftiLabelTable;
    class ClusterContainer;
    class LabelFile;
    class TopologyHelper;
    class VolumeFile;
    class VolumeSpace;
    
    class CaretMappableDataFileClusterFinder : public CaretObject {
            
    public:
        /**
         * Mode for searchinhg
         */
        enum class FindMode {
            CIFTI_DENSE_LABEL,
            GIFTI_LABEL,
            VOLUME_LABEL
        };
        
        CaretMappableDataFileClusterFinder(const FindMode findMode,
                                           const CaretMappableDataFile* mapFile,
                                           const int32_t mapIndex);
        
        virtual ~CaretMappableDataFileClusterFinder();
        
        CaretMappableDataFileClusterFinder(const CaretMappableDataFileClusterFinder&) = delete;

        CaretMappableDataFileClusterFinder& operator=(const CaretMappableDataFileClusterFinder&) = delete;
        
        std::unique_ptr<CaretResult> findClusters();
        
        const ClusterContainer* getClusterContainer() const;
        
        ClusterContainer* takeClusterContainer();
        
        AString getClustersInFormattedString() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        class Brainordinate {
        public:
            Brainordinate(const VoxelIJK& voxelIJK,
                          const Vector3D& xyz)
            : m_voxelIjk(voxelIJK),
            m_nodeIndex(-1),
            m_xyz(xyz)
            { }
            
            Brainordinate(const int32_t nodeIndex,
                          const Vector3D& xyz)
            : m_voxelIjk(-1,-1,-1),
            m_nodeIndex(nodeIndex),
            m_xyz(xyz)
            { }
            
            VoxelIJK m_voxelIjk;
            int32_t m_nodeIndex;
            Vector3D m_xyz;
        };
        
        std::unique_ptr<CaretResult> findClustersInFile(const StructureEnum::Enum surfaceStructure);
        
        std::vector<int32_t> getSurfaceNeighbors(const TopologyHelper* topologyHelper,
                                                 const int32_t nodeNumber,
                                                 std::vector<char>& nodeVisited);
        
        void ijkFromIndex(const int64_t index,
                          int64_t& iOut,
                          int64_t& jOut,
                          int64_t& kOut) const;
        
        const FindMode m_findMode;
        
        const CaretMappableDataFile* m_mapFile;
        
        const int32_t m_mapIndex;
        
        const GiftiLabelTable* m_labelTable = NULL;
        
        const float* m_dataPointer = NULL;
        
        std::vector<float> m_dataStorage;
        
        int64_t m_numData = 0;
        
        int64_t m_volumeDimI = -1;
        
        int64_t m_volumeDimJ = -1;
        
        int64_t m_volumeDimK = -1;
        
        const VolumeFile* m_volumeFile = NULL;
        
        const VolumeSpace* m_volumeSpace = NULL;
        
        std::unique_ptr<ClusterContainer> m_clusterContainer;
                
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_MAPPABLE_DATA_FILE_CLUSTER_FINDER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_MAPPABLE_DATA_FILE_CLUSTER_FINDER_DECLARE__

} // namespace
#endif  //__CARET_MAPPABLE_DATA_FILE_CLUSTER_FINDER_H__
