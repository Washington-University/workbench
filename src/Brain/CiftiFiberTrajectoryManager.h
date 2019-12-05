#ifndef __CIFTI_FIBER_TRAJECTORY_MANAGER_H__
#define __CIFTI_FIBER_TRAJECTORY_MANAGER_H__

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
#include "VoxelIJK.h"

namespace caret {

    class Brain;
    class HtmlTableBuilder;
    class SurfaceFile;
    
    class CiftiFiberTrajectoryManager : public CaretObject {
        
    public:
        CiftiFiberTrajectoryManager();
        
        virtual ~CiftiFiberTrajectoryManager();
        
        bool loadDataForSurfaceNode(Brain* brain,
                                    const SurfaceFile* surfaceFile,
                                    const int32_t nodeIndex,
                                    std::vector<AString>& rowColumnInformationOut,
                                    HtmlTableBuilder& htmlTableBuilder);
        
        bool loadDataAverageForSurfaceNodes(Brain* brain,
                                            const SurfaceFile* surfaceFile,
                                            const std::vector<int32_t>& nodeIndices);
        bool loadDataForVoxelAtCoordinate(Brain* brain,
                                          const float xyz[3],
                                          std::vector<AString>& rowColumnInformationOut,
                                          HtmlTableBuilder& htmlTableBuilder);
        
        bool loadAverageDataForVoxelIndices(Brain* brain,
                                            const int64_t volumeDimensionIJK[3],
                                            const std::vector<VoxelIJK>& voxelIndices);
        
        // ADD_NEW_METHODS_HERE
        
    private:
        CiftiFiberTrajectoryManager(const CiftiFiberTrajectoryManager&);

        CiftiFiberTrajectoryManager& operator=(const CiftiFiberTrajectoryManager&);
        
        // ADD_NEW_MEMBERS_HERE
    };
    
#ifdef __CIFTI_FIBER_TRAJECTORY_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_FIBER_TRAJECTORY_MANAGER_DECLARE__

} // namespace
#endif  //__CIFTI_FIBER_TRAJECTORY_MANAGER_H__
