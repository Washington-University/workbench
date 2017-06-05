#ifndef __BRAIN_OPEN_G_L_CHART_DRAWING_FIXED_PIPELINE_H__
#define __BRAIN_OPEN_G_L_CHART_DRAWING_FIXED_PIPELINE_H__

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


#include "BrainOpenGLChartDrawingInterface.h"
#include "CaretColorEnum.h"

namespace caret {

    class Brain;
    class BrainOpenGLFixedPipeline;
    class ChartAxis;
    class ChartAxisCartesian;
    class ChartDataCartesian;
    class ChartModelCartesian;
    class ChartModelDataSeries;
    class ChartModelFrequencySeries;
    class ChartModelTimeSeries;
    class ChartableMatrixInterface;
    
    class BrainOpenGLChartDrawingFixedPipeline : public BrainOpenGLChartDrawingInterface {
        
    public:
        BrainOpenGLChartDrawingFixedPipeline();
        
        virtual ~BrainOpenGLChartDrawingFixedPipeline();
        
        virtual void drawCartesianChart(Brain* brain,
                                        BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                        const int32_t viewport[4],
                                        BrainOpenGLTextRenderInterface* textRenderer,
                                        ChartModelCartesian* cartesianChart,
                                        const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                        const int32_t tabIndex);
        
        virtual void drawMatrixChart(Brain* brain,
                                     BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                     const int32_t viewport[4],
                                     BrainOpenGLTextRenderInterface* textRenderer,
                                     ChartableMatrixInterface* chartMatrixInterface,
                                     const int32_t scalarDataSeriesMapIndex,
                                     const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                     const int32_t tabIndex);
        
    private:
        class Margins {
        public:
            Margins(const double defaultSize) {
                m_bottom = defaultSize;
                m_left   = defaultSize;
                m_right  = defaultSize;
                m_top    = defaultSize;
            }
            
            double m_bottom;
            double m_left;
            double m_right;
            double m_top;
        };
        
        BrainOpenGLChartDrawingFixedPipeline(const BrainOpenGLChartDrawingFixedPipeline&);

        BrainOpenGLChartDrawingFixedPipeline& operator=(const BrainOpenGLChartDrawingFixedPipeline&);
        
        void drawChartGraphicsLineSeries(BrainOpenGLTextRenderInterface* textRenderer,
                                         ChartModelCartesian* chart);
        
        void drawChartGraphicsMatrix(const int32_t viewport[4],
                                     BrainOpenGLTextRenderInterface* textRenderer,
                                     ChartableMatrixInterface* chartMatrixInterface,
                                     const int32_t scalarDataSeriesMapIndex);

        void drawChartGraphicsBoxAndSetViewport(const float vpX,
                               const float vpY,
                               const float vpWidth,
                               const float vpHeight,
                               const Margins& margins,
                               int32_t chartGraphicsDrawingViewportOut[4]);
        
        void drawChartAxis(const float vpX,
                           const float vpY,
                           const float vpWidth,
                           const float vpHeight,
                           Margins& margins,
                           BrainOpenGLTextRenderInterface* textRenderer,
                           ChartAxis* axis);
        
        void drawChartAxisCartesian(const float vpX,
                                    const float vpY,
                                    const float vpWidth,
                                    const float vpHeight,
                                    Margins& margins,
                                    BrainOpenGLTextRenderInterface* textRenderer,
                                    ChartAxisCartesian* axis);
        
        void drawChartDataCartesian(const int32_t chartDataIndex,
                                    const ChartDataCartesian* chartDataCartesian,
                                    const float lineWidth,
                                    const float rgb[3]);
        
        void estimateCartesianChartAxisLegendsWidthHeight(BrainOpenGLTextRenderInterface* textRenderer,
                                                          const float viewportWidth,
                                                          const float viewportHeight,
                                                          ChartAxis* axis,
                                                          double& widthOut,
                                                          double& heightOut);
        
        void restoreStateOfOpenGL();
        
        void saveStateOfOpenGL();
        
        void addToChartLineIdentification(const int32_t chartDataIndex,
                                          const int32_t lineIndex,
                                          uint8_t rgbaForColorIdentification[4]);
        
        void addToChartMatrixIdentification(const int32_t matrixRowIndex,
                                          const int32_t matrixColumnIndex,
                                          uint8_t rgbaForColorIdentification[4]);
        
        void resetIdentification();
        
        void processIdentification();

    public:

        // ADD_NEW_METHODS_HERE

    private:
        Brain* m_brain;
        
        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing;
        
        ChartModelDataSeries* m_chartModelDataSeriesBeingDrawnForIdentification;
        
        ChartModelFrequencySeries* m_chartModelFrequencySeriesBeingDrawnForIdentification;
        
        ChartModelTimeSeries* m_chartModelTimeSeriesBeingDrawnForIdentification;
        
        SelectionItemDataTypeEnum::Enum m_chartCartesianSelectionTypeForIdentification;
        
        ChartableMatrixInterface* m_chartableMatrixInterfaceBeingDrawnForIdentification;
        
        SelectionItemDataTypeEnum::Enum m_chartableMatrixSelectionTypeForIdentification;
        
        int32_t m_tabIndex;

        std::vector<int32_t> m_identificationIndices;
        
        bool m_identificationModeFlag;
        
        // ADD_NEW_MEMBERS_HERE

        static const int32_t IDENTIFICATION_INDICES_PER_CHART_LINE;
        static const int32_t IDENTIFICATION_INDICES_PER_MATRIX_ELEMENT;
    };
    
#ifdef __BRAIN_OPEN_G_L_CHART_DRAWING_FIXED_PIPELINE_DECLARE__
    const int32_t BrainOpenGLChartDrawingFixedPipeline::IDENTIFICATION_INDICES_PER_CHART_LINE = 2;
    const int32_t BrainOpenGLChartDrawingFixedPipeline::IDENTIFICATION_INDICES_PER_MATRIX_ELEMENT = 2;
#endif // __BRAIN_OPEN_G_L_CHART_DRAWING_FIXED_PIPELINE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_CHART_DRAWING_FIXED_PIPELINE_H__
