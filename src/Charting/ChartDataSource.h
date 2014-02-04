#ifndef __CHART_DATA_SOURCE_H__
#define __CHART_DATA_SOURCE_H__

/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_DATA_SOURCE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_DATA_SOURCE_DECLARE__

} // namespace
#endif  //__CHART_DATA_SOURCE_H__
