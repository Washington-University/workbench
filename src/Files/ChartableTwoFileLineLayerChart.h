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
#include "CaretColorEnum.h"
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
        
        ChartTwoDataCartesian* getChartMapLine(const int32_t chartMapIndex);
        
        CaretColorEnum::Enum getDefaultColor() const;
        
        void setDefaultColor(const CaretColorEnum::Enum defaultColor);
        
        float getDefaultLineWidth() const;
        
        void setDefaultLineWidth(const float defaultLineWidth);
        
        void clearChartLines();
        
        bool getBounds(const int32_t mapIndex,
                       BoundingBox& boundingBoxOut) const;
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass) override;

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass) override;

    private:
        static CaretColorEnum::Enum generateDefaultColor();
        
        void validateDefaultColor();
        
        ChartTwoDataCartesian* loadChartForMapFileSelector(const MapFileDataSelector& mapFileSelector);
        
        ChartTwoDataCartesian* createChartData() const;
        
        void getMapNamesFromCiftiBrainMap(const CiftiBrainModelsMap& brainModelsMap,
                                          std::vector<AString>& mapNames);

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::vector<std::unique_ptr<ChartTwoDataCartesian>> m_mapLineCharts;
        
        std::vector<AString> m_mapLineChartNames;
        
        ChartTwoLineLayerContentTypeEnum::Enum m_lineLayerContentType;
        
        CaretColorEnum::Enum m_defaultColor = CaretColorEnum::BLUE;
        
        float m_defaultLineWidth = 1.0;

        // ADD_NEW_MEMBERS_HERE

        static int32_t s_defaultColorIndexGenerator;
    };
    
#ifdef __CHARTABLE_TWO_FILE_LINE_LAYER_CHART_DECLARE__
    int32_t ChartableTwoFileLineLayerChart::s_defaultColorIndexGenerator = 0;
#endif // __CHARTABLE_TWO_FILE_LINE_LAYER_CHART_DECLARE__

} // namespace
#endif  //__CHARTABLE_TWO_FILE_LINE_LAYER_CHART_H__
