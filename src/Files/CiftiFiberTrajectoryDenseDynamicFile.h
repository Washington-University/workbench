#ifndef __CIFTI_BRAINORDINATE_FIBER_TRAJECTORY_DYNAMIC_FILE_H__
#define __CIFTI_BRAINORDINATE_FIBER_TRAJECTORY_DYNAMIC_FILE_H__

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

#include <memory>

#include "BrainConstants.h"
#include "ChartableLineSeriesBrainordinateInterface.h"
#include "CiftiFiberTrajectoryLoadingInterface.h"
#include "CiftiMappableDataFile.h"

namespace caret {

    class CiftiDenseSparseFile;
    class CiftiFiberTrajectoryFile;
    class CiftiMappableConnectivityMatrixDataFile;
    class SceneClassAssistant;
    
    class CiftiFiberTrajectoryDenseDynamicFile :
    public CiftiMappableDataFile,
    public ChartableLineSeriesBrainordinateInterface,
    public CiftiFiberTrajectoryLoadingInterface {
    
    public:
        
        static int32_t getFiberCountsMapIndex();
        
        static int32_t getFiberDistancesMapIndex();
        
        CiftiFiberTrajectoryDenseDynamicFile(CiftiFiberTrajectoryFile* parentFiberTrajectoryFile);
        
        virtual ~CiftiFiberTrajectoryDenseDynamicFile();
        
        bool isValidFile();
        
        virtual bool isLineSeriesChartingEnabled(const int32_t tabIndex) const;
        
        virtual void setLineSeriesChartingEnabled(const int32_t tabIndex,
                                        const bool enabled);
        
        virtual bool isLineSeriesChartingSupported() const;

        virtual ChartDataCartesian* loadLineSeriesChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                                                   const int32_t nodeIndex);
        
        virtual ChartDataCartesian* loadAverageLineSeriesChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                               const std::vector<int32_t>& nodeIndices);
        
        virtual ChartDataCartesian* loadLineSeriesChartDataForVoxelAtCoordinate(const float xyz[3]);
        

        virtual void getSupportedLineSeriesChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const;
        
        bool isDataValid() const;
        
        virtual bool isEnabledAsLayerSupported() const override;
        
        virtual bool isEnabledAsLayer() const override;
        
        virtual void setEnabledAsLayer(const bool status) override;
        
        virtual int64_t loadDataForSurfaceNode(const StructureEnum::Enum structure,
                                               const int32_t surfaceNumberOfNodes,
                                               const int32_t nodeIndex) override;
        
        virtual bool loadDataAverageForSurfaceNodes(const StructureEnum::Enum structure,
                                                    const int32_t surfaceNumberOfNodes,
                                                    const std::vector<int32_t>& nodeIndices) override;
        
        virtual int64_t loadDataForVoxelAtCoordinate(const float xyz[3]) override;
        
        virtual bool loadAverageDataForVoxelIndices(const int64_t volumeDimensionIJK[3],
                                                    const std::vector<VoxelIJK>& voxelIndices) override;
        
        virtual bool loadDataForRowIndex(const int64_t rowIndex);
        
        virtual bool isDataLoadingSupported() const override;

        virtual bool isDataLoadingEnabled() const override;
        
        virtual void setDataLoadingEnabled(const bool loadingEnabled) override;
        
        virtual bool isMatchingFiberOrientationFileSupported() const override;
        
        virtual CiftiFiberOrientationFile* getMatchingFiberOrientationFile() override;
        
        virtual const CiftiFiberOrientationFile* getMatchingFiberOrientationFile() const override;
        
        virtual bool isFiberOrientationFileCombatible(const CiftiFiberOrientationFile* fiberOrientationFile) const override;
        
        virtual void setMatchingFiberOrientationFile(CiftiFiberOrientationFile* matchingFiberOrientationFile) override;
        
        virtual void updateMatchingFiberOrientationFileFromList(std::vector<CiftiFiberOrientationFile*> matchingFiberOrientationFiles) override;

        CiftiFiberTrajectoryFile* getParentFiberTrajectoryFile();
        
        const CiftiFiberTrajectoryFile* getParentFiberTrajectoryFile() const;
        
        void setDynamicConnectivityData(const std::vector<float>& dynamicConnectivityFiberCountsData,
                                        const std::vector<float>& dynamicConnectivityFiberDistancesData,
                                        const AString& dataLoadingName);
        
        AString getDataLoadingName() const;
        
    private:
        CiftiFiberTrajectoryDenseDynamicFile(const CiftiFiberTrajectoryDenseDynamicFile&);

        CiftiFiberTrajectoryDenseDynamicFile& operator=(const CiftiFiberTrajectoryDenseDynamicFile&);
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        std::unique_ptr<SceneClassAssistant> m_sceneClassAssistant;
        
        bool m_chartingEnabledForTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        CiftiFiberTrajectoryFile* m_parentFiberTrajectoryFile = NULL;
        
        AString m_dataLoadingName;
        
        bool m_enabledAsLayerFlag = true;
        
        bool m_dataLoadingEnabled = true;
        
        bool m_validFileFlag = false;
        
        static constexpr int32_t s_fiberCountsMapIndex = 0;
        
        static constexpr int32_t s_fiberDistancesMapIndex = 1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_BRAINORDINATE_FIBER_TRAJECTORY_DYNAMIC_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_BRAINORDINATE_FIBER_TRAJECTORY_DYNAMIC_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_BRAINORDINATE_FIBER_TRAJECTORY_DYNAMIC_FILE_H__
