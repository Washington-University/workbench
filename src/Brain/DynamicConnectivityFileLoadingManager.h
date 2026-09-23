#ifndef __DYNAMIC_CONNECTIVITY_FILE_LOADING_MANAGER_H__
#define __DYNAMIC_CONNECTIVITY_FILE_LOADING_MANAGER_H__

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
#include "GeneralYokingGroupEnum.h"
#include "VoxelIJK.h"

namespace caret {

    class Brain;
    class CaretMappableDataFile;
    class CiftiConnectivityMatrixParcelFile;
    class ConnectivityFileInterface;
    class DynamicConnectivityFileInterface;
    class HtmlTableBuilder;
    class SurfaceFile;
    
    class DynamicConnectivityFileLoadingManager
    : public CaretObject
    {
        
    public:
        DynamicConnectivityFileLoadingManager();
        
        virtual ~DynamicConnectivityFileLoadingManager();
        
        bool loadDataForSurfaceNode(Brain* brain,
                                    const SurfaceFile* surfaceFile,
                                    const int32_t nodeIndex,
                                    const CaretMappableDataFile* loadDataOnlyForThisFileIfNotNULL,
                                    std::vector<AString>& rowColumnInformationOut,
                                    HtmlTableBuilder& htmlTableBuilder);
        
        bool loadAverageDataForSurfaceNodes(Brain* brain,
                                            const SurfaceFile* surfaceFile,
                                            const std::vector<int32_t>& nodeIndices);
        
        bool loadDataForVoxelAtCoordinate(Brain* brain,
                                          const float xyz[3],
                                          std::vector<AString>& rowColumnInformationOut,
                                          HtmlTableBuilder& htmlTableBuilder);
        
        bool loadAverageDataForVoxelIndices(Brain* brain,
                                            const int64_t volumeDimensionIJK[3],
                                            const std::vector<VoxelIJK>& voxelIndices);

        bool loadRowOrColumnFromParcelFile(Brain* brain,
                                           CiftiConnectivityMatrixParcelFile* ciftiConnMatrixFile,
                                           const int32_t rowIndex,
                                           const int32_t columnIndex,
                                           std::vector<AString>& rowColumnInformationOut,
                                           HtmlTableBuilder& htmlTableBuilder);
        
        bool loadRowOrColumnFromConnectivityMatrixFile(ConnectivityFileInterface* parcelFile,
                                                       const int32_t rowIndex,
                                                       const int32_t columnIndex,
                                                       std::vector<AString>& rowColumnInformationOut,
                                                       HtmlTableBuilder& htmlTableBuilder);
        
        bool hasNetworkFiles(Brain* brain) const;
        
    private:
        DynamicConnectivityFileLoadingManager(const DynamicConnectivityFileLoadingManager&);

        DynamicConnectivityFileLoadingManager& operator=(const DynamicConnectivityFileLoadingManager&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        std::vector<ConnectivityFileInterface*> getConnectivityFilesWithYokingGroup(Brain* brain,
                                                                                    const GeneralYokingGroupEnum::Enum yokingGroup) const;
        
        void correlateWithOtherFiles(std::vector<ConnectivityFileInterface*> allYokedConnFiles,
                                     const CaretMappableDataFile* mapFileThatLoadedData,
                                     std::vector<float>& brainordinateSeriesData);
        void correlateWithOtherFiles(std::vector<DynamicConnectivityFileInterface*> allYokedDynFiles,
                                     const CaretMappableDataFile* mapFileThatLoadedData,
                                     std::vector<float>& brainordinateSeriesData);
        
        // ADD_NEW_MEMBERS_HERE
    };
    
#ifdef __DYNAMIC_CONNECTIVITY_FILE_LOADING_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DYNAMIC_CONNECTIVITY_FILE_LOADING_MANAGER_DECLARE__

} // namespace
#endif  //__DYNAMIC_CONNECTIVITY_FILE_LOADING_MANAGER_H__
