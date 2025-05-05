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
#include "Vector3D.h"

namespace caret {

    class CaretMappableDataFile;
    class ClusterContainer;
    class VolumeFile;
    
    class CaretMappableDataFileClusterFinder : public CaretObject {
            
    public:
        /**
         * Mode for searchinhg
         */
        enum class FindMode {
            CIFTI_DENSE_LABEL,
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
        std::unique_ptr<CaretResult> findLabelVolumeClusters(const VolumeFile* volumeFile);
        
        const FindMode m_findMode;
        
        const CaretMappableDataFile* m_mapFile;
        
        const int32_t m_mapIndex;
        
        std::unique_ptr<ClusterContainer> m_clusterContainer;
                
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_MAPPABLE_DATA_FILE_CLUSTER_FINDER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_MAPPABLE_DATA_FILE_CLUSTER_FINDER_DECLARE__

} // namespace
#endif  //__CARET_MAPPABLE_DATA_FILE_CLUSTER_FINDER_H__
