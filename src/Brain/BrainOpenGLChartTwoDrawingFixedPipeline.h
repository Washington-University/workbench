#ifndef __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_H__
#define __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_H__

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

#include <set>
#include "BrainOpenGLChartTwoDrawingInterface.h"
#include "CaretColorEnum.h"
#include "ChartAxisLocationEnum.h"
#include "ChartTwoDataTypeEnum.h"
#include "ChartTwoMatrixTriangularViewingModeEnum.h"

namespace caret {

    class Annotation;
    class AnnotationPercentSizeText;
    class BrowserTabContent;
    class CaretPreferences;
    class ChartTwoCartesianAxis;
    class ChartTwoDataCartesian;
    class ChartTwoOverlaySet;
    class ChartableTwoFileHistogramChart;
    class ChartableTwoFileLineSeriesChart;
    class ChartableTwoFileMatrixChart;
    class GraphicsPrimitive;
    class SelectionItemAnnotation;
    class SelectionItemChartTwoHistogram;
    class SelectionItemChartTwoLineSeries;
    class SelectionItemChartTwoMatrix;
    
    class BrainOpenGLChartTwoDrawingFixedPipeline : public BrainOpenGLChartTwoDrawingInterface {
        
    public:
        BrainOpenGLChartTwoDrawingFixedPipeline();
        
        virtual ~BrainOpenGLChartTwoDrawingFixedPipeline();
        
        virtual void drawChartOverlaySet(Brain* brain,
                                         BrainOpenGLViewportContent* viewportContent,
                                         ModelChartTwo* chartTwoModel,
                                         BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                         const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                         const int32_t viewport[4],
                                         std::vector<Annotation*>& annotationsOut) override;

        // ADD_NEW_METHODS_HERE

    private:
        class HistogramChartDrawingInfo {
        public:
            HistogramChartDrawingInfo(ChartableTwoFileHistogramChart* histogramChart,
                                      int32_t mapIndex,
                                      ChartAxisLocationEnum::Enum verticalAxisLocation,
                                      const bool allMapsSelected);
            
            ChartableTwoFileHistogramChart* m_histogramChart;
            int32_t m_mapIndex;
            ChartAxisLocationEnum::Enum m_verticalAxisLocation;
            const bool m_allMapsSelected;
            
            ~HistogramChartDrawingInfo();
        };
        
        class LineSeriesChartDrawingInfo {
        public:
            LineSeriesChartDrawingInfo(const ChartableTwoFileLineSeriesChart* lineSeriesChart,
                                       const ChartTwoDataCartesian* chartTwoCartesianData,
                                       const ChartAxisLocationEnum::Enum verticalAxisLocation)
            : m_lineSeriesChart(lineSeriesChart),
            m_chartTwoCartesianData(chartTwoCartesianData),
            m_verticalAxisLocation(verticalAxisLocation) { }
            
            const ChartableTwoFileLineSeriesChart* m_lineSeriesChart;
            const ChartTwoDataCartesian* m_chartTwoCartesianData;
            const ChartAxisLocationEnum::Enum m_verticalAxisLocation;
        };
        
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
        
        class RowColumnMinMax {
        public:
            RowColumnMinMax() { }

            int32_t m_min =  9999999;
            int32_t m_max = -9999999;
        };
        
        class AxisDrawingInfo {
        public:
            AxisDrawingInfo(BrainOpenGLTextRenderInterface* textRenderer,
                            const int32_t tabViewport[4],
                            const float dataMinX,
                            const float dataMaxX,
                            const float dataMinY,
                            const float dataMaxY,
                            const ChartAxisLocationEnum::Enum axisLocation,
                            const ChartTwoCartesianAxis* axis,
                            const AString& labelText,
                            const float lineWidthPercentage);
            
            void setAxisViewport(const float bottomAxisHeight,
                                 const float topAxisHeight,
                                 const float leftAxisWidth,
                                 const float rightAxisWidth);

            void setLabelAndNumericsCoordinates(const float foregroundFloatRGBA[4]);
            
            void drawAxis(BrainOpenGLChartTwoDrawingFixedPipeline* chartDrawing,
                          const float foregroundFloatRGBA[4],
                          float& axisMinimumValueOut,
                          float& axisMaximumValueOut);
            
            std::unique_ptr<AnnotationPercentSizeText> m_labelText;
            
            std::vector<std::unique_ptr<AnnotationPercentSizeText>> m_numericsText;

            float m_axisWidth = 0.0f;
            float m_axisHeight = 0.0f;
            
            /** Axis minimum value is produced by the axis scaling user/auto */
            float m_axisMinimumValue = 0.0f;
            float m_axisMaximumValue = 0.0f;
            
            float m_labelPaddingSizePixels    = 0.0f;
            float m_numericsPaddingSizePixels = 0.0f;
            bool m_axisValid = false;
            bool m_axisDisplayedFlag = false;
            
            int32_t m_axisViewport[4];
            
        private:
            const ChartAxisLocationEnum::Enum m_axisLocation;
            const ChartTwoCartesianAxis* m_axis;
            BrainOpenGLTextRenderInterface* m_textRenderer;
            const float m_tabViewportX = 0.0f;
            const float m_tabViewportY = 0.0f;
            const float m_tabViewportWidth = 0.0f;
            const float m_tabViewportHeight = 0.0f;
            float m_lineDrawingWidth = 1.0f;
            float m_tickLength = 0.0f;
            
            void initializeNumericText(const float dataMinimumDataValue,
                                       const float dataMaximumDataValue,
                                       float& maxWidthOut,
                                       float& maxHeightOut);
            
            void initializeLabel(const AString& labelText,
                                       float& widthOut,
                                       float& heightOut);
            
        };
        
        class AxisSizeInfo {
        public:
            AxisSizeInfo()
            : m_width(0.0),
            m_height(0.0),
            m_paddingSize(0.0),
            m_linesWidth(0.0),
            m_tickLength(0.0),
            m_axisSize(0.0),
            m_labelOffset(0.0),
            m_numericsOffset(0.0) { }
            
            AxisSizeInfo(const double width,
                         const double height,
                         const double paddingSize,
                         const double linesWidth,
                         const double tickLength,
                         const double labelHeight,
                         const double numericsWidthOrHeight)
            : m_width(width),
            m_height(height),
            m_paddingSize(paddingSize),
            m_linesWidth(linesWidth),
            m_tickLength(tickLength) {
                m_labelOffset = m_paddingSize;
                m_axisSize = (m_paddingSize
                              + labelHeight
                              + numericsWidthOrHeight
                              + tickLength);
                m_numericsOffset = m_axisSize - tickLength;
            }
            
            double m_width;
            double m_height;
            double m_paddingSize;
            double m_linesWidth;
            double m_tickLength;
            
            double m_axisSize;
            double m_labelOffset;
            double m_numericsOffset;
        };

        BrainOpenGLChartTwoDrawingFixedPipeline(const BrainOpenGLChartTwoDrawingFixedPipeline&);

        BrainOpenGLChartTwoDrawingFixedPipeline& operator=(const BrainOpenGLChartTwoDrawingFixedPipeline&);
        
        void restoreStateOfOpenGL();
        
        void saveStateOfOpenGL();
        
        void drawMatrixChart();
        
        void drawMatrixChartContent(const ChartableTwoFileMatrixChart* matrixChart,
                                 const ChartTwoMatrixTriangularViewingModeEnum::Enum chartViewingType,
                                 const float cellWidth,
                                 const float cellHeight,
                                 const float zooming);
        
        void drawHistogramOrLineSeriesChart(const ChartTwoDataTypeEnum::Enum chartDataType);
        
        void drawChartGraphicsBoxAndSetViewport(const float vpX,
                                                const float vpY,
                                                const float vpWidth,
                                                const float vpHeight,
                                                const float lineThickness,
                                                const Margins& margins,
                                                const bool drawBoxFlag,
                                                int32_t chartGraphicsDrawingViewportOut[4]);
        
        void drawChartGraphicsBoxAndSetViewport(const float vpX,
                                                const float vpY,
                                                const float vpWidth,
                                                const float vpHeight,
                                                const float lineThicknessPercentage,
                                                const float bottomAxisHeight,
                                                const float topAxisHeight,
                                                const float leftAxisWidth,
                                                const float rightAxisWidth,                                              const bool drawBoxFlag,
                                                int32_t chartGraphicsDrawingViewportOut[4]);
        
        bool drawChartAxisCartesian(const float minimumDataValue,
                                    const float maximumDataValue,
                                    const float tabViewportX,
                                    const float tabViewportY,
                                    const float tabViewportWidth,
                                    const float tabViewportHeight,
                                    const Margins& margins,
                                    const AxisSizeInfo& axisSizeInfo,
                                    ChartTwoCartesianAxis* axis,
                                    AnnotationPercentSizeText* chartAxisLabel,
                                    float& axisMinimumOut,
                                    float& axisMaximumOut);
        
        void drawChartTitle(const float tabViewportX,
                            const float tabViewportY,
                            const float tabViewportWidth,
                            const float tabViewportHeight,
                            const Margins& margins,
                            AnnotationPercentSizeText* chartTitle);
        
        void estimateCartesianChartAxisLegendsWidthHeight(const float minimumDataValue,
                                                          const float maximumDataValue,
                                                          const float viewportWidth,
                                                          const float viewportHeight,
                                                          const float lineWidthPercentage,
                                                          ChartTwoCartesianAxis* cartesianAxis,
                                                          AnnotationPercentSizeText* chartAxisLabel,
                                                          AxisSizeInfo& axisSizeInfoOut);
        
        void estimateChartTitleHeight(const float viewportWidth,
                                      const float viewportHeight,
                                      AnnotationPercentSizeText* chartTitle,
                                      double& heightOut);
        
        void drawPrimitivePrivate(GraphicsPrimitive* primitive);
        
        void updateViewportContentForCharting(const int32_t viewport[4]);
        
        static float convertPercentageOfViewportToPixels(const float percentageWidthOrHeight,
                                                         const float viewportWidthOrHeight);
        
        Brain* m_brain;
        
        BrainOpenGLViewportContent* m_viewportContent;
        
        BrowserTabContent* m_browserTabContent;
        
        ModelChartTwo* m_chartTwoModel;
        
        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing;
        
        BrainOpenGLTextRenderInterface* m_textRenderer;
        
        ChartTwoOverlaySet* m_chartOverlaySet;
        
        SelectionItemDataTypeEnum::Enum m_selectionItemDataType;
        
        int32_t m_viewport[4];
        
        int32_t m_tabIndex;
        
        float m_translation[3];
        
        float m_zooming;
        
        CaretPreferences* m_preferences;
        
        SelectionItemAnnotation* m_selectionItemAnnotation;
        
        SelectionItemChartTwoHistogram* m_selectionItemHistogram;
        
        SelectionItemChartTwoLineSeries* m_selectionItemLineSeries;

        SelectionItemChartTwoMatrix* m_selectionItemMatrix;

        const float GRID_LINE_WIDTH = 2.0;
        
        bool m_identificationModeFlag;
        
        /** These annotations will be drawn by the annotation drawing code */
        std::vector<Annotation*> m_annotationsForDrawingOutput;
        
        static const int32_t IDENTIFICATION_INDICES_PER_HISTOGRAM      = 2;
        static const int32_t IDENTIFICATION_INDICES_PER_CHART_LINE     = 2;
        static const int32_t IDENTIFICATION_INDICES_PER_MATRIX_ELEMENT = 2;
        
        static constexpr float s_tickLengthPixels = 5.0;

        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_H__
