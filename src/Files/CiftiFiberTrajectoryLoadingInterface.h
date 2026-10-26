#ifndef __CIFTI_FIBER_TRAJECTORY_LOADING_INTERFACE_H_
#define __CIFTI_FIBER_TRAJECTORY_LOADING_INTERFACE_H_

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#include "StructureEnum.h"
#include "VoxelIJK.h"

namespace caret {

    class CaretMappableDataFile;
    class CiftiFiberOrientationFile;
    
    class CiftiFiberTrajectoryLoadingInterface {
        
    public:
        CiftiFiberTrajectoryLoadingInterface();
        
        virtual ~CiftiFiberTrajectoryLoadingInterface();
        
        CiftiFiberTrajectoryLoadingInterface(const CiftiFiberTrajectoryLoadingInterface&) = delete;

        CiftiFiberTrajectoryLoadingInterface& operator=(const CiftiFiberTrajectoryLoadingInterface&) = delete;
        
        virtual bool isDataLoadingSupported() const = 0;
        
        virtual bool isDataLoadingEnabled() const = 0;
        
        virtual void setDataLoadingEnabled(const bool enabled) = 0;

        virtual bool isEnabledAsLayerSupported() const = 0;
        
        virtual bool isEnabledAsLayer() const = 0;
        
        virtual void setEnabledAsLayer(const bool status) = 0;
        
        virtual int64_t loadDataForSurfaceNode(const StructureEnum::Enum structure,
                                               const int32_t surfaceNumberOfNodes,
                                               const int32_t nodeIndex) = 0;
        
        virtual bool loadDataAverageForSurfaceNodes(const StructureEnum::Enum structure,
                                                    const int32_t surfaceNumberOfNodes,
                                                    const std::vector<int32_t>& nodeIndices) = 0;
        
        virtual int64_t loadDataForVoxelAtCoordinate(const float xyz[3]) = 0;
        
        virtual bool loadAverageDataForVoxelIndices(const int64_t volumeDimensionIJK[3],
                                                       const std::vector<VoxelIJK>& voxelIndices) = 0;
        
        virtual bool loadDataForRowIndex(const int64_t rowIndex) = 0;
        
        virtual bool isMatchingFiberOrientationFileSupported() const = 0;
        
        virtual CiftiFiberOrientationFile* getMatchingFiberOrientationFile() = 0;
        
        virtual const CiftiFiberOrientationFile* getMatchingFiberOrientationFile() const = 0;
        
        virtual bool isFiberOrientationFileCombatible(const CiftiFiberOrientationFile* fiberOrientationFile) const = 0;
        
        virtual void setMatchingFiberOrientationFile(CiftiFiberOrientationFile* matchingFiberOrientationFile) = 0;
        
        virtual void updateMatchingFiberOrientationFileFromList(std::vector<CiftiFiberOrientationFile*> matchingFiberOrientationFiles) = 0;
        
        AString getMapFileNameNoPath() const;
        
        AString getMapFileName();
        
        CaretMappableDataFile* getMapFile();
        
        const CaretMappableDataFile* getMapFile() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
} // namespace
#endif  //__CIFTI_FIBER_TRAJECTORY_LOADING_INTERFACE_H_
