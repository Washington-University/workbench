#ifndef __METRIC_DYNAMIC_CONNECTIVITY_FILE_H__
#define __METRIC_DYNAMIC_CONNECTIVITY_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include "MetricFile.h"



namespace caret {

    class ConnectivityCorrelation;
    class ConnectivityDataLoaded;
    
    class MetricDynamicConnectivityFile : public MetricFile {
        
    public:
        MetricDynamicConnectivityFile(MetricFile* parentMetricFile);
        
        virtual ~MetricDynamicConnectivityFile();
        
        MetricDynamicConnectivityFile(const MetricDynamicConnectivityFile&) = delete;

        MetricDynamicConnectivityFile& operator=(const MetricDynamicConnectivityFile&) = delete;
        
        void initializeFile();
        
        virtual void clear() override;
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) override;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;
        
        virtual bool supportsWriting() const override;
        
        MetricFile* getParentMetricFile();
        
        const MetricFile* getParentMetricFile() const;
        
        bool isDataValid() const;
        
        bool isEnabledAsLayer() const;
        
        void setEnabledAsLayer(const bool enabled);
        
        bool loadConnectivityForVoxelXYZ(const float xyz[3]);
        
        bool loadMapAverageDataForVoxelIndices(const int64_t volumeDimensionIJK[3],
                                               const std::vector<VoxelIJK>& voxelIndices);
        
        bool isDataLoadingEnabled() const;
        
        void setDataLoadingEnabled(const bool enabled);
        
        const ConnectivityDataLoaded* getConnectivityDataLoaded() const;

        bool loadDataForSurfaceNode(const int32_t surfaceNumberOfNodes,
                                    const StructureEnum::Enum structure,
                                    const int32_t nodeIndex);
        
        bool loadAverageDataForSurfaceNodes(const int32_t surfaceNumberOfNodes,
                                            const StructureEnum::Enum structure,
                                            const std::vector<int32_t>& nodeIndices);

        MetricFile* newMetricFileFromLoadedData(const AString& directoryName,
                                                AString& errorMessageOut);
        
        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass) override;
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass) override;

    private:
        void clearPrivateData();
        
        void clearVertexValues();
        
        bool getConnectivityForVertexIndex(const int32_t vertexIndex,
                                           std::vector<float>& vertexDataOut);
        
        ConnectivityCorrelation* getConnectivityCorrelation();
        
        const MetricFile* m_parentMetricFile;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        std::unique_ptr<ConnectivityCorrelation> m_connectivityCorrelation;
        
        AString m_dataLoadedName;
        
        int32_t m_numberOfVertices = 0;
        
        bool m_validDataFlag = false;
        
        bool m_enabledAsLayer = true;
        
        bool m_dataLoadingEnabledFlag = true;
        
        std::unique_ptr<ConnectivityDataLoaded> m_connectivityDataLoaded;
        
        bool m_connectivityCorrelationFailedFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __METRIC_DYNAMIC_CONNECTIVITY_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __METRIC_DYNAMIC_CONNECTIVITY_FILE_DECLARE__

} // namespace
#endif  //__METRIC_DYNAMIC_CONNECTIVITY_FILE_H__
