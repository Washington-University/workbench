#ifndef __CHART_TWO_DATA_CARTESIAN_H__
#define __CHART_TWO_DATA_CARTESIAN_H__

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

#include "CaretColor.h"
#include "CaretObjectTracksModification.h"
#include "CaretUnitsTypeEnum.h"
#include "ChartTwoDataTypeEnum.h"
#include "GraphicsPrimitive.h"
#include "SceneableInterface.h"


namespace caret {

    class ChartPoint;
    class GraphicsPrimitiveV3f;
    class MapFileDataSelector;
    class SceneClassAssistant;
    
    class ChartTwoDataCartesian : public CaretObjectTracksModification, public SceneableInterface {
        
    public:
        ChartTwoDataCartesian(const ChartTwoDataTypeEnum::Enum chartDataType,
                              const CaretUnitsTypeEnum::Enum dataAxisUnitsX,
                              const CaretUnitsTypeEnum::Enum dataAxisUnitsY,
                              const GraphicsPrimitive::PrimitiveType graphicsPrimitiveType);
        
        virtual ~ChartTwoDataCartesian();
        
        virtual ChartTwoDataCartesian* clone() const;
        
        bool isSelected() const;
        
        void setSelected(const bool selectionStatus);
        
        void addPoint(const float x,
                      const float y);
        
        bool getBounds(BoundingBox& boundingBoxOut) const;
        
        GraphicsPrimitiveV3f* getGraphicsPrimitive() const;
        
        const MapFileDataSelector* getMapFileDataSelector() const;
        
        void setMapFileDataSelector(const MapFileDataSelector& mapFileDataSelector);
        
        CaretUnitsTypeEnum::Enum getDataAxisUnitsX();
        
        CaretUnitsTypeEnum::Enum getDataAxisUnitsY();
        
        CaretColor getColor() const;
        
        CaretColorEnum::Enum getColorEnum() const;
        
        void setColorEnum(const CaretColorEnum::Enum colorEnum);
        
        void setColor(const CaretColor& color);
        
        float getLineWidth() const;
        
        void setLineWidth(const float lineWidth);
        
        float getTimeStartInSecondsAxisX() const;
        
        void setTimeStartInSecondsAxisX(const float timeStart);
        
        float getTimeStepInSecondsAxisX() const;
        
        void setTimeStepInSecondsAxisX(const float timeStep);

        // ADD_NEW_METHODS_HERE

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        /**
         * @return The default line width for cartesian charts.
         */
        static float getDefaultLineWidth() { return 0.5f; }
        
    protected:
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass);
//        
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass);
        
        
    private:
        ChartTwoDataCartesian(const ChartTwoDataCartesian& obj);
        
        ChartTwoDataCartesian& operator=(const ChartTwoDataCartesian& obj);
        
        void copyHelperChartTwoDataCartesian(const ChartTwoDataCartesian& obj);

        void initializeMembersChartTwoDataCartesian();
        
        std::unique_ptr<GraphicsPrimitiveV3f> createGraphicsPrimitive();
        
        std::unique_ptr<MapFileDataSelector> m_mapFileDataSelector;
        
        std::unique_ptr<GraphicsPrimitiveV3f> m_graphicsPrimitive;
        
        CaretUnitsTypeEnum::Enum m_dataAxisUnitsX;
        
        CaretUnitsTypeEnum::Enum m_dataAxisUnitsY;
        
        const GraphicsPrimitive::PrimitiveType m_graphicsPrimitiveType;
        
        bool m_selectionStatus;
        
        CaretColor m_caretColor;
        
        float m_lineWidth;
        
        float m_timeStartInSecondsAxisX;
        
        float m_timeStepInSecondsAxisX;
        
        static int32_t caretColorIndex;
        
        SceneClassAssistant* m_sceneAssistant;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_DATA_CARTESIAN_DECLARE__
    int32_t ChartTwoDataCartesian::caretColorIndex = 0;
#endif // __CHART_TWO_DATA_CARTESIAN_DECLARE__

} // namespace
#endif  //__CHART_TWO_DATA_CARTESIAN_H__
