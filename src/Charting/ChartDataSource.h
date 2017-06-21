#ifndef __CHART_DATA_SOURCE_H__
#define __CHART_DATA_SOURCE_H__

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
#include "ChartDataSourceModeEnum.h"
#include "SceneableInterface.h"

namespace caret {

    class SceneClassAssistant;
    
    class ChartDataSource : public CaretObject, public SceneableInterface {
        
    public:
        ChartDataSource();
        
        virtual ~ChartDataSource();
        
        ChartDataSource(const ChartDataSource&);
        
        ChartDataSource& operator=(const ChartDataSource&);
        
        bool operator==(const ChartDataSource&) const;
        
        bool operator<(const ChartDataSource&) const;
        
        void copy(const ChartDataSource* copyFrom);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        ChartDataSourceModeEnum::Enum getDataSourceMode() const;
        
        AString getChartableFileName() const;
        
        void getSurfaceNode(AString& surfaceStructureName,
                            int32_t& surfaceNumberOfNodes,
                            int32_t& nodeIndex) const;
        
        void setSurfaceNode(const AString& chartableFileName,
                            const AString& surfaceStructureName,
                            const int32_t surfaceNumberOfNodes,
                            const int32_t nodeIndex);
        
        bool isSurfaceNodeSourceOfData(const AString& surfaceStructureName,
                                       const int32_t nodeIndex) const;
        
        void getSurfaceNodeAverage(AString& surfaceStructureName,
                                   int32_t& surfaceNumberOfNodes,
                                   std::vector<int32_t>& nodeIndices) const;
        
        void setSurfaceNodeAverage(const AString& chartableFileName,
                                   const AString& surfaceStructureName,
                                   const int32_t surfaceNumberOfNodes,
                                   const std::vector<int32_t>& nodeIndices);
        
        void getVolumeVoxel(float xyz[3]) const;
        
        void setVolumeVoxel(const AString& chartableFileName,
                            const float xyz[3]);
        
        void getFileRow(AString& chartableFileName,
                        int32_t& fileRowIndex) const;
        
        void setFileRow(const AString& chartableFileName,
                        const int32_t fileRowIndex);
        
        AString getDescription() const;
        
    private:
        void copyHelperChartDataSource(const ChartDataSource& obj);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void initializeMembersChartDataSource();
        
        SceneClassAssistant* m_sceneAssistant;
        
        ChartDataSourceModeEnum::Enum m_dataSourceMode;
        
        AString m_chartableFileName;
        
        int32_t m_surfaceNumberOfNodes;
        
        AString m_surfaceStructureName;
        
        std::vector<int32_t> m_nodeIndicesAverage;
        
        int32_t m_nodeIndex;
        
        float m_voxelXYZ[3];
        
        int32_t m_fileRowIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_DATA_SOURCE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_DATA_SOURCE_DECLARE__

} // namespace
#endif  //__CHART_DATA_SOURCE_H__
