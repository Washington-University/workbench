#ifndef __CHARTABLE_TWO_FILE_LINE_LAYER_CHART_H__
#define __CHARTABLE_TWO_FILE_LINE_LAYER_CHART_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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
#include "ChartableTwoFileBaseChart.h"
#include "ChartTwoLineLayerContentTypeEnum.h"


namespace caret {
    class BoundingBox;
    class ChartTwoDataCartesian;
    class CiftiBrainModelsMap;
    class MapFileDataSelector;
    class SceneClassAssistant;

    class ChartableTwoFileLineLayerChart : public ChartableTwoFileBaseChart {
        
    public:
        ChartableTwoFileLineLayerChart(const ChartTwoLineLayerContentTypeEnum::Enum lineLayerContentType,
                                                CaretMappableDataFile* parentCaretMappableDataFile);
        
        ChartableTwoFileLineLayerChart(const ChartableTwoFileLineLayerChart&);
        
        ChartableTwoFileLineLayerChart& operator=(const ChartableTwoFileLineLayerChart&);
        
        virtual ~ChartableTwoFileLineLayerChart();
        
        ChartTwoLineLayerContentTypeEnum::Enum getLineLayerContentType() const;
        
        virtual bool isValid() const override;
        
        virtual bool isEmpty() const override;
        
        virtual void receiveEvent(Event* event) override;
        
        int32_t getNumberOfChartMaps() const;
        
        void getChartMapNames(std::vector<AString>& mapNamesOut);
        
        void clearChartLines();
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected:
        /* Since line may be normalized, must access throught ChartTwoOverlay */
        ChartTwoDataCartesian* getChartMapLineForChartTwoOverlay(const int32_t chartMapIndex);
        
        /* Since line may be normalized, must access throught ChartTwoOverlay */
        bool getBoundsForChartTwoOverlay(const int32_t mapIndex,
                                         BoundingBox& boundingBoxOut) const;
        
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass) override;

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass) override;

    private:
        enum class VolumeOpMode {
            MAP_NAMES,
            VOXEL_XYZ
        };
        
        ChartTwoDataCartesian* loadChartForMapFileSelector(const MapFileDataSelector& mapFileSelector);
        
        ChartTwoDataCartesian* createChartData() const;
        
        void getMapNamesFromCiftiBrainMap(const CiftiBrainModelsMap& brainModelsMap,
                                          std::vector<AString>& mapNames);

        void setVolumeMapNamesAndVoxelXYZ();
        
        ChartTwoDataCartesian* getChartMapLinePrivate(const int32_t chartMapIndex);
        
        bool getBoundsPrivate(const int32_t mapIndex,
                              BoundingBox& boundingBoxOut) const;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::vector<std::unique_ptr<ChartTwoDataCartesian>> m_mapLineCharts;
        
        std::vector<AString> m_mapLineChartNames;
        
        std::vector<float> m_voxelXYZ;
        
        ChartTwoLineLayerContentTypeEnum::Enum m_lineLayerContentType;

        bool m_volumeAttributesValid = false;
        
        // ADD_NEW_MEMBERS_HERE

        friend class ChartTwoOverlay;
    };
    
#ifdef __CHARTABLE_TWO_FILE_LINE_LAYER_CHART_DECLARE__
#endif // __CHARTABLE_TWO_FILE_LINE_LAYER_CHART_DECLARE__

} // namespace
#endif  //__CHARTABLE_TWO_FILE_LINE_LAYER_CHART_H__
