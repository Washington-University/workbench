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

#include <memory>
#include <set>
#include "BrainOpenGLChartTwoDrawingInterface.h"
#include "CaretColor.h"
#include "ChartAxisLocationEnum.h"
#include "ChartTwoDataTypeEnum.h"
#include "ChartTwoMatrixTriangularViewingModeEnum.h"
#include "GraphicsPrimitive.h"

namespace caret {

    class Annotation;
    class AnnotationPercentSizeText;
    class BrainOpenGLViewportContent;
    class BrowserTabContent;
    class CaretPreferences;
    class ChartTwoCartesianAxis;
    class ChartTwoCartesianOrientedAxes;
    class ChartTwoDataCartesian;
    class ChartTwoOverlay;
    class ChartTwoOverlaySet;
    class ChartTwoTitle;
    class ChartableTwoFileHistogramChart;
    class ChartableTwoFileLineLayerChart;
    class ChartableTwoFileLineSeriesChart;
    class ChartableTwoFileMatrixChart;
    class SelectionItemAnnotation;
    class SelectionItemChartTwoHistogram;
    class SelectionItemChartTwoLabel;
    class SelectionItemChartTwoLineLayer;
    class SelectionItemChartTwoLineLayerVerticalNearest;
    class SelectionItemChartTwoLineSeries;
    class SelectionItemChartTwoMatrix;
    
    class BrainOpenGLChartTwoDrawingFixedPipeline : public BrainOpenGLChartTwoDrawingInterface {
        
    public:
        BrainOpenGLChartTwoDrawingFixedPipeline(const BrainOpenGLViewportContent* viewportContent);
        
        virtual ~BrainOpenGLChartTwoDrawingFixedPipeline();
        
        virtual void drawChartOverlaySet(Brain* brain,
                                         ModelChartTwo* chartTwoModel,
                                         BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                         const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                         const int32_t viewport[4],
                                         std::vector<Annotation*>& annotationsOut) override;

        // ADD_NEW_METHODS_HERE

    private:
        /**
         * Contains histogram for drawing
         */
        class HistogramChartDrawingInfo {
        public:
            HistogramChartDrawingInfo(ChartableTwoFileHistogramChart* histogramChart,
                                      int32_t mapIndex,
                                      const bool allMapsSelected);
            
            ChartableTwoFileHistogramChart* m_histogramChart;
            int32_t m_mapIndex;
            const bool m_allMapsSelected;
            
            ~HistogramChartDrawingInfo();
        };
        
        /**
         * Contains line chart for drawing line layer
         */
        class LineLayerChartDrawingInfo {
        public:
            LineLayerChartDrawingInfo(const ChartableTwoFileLineLayerChart* lineLayerChart,
                                      ChartTwoDataCartesian* chartTwoCartesianData,
                                      const ChartTwoOverlay* chartTwoOverlay,
                                      const CaretColor& lineChartColor,
                                      const float lineWidth)
            : m_lineLayerChart(lineLayerChart),
            m_chartTwoCartesianData(chartTwoCartesianData),
            m_chartTwoOverlay(chartTwoOverlay),
            m_lineChartColor(lineChartColor),
            m_lineWidth(lineWidth) { }
            
            const ChartableTwoFileLineLayerChart* m_lineLayerChart;
            ChartTwoDataCartesian* m_chartTwoCartesianData;
            const ChartTwoOverlay* m_chartTwoOverlay;
            const CaretColor m_lineChartColor;
            const float m_lineWidth;
        };
        
        /**
         * Contains line chart for drawing line series
         */
        class LineSeriesChartDrawingInfo {
        public:
            LineSeriesChartDrawingInfo(const ChartableTwoFileLineSeriesChart* lineSeriesChart,
                                       const ChartTwoDataCartesian* chartTwoCartesianData)
            : m_lineSeriesChart(lineSeriesChart),
            m_chartTwoCartesianData(chartTwoCartesianData) { }
            
            const ChartableTwoFileLineSeriesChart* m_lineSeriesChart;
            const ChartTwoDataCartesian* m_chartTwoCartesianData;
        };
        
        class MatrixChartDrawingInfo {
        public:
            MatrixChartDrawingInfo(const ChartableTwoFileMatrixChart* matrixChart,
                                   const GraphicsPrimitive* matrixPrimitive,
                                   const ChartTwoOverlay* chartTwoOverlay,
                                   const ChartTwoMatrixTriangularViewingModeEnum::Enum triangularMode,
                                   const float opacity)
            : m_matrixChart(matrixChart),
            m_matrixPrimitive(matrixPrimitive),
            m_chartTwoOverlay(chartTwoOverlay),
            m_triangularMode(triangularMode),
            m_opacity(opacity) { }
            
            const ChartableTwoFileMatrixChart* m_matrixChart;
            const GraphicsPrimitive* m_matrixPrimitive;
            const ChartTwoOverlay* m_chartTwoOverlay;
            const ChartTwoMatrixTriangularViewingModeEnum::Enum m_triangularMode;
            const float m_opacity;
        };
        /**
         * Determines size of title and draws the title
         */
        class TitleDrawingInfo {
        public:
            TitleDrawingInfo(BrainOpenGLTextRenderInterface* textRenderer,
                             const int32_t tabViewport[4],
                             const ChartTwoTitle* title);
            
            void setTitleViewport(const float leftAxisWidth,
                                  const float rightAxisWidth);
            
            void drawTitle(const float foregroundFloatRGBA[4]);
            
            float m_titleHeight = 0.0f;
            float m_titleWidth = 0.0f;
            bool m_titleDisplayedFlag;
            
        private:
            BrainOpenGLTextRenderInterface* m_textRenderer;
            const float m_tabViewportWidth = 0.0f;
            const float m_tabViewportHeight = 0.0f;
            const ChartTwoTitle* m_title;
            
            std::unique_ptr<AnnotationPercentSizeText> m_text;
            float m_paddingSizePixels = 0.0f;
            float m_titleViewport[4];
        };
        
        /**
         * Determines size of an axis and draws the axis
         */
        class AxisDrawingInfo {
        public:
            AxisDrawingInfo(BrainOpenGLTextRenderInterface* textRenderer,
                            const int32_t tabViewport[4],
                            const float dataMinX,
                            const float dataMaxX,
                            const float dataMinY,
                            const float dataMaxY,
                            const ChartAxisLocationEnum::Enum axisLocation,
                            const ChartTwoCartesianOrientedAxes* orientedAxes,
                            const ChartTwoCartesianAxis* axis,
                            const AString& labelText,
                            const std::vector<AString>& mapNames,
                            const float lineWidthPercentage);
            
            void setAxisViewport(const float titleHeight,
                                 const float bottomAxisHeight,
                                 const float topAxisHeight,
                                 const float leftAxisWidth,
                                 const float rightAxisWidth);

            void setLabelAndNumericsCoordinates(const float foregroundFloatRGBA[4],
                                                const float graphicsBoxLineThickness);
            
            void drawAxis(BrainOpenGLChartTwoDrawingFixedPipeline* chartDrawing,
                          ChartTwoOverlaySet* chartTwoOverlaySet,
                          const int32_t mouseX,
                          const int32_t mouseY,
                          const float foregroundFloatRGBA[4]);
            
            std::unique_ptr<AnnotationPercentSizeText> m_labelText;
            
            std::vector<AString> m_mapNames;
            
            std::vector<std::unique_ptr<AnnotationPercentSizeText>> m_numericsText;

            float m_axisWidth = 0.0f;
            float m_axisHeight = 0.0f;
            
            /** Axis minimum value is produced by the axis scaling user/auto */
            float m_axisMinimumValue = 0.0f;
            float m_axisMaximumValue = 0.0f;
            
            float m_labelPaddingSizePixels    = 0.0f;
            float m_numericsTicksPaddingSizePixels = 1.0f;
            bool m_axisValid = false;
            bool m_axisDisplayedFlag = false;
            
            int32_t m_axisViewport[4];
            
        private:
            const ChartAxisLocationEnum::Enum m_axisLocation;
            const ChartTwoCartesianOrientedAxes* m_orientedAxes;
            const ChartTwoCartesianAxis* m_axis;
            BrainOpenGLTextRenderInterface* m_textRenderer;
            const float m_tabViewportWidth = 0.0f;
            const float m_tabViewportHeight = 0.0f;
            float m_lineDrawingWidth = 1.0f;
            float m_tickLength = 0.0f;
            float m_labelWidth  = 0.0f;
            float m_labelHeight = 0.0f;
            
            void initializeNumericText(const float dataMinimumDataValue,
                                       const float dataMaximumDataValue,
                                       const bool showNumericFlag,
                                       const bool rotateNumericFlag,
                                       float& maxWidthOut,
                                       float& maxHeightOut);
            
            void initializeLabel(const AString& labelText,
                                       float& widthOut,
                                       float& heightOut);
            
        };
        
        class MatrixRowColumnHighight {
        public:
            MatrixRowColumnHighight(GraphicsPrimitive* graphicsPrimitive,
                                    const float modelViewMatrix[16])
            : m_graphicsPrimitive(std::unique_ptr<GraphicsPrimitive>(graphicsPrimitive)) {
                for (int32_t i = 0; i < 16; i++) m_modelViewMatrix[i] = modelViewMatrix[i];
            }
            
            std::unique_ptr<GraphicsPrimitive> m_graphicsPrimitive;
            float m_modelViewMatrix[16];
        };
        
        BrainOpenGLChartTwoDrawingFixedPipeline(const BrainOpenGLChartTwoDrawingFixedPipeline&);

        BrainOpenGLChartTwoDrawingFixedPipeline& operator=(const BrainOpenGLChartTwoDrawingFixedPipeline&);
        
        void restoreStateOfOpenGL();
        
        void saveStateOfOpenGL();
        
        void drawMatrixChartContent(const ChartableTwoFileMatrixChart* matrixChart,
                                    const ChartTwoMatrixTriangularViewingModeEnum::Enum chartViewingType,
                                    const float opacity,
                                    std::vector<MatrixRowColumnHighight*>& rowColumnHighlightingOut,
                                    const bool blendingEnabled);
        
        void drawHistogramOrLineChart(const ChartTwoDataTypeEnum::Enum chartDataType);
        
        void drawChartGraphicsBoxAndSetViewport(const float vpX,
                                                const float vpY,
                                                const float vpWidth,
                                                const float vpHeight,
                                                const float lineThicknessPercentage,
                                                const float titleHeight,
                                                const float bottomAxisHeight,
                                                const float topAxisHeight,
                                                const float leftAxisWidth,
                                                const float rightAxisWidth,
                                                const bool drawBoxFlag,
                                                int32_t chartGraphicsDrawingViewportOut[4]);
        
        bool createChartDrawingViewport(const float vpX,
                                        const float vpY,
                                        const float vpWidth,
                                        const float vpHeight,
                                        const float lineThicknessPercentage,
                                        const float titleHeight,
                                        const float bottomAxisHeight,
                                        const float topAxisHeight,
                                        const float leftAxisWidth,
                                        const float rightAxisWidth,
                                        int32_t chartGraphicsDrawingViewportOut[4]);
        
        void drawPrimitivePrivate(GraphicsPrimitive* primitive);
        
        void updateViewportContentForCharting(const int32_t viewport[4]);
        
        static float convertPercentageOfViewportToPixels(const float percentageWidthOrHeight,
                                                         const float viewportWidthOrHeight);
        
        static float convertPercentageOfViewportToOpenGLLineWidth(const float percentageWidthOrHeight,
                                                                  const float viewportWidthOrHeight);
        
        Brain* m_brain;
        
        const BrainOpenGLViewportContent* m_viewportContent;
        
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
        
        SelectionItemChartTwoLineLayer* m_selectionItemLineLayer;

        SelectionItemChartTwoLineLayerVerticalNearest* m_selectionItemLineLayerVerticalNearest;
        
        SelectionItemChartTwoLineSeries* m_selectionItemLineSeries;
        
        SelectionItemChartTwoMatrix* m_selectionItemMatrix;

        SelectionItemChartTwoLabel* m_selectionItemChartLabel;

        bool m_identificationModeFlag;
        
        /** These annotations will be drawn by the annotation drawing code */
        std::vector<Annotation*> m_annotationsForDrawingOutput;
        
        static float s_minimumLineWidthOpenGL;
        static float s_maximumLineWidthOpenGL;
        
        static const int32_t IDENTIFICATION_INDICES_PER_HISTOGRAM      = 2;
        static const int32_t IDENTIFICATION_INDICES_PER_CHART_LINE     = 2;
        static const int32_t IDENTIFICATION_INDICES_PER_MATRIX_ELEMENT = 2;
        
        // ADD_NEW_MEMBERS_HERE

        friend class AxisDrawingInfo;
    };
    
#ifdef __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_DECLARE__
    float BrainOpenGLChartTwoDrawingFixedPipeline::s_minimumLineWidthOpenGL = 1.0f;
    float BrainOpenGLChartTwoDrawingFixedPipeline::s_maximumLineWidthOpenGL = 1.0f;
#endif // __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_H__
