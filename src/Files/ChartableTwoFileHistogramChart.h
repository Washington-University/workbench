#ifndef __CHARTABLE_TWO_FILE_HISTOGRAM_CHART_H__
#define __CHARTABLE_TWO_FILE_HISTOGRAM_CHART_H__

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
#include <map>

#include "ChartableTwoFileBaseChart.h"
#include "ChartTwoHistogramContentTypeEnum.h"
#include "GraphicsPrimitiveV3fC4f.h"  // forward declaration has problems

namespace caret {
    class Histogram;
    
    class ChartableTwoFileHistogramChart : public ChartableTwoFileBaseChart {
        
    public:
        /**
         * Contains the graphics primitives for drawing
         * the histogram threshold, bars, and envelope.
         */
        class HistogramPrimitives {
        public:
            HistogramPrimitives();
            
            HistogramPrimitives(GraphicsPrimitiveV3fC4f* thresholdPrimitive,
                                GraphicsPrimitiveV3fC4f* barsPrimitive,
                                GraphicsPrimitiveV3fC4f* envelopePrimitive);
            
            ~HistogramPrimitives();
            
            /*
             * @return Graphics primitive for drawing threshlold.
             * NULL if invalid.
             */
            GraphicsPrimitiveV3fC4f* getThresholdPrimitive() { return m_thresholdPrimitive.get(); }
            
            /*
             * @return Graphics primitive for drawing histogram bars.
             * NULL if invalid.
             */
            GraphicsPrimitiveV3fC4f* getBarsPrimitive() { return m_barsPrimitive.get(); }
            
            /*
             * @return Graphics primitive for drawing histogram envelope.
             * NULL if invalid.
             */
            GraphicsPrimitiveV3fC4f* getEnvelopePrimitive() { return m_envelopePrimitive.get(); }
            
        private:
            std::unique_ptr<GraphicsPrimitiveV3fC4f> m_thresholdPrimitive;
            std::unique_ptr<GraphicsPrimitiveV3fC4f> m_barsPrimitive;
            std::unique_ptr<GraphicsPrimitiveV3fC4f> m_envelopePrimitive;
        };
        
        ChartableTwoFileHistogramChart(const ChartTwoHistogramContentTypeEnum::Enum histogramContentType,
                                               CaretMappableDataFile* parentCaretMappableDataFile);
        
        virtual ~ChartableTwoFileHistogramChart();
        
        ChartTwoHistogramContentTypeEnum::Enum getHistogramContentType() const;
        
        virtual bool isValid() const override;
        
        virtual bool isEmpty() const override;
        
        const Histogram* getHistogramForChartDrawing(const int32_t mapIndex,
                                                     const bool useDataFromAllMapsFlag);
        
        HistogramPrimitives* getMapHistogramDrawingPrimitives(const int32_t mapIndex,
                                                             const bool useDataFromAllMapsFlag);
        
        void invalidateAllColoring();
        
        // ADD_NEW_METHODS_HERE
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass) override;

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass) override;

    private:
        ChartableTwoFileHistogramChart(const ChartableTwoFileHistogramChart&);

        ChartableTwoFileHistogramChart& operator=(const ChartableTwoFileHistogramChart&);
        
        SceneClassAssistant* m_sceneAssistant;

        const ChartTwoHistogramContentTypeEnum::Enum m_histogramContentType;
        
        typedef std::map<int32_t, std::unique_ptr<HistogramPrimitives>> MapIndexPrimitiveContainer;
        
        MapIndexPrimitiveContainer m_mapHistogramBarsPrimitive;
        
        MapIndexPrimitiveContainer m_mapHistogramThresholdPrimitive;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_TWO_FILE_HISTOGRAM_CHART_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_TWO_FILE_HISTOGRAM_CHART_DECLARE__

} // namespace
#endif  //__CHARTABLE_TWO_FILE_HISTOGRAM_CHART_H__
