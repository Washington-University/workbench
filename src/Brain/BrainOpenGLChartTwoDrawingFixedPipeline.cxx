
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

#define __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_DECLARE__
#include "BrainOpenGLChartTwoDrawingFixedPipeline.h"
#undef __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_DECLARE__

#include <algorithm>

#include "AnnotationCoordinate.h"
#include "AnnotationColorBar.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationPointSizeText.h"
#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoCartesianOrientedAxes.h"
#include "ChartTwoDataCartesian.h"
#include "ChartTwoLineSeriesHistory.h"
#include "ChartTwoMatrixDisplayProperties.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoOverlaySet.h"
#include "ChartTwoTitle.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "ChartableTwoFileMatrixChart.h"
#include "ChartableTwoFileLineLayerChart.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "DeveloperFlagsEnum.h"
#include "EventManager.h"
#include "EventOpenGLObjectToWindowTransform.h"
#include "FastStatistics.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3f.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "GraphicsShape.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "IdentificationManager.h"
#include "IdentificationWithColor.h"
#include "MathFunctions.h"
#include "ModelChartTwo.h"
#include "NodeAndVoxelColoring.h"
#include "PaletteColorMapping.h"
#include "SessionManager.h"
#include "SelectionItemAnnotation.h"
#include "SelectionItemChartTwoHistogram.h"
#include "SelectionItemChartTwoLabel.h"
#include "SelectionItemChartTwoLineLayer.h"
#include "SelectionItemChartTwoLineLayerVerticalNearest.h"
#include "SelectionItemChartTwoLineSeries.h"
#include "SelectionItemChartTwoMatrix.h"
#include "SelectionManager.h"

using namespace caret;

static bool debugFlag = false;

/**
 * \class caret::BrainOpenGLChartTwoDrawingFixedPipeline
 * \brief Drawing of version two charts.
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param viewportContent
 *     The content of the viewport.
 */
BrainOpenGLChartTwoDrawingFixedPipeline::BrainOpenGLChartTwoDrawingFixedPipeline(const BrainOpenGLViewportContent* viewportContent)
: BrainOpenGLChartTwoDrawingInterface(),
m_viewportContent(viewportContent)
{
    m_preferences = SessionManager::get()->getCaretPreferences();
    
    BrainOpenGL::getMinMaxLineWidth(s_minimumLineWidthOpenGL,
                                    s_maximumLineWidthOpenGL);
}

/**
 * Destructor.
 */
BrainOpenGLChartTwoDrawingFixedPipeline::~BrainOpenGLChartTwoDrawingFixedPipeline()
{
}

/**
 * Draw charts from a chart overlay.
 *
 * @param brain
 *     Brain.
 * @param chartTwoModel
 *     The chart two model.
 * @param fixedPipelineDrawing
 *     The fixed pipeline OpenGL drawing.
 * @param selectionItemDataType
 *     Selected data type.
 * @param viewport
 *     Viewport for the chart.
 * @param annotationsOut
 *     Output containing annotations that will be drawing by annotation drawing code.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawChartOverlaySet(Brain* brain,
                                                             ModelChartTwo* chartTwoModel,
                                                             BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                             const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                                             const int32_t viewport[4],
                                                             std::vector<Annotation*>& annotationsOut)
{
    annotationsOut.clear();
    m_annotationsForDrawingOutput.clear();
    
    CaretAssert(m_viewportContent);
    BrowserTabContent* browserTabContent = m_viewportContent->getBrowserTabContent();
    CaretAssert(browserTabContent);
    CaretAssert(brain);
    CaretAssert(browserTabContent);
    CaretAssert(chartTwoModel);
    CaretAssert(fixedPipelineDrawing);
    
    m_brain = brain;
    m_chartTwoModel = chartTwoModel;
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    m_textRenderer = fixedPipelineDrawing->getTextRenderer();
    m_browserTabContent = browserTabContent;
    m_translation[0] = 0.0;
    m_translation[1] = 0.0;
    m_translation[2] = 0.0;
    m_browserTabContent->getTranslation(m_translation);
    m_tabIndex = m_browserTabContent->getTabNumber();
    m_zooming        = m_browserTabContent->getScaling();
    m_chartOverlaySet = m_chartTwoModel->getChartTwoOverlaySet(m_tabIndex);
    m_selectionItemDataType = selectionItemDataType;
    m_viewport[0] = viewport[0];
    m_viewport[1] = viewport[1];
    m_viewport[2] = viewport[2];
    m_viewport[3] = viewport[3];
    
    m_selectionItemAnnotation = m_brain->getSelectionManager()->getAnnotationIdentification();
    m_selectionItemHistogram  = m_brain->getSelectionManager()->getChartTwoHistogramIdentification();
    m_selectionItemLineSeries = m_brain->getSelectionManager()->getChartTwoLineSeriesIdentification();
    m_selectionItemLineLayer  = m_brain->getSelectionManager()->getChartTwoLineLayerIdentification();
    m_selectionItemLineLayerVerticalNearest = m_brain->getSelectionManager()->getChartTwoLineLayerVerticalNearestIdentification();
    m_selectionItemMatrix     = m_brain->getSelectionManager()->getChartTwoMatrixIdentification();
    m_selectionItemChartLabel = m_brain->getSelectionManager()->getChartTwoLabelIdentification();
    
    m_fixedPipelineDrawing->disableLighting();
    
    /*
     * Find color bars for this tab and decrease height of
     * viewport so chart is above color bars.
     */
    std::vector<AnnotationColorBar*> colorBars;
    m_browserTabContent->getAnnotationColorBars(colorBars);
    if ( ! colorBars.empty()) {
        int heightOfAllColorBars = 0;
        
        for (auto cb : colorBars) {
            bool useItFlag = false;
            switch (cb->getCoordinateSpace()) {
                case AnnotationCoordinateSpaceEnum::CHART:
                    break;
                case AnnotationCoordinateSpaceEnum::SPACER:
                    break;
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    break;
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                    useItFlag = true;
                    break;
                case AnnotationCoordinateSpaceEnum::VIEWPORT:
                    break;
                case AnnotationCoordinateSpaceEnum::WINDOW:
                    break;
            }
            
            if (useItFlag) {
                float bottomLeft[3], bottomRight[3], topRight[3], topLeft[3];
                float viewportXYZ[3] = { 5.0, 5.0, 0.0 };
                const bool boundsValid = cb->getShapeBounds(m_viewport[2], m_viewport[3],
                                                            viewportXYZ,
                                                            bottomLeft, bottomRight, topRight, topLeft);
                if (boundsValid) {
                    heightOfAllColorBars += (topRight[1] - bottomRight[1]);
                }
            }
        }
        
        if (heightOfAllColorBars > 0) {
            const int32_t extraSpace = 5;
            heightOfAllColorBars += extraSpace;
            if (heightOfAllColorBars < viewport[3]) {
                m_viewport[1] += heightOfAllColorBars;
                m_viewport[3] -= heightOfAllColorBars;
            }
        }
    }
    
    bool drawHistogramFlag  = true;
    bool drawLineSeriesFlag = true;
    bool drawLineLayerFlag  = true;
    bool drawMatrixFlag     = true;
    
    /*
     * Check for a 'selection' type mode
     */
    m_identificationModeFlag = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if ( (! m_selectionItemHistogram->isEnabledForSelection())
                && (! m_selectionItemAnnotation->isEnabledForSelection())
                && (! m_selectionItemChartLabel->isEnabledForSelection()) ) {
                drawHistogramFlag = false;
            }
            if ( (! m_selectionItemLineLayer->isEnabledForSelection())
                && (! m_selectionItemAnnotation->isEnabledForSelection())
                && (! m_selectionItemChartLabel->isEnabledForSelection()) ) {
                drawLineLayerFlag = false;
            }
            if ( (! m_selectionItemLineSeries->isEnabledForSelection())
                && (! m_selectionItemAnnotation->isEnabledForSelection())
                && (! m_selectionItemChartLabel->isEnabledForSelection()) ) {
                drawLineSeriesFlag = false;
            }
            if ( (! m_selectionItemMatrix->isEnabledForSelection())
                && (! m_selectionItemAnnotation->isEnabledForSelection()) ) {
                drawMatrixFlag = false;
            }
            if (drawHistogramFlag
                || drawLineSeriesFlag
                || drawMatrixFlag) {
                m_identificationModeFlag = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                return;
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    saveStateOfOpenGL();
    
    if (m_chartOverlaySet != NULL) {
        const int32_t numberOfOverlays = m_chartOverlaySet->getNumberOfDisplayedOverlays();
        if (numberOfOverlays > 0) {
            ChartTwoOverlay* topOverlay = m_chartOverlaySet->getOverlay(0);
            CaretAssert(topOverlay);
            CaretMappableDataFile* mapFile = NULL;
            ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
            int32_t selectedIndex = -1;
            topOverlay->getSelectionData(mapFile,
                                         selectedIndexType,
                                         selectedIndex);
            if (mapFile != NULL) {
                const ChartTwoDataTypeEnum::Enum chartDataType = topOverlay->getChartTwoDataType();
                switch (chartDataType) {
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                        if (drawHistogramFlag) {
                            drawHistogramOrLineChart(ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM);
                        }
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                        if (drawLineLayerFlag) {
                            drawHistogramOrLineChart(ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER);
                        }
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                        if (drawLineSeriesFlag) {
                            drawHistogramOrLineChart(ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES);
                        }
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                        if (drawMatrixFlag) {
                            drawHistogramOrLineChart(ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX);
                        }
                        break;
                }
            }
        }
    }
    
    restoreStateOfOpenGL();
    
    annotationsOut = m_annotationsForDrawingOutput;
}

/**
 * Draw a histogram or line series chart.
 *
 * @param chartDataType
 *     Type of chart to draw.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawHistogramOrLineChart(const ChartTwoDataTypeEnum::Enum chartDataType)
{
    bool drawHistogramFlag  = false;
    bool drawLineLayerFlag  = false;
    bool drawMatrixFlag     = false;
    bool drawLineSeriesFlag = false;
    switch (chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            CaretAssert(0);
            return;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            drawHistogramFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
            drawLineLayerFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            drawLineSeriesFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            drawMatrixFlag = true;
            break;
    }
    
    const int32_t tabViewportX = m_viewport[0];
    const int32_t tabViewportY = m_viewport[1];
    const int32_t tabViewportWidth  = m_viewport[2];
    const int32_t tabViewportHeight = m_viewport[3];
    
    int32_t chartGraphicsDrawingViewport[4] = {
        tabViewportX,
        tabViewportY,
        tabViewportWidth,
        tabViewportHeight
    };
    
    const int32_t numberOfOverlays = m_chartOverlaySet->getNumberOfDisplayedOverlays();
    CaretAssert(numberOfOverlays > 0);
    CaretUsedInDebugCompileOnly(const ChartTwoOverlay* topOverlay = m_chartOverlaySet->getOverlay(0));
    CaretUsedInDebugCompileOnly(const ChartTwoCompoundDataType* cdt = topOverlay->getChartTwoCompoundDataType());
    if (drawHistogramFlag) {
        CaretAssert(cdt->getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM);
    }
    else if (drawLineSeriesFlag) {
        CaretAssert(cdt->getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES);
    }
    else if (drawLineLayerFlag) {
        CaretAssert(cdt->getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER);
    }
    else if (drawMatrixFlag) {
        CaretAssert(cdt->getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX);
    }
    else {
        CaretAssert(0);
    }
    
    std::vector<std::unique_ptr<HistogramChartDrawingInfo>> histogramDrawingInfo;
    std::deque<LineSeriesChartDrawingInfo> lineSeriesChartsToDraw;
    std::deque<LineLayerChartDrawingInfo> lineLayerChartsToDraw;
    std::deque<MatrixChartDrawingInfo> matrixChartsToDraw;
    
    /*
     * Get the histogram drawing information and overall extent
     */
    float xMinBottomTop(0.0), xMaxBottomTop(0.0);
    m_chartOverlaySet->getHorizontalAxes()->getUserScaleMinimumMaximumValues(xMinBottomTop,
                                                                             xMaxBottomTop);

    float yMinLeftRight(0.0), yMaxLeftRight(0.0);
    m_chartOverlaySet->getVerticalAxes()->getUserScaleMinimumMaximumValues(yMinLeftRight,
                                                                           yMaxLeftRight);
    
    /*
     * Find histograms or line-series charts for drawing
     * and also find min/max coordinates on axes
     */
    for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
        ChartTwoOverlay* chartOverlay = m_chartOverlaySet->getOverlay(iOverlay);
        if ( ! chartOverlay->isEnabled()) {
            continue;
        }
        
        CaretMappableDataFile* mapFile = NULL;
        ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
        int32_t selectedIndex = -1;
        chartOverlay->getSelectionData(mapFile,
                                       selectedIndexType,
                                       selectedIndex);
        if (mapFile == NULL) {
            continue;
        }
        
        ChartableTwoFileDelegate* chartDelegate = mapFile->getChartingDelegate();
        
        if (drawHistogramFlag) {
            ChartableTwoFileHistogramChart* histogramChart = chartDelegate->getHistogramCharting();
            
            if (histogramChart->isValid()) {
                CaretAssert(selectedIndexType == ChartTwoOverlay::SelectedIndexType::MAP);
                AString errorMessage;
                histogramDrawingInfo.push_back(std::unique_ptr<HistogramChartDrawingInfo>(new HistogramChartDrawingInfo(histogramChart,
                                                                                                                        selectedIndex,
                                                                                                                        (chartOverlay->isAllMapsSupported()
                                                                                                                         && chartOverlay->isAllMapsSelected()))));
                const Histogram* histogram = histogramChart->getHistogramForChartDrawing(selectedIndex,
                                                                                         (chartOverlay->isAllMapsSupported()
                                                                                          && chartOverlay->isAllMapsSelected()));
                if (histogram == NULL) {
                    histogramDrawingInfo.pop_back();
                    continue;
                }
            }
        }
        
        if (drawLineSeriesFlag) {
            const ChartableTwoFileLineSeriesChart* lineSeriesChart = chartDelegate->getLineSeriesCharting();
            const ChartTwoLineSeriesHistory* lineSeriesHistory = lineSeriesChart->getHistory();
            const int32_t numHistory = lineSeriesHistory->getHistoryCount();
            for (int32_t iHistory = (numHistory - 1); iHistory >= 0; iHistory--) {
                const ChartTwoDataCartesian* data = lineSeriesHistory->getHistoryItem(iHistory);
                CaretAssert(data);
                if (data->isSelected()) {
                    BoundingBox boundingBox;
                    if (data->getBounds(boundingBox)) {
                        lineSeriesChartsToDraw.push_back(LineSeriesChartDrawingInfo(lineSeriesChart,
                                                                                    data));
                    }
                }
            }
        }
        
        if (drawLineLayerFlag) {
            ChartableTwoFileLineLayerChart* lineLayerChart = chartDelegate->getLineLayerCharting();
            if (lineLayerChart->isValid()) {
                ChartTwoDataCartesian* data = chartOverlay->getLineLayerChartDisplayedCartesianData();
                CaretAssert(data);
                if (data->isSelected()) {
                    BoundingBox boundingBox;
                    if (data->getBounds(boundingBox)) {
                        lineLayerChartsToDraw.push_back(LineLayerChartDrawingInfo(lineLayerChart,
                                                                                  data,
                                                                                  chartOverlay,
                                                                                  chartOverlay->getLineLayerColor(),
                                                                                  chartOverlay->getLineLayerLineWidth()));
                    }
                }
            }
        }
        
        if (drawMatrixFlag) {
            ChartableTwoFileMatrixChart* matrixChart = chartDelegate->getMatrixCharting();
            if (matrixChart->isValid()) {
                ChartTwoMatrixTriangularViewingModeEnum::Enum triangleMode = chartOverlay->getMatrixTriangularViewingMode();
                GraphicsPrimitive* primitive = matrixChart->getMatrixChartingGraphicsPrimitive(triangleMode,
                                                                                               CiftiMappableDataFile::MatrixGridMode::FILLED_TEXTURE);
                if (primitive->isValid()) {
                    BoundingBox boundingBox;
                    if (primitive->getVertexBounds(boundingBox)) {
                        matrixChartsToDraw.push_back(MatrixChartDrawingInfo(matrixChart,
                                                                            primitive,
                                                                            chartOverlay,
                                                                            triangleMode,
                                                                            chartOverlay->getMatrixOpacity()));
                    }
                }
            }
        }
    }
    
    /*
     * Bounds valid?
     * For X, maximum value must be greater than minimum value
     * For Y, maximum value must be greater than or equal to minimum value since
     * all data points may be zero.
     */
    const bool xBottomTopValid = (xMinBottomTop < xMaxBottomTop);
    const bool yLeftRightValid = (yMinLeftRight <= yMaxLeftRight);
    if (xBottomTopValid && yLeftRightValid) {
        /*
         * Make invalid ranges zero
         */
        if ( ! yLeftRightValid) {
            yMinLeftRight = 0.0;
            yMaxLeftRight = 0.0;
        }
        if ( ! xBottomTopValid) {
            xMinBottomTop = 0.0;
            xMaxBottomTop = 0.0;
        }
        
        ChartTwoCartesianAxis* leftAxis   = NULL;
        ChartTwoCartesianAxis* rightAxis  = NULL;
        ChartTwoCartesianAxis* bottomAxis = NULL;
        ChartTwoCartesianAxis* topAxis    = NULL;
        
        std::vector<ChartTwoCartesianAxis*> displayedAxes;
        m_chartOverlaySet->getDisplayedChartAxes(displayedAxes);
        for (auto axis : displayedAxes) {
            CaretAssert(axis);
            switch (axis->getAxisLocation()) {
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                    bottomAxis = axis;
                    break;
                case  ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                    leftAxis = axis;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                    rightAxis = axis;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                    topAxis = axis;
                    break;
            }
        }
        
        ChartTwoTitle* chartTitle = m_chartOverlaySet->getChartTitle();
        
        GLint vp[4];
        glGetIntegerv(GL_VIEWPORT, vp);
        
        glViewport(tabViewportX,
                   tabViewportY,
                   tabViewportWidth,
                   tabViewportHeight);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, tabViewportWidth, 0, tabViewportHeight, -1.0, 1.0);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        const float lineWidthPercentage = m_chartOverlaySet->getAxisLineThickness();
        
        /*
         * Estimate size of title
         */
        TitleDrawingInfo titleInfo(m_textRenderer,
                                   m_viewport,
                                   chartTitle);
        
        /*
         * Estimate axes sizes but at this point we can only find the
         * size one dimension (x or y)
         */
        AxisDrawingInfo leftAxisInfo(m_textRenderer,
                                     m_viewport,
                                     xMinBottomTop,
                                     xMaxBottomTop,
                                     yMinLeftRight,
                                     yMaxLeftRight,
                                     ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT,
                                     m_chartOverlaySet->getVerticalAxes(),
                                     leftAxis,
                                     m_chartOverlaySet->getAxisLabel(leftAxis),
                                     lineWidthPercentage);
        AxisDrawingInfo rightAxisInfo(m_textRenderer,
                                      m_viewport,
                                      xMinBottomTop,
                                      xMaxBottomTop,
                                      yMinLeftRight,
                                      yMaxLeftRight,
                                      ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT,
                                      m_chartOverlaySet->getVerticalAxes(),
                                      rightAxis,
                                      m_chartOverlaySet->getAxisLabel(rightAxis),
                                      lineWidthPercentage);
        AxisDrawingInfo bottomAxisInfo(m_textRenderer,
                                       m_viewport,
                                       xMinBottomTop,
                                       xMaxBottomTop,
                                       yMinLeftRight,
                                       yMaxLeftRight,
                                       ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM,
                                       m_chartOverlaySet->getHorizontalAxes(),
                                       bottomAxis,
                                       m_chartOverlaySet->getAxisLabel(bottomAxis),
                                       lineWidthPercentage);
        AxisDrawingInfo topAxisInfo(m_textRenderer,
                                    m_viewport,
                                    xMinBottomTop,
                                    xMaxBottomTop,
                                    yMinLeftRight,
                                    yMaxLeftRight,
                                    ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP,
                                    m_chartOverlaySet->getHorizontalAxes(),
                                    topAxis,
                                    m_chartOverlaySet->getAxisLabel(topAxis),
                                    lineWidthPercentage);
        
        float topTitleHeight   = titleInfo.m_titleHeight;
        const float topAxisHeight    = topAxisInfo.m_axisHeight;
        if (titleInfo.m_titleDisplayedFlag) {
            /* nothing */
        }
        else {
            topTitleHeight = 0.0f;
        }
        const float bottomAxisHeight = bottomAxisInfo.m_axisHeight;
        const float leftAxisWidth    = leftAxisInfo.m_axisWidth;
        const float rightAxisWidth   = rightAxisInfo.m_axisWidth;
        
        /*
         * The constructors were only able to find width or height but not both.
         * Now we can find the other value.
         */
        titleInfo.setTitleViewport(leftAxisWidth,
                                   rightAxisWidth);
        leftAxisInfo.setAxisViewport(topTitleHeight,
                                     bottomAxisHeight,
                                     topAxisHeight,
                                     leftAxisWidth,
                                     rightAxisWidth);
        rightAxisInfo.setAxisViewport(topTitleHeight,
                                      bottomAxisHeight,
                                      topAxisHeight,
                                      leftAxisWidth,
                                      rightAxisWidth);
        bottomAxisInfo.setAxisViewport(topTitleHeight,
                                       bottomAxisHeight,
                                       topAxisHeight,
                                       leftAxisWidth,
                                       rightAxisWidth);
        topAxisInfo.setAxisViewport(topTitleHeight,
                                    bottomAxisHeight,
                                    topAxisHeight,
                                    leftAxisWidth,
                                    rightAxisWidth);
        
        const float foregroundRGBA[4] = {
            m_fixedPipelineDrawing->m_foregroundColorFloat[0],
            m_fixedPipelineDrawing->m_foregroundColorFloat[1],
            m_fixedPipelineDrawing->m_foregroundColorFloat[2],
            1.0f
        };
        
        const float graphicsBoxLineThickness = convertPercentageOfViewportToOpenGLLineWidth(m_chartOverlaySet->getAxisLineThickness(),
                                                                                   tabViewportHeight);
        leftAxisInfo.setLabelAndNumericsCoordinates(foregroundRGBA,
                                                    graphicsBoxLineThickness);
        rightAxisInfo.setLabelAndNumericsCoordinates(foregroundRGBA,
                                                     graphicsBoxLineThickness);
        bottomAxisInfo.setLabelAndNumericsCoordinates(foregroundRGBA,
                                                      graphicsBoxLineThickness);
        topAxisInfo.setLabelAndNumericsCoordinates(foregroundRGBA,
                                                   graphicsBoxLineThickness);
        
        const bool drawAxesFlag = createChartDrawingViewport(tabViewportX,
                                                         tabViewportY,
                                                         tabViewportWidth,
                                                         tabViewportHeight,
                                                         m_chartOverlaySet->getAxisLineThickness(),
                                                         topTitleHeight,
                                                         bottomAxisHeight,
                                                         topAxisHeight,
                                                         leftAxisWidth,
                                                         rightAxisWidth,
                                                         chartGraphicsDrawingViewport);

        if (drawAxesFlag) {
            titleInfo.drawTitle(foregroundRGBA);
            leftAxisInfo.drawAxis(this,
                                  m_chartOverlaySet,
                                  m_fixedPipelineDrawing->mouseX,
                                  m_fixedPipelineDrawing->mouseY,
                                  foregroundRGBA);
            rightAxisInfo.drawAxis(this,
                                   m_chartOverlaySet,
                                   m_fixedPipelineDrawing->mouseX,
                                   m_fixedPipelineDrawing->mouseY,
                                   foregroundRGBA);
            topAxisInfo.drawAxis(this,
                                 m_chartOverlaySet,
                                 m_fixedPipelineDrawing->mouseX,
                                 m_fixedPipelineDrawing->mouseY,
                                 foregroundRGBA);
            bottomAxisInfo.drawAxis(this,
                                    m_chartOverlaySet,
                                    m_fixedPipelineDrawing->mouseX,
                                    m_fixedPipelineDrawing->mouseY,
                                    foregroundRGBA);
            
            drawChartGraphicsBoxAndSetViewport(tabViewportX,
                                               tabViewportY,
                                               tabViewportWidth,
                                               tabViewportHeight,
                                               m_chartOverlaySet->getAxisLineThickness(),
                                               topTitleHeight,
                                               bottomAxisHeight,
                                               topAxisHeight,
                                               leftAxisWidth,
                                               rightAxisWidth,
                                               true, /* draw the box */
                                               chartGraphicsDrawingViewport);
        }
        else {
            drawChartGraphicsBoxAndSetViewport(tabViewportX,
                                               tabViewportY,
                                               tabViewportWidth,
                                               tabViewportHeight,
                                               m_chartOverlaySet->getAxisLineThickness(),
                                               topTitleHeight,
                                               0.0f, /* bottom axis height */
                                               0.0f, /* top axis height */
                                               0.0f, /* left axis width */
                                               0.0f, /* right axis width */
                                               true, /* draw the box */
                                               chartGraphicsDrawingViewport);
        }
        
        /*
         * When the user is editing an axis minimum or maximum value,
         * their difference may become zero which will cause
         * a problem with the orthographic projection.  So
         * in this rare instance, make the maximum value
         * slightly larger than the minimum value.
         */
        const float smallRange = 0.01;
        if (xMinBottomTop >= xMaxBottomTop) {
            xMaxBottomTop = xMinBottomTop + smallRange;
        }
        if (yMinLeftRight >= yMaxLeftRight) {
            yMaxLeftRight = yMinLeftRight + smallRange;
        }
        
        glViewport(chartGraphicsDrawingViewport[0],
                   chartGraphicsDrawingViewport[1],
                   chartGraphicsDrawingViewport[2],
                   chartGraphicsDrawingViewport[3]);
        
        
        if (drawHistogramFlag) {
            /*
             * Draw the bars for all histogram and then draw the envelopes
             * so the envelopes are not obscured by the bars
             */
            for (auto& drawInfo : histogramDrawingInfo) {
                if (drawInfo->m_histogramChart->isValid()) {
                    const CaretMappableDataFile* cmdf = drawInfo->m_histogramChart->getCaretMappableDataFile();
                    CaretAssert(cmdf);
                    const PaletteColorMapping* paletteColorMapping = cmdf->getMapPaletteColorMapping(drawInfo->m_mapIndex);
                    const bool drawBarsFlag     = paletteColorMapping->isHistogramBarsVisible();
                    const bool drawEnvelopeFlag = paletteColorMapping->isHistogramEnvelopeVisible();
                    
                    if (drawBarsFlag
                        || drawEnvelopeFlag) {
                        
                        glMatrixMode(GL_PROJECTION);
                        glLoadIdentity();
                        CaretAssert(xMinBottomTop < xMaxBottomTop);
                        glOrtho(xMinBottomTop, xMaxBottomTop,
                                yMinLeftRight, yMaxLeftRight,
                                -10.0, 10.0);

                        glMatrixMode(GL_MODELVIEW);
                        glLoadIdentity();
                        
                        bool applyTransformationsFlag = false;
                        if (applyTransformationsFlag) {
                            glTranslatef(m_translation[0],
                                         m_translation[1],
                                         0.0);
                            
                            const float chartWidth  = chartGraphicsDrawingViewport[2];
                            const float chartHeight = chartGraphicsDrawingViewport[3];
                            const float halfWidth   = chartWidth  / 2.0;
                            const float halfHeight  = chartHeight / 2.0;
                            glTranslatef(halfWidth,
                                         halfHeight,
                                         0.0);
                            glScalef(m_zooming,
                                     m_zooming,
                                     1.0);
                            glTranslatef(-halfWidth,
                                         -halfHeight,
                                         0.0);
                        }
                        
                        /*
                         * Save the transformation matrices and the viewport
                         * If there is more than one line chart, this code will be executed
                         * several times but since the top overlay is drawn last, the contents
                         * of the top overlay will be used.
                         */
                        updateViewportContentForCharting(chartGraphicsDrawingViewport);
                        
                        ChartableTwoFileHistogramChart::HistogramPrimitives* histogramPrimitives =
                        drawInfo->m_histogramChart->getMapHistogramDrawingPrimitives(drawInfo->m_mapIndex,
                                                                                     drawInfo->m_allMapsSelected);
                        
                        if (histogramPrimitives != NULL) {
                            const float lineWidthPercentage = histogramPrimitives->getEnvelopeLineWidthPercentage();
                            
                            if (m_identificationModeFlag) {
                                int32_t primitiveIndex = -1;
                                float   primitiveDepth = 0.0;
                                
                                if (drawBarsFlag) {
                                    /*
                                     * Scale histogram bars in Y-axis so that identification
                                     * will function when mouse is above a bar.  This is helpful
                                     * for identification of bars with a very small height.
                                     */
                                    glPushMatrix();
                                    glScalef(1.0, 1000.0, 1.0);
                                    GraphicsEngineDataOpenGL::drawWithSelection(histogramPrimitives->getBarsPrimitive(),
                                                                                m_fixedPipelineDrawing->mouseX,
                                                                                m_fixedPipelineDrawing->mouseY,
                                                                                primitiveIndex,
                                                                                primitiveDepth);
                                    
                                    /*
                                     * Each bar is drawn using two triangles
                                     */
                                    CaretAssert(histogramPrimitives->getBarsPrimitive()->getPrimitiveType()
                                                == GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES);
                                    if (primitiveIndex > 0) {
                                        primitiveIndex /= 2;
                                    }
                                    glPopMatrix();
                                }
                                else if (drawEnvelopeFlag) {
                                    /*
                                     * Increase line width for identification
                                     */
                                    histogramPrimitives->getEnvelopePrimitive()->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                                                              (lineWidthPercentage + 0.5));
                                    GraphicsEngineDataOpenGL::drawWithSelection(histogramPrimitives->getEnvelopePrimitive(),
                                                                                m_fixedPipelineDrawing->mouseX,
                                                                                m_fixedPipelineDrawing->mouseY,
                                                                                primitiveIndex,
                                                                                primitiveDepth);
                                }
                                
                                if (primitiveIndex >= 0) {
                                    if (m_selectionItemHistogram->isOtherScreenDepthCloserToViewer(primitiveDepth)) {
                                        m_selectionItemHistogram->setHistogramChart(const_cast<ChartableTwoFileHistogramChart*>(drawInfo->m_histogramChart),
                                                                                    drawInfo->m_mapIndex,
                                                                                    primitiveIndex,
                                                                                    drawInfo->m_allMapsSelected);
                                    }
                                }
                            }
                            else {
                                drawPrimitivePrivate(histogramPrimitives->getThresholdPrimitive());
                                if (drawBarsFlag) {
                                    drawPrimitivePrivate(histogramPrimitives->getBarsPrimitive());
                                }
                                if (drawEnvelopeFlag) {
                                    histogramPrimitives->getEnvelopePrimitive()->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                                                              lineWidthPercentage);
                                    drawPrimitivePrivate(histogramPrimitives->getEnvelopePrimitive());
                                }
                            }
                        }
                    }
                }
            }
        }
        
        if (drawLineSeriesFlag) {
            for (const auto lineChart : lineSeriesChartsToDraw) {
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                CaretAssert(xMinBottomTop < xMaxBottomTop);
                CaretAssert(yMinLeftRight < yMaxLeftRight);
                glOrtho(xMinBottomTop, xMaxBottomTop,
                        yMinLeftRight, yMaxLeftRight,
                        -10.0, 10.0);

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                
                bool applyTransformationsFlag = false;
                if (applyTransformationsFlag) {
                    glTranslatef(m_translation[0],
                                 m_translation[1],
                                 0.0f);
                    
                    const float chartWidth  = chartGraphicsDrawingViewport[2];
                    const float chartHeight = chartGraphicsDrawingViewport[3];
                    const float halfWidth   = chartWidth  / 2.0f;
                    const float halfHeight  = chartHeight / 2.0f;
                    glTranslatef(halfWidth,
                                 halfHeight,
                                 0.0f);
                    glScalef(m_zooming,
                             m_zooming,
                             1.0f);
                    glTranslatef(-halfWidth,
                                 -halfHeight,
                                 0.0f);
                }
                
                if (m_identificationModeFlag) {
                    int32_t primitiveIndex = -1;
                    float   primitiveDepth = 0.0f;
                    
                    GraphicsEngineDataOpenGL::drawWithSelection(lineChart.m_chartTwoCartesianData->getGraphicsPrimitive(),
                                                                m_fixedPipelineDrawing->mouseX,
                                                                m_fixedPipelineDrawing->mouseY,
                                                                primitiveIndex,
                                                                primitiveDepth);
                    
                    if (primitiveIndex >= 0) {
                        if (m_selectionItemLineSeries->isOtherScreenDepthCloserToViewer(primitiveDepth)) {
                            m_selectionItemLineSeries->setLineSeriesChart(const_cast<ChartableTwoFileLineSeriesChart*>(lineChart.m_lineSeriesChart),
                                                                          const_cast<ChartTwoDataCartesian*>(lineChart.m_chartTwoCartesianData),
                                                                          primitiveIndex);
                        }
                    }
                }
                else {
                    GraphicsEngineDataOpenGL::draw(lineChart.m_chartTwoCartesianData->getGraphicsPrimitive());
                }
                
                /*
                 * Save the transformation matrices and the viewport
                 * If there is more than one line chart, this code will be executed
                 * several times but since the top overlay is drawn last, the contents
                 * of the top overlay will be used.
                 */
                updateViewportContentForCharting(chartGraphicsDrawingViewport);
            }
        }
    }

    if (drawLineLayerFlag) {
        
        /*
         * Need to draw tooltips last so they are not under other chart lines
         */
        std::vector<std::tuple<float, float, float, AnnotationPercentSizeText>> textToolTips;
        
        for (const auto lineChart : lineLayerChartsToDraw) {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            const float xMin(xMinBottomTop);
            const float xMax(xMaxBottomTop);
            const float yMin = yMinLeftRight;
            const float yMax = yMaxLeftRight;
            CaretAssert(xMin <= xMax);
            CaretAssert(yMin <= yMax);
            glOrtho(xMin, xMax,
                    yMin, yMax,
                    -10.0, 10.0);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            
            bool applyTransformationsFlag = false;
            if (applyTransformationsFlag) {
                glTranslatef(m_translation[0],
                             m_translation[1],
                             0.0f);
                
                const float chartWidth  = chartGraphicsDrawingViewport[2];
                const float chartHeight = chartGraphicsDrawingViewport[3];
                const float halfWidth   = chartWidth  / 2.0f;
                const float halfHeight  = chartHeight / 2.0f;
                glTranslatef(halfWidth,
                             halfHeight,
                             0.0f);
                glScalef(m_zooming,
                         m_zooming,
                         1.0f);
                glTranslatef(-halfWidth,
                             -halfHeight,
                             0.0f);
            }
            
            if (m_identificationModeFlag) {
                /*
                 * First find line nearest to mouse in vertical distance
                 */
                bool performNearestVerticalIdentificationFlag(false);
                switch (lineChart.m_chartTwoOverlay->getLineChartActiveMode()) {
                    case ChartTwoOverlayActiveModeEnum::ACTIVE:
                        performNearestVerticalIdentificationFlag = true;
                        break;
                    case ChartTwoOverlayActiveModeEnum::OFF:
                        break;
                    case ChartTwoOverlayActiveModeEnum::ON:
                        performNearestVerticalIdentificationFlag = true;
                        break;
                }
                if (performNearestVerticalIdentificationFlag) {
                    /*
                     * Identify point on line that is neartest to mouse
                     * in a vertical direction
                     */
                    EventOpenGLObjectToWindowTransform windowTransform(EventOpenGLObjectToWindowTransform::SpaceType::MODEL);
                    EventManager::get()->sendEvent(windowTransform.getPointer());
                    if (windowTransform.isValid()) {
                        const float mouseWindowXYZ[3] {
                            static_cast<float>(m_fixedPipelineDrawing->mouseX),
                            static_cast<float>(m_fixedPipelineDrawing->mouseY),
                            0.0f
                        };
                        float mouseModelXYZ[3];
                        windowTransform.inverseTransformPoint(mouseWindowXYZ,
                                                              mouseModelXYZ);
                        
                        if ((mouseModelXYZ[0] >= xMin)
                            && (mouseModelXYZ[0] <= xMax)
                            && (mouseModelXYZ[1] >= yMin)
                            && (mouseModelXYZ[1] <= yMax)) {
                            float distance(0.0);
                            int32_t pointIndex(-1);
                            float chartXYZ[3];
                            if (lineChart.m_chartTwoCartesianData->getVerticalDistanceToXY(mouseModelXYZ[0],
                                                                                           mouseModelXYZ[1],
                                                                                           distance,
                                                                                           pointIndex,
                                                                                           chartXYZ)) {
                                bool debugFlag(false);
                                if (debugFlag) {
                                    std::cout << "Distance: " << distance << std::endl;
                                    std::cout << "Chart XYZ: " << AString::fromNumbers(chartXYZ, 3, ",") << std::endl;
                                    float chartWindowXYZ[3];
                                    windowTransform.transformPoint(chartXYZ, chartWindowXYZ);
                                    std::cout << "   Chart Window XYZ: " << AString::fromNumbers(chartWindowXYZ, 3, ",") << std::endl;
                                    std::cout << "   Window XYZ: " << AString::fromNumbers(mouseWindowXYZ, 3, ",") << std::endl;
                                    std::cout << "   Window Distance: " << MathFunctions::distance3D(chartWindowXYZ, mouseWindowXYZ) << std::endl;
                                    float pointModelXYZ[3];
                                    lineChart.m_chartTwoCartesianData->getPointXYZ(pointIndex, pointModelXYZ);
                                    float pointWindowXYZ[3];
                                    windowTransform.transformPoint(pointModelXYZ, pointWindowXYZ);
                                    std::cout << "   Point Distance: " << MathFunctions::distance3D(pointWindowXYZ, mouseWindowXYZ) << std::endl;
                                }


                                m_selectionItemLineLayerVerticalNearest->setLineLayerChart(const_cast<ChartableTwoFileLineLayerChart*>(lineChart.m_lineLayerChart),
                                                                                           const_cast<ChartTwoDataCartesian*>(lineChart.m_chartTwoCartesianData),
                                                                                           const_cast<ChartTwoOverlay*>(lineChart.m_chartTwoOverlay),
                                                                                           distance,
                                                                                           pointIndex);
                            }
                        }
                        else {
                            /*
                             * Outside chart coordinate bounds
                             */
                            m_selectionItemLineLayerVerticalNearest->setOutsideChartBound(true);
                        }
                    }
                }
                else {
                    /*
                     * Identify line segment point under mouse
                     */
                    int32_t primitiveIndex = -1;
                    float   primitiveDepth = 0.0f;
                    
                    /*
                     * Increase width as thin lines are difficult to select
                     */
                    const float minWidth(8.0);
                    const float lineWidthPercentage(std::max(lineChart.m_lineWidth, minWidth));
                    lineChart.m_chartTwoCartesianData->setLineWidth(lineWidthPercentage);
                    GraphicsEngineDataOpenGL::drawWithSelection(lineChart.m_chartTwoCartesianData->getGraphicsPrimitive(),
                                                                m_fixedPipelineDrawing->mouseX,
                                                                m_fixedPipelineDrawing->mouseY,
                                                                primitiveIndex,
                                                                primitiveDepth);
                    
                    if (primitiveIndex >= 0) {
                        if (m_selectionItemLineLayer->isOtherScreenDepthCloserToViewer(primitiveDepth)) {
                            m_selectionItemLineLayer->setLineLayerChart(const_cast<ChartableTwoFileLineLayerChart*>(lineChart.m_lineLayerChart),
                                                                        const_cast<ChartTwoDataCartesian*>(lineChart.m_chartTwoCartesianData),
                                                                        const_cast<ChartTwoOverlay*>(lineChart.m_chartTwoOverlay),
                                                                        primitiveIndex);
                            m_selectionItemLineLayer->setScreenDepth(primitiveDepth);
                        }
                    }
                }
            }
            else {
                IdentificationManager* idManager = m_brain->getIdentificationManager();
                const float symbolSize = idManager->getChartLineLayerSymbolSize();
                const float textSize = idManager->getChartLineLayerToolTipTextSize();
                
                if (lineChart.m_lineChartColor != lineChart.m_chartTwoCartesianData->getColor()) {
                    lineChart.m_chartTwoCartesianData->setColor(lineChart.m_lineChartColor);
                }
                
                /*
                 * Prevent zero line width that causes OpenGL error
                 */
                const float lineWidth((lineChart.m_lineWidth > 0.0)
                                      ? lineChart.m_lineWidth
                                      : 0.1);
                lineChart.m_chartTwoCartesianData->setLineWidth(lineWidth);

                GraphicsEngineDataOpenGL::draw(lineChart.m_chartTwoCartesianData->getGraphicsPrimitive());
                
                bool showCircleFlag(false);
                bool showRingFlag(false);
                switch (lineChart.m_chartTwoOverlay->getLineChartActiveMode()) {
                    case ChartTwoOverlayActiveModeEnum::ACTIVE:
                        showCircleFlag = true;
                        break;
                    case ChartTwoOverlayActiveModeEnum::OFF:
                        break;
                    case ChartTwoOverlayActiveModeEnum::ON:
                        showRingFlag = true;
                        break;
                }
                if (showCircleFlag
                    | showRingFlag) {
                    std::array<float, 3> xyz;
                    if (lineChart.m_chartTwoOverlay->getSelectedLineChartPointXYZ(xyz)) {
                        const uint8_t foregroundRGBA[4] = {
                            m_fixedPipelineDrawing->m_foregroundColorByte[0],
                            m_fixedPipelineDrawing->m_foregroundColorByte[1],
                            m_fixedPipelineDrawing->m_foregroundColorByte[2],
                            255
                        };

                        /*
                         * May have NaN of Inf in the Y-component so
                         * play Y at middle of screen.  Need to leave
                         * the NaN/Inf in the xyz for conversion to text
                         * so that user sees NaN/Inf
                         */
                        auto xyzSymbol(xyz);
                        if ( ! MathFunctions::isNumeric(xyz[1])) {
                            xyzSymbol[1] = (yMinLeftRight + yMaxLeftRight) / 2.0;
                        }

                        std::array<float, 3> windowXYZ;
                        if (showCircleFlag) {
                            GraphicsShape::drawCircleFilledPercentViewportHeight(xyzSymbol.data(),
                                                                                 foregroundRGBA,
                                                                                 symbolSize,
                                                                                 &windowXYZ);
                        }
                        else if (showRingFlag) {
                            GraphicsShape::drawRingPercentViewportHeight(xyzSymbol.data(),
                                                                         foregroundRGBA,
                                                                         symbolSize * 0.65, /* inner diameter */
                                                                         symbolSize, /* diameter */
                                                                         &windowXYZ);
                        }
                        else {
                            CaretAssertMessage(0, "Does new shape need to be drawn?");
                        }
                        {
                            const QString info("Index: "
                                               + QString::number(lineChart.m_chartTwoOverlay->getSelectedLineChartPointIndex())
                                               + "\nX: "
                                               + QString::number(xyz[0], 'f')
                                               + "\nY: "
                                               + QString::number(xyz[1], 'f'));
                            AnnotationPercentSizeText text(AnnotationAttributesDefaultTypeEnum::NORMAL,
                                                            AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_HEIGHT);
                            text.setText(info);
                            text.setFontPercentViewportSize(textSize);
                            text.setBoldStyleEnabled(true);
                            text.setTextColor(CaretColorEnum::BLACK);
                            text.setBackgroundColor(CaretColorEnum::CUSTOM);
                            const uint8_t backgroundColor[4] { 200, 200, 200, 255 };
                            text.setCustomBackgroundColor(backgroundColor);
                            text.setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
                            text.getCoordinate()->setXYZ(windowXYZ.data());

                            const float vpWidth(chartGraphicsDrawingViewport[2]);
                            const float vpHeight(chartGraphicsDrawingViewport[3]);
                            float vpX(windowXYZ[0] - chartGraphicsDrawingViewport[0]);
                            float vpY(windowXYZ[1] - chartGraphicsDrawingViewport[1]);
                            const float offsetXY(10.0);
                            
                            double textWidth(0.0);
                            double textHeight(0.0);
                            m_textRenderer->getTextWidthHeightInPixels(text,BrainOpenGLTextRenderInterface::DrawingFlags(),
                                                                       vpWidth, vpHeight,
                                                                       textWidth, textHeight);
                            const double halfTextWidth(textWidth / 2.0);
                            
                            text.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
                            text.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                            switch (lineChart.m_chartTwoOverlay->getSelectedLineChartTextOffset()) {
                                case CardinalDirectionEnum::AUTO:
                                    if (vpX > (vpWidth / 2.0)) {
                                        /*
                                         * Place text to the left of the selected point.
                                         * Need additional offset of text width since the text is
                                         * aligned on the left.  Right alignment looks bad when
                                         * the lines of text are different lengths and aligned on right.
                                         */
                                        vpX -= (offsetXY + textWidth);
                                    }
                                    else {
                                        /*
                                         * Place text to the right of the selected point
                                         */
                                        vpX += offsetXY;
                                    }
                                    if (vpY > (vpHeight / 2.0)) {
                                        /*
                                         * Place text below selected point
                                         */
                                        vpY -= offsetXY;
                                        text.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                                    }
                                    else {
                                        /*
                                         * Place text above selected point
                                         */
                                        vpY += offsetXY;
                                        text.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                                    }
                                    break;
                                case CardinalDirectionEnum::EAST:
                                    vpX += offsetXY;
                                    text.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
                                    break;
                                case CardinalDirectionEnum::NORTH:
                                    vpX -= halfTextWidth;
                                    vpY += offsetXY;
                                    text.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                                    break;
                                case CardinalDirectionEnum::NORTHEAST:
                                    vpX += offsetXY;
                                    vpY += offsetXY;
                                    text.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                                    break;
                                case CardinalDirectionEnum::NORTHWEST:
                                    vpX -= (offsetXY + textWidth);
                                    vpY += offsetXY;
                                    text.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                                    break;
                                case CardinalDirectionEnum::SOUTH:
                                    vpX -= (halfTextWidth);
                                    vpY -= offsetXY;
                                    text.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                                    break;
                                case CardinalDirectionEnum::SOUTHEAST:
                                    vpX += offsetXY;
                                    vpY -= offsetXY;
                                    text.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                                    break;
                                case CardinalDirectionEnum::SOUTHWEST:
                                    vpX -= (offsetXY + textWidth);
                                    vpY -= offsetXY;
                                    text.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                                    break;
                                case CardinalDirectionEnum::WEST:
                                    vpX -= (offsetXY + textWidth);
                                    text.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
                                    break;
                            }
                            
                            const float vpZ(0.0);
                            textToolTips.push_back(std::make_tuple(vpX, vpY, vpZ, text));
                        }
                    }
                }
            }
            
            /*
             * Save the transformation matrices and the viewport
             * If there is more than one line chart, this code will be executed
             * several times but since the top overlay is drawn last, the contents
             * of the top overlay will be used.
             */
            updateViewportContentForCharting(chartGraphicsDrawingViewport);
        }
        
        /*
         * Draw tooltips AFTER lines to that the tooltips are
         * not behind any lines
         */
        for (auto tt : textToolTips) {
            m_textRenderer->drawTextAtViewportCoords(std::get<0>(tt),
                                                     std::get<1>(tt),
                                                     std::get<2>(tt),
                                                     std::get<3>(tt),
                                                     BrainOpenGLTextRenderInterface::DrawingFlags());
        }
    }
    
    if (drawMatrixFlag) {
        std::vector<MatrixRowColumnHighight*> rowColumnHighlighting;
        
        bool firstFlag(true);
        for (const auto matrixChart : matrixChartsToDraw) {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            const float xMin(xMinBottomTop);
            const float xMax(xMaxBottomTop);
            const float yMin = yMinLeftRight;
            const float yMax = yMaxLeftRight;
            CaretAssert(xMin <= xMax);
            CaretAssert(yMin <= yMax);
            glOrtho(xMin, xMax,
                    yMin, yMax,
                    -10.0, 10.0);
            
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            
            const float opacity(firstFlag ? 1.0 : matrixChart.m_opacity);
            drawMatrixChartContent(matrixChart.m_matrixChart,
                                   matrixChart.m_triangularMode,
                                   opacity,
                                   rowColumnHighlighting);
            firstFlag = false;
            
            /*
             * Save the transformation matrices and the viewport
             * If there is more than one line chart, this code will be executed
             * several times but since the top overlay is drawn last, the contents
             * of the top overlay will be used.
             */
            updateViewportContentForCharting(chartGraphicsDrawingViewport);
        }
        
        /*
         * Row/column highlighting must be drawn after matrices to that
         * the highlights are over any and all displayed matrices
         */
        const int32_t numHighlights = static_cast<int32_t>(rowColumnHighlighting.size());
        if (numHighlights > 0) {
            for (int32_t ctr = 0; ctr < numHighlights; ctr++) {
                MatrixRowColumnHighight* mrch = rowColumnHighlighting[ctr];
                CaretAssert(mrch);
                
                glPushMatrix();
                glLoadMatrixf(mrch->m_modelViewMatrix);
                
                drawPrimitivePrivate(mrch->m_graphicsPrimitive.get());
                delete mrch;
                
                glPopMatrix();
            }
            rowColumnHighlighting.clear();
        }
    }
    
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        const float xMin(xMinBottomTop);
        const float xMax(xMaxBottomTop);
        const float yMin = yMinLeftRight;
        const float yMax = yMaxLeftRight;
        CaretAssert(xMin <= xMax);
        CaretAssert(yMin <= yMax);
        glOrtho(xMin, xMax,
                yMin, yMax,
                -10.0, 10.0);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
        if (m_chartOverlaySet->getChartSelectionBounds(minX, minY, maxX, maxY)) {
            std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_BEVEL_JOIN,
                                                                                 this->m_fixedPipelineDrawing->m_foregroundColorFloat));
            primitive->addVertex(minX, minY);
            primitive->addVertex(maxX, minY);
            primitive->addVertex(maxX, maxY);
            primitive->addVertex(minX, maxY);
            primitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT, 1.5);
            drawPrimitivePrivate(primitive.get());
        }
    }
}

/*
 * Draw a matrix chart content.
 *
 * @param matrixChart
 *     Matrix chart that is drawn.
 * @param chartViewingType
 *     Type of chart viewing.
 * @param opacity
 *     Opacity for matrix drawing.
 * @param rowColumnHighlightingOut
 *     Current Output with row/column highlighting.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawMatrixChartContent(const ChartableTwoFileMatrixChart* matrixChart,
                                                                const ChartTwoMatrixTriangularViewingModeEnum::Enum chartViewingType,
                                                                const float /*opacity*/,
                                                                std::vector<MatrixRowColumnHighight*>& rowColumnHighlightingOut)
{
    GraphicsPrimitive* matrixPrimitive(NULL);
    const bool useTextureFlag(true);
    if (useTextureFlag) {
        matrixPrimitive = matrixChart->getMatrixChartingGraphicsPrimitive(chartViewingType,
                                                                          CiftiMappableDataFile::MatrixGridMode::FILLED_TEXTURE);
    }
    else {
        matrixPrimitive = matrixChart->getMatrixChartingGraphicsPrimitive(chartViewingType,
                                                                          CiftiMappableDataFile::MatrixGridMode::FILLED_TRIANGLES);
    }
    
    if (matrixPrimitive == NULL) {
        return;
    }
    
    if (m_identificationModeFlag) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    glPushMatrix();
    /*
     * Enable alpha blending so voxels that are not drawn from higher layers
     * allow voxels from lower layers to be seen.
     */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if (m_identificationModeFlag) {
        EventOpenGLObjectToWindowTransform transformEvent(EventOpenGLObjectToWindowTransform::SpaceType::MODEL);
        EventManager::get()->sendEvent(transformEvent.getPointer());
        if (transformEvent.isValid()) {
            const float windowPos[3] {
                static_cast<float>(m_fixedPipelineDrawing->mouseX),
                static_cast<float>(m_fixedPipelineDrawing->mouseY),
                0.0f
            };
            float modelPos[3];
            transformEvent.inverseTransformPoint(windowPos,
                                                 modelPos);
            
            int32_t numberOfRows(0);
            int32_t numberOfColumns(0);
            matrixChart->getMatrixDimensions(numberOfRows,
                                             numberOfColumns);
            const int32_t rowIndex(numberOfRows - modelPos[1]);
            const int32_t colIndex(modelPos[0]);
            
            if ((rowIndex >= 0)
                && (rowIndex < numberOfRows)
                && (colIndex >= 0)
                && (colIndex < numberOfColumns)) {
                m_selectionItemMatrix->setMatrixChart(const_cast<ChartableTwoFileMatrixChart*>(matrixChart),
                                                      rowIndex,
                                                      colIndex);
            }
        }
    }
    else {
        drawPrimitivePrivate(matrixPrimitive);
        
        const ChartTwoMatrixDisplayProperties* matrixProperties = m_browserTabContent->getChartTwoMatrixDisplayProperties();
        CaretAssert(matrixProperties);
        
        if (matrixProperties->isGridLinesDisplayed()) {
            GraphicsPrimitive* matrixGridPrimitive = matrixChart->getMatrixChartingGraphicsPrimitive(chartViewingType,
                                                                                                           CiftiMappableDataFile::MatrixGridMode::OUTLINE);
            drawPrimitivePrivate(matrixGridPrimitive);
        }
        
        int32_t numberOfRows = 0;
        int32_t numberOfColumns = 0;
        matrixChart->getMatrixDimensions(numberOfRows,
                                         numberOfColumns);
        
        if (matrixProperties->isSelectedRowColumnHighlighted()) {
            uint8_t highlightRGBByte[3];
            m_preferences->getBackgroundAndForegroundColors()->getColorForegroundChartView(highlightRGBByte);
            const float highlightRGBA[4] = {
                highlightRGBByte[0] / 255.0f,
                highlightRGBByte[1] / 255.0f,
                highlightRGBByte[2] / 255.0f,
                1.0f
            };
            
            std::vector<int32_t> selectedColumnIndices;
            std::vector<int32_t> selectedRowIndices;
            ChartTwoMatrixLoadingDimensionEnum::Enum selectedRowColumnDimension;
            matrixChart->getSelectedRowColumnIndices(m_tabIndex,
                                                     selectedRowColumnDimension,
                                                     selectedRowIndices,
                                                     selectedColumnIndices);
            
            /*
             * If true, selected row highlighting will be limited
             * to a upper or lower triangular cells when
             * triangular display is selected
             * Disabled by WB-741
             */
            const bool limitSelectionToTriangularFlag = false;
            const float lineWidthPercentageHeight = 1.0f;
            
            for (auto rowIndex : selectedRowIndices) {
                float minX = 0;
                float maxX = numberOfColumns;
                const float minY = numberOfRows - rowIndex - 1;
                const float maxY = minY + 1.0f;
                
                if (limitSelectionToTriangularFlag) {
                    switch (chartViewingType) {
                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL:
                            break;
                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
                            break;
                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
                            maxX = rowIndex;  /* matrix is symmetric */
                            break;
                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
                            minX = rowIndex + 1;  /* matrix is symmetric */
                            break;
                    }
                }
                
                GraphicsPrimitiveV3fC4f* rowOutlineData = GraphicsPrimitiveV3fC4f::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_MITER_JOIN);
                rowOutlineData->reserveForNumberOfVertices(4);
                rowOutlineData->addVertex(minX, minY, highlightRGBA);
                rowOutlineData->addVertex(maxX, minY, highlightRGBA);
                rowOutlineData->addVertex(maxX, maxY, highlightRGBA);
                rowOutlineData->addVertex(minX, maxY, highlightRGBA);
                rowOutlineData->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                             lineWidthPercentageHeight);
                
                float modelViewMatrix[16];
                glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
                rowColumnHighlightingOut.push_back(new MatrixRowColumnHighight(rowOutlineData,
                                                                               modelViewMatrix));
            }
            
            for (auto columnIndex : selectedColumnIndices) {
                const float minX = columnIndex;
                const float maxX = columnIndex + 1;
                float minY = 0;
                float maxY = numberOfRows;
                
                if (limitSelectionToTriangularFlag) {
                    switch (chartViewingType) {
                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL:
                            break;
                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
                            break;
                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
                            maxY = columnIndex;  /* matrix is symmetric */
                            break;
                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
                            minY = columnIndex + 1;  /* matrix is symmetric */
                            break;
                    }
                }
                
                GraphicsPrimitiveV3fC4f* columnOutlineData = GraphicsPrimitiveV3fC4f::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_MITER_JOIN);
                columnOutlineData->reserveForNumberOfVertices(4);
                columnOutlineData->addVertex(minX, minY, highlightRGBA);
                columnOutlineData->addVertex(maxX, minY, highlightRGBA);
                columnOutlineData->addVertex(maxX, maxY, highlightRGBA);
                columnOutlineData->addVertex(minX, maxY, highlightRGBA);
                columnOutlineData->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                lineWidthPercentageHeight);
                
                float modelViewMatrix[16];
                glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
                rowColumnHighlightingOut.push_back(new MatrixRowColumnHighight(columnOutlineData,
                                                                               modelViewMatrix));
            }
        }
     }
    
    glDisable(GL_BLEND);
    glPopMatrix();
}

/**
 * Save the state of OpenGL.
 * Copied from Qt's qgl.cpp, qt_save_gl_state().
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::saveStateOfOpenGL()
{
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    glShadeModel(GL_FLAT);
    glDisable(GL_LIGHTING);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

/**
 * Restore the state of OpenGL.
 * Copied from Qt's qgl.cpp, qt_restore_gl_state().
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::restoreStateOfOpenGL()
{
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
    glPopClientAttrib();
    
}

/**
 * Convert a percentage of viewport width or height to an OpenGL Line Width.
 * When percentage width or height is zero, zero will be returned.
 * Otherwise the returned value will ALWAYS be at least one.
 *
 *
 * @param percentageWidthOrHeight
 *     The percentage of width or height (ranges 0.0 to 100.0).
 * @param viewportWidthOrHeight
 *     The viewport width or height.
 * @return
 *     Pixels size value.
 */
float
BrainOpenGLChartTwoDrawingFixedPipeline::convertPercentageOfViewportToOpenGLLineWidth(const float percentageWidthOrHeight,
                                                                                      const float viewportWidthOrHeight)
{
    if (percentageWidthOrHeight <= 0.0f) {
        return 0.0f;
    }
    
    float pixelsWidthOrHeight = ((percentageWidthOrHeight / 100.0f) * viewportWidthOrHeight);
    
    if (pixelsWidthOrHeight < s_minimumLineWidthOpenGL) {
        pixelsWidthOrHeight = s_minimumLineWidthOpenGL;
    }
    return pixelsWidthOrHeight;
}

/**
 * Convert a percentage of viewport width or height to pixels.
 * When percentage width or height is zero, zero will be returned.
 * Otherwise the returned value will ALWAYS be at least one.
 *
 * @param percentageWidthOrHeight
 *     The percentage of width or height (ranges 0.0 to 100.0).
 * @param viewportWidthOrHeight
 *     The viewport width or height.
 * @return
 *     Pixels size value.
 */
float
BrainOpenGLChartTwoDrawingFixedPipeline::convertPercentageOfViewportToPixels(const float percentageWidthOrHeight,
                                                                             const float viewportWidthOrHeight)
{
    if (percentageWidthOrHeight <= 0.0f) {
        return 0.0f;
    }
    
    float pixelsWidthOrHeight = ((percentageWidthOrHeight / 100.0f) * viewportWidthOrHeight);
    
    return pixelsWidthOrHeight;
}

/**
 * Draw the chart graphics surrounding box and set the graphics viewport.
 *
 * @param vpX
 *     Viewport X
 * @param vpY
 *     Viewport Y
 * @param vpWidth
 *     Viewport width
 * @param vpHeight
 *     Viewport height
 * @param lineThicknessPercentage
 *     Thickness of lines in percentage of viewport height
 * @param titleHeight
 *     Height of the title
 * @param bottomAxisHeight
 *     Height of bottom axis
 * @param topAxisHeight
 *     Height of top axis
 * @param leftAxisWidth
 *     Width of left axis
 * @param rightAxisWidth
 *     Width of right axis
 * @param drawBoxFlag
 *     Controls drawing of box.
 * @param chartGraphicsDrawingViewportOut
 *     Output containing viewport for drawing chart graphics within
 *     the box/grid that is adjusted for the box's line thickness.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawChartGraphicsBoxAndSetViewport(const float vpX,
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
                                                                            int32_t chartGraphicsDrawingViewportOut[4])
{
    const float lineThicknessPixels = convertPercentageOfViewportToOpenGLLineWidth(lineThicknessPercentage,
                                                                                   vpHeight);
    const float halfGridLineWidth = lineThicknessPixels / 2.0;
    
    glViewport(vpX,
               vpY,
               vpWidth,
               vpHeight);
    
    if (drawBoxFlag) {
        /*
         * For box, set coordinates for the middle of the line
         */
        const float boxLeft   = leftAxisWidth + halfGridLineWidth;
        const float boxRight  = vpWidth - rightAxisWidth - halfGridLineWidth;
        const float boxBottom = bottomAxisHeight + halfGridLineWidth;
        const float boxTop    = vpHeight - topAxisHeight - titleHeight - halfGridLineWidth;
        
        /*
         * We adjust the horizontal lines by half the line width.
         * Otherwise, there will be 'corner gaps' where the horizontal
         * and vertical lines are joined
         */
        std::unique_ptr<GraphicsPrimitiveV3f> boxData
        = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_MITER_JOIN,
                                                                                   m_fixedPipelineDrawing->m_foregroundColorFloat));
        boxData->reserveForNumberOfVertices(4);
        boxData->addVertex(boxLeft,  boxBottom);
        boxData->addVertex(boxRight, boxBottom);
        boxData->addVertex(boxRight, boxTop);
        boxData->addVertex(boxLeft, boxTop);
        boxData->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                              lineThicknessPercentage);
        drawPrimitivePrivate(boxData.get());
    }
    
    /*
     * Region INSIDE the grid's box so that any data plots
     * are WITHIN the box
     */
    const float graphicsLeft   = vpX + leftAxisWidth + lineThicknessPixels;
    const float graphicsRight  = vpX + vpWidth - rightAxisWidth - lineThicknessPixels;
    const float graphicsBottom = vpY + bottomAxisHeight + lineThicknessPixels;
    const float graphicsTop    = vpY + vpHeight - topAxisHeight - titleHeight - lineThicknessPixels;
    
    const int32_t graphicsWidth = graphicsRight - graphicsLeft;
    const int32_t graphicsHeight = graphicsTop  - graphicsBottom;
    chartGraphicsDrawingViewportOut[0] = graphicsLeft;
    chartGraphicsDrawingViewportOut[1] = graphicsBottom;
    chartGraphicsDrawingViewportOut[2] = graphicsWidth;
    chartGraphicsDrawingViewportOut[3] = graphicsHeight;
    
    CaretAssert(graphicsWidth > 0);
    CaretAssert(graphicsHeight > 0);
}

/**
 * Create the graphics viewport.
 *
 * @param vpX
 *     Viewport X
 * @param vpY
 *     Viewport Y
 * @param vpWidth
 *     Viewport width
 * @param vpHeight
 *     Viewport height
 * @param lineThicknessPercentage
 *     Thickness of lines in percentage of viewport height
 * @param titleHeight
 *     Height of the title
 * @param bottomAxisHeight
 *     Height of bottom axis
 * @param topAxisHeight
 *     Height of top axis
 * @param leftAxisWidth
 *     Width of left axis
 * @param rightAxisWidth
 *     Width of right axis
 * @param chartGraphicsDrawingViewportOut
 *     Output containing viewport for drawing chart graphics within
 *     the box/grid that is adjusted for the box's line thickness.
 * @return 
 *     True if axes should be drawn, otherwise false.  When chart
 *     gets too small, axes are not drawn.
 */
bool
BrainOpenGLChartTwoDrawingFixedPipeline::createChartDrawingViewport(const float vpX,
                                const float vpY,
                                const float vpWidth,
                                const float vpHeight,
                                const float lineThicknessPercentage,
                                const float titleHeight,
                                const float bottomAxisHeight,
                                const float topAxisHeight,
                                const float leftAxisWidth,
                                const float rightAxisWidth,
                                int32_t chartGraphicsDrawingViewportOut[4])
{
    const float lineThicknessPixels = convertPercentageOfViewportToOpenGLLineWidth(lineThicknessPercentage,
                                                                                   vpHeight);
    /*
     * Region INSIDE the grid's box so that any data plots
     * are WITHIN the box
     */
    float graphicsLeft   = vpX + leftAxisWidth + lineThicknessPixels;
    float graphicsRight  = vpX + vpWidth - rightAxisWidth - lineThicknessPixels;
    float graphicsBottom = vpY + bottomAxisHeight + lineThicknessPixels;
    float graphicsTop    = vpY + vpHeight - topAxisHeight - titleHeight - lineThicknessPixels;
    
    int32_t graphicsWidth = graphicsRight - graphicsLeft;
    int32_t graphicsHeight = graphicsTop  - graphicsBottom;
    
    bool drawAxesFlag = true;
    
    const int32_t minViewportSize = 20;
    if ((graphicsWidth < minViewportSize)
        || (graphicsHeight < minViewportSize)) {
        graphicsLeft   = vpX;
        graphicsRight  = vpX + vpWidth;
        graphicsBottom = vpY;
        graphicsTop    = vpY + vpHeight;
        
        graphicsWidth = graphicsRight - graphicsLeft;
        graphicsHeight = graphicsTop  - graphicsBottom;
        drawAxesFlag = false;
    }
    
    chartGraphicsDrawingViewportOut[0] = graphicsLeft;
    chartGraphicsDrawingViewportOut[1] = graphicsBottom;
    chartGraphicsDrawingViewportOut[2] = graphicsWidth;
    chartGraphicsDrawingViewportOut[3] = graphicsHeight;
    
    CaretAssert(graphicsWidth > 0);
    CaretAssert(graphicsHeight > 0);
    
    return drawAxesFlag;
}

/**
 * Update the viewport content with charting viewport and matrices.
 *
 * @param viewport
 *    The viewport.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::updateViewportContentForCharting(const int32_t viewport[4])
{
    CaretAssert(viewport[2] > 0);
    CaretAssert(viewport[3] > 0);
    
    GLfloat modelviewArray[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelviewArray);
    GLfloat projectionArray[16];
    glGetFloatv(GL_PROJECTION_MATRIX, projectionArray);
    Matrix4x4 modelviewMatrix;
    modelviewMatrix.setMatrixFromOpenGL(modelviewArray);
    Matrix4x4 projectionMatrix;
    projectionMatrix.setMatrixFromOpenGL(projectionArray);
    m_viewportContent->setChartDataMatricesAndViewport(projectionMatrix,
                                                       modelviewMatrix,
                                                       viewport);
}

/**
 * Draw the graphics primitive.
 *
 * @param primitive
 *     Primitive that will be drawn.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawPrimitivePrivate(GraphicsPrimitive* primitive)
{
    if (primitive == NULL) {
        return;
    }
    
    GraphicsEngineDataOpenGL::draw(primitive);
}

/**
 * Constructor.
 *
 * @param histogramChart
 *    The file's histogram charting
 * @param mapIndex
 *    Index of the map for which histogram is displayed.
 * @param allMapsSelected
 *    True if ALL MAPS selected for histogram, else false.
 */
BrainOpenGLChartTwoDrawingFixedPipeline::HistogramChartDrawingInfo::HistogramChartDrawingInfo(ChartableTwoFileHistogramChart* histogramChart,
                                                                                              int32_t mapIndex,
                                                                                              const bool allMapsSelected)
:
m_histogramChart(histogramChart),
m_mapIndex(mapIndex),
m_allMapsSelected(allMapsSelected)
{
}

/**
 * Destructor.
 */
BrainOpenGLChartTwoDrawingFixedPipeline::HistogramChartDrawingInfo::~HistogramChartDrawingInfo()
{
}

/**
 * Constructor.
 *
 * @param textRenderer
 *     The text renderer.
 * @param tabViewport
 *     Viewport of the tab containing the chart.
 * @param dataMinX
 *     Minimum X-data value.
 * @param dataMaxX
 *     Maximum X-data value.
 * @param dataMinY
 *     Minimum Y-data value.
 * @param dataMaxX
 *     Maximum Y-data value.
 * @param axisLocation
 *     Location of axis as 'axis' may be NULL.
 * @param orientedAxes
 *     The oriented axes parent of the 'axis'
 * @param axis
 *     Axis being setup.
 * @param labelText
 *     Text for the label.
 * @param lineWidthPercentage
 *     The line width percentage.
 */
BrainOpenGLChartTwoDrawingFixedPipeline::AxisDrawingInfo::AxisDrawingInfo(BrainOpenGLTextRenderInterface* textRenderer,
                                                                          const int32_t tabViewport[4],
                                                                          const float dataMinX,
                                                                          const float dataMaxX,
                                                                          const float dataMinY,
                                                                          const float dataMaxY,
                                                                          const ChartAxisLocationEnum::Enum axisLocation,
                                                                          const ChartTwoCartesianOrientedAxes* orientedAxes,
                                                                          const ChartTwoCartesianAxis* axis,
                                                                          const AString& labelText,
                                                                          const float lineWidthPercentage)
: m_axisLocation(axisLocation),
m_orientedAxes(orientedAxes),
m_axis(axis),
m_textRenderer(textRenderer),
m_tabViewportWidth(tabViewport[2]),
m_tabViewportHeight(tabViewport[3])
{
    m_axisValid  = false;
    m_axisWidth  = 0.0f;
    m_axisHeight = 0.0f;
    switch (axisLocation) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            m_axisMinimumValue = dataMinX;
            m_axisMaximumValue = dataMaxX;
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
            m_axisMinimumValue = dataMinY;
            m_axisMaximumValue = dataMaxY;
            break;
    }
    float dataMinimumValue = m_axisMinimumValue;
    float dataMaximumValue = m_axisMaximumValue;
    
    m_axisDisplayedFlag = false;
    if (m_axis != NULL) {
        CaretAssert(m_axis->getAxisLocation() == axisLocation);
        m_axisDisplayedFlag = axis->isDisplayedByUser();
    }
    if ( ! m_axisDisplayedFlag) {
        /*
         * If axis is not displayed, use padding with a small percentage of space.
         */
        const float emptyAxisPaddingPercentage = 1.0f;
        switch (axisLocation) {
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                m_axisHeight = convertPercentageOfViewportToPixels(emptyAxisPaddingPercentage,
                                                                   m_tabViewportHeight);
                m_labelPaddingSizePixels = m_axisHeight;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                m_axisWidth = convertPercentageOfViewportToPixels(emptyAxisPaddingPercentage,
                                                                  m_tabViewportWidth);
                m_labelPaddingSizePixels = m_axisWidth;
                break;
        }
        
        /*
         * This is necessary so that the correct axis minimum and
         * maximum values (m_axisMinimumValue and m_axisMaximumValue).
         */
        if ((m_axis != NULL)
            && (m_orientedAxes != NULL)) {
            std::vector<float> scaleValuePositions;
            std::vector<AString> scaleValuesText;
            m_orientedAxes->getScaleValuesAndOffsets(m_axis,
                                                     dataMinimumValue,
                                                     dataMaximumValue,
                                                     1.0,
                                                     m_axisMinimumValue,
                                                     m_axisMaximumValue,
                                                     scaleValuePositions,
                                                     scaleValuesText);
        }

        return;
    }
    
    if (m_axis == NULL) {
        return;
    }
    m_axisValid = true;
    
    /*
     * Padding values are percentages so convert to pixels.
     */
    const float numericsTicksPaddingPercentage = 1.0f;
    m_labelPaddingSizePixels = 0.0f;
    m_numericsTicksPaddingSizePixels = 0.0f;
    switch (axisLocation) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            m_labelPaddingSizePixels = convertPercentageOfViewportToPixels(axis->getPaddingSize(),
                                                                           m_tabViewportHeight);
            m_numericsTicksPaddingSizePixels = convertPercentageOfViewportToPixels(numericsTicksPaddingPercentage,
                                                                                   m_tabViewportHeight);
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
            m_labelPaddingSizePixels = convertPercentageOfViewportToPixels(axis->getPaddingSize(),
                                                                           m_tabViewportHeight);
            m_numericsTicksPaddingSizePixels = convertPercentageOfViewportToPixels(numericsTicksPaddingPercentage,
                                                                                   m_tabViewportHeight);
            break;
    }
    
    m_lineDrawingWidth = convertPercentageOfViewportToOpenGLLineWidth(lineWidthPercentage,
                                                             m_tabViewportHeight);
    m_tickLength = 0.0f;
    if (m_axis->isShowTickmarks()) {
        const float tickLengthPercentage = 1.0f;
        m_tickLength = convertPercentageOfViewportToOpenGLLineWidth(tickLengthPercentage, m_tabViewportHeight);
    }
    
    /*
     * Width/height of label
     */
    initializeLabel(labelText, m_labelWidth, m_labelHeight);
    
    /*
     * Maximum width/height of numeric values
     */
    float numericsWidth  = 0.0f;
    float numericsHeight = 0.0f;
    initializeNumericText(dataMinimumValue, dataMaximumValue,
                          m_axis->isNumericsTextDisplayed(),
                          m_axis->isNumericsTextRotated(),
                          numericsWidth, numericsHeight);
    
    /*
     * For the bottom and the top, we now have its height but we do not yet know its width
     * and likewise for left and right.
     */
    switch (axisLocation) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            m_axisHeight = std::ceil(m_labelPaddingSizePixels + m_labelHeight + numericsHeight + m_numericsTicksPaddingSizePixels + m_tickLength);
            CaretAssert(m_axisHeight >= 0.0f);
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
            /* Note: label is rotated on left/right so use it's height */
            m_axisWidth = std::ceil(m_labelPaddingSizePixels + m_labelHeight + numericsWidth + m_numericsTicksPaddingSizePixels + m_tickLength);
            CaretAssert(m_axisWidth >= 0.0f);
            break;
    }
}

/**
 * Initialize the numeric label by creating an annotation for the label
 * and determining the width and height of the label.
 *
 * @param labelText
 *     Text of the label.
 * @param widthOut
 *     Output with width of label.
 * @param heightOut
 *     Output with hight of label.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::AxisDrawingInfo::initializeLabel(const AString& labelText,
                                                                          float& widthOut,
                                                                          float& heightOut)
{
    widthOut = 0.0f;
    heightOut = 0.0f;
    
    if (m_axis == NULL) {
        return;
    }
    
    AnnotationPercentSizeText* text = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL,
                                                                    AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_HEIGHT);
    if (m_axis->isShowLabel()) {
        float xyz[3] = { 0.0f, 0.0f, 0.0f };
        switch (m_axisLocation) {
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                xyz[0] = 0.5f;
                text->setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                text->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                xyz[0] = 0.5f;
                text->setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                text->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                xyz[1] = 0.5f;
                text->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
                text->setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                text->setRotationAngle(-90.0f);
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                xyz[1] = 0.5f;
                text->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
                text->setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                text->setRotationAngle(-90.0f);
                break;
        }
        text->setText(labelText);
        text->setFontPercentViewportSize(m_axis->getLabelTextSize());
        text->getCoordinate()->setXYZ(xyz);
        
        double textWidth = 0.0;
        double textHeight = 0.0;
        m_textRenderer->getTextWidthHeightInPixels(*text,
                                                   BrainOpenGLTextRenderInterface::DrawingFlags(),
                                                   m_tabViewportWidth, m_tabViewportHeight,
                                                   textWidth, textHeight);
        widthOut  = textWidth;
        heightOut = textHeight;
    }
    
    m_labelText.reset(text);
}

/**
 * Initialize the numeric text by creating an annotation for each numeric value
 * and find the maximum width and height of the numeric values.
 *
 * @param dataMinimumDataValue
 *     Minimum data value for axis
 * @param dataMaximumDataValue
 *     Maximum data value for axis
 * @param showNumericFlag,
 *     Show numerics
 * @param rotateNumericFlag
 *     Rotate numerics.
 * @param maxWidthOut
 *     Output with maximum width of numeric text.
 * @param maxHeightOut
 *     Output with maximum height of numeric text.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::AxisDrawingInfo::initializeNumericText(const float dataMinimumDataValue,
                                                                                const float dataMaximumDataValue,
                                                                                const bool showNumericFlag,
                                                                                const bool rotateNumericFlag,
                                                                                float& maxWidthOut,
                                                                                float& maxHeightOut)
{
    maxWidthOut = 0.0f;
    maxHeightOut = 0.0f;
    
    if (m_axis == NULL) {
        return;
    }
    
    /*
     * Using an axis length of 1.0 will result in the
     * offsets ranging (0.0 to 1.0)
     */
    const float axisLength = 1.0;
    std::vector<float> scaleValuePositions;
    std::vector<AString> scaleValuesText;
    m_orientedAxes->getScaleValuesAndOffsets(m_axis,
                                             dataMinimumDataValue,
                                             dataMaximumDataValue,
                                             axisLength,
                                             m_axisMinimumValue,
                                             m_axisMaximumValue,
                                             scaleValuePositions,
                                             scaleValuesText);

    /*
     * For each numeric value, create a text annotation and determine the 
     * width and height of the text.  Also, set either the X or Y-coordinate
     * with the normalized coordinate (zero to one) that will be used
     * later to position the numeric value.
     */
    CaretAssert(scaleValuePositions.size() == scaleValuesText.size());
    const int32_t numValues = static_cast<int32_t>(scaleValuesText.size());
    for (int32_t i = 0; i < numValues; i++) {
        AnnotationPercentSizeText* text = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL,
                                                                        AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_HEIGHT);
        text->setFontPercentViewportSize(m_axis->getNumericsTextSize());
        if (rotateNumericFlag) {
            text->setRotationAngle(-90.0f);
        }
        
        float xyz[3] = { 0.0f, 0.0f, 0.0f };
        
        AnnotationTextAlignVerticalEnum::Enum verticalAlignment = AnnotationTextAlignVerticalEnum::MIDDLE;
        AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment = AnnotationTextAlignHorizontalEnum::CENTER;
        switch (m_axisLocation) {
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                xyz[0] = scaleValuePositions[i];
                if (rotateNumericFlag) {
                    horizontalAlignment = AnnotationTextAlignHorizontalEnum::RIGHT;
//                    if (i == firstIndex) {
//                        verticalAlignment = AnnotationTextAlignVerticalEnum::TOP;
//                    }
//                    else if (i == lastIndex) {
//                        verticalAlignment = AnnotationTextAlignVerticalEnum::BOTTOM;
//                    }
//                    else {
//                        verticalAlignment = AnnotationTextAlignVerticalEnum::MIDDLE;
//                    }
                    verticalAlignment = AnnotationTextAlignVerticalEnum::MIDDLE;
                }
                else {
                    verticalAlignment = AnnotationTextAlignVerticalEnum::TOP;
//                    if (i == firstIndex) {
//                        horizontalAlignment = AnnotationTextAlignHorizontalEnum::LEFT;
//                    }
//                    else if (i == lastIndex) {
//                        horizontalAlignment = AnnotationTextAlignHorizontalEnum::RIGHT;
//                    }
//                    else {
//                        horizontalAlignment = AnnotationTextAlignHorizontalEnum::CENTER;
//                    }
                    horizontalAlignment = AnnotationTextAlignHorizontalEnum::CENTER;
                }
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                xyz[0] = scaleValuePositions[i];
                if (rotateNumericFlag) {
                    horizontalAlignment = AnnotationTextAlignHorizontalEnum::LEFT;
//                    if (i == firstIndex) {
//                        verticalAlignment = AnnotationTextAlignVerticalEnum::TOP;
//                    }
//                    else if (i == lastIndex) {
//                        verticalAlignment = AnnotationTextAlignVerticalEnum::BOTTOM;
//                    }
//                    else {
//                        verticalAlignment = AnnotationTextAlignVerticalEnum::MIDDLE;
//                    }
                    verticalAlignment = AnnotationTextAlignVerticalEnum::MIDDLE;
                }
                else {
                    verticalAlignment = AnnotationTextAlignVerticalEnum::BOTTOM;
//                    if (i == firstIndex) {
//                        horizontalAlignment = AnnotationTextAlignHorizontalEnum::LEFT;
//                    }
//                    else if (i == lastIndex) {
//                        horizontalAlignment = AnnotationTextAlignHorizontalEnum::RIGHT;
//                    }
//                    else {
//                        horizontalAlignment = AnnotationTextAlignHorizontalEnum::CENTER;
//                    }
                    horizontalAlignment = AnnotationTextAlignHorizontalEnum::CENTER;
                }
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                xyz[1] = scaleValuePositions[i];
                if (rotateNumericFlag) {
                    verticalAlignment = AnnotationTextAlignVerticalEnum::BOTTOM;
//                    if (i == firstIndex) {
//                        horizontalAlignment = AnnotationTextAlignHorizontalEnum::LEFT;
//                    }
//                    else if (i == lastIndex) {
//                        horizontalAlignment = AnnotationTextAlignHorizontalEnum::RIGHT;
//                    }
//                    else {
//                        horizontalAlignment = AnnotationTextAlignHorizontalEnum::CENTER;
//                    }
                    horizontalAlignment = AnnotationTextAlignHorizontalEnum::CENTER;
                }
                else {
                    horizontalAlignment = AnnotationTextAlignHorizontalEnum::RIGHT;
//                    if (i == firstIndex) {
//                        verticalAlignment = AnnotationTextAlignVerticalEnum::BOTTOM;
//                    }
//                    else if (i == lastIndex) {
//                        verticalAlignment = AnnotationTextAlignVerticalEnum::TOP;
//                    }
//                    else {
//                        verticalAlignment = AnnotationTextAlignVerticalEnum::MIDDLE;
//                    }
                    verticalAlignment = AnnotationTextAlignVerticalEnum::MIDDLE;
                }
                
                
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                xyz[1] = scaleValuePositions[i];
                if (rotateNumericFlag) {
                    verticalAlignment = AnnotationTextAlignVerticalEnum::TOP;
//                    if (i == firstIndex) {
//                        horizontalAlignment = AnnotationTextAlignHorizontalEnum::LEFT;
//                    }
//                    else if (i == lastIndex) {
//                        horizontalAlignment = AnnotationTextAlignHorizontalEnum::RIGHT;
//                    }
//                    else {
//                        horizontalAlignment = AnnotationTextAlignHorizontalEnum::CENTER;
//                    }
                    horizontalAlignment = AnnotationTextAlignHorizontalEnum::CENTER;
                }
                else {
                    horizontalAlignment = AnnotationTextAlignHorizontalEnum::LEFT;
//                    if (i == firstIndex) {
//                        verticalAlignment = AnnotationTextAlignVerticalEnum::BOTTOM;
//                    }
//                    else if (i == lastIndex) {
//                        verticalAlignment = AnnotationTextAlignVerticalEnum::TOP;
//                    }
//                    else {
//                        verticalAlignment = AnnotationTextAlignVerticalEnum::MIDDLE;
//                    }
                    verticalAlignment = AnnotationTextAlignVerticalEnum::MIDDLE;
                }
                
                break;
        }
        text->setHorizontalAlignment(horizontalAlignment);
        text->setVerticalAlignment(verticalAlignment);

        
        if (showNumericFlag) {
            text->setText(scaleValuesText[i]);
        }
        
        text->getCoordinate()->setXYZ(xyz);
        
        double textWidth = 0.0;
        double textHeight = 0.0;
        m_textRenderer->getTextWidthHeightInPixels(*text,
                                                   BrainOpenGLTextRenderInterface::DrawingFlags(),
                                                   m_tabViewportWidth, m_tabViewportHeight,
                                                   textWidth, textHeight);
        if (rotateNumericFlag) {
            std::swap(textWidth,
                      textHeight);
        }
        
        maxWidthOut = std::max(maxWidthOut, static_cast<float>(textWidth));
        maxHeightOut = std::max(maxHeightOut, static_cast<float>(textHeight));
        
        m_numericsText.push_back(std::unique_ptr<AnnotationPercentSizeText>(text));
    }
}

/**
 * Set the axis viewport.  
 * 
 * In the constructor, we were able to determine the width of the left and right
 * axes and the height of the bottom and top axes.  Now we can determine
 * the size of the other dimension and computer a viewport for the axis.
 *
 * @param titleHeight
 *    Height of the title.
 * @param bottomAxisHeight
 *    Height of the bottom axis.
 * @param topAxisHeight
 *    Height of the top axis.
 * @param leftAxisWidth
 *    Width of the left axis.
 * @param rightAxisWidth
 *    Width of the right axis.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::AxisDrawingInfo::setAxisViewport(const float titleHeight,
                                                                          const float bottomAxisHeight,
                                                                          const float topAxisHeight,
                                                                          const float leftAxisWidth,
                                                                          const float rightAxisWidth)
{
    switch (m_axisLocation) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            m_axisWidth = m_tabViewportWidth - (leftAxisWidth + rightAxisWidth);
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
            m_axisHeight = m_tabViewportHeight - (bottomAxisHeight + topAxisHeight + titleHeight);
            break;
    }
    if (m_axisWidth < 1.0f) {
        m_axisWidth = 1.0f;
    }
    if (m_axisHeight < 1.0f) {
        m_axisHeight = 1.0f;
    }
    
    switch (m_axisLocation) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
            m_axisViewport[0] = leftAxisWidth;
            m_axisViewport[1] = 0.0;
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            m_axisViewport[0] = leftAxisWidth;
            m_axisViewport[1] = m_tabViewportHeight - topAxisHeight - titleHeight;
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
            m_axisViewport[0] = 0.0f;
            m_axisViewport[1] = bottomAxisHeight;
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
            m_axisViewport[0] = m_tabViewportWidth - rightAxisWidth;
            m_axisViewport[1] = bottomAxisHeight;
            break;
    }
    
    m_axisViewport[2] = m_axisWidth;
    m_axisViewport[3] = m_axisHeight;
    
    CaretAssert(m_axisViewport[0] >= 0.0f);
    CaretAssert(m_axisViewport[1] >= 0.0f);
    CaretAssert(m_axisViewport[2] >= 0.0f);
    CaretAssert(m_axisViewport[3] >= 0.0f);
}

/**
 * When this method is called, we have the viewport for the axis and we can now use the 
 * viewport to position the numerical value and the label.
 *
 * @param foregroundFloatRGBA
 *     Color of the foreground.
 * @param graphicsBoxLineThickness
 *     Line thickness of the graphics box.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::AxisDrawingInfo::setLabelAndNumericsCoordinates(const float foregroundFloatRGBA[4],
                                                                                         const float graphicsBoxLineThickness)
{
    if (m_axis == NULL) {
        return;
    }
    
    if ( ! m_axisValid) {
        return;
    }
    
    if ( ! m_axisDisplayedFlag) {
        return;
    }
    
    /*
     * Adjust the 'viewport' to account for the thickness of the lines
     * for the box that contains the plot of data
     */
    float vpX = m_axisViewport[0];
    float vpY = m_axisViewport[1];
    float vpWidth = m_axisViewport[2];
    float vpHeight = m_axisViewport[3];
    switch (m_axisLocation) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            vpX     += graphicsBoxLineThickness;
            vpWidth -= (graphicsBoxLineThickness * 2.0);
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
            vpY      += graphicsBoxLineThickness;
            vpHeight -= (graphicsBoxLineThickness * 2.0);
            break;
    }
    
    if (debugFlag) {
        glColor3f(1.0, 0.0, 0.0);
        const float v1[3] = { vpX, vpY, 0.0f };
        const float v2[3] = { vpX + vpWidth, vpY, 0.0f };
        const float v3[3] = { vpX + vpWidth, vpY + vpHeight, 0.0f };
        const float v4[3] = { vpX, vpY + vpHeight, 0.0f };
        const uint8_t rgba[4] { 255, 0, 0, 255};
        GraphicsShape::drawBoxOutlineByteColor(v1, v2, v3, v4,
                                               rgba,
                                               GraphicsPrimitive::LineWidthType::PIXELS, 1.0f);
    }
    
    /*
     * 'Center' the label in the viewport
     */
    if (m_axis->isShowLabel()) {
        float xyz[3] = { 0.0, 0.0, 0.0 };
        switch (m_axisLocation) {
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                xyz[0] = vpX + (vpWidth / 2.0f);
                xyz[1] = vpY + m_labelPaddingSizePixels;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                xyz[0] = vpX + m_labelPaddingSizePixels;
                xyz[1] = vpY + (vpHeight / 2.0f);
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                xyz[0] = vpX + vpWidth -  m_labelPaddingSizePixels;
                xyz[1] = vpY + (vpHeight / 2.0f);
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                xyz[0] = vpX + (vpWidth / 2.0f);
                xyz[1] = vpY + vpHeight - m_labelPaddingSizePixels;
                break;
        }
        m_labelText->setTextColor(CaretColorEnum::CUSTOM);
        m_labelText->setCustomTextColor(foregroundFloatRGBA);
        m_labelText->getCoordinate()->setXYZ(xyz);
    }
    
    /*
     * Now convert the 'normalized' (zero to one) numeric value coordinate
     * to real coordinate.
     */
    for (auto& text : m_numericsText) {
        float xyz[3] = { 0.0, 0.0, 0.0 };
        text->getCoordinate()->getXYZ(xyz);
        
        switch (m_axisLocation) {
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                xyz[0] = vpX + (vpWidth * xyz[0]);
                xyz[1] = vpY + vpHeight - m_tickLength - m_numericsTicksPaddingSizePixels;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                xyz[0] = vpX + vpWidth - m_tickLength - m_numericsTicksPaddingSizePixels;
                xyz[1] = vpY + (vpHeight * xyz[1]);
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                xyz[0] = vpX + m_tickLength + m_numericsTicksPaddingSizePixels;
                xyz[1] = vpY + (vpHeight * xyz[1]);
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                xyz[0] = vpX + (vpWidth * xyz[0]);
                xyz[1] = vpY + m_tickLength + m_numericsTicksPaddingSizePixels;
                break;
        }
        
        text->setTextColor(CaretColorEnum::CUSTOM);
        text->setCustomTextColor(foregroundFloatRGBA);
        text->getCoordinate()->setXYZ(xyz);
    }
}

/**
 * Draw the axis.
 *
 * @param chartDrawing
 *     The chart drawing parent.
 * @param chartTwoOverlaySet
 *     Chart overlay set containing the axis.
 * @param mouseX
 *     X-coordinate of the mouse.
 * @param mouseY
 *     Y-coordinate of the mouse.
 * @param foregroundFloatRGBA
 *     Color of the foreground.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::AxisDrawingInfo::drawAxis(BrainOpenGLChartTwoDrawingFixedPipeline* chartDrawing,
                                                                   ChartTwoOverlaySet* chartTwoOverlaySet,
                                                                   const int32_t mouseX,
                                                                   const int32_t mouseY,
                                                                   const float foregroundFloatRGBA[4])
{
    if (m_axis == NULL) {
        return;
    }
    if ( ! m_axisValid) {
        return;
    }
    
    if ( ! m_axisDisplayedFlag) {
        return;
    }
    
    const int32_t numScaleValuesToDraw = static_cast<int32_t>(m_numericsText.size());
    
    if (numScaleValuesToDraw > 0) {
        const bool showTicksEnabledFlag = m_axis->isShowTickmarks();
        
        GraphicsPrimitive::PrimitiveType primitiveType = GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES;
        std::unique_ptr<GraphicsPrimitiveV3f> ticksData
        = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(primitiveType,
                                                                                   foregroundFloatRGBA));
        ticksData->reserveForNumberOfVertices(numScaleValuesToDraw * 2);
        
        const int32_t firstTickIndex = 0;
        const int32_t lastTickIndex = numScaleValuesToDraw - 1;
        
        for (int32_t i = 0; i < numScaleValuesToDraw; i++) {
            /*
             * Coordinate of numeric value text
             */
            CaretAssertVectorIndex(m_numericsText, i);
            AnnotationPercentSizeText* text = m_numericsText[i].get();
            float xyz[3];
            text->getCoordinate()->getXYZ(xyz);
            float textX = xyz[0];
            float textY = xyz[1];
            
            /*
             * Tick starts at an offset from its corresponding numerical value.
             * The length of the tick is increased by half the line width so
             * that the tick connects to the box around the data plot without
             * a gap.
             */
            float tickStartX = textX;
            float tickStartY = textY;
            float tickEndX   = 0.0;
            float tickEndY   = 0.0;
            const float tickLength = m_tickLength + (m_lineDrawingWidth / 2.0f);
            switch (m_axisLocation) {
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                    tickStartY += m_numericsTicksPaddingSizePixels;
                    tickEndX   = tickStartX;
                    tickEndY   = tickStartY + tickLength;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                    tickStartY -= m_numericsTicksPaddingSizePixels;
                    tickEndX   = tickStartX;
                    tickEndY   = tickStartY - tickLength;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                    tickStartX += m_numericsTicksPaddingSizePixels;
                    tickEndX   = tickStartX + tickLength;
                    tickEndY   = tickStartY;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                    tickStartX -= m_numericsTicksPaddingSizePixels;
                    tickEndX   = tickStartX - tickLength;
                    tickEndY   = tickStartY;
                    break;
            }
            
            const float viewportX      = m_axisViewport[0];
            const float viewportY      = m_axisViewport[1];
            const float viewportWidth  = m_axisViewport[2];
            const float viewportHeight = m_axisViewport[3];

            double textWidth(0.0);
            double textHeight(0.0);
            m_textRenderer->getTextWidthHeightInPixels(*text,
                                                       BrainOpenGLTextRenderInterface::DrawingFlags(),
                                                       m_tabViewportWidth, m_tabViewportHeight,
                                                       textWidth, textHeight);
            if (showTicksEnabledFlag) {
                bool showTicksFlag(true);
                const bool hideFirstAndLastTicksFlag(false);
                if (hideFirstAndLastTicksFlag) {
                    if ((i == firstTickIndex)
                        || (i == lastTickIndex)) {
                    }
                }
                if (showTicksFlag) {
                    ticksData->addVertex(tickStartX, tickStartY);
                    ticksData->addVertex(tickEndX,   tickEndY);
                }
            }
            
            switch (m_axisLocation) {
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                {
                    /*
                     * Text alignment is center;
                     */
                    const float viewportLeft(viewportX);
                    const float viewportRight(viewportX + viewportWidth);
                    const float halfTextSize(m_axis->isNumericsTextRotated()
                                             ? (textHeight / 2.0)
                                             : (textWidth / 2.0));
//                    const float halfTextWidth(textWidth / 2.0);
                    const float textRight(textX + halfTextSize);
                    if (textRight > viewportRight) {
                        textX = viewportRight - halfTextSize;
                    }
                    
                    const float textLeft = (textX - halfTextSize);
                    if (textLeft < viewportLeft) {
                        textX = viewportLeft + halfTextSize;
                    }
                }
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                {
                    /*
                     * Text alignment is middle;
                     */
                    const float viewportBottom(viewportY);
                    const float viewportTop(viewportY + viewportHeight);
                    const float halfTextSize(m_axis->isNumericsTextRotated()
                                             ? (textWidth / 2.0)
                                             : (textHeight / 2.0));
                    const float textTop(textY + halfTextSize);
                    if (textTop > viewportTop) {
                        textY = viewportTop - halfTextSize;
                    }
                    
                    const float textBottom = (textY - halfTextSize);
                    if (textBottom < viewportBottom) {
                        textY = viewportBottom + halfTextSize;
                    }
                }
                    break;
            }
            
            /*
             * Draw the numeric value.
             */
            m_textRenderer->drawTextAtViewportCoords(textX,
                                                     textY,
                                                     0.0,
                                                     *text,
                                                     BrainOpenGLTextRenderInterface::DrawingFlags());
        }
        
        /*
         * Draw the ticks.
         * Note Line width is set in pixel and was converted from PERCENTAGE OF VIEWPORT HEIGHT
         */
        if (ticksData->getNumberOfVertices() > 1) {
            ticksData->setLineWidth(GraphicsPrimitive::LineWidthType::PIXELS, m_lineDrawingWidth);
            chartDrawing->drawPrimitivePrivate(ticksData.get());
        }
    }
    
    /*
     * Draw the label
     */
    if (m_axis->isShowLabel()) {
        float xyz[3];
        m_labelText->getCoordinate()->getXYZ(xyz);
        if (chartDrawing->m_identificationModeFlag) {
            /*
             * For identification simply draw a box using a triangle strip
             */
            int32_t primitiveIndex = -1;
            float   primitiveDepth = 0.0;
            
            float bottomLeft[3], bottomRight[3], topLeft[3], topRight[3];
            m_textRenderer->getBoundsForTextAtViewportCoords(*(m_labelText.get()),
                                                             BrainOpenGLTextRenderInterface::DrawingFlags(),
                                                             xyz[0], xyz[1], xyz[2],
                                                             m_tabViewportWidth, m_tabViewportHeight,
                                                             bottomLeft, bottomRight, topRight, topLeft);
            
            std::unique_ptr<GraphicsPrimitiveV3f> boxPrimitive = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                                                                                                                          foregroundFloatRGBA));
            boxPrimitive->reserveForNumberOfVertices(4);
            boxPrimitive->addVertex(topLeft);
            boxPrimitive->addVertex(bottomLeft);
            boxPrimitive->addVertex(topRight);
            boxPrimitive->addVertex(bottomRight);
            
            GraphicsEngineDataOpenGL::drawWithSelection(boxPrimitive.get(),
                                                        mouseX,
                                                        mouseY,
                                                        primitiveIndex,
                                                        primitiveDepth);
            
            if (primitiveIndex >= 0) {
                if (chartDrawing->m_selectionItemChartLabel->isOtherScreenDepthCloserToViewer(primitiveDepth)) {
                    chartDrawing->m_selectionItemChartLabel->setChartTwoCartesianAxis(const_cast<ChartTwoCartesianAxis*>(m_axis),
                                                                                      chartTwoOverlaySet);
                }
            }
        }
        else {
            m_textRenderer->drawTextAtViewportCoords(xyz[0],
                                                     xyz[1],
                                                     0.0,
                                                     *(m_labelText.get()),
                                                     BrainOpenGLTextRenderInterface::DrawingFlags());
        }
    }
}

/**
 * Constructor.
 *
 * @param textRenderer
 *     The text renderer.
 * @param tabViewport
 *     Viewport of the tab containing the chart.
 * @param title
 *     The chart title.
 */
BrainOpenGLChartTwoDrawingFixedPipeline::TitleDrawingInfo::TitleDrawingInfo(BrainOpenGLTextRenderInterface* textRenderer,
                                                                            const int32_t tabViewport[4],
                                                                            const ChartTwoTitle* title)
: m_textRenderer(textRenderer),
m_tabViewportWidth(tabViewport[2]),
m_tabViewportHeight(tabViewport[3]),
m_title(title)
{
    const AString titleText = m_title->getText().trimmed();
    m_titleDisplayedFlag = (m_title->isDisplayed()
                            && ( ! titleText.isEmpty()));
    
    if ( ! m_titleDisplayedFlag) {
        /*
         * If axis is not displayed, use padding with a small percentage of space.
         */
        const float emptyAxisPaddingPercentage = 1.0f;
        m_paddingSizePixels = convertPercentageOfViewportToPixels(emptyAxisPaddingPercentage,
                                                                  m_tabViewportHeight);
        m_titleHeight = m_paddingSizePixels;
        return;
    }
    
    m_paddingSizePixels = convertPercentageOfViewportToPixels(m_title->getPaddingSize(),
                                                              m_tabViewportHeight);
    
    /*
     * Create a text annotation for drawing the title
     */
    AnnotationPercentSizeText* text = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL,
                                                                    AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_HEIGHT);
    float xyz[3] = { 0.0f, 0.0f, 0.0f };
    xyz[0] = 0.5f;
    text->setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
    text->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
    text->setText(titleText);
    text->setFontPercentViewportSize(m_title->getTextSize());
    text->getCoordinate()->setXYZ(xyz);
    
    double m_textWidth = 0.0;
    double m_textHeight = 0.0;
    m_textRenderer->getTextWidthHeightInPixels(*text,
                                               BrainOpenGLTextRenderInterface::DrawingFlags(),
                                               m_tabViewportWidth, m_tabViewportHeight,
                                               m_textWidth, m_textHeight);
    m_text.reset(text);
    
    m_titleHeight = m_paddingSizePixels + m_textHeight;
    CaretAssert(m_titleHeight >= 0.0f);
}

/**
 * Set the title viewport.  Once the width of the left and right and the height of the
 * bottom and top axes are known, we can determine the sizes for the title.
 *
 * @param leftAxisWidth
 *    Width of the left axis.
 * @param rightAxisWidth
 *    Width of the right axis.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::TitleDrawingInfo::setTitleViewport(const float leftAxisWidth,
                                                                            const float rightAxisWidth)
{
    if ( ! m_titleDisplayedFlag) {
        return;
    }
    
    m_titleWidth = m_tabViewportWidth - (leftAxisWidth + rightAxisWidth);
    if (m_titleWidth < 1.0f) {
        m_titleWidth = 1.0f;
    }
    if (m_titleHeight < 1.0f) {
        m_titleHeight = 1.0f;
    }
    
    m_titleWidth = m_tabViewportWidth - (leftAxisWidth + rightAxisWidth);
    m_titleViewport[0] = leftAxisWidth;
    m_titleViewport[1] = m_tabViewportHeight - m_titleHeight;
    
    m_titleViewport[2] = m_titleWidth;
    m_titleViewport[3] = m_titleHeight;
    
    CaretAssert(m_titleViewport[0] >= 0.0f);
    CaretAssert(m_titleViewport[1] >= 0.0f);
    CaretAssert(m_titleViewport[2] >= 0.0f);
    CaretAssert(m_titleViewport[3] >= 0.0f);
    
    float xyz[3];
    m_text->getCoordinate()->getXYZ(xyz);
    xyz[0] = m_titleViewport[0] + (m_titleWidth / 2.0f);
    xyz[1] = m_titleViewport[1] + m_titleHeight - m_paddingSizePixels;
    m_text->getCoordinate()->setXYZ(xyz);
}

/**
 * Draw the title.
 *
 * @param foregroundFloatRGBA
 *     Color of the foreground.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::TitleDrawingInfo::drawTitle(const float foregroundFloatRGBA[4])
{
    if (m_title == NULL) {
        return;
    }
    if ( ! m_titleDisplayedFlag) {
        return;
    }
    
    if (debugFlag) {
        const float v1[3] = { m_titleViewport[0], m_titleViewport[1], 0.0f };
        const float v2[3] = { m_titleViewport[0] + m_titleViewport[2], m_titleViewport[1], 0.0f };
        const float v3[3] = { m_titleViewport[0] + m_titleViewport[2], m_titleViewport[1] + m_titleViewport[3], 0.0f };
        const float v4[3] = { m_titleViewport[0], m_titleViewport[1] + m_titleViewport[3], 0.0f };
        const uint8_t rgba[4] { 255, 0, 0, 255};
        GraphicsShape::drawBoxOutlineByteColor(v1, v2, v3, v4,
                                               rgba,
                                               GraphicsPrimitive::LineWidthType::PIXELS, 2.0f);
    }
    
    m_text->setTextColor(CaretColorEnum::CUSTOM);
    m_text->setCustomTextColor(foregroundFloatRGBA);
    
    float xyz[3];
    m_text->getCoordinate()->getXYZ(xyz);
    m_textRenderer->drawTextAtViewportCoords(xyz[0],
                                             xyz[1],
                                             0.0,
                                             *(m_text.get()),
                                             BrainOpenGLTextRenderInterface::DrawingFlags());
}
