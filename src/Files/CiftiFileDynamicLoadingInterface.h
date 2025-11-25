#ifndef __CIFTI_FILE_DYNAMIC_LOADING_INTERFACE_H__
#define __CIFTI_FILE_DYNAMIC_LOADING_INTERFACE_H__

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

    class CiftiFileDynamicLoadingInterface {
        
    public:
        CiftiFileDynamicLoadingInterface() { };
        
        virtual ~CiftiFileDynamicLoadingInterface() { };
        
        CiftiFileDynamicLoadingInterface(const CiftiFileDynamicLoadingInterface&) = delete;

        CiftiFileDynamicLoadingInterface& operator=(const CiftiFileDynamicLoadingInterface&) = delete;
        
        virtual bool isMapDataLoadingEnabled(const int32_t mapIndex) const = 0;
        
        virtual void setMapDataLoadingEnabled(const int32_t mapIndex,
                                              const bool enabled) = 0;

        virtual void loadMapDataForSurfaceNode(const int32_t /*mapIndex*/,
                                               const int32_t surfaceNumberOfNodes,
                                               const StructureEnum::Enum structure,
                                               const int32_t nodeIndex,
                                               int64_t& rowIndexOut,
                                               int64_t& columnIndexOut) = 0;
        
        virtual void loadMapAverageDataForSurfaceNodes(const int32_t /*mapIndex*/,
                                                       const int32_t surfaceNumberOfNodes,
                                                       const StructureEnum::Enum structure,
                                                       const std::vector<int32_t>& nodeIndices) = 0;
        
        virtual void loadMapDataForVoxelAtCoordinate(const int32_t mapIndex,
                                                     const float xyz[3],
                                                     int64_t& rowIndexOut,
                                                     int64_t& columnIndexOut) = 0;
        
        virtual bool loadMapAverageDataForVoxelIndices(const int32_t mapIndex,
                                                       const int64_t volumeDimensionIJK[3],
                                                       const std::vector<VoxelIJK>& voxelIndices) = 0;
        
        virtual void loadDataForRowIndex(const int64_t rowIndex) = 0;
        
        virtual void loadDataForColumnIndex(const int64_t columnIndex) = 0;
        

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_FILE_DYNAMIC_LOADING_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_FILE_DYNAMIC_LOADING_INTERFACE_DECLARE__

} // namespace
#endif  //__CIFTI_FILE_DYNAMIC_LOADING_INTERFACE_H__
