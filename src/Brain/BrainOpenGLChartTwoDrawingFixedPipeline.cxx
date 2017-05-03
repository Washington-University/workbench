
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

#include "AnnotationColorBar.h"
#include "AnnotationPointSizeText.h"
#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoMatrixDisplayProperties.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoOverlaySet.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "ChartableTwoFileMatrixChart.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "FastStatistics.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3f.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "HistogramDrawingInfo.h"
#include "IdentificationWithColor.h"
#include "MathFunctions.h"
#include "ModelChartTwo.h"
#include "NodeAndVoxelColoring.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
#include "SessionManager.h"
#include "SelectionItemChartTwoHistogram.h"
#include "SelectionItemChartTwoLineSeries.h"
#include "SelectionItemChartTwoMatrix.h"
#include "SelectionManager.h"

using namespace caret;

/**
 * \class caret::BrainOpenGLChartTwoDrawingFixedPipeline 
 * \brief Drawing of version two charts.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLChartTwoDrawingFixedPipeline::BrainOpenGLChartTwoDrawingFixedPipeline()
: BrainOpenGLChartTwoDrawingInterface()
{
    m_preferences = SessionManager::get()->getCaretPreferences();
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
 * @param browserTabContent
 *     Content of the browser tab.
 * @param chartTwoModel
 *     The chart two model.
 * @param fixedPipelineDrawing
 *     The fixed pipeline OpenGL drawing.
 * @param selectionItemDataType
 *     Selected data type.
 * @param viewport
 *     Viewport for the chart.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawChartOverlaySet(Brain* brain,
                                                             BrowserTabContent* browserTabContent,
                                                             ModelChartTwo* chartTwoModel,
                                                             BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                             const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                                             const int32_t viewport[4])
{
    CaretAssert(brain);
    CaretAssert(browserTabContent);
    CaretAssert(chartTwoModel);
    CaretAssert(fixedPipelineDrawing);
    m_brain = brain;
    m_chartTwoModel = chartTwoModel;
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    m_textRenderer = fixedPipelineDrawing->getTextRenderer();
    m_translation[0] = 0.0;
    m_translation[1] = 0.0;
    m_translation[2] = 0.0;
    browserTabContent->getTranslation(m_translation);
    m_tabIndex = browserTabContent->getTabNumber();
    m_zooming        = browserTabContent->getScaling();
    m_chartOverlaySet = m_chartTwoModel->getChartTwoOverlaySet(m_tabIndex);
    m_selectionItemDataType = selectionItemDataType;
    m_viewport[0] = viewport[0];
    m_viewport[1] = viewport[1];
    m_viewport[2] = viewport[2];
    m_viewport[3] = viewport[3];

    m_selectionItemHistogram  = m_brain->getSelectionManager()->getChartTwoHistogramIdentification();
    m_selectionItemLineSeries = m_brain->getSelectionManager()->getChartTwoLineSeriesIdentification();
    m_selectionItemMatrix     = m_brain->getSelectionManager()->getChartTwoMatrixIdentification();

    m_fixedPipelineDrawing->disableLighting();
    
    /*
     * Find color bars for this tab and decrease height of 
     * viewport so chart is above color bars.
     */
    std::vector<AnnotationColorBar*> colorBars;
    browserTabContent->getAnnotationColorBars(colorBars);
    if ( ! colorBars.empty()) {
        int heightOfAllColorBars = 0;
        
        for (auto cb : colorBars) {
            bool useItFlag = false;
            switch (cb->getCoordinateSpace()) {
                case AnnotationCoordinateSpaceEnum::PIXELS:
                    break;
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    break;
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                    useItFlag = true;
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
    bool drawMatrixFlag     = true;
    
    /*
     * Check for a 'selection' type mode
     */
    m_identificationModeFlag = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if ( ! m_selectionItemHistogram->isEnabledForSelection()) {
                drawHistogramFlag = false;
            }
            if ( ! m_selectionItemLineSeries->isEnabledForSelection()) {
                drawLineSeriesFlag = false;
            }
            if ( ! m_selectionItemMatrix->isEnabledForSelection()) {
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
    
    resetIdentification();
    
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
                                drawHistogramChart();
                            }
                            break;
                        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                            if (drawLineSeriesFlag) {
                                
                            }
                            break;
                        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                            if (drawMatrixFlag) {
                                drawMatrixChart();
                            }
                            break;
                    }
                }
        }
    }
    
    restoreStateOfOpenGL();
}

/**
 * Draw histogram charts.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawHistogramChart()
{
    const int32_t vpX      = m_viewport[0];
    const int32_t vpY      = m_viewport[1];
    const int32_t vpWidth  = m_viewport[2];
    const int32_t vpHeight = m_viewport[3];
    
    int32_t chartGraphicsDrawingViewport[4] = {
        vpX,
        vpY,
        vpWidth,
        vpHeight
    };
    
    const int32_t numberOfOverlays = m_chartOverlaySet->getNumberOfDisplayedOverlays();
    CaretAssert(numberOfOverlays > 0);
    const ChartTwoOverlay* topOverlay = m_chartOverlaySet->getOverlay(0);
    const ChartTwoCompoundDataType cdt = topOverlay->getChartTwoCompoundDataType();
    CaretAssert(cdt.getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM);
    
    std::vector<std::unique_ptr<HistogramChartDrawingInfo>> drawingInfo;
    
    /*
     * Get the histogram drawing information and overall extent
     */
    float xMin = std::numeric_limits<float>::max();
    float xMax = -std::numeric_limits<float>::max();
    float yMinLeft  = std::numeric_limits<float>::max();
    float yMaxLeft  = -std::numeric_limits<float>::max();
    float yMinRight = std::numeric_limits<float>::max();
    float yMaxRight = -std::numeric_limits<float>::max();
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
        
        const ChartableTwoFileDelegate* chartDelegate        = mapFile->getChartingDelegate();
        const ChartableTwoFileHistogramChart* histogramChart = chartDelegate->getHistogramCharting();
        
        if (histogramChart->isValid()) {
            CaretAssert(selectedIndexType == ChartTwoOverlay::SelectedIndexType::MAP);
            AString errorMessage;
            HistogramDrawingInfo* histogramDrawingInfo = new HistogramDrawingInfo();
            if (mapFile->getMapHistogramDrawingInfo(selectedIndex,
                                                    chartOverlay->isAllMapsSelected(),
                                                    false,
                                                    *histogramDrawingInfo,
                                                    errorMessage)) {
                drawingInfo.push_back(std::unique_ptr<HistogramChartDrawingInfo>(new HistogramChartDrawingInfo(histogramDrawingInfo,
                                                                                                               histogramChart,
                                                                                                               selectedIndex,
                                                                                                               chartOverlay->getCartesianVerticalAxisLocation())));
                
                float bounds[4];
                if (histogramDrawingInfo->getBounds(bounds)) {
                    xMin = std::min(xMin, bounds[0]);
                    xMax = std::max(xMax, bounds[1]);
                    
                    switch (chartOverlay->getCartesianVerticalAxisLocation()) {
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                            CaretAssertMessage(0, "TOP axis not allowed for vertical axis");
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                            yMinRight = std::min(yMinRight, bounds[2]);
                            yMaxRight = std::max(yMaxRight, bounds[3]);
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                            yMinLeft = std::min(yMinLeft, bounds[2]);
                            yMaxLeft = std::max(yMaxLeft, bounds[3]);
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                            CaretAssertMessage(0, "BOTTOM axis not allowed for vertical axis");
                            break;
                    }
                }
            }
            else {
                delete histogramDrawingInfo;
                CaretLogWarning(errorMessage + mapFile->getFileName());
            }
        }
    }
    
    /*
     * Bounds valid?
     */
    if ((xMin < xMax)
        && ((yMinLeft < yMaxLeft) || (yMinRight < yMaxRight))) {
        /*
         * Margin is region around the chart in which
         * the axes legends, values, and ticks are drawn.
         */
        const double marginSize = 10;
        Margins margins(marginSize);
        
        ChartTwoCartesianAxis* leftAxis   = NULL;
        ChartTwoCartesianAxis* rightAxis  = NULL;
        ChartTwoCartesianAxis* bottomAxis = NULL;
        ChartTwoCartesianAxis* topAxis    = NULL;
        
        std::vector<ChartTwoCartesianAxis*> displayedAxes = m_chartOverlaySet->getDisplayedChartAxes();
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
        
        const float boundsLeftBottomTop[4] = { xMin, xMax, yMinLeft, yMaxLeft };
        const float boundsRight[4] = { xMin, xMax, yMinRight, yMaxRight };
        double width = 0.0, height = 0.0;
        
        estimateCartesianChartAxisLegendsWidthHeight(boundsLeftBottomTop, vpHeight, leftAxis, width, height);
        margins.m_left = std::max(margins.m_left, width);
        estimateCartesianChartAxisLegendsWidthHeight(boundsRight, vpHeight, rightAxis, width, height);
        margins.m_right = std::max(margins.m_right, width);
        estimateCartesianChartAxisLegendsWidthHeight(boundsLeftBottomTop, vpHeight, topAxis, width, height);
        margins.m_top = std::max(margins.m_top, height);
        estimateCartesianChartAxisLegendsWidthHeight(boundsLeftBottomTop, vpHeight, bottomAxis, width, height);
        margins.m_bottom = std::max(margins.m_bottom, height);
        
        if (margins.m_left > marginSize) margins.m_left += 10;
        if (margins.m_right > marginSize) margins.m_right += 10;
        
        /*
         * Ensure that there is sufficient space for the axes data display.
         */
        if ((vpWidth > (marginSize * 3))
            && (vpHeight > (marginSize * 3))) {
            
            float axisMinimumValue = 0.0;
            float axisMaximumValue = 0.0;
            
            /* Draw legends and grids */
            if (drawChartAxisCartesian(boundsLeftBottomTop,
                                       vpX,
                                       vpY,
                                       vpWidth,
                                       vpHeight,
                                       margins,
                                       leftAxis,
                                       axisMinimumValue,
                                       axisMaximumValue)) {
                yMinLeft = axisMinimumValue;
                yMaxLeft = axisMaximumValue;
            }
            
            if (drawChartAxisCartesian(boundsRight,
                                       vpX,
                                       vpY,
                                       vpWidth,
                                       vpHeight,
                                       margins,
                                       rightAxis,
                                       axisMinimumValue,
                                       axisMaximumValue)) {
                yMinRight = axisMinimumValue;
                yMaxRight = axisMaximumValue;
            }
            if (drawChartAxisCartesian(boundsLeftBottomTop,
                                       vpX,
                                       vpY,
                                       vpWidth,
                                       vpHeight,
                                       margins,
                                       bottomAxis,
                                       axisMinimumValue,
                                       axisMaximumValue)) {
                xMin = axisMinimumValue;
                xMax = axisMaximumValue;
            }
            
            drawChartAxisCartesian(boundsLeftBottomTop,
                                   vpX,
                                   vpY,
                                   vpWidth,
                                   vpHeight,
                                   margins,
                                   topAxis,
                                   axisMinimumValue,
                                   axisMaximumValue);
            
            drawChartGraphicsBoxAndSetViewport(vpX,
                                               vpY,
                                               vpWidth,
                                               vpHeight,
                                               margins,
                                               chartGraphicsDrawingViewport);
        }
        
        glViewport(chartGraphicsDrawingViewport[0],
                   chartGraphicsDrawingViewport[1],
                   chartGraphicsDrawingViewport[2],
                   chartGraphicsDrawingViewport[3]);
        
        
        /*
         * Draw the bars for all histogram and then draw the envelopes
         * so the envelopes are not obscured by the bars
         */
        for (int32_t drawModeInt = 0; drawModeInt < 2; drawModeInt++) {
            for (auto& drawInfo : drawingInfo) {
                if (drawInfo->m_histogramDrawingInfo->isValid()) {
                    const CaretMappableDataFile* cmdf = drawInfo->m_histogramChart->getCaretMappableDataFile();
                    CaretAssert(cmdf);
                    const PaletteColorMapping* paletteColorMapping = cmdf->getMapPaletteColorMapping(drawInfo->m_mapIndex);
                    bool drawBarsFlag = false;
                    bool drawEnvelopeFlag = false;
                    if (drawModeInt == 0) {
                        drawBarsFlag = paletteColorMapping->isHistogramBarsVisible();
                    }
                    else if (drawModeInt == 1) {
                        drawEnvelopeFlag = paletteColorMapping->isHistogramEnvelopeVisible();
                    }
                    
                    if (drawBarsFlag
                        || drawEnvelopeFlag) {
                        bool leftVerticalAxisFlag = true;
                        switch (drawInfo->m_verticalAxisLocation) {
                            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                                break;
                            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                                break;
                            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                                leftVerticalAxisFlag = false;
                                break;
                            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                                break;
                        }
                        
                        glMatrixMode(GL_PROJECTION);
                        glLoadIdentity();
                        if (leftVerticalAxisFlag) {
                            glOrtho(xMin, xMax,
                                    yMinLeft, yMaxLeft,
                                    -10.0, 10.0);
                        }
                        else {
                            glOrtho(xMin, xMax,
                                    yMinRight, yMaxRight,
                                    -10.0, 10.0);
                        }
                        
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
                        
                        const CaretColorEnum::Enum envelopeColor = paletteColorMapping->getHistogramEnvelopeColor();
                        drawHistogramChartContent(drawInfo.get(),
                                                  envelopeColor,
                                                  drawBarsFlag,
                                                  drawEnvelopeFlag);
                    }
                }
            }
        }
    }
}

/*
 * Draw the given histogram chart.
 *
 * @param drawingInfo
 *    Histogram drawing information.
 * @param envelopeColor
 *    Color for drawing envelope.
 * @param drawHistogramBarsFlag
 *    Draw the histogram as bars.
 * @param drawHistogramEnvelopeFlag
 *    Draw the histogram as an envelope.
 *
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawHistogramChartContent(const HistogramChartDrawingInfo* drawingInfo,
                                                                   const CaretColorEnum::Enum envelopeColor,
                                                                   const bool drawHistogramBarsFlag,
                                                                   const bool drawHistogramEnvelopeFlag)
{
    float envelopeSolidColorRGBA[4];
    CaretColorEnum::toRGBAFloat(envelopeColor,
                                envelopeSolidColorRGBA);
    
    const ChartableTwoFileHistogramChart* histogramChart = drawingInfo->m_histogramChart;
    const int32_t mapIndex = drawingInfo->m_mapIndex;
    const HistogramDrawingInfo& histogramDrawingInfo = *drawingInfo->m_histogramDrawingInfo;
    
    if ( ! histogramDrawingInfo.isValid()) {
        return;
    }
    float bounds[4];
    if ( ! histogramDrawingInfo.getBounds(bounds)) {
        return;
    }
    if (m_identificationModeFlag) {
        resetIdentification();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    const std::vector<float>& xValues = histogramDrawingInfo.getDataX();
    const std::vector<float>& yValues = histogramDrawingInfo.getDataY();
    const std::vector<float>& rgbaValues = histogramDrawingInfo.getDataRGBA();
    CaretAssert(xValues.size() == yValues.size());
    CaretAssert((xValues.size() * 4) == rgbaValues.size());
    
    const int32_t numValues = static_cast<int32_t>(xValues.size() - 1);
    if (numValues > 1) {
        {
            uint8_t thresholdRGBByte[3];
            m_preferences->getBackgroundAndForegroundColors()->getColorChartHistogramThreshold(thresholdRGBByte);
            const float histogramRGBA[4] = {
                thresholdRGBByte[0] / 255.0,
                thresholdRGBByte[1] / 255.0,
                thresholdRGBByte[2] / 255.0,
                1.0
            };
            std::vector<float> histogramNormals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1 };
            CaretAssert(histogramNormals.size() == 12);
            
            const std::vector<float>& histogramOneXYZ = histogramDrawingInfo.getThresholdOneBounds();
            if (histogramOneXYZ.size() == 12) {
                std::unique_ptr<GraphicsPrimitiveV3f> threshPrim
                = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGON, histogramRGBA));
                threshPrim->addVertices(&histogramOneXYZ[0],
                                        4);
                drawPrimitivePrivate(threshPrim.get());
            }
            
            const std::vector<float>& histogramTwoXYZ = histogramDrawingInfo.getThresholdTwoBounds();
            if (histogramTwoXYZ.size() == 12) {
                std::unique_ptr<GraphicsPrimitiveV3f> threshPrim
                = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGON, histogramRGBA));
                threshPrim->addVertices(&histogramTwoXYZ[0],
                                        4);
                drawPrimitivePrivate(threshPrim.get());
            }
        }
        
        if (drawHistogramBarsFlag) {
            /*
             * Reserve to prevent reszing of vectors
             * as elements are added.
             */
            const int32_t verticesPerBar = 4;
            const int32_t totalVertices  = verticesPerBar * numValues;
            
            std::unique_ptr<GraphicsPrimitiveV3fC4f> histogramFloatRGBA
            = std::unique_ptr<GraphicsPrimitiveV3fC4f>(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::QUADS));
            std::unique_ptr<GraphicsPrimitiveV3fC4ub> histogramByteRGBA
            = std::unique_ptr<GraphicsPrimitiveV3fC4ub>(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::QUADS));
            
            
            if(m_identificationModeFlag) {
                histogramByteRGBA->reserveForNumberOfVertices(totalVertices);
            }
            else {
                histogramFloatRGBA->reserveForNumberOfVertices(totalVertices);
            }
            
            const float z = 0.;
            
            for (int32_t i = 0; i < numValues; i++) {
                CaretAssertVectorIndex(rgbaValues, i*4 + 3);
                const float* rgba = &rgbaValues[i * 4];
                if (rgba[3] > 0.0) {
                    CaretAssertVectorIndex(xValues, i + 1);
                    float left   = xValues[i];
                    float right  = xValues[i+1];
                    float bottom = 0;
                    CaretAssertVectorIndex(yValues, i);
                    float top = yValues[i];
                    
                    if (m_identificationModeFlag) {
                        uint8_t idRGBA[4];
                        addToHistogramIdentification(mapIndex, i, idRGBA);
                        histogramByteRGBA->addVertex(left, bottom, z, idRGBA);
                        histogramByteRGBA->addVertex(right, bottom, z, idRGBA);
                        histogramByteRGBA->addVertex(right, top, z, idRGBA);
                        histogramByteRGBA->addVertex(left, top, z, idRGBA);
                    }
                    else {
                        histogramFloatRGBA->addVertex(left, bottom, z, rgba);
                        histogramFloatRGBA->addVertex(right, bottom, z, rgba);
                        histogramFloatRGBA->addVertex(right, top, z, rgba);
                        histogramFloatRGBA->addVertex(left, top, z, rgba);
                    }
                }
            }
            
            /*
             * Draw the bar elements.
             */
            if (m_identificationModeFlag) {
                drawPrimitivePrivate(histogramByteRGBA.get());
            }
            else {
                drawPrimitivePrivate(histogramFloatRGBA.get());
            }
        }
        
        if (drawHistogramEnvelopeFlag) {
            std::unique_ptr<GraphicsPrimitiveV3fC4f> histogramFloatRGBA
            = std::unique_ptr<GraphicsPrimitiveV3fC4f>(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::LINE_STRIP));
            std::unique_ptr<GraphicsPrimitiveV3fC4ub> histogramByteRGBA
            = std::unique_ptr<GraphicsPrimitiveV3fC4ub>(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::LINE_STRIP));
            
            /*
             * Reserve to prevent reszing of vectors
             * as elements are added.
             */
            const int32_t verticesPerBar = 2;
            const int32_t totalVertices  = verticesPerBar * numValues + 4;
            if (m_identificationModeFlag) {
                histogramByteRGBA->reserveForNumberOfVertices(totalVertices);
            }
            else {
                histogramFloatRGBA->reserveForNumberOfVertices(totalVertices);
            }
            
            const float z = 0.0;
            
            const int32_t lastValueIndex = numValues - 1;
            for (int32_t i = 0; i < numValues; i++) {
                CaretAssertVectorIndex(xValues, i + 1);
                float left   = xValues[i];
                float right  = xValues[i + 1];
                
                CaretAssert(right >= left);
                float bottom = 0;
                
                CaretAssertVectorIndex(yValues, i + 1);
                float topLeft = yValues[i];
                float topRight = yValues[i+1];
                
                float leftRGBA[4];
                float rightRGBA[4];
                float middleRGBA[4];
                if (envelopeColor == CaretColorEnum::CUSTOM) {
                    CaretAssertVectorIndex(rgbaValues, (i + 1) * 4 + 3);
                    const float* leftColorPtr = &rgbaValues[i * 4];
                    const float* rightColorPtr = &rgbaValues[i * 4];
                    for (int32_t m = 0; m < 4; m++) {
                        leftRGBA[m] = leftColorPtr[m];
                        rightRGBA[m] = rightColorPtr[m];
                        middleRGBA[m] = (leftColorPtr[m] + rightColorPtr[m]) / 2.0;
                    };
                }
                else {
                    for (int32_t m = 0; m < 4; m++) {
                        leftRGBA[m] = envelopeSolidColorRGBA[m];
                        rightRGBA[m] = envelopeSolidColorRGBA[m];
                        middleRGBA[m] = envelopeSolidColorRGBA[m];
                    };
                }
                uint8_t idRGBA[4];
                if (m_identificationModeFlag) {
                    addToHistogramIdentification(mapIndex, i, idRGBA);
                }
                if (i == 0) {
                    if (m_identificationModeFlag) {
                        histogramByteRGBA->addVertex(left, bottom, z, idRGBA);
                    }
                    else {
                        histogramFloatRGBA->addVertex(left, bottom, z, leftRGBA);
                    }
                    
                    if (m_identificationModeFlag) {
                        histogramByteRGBA->addVertex(left, topLeft, z, idRGBA);
                    }
                    else {
                        histogramFloatRGBA->addVertex(left, topLeft, z, leftRGBA);
                    }
                }
                
                if (m_identificationModeFlag) {
                    histogramByteRGBA->addVertex((left+right)/2.0, topLeft, z, idRGBA);
                }
                else {
                    histogramFloatRGBA->addVertex((left+right)/2.0, topLeft, z, middleRGBA);
                }
                
                if (i == lastValueIndex) {
                    if (m_identificationModeFlag) {
                        histogramByteRGBA->addVertex(right, topRight, z, idRGBA);
                    }
                    else {
                        histogramFloatRGBA->addVertex(right, topRight, z, rightRGBA);
                    }

                    if (m_identificationModeFlag) {
                        histogramByteRGBA->addVertex(right, bottom, z, idRGBA);
                    }
                    else {
                        histogramFloatRGBA->addVertex(right, bottom, z, rightRGBA);
                    }
                }
            }
            
            /*
             * Draw the elements.
             */
            if (m_identificationModeFlag) {
                glLineWidth(5.0);
                drawPrimitivePrivate(histogramByteRGBA.get());
            }
            else {
                glLineWidth(2.0);
                drawPrimitivePrivate(histogramFloatRGBA.get());
            }
        }
    }
    
    if (m_identificationModeFlag) {
        processHistogramIdentification(histogramChart);
    }
}

/**
 * Draw a matrix chart.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawMatrixChart()
{
    glViewport(m_viewport[0],
               m_viewport[1],
               m_viewport[2],
               m_viewport[3]);
    
    bool applyTransformationsFlag = true;
    float cellWidth = 1.0;
    float cellHeight = 1.0;
    
    /*
     * Setup width/height of area in which matrix is drawn with a
     * small margin along all of the edges
     */
    float margin = 10.0;
    if ((m_viewport[2] < (margin * 3.0))
        || (m_viewport[3] < (margin * 3.0))) {
        margin = 0.0;
    }
    const float graphicsWidth  = m_viewport[2] - (margin * 2.0);
    const float graphicsHeight = m_viewport[3] - (margin * 2.0);

    /*
     * First overlay is ALWAYS ON and since all matrices must have 
     * same number of rows/columns, use first matrix for rows/columns
     */
    const int32_t numberOfOverlays = m_chartOverlaySet->getNumberOfDisplayedOverlays();
    CaretAssert(numberOfOverlays > 0);
    const ChartTwoOverlay* topOverlay = m_chartOverlaySet->getOverlay(0);
    const ChartTwoCompoundDataType cdt = topOverlay->getChartTwoCompoundDataType();
    CaretAssert(cdt.getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX);
    const int32_t numberOfRows = cdt.getMatrixNumberOfRows();
    const int32_t numberOfCols = cdt.getMatrixNumberOfColumns();
    if ((numberOfRows > 0)
        && (numberOfCols > 0)) {
        cellWidth  = graphicsWidth  / numberOfCols;
        cellHeight = graphicsHeight / numberOfRows;
    }
    else {
        return;
    }
    
    /*
     * Set the width and neight of each matrix cell.
     */
    const ChartTwoMatrixDisplayProperties* matrixProperties = m_chartTwoModel->getChartTwoMatrixDisplayProperties(m_tabIndex);
    CaretAssert(matrixProperties);
    const float cellWidthZoom  = matrixProperties->getCellPercentageZoomWidth()  / 100.0;
    const float cellHeightZoom = matrixProperties->getCellPercentageZoomHeight() / 100.0;
    if ((cellWidthZoom > 0.0)
        && (cellHeightZoom > 0.0)) {
        cellWidth  *= cellWidthZoom;
        cellHeight *= cellHeightZoom;
    }
    
    /*
     * Set the coordinates for the area in which the matrix is drawn.
     */
    const float xMin = -margin;
    const float xMax = graphicsWidth + margin;
    const float yMin = -margin;
    const float yMax = graphicsHeight + margin;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xMin, xMax,
            yMin, yMax,
            -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    if (applyTransformationsFlag) {
        glTranslatef(m_translation[0],
                     m_translation[1],
                     0.0);
        
        const float chartWidth  = cellWidth  * numberOfCols;
        const float chartHeight = cellHeight * numberOfRows;
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
        
        const ChartableTwoFileMatrixChart* matrixChart = mapFile->getChartingDelegate()->getMatrixCharting();
        int32_t overlayRows = 0;
        int32_t overlayColumns = 0;
        matrixChart->getMatrixDimensions(overlayRows,
                                         overlayColumns);
        
        /*
         * All matrices must have same rows/columns
         */
        if ((overlayRows == numberOfRows)
            && (overlayColumns == numberOfCols)) {
            drawMatrixChartContent(matrixChart,
                                   chartOverlay->getMatrixTriangularViewingMode(),
                                cellWidth,
                                cellHeight,
                                m_zooming);
        }
    }
}

/*
 * Draw a matrix chart.
 *
 * @param matrixChart
 *     Matrix chart that is drawn.
 * @param chartViewingType
 *     Type of chart viewing.
 * @param cellWidth
 *     Width of cell.
 * @param cellHeight
 *     Height of cell.
 * @param zooming
 *     Current zooming.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawMatrixChartContent(const ChartableTwoFileMatrixChart* matrixChart,
                                                             const ChartTwoMatrixTriangularViewingModeEnum::Enum chartViewingType,
                                                             const float cellWidth,
                                                             const float cellHeight,
                                                             const float zooming)
{
    GraphicsPrimitiveV3fC4f* matrixPrimitive = matrixChart->getMatrixChartingGraphicsPrimitive(chartViewingType);
    if (matrixPrimitive == NULL) {
        return;
    }
    
    if (m_identificationModeFlag) {
        resetIdentification();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    glPushMatrix();
    glScalef(cellWidth, cellHeight, 1.0);
    /*
     * Enable alpha blending so voxels that are not drawn from higher layers
     * allow voxels from lower layers to be seen.
     */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if (m_identificationModeFlag) {
        int32_t primitiveIndex = -1;
        float   primitiveDepth = 0.0;
        GraphicsEngineDataOpenGL::drawWithSelection(m_fixedPipelineDrawing->getOpenGLContextPointer(),
                                                    matrixPrimitive,
                                                    m_fixedPipelineDrawing->mouseX,
                                                    m_fixedPipelineDrawing->mouseY,
                                                    primitiveIndex,
                                                    primitiveDepth);
        //std::cout << "Matrix selection: " << primitiveIndex << std::endl;
        if (primitiveIndex >= 0) {
            int32_t numberOfRows = 0;
            int32_t numberOfColumns = 0;
            matrixChart->getMatrixDimensions(numberOfRows,
                                             numberOfColumns);
            
            const int32_t rowIndex = primitiveIndex / numberOfColumns;
            const int32_t colIndex = primitiveIndex % numberOfColumns;
            //std::cout << "   row=" << rowIndex << " col=" << colIndex << std::endl;
            
            if (m_selectionItemMatrix->isOtherScreenDepthCloserToViewer(primitiveDepth)) {
                m_selectionItemMatrix->setMatrixChart(const_cast<ChartableTwoFileMatrixChart*>(matrixChart),
                                                      rowIndex,
                                                      colIndex);
            }
        }
    }
    else {
        drawPrimitivePrivate(matrixPrimitive);
        
        const ChartTwoMatrixDisplayProperties* matrixProperties = m_chartTwoModel->getChartTwoMatrixDisplayProperties(m_tabIndex);
        CaretAssert(matrixProperties);
        
        if (matrixProperties->isGridLinesDisplayed()) {
//            uint8_t gridLineColorBytes[4];
//            m_preferences->getBackgroundAndForegroundColors()->getColorChartMatrixGridLines(gridLineColorBytes);
//            float gridLineColorFloats[4];
//            CaretPreferences::byteRgbToFloatRgb(gridLineColorBytes,
//                                                gridLineColorFloats);
//            gridLineColorFloats[3] = 1.0;
//            GraphicsEngineDataOpenGL::drawWithOverrideColor(m_fixedPipelineDrawing->getOpenGLContextPointer(),
//                                                            matrixPrimitive,
//                                                            gridLineColorFloats);
            
            glPolygonMode(GL_FRONT,
                          GL_LINE);
            
            GraphicsEngineDataOpenGL::drawWithAlternativeColor(m_fixedPipelineDrawing->getOpenGLContextPointer(),
                                                               matrixPrimitive,
                                                               matrixChart->getMatrixChartGraphicsPrimitiveGridColorIdentifier());
            glPolygonMode(GL_FRONT,
                          GL_FILL);
        }
        
        int32_t numberOfRows = 0;
        int32_t numberOfColumns = 0;
        matrixChart->getMatrixDimensions(numberOfRows,
                                         numberOfColumns);
        
        if (matrixProperties->isSelectedRowColumnHighlighted()) {
            uint8_t highlightRGBByte[3];
            m_preferences->getBackgroundAndForegroundColors()->getColorForegroundChartView(highlightRGBByte);
            const float highlightRGBA[4] = {
                highlightRGBByte[0] / 255.0,
                highlightRGBByte[1] / 255.0,
                highlightRGBByte[2] / 255.0,
                1.0
            };
            
            std::vector<int32_t> selectedColumnIndices;
            std::vector<int32_t> selectedRowIndices;
            ChartTwoMatrixLoadingDimensionEnum::Enum selectedRowColumnDimension;
            matrixChart->getSelectedRowColumnIndices(m_tabIndex,
                                                     selectedRowColumnDimension,
                                                     selectedRowIndices,
                                                     selectedColumnIndices);
            for (auto rowIndex : selectedRowIndices) {
                std::unique_ptr<GraphicsPrimitiveV3f> rowOutlineData4f
                = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::LINE_LOOP,
                                                                                           highlightRGBA));
                rowOutlineData4f->reserveForNumberOfVertices(4);
                
                float minX = 0;
                float maxX = numberOfColumns;
                const float minY = numberOfRows - rowIndex - 1;
                const float maxY = minY + 1.0f;
                
                switch (chartViewingType) {
                    case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL:
                        break;
                    case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
                        break;
                    case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
                        maxX = rowIndex;  // matrix is symmetric
                        break;
                    case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
                        minX = rowIndex + 1;  // matrix is symmetric
                        break;
                }
                
                rowOutlineData4f->addVertex(minX, minY);
                rowOutlineData4f->addVertex(maxX, minY);
                rowOutlineData4f->addVertex(maxX, maxY);
                rowOutlineData4f->addVertex(minX, maxY);
                const float highlightLineWidth = std::max(((zooming) * 0.20), 3.0);
                glLineWidth(highlightLineWidth);
                drawPrimitivePrivate(rowOutlineData4f.get());
            }
            
            for (auto columnIndex : selectedColumnIndices) {
                std::unique_ptr<GraphicsPrimitiveV3f> columnOutlineData4f
                = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::LINE_LOOP,
                                                                                           highlightRGBA));
                columnOutlineData4f->reserveForNumberOfVertices(4);
                
                const float minX = columnIndex;
                const float maxX = columnIndex + 1;
                float minY = 0;
                float maxY = numberOfRows;
                
                switch (chartViewingType) {
                    case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL:
                        break;
                    case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
                        break;
                    case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
                        maxY = columnIndex;  // matrix is symmetric
                        break;
                    case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
                        minY = columnIndex + 1;  // matrix is symmetric
                        break;
                }
                columnOutlineData4f->addVertex(minX, minY);
                columnOutlineData4f->addVertex(maxX, minY);
                columnOutlineData4f->addVertex(maxX, maxY);
                columnOutlineData4f->addVertex(minX, maxY);
                const float highlightLineWidth = std::max(((zooming) * 0.20), 3.0);
                glLineWidth(highlightLineWidth);
                drawPrimitivePrivate(columnOutlineData4f.get());
            }
        }
    }
    
//    if (m_identificationModeFlag) {
//        int32_t numberOfRows = 0;
//        int32_t numberOfColumns = 0;
//        matrixChart->getMatrixDimensions(numberOfRows,
//                                         numberOfColumns);
//
//        /*
//         * While this works, it uses OpenGL functions (gluUnproject) that
//         * are removed in in the programmable pipeline OpenGL
//         */
//        GLint viewport[4];
//        glGetIntegerv(GL_VIEWPORT,
//                      viewport);
//        GLdouble projectionMatrix[16];
//        glGetDoublev(GL_PROJECTION_MATRIX,
//                     projectionMatrix);
//        GLdouble modelMatrix[16];
//        glGetDoublev(GL_MODELVIEW_MATRIX,
//                     modelMatrix);
//        GLdouble windowXYZ[3] = { m_fixedPipelineDrawing->mouseX, m_fixedPipelineDrawing->mouseY, 0.0 };
//        
//        GLdouble matrixXYZ[3];
//        if (gluUnProject(windowXYZ[0], windowXYZ[1], windowXYZ[2],
//                         modelMatrix, projectionMatrix, viewport,
//                         &matrixXYZ[0], &matrixXYZ[1], &matrixXYZ[2])) {
//            const int32_t rowOriginBottomIndex = static_cast<int32_t>(matrixXYZ[1]);
//            const int32_t columnIndex = static_cast<int32_t>(matrixXYZ[0]);
//            if ((rowOriginBottomIndex >= 0)
//                && (rowOriginBottomIndex < numberOfRows)
//                && (columnIndex >= 0)
//                && (columnIndex < numberOfColumns)) {
//                const int32_t rowIndex = numberOfRows - rowOriginBottomIndex - 1;
//                
//                float pixelRGBA[4];
//                float pixelDepth = 0.0;
//                if (m_fixedPipelineDrawing->getPixelDepthAndRGBA(m_fixedPipelineDrawing->mouseX,
//                                                                 m_fixedPipelineDrawing->mouseY,
//                                                                 pixelDepth,
//                                                                 pixelRGBA)) {
//                    
//                    std::cout << "Pixel Depth/RGBA: " << pixelDepth << ", " << AString::fromNumbers(pixelRGBA, 4, ",") << std::endl;
//                    if (m_selectionItemMatrix->isOtherScreenDepthCloserToViewer(pixelDepth)) {
//                        if (pixelRGBA[3] > 0.0) {
//                            m_selectionItemMatrix->setMatrixChart(const_cast<ChartableTwoFileMatrixChart*>(matrixChart),
//                                                                  rowIndex,
//                                                                  columnIndex);
//                        }
//                    }
//                }
//            }
//        }
//    }
    
    glDisable(GL_BLEND);
    glPopMatrix();
    
    
    const bool drawOLD = false;
    if (drawOLD) {
        int32_t numberOfRows = 0;
        int32_t numberOfColumns = 0;
        std::vector<float> matrixRGBA;
        if ( ! matrixChart->getMatrixDataRGBA(numberOfRows,
                                              numberOfColumns,
                                              matrixRGBA)) {
            CaretLogWarning("Matrix RGBA invalid");
            return;
        }
        if ((numberOfRows <= 0)
            || (numberOfColumns <= 0)) {
            return;
        }
        
        const ChartTwoMatrixDisplayProperties* matrixProperties = m_chartTwoModel->getChartTwoMatrixDisplayProperties(m_tabIndex);
        CaretAssert(matrixProperties);
        
        bool displayGridLinesFlag = matrixProperties->isGridLinesDisplayed();
        const bool highlightSelectedRowColumnFlag = matrixProperties->isSelectedRowColumnHighlighted();
        
        
        std::vector<int32_t> selectedColumnIndices;
        std::vector<int32_t> selectedRowIndices;
        ChartTwoMatrixLoadingDimensionEnum::Enum selectedRowColumnDimension;
        matrixChart->getSelectedRowColumnIndices(m_tabIndex,
                                                 selectedRowColumnDimension,
                                                 selectedRowIndices,
                                                 selectedColumnIndices);
        std::map<int32_t, RowColumnMinMax> columnRangesForRow;
        for (int32_t rowIndex : selectedRowIndices) {
            columnRangesForRow[rowIndex] = RowColumnMinMax();
        }
        const bool haveSelectedRowsFlag = ( ! columnRangesForRow.empty());
        
        std::map<int32_t, RowColumnMinMax> rowRangesForColumn;
        for (int32_t columnIndex : selectedColumnIndices) {
            rowRangesForColumn[columnIndex] = RowColumnMinMax();
        }
        const bool haveSelectedColumnsFlag = ( ! rowRangesForColumn.empty());
        
        uint8_t highlightRGBByte[3];
        m_preferences->getBackgroundAndForegroundColors()->getColorForegroundChartView(highlightRGBByte);
        const float highlightRGBA[4] = {
            highlightRGBByte[0] / 255.0,
            highlightRGBByte[1] / 255.0,
            highlightRGBByte[2] / 255.0,
            1.0
        };
        
        int32_t rgbaOffset = 0;
        
        std::unique_ptr<GraphicsPrimitiveV3fC4f> quadsData4f
        = std::unique_ptr<GraphicsPrimitiveV3fC4f>(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::QUADS));
        std::unique_ptr<GraphicsPrimitiveV3fC4ub> quadsData4ub
        = std::unique_ptr<GraphicsPrimitiveV3fC4ub>(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::QUADS));
        const int32_t numberOfCells = numberOfColumns * numberOfRows;
        if (m_identificationModeFlag) {
            quadsData4ub->reserveForNumberOfVertices(numberOfCells);
        }
        else {
            quadsData4f->reserveForNumberOfVertices(numberOfCells);
        }
        
        float cellY = (numberOfRows - 1) * cellHeight;
        for (int32_t rowIndex = 0; rowIndex < numberOfRows; rowIndex++) {
            float cellX = 0;
            for (int32_t columnIndex = 0; columnIndex < numberOfColumns; columnIndex++) {
                CaretAssertVectorIndex(matrixRGBA, rgbaOffset+3);
                const float* rgba = &matrixRGBA[rgbaOffset];
                rgbaOffset += 4;
                
                bool drawCellFlag = true;
                if (chartViewingType != ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL) {
                    if (numberOfRows == numberOfColumns) {
                        drawCellFlag = false;
                        switch (chartViewingType) {
                            case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL:
                                break;
                            case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
                                if (rowIndex != columnIndex) {
                                    drawCellFlag = true;
                                }
                                break;
                            case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
                                if (rowIndex > columnIndex) {
                                    drawCellFlag = true;
                                }
                                break;
                            case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
                                if (rowIndex < columnIndex) {
                                    drawCellFlag = true;
                                }
                                break;
                        }
                    }
                    else {
                        drawCellFlag = false;
                        const float slope = static_cast<float>(numberOfRows) / static_cast<float>(numberOfColumns);
                        const int32_t diagonalRow = static_cast<int32_t>(slope * columnIndex);
                        
                        switch (chartViewingType) {
                            case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL:
                                drawCellFlag = true;
                                break;
                            case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
                                if (rowIndex != diagonalRow) {
                                    drawCellFlag = true;
                                }
                                break;
                            case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
                                if (rowIndex > diagonalRow) {
                                    drawCellFlag = true;
                                }
                                break;
                            case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
                                if (rowIndex < diagonalRow) {
                                    drawCellFlag = true;
                                }
                                break;
                        }
                    }
                }
                
                if (drawCellFlag) {
                    if (haveSelectedRowsFlag) {
                        std::map<int32_t, RowColumnMinMax>::iterator minMaxIter = columnRangesForRow.find(rowIndex);
                        if (minMaxIter != columnRangesForRow.end()) {
                            if (columnIndex < minMaxIter->second.m_min) {
                                minMaxIter->second.m_min = columnIndex;
                            }
                            if (columnIndex > minMaxIter->second.m_max) {
                                minMaxIter->second.m_max = columnIndex;
                            }
                        }
                    }
                    
                    if (haveSelectedColumnsFlag) {
                        std::map<int32_t, RowColumnMinMax>::iterator minMaxIter = rowRangesForColumn.find(columnIndex);
                        if (minMaxIter != rowRangesForColumn.end()) {
                            if (rowIndex < minMaxIter->second.m_min) {
                                minMaxIter->second.m_min = rowIndex;
                            }
                            if (rowIndex > minMaxIter->second.m_max) {
                                minMaxIter->second.m_max = rowIndex;
                            }
                        }
                    }
                    
                    uint8_t idRGBA[4];
                    if (m_identificationModeFlag) {
                        addToChartMatrixIdentification(rowIndex,
                                                       columnIndex,
                                                       idRGBA);
                    }
                    
                    if (m_identificationModeFlag) {
                        quadsData4ub->addVertex(cellX, cellY, 0.0, idRGBA);
                        quadsData4ub->addVertex(cellX + cellWidth, cellY, 0.0, idRGBA);
                        quadsData4ub->addVertex(cellX + cellWidth, cellY + cellHeight, 0.0, idRGBA);
                        quadsData4ub->addVertex(cellX, cellY + cellHeight, 0.0, idRGBA);
                    }
                    else {
                        quadsData4f->addVertex(cellX, cellY, 0.0, rgba);
                        quadsData4f->addVertex(cellX + cellWidth, cellY, 0.0, rgba);
                        quadsData4f->addVertex(cellX + cellWidth, cellY + cellHeight, 0.0, rgba);
                        quadsData4f->addVertex(cellX, cellY + cellHeight, 0.0, rgba);
                    }
                }
                
                cellX += cellWidth;
            }
            
            cellY -= cellHeight;
        }
        
        /*
         * Draw the matrix elements.
         */
        if (m_identificationModeFlag) {
            drawPrimitivePrivate(quadsData4ub.get());
        }
        else {
            /*
             * Enable alpha blending so voxels that are not drawn from higher layers
             * allow voxels from lower layers to be seen.
             */
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            drawPrimitivePrivate(quadsData4f.get());
            
            glDisable(GL_BLEND);
            
            
            /*
             * Drawn an outline around the matrix elements.
             */
            if (displayGridLinesFlag) {
                uint8_t gridLineColorBytes[4];
                m_preferences->getBackgroundAndForegroundColors()->getColorChartMatrixGridLines(gridLineColorBytes);
                float gridLineColorFloats[4];
                CaretPreferences::byteRgbToFloatRgb(gridLineColorBytes,
                                                    gridLineColorFloats);
                gridLineColorFloats[3] = 1.0;
                
                const std::vector<float>& gridXYZ = quadsData4f->getFloatXYZ();
                const int32_t numCells = static_cast<int32_t>(gridXYZ.size() / 3);
                
                std::unique_ptr<GraphicsPrimitiveV3f> outlineData4f
                = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::QUADS,
                                                                                           gridLineColorFloats));
                outlineData4f->reserveForNumberOfVertices(numCells);
                for (int32_t i = 0; i < numCells; i++) {
                    CaretAssertVectorIndex(gridXYZ, i*3);
                    outlineData4f->addVertex(&gridXYZ[i*3]);
                }
                glPolygonMode(GL_FRONT, GL_LINE);
                glLineWidth(1.0);
                drawPrimitivePrivate(outlineData4f.get());
                glPolygonMode(GL_FRONT, GL_FILL);
            }
            
            if ( (! selectedRowIndices.empty())
                && highlightSelectedRowColumnFlag) {
                for (auto rowIndex : selectedRowIndices) {
                    const float rowY = (numberOfRows - rowIndex - 1) * cellHeight;
                    
                    int32_t minColumn = 0;
                    int32_t maxColumn = numberOfColumns;
                    if (haveSelectedRowsFlag) {
                        std::map<int32_t, RowColumnMinMax>::iterator minMaxIter = columnRangesForRow.find(rowIndex);
                        if (minMaxIter != columnRangesForRow.end()) {
                            const int32_t minValue = minMaxIter->second.m_min;
                            const int32_t rowMaxColumn = minMaxIter->second.m_max;
                            if (minValue < rowMaxColumn) {
                                minColumn = minValue;
                                maxColumn = rowMaxColumn + 1;
                            }
                        }
                    }
                    
                    const float minX = minColumn * cellWidth;
                    const float maxX = maxColumn * cellWidth;
                    
                    std::unique_ptr<GraphicsPrimitiveV3f> rowOutlineData4f
                    = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::LINE_LOOP,
                                                                                               highlightRGBA));
                    rowOutlineData4f->reserveForNumberOfVertices(4);
                    
                    rowOutlineData4f->addVertex(minX, rowY);
                    rowOutlineData4f->addVertex(maxX, rowY);
                    rowOutlineData4f->addVertex(maxX, rowY + cellHeight, 0.0);
                    rowOutlineData4f->addVertex(minX, rowY + cellHeight, 0.0);
                    const float highlightLineWidth = std::max(((cellHeight * zooming) * 0.20), 3.0);
                    glLineWidth(highlightLineWidth);
                    drawPrimitivePrivate(rowOutlineData4f.get());
                }
                
                glLineWidth(1.0);
            }
            
            if ( (! selectedColumnIndices.empty())
                && highlightSelectedRowColumnFlag) {
                for (auto columnIndex : selectedColumnIndices) {
                    const float colX = columnIndex * cellWidth;
                    
                    int32_t minRow = 0;
                    int32_t maxRow = numberOfRows;
                    if (haveSelectedColumnsFlag) {
                        std::map<int32_t, RowColumnMinMax>::iterator minMaxIter = rowRangesForColumn.find(columnIndex);
                        if (minMaxIter != rowRangesForColumn.end()) {
                            const int32_t minValue = minMaxIter->second.m_min;
                            const int32_t maxValue = minMaxIter->second.m_max;
                            if (minValue < maxValue) {
                                minRow = minValue;
                                maxRow = maxValue + 1;
                            }
                        }
                    }
                    
                    const float minY = minRow * cellHeight;
                    const float maxY = maxRow * cellHeight;
                    
                    std::unique_ptr<GraphicsPrimitiveV3f> columnOutlineData4f
                    = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::LINE_LOOP,
                                                                                               highlightRGBA));
                    columnOutlineData4f->reserveForNumberOfVertices(4);
                    
                    columnOutlineData4f->addVertex(colX, minY);
                    columnOutlineData4f->addVertex(colX + cellWidth, minY);
                    columnOutlineData4f->addVertex(colX + cellWidth, maxY);
                    columnOutlineData4f->addVertex(colX, maxY);
                    const float highlightLineWidth = std::max(((cellHeight * zooming) * 0.20), 3.0);
                    glLineWidth(highlightLineWidth);
                    drawPrimitivePrivate(columnOutlineData4f.get());
                }
                glLineWidth(1.0);
            }
            
            glPolygonMode(GL_FRONT, GL_FILL);
        }
        
        if (m_identificationModeFlag) {
            processMatrixIdentification(matrixChart);
        }
    }
}

///*
// * Draw a matrix chart.
// *
// * @param matrixChart
// *     Matrix chart that is drawn.
// * @param chartViewingType
// *     Type of chart viewing.
// * @param cellWidth
// *     Width of cell.
// * @param cellHeight
// *     Height of cell.
// * @param zooming
// *     Current zooming.
// */
//void
//BrainOpenGLChartTwoDrawingFixedPipeline::OLDdrawMatrixChartContent(const ChartableTwoFileMatrixChart* matrixChart,
//                                                                const ChartTwoMatrixTriangularViewingModeEnum::Enum chartViewingType,
//                                                                const float cellWidth,
//                                                                const float cellHeight,
//                                                                const float zooming)
//{
//    if (m_identificationModeFlag) {
//        resetIdentification();
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    }
//    
//    int32_t numberOfRows = 0;
//    int32_t numberOfColumns = 0;
//    std::vector<float> matrixRGBA;
//    if ( ! matrixChart->getMatrixDataRGBA(numberOfRows,
//                                          numberOfColumns,
//                                          matrixRGBA)) {
//        CaretLogWarning("Matrix RGBA invalid");
//        return;
//    }
//    if ((numberOfRows <= 0)
//        || (numberOfColumns <= 0)) {
//        return;
//    }
//    
//    const ChartTwoMatrixDisplayProperties* matrixProperties = m_chartTwoModel->getChartTwoMatrixDisplayProperties(m_tabIndex);
//    CaretAssert(matrixProperties);
//    
//    bool displayGridLinesFlag = matrixProperties->isGridLinesDisplayed();
//    const bool highlightSelectedRowColumnFlag = matrixProperties->isSelectedRowColumnHighlighted();
//    
//    
//    std::vector<int32_t> selectedColumnIndices;
//    std::vector<int32_t> selectedRowIndices;
//    ChartTwoMatrixLoadingDimensionEnum::Enum selectedRowColumnDimension;
//    matrixChart->getSelectedRowColumnIndices(m_tabIndex,
//                                             selectedRowColumnDimension,
//                                             selectedRowIndices,
//                                             selectedColumnIndices);
//    std::map<int32_t, RowColumnMinMax> columnRangesForRow;
//    for (int32_t rowIndex : selectedRowIndices) {
//        columnRangesForRow[rowIndex] = RowColumnMinMax();
//    }
//    const bool haveSelectedRowsFlag = ( ! columnRangesForRow.empty());
//    
//    std::map<int32_t, RowColumnMinMax> rowRangesForColumn;
//    for (int32_t columnIndex : selectedColumnIndices) {
//        rowRangesForColumn[columnIndex] = RowColumnMinMax();
//    }
//    const bool haveSelectedColumnsFlag = ( ! rowRangesForColumn.empty());
//    
//    uint8_t highlightRGBByte[3];
//    m_preferences->getBackgroundAndForegroundColors()->getColorForegroundChartView(highlightRGBByte);
//    const float highlightRGBA[4] = {
//        highlightRGBByte[0] / 255.0,
//        highlightRGBByte[1] / 255.0,
//        highlightRGBByte[2] / 255.0,
//        1.0
//    };
//    
//    int32_t rgbaOffset = 0;
//    
//    std::unique_ptr<GraphicsPrimitiveV3fC4f> quadsData4f
//    = std::unique_ptr<GraphicsPrimitiveV3fC4f>(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::QUADS));
//    std::unique_ptr<GraphicsPrimitiveV3fC4ub> quadsData4ub
//    = std::unique_ptr<GraphicsPrimitiveV3fC4ub>(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::QUADS));
//    const int32_t numberOfCells = numberOfColumns * numberOfRows;
//    if (m_identificationModeFlag) {
//        quadsData4ub->reserveForNumberOfVertices(numberOfCells);
//    }
//    else {
//        quadsData4f->reserveForNumberOfVertices(numberOfCells);
//    }
//    
//    float cellY = (numberOfRows - 1) * cellHeight;
//    for (int32_t rowIndex = 0; rowIndex < numberOfRows; rowIndex++) {
//        float cellX = 0;
//        for (int32_t columnIndex = 0; columnIndex < numberOfColumns; columnIndex++) {
//            CaretAssertVectorIndex(matrixRGBA, rgbaOffset+3);
//            const float* rgba = &matrixRGBA[rgbaOffset];
//            rgbaOffset += 4;
//            
//            bool drawCellFlag = true;
//            if (chartViewingType != ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL) {
//                if (numberOfRows == numberOfColumns) {
//                    drawCellFlag = false;
//                    switch (chartViewingType) {
//                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL:
//                            break;
//                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
//                            if (rowIndex != columnIndex) {
//                                drawCellFlag = true;
//                            }
//                            break;
//                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
//                            if (rowIndex > columnIndex) {
//                                drawCellFlag = true;
//                            }
//                            break;
//                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
//                            if (rowIndex < columnIndex) {
//                                drawCellFlag = true;
//                            }
//                            break;
//                    }
//                }
//                else {
//                    drawCellFlag = false;
//                    const float slope = static_cast<float>(numberOfRows) / static_cast<float>(numberOfColumns);
//                    const int32_t diagonalRow = static_cast<int32_t>(slope * columnIndex);
//                    
//                    switch (chartViewingType) {
//                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL:
//                            drawCellFlag = true;
//                            break;
//                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL_NO_DIAGONAL:
//                            if (rowIndex != diagonalRow) {
//                                drawCellFlag = true;
//                            }
//                            break;
//                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_LOWER_NO_DIAGONAL:
//                            if (rowIndex > diagonalRow) {
//                                drawCellFlag = true;
//                            }
//                            break;
//                        case ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_UPPER_NO_DIAGONAL:
//                            if (rowIndex < diagonalRow) {
//                                drawCellFlag = true;
//                            }
//                            break;
//                    }
//                }
//            }
//            
//            if (drawCellFlag) {
//                if (haveSelectedRowsFlag) {
//                    std::map<int32_t, RowColumnMinMax>::iterator minMaxIter = columnRangesForRow.find(rowIndex);
//                    if (minMaxIter != columnRangesForRow.end()) {
//                        if (columnIndex < minMaxIter->second.m_min) {
//                            minMaxIter->second.m_min = columnIndex;
//                        }
//                        if (columnIndex > minMaxIter->second.m_max) {
//                            minMaxIter->second.m_max = columnIndex;
//                        }
//                    }
//                }
//                
//                if (haveSelectedColumnsFlag) {
//                    std::map<int32_t, RowColumnMinMax>::iterator minMaxIter = rowRangesForColumn.find(columnIndex);
//                    if (minMaxIter != rowRangesForColumn.end()) {
//                        if (rowIndex < minMaxIter->second.m_min) {
//                            minMaxIter->second.m_min = rowIndex;
//                        }
//                        if (rowIndex > minMaxIter->second.m_max) {
//                            minMaxIter->second.m_max = rowIndex;
//                        }
//                    }
//                }
//                
//                uint8_t idRGBA[4];
//                if (m_identificationModeFlag) {
//                    addToChartMatrixIdentification(rowIndex,
//                                                   columnIndex,
//                                                   idRGBA);
//                }
//                
//                if (m_identificationModeFlag) {
//                    quadsData4ub->addVertex(cellX, cellY, 0.0, idRGBA);
//                    quadsData4ub->addVertex(cellX + cellWidth, cellY, 0.0, idRGBA);
//                    quadsData4ub->addVertex(cellX + cellWidth, cellY + cellHeight, 0.0, idRGBA);
//                    quadsData4ub->addVertex(cellX, cellY + cellHeight, 0.0, idRGBA);
//                }
//                else {
//                    quadsData4f->addVertex(cellX, cellY, 0.0, rgba);
//                    quadsData4f->addVertex(cellX + cellWidth, cellY, 0.0, rgba);
//                    quadsData4f->addVertex(cellX + cellWidth, cellY + cellHeight, 0.0, rgba);
//                    quadsData4f->addVertex(cellX, cellY + cellHeight, 0.0, rgba);
//                }
//            }
//            
//            cellX += cellWidth;
//        }
//        
//        cellY -= cellHeight;
//    }
//    
//    /*
//     * Draw the matrix elements.
//     */
//    if (m_identificationModeFlag) {
//        drawPrimitivePrivate(quadsData4ub.get());
//    }
//    else {
//        /*
//         * Enable alpha blending so voxels that are not drawn from higher layers
//         * allow voxels from lower layers to be seen.
//         */
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        
//        drawPrimitivePrivate(quadsData4f.get());
//        
//        glDisable(GL_BLEND);
//        
//        
//        /*
//         * Drawn an outline around the matrix elements.
//         */
//        if (displayGridLinesFlag) {
//            uint8_t gridLineColorBytes[4];
//            m_preferences->getBackgroundAndForegroundColors()->getColorChartMatrixGridLines(gridLineColorBytes);
//            float gridLineColorFloats[4];
//            CaretPreferences::byteRgbToFloatRgb(gridLineColorBytes,
//                                                gridLineColorFloats);
//            gridLineColorFloats[3] = 1.0;
//            
//            const std::vector<float>& gridXYZ = quadsData4f->getFloatXYZ();
//            const int32_t numCells = static_cast<int32_t>(gridXYZ.size() / 3);
//            
//            std::unique_ptr<GraphicsPrimitiveV3f> outlineData4f
//            = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::QUADS,
//                                                                                       gridLineColorFloats));
//            outlineData4f->reserveForNumberOfVertices(numCells);
//            for (int32_t i = 0; i < numCells; i++) {
//                CaretAssertVectorIndex(gridXYZ, i*3);
//                outlineData4f->addVertex(&gridXYZ[i*3]);
//            }
//            glPolygonMode(GL_FRONT, GL_LINE);
//            glLineWidth(1.0);
//            drawPrimitivePrivate(outlineData4f.get());
//            glPolygonMode(GL_FRONT, GL_FILL);
//        }
//        
//        if ( (! selectedRowIndices.empty())
//            && highlightSelectedRowColumnFlag) {
//            for (auto rowIndex : selectedRowIndices) {
//                const float rowY = (numberOfRows - rowIndex - 1) * cellHeight;
//                
//                int32_t minColumn = 0;
//                int32_t maxColumn = numberOfColumns;
//                if (haveSelectedRowsFlag) {
//                    std::map<int32_t, RowColumnMinMax>::iterator minMaxIter = columnRangesForRow.find(rowIndex);
//                    if (minMaxIter != columnRangesForRow.end()) {
//                        const int32_t minValue = minMaxIter->second.m_min;
//                        const int32_t rowMaxColumn = minMaxIter->second.m_max;
//                        if (minValue < rowMaxColumn) {
//                            minColumn = minValue;
//                            maxColumn = rowMaxColumn + 1;
//                        }
//                    }
//                }
//                
//                const float minX = minColumn * cellWidth;
//                const float maxX = maxColumn * cellWidth;
//                
//                std::unique_ptr<GraphicsPrimitiveV3f> rowOutlineData4f
//                = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::LINE_LOOP,
//                                                                                           highlightRGBA));
//                rowOutlineData4f->reserveForNumberOfVertices(4);
//                
//                rowOutlineData4f->addVertex(minX, rowY);
//                rowOutlineData4f->addVertex(maxX, rowY);
//                rowOutlineData4f->addVertex(maxX, rowY + cellHeight, 0.0);
//                rowOutlineData4f->addVertex(minX, rowY + cellHeight, 0.0);
//                const float highlightLineWidth = std::max(((cellHeight * zooming) * 0.20), 3.0);
//                glLineWidth(highlightLineWidth);
//                drawPrimitivePrivate(rowOutlineData4f.get());
//            }
//            
//            glLineWidth(1.0);
//        }
//        
//        if ( (! selectedColumnIndices.empty())
//            && highlightSelectedRowColumnFlag) {
//            for (auto columnIndex : selectedColumnIndices) {
//                const float colX = columnIndex * cellWidth;
//                
//                int32_t minRow = 0;
//                int32_t maxRow = numberOfRows;
//                if (haveSelectedColumnsFlag) {
//                    std::map<int32_t, RowColumnMinMax>::iterator minMaxIter = rowRangesForColumn.find(columnIndex);
//                    if (minMaxIter != rowRangesForColumn.end()) {
//                        const int32_t minValue = minMaxIter->second.m_min;
//                        const int32_t maxValue = minMaxIter->second.m_max;
//                        if (minValue < maxValue) {
//                            minRow = minValue;
//                            maxRow = maxValue + 1;
//                        }
//                    }
//                }
//                
//                const float minY = minRow * cellHeight;
//                const float maxY = maxRow * cellHeight;
//                
//                std::unique_ptr<GraphicsPrimitiveV3f> columnOutlineData4f
//                = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::LINE_LOOP,
//                                                                                           highlightRGBA));
//                columnOutlineData4f->reserveForNumberOfVertices(4);
//                
//                columnOutlineData4f->addVertex(colX, minY);
//                columnOutlineData4f->addVertex(colX + cellWidth, minY);
//                columnOutlineData4f->addVertex(colX + cellWidth, maxY);
//                columnOutlineData4f->addVertex(colX, maxY);
//                const float highlightLineWidth = std::max(((cellHeight * zooming) * 0.20), 3.0);
//                glLineWidth(highlightLineWidth);
//                drawPrimitivePrivate(columnOutlineData4f.get());
//            }
//            glLineWidth(1.0);
//        }
//        
//        glPolygonMode(GL_FRONT, GL_FILL);
//    }
//    
//    if (m_identificationModeFlag) {
//        processMatrixIdentification(matrixChart);
//    }
//}

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
 * Reset identification.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::resetIdentification()
{
    m_identificationIndices.clear();
    
    if (m_identificationModeFlag) {
        const int32_t estimatedNumberOfItems = 1000;
        m_identificationIndices.reserve(estimatedNumberOfItems);
    }
}
/**
 * Add an item for matrix identification.
 *
 * @param mapIndex
 *     Index of the map whose histogram is displayed (negative indicates all maps).
 * @param bucketIndex
 *     Index of the histogram bar.
 * @param rgbaForColorIdentificationOut
 *     Encoded identification in RGBA color OUTPUT
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::addToHistogramIdentification(const int32_t mapIndex,
                                                                      const int32_t bucketIndex,
                                                                      uint8_t rgbaForColorIdentificationOut[4])
{
    const int32_t idIndex = m_identificationIndices.size() / IDENTIFICATION_INDICES_PER_HISTOGRAM;
    
    m_fixedPipelineDrawing->colorIdentification->addItem(rgbaForColorIdentificationOut,
                                                         m_selectionItemDataType,
                                                         idIndex);
    rgbaForColorIdentificationOut[3] = 255;
    
    /*
     * If these items change, need to update reset and
     * processing of identification.
     */
    m_identificationIndices.push_back(mapIndex);
    m_identificationIndices.push_back(bucketIndex);
}

/**
 * Process identification for histogram.
 *
 * @param histogramChart
 *     The histogram chart.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::processHistogramIdentification(const ChartableTwoFileHistogramChart* histogramChart)
{
    if (histogramChart != NULL) {
        int32_t identifiedItemIndex;
        float depth = -1.0;
        m_fixedPipelineDrawing->getIndexFromColorSelection(m_selectionItemDataType,
                                                           m_fixedPipelineDrawing->mouseX,
                                                           m_fixedPipelineDrawing->mouseY,
                                                           identifiedItemIndex,
                                                           depth);
        if (identifiedItemIndex >= 0) {
            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_HISTOGRAM;
            const int32_t mapIndex = m_identificationIndices[idIndex];
            const int32_t bucketIndex = m_identificationIndices[idIndex+1];
            
            if (m_selectionItemHistogram->isOtherScreenDepthCloserToViewer(depth)) {
                m_selectionItemHistogram->setHistogramChart(const_cast<ChartableTwoFileHistogramChart*>(histogramChart),
                                                            mapIndex,
                                                            bucketIndex);
            }
        }
    }
}

/**
 * Process identification for line-series.
 *
 * @param lineSeriesChart
 *     The line-series chart.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::processLineSeriesIdentification(const ChartableTwoFileLineSeriesChart* lineSeriesChart)
{
    //    int32_t identifiedItemIndex;
    //    float depth = -1.0;
    //
    //    if (m_chartModelDataSeriesBeingDrawnForIdentification != NULL) {
    //        m_fixedPipelineDrawing->getIndexFromColorSelection(m_chartCartesianSelectionTypeForIdentification,
    //                                                           m_fixedPipelineDrawing->mouseX,
    //                                                           m_fixedPipelineDrawing->mouseY,
    //                                                           identifiedItemIndex,
    //                                                           depth);
    //        if (identifiedItemIndex >= 0) {
    //            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_CHART_LINE;
    //            const int32_t chartDataIndex = m_identificationIndices[idIndex];
    //            const int32_t chartLineIndex = m_identificationIndices[idIndex + 1];
    //
    //            SelectionItemChartDataSeries* chartDataSeriesID = m_brain->getSelectionManager()->getChartDataSeriesIdentification();
    //            if (chartDataSeriesID->isOtherScreenDepthCloserToViewer(depth)) {
    //                ChartDataCartesian* chartDataCartesian =
    //                dynamic_cast<ChartDataCartesian*>(m_chartModelDataSeriesBeingDrawnForIdentification->getChartDataAtIndex(chartDataIndex));
    //                CaretAssert(chartDataCartesian);
    //                chartDataSeriesID->setChart(m_chartModelDataSeriesBeingDrawnForIdentification,
    //                                            chartDataCartesian,
    //                                            chartLineIndex);
    //
    //                const ChartPoint* chartPoint = chartDataCartesian->getPointAtIndex(chartLineIndex);
    //                const float lineXYZ[3] = {
    //                    chartPoint->getX(),
    //                    chartPoint->getY(),
    //                    0.0
    //                };
    //
    //                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(chartDataSeriesID,
    //                                                                 lineXYZ);
    //            }
    //        }
    //    }
    //    else if (m_chartModelFrequencySeriesBeingDrawnForIdentification != NULL) {
    //        m_fixedPipelineDrawing->getIndexFromColorSelection(m_chartCartesianSelectionTypeForIdentification,
    //                                                           m_fixedPipelineDrawing->mouseX,
    //                                                           m_fixedPipelineDrawing->mouseY,
    //                                                           identifiedItemIndex,
    //                                                           depth);
    //        if (identifiedItemIndex >= 0) {
    //            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_CHART_LINE;
    //            const int32_t chartDataIndex = m_identificationIndices[idIndex];
    //            const int32_t chartLineIndex = m_identificationIndices[idIndex + 1];
    //
    //            SelectionItemChartFrequencySeries* chartFrequencySeriesID = m_brain->getSelectionManager()->getChartFrequencySeriesIdentification();
    //            if (chartFrequencySeriesID->isOtherScreenDepthCloserToViewer(depth)) {
    //                ChartDataCartesian* chartDataCartesian =
    //                dynamic_cast<ChartDataCartesian*>(m_chartModelFrequencySeriesBeingDrawnForIdentification->getChartDataAtIndex(chartDataIndex));
    //                CaretAssert(chartDataCartesian);
    //                chartFrequencySeriesID->setChart(m_chartModelFrequencySeriesBeingDrawnForIdentification,
    //                                                 chartDataCartesian,
    //                                                 chartLineIndex);
    //
    //                const ChartPoint* chartPoint = chartDataCartesian->getPointAtIndex(chartLineIndex);
    //                const float lineXYZ[3] = {
    //                    chartPoint->getX(),
    //                    chartPoint->getY(),
    //                    0.0
    //                };
    //
    //                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(chartFrequencySeriesID,
    //                                                                 lineXYZ);
    //            }
    //        }
    //    }
    //    else if (m_chartModelTimeSeriesBeingDrawnForIdentification != NULL) {
    //        m_fixedPipelineDrawing->getIndexFromColorSelection(m_chartCartesianSelectionTypeForIdentification,
    //                                                           m_fixedPipelineDrawing->mouseX,
    //                                                           m_fixedPipelineDrawing->mouseY,
    //                                                           identifiedItemIndex,
    //                                                           depth);
    //        if (identifiedItemIndex >= 0) {
    //            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_CHART_LINE;
    //            const int32_t chartDataIndex = m_identificationIndices[idIndex];
    //            const int32_t chartLineIndex = m_identificationIndices[idIndex + 1];
    //
    //            SelectionItemChartTimeSeries* chartTimeSeriesID = m_brain->getSelectionManager()->getChartTimeSeriesIdentification();
    //            if (chartTimeSeriesID->isOtherScreenDepthCloserToViewer(depth)) {
    //                ChartDataCartesian* chartDataCartesian =
    //                dynamic_cast<ChartDataCartesian*>(m_chartModelTimeSeriesBeingDrawnForIdentification->getChartDataAtIndex(chartDataIndex));
    //                CaretAssert(chartDataCartesian);
    //                chartTimeSeriesID->setChart(m_chartModelTimeSeriesBeingDrawnForIdentification,
    //                                            chartDataCartesian,
    //                                            chartLineIndex);
    //
    //                const ChartPoint* chartPoint = chartDataCartesian->getPointAtIndex(chartLineIndex);
    //                const float lineXYZ[3] = {
    //                    chartPoint->getX(),
    //                    chartPoint->getY(),
    //                    0.0
    //                };
    //                
    //                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(chartTimeSeriesID,
    //                                                                 lineXYZ);
    //            }
    //        }
    //    }
    
}

/**
 * Add an item for matrix identification.
 *
 * @param matrixRowIndex
 *     Index of the row
 * @param matrixColumnIndex
 *     Index of the column
 * @param rgbaForColorIdentificationOut
 *    Encoded identification in RGBA color OUTPUT
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::addToChartMatrixIdentification(const int32_t matrixRowIndex,
                                                                        const int32_t matrixColumnIndex,
                                                                        uint8_t rgbaForColorIdentificationOut[4])
{
    const int32_t idIndex = m_identificationIndices.size() / IDENTIFICATION_INDICES_PER_MATRIX_ELEMENT;
    
    m_fixedPipelineDrawing->colorIdentification->addItem(rgbaForColorIdentificationOut,
                                                         m_selectionItemDataType,
                                                         idIndex);
    rgbaForColorIdentificationOut[3] = 255;
    
    /*
     * If these items change, need to update reset and
     * processing of identification.
     */
    m_identificationIndices.push_back(matrixRowIndex);
    m_identificationIndices.push_back(matrixColumnIndex);
}

/**
 * Process identification for matrix.
 *
 * @param matrixChart
 *     The matrix chart.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::processMatrixIdentification(const ChartableTwoFileMatrixChart* matrixChart)
{
   if (matrixChart != NULL) {
        int32_t identifiedItemIndex;
        float depth = -1.0;
        m_fixedPipelineDrawing->getIndexFromColorSelection(m_selectionItemDataType,
                                                           m_fixedPipelineDrawing->mouseX,
                                                           m_fixedPipelineDrawing->mouseY,
                                                           identifiedItemIndex,
                                                           depth);
        if (identifiedItemIndex >= 0) {
            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_MATRIX_ELEMENT;
            const int32_t rowIndex = m_identificationIndices[idIndex];
            const int32_t columnIndex = m_identificationIndices[idIndex + 1];
            
            if (m_selectionItemMatrix->isOtherScreenDepthCloserToViewer(depth)) {
                m_selectionItemMatrix->setMatrixChart(const_cast<ChartableTwoFileMatrixChart*>(matrixChart),
                                                      rowIndex,
                                                      columnIndex);
            }
        }
    }
}

/**
 * Estimate the size of the axis' text.
 *
 * @param dataBounds
 *     Bounds of data [minX, maxX, minY, maxY].
 * @param viewportHeight
 *     Height of viewport.
 * @param cartesianAxis
 *    The axis.
 * @param widthOut
 *    Width of text out.
 * @param heightOut
 *    Heigh of text out.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::estimateCartesianChartAxisLegendsWidthHeight(const float dataBounds[4],
                                                                                      const float viewportHeight,
                                                                                      ChartTwoCartesianAxis* cartesianAxis,
                                                                                      double& widthOut,
                                                                                      double& heightOut)
{
    widthOut  = 0;
    heightOut = 0;
    
    if (cartesianAxis == NULL) {
        return;
    }
    if ( ! cartesianAxis->isVisible()) {
        return;
    }
    
    /*
     * The length of the axis does not matter here.
     * We are just estimating the width and height of the axes text labels.
     */
    const float axisLength = 1000.0;
    float minimumValue = 0.0;
    float maximumValue = 0.0;
    std::vector<float> scaleValueOffsetInPixels;
    std::vector<AString> scaleValuesText;
    cartesianAxis->getScaleValuesAndOffsets(dataBounds,
                                   axisLength,
                                   minimumValue,
                                   maximumValue,
                                   scaleValueOffsetInPixels,
                                   scaleValuesText);
    for (std::vector<AString>::iterator iter = scaleValuesText.begin();
         iter != scaleValuesText.end();
         iter++) {
        const AString text = *iter;
        if ( ! text.isEmpty()) {
            AnnotationPointSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
            annotationText.setText(text);
            double textWidth = 0.0;
            double textHeight = 0.0;
            m_textRenderer->getTextWidthHeightInPixels(annotationText, viewportHeight, textWidth, textHeight);
            
            widthOut  = std::max(widthOut,  textWidth);
            heightOut = std::max(heightOut, textHeight);
        }
    }
    
    switch (cartesianAxis->getAxisLocation()) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            if (heightOut > 0) {
                heightOut += s_tickLength;
            }
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
            if (widthOut > 0) {
                widthOut += s_tickLength;
            }
            break;
    }
    
    const AString axisTitle = cartesianAxis->getAxisTitle();
    if ( ! axisTitle.isEmpty()) {
        AnnotationPointSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        annotationText.setText(axisTitle);
        switch (cartesianAxis->getAxisLocation()) {
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                annotationText.setOrientation(AnnotationTextOrientationEnum::HORIZONTAL);
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                annotationText.setOrientation(AnnotationTextOrientationEnum::HORIZONTAL);
                annotationText.setRotationAngle(-90.0);
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                annotationText.setOrientation(AnnotationTextOrientationEnum::HORIZONTAL);
                annotationText.setRotationAngle(-90.0);
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                annotationText.setOrientation(AnnotationTextOrientationEnum::HORIZONTAL);
                break;
        }
        
        double textWidth = 0.0;
        double textHeight = 0.0;
        m_textRenderer->getTextWidthHeightInPixels(annotationText, viewportHeight, textWidth, textHeight);
        
        /*
         * Note: Text on left and right is rotated but we need the width
         * and height after rotation so swap the width and height of the text
         */
        switch (cartesianAxis->getAxisLocation()) {
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                heightOut += textHeight;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                std::swap(textWidth, textHeight);
                widthOut += textWidth;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                std::swap(textWidth, textHeight);
                widthOut += textWidth;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                heightOut += textHeight;
                break;
        }
    }
}

/**
 * Draw the chart graphics surrounding box and set the graphics viewport.
 *  drawChartGraphicsBoxAndSetViewport
 * @param vpX
 *     Viewport X
 * @param vpY
 *     Viewport Y
 * @param vpWidth
 *     Viewport width
 * @param vpHeight
 *     Viewport height
 * @param marginSize
 *     Margin around grid/box
 * @param chartGraphicsDrawingViewportOut
 *     Output containing viewport for drawing chart graphics within
 *     the box/grid that is adjusted for the box's line thickness.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawChartGraphicsBoxAndSetViewport(const float vpX,
                                                                         const float vpY,
                                                                         const float vpWidth,
                                                                         const float vpHeight,
                                                                         const Margins& margins,
                                                                         int32_t chartGraphicsDrawingViewportOut[4])
{
    const float halfGridLineWidth = GRID_LINE_WIDTH / 2.0;
    
    const float gridLeft   = vpX + margins.m_left;
    const float gridRight  = vpX + vpWidth - margins.m_right;
    const float gridBottom = vpY + margins.m_bottom;
    const float gridTop    = vpY + vpHeight - margins.m_top;
    
    glViewport(vpX,
               vpY,
               vpWidth,
               vpHeight);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(vpX, (vpX + vpWidth),
            vpY, (vpY + vpHeight),
            -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    std::unique_ptr<GraphicsPrimitiveV3f> gridData
    = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::LINES,
                                                                               m_fixedPipelineDrawing->m_foregroundColorFloat));
    gridData->reserveForNumberOfVertices(8);
    gridData->addVertex(gridLeft,  gridBottom + halfGridLineWidth);
    gridData->addVertex(gridRight, gridBottom + halfGridLineWidth);
    gridData->addVertex(gridRight - halfGridLineWidth, gridBottom);
    gridData->addVertex(gridRight - halfGridLineWidth, gridTop);
    gridData->addVertex(gridRight, gridTop - halfGridLineWidth);
    gridData->addVertex(gridLeft,  gridTop - halfGridLineWidth);
    gridData->addVertex(gridLeft + halfGridLineWidth, gridTop);
    gridData->addVertex(gridLeft + halfGridLineWidth, gridBottom);
    glLineWidth(GRID_LINE_WIDTH);
    drawPrimitivePrivate(gridData.get());
    
    /*
     * Region inside the grid's box
     */
    const int32_t graphicsLeft   = static_cast<int32_t>(gridLeft   + std::ceil(GRID_LINE_WIDTH  + 1.0));
    const int32_t graphicsRight  = static_cast<int32_t>(gridRight  - std::floor(GRID_LINE_WIDTH + 1.0));
    const int32_t graphicsBottom = static_cast<int32_t>(gridBottom + std::ceil(GRID_LINE_WIDTH  + 1.0));
    const int32_t graphicsTop    = static_cast<int32_t>(gridTop    - std::floor(GRID_LINE_WIDTH + 1.0));
    
    const int32_t graphicsWidth = graphicsRight - graphicsLeft;
    const int32_t graphicsHeight = graphicsTop  - graphicsBottom;
    chartGraphicsDrawingViewportOut[0] = graphicsLeft;
    chartGraphicsDrawingViewportOut[1] = graphicsBottom;
    chartGraphicsDrawingViewportOut[2] = graphicsWidth;
    chartGraphicsDrawingViewportOut[3] = graphicsHeight;
}

/**
 * Draw the chart axes grid/box
 *
 * @param vpX
 *     Viewport X for all chart content
 * @param vpY
 *     Viewport Y for all chart content
 * @param vpWidth
 *     Viewport width for all chart content
 * @param vpHeight
 *     Viewport height for all chart content
 * @param margins
 *     Margin around graphics region.  The margin corresponding to the
 *     axis may be changed so that all text in the axis is visible
 *     (and not cut off).
 * @param axis
 *     Axis that is drawn.
 * @param axisMinimumOut
 *     Minimum value along the axis.
 * @param axisMaximumOut
 *     Maximum value along the axis.
 * @return
 *     True if the axis is valid and was drawn, else false.
 */
bool
BrainOpenGLChartTwoDrawingFixedPipeline::drawChartAxisCartesian(const float dataBounds[4],
                                                                const float vpX,
                                                                const float vpY,
                                                                const float vpWidth,
                                                                const float vpHeight,
                                                                const Margins& margins,
                                                                ChartTwoCartesianAxis* axis,
                                                                float& axisMinimumOut,
                                                                float& axisMaximumOut)
{
    if (axis == NULL) {
        return false;
    }
    if ( ! axis->isVisible()) {
        return false;
    }
    
    
    int32_t axisVpX      = 0;
    int32_t axisVpY      = 0;
    int32_t axisVpWidth  = 0;
    int32_t axisVpHeight = 0;
    float axisLength = 0.0;
    switch (axis->getAxisLocation()) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
            axisVpX      = vpX + margins.m_left;
            axisVpY      = vpY;
            axisVpWidth  = vpWidth - (margins.m_left + margins.m_right);
            axisVpHeight = margins.m_bottom;
            axisLength   = axisVpWidth;
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            axisVpX      = vpX + margins.m_left;
            axisVpY      = vpY + vpHeight - margins.m_top;
            axisVpWidth  = vpWidth - (margins.m_left + margins.m_right);
            axisVpHeight = margins.m_top;
            axisLength   = axisVpWidth;
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
            axisVpX      = vpX;
            axisVpY      = vpY + margins.m_bottom;
            axisVpWidth  = margins.m_left + 1; // so ticks go to box
            axisVpHeight = vpHeight - (margins.m_bottom + margins.m_top);
            axisLength   = axisVpHeight;
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
            axisVpX      = vpX + vpWidth - margins.m_right;
            axisVpY      = vpY + margins.m_bottom;
            axisVpWidth  = margins.m_right;
            axisVpHeight = vpHeight - (margins.m_bottom + margins.m_top);
            axisLength   = axisVpHeight;
            break;
    }
    const int32_t axisViewport[4] {
        axisVpX,
        axisVpY,
        axisVpWidth,
        axisVpHeight
    };
    
    std::vector<float> scaleValueOffsetInPixels;
    std::vector<AString> scaleValuesText;
    const bool validAxisFlag = axis->getScaleValuesAndOffsets(dataBounds,
                                                           axisLength,
                                                           axisMinimumOut,
                                                           axisMaximumOut,
                                                           scaleValueOffsetInPixels,
                                                           scaleValuesText);
    if ( ! validAxisFlag) {
        return false;
    }
    
    const float rgba[4] = {
        m_fixedPipelineDrawing->m_foregroundColorFloat[0],
        m_fixedPipelineDrawing->m_foregroundColorFloat[1],
        m_fixedPipelineDrawing->m_foregroundColorFloat[2],
        1.0
    };
    
    const int32_t numScaleValuesToDraw = static_cast<int32_t>(scaleValuesText.size());
    AnnotationPointSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
    annotationText.setCustomTextColor(rgba);
    annotationText.setTextColor(CaretColorEnum::CUSTOM);
    
    if (numScaleValuesToDraw > 0) {
        float startX = 0.0;
        float startY = 0.0;
        float offsetMultiplierX = 0.0;
        float offsetMultiplierY = 0.0;
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        
        float tickDeltaXY[2] = { 0.0, 0.0 };
        switch (axis->getAxisLocation()) {
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                startX = 0;
                startY = axisVpHeight;
                offsetMultiplierX = 1.0;
                offsetMultiplierY = 0.0;
                annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
                annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                tickDeltaXY[0] = 0.0;
                tickDeltaXY[1] = -s_tickLength;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                startX = 0.0;
                startY = 0.0;
                offsetMultiplierX = 1.0;
                offsetMultiplierY = 0.0;
                annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
                annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                tickDeltaXY[0] = 0.0;
                tickDeltaXY[1] = s_tickLength;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                startX = axisVpWidth;
                startY = 0.0;
                offsetMultiplierX = 0.0;
                offsetMultiplierY = 1.0;
                annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
                annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
                tickDeltaXY[0] = -s_tickLength;
                tickDeltaXY[1] = 0.0;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                startX = 0.0;
                startY = 0.0;
                offsetMultiplierX = 0.0;
                offsetMultiplierY = 1.0;
                annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
                annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
                tickDeltaXY[0] = s_tickLength;
                tickDeltaXY[1] = 0.0;
                break;
        }
        
        glViewport(axisViewport[0],
                   axisViewport[1],
                   axisViewport[2],
                   axisViewport[3]);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, axisVpWidth, 0, axisVpHeight, -1.0, 1.0);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        glColor3fv(m_fixedPipelineDrawing->m_foregroundColorFloat);
        
        std::unique_ptr<GraphicsPrimitiveV3f> ticksData
        = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::LINES,
                                                                                   m_fixedPipelineDrawing->m_foregroundColorFloat));
        ticksData->reserveForNumberOfVertices(numScaleValuesToDraw * 2);
        
        const bool showTicksEnabledFlag = axis->isShowTickmarks();
        
        const float halfGridLineWidth = GRID_LINE_WIDTH / 2.0;
        const int32_t firstIndex = 0;
        const int32_t lastIndex  = numScaleValuesToDraw - 1;
        for (int32_t i = 0; i < numScaleValuesToDraw; i++) {
            float tickStartX = startX + scaleValueOffsetInPixels[i] * offsetMultiplierX;
            float tickStartY = startY + scaleValueOffsetInPixels[i] * offsetMultiplierY;
            float textOffsetX = 0.0;
            float textOffsetY = 0.0;
            bool showTickAtScaleValueFlag = showTicksEnabledFlag;
            switch (axis->getAxisLocation()) {
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                    if (i == firstIndex) {
                        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
                        tickStartX += halfGridLineWidth;
                    }
                    else if (i == lastIndex) {
                        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
                        tickStartX -= halfGridLineWidth;
                    }
                    textOffsetY = 2.0;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
                    tickStartX -= halfGridLineWidth;
                    if (i == firstIndex) {
                        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                        tickStartY += halfGridLineWidth;
                    }
                    else if (i == lastIndex) {
                        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                        tickStartY -= halfGridLineWidth;
                    }
                    textOffsetX = 2.0;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
                    tickStartX += halfGridLineWidth;
                    if (i == firstIndex) {
                        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                        tickStartY += halfGridLineWidth;
                    }
                    else if (i == lastIndex) {
                        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                        tickStartY -= halfGridLineWidth;
                    }
                    textOffsetX = -2.0;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                    if (i == firstIndex) {
                        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
                        tickStartX += halfGridLineWidth;
                    }
                    else if (i == lastIndex) {
                        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
                        tickStartX -= halfGridLineWidth;
                    }
                    textOffsetY = -2.0;
                    break;
            }
            const float tickEndX = tickStartX + tickDeltaXY[0];
            const float tickEndY = tickStartY + tickDeltaXY[1];
            
            if ((i == firstIndex)
                || (i == lastIndex)) {
                showTickAtScaleValueFlag = false;
            }
            
            if (showTickAtScaleValueFlag) {
                ticksData->addVertex(tickStartX, tickStartY);
                ticksData->addVertex(tickEndX,   tickEndY);
            }
            
            const float textX = tickEndX + textOffsetX;
            const float textY = tickEndY + textOffsetY;
            annotationText.setText(scaleValuesText[i]);
            m_textRenderer->drawTextAtViewportCoords(textX,
                                                     textY,
                                                     0.0,
                                                     annotationText);
        }

        if (ticksData->isValid()) {
            glLineWidth(GRID_LINE_WIDTH);
            drawPrimitivePrivate(ticksData.get());
        }
        
        const AString axisTitle = axis->getAxisTitle();
        if ( ! axisTitle.isEmpty()) {
            annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
            annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
            
            bool drawAxisTextVerticalFlag = false;
            float axisTextCenterX = axisVpWidth / 2.0;
            float axisTextCenterY = axisVpHeight / 2.0;
            const float textMarginOffset = 5.0;
            switch (axis->getAxisLocation()) {
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                    axisTextCenterX = (axisVpWidth / 2.0);
                    axisTextCenterY = textMarginOffset;
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                    axisTextCenterX = (axisVpWidth / 2.0);
                    axisTextCenterY = axisVpHeight - textMarginOffset;
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                    axisTextCenterX = textMarginOffset;
                    axisTextCenterY = (axisVpHeight / 2.0);
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                    drawAxisTextVerticalFlag = true;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                    axisTextCenterX = axisVpWidth - textMarginOffset;
                    axisTextCenterY = (axisVpHeight / 2.0);
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                    drawAxisTextVerticalFlag = true;
                    break;
            }
            if (drawAxisTextVerticalFlag) {
                annotationText.setRotationAngle(-90.0);
                annotationText.setText(axisTitle);
                m_textRenderer->drawTextAtViewportCoords(axisTextCenterX,
                                                         axisTextCenterY,
                                                         0.0,
                                                         annotationText);
                annotationText.setRotationAngle(0.0);
            }
            else {
                annotationText.setOrientation(AnnotationTextOrientationEnum::HORIZONTAL);
                annotationText.setText(axisTitle);
                m_textRenderer->drawTextAtViewportCoords(axisTextCenterX,
                                                         axisTextCenterY,
                                                         0.0,
                                                         annotationText);
            }
        }
    }
    
    return true;
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
    GraphicsEngineDataOpenGL::draw(m_fixedPipelineDrawing->getOpenGLContextPointer(),
                                   primitive);
}


BrainOpenGLChartTwoDrawingFixedPipeline::HistogramChartDrawingInfo::HistogramChartDrawingInfo(HistogramDrawingInfo* histogramDrawingInfo,
                          const ChartableTwoFileHistogramChart* histogramChart,
                          int32_t mapIndex,
                          ChartAxisLocationEnum::Enum verticalAxisLocation)
: m_histogramDrawingInfo(histogramDrawingInfo),
m_histogramChart(histogramChart),
m_mapIndex(mapIndex),
m_verticalAxisLocation(verticalAxisLocation)
{
}


BrainOpenGLChartTwoDrawingFixedPipeline::HistogramChartDrawingInfo::~HistogramChartDrawingInfo() {
    delete m_histogramDrawingInfo;
}
