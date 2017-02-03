
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

#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "ChartTwoDataHistogram.h"
#include "ChartTwoMatrixDisplayProperties.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoOverlaySet.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "ChartableTwoFileMatrixChart.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "FastStatistics.h"
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
 * @parm chartTwoModel
 *     The chart two model in the window.
 * @param fixedPipelineDrawing
 *     The fixed pipeline OpenGL drawing.
 * @param textRenderer
 *     Text rendering.
 * @param translation
 *     The chart's translation.
 * @param zooming
 *     The chart's zooming.
 * @param chartOverlaySet
 *     Chart overlay set that is drawn.
 * @param selectionItemDataType
 *     Selected data type.
 * @param viewport
 *     Viewport for the chart.
 * @param tabIndex
 *     Index of the tab.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawChartOverlaySet(Brain* brain,
                                                             ModelChartTwo* chartTwoModel,
                                                             BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                             BrainOpenGLTextRenderInterface* textRenderer,
                                                             const float translation[3],
                                                             const float zooming,
                                                             ChartTwoOverlaySet* chartOverlaySet,
                                                             const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                                             const int32_t viewport[4],
                                                             const int32_t tabIndex)
{
    m_brain = brain;
    m_chartTwoModel = chartTwoModel;
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    m_textRenderer = textRenderer;
    m_translation[0] = translation[0];
    m_translation[1] = translation[1];
    m_translation[2] = translation[2];
    m_zooming        = zooming;
    m_chartOverlaySet = chartOverlaySet;
    m_selectionItemDataType = selectionItemDataType;
    m_viewport[0] = viewport[0];
    m_viewport[1] = viewport[1];
    m_viewport[2] = viewport[2];
    m_viewport[3] = viewport[3];
    m_tabIndex = tabIndex;

    m_selectionItemHistogram  = m_brain->getSelectionManager()->getChartTwoHistogramIdentification();
    m_selectionItemLineSeries = m_brain->getSelectionManager()->getChartTwoLineSeriesIdentification();
    m_selectionItemMatrix     = m_brain->getSelectionManager()->getChartTwoMatrixIdentification();

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
            if (topOverlay->isEnabled()) {
                CaretMappableDataFile* cmdf = NULL;
                int32_t mapIndex = -1;
                topOverlay->getSelectionData(cmdf, mapIndex);
                if (cmdf != NULL) {
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
    }
    
    restoreStateOfOpenGL();
}

///**
// * Draw histogram charts.
// */
//void
//BrainOpenGLChartTwoDrawingFixedPipeline::drawHistogramChart()
//{
//    const int32_t vpX      = m_viewport[0];
//    const int32_t vpY      = m_viewport[1];
//    const int32_t vpWidth  = m_viewport[2];
//    const int32_t vpHeight = m_viewport[3];
//    
//    int32_t chartGraphicsDrawingViewport[4] = {
//        vpX,
//        vpY,
//        vpWidth,
//        vpHeight
//    };
//    
//    
////    /*
////     * Margin is region around the chart in which
////     * the axes legends, values, and ticks are drawn.
////     */
//    const double marginSize = 30;
//    Margins margins(marginSize);
////    
////    double width, height;
////    estimateCartesianChartAxisLegendsWidthHeight(textRenderer, vpHeight, cartesianChart->getLeftAxis(), width, height);
////    margins.m_left = std::max(margins.m_left, width);
////    estimateCartesianChartAxisLegendsWidthHeight(textRenderer, vpHeight, cartesianChart->getRightAxis(), width, height);
////    margins.m_right = std::max(margins.m_right, width);
////    estimateCartesianChartAxisLegendsWidthHeight(textRenderer, vpHeight, cartesianChart->getTopAxis(), width, height);
////    margins.m_top = std::max(margins.m_top, height);
////    estimateCartesianChartAxisLegendsWidthHeight(textRenderer, vpHeight, cartesianChart->getBottomAxis(), width, height);
////    margins.m_bottom = std::max(margins.m_bottom, height);
////    
////    if (margins.m_left > marginSize) margins.m_left += 10;
////    if (margins.m_right > marginSize) margins.m_right += 10;
////    
////    /*
////     * Ensure that there is sufficient space for the axes data display.
////     */
////    if ((vpWidth > (marginSize * 3))
////        && (vpHeight > (marginSize * 3))) {
////        
////        /* Draw legends and grids */
////        drawChartAxis(vpX,
////                      vpY,
////                      vpWidth,
////                      vpHeight,
////                      margins,
////                      textRenderer,
////                      cartesianChart->getLeftAxis());
////        
////        drawChartAxis(vpX,
////                      vpY,
////                      vpWidth,
////                      vpHeight,
////                      margins,
////                      textRenderer,
////                      cartesianChart->getRightAxis());
////        
////        drawChartAxis(vpX,
////                      vpY,
////                      vpWidth,
////                      vpHeight,
////                      margins,
////                      textRenderer,
////                      cartesianChart->getBottomAxis());
////        
////        drawChartAxis(vpX,
////                      vpY,
////                      vpWidth,
////                      vpHeight,
////                      margins,
////                      textRenderer,
////                      cartesianChart->getTopAxis());
////    
////        
////        drawChartGraphicsBoxAndSetViewport(vpX,
////                                           vpY,
////                                           vpWidth,
////                                           vpHeight,
////                                           margins,
////                                           chartGraphicsDrawingViewport);
////    }
//    
//    
//    const int32_t numberOfOverlays = m_chartOverlaySet->getNumberOfDisplayedOverlays();
//    CaretAssert(numberOfOverlays > 0);
//    const ChartTwoOverlay* topOverlay = m_chartOverlaySet->getOverlay(0);
//    const ChartTwoCompoundDataType cdt = topOverlay->getChartTwoCompoundDataType();
//    CaretAssert(cdt.getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM);
//    
//    /*
//     * Get extent of histogram data
//     */
//    float xMin =   0.0;
//    float xMax =   0.0;
//    float yMin =   0.0;
//    float yMax =   0.0;
//    for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
//        ChartTwoOverlay* chartOverlay = m_chartOverlaySet->getOverlay(iOverlay);
//        if ( ! chartOverlay->isEnabled()) {
//            continue;
//        }
//        
//        CaretMappableDataFile* mapFile = NULL;
//        int32_t mapIndex = 1;
//        chartOverlay->getSelectionData(mapFile,
//                                       mapIndex);
//        if (mapFile == NULL) {
//            continue;
//        }
//        
//        const ChartableTwoFileDelegate* chartDelegate        = mapFile->getChartingDelegate();
//        const ChartableTwoFileHistogramChart* histogramChart = chartDelegate->getHistogramCharting();
//        if (histogramChart->isValid()) {
//            const ChartTwoDataHistogram* histogramData = histogramChart->getMapHistogramChart(mapIndex);
//            float bounds[4];
//            if (histogramData->getBounds(bounds)) {
//                xMin = std::min(xMin, bounds[0]);
//                xMax = std::max(xMax, bounds[1]);
//                yMin = std::min(yMin, bounds[2]);
//                yMax = std::max(yMax, bounds[3]);
//            }
//        }
//    }
//    
//    /*
//     * Bounds valid?
//     */
//    if ((xMin >= xMax)
//        || (yMin >= yMax)) {
//        return;
//    }
//    
////    std::cout << "All bounds: "
////    << xMin << " "
////    << xMax << " "
////    << yMin << " "
////    << yMax << " " << std::endl;
//    
//    glViewport(chartGraphicsDrawingViewport[0],
//               chartGraphicsDrawingViewport[1],
//               chartGraphicsDrawingViewport[2],
//               chartGraphicsDrawingViewport[3]);
//    
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(xMin, xMax,
//            yMin, yMax,
//            -10.0, 10.0);
//    
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    
//    bool applyTransformationsFlag = true;
//    if (applyTransformationsFlag) {
//        glTranslatef(m_translation[0],
//                     m_translation[1],
//                     0.0);
//        
//        const float chartWidth  = chartGraphicsDrawingViewport[2];
//        const float chartHeight = chartGraphicsDrawingViewport[3];
//        const float halfWidth   = chartWidth  / 2.0;
//        const float halfHeight  = chartHeight / 2.0;
//        glTranslatef(halfWidth,
//                     halfHeight,
//                     0.0);
//        glScalef(m_zooming,
//                 m_zooming,
//                 1.0);
//        glTranslatef(-halfWidth,
//                     -halfHeight,
//                     0.0);
//    }
//    
//    int32_t layerIndex = 0;
//    for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
//        ChartTwoOverlay* chartOverlay = m_chartOverlaySet->getOverlay(iOverlay);
//        if ( ! chartOverlay->isEnabled()) {
//            continue;
//        }
//        
//        CaretMappableDataFile* mapFile = NULL;
//        int32_t mapIndex = 1;
//        chartOverlay->getSelectionData(mapFile,
//                                       mapIndex);
//        if (mapFile == NULL) {
//            continue;
//        }
//        
//        const ChartableTwoFileDelegate* chartDelegate        = mapFile->getChartingDelegate();
//        const ChartableTwoFileHistogramChart* histogramChart = chartDelegate->getHistogramCharting();
//        if (histogramChart->isValid()) {
//            drawHistogramChartContent(histogramChart,
//                                      mapIndex);
//            layerIndex++;
//        }
//    }
//}

/*
 * REMOVE THIS
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::colorHistogramWithPalette(const ChartableTwoFileHistogramChart* histogramChart,
                          const int32_t mapIndex,
                          std::vector<float>& rgbaOut)
{
    CaretMappableDataFile* cmdf = const_cast<CaretMappableDataFile*>(histogramChart->getCaretMappableDataFile());
    
    FastStatistics* statistics = NULL;
    switch (cmdf->getPaletteNormalizationMode()) {
        case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
            statistics = const_cast<FastStatistics*>(cmdf->getFileFastStatistics());
            break;
        case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
            statistics = const_cast<FastStatistics*>(cmdf->getMapFastStatistics(mapIndex));
            break;
    }
    CaretAssert(statistics);
    
    PaletteColorMapping* paletteColorMapping = cmdf->getMapPaletteColorMapping(mapIndex);
    CaretAssert(paletteColorMapping);
    PaletteFile* paletteFile = m_brain->getPaletteFile();
    CaretAssert(paletteFile);
    const Palette* palette = paletteFile->getPaletteByName(paletteColorMapping->getSelectedPaletteName());

    const Histogram* histogram = cmdf->getMapHistogram(mapIndex);
    CaretAssert(histogram);
    
    if ((paletteColorMapping != NULL)
        && (statistics != NULL)
        && (palette != NULL)
        && (histogram != NULL)) {
        
        const std::vector<float>& buckets = histogram->getHistogramDisplay();
        if ( ! buckets.empty()) {
            rgbaOut.resize(buckets.size() * 4);

            const int32_t numDataValues = static_cast<int32_t>(buckets.size());
            std::vector<float> data(numDataValues);
            
            const float minValue = statistics->getMin();
            const float maxValue = statistics->getMax();
            float step = 1.0;
            if (numDataValues > 1) {
                step = ((maxValue - minValue)
                        / static_cast<float>(numDataValues));
            }
            for (int64_t ix = 0; ix < numDataValues; ix++) {
                const float value = (minValue
                                     + (ix * step));
                CaretAssertVectorIndex(data, ix);
                data[ix] = value;
            }
            
            const float* dataValues = &data[0];
            float* dataRGBA = &rgbaOut[0];
            
            NodeAndVoxelColoring::colorScalarsWithPalette(statistics,
                                                          paletteColorMapping,
                                                          palette,
                                                          dataValues,
                                                          dataValues,
                                                          numDataValues,
                                                          dataRGBA,
                                                          true); // ignore thresholding
        }
    }
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
    
    
    //    /*
    //     * Margin is region around the chart in which
    //     * the axes legends, values, and ticks are drawn.
    //     */
    const double marginSize = 30;
    Margins margins(marginSize);
    //
    //    double width, height;
    //    estimateCartesianChartAxisLegendsWidthHeight(textRenderer, vpHeight, cartesianChart->getLeftAxis(), width, height);
    //    margins.m_left = std::max(margins.m_left, width);
    //    estimateCartesianChartAxisLegendsWidthHeight(textRenderer, vpHeight, cartesianChart->getRightAxis(), width, height);
    //    margins.m_right = std::max(margins.m_right, width);
    //    estimateCartesianChartAxisLegendsWidthHeight(textRenderer, vpHeight, cartesianChart->getTopAxis(), width, height);
    //    margins.m_top = std::max(margins.m_top, height);
    //    estimateCartesianChartAxisLegendsWidthHeight(textRenderer, vpHeight, cartesianChart->getBottomAxis(), width, height);
    //    margins.m_bottom = std::max(margins.m_bottom, height);
    //
    //    if (margins.m_left > marginSize) margins.m_left += 10;
    //    if (margins.m_right > marginSize) margins.m_right += 10;
    //
    //    /*
    //     * Ensure that there is sufficient space for the axes data display.
    //     */
    //    if ((vpWidth > (marginSize * 3))
    //        && (vpHeight > (marginSize * 3))) {
    //
    //        /* Draw legends and grids */
    //        drawChartAxis(vpX,
    //                      vpY,
    //                      vpWidth,
    //                      vpHeight,
    //                      margins,
    //                      textRenderer,
    //                      cartesianChart->getLeftAxis());
    //
    //        drawChartAxis(vpX,
    //                      vpY,
    //                      vpWidth,
    //                      vpHeight,
    //                      margins,
    //                      textRenderer,
    //                      cartesianChart->getRightAxis());
    //
    //        drawChartAxis(vpX,
    //                      vpY,
    //                      vpWidth,
    //                      vpHeight,
    //                      margins,
    //                      textRenderer,
    //                      cartesianChart->getBottomAxis());
    //
    //        drawChartAxis(vpX,
    //                      vpY,
    //                      vpWidth,
    //                      vpHeight,
    //                      margins,
    //                      textRenderer,
    //                      cartesianChart->getTopAxis());
    //
    //
    //        drawChartGraphicsBoxAndSetViewport(vpX,
    //                                           vpY,
    //                                           vpWidth,
    //                                           vpHeight,
    //                                           margins,
    //                                           chartGraphicsDrawingViewport);
    //    }
    
    
    const int32_t numberOfOverlays = m_chartOverlaySet->getNumberOfDisplayedOverlays();
    CaretAssert(numberOfOverlays > 0);
    const ChartTwoOverlay* topOverlay = m_chartOverlaySet->getOverlay(0);
    const ChartTwoCompoundDataType cdt = topOverlay->getChartTwoCompoundDataType();
    CaretAssert(cdt.getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM);
    
    std::vector<HistogramDrawingInfo*> histogramDrawingInfoVector;
    std::vector<const ChartableTwoFileHistogramChart*> histogramChartVector;
    std::vector<int32_t> mapIndexVector;
    /*
     * Get the histogram drawing information and overall extent
     */
    float xMin =   0.0;
    float xMax =   0.0;
    float yMin =   0.0;
    float yMax =   0.0;
    for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
        ChartTwoOverlay* chartOverlay = m_chartOverlaySet->getOverlay(iOverlay);
        if ( ! chartOverlay->isEnabled()) {
            continue;
        }
        
        CaretMappableDataFile* mapFile = NULL;
        int32_t mapIndex = 1;
        chartOverlay->getSelectionData(mapFile,
                                       mapIndex);
        if (mapFile == NULL) {
            continue;
        }
        
        const ChartableTwoFileDelegate* chartDelegate        = mapFile->getChartingDelegate();
        const ChartableTwoFileHistogramChart* histogramChart = chartDelegate->getHistogramCharting();
        histogramChartVector.push_back(histogramChart);
        mapIndexVector.push_back(mapIndex);
        histogramDrawingInfoVector.push_back(new HistogramDrawingInfo());
        const int32_t histoIndex = static_cast<int32_t>(histogramDrawingInfoVector.size() - 1);
        
        if (histogramChart->isValid()) {
            AString errorMessage;
            CaretAssertVectorIndex(histogramDrawingInfoVector, histoIndex);
            if (mapFile->getMapHistogramDrawingInfo(mapIndex,
                                                       false,
                                                       *histogramDrawingInfoVector[histoIndex],
                                                       errorMessage)) {
                float bounds[4];
                if (histogramDrawingInfoVector[histoIndex]->getBounds(bounds)) {
                    xMin = std::min(xMin, bounds[0]);
                    xMax = std::max(xMax, bounds[1]);
                    yMin = std::min(yMin, bounds[2]);
                    yMax = std::max(yMax, bounds[3]);
                }
            }
            else {
                CaretLogWarning(errorMessage + mapFile->getFileName());
            }
        }
    }
    CaretAssert(histogramDrawingInfoVector.size() == histogramChartVector.size());
    CaretAssert(histogramDrawingInfoVector.size() == mapIndexVector.size());
    
    
//    /*
//     * Get extent of histogram data
//     */
//    float xMin =   0.0;
//    float xMax =   0.0;
//    float yMin =   0.0;
//    float yMax =   0.0;
//    for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
//        ChartTwoOverlay* chartOverlay = m_chartOverlaySet->getOverlay(iOverlay);
//        if ( ! chartOverlay->isEnabled()) {
//            continue;
//        }
//        
//        CaretMappableDataFile* mapFile = NULL;
//        int32_t mapIndex = 1;
//        chartOverlay->getSelectionData(mapFile,
//                                       mapIndex);
//        if (mapFile == NULL) {
//            continue;
//        }
//        
//        const ChartableTwoFileDelegate* chartDelegate        = mapFile->getChartingDelegate();
//        const ChartableTwoFileHistogramChart* histogramChart = chartDelegate->getHistogramCharting();
//        if (histogramChart->isValid()) {
//            const ChartTwoDataHistogram* histogramData = histogramChart->getMapHistogramChart(mapIndex);
//            float bounds[4];
//            if (histogramData->getBounds(bounds)) {
//                xMin = std::min(xMin, bounds[0]);
//                xMax = std::max(xMax, bounds[1]);
//                yMin = std::min(yMin, bounds[2]);
//                yMax = std::max(yMax, bounds[3]);
//            }
//        }
//    }
    
    /*
     * Bounds invalid?
     */
    if ((xMin < xMax)
        || (yMin < yMax)) {
        //    std::cout << "All bounds: "
        //    << xMin << " "
        //    << xMax << " "
        //    << yMin << " "
        //    << yMax << " " << std::endl;
        
        glViewport(chartGraphicsDrawingViewport[0],
                   chartGraphicsDrawingViewport[1],
                   chartGraphicsDrawingViewport[2],
                   chartGraphicsDrawingViewport[3]);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(xMin, xMax,
                yMin, yMax,
                -10.0, 10.0);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        bool applyTransformationsFlag = true;
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
        
        const int32_t numToDraw = static_cast<int32_t>(histogramChartVector.size());
        for (int32_t i = 0; i < numToDraw; i++) {
            CaretAssertVectorIndex(histogramChartVector, i);
            CaretAssertVectorIndex(mapIndexVector, i);
            CaretAssertVectorIndex(histogramDrawingInfoVector, i);
            if (histogramDrawingInfoVector[i]->isValid()) {
                drawHistogramChartContent(histogramChartVector[i],
                                          mapIndexVector[i],
                                          *histogramDrawingInfoVector[i]);
            }
        }
    }
    
    
    for (auto histDrawPtr : histogramDrawingInfoVector) {
        delete histDrawPtr;
    }
    histogramDrawingInfoVector.clear();
    
//    int32_t layerIndex = 0;
//    for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
//        ChartTwoOverlay* chartOverlay = m_chartOverlaySet->getOverlay(iOverlay);
//        if ( ! chartOverlay->isEnabled()) {
//            continue;
//        }
//        
//        CaretMappableDataFile* mapFile = NULL;
//        int32_t mapIndex = 1;
//        chartOverlay->getSelectionData(mapFile,
//                                       mapIndex);
//        if (mapFile == NULL) {
//            continue;
//        }
//        
//        const ChartableTwoFileDelegate* chartDelegate        = mapFile->getChartingDelegate();
//        const ChartableTwoFileHistogramChart* histogramChart = chartDelegate->getHistogramCharting();
//        if (histogramChart->isValid()) {
//            drawHistogramChartContent(histogramChart,
//                                      mapIndex);
//            layerIndex++;
//        }
//    }
}

/*
 * Draw the given histogram chart.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawHistogramChartContent(const ChartableTwoFileHistogramChart* histogramChart,
                                                                   const int32_t mapIndex,
                                                                   const HistogramDrawingInfo& histogramDrawingInfo)
{
//    const ChartTwoDataHistogram* histogramData = histogramChart->getMapHistogramChart(mapIndex);
//    CaretMappableDataFile* mapFile = const_cast<CaretMappableDataFile*>(histogramChart->getCaretMappableDataFile());
//    CaretAssert(mapFile);
//    
//    HistogramDrawingInfo histogramDrawingInfo;
//    AString errorMessage;
//    if ( ! mapFile->getMapHistogramDrawingInfo(mapIndex,
//                                               false,
//                                               histogramDrawingInfo,
//                                               errorMessage)) {
//        CaretLogWarning(errorMessage + mapFile->getFileName());
//        return;
//    }
    
    const ChartTwoDataHistogram* histogramData = histogramChart->getMapHistogramChart(mapIndex);

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
    
    //    m_identificationModeFlag = false;
    //    switch (m_fixedPipelineDrawing->mode) {
    //        case BrainOpenGLFixedPipeline::MODE_DRAWING:
    //            break;
    //        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
    //            if (m_selectionItemMatrix->isEnabledForSelection()) {
    //                m_identificationModeFlag = true;
    //                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //            }
    //            else {
    //                return;
    //            }
    //            break;
    //        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
    //            return;
    //            break;
    //    }
    
    
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(bounds[0], bounds[2], 0.0);
    glVertex3f(bounds[1], bounds[3], 0.0);
    glEnd();
    
    const std::vector<float>& xValues = histogramDrawingInfo.getDataX();
    const std::vector<float>& yValues = histogramDrawingInfo.getDataY();
    const std::vector<float>& rgbaValues = histogramDrawingInfo.getDataRGBA();
    CaretAssert(xValues.size() == yValues.size());
    CaretAssert((xValues.size() * 4) == rgbaValues.size());
    
    const int32_t numValues = static_cast<int32_t>(xValues.size() - 1);
    if (numValues > 1) {
        if ( ! m_identificationModeFlag) {
            bool solidColorFlag = false;
            if (solidColorFlag) {
                
            }
        }
        std::vector<float> quadVerticesXYZ;
        std::vector<float> quadVerticesFloatRGBA;
        std::vector<uint8_t> quadVerticesByteRGBA;
        
        switch (histogramData->getHistogramViewingType()) {
            case ChartTwoHistogramViewingTypeEnum::HISTOGRAM_VIEWING_BARS:
            {
                /*
                 * Reserve to prevent reszing of vectors
                 * as elements are added.
                 */
                const int32_t verticesPerBar = 4;
                const int32_t totalVertices  = verticesPerBar * numValues;
                quadVerticesXYZ.reserve(totalVertices * 3);
                quadVerticesFloatRGBA.reserve(totalVertices * 4);
                quadVerticesByteRGBA.reserve(totalVertices * 4);
                
                const float z = 0.;
                
                for (int32_t i = 0; i < numValues; i++) {
                    CaretAssertVectorIndex(xValues, i + 1);
                    float left   = xValues[i];
                    float right  = xValues[i+1];
                    float bottom = 0;
                    CaretAssertVectorIndex(yValues, i);
                    float top = yValues[i];
                    
                    quadVerticesXYZ.push_back(left);
                    quadVerticesXYZ.push_back(bottom);
                    quadVerticesXYZ.push_back(z);
                    quadVerticesXYZ.push_back(right);
                    quadVerticesXYZ.push_back(bottom);
                    quadVerticesXYZ.push_back(z);
                    quadVerticesXYZ.push_back(right);
                    quadVerticesXYZ.push_back(top);
                    quadVerticesXYZ.push_back(z);
                    quadVerticesXYZ.push_back(left);
                    quadVerticesXYZ.push_back(top);
                    quadVerticesXYZ.push_back(z);
                    
                    if (m_identificationModeFlag) {
                        uint8_t idRGBA[4];
                        addToHistogramIdentification(i, idRGBA);
                        
                        for (int32_t iRGB = 0; iRGB < verticesPerBar; iRGB++) {
                            quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                        idRGBA, idRGBA + 4);
                        }
                    }
                    else {
                        CaretAssertVectorIndex(rgbaValues, i*4 + 3);
                        const float* rgba = &rgbaValues[i * 4];
                        for (int32_t iRGB = 0; iRGB < verticesPerBar; iRGB++) {
                            quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                         rgba, rgba + 4);
                        }
                    }
                    
                }
                
                /*
                 * Draw the bar elements.
                 */
                if (m_identificationModeFlag) {
                    CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesByteRGBA.size() / 4));
                    const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
                    glBegin(GL_QUADS);
                    for (int32_t i = 0; i < numberQuadVertices; i++) {
                        CaretAssertVectorIndex(quadVerticesByteRGBA, i*4 + 3);
                        glColor4ubv(&quadVerticesByteRGBA[i*4]);
                        CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
                        glVertex3fv(&quadVerticesXYZ[i*3]);
                    }
                    glEnd();
                }
                else {
                    CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesFloatRGBA.size() / 4));
                    const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
                    glBegin(GL_QUADS);
                    for (int32_t i = 0; i < numberQuadVertices; i++) {
                        CaretAssertVectorIndex(quadVerticesFloatRGBA, i*4 + 3);
                        glColor4fv(&quadVerticesFloatRGBA[i*4]);
                        CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
                        glVertex3fv(&quadVerticesXYZ[i*3]);
                    }
                    glEnd();
                }
            }
                break;
            case ChartTwoHistogramViewingTypeEnum::HISTOGRAM_VIEWING_ENVELOPE:
            {
                /*
                 * Reserve to prevent reszing of vectors
                 * as elements are added.
                 */
                const int32_t verticesPerBar = 2;
                const int32_t totalVertices  = verticesPerBar * numValues + 4;
                quadVerticesXYZ.reserve(totalVertices * 3);
                quadVerticesFloatRGBA.reserve(totalVertices * 4);
                quadVerticesByteRGBA.reserve(totalVertices * 4);
                
                const float z = 0.0;
                
                const int32_t lastValueIndex = numValues - 1;
                for (int32_t i = 0; i < numValues; i++) {
                    CaretAssertVectorIndex(xValues, i + 1);
                    float left   = xValues[i];
                    float right  = xValues[i + 1];
                    float bottom = 0;
                    CaretAssertVectorIndex(yValues, i);
                    float top = yValues[i];
                    
                    CaretAssertVectorIndex(rgbaValues, i*4 + 3);
                    const float* rgba = &rgbaValues[i * 4];
                    
                    uint8_t idRGBA[4];
                    if (m_identificationModeFlag) {
                        addToHistogramIdentification(i, idRGBA);
                    }
                    
                    if (i == 0) {
                        /*
                         * Left side of first bar
                         */
                        quadVerticesXYZ.push_back(left);
                        quadVerticesXYZ.push_back(bottom);
                        quadVerticesXYZ.push_back(z);
                        if (m_identificationModeFlag) {
                            quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                        idRGBA, idRGBA + 4);
                        }
                        else {
                            quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                         rgba, rgba + 4);
                        }
                        quadVerticesXYZ.push_back(left);
                        quadVerticesXYZ.push_back(top);
                        quadVerticesXYZ.push_back(z);
                        if (m_identificationModeFlag) {
                            quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                        idRGBA, idRGBA + 4);
                        }
                        else {
                            quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                         rgba, rgba + 4);
                        }
                    }
                    else {
                        if (top > yValues[i - 1]) {
                            /*
                             * Line from previous bar that is lower in
                             * height than this bar
                             */
                            quadVerticesXYZ.push_back(left);
                            quadVerticesXYZ.push_back(yValues[i - 1]);
                            quadVerticesXYZ.push_back(z);
                            if (m_identificationModeFlag) {
                                quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                            idRGBA, idRGBA + 4);
                            }
                            else {
                                quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                             rgba, rgba + 4);
                            }
                            
                            quadVerticesXYZ.push_back(left);
                            quadVerticesXYZ.push_back(top);
                            quadVerticesXYZ.push_back(z);
                            if (m_identificationModeFlag) {
                                quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                            idRGBA, idRGBA + 4);
                            }
                            else {
                                quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                             rgba, rgba + 4);
                            }
                        }
                    }
                    
                    /*
                     * Draw line across top
                     */
                    quadVerticesXYZ.push_back(left);
                    quadVerticesXYZ.push_back(top);
                    quadVerticesXYZ.push_back(z);
                    if (m_identificationModeFlag) {
                        quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                    idRGBA, idRGBA + 4);
                    }
                    else {
                        quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                     rgba, rgba + 4);
                    }
                    quadVerticesXYZ.push_back(right);
                    quadVerticesXYZ.push_back(top);
                    quadVerticesXYZ.push_back(z);
                    if (m_identificationModeFlag) {
                        quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                    idRGBA, idRGBA + 4);
                    }
                    else {
                        quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                     rgba, rgba + 4);
                    }
                    
                    if (i == lastValueIndex) {
                        /*
                         * Right side of last bar
                         */
                        quadVerticesXYZ.push_back(right);
                        quadVerticesXYZ.push_back(top);
                        quadVerticesXYZ.push_back(z);
                        if (m_identificationModeFlag) {
                            quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                        idRGBA, idRGBA + 4);
                        }
                        else {
                            quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                         rgba, rgba + 4);
                        }
                        quadVerticesXYZ.push_back(right);
                        quadVerticesXYZ.push_back(bottom);
                        quadVerticesXYZ.push_back(z);
                        if (m_identificationModeFlag) {
                            quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                        idRGBA, idRGBA + 4);
                        }
                        else {
                            quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                         rgba, rgba + 4);
                        }
                    }
                    else {
                        if (top > yValues[i + 1]) {
                            /*
                             * Line from bar down to next bar
                             * with a lower height
                             */
                            quadVerticesXYZ.push_back(right);
                            quadVerticesXYZ.push_back(top);
                            quadVerticesXYZ.push_back(z);
                            if (m_identificationModeFlag) {
                                quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                            idRGBA, idRGBA + 4);
                            }
                            else {
                                quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                             rgba, rgba + 4);
                            }
                            quadVerticesXYZ.push_back(right);
                            quadVerticesXYZ.push_back(yValues[i + 1]);
                            quadVerticesXYZ.push_back(z);
                            if (m_identificationModeFlag) {
                                quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                            idRGBA, idRGBA + 4);
                            }
                            else {
                                quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                             rgba, rgba + 4);
                            }
                        }
                    }
                }
                
                /*
                 * Draw the line elements.
                 */
                if (m_identificationModeFlag) {
                    CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesByteRGBA.size() / 4));
                    const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
                    m_fixedPipelineDrawing->setLineWidth(5);
                    glBegin(GL_LINES);
                    for (int32_t i = 0; i < numberQuadVertices; i++) {
                        CaretAssertVectorIndex(quadVerticesByteRGBA, i*4 + 3);
                        glColor4ubv(&quadVerticesByteRGBA[i*4]);
                        CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
                        glVertex3fv(&quadVerticesXYZ[i*3]);
                    }
                    glEnd();
                }
                else {
                    m_fixedPipelineDrawing->setLineWidth(2);
                    CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesFloatRGBA.size() / 4));
                    const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
                    glBegin(GL_LINES);
                    for (int32_t i = 0; i < numberQuadVertices; i++) {
                        CaretAssertVectorIndex(quadVerticesFloatRGBA, i*4 + 3);
                        glColor4fv(&quadVerticesFloatRGBA[i*4]);
                        CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
                        glVertex3fv(&quadVerticesXYZ[i*3]);
                    }
                    glEnd();
                }
            }
                break;
        }
        
    }
    
    if (m_identificationModeFlag) {
        processHistogramIdentification(histogramChart);
    }
}


/*
 * Draw the given histogram chart.
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::drawHistogramChartContent(const ChartableTwoFileHistogramChart* histogramChart,
                                                                   const int32_t mapIndex)
{
    const ChartTwoDataHistogram* histogramData = histogramChart->getMapHistogramChart(mapIndex);
    
    if (m_identificationModeFlag) {
        resetIdentification();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
//    m_identificationModeFlag = false;
//    switch (m_fixedPipelineDrawing->mode) {
//        case BrainOpenGLFixedPipeline::MODE_DRAWING:
//            break;
//        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
//            if (m_selectionItemMatrix->isEnabledForSelection()) {
//                m_identificationModeFlag = true;
//                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            }
//            else {
//                return;
//            }
//            break;
//        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
//            return;
//            break;
//    }

    float bounds[4];
    if ( ! histogramData->getBounds(bounds)) {
        return;
    }
    
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(bounds[0], bounds[2], 0.0);
    glVertex3f(bounds[1], bounds[3], 0.0);
    glEnd();
    
    const std::vector<float> heightValues = histogramData->getHistogramValues();
    const int32_t numValues = static_cast<int32_t>(heightValues.size());
    if (numValues > 1) {
        std::vector<float> dataColoringVectorRGBA;
        if ( ! m_identificationModeFlag) {
            bool solidColorFlag = false;
            if (solidColorFlag) {
                
            }
            else {
                colorHistogramWithPalette(histogramChart,
                                          mapIndex,
                                          dataColoringVectorRGBA);
                const size_t rgbaSize = numValues * 4;
                CaretAssert(dataColoringVectorRGBA.size() == rgbaSize);
            }
        }
        std::vector<float> quadVerticesXYZ;
        std::vector<float> quadVerticesFloatRGBA;
        std::vector<uint8_t> quadVerticesByteRGBA;
        
        switch (histogramData->getHistogramViewingType()) {
            case ChartTwoHistogramViewingTypeEnum::HISTOGRAM_VIEWING_BARS:
            {
                /*
                 * Reserve to prevent reszing of vectors 
                 * as elements are added.
                 */
                const int32_t verticesPerBar = 4;
                const int32_t totalVertices  = verticesPerBar * numValues;
                quadVerticesXYZ.reserve(totalVertices * 3);
                quadVerticesFloatRGBA.reserve(totalVertices * 4);
                quadVerticesByteRGBA.reserve(totalVertices * 4);
                
                const float z = 0.;
                
                for (int32_t i = 0; i < numValues; i++) {
                    float left   = i;
                    float right  = i + 1;
                    float bottom = 0;
                    CaretAssertVectorIndex(heightValues, i);
                    float top = heightValues[i];
                    
                    quadVerticesXYZ.push_back(left);
                    quadVerticesXYZ.push_back(bottom);
                    quadVerticesXYZ.push_back(z);
                    quadVerticesXYZ.push_back(right);
                    quadVerticesXYZ.push_back(bottom);
                    quadVerticesXYZ.push_back(z);
                    quadVerticesXYZ.push_back(right);
                    quadVerticesXYZ.push_back(top);
                    quadVerticesXYZ.push_back(z);
                    quadVerticesXYZ.push_back(left);
                    quadVerticesXYZ.push_back(top);
                    quadVerticesXYZ.push_back(z);
                    
                    if (m_identificationModeFlag) {
                        uint8_t idRGBA[4];
                        addToHistogramIdentification(i, idRGBA);

                        for (int32_t iRGB = 0; iRGB < verticesPerBar; iRGB++) {
                            quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                        idRGBA, idRGBA + 4);
                        }
                    }
                    else {
                        CaretAssertVectorIndex(dataColoringVectorRGBA, i*4 + 3);
                        const float* rgba = &dataColoringVectorRGBA[i * 4];
                        for (int32_t iRGB = 0; iRGB < verticesPerBar; iRGB++) {
                            quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                         rgba, rgba + 4);
                        }
                    }
                   
                }
                
                /*
                 * Draw the bar elements.
                 */
                if (m_identificationModeFlag) {
                    CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesByteRGBA.size() / 4));
                    const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
                    glBegin(GL_QUADS);
                    for (int32_t i = 0; i < numberQuadVertices; i++) {
                        CaretAssertVectorIndex(quadVerticesByteRGBA, i*4 + 3);
                        glColor4ubv(&quadVerticesByteRGBA[i*4]);
                        CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
                        glVertex3fv(&quadVerticesXYZ[i*3]);
                    }
                    glEnd();
                }
                else {
                    CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesFloatRGBA.size() / 4));
                    const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
                    glBegin(GL_QUADS);
                    for (int32_t i = 0; i < numberQuadVertices; i++) {
                        CaretAssertVectorIndex(quadVerticesFloatRGBA, i*4 + 3);
                        glColor4fv(&quadVerticesFloatRGBA[i*4]);
                        CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
                        glVertex3fv(&quadVerticesXYZ[i*3]);
                    }
                    glEnd();
                }
            }
                break;
            case ChartTwoHistogramViewingTypeEnum::HISTOGRAM_VIEWING_ENVELOPE:
            {
                /*
                 * Reserve to prevent reszing of vectors
                 * as elements are added.
                 */
                const int32_t verticesPerBar = 2;
                const int32_t totalVertices  = verticesPerBar * numValues + 4;
                quadVerticesXYZ.reserve(totalVertices * 3);
                quadVerticesFloatRGBA.reserve(totalVertices * 4);
                quadVerticesByteRGBA.reserve(totalVertices * 4);
                
                const float z = 0.0;
                
                const int32_t lastValueIndex = numValues - 1;
                for (int32_t i = 0; i < numValues; i++) {
                    float left   = i;
                    float right  = i + 1;
                    float bottom = 0;
                    CaretAssertVectorIndex(heightValues, i);
                    float top = heightValues[i];
                    
                    float rgba[4] = { 1.0, 0.5, 0.0, 1.0 };
                    
                    uint8_t idRGBA[4];
                    if (m_identificationModeFlag) {
                        addToHistogramIdentification(i, idRGBA);
                    }
                    
                    if (i == 0) {
                        /*
                         * Left side of first bar
                         */
                        quadVerticesXYZ.push_back(left);
                        quadVerticesXYZ.push_back(bottom);
                        quadVerticesXYZ.push_back(z);
                        if (m_identificationModeFlag) {
                            quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                        idRGBA, idRGBA + 4);
                        }
                        else {
                            quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                         rgba, rgba + 4);
                        }
                        quadVerticesXYZ.push_back(left);
                        quadVerticesXYZ.push_back(top);
                        quadVerticesXYZ.push_back(z);
                        if (m_identificationModeFlag) {
                            quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                        idRGBA, idRGBA + 4);
                        }
                        else {
                            quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                         rgba, rgba + 4);
                        }
                    }
                    else {
                        if (top > heightValues[i - 1]) {
                            /*
                             * Line from previous bar that is lower in 
                             * height than this bar
                             */
                            quadVerticesXYZ.push_back(left);
                            quadVerticesXYZ.push_back(heightValues[i - 1]);
                            quadVerticesXYZ.push_back(z);
                            if (m_identificationModeFlag) {
                                quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                            idRGBA, idRGBA + 4);
                            }
                            else {
                                quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                             rgba, rgba + 4);
                            }

                            quadVerticesXYZ.push_back(left);
                            quadVerticesXYZ.push_back(top);
                            quadVerticesXYZ.push_back(z);
                            if (m_identificationModeFlag) {
                                quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                            idRGBA, idRGBA + 4);
                            }
                            else {
                                quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                             rgba, rgba + 4);
                            }
                        }
                    }
                    
                    /*
                     * Draw line across top
                     */
                    quadVerticesXYZ.push_back(left);
                    quadVerticesXYZ.push_back(top);
                    quadVerticesXYZ.push_back(z);
                    if (m_identificationModeFlag) {
                        quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                    idRGBA, idRGBA + 4);
                    }
                    else {
                        quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                     rgba, rgba + 4);
                    }
                    quadVerticesXYZ.push_back(right);
                    quadVerticesXYZ.push_back(top);
                    quadVerticesXYZ.push_back(z);
                    if (m_identificationModeFlag) {
                        quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                    idRGBA, idRGBA + 4);
                    }
                    else {
                        quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                     rgba, rgba + 4);
                    }

                    if (i == lastValueIndex) {
                        /*
                         * Right side of last bar
                         */
                        quadVerticesXYZ.push_back(right);
                        quadVerticesXYZ.push_back(top);
                        quadVerticesXYZ.push_back(z);
                        if (m_identificationModeFlag) {
                            quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                        idRGBA, idRGBA + 4);
                        }
                        else {
                            quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                         rgba, rgba + 4);
                        }
                        quadVerticesXYZ.push_back(right);
                        quadVerticesXYZ.push_back(bottom);
                        quadVerticesXYZ.push_back(z);
                        if (m_identificationModeFlag) {
                            quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                        idRGBA, idRGBA + 4);
                        }
                        else {
                            quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                         rgba, rgba + 4);
                        }
                    }
                    else {
                        if (top > heightValues[i + 1]) {
                            /*
                             * Line from bar down to next bar
                             * with a lower height
                             */
                            quadVerticesXYZ.push_back(right);
                            quadVerticesXYZ.push_back(top);
                            quadVerticesXYZ.push_back(z);
                            if (m_identificationModeFlag) {
                                quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                            idRGBA, idRGBA + 4);
                            }
                            else {
                                quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                             rgba, rgba + 4);
                            }
                            quadVerticesXYZ.push_back(right);
                            quadVerticesXYZ.push_back(heightValues[i + 1]);
                            quadVerticesXYZ.push_back(z);
                            if (m_identificationModeFlag) {
                                quadVerticesByteRGBA.insert(quadVerticesByteRGBA.end(),
                                                            idRGBA, idRGBA + 4);
                            }
                            else {
                                quadVerticesFloatRGBA.insert(quadVerticesFloatRGBA.end(),
                                                             rgba, rgba + 4);
                            }
                        }
                    }
                }
                
                /*
                 * Draw the line elements.
                 */
                if (m_identificationModeFlag) {
                    CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesByteRGBA.size() / 4));
                    const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
                    m_fixedPipelineDrawing->setLineWidth(5);
                    glBegin(GL_LINES);
                    for (int32_t i = 0; i < numberQuadVertices; i++) {
                        CaretAssertVectorIndex(quadVerticesByteRGBA, i*4 + 3);
                        glColor4ubv(&quadVerticesByteRGBA[i*4]);
                        CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
                        glVertex3fv(&quadVerticesXYZ[i*3]);
                    }
                    glEnd();
                }
                else {
                    m_fixedPipelineDrawing->setLineWidth(2);
                    CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesFloatRGBA.size() / 4));
                    const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
                    glBegin(GL_LINES);
                    for (int32_t i = 0; i < numberQuadVertices; i++) {
                        CaretAssertVectorIndex(quadVerticesFloatRGBA, i*4 + 3);
                        glColor4fv(&quadVerticesFloatRGBA[i*4]);
                        CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
                        glVertex3fv(&quadVerticesXYZ[i*3]);
                    }
                    glEnd();
                }
            }
                break;
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
        int32_t mapIndex = 1;
        chartOverlay->getSelectionData(mapFile,
                                       mapIndex);
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

///**
// * Get highlighting of rows or columns for a matrix.
// *
// * @param matrixChart
// *     Matrix chart that is drawn.
// * @param rowIndicesOut
// *     Output with row indices for highlighting.
// * @param columnIndicesOut
// *     Output with column indices for highlighting.
// */
//void
//BrainOpenGLChartTwoDrawingFixedPipeline::getMatrixHighlighting(const ChartableTwoFileMatrixChart* matrixChart,
//                                                               std::set<int32_t>& rowIndicesOut,
//                                                               std::set<int32_t>& columnIndicesOut) const
//{
//    const CiftiMappableDataFile* ciftiMapFile = matrixChart->getCiftiMappableDataFile();
//    const CiftiMappableConnectivityMatrixDataFile* connMapFile = dynamic_cast<const CiftiMappableConnectivityMatrixDataFile*>(ciftiMapFile);
//    if (connMapFile != NULL) {
//        const ConnectivityDataLoaded* connDataLoaded = connMapFile->getConnectivityDataLoaded();
//        if (connDataLoaded != NULL) {
//            int64_t loadedRowIndex = -1;
//            int64_t loadedColumnIndex = -1;
//            connDataLoaded->getRowColumnLoading(loadedRowIndex,
//                                                loadedColumnIndex);
//            if (loadedRowIndex >= 0) {
//                rowIndicesOut.insert(loadedRowIndex);
//            }
//            else if (loadedColumnIndex >= 0) {
//                columnIndicesOut.insert(loadedColumnIndex);
//            }
//        }
//    }
//    
//    const CiftiParcelScalarFile* parcelScalarFile = dynamic_cast<const CiftiParcelScalarFile*>(ciftiMapFile);
//    if (parcelScalarFile != NULL) {
//        EventCaretMappableDataFileMapsViewedInOverlays mapOverlayEvent(parcelScalarFile);
//        EventManager::get()->sendEvent(mapOverlayEvent.getPointer());
//        columnIndicesOut = mapOverlayEvent.getSelectedMapIndices();
//    }
//    
//    const CiftiParcelLabelFile* parcelLabelFile = dynamic_cast<const CiftiParcelLabelFile*>(ciftiMapFile);
//    if (parcelLabelFile != NULL) {
//        EventCaretMappableDataFileMapsViewedInOverlays mapOverlayEvent(parcelLabelFile);
//        EventManager::get()->sendEvent(mapOverlayEvent.getPointer());
//        columnIndicesOut = mapOverlayEvent.getSelectedMapIndices();
//    }
//    
//    const CiftiScalarDataSeriesFile* scalarDataSeriesFile = dynamic_cast<const CiftiScalarDataSeriesFile*>(ciftiMapFile);
//    if (scalarDataSeriesFile != NULL) {
//        const int32_t scalarDataSeriesMapIndex = scalarDataSeriesFile->getSelectedMapIndex(m_tabIndex);
//        if (scalarDataSeriesMapIndex >= 0) {
//            rowIndicesOut.insert(scalarDataSeriesMapIndex);
//        }
//    }
//}

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
    if (m_identificationModeFlag) {
        resetIdentification();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
//    m_identificationModeFlag = false;
//    switch (m_fixedPipelineDrawing->mode) {
//        case BrainOpenGLFixedPipeline::MODE_DRAWING:
//            break;
//        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
//            if (m_selectionItemMatrix->isEnabledForSelection()) {
//                m_identificationModeFlag = true;
//                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            }
//            else {
//                return;
//            }
//            break;
//        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
//            return;
//            break;
//    }
    
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
    const float highlightRGB[3] = {
        highlightRGBByte[0] / 255.0,
        highlightRGBByte[1] / 255.0,
        highlightRGBByte[2] / 255.0
    };

    int32_t rgbaOffset = 0;
    std::vector<float> quadVerticesXYZ;
    quadVerticesXYZ.reserve(numberOfRows * numberOfColumns * 3);
    std::vector<float> quadVerticesFloatRGBA;
    quadVerticesFloatRGBA.reserve(numberOfRows * numberOfColumns * 4);
    std::vector<uint8_t> quadVerticesByteRGBA;
    quadVerticesByteRGBA.reserve(numberOfRows * numberOfColumns * 4);
    
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
                    quadVerticesByteRGBA.push_back(idRGBA[0]);
                    quadVerticesByteRGBA.push_back(idRGBA[1]);
                    quadVerticesByteRGBA.push_back(idRGBA[2]);
                    quadVerticesByteRGBA.push_back(idRGBA[3]);
                }
                else {
                    quadVerticesFloatRGBA.push_back(rgba[0]);
                    quadVerticesFloatRGBA.push_back(rgba[1]);
                    quadVerticesFloatRGBA.push_back(rgba[2]);
                    quadVerticesFloatRGBA.push_back(rgba[3]);
                }
                quadVerticesXYZ.push_back(cellX);
                quadVerticesXYZ.push_back(cellY);
                quadVerticesXYZ.push_back(0.0);
                
                if (m_identificationModeFlag) {
                    quadVerticesByteRGBA.push_back(idRGBA[0]);
                    quadVerticesByteRGBA.push_back(idRGBA[1]);
                    quadVerticesByteRGBA.push_back(idRGBA[2]);
                    quadVerticesByteRGBA.push_back(idRGBA[3]);
                }
                else {
                    quadVerticesFloatRGBA.push_back(rgba[0]);
                    quadVerticesFloatRGBA.push_back(rgba[1]);
                    quadVerticesFloatRGBA.push_back(rgba[2]);
                    quadVerticesFloatRGBA.push_back(rgba[3]);
                }
                quadVerticesXYZ.push_back(cellX + cellWidth);
                quadVerticesXYZ.push_back(cellY);
                quadVerticesXYZ.push_back(0.0);
                
                if (m_identificationModeFlag) {
                    quadVerticesByteRGBA.push_back(idRGBA[0]);
                    quadVerticesByteRGBA.push_back(idRGBA[1]);
                    quadVerticesByteRGBA.push_back(idRGBA[2]);
                    quadVerticesByteRGBA.push_back(idRGBA[3]);
                }
                else {
                    quadVerticesFloatRGBA.push_back(rgba[0]);
                    quadVerticesFloatRGBA.push_back(rgba[1]);
                    quadVerticesFloatRGBA.push_back(rgba[2]);
                    quadVerticesFloatRGBA.push_back(rgba[3]);
                }
                quadVerticesXYZ.push_back(cellX + cellWidth);
                quadVerticesXYZ.push_back(cellY + cellHeight);
                quadVerticesXYZ.push_back(0.0);
                
                if (m_identificationModeFlag) {
                    quadVerticesByteRGBA.push_back(idRGBA[0]);
                    quadVerticesByteRGBA.push_back(idRGBA[1]);
                    quadVerticesByteRGBA.push_back(idRGBA[2]);
                    quadVerticesByteRGBA.push_back(idRGBA[3]);
                }
                else {
                    quadVerticesFloatRGBA.push_back(rgba[0]);
                    quadVerticesFloatRGBA.push_back(rgba[1]);
                    quadVerticesFloatRGBA.push_back(rgba[2]);
                    quadVerticesFloatRGBA.push_back(rgba[3]);
                }
                quadVerticesXYZ.push_back(cellX);
                quadVerticesXYZ.push_back(cellY + cellHeight);
                quadVerticesXYZ.push_back(0.0);
            }
            
            cellX += cellWidth;
        }
        
        cellY -= cellHeight;
    }
    
    /*
     * Draw the matrix elements.
     */
    if (m_identificationModeFlag) {
        CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesByteRGBA.size() / 4));
        const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
        glBegin(GL_QUADS);
        for (int32_t i = 0; i < numberQuadVertices; i++) {
            CaretAssertVectorIndex(quadVerticesByteRGBA, i*4 + 3);
            glColor4ubv(&quadVerticesByteRGBA[i*4]);
            CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
            glVertex3fv(&quadVerticesXYZ[i*3]);
        }
        glEnd();
    }
    else {
        /*
         * Enable alpha blending so voxels that are not drawn from higher layers
         * allow voxels from lower layers to be seen.
         */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesFloatRGBA.size() / 4));
        const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
        glBegin(GL_QUADS);
        for (int32_t i = 0; i < numberQuadVertices; i++) {
            CaretAssertVectorIndex(quadVerticesFloatRGBA, i*4 + 3);
            glColor4fv(&quadVerticesFloatRGBA[i*4]);
            CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
            glVertex3fv(&quadVerticesXYZ[i*3]);
        }
        glEnd();
        
        glDisable(GL_BLEND);
        
        
        /*
         * Drawn an outline around the matrix elements.
         */
        if (displayGridLinesFlag) {
            uint8_t gridLineColorBytes[3];
            m_preferences->getBackgroundAndForegroundColors()->getColorChartMatrixGridLines(gridLineColorBytes);
            float gridLineColorFloats[4];
            CaretPreferences::byteRgbToFloatRgb(gridLineColorBytes,
                                                gridLineColorFloats);
            gridLineColorFloats[3] = 1.0;
            std::vector<float> outlineRGBA;
            outlineRGBA.reserve(numberQuadVertices * 4);
            for (int32_t i = 0; i < numberQuadVertices; i++) {
                outlineRGBA.push_back(gridLineColorFloats[0]);
                outlineRGBA.push_back(gridLineColorFloats[1]);
                outlineRGBA.push_back(gridLineColorFloats[2]);
                outlineRGBA.push_back(gridLineColorFloats[3]);
            }
            
            glPolygonMode(GL_FRONT, GL_LINE);
            glLineWidth(1.0);
            glBegin(GL_QUADS);
            for (int32_t i = 0; i < numberQuadVertices; i++) {
                CaretAssertVectorIndex(outlineRGBA, i*4 + 3);
                glColor4fv(&outlineRGBA[i*4]);
                CaretAssertVectorIndex(quadVerticesXYZ, i*3 + 2);
                glVertex3fv(&quadVerticesXYZ[i*3]);
            }
            glEnd();
        }
        
        if ( (! selectedRowIndices.empty())
            && highlightSelectedRowColumnFlag) {
            std::vector<float> rowXYZ;
            std::vector<float> rowRGBA;
            
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
                
                rowXYZ.push_back(minX);
                rowXYZ.push_back(rowY);
                rowXYZ.push_back(0.0);
                rowRGBA.push_back(highlightRGB[0]);
                rowRGBA.push_back(highlightRGB[1]);
                rowRGBA.push_back(highlightRGB[2]);
                rowRGBA.push_back(1.0);
                
                rowXYZ.push_back(minX);
                rowXYZ.push_back(rowY + cellHeight);
                rowXYZ.push_back(0.0);
                rowRGBA.push_back(highlightRGB[0]);
                rowRGBA.push_back(highlightRGB[1]);
                rowRGBA.push_back(highlightRGB[2]);
                rowRGBA.push_back(1.0);
                
                rowXYZ.push_back(maxX);
                rowXYZ.push_back(rowY + cellHeight);
                rowXYZ.push_back(0.0);
                rowRGBA.push_back(highlightRGB[0]);
                rowRGBA.push_back(highlightRGB[1]);
                rowRGBA.push_back(highlightRGB[2]);
                rowRGBA.push_back(1.0);
                
                
                rowXYZ.push_back(maxX);
                rowXYZ.push_back(rowY);
                rowXYZ.push_back(0.0);
                rowRGBA.push_back(highlightRGB[0]);
                rowRGBA.push_back(highlightRGB[1]);
                rowRGBA.push_back(highlightRGB[2]);
                rowRGBA.push_back(1.0);
            }
            
            CaretAssert((rowXYZ.size() / 3) == (rowRGBA.size() / 4));
            
            const int32_t numberOfVertices = static_cast<int32_t>(rowXYZ.size() / 3);
            const int32_t numberOfQuads = numberOfVertices / 4;
            CaretAssert((numberOfQuads * 4) == numberOfVertices);
            
            /*
             * As cells get larger, increase linewidth for selected row
             */
            const float highlightLineWidth = std::max(((cellHeight * zooming) * 0.20), 3.0);
            glLineWidth(highlightLineWidth);
            
            for (int32_t iQuad = 0; iQuad < numberOfQuads; iQuad++) {
                glBegin(GL_LINE_LOOP);
                for (int32_t iVert = 0; iVert < 4; iVert++) {
                    const int32_t rgbaOffset = (iQuad * 16) + (iVert * 4);
                    CaretAssertVectorIndex(rowRGBA, rgbaOffset + 3);
                    glColor4fv(&rowRGBA[rgbaOffset]);
                    
                    const int32_t xyzOffset = (iQuad * 12) + (iVert * 3);
                    CaretAssertVectorIndex(rowXYZ, xyzOffset + 2);
                    glVertex3fv(&rowXYZ[xyzOffset]);
                }
                glEnd();
            }
            glLineWidth(1.0);
        }
        
        if ( (! selectedColumnIndices.empty())
            && highlightSelectedRowColumnFlag) {
            std::vector<float> columnXYZ;
            std::vector<float> columnRGBA;
            
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
                
                
                columnXYZ.push_back(colX);
                columnXYZ.push_back(minY);
                columnXYZ.push_back(0.0);
                columnRGBA.push_back(highlightRGB[0]);
                columnRGBA.push_back(highlightRGB[1]);
                columnRGBA.push_back(highlightRGB[2]);
                columnRGBA.push_back(1.0);
                
                columnXYZ.push_back(colX + cellWidth);
                columnXYZ.push_back(minY);
                columnXYZ.push_back(0.0);
                columnRGBA.push_back(highlightRGB[0]);
                columnRGBA.push_back(highlightRGB[1]);
                columnRGBA.push_back(highlightRGB[2]);
                columnRGBA.push_back(1.0);
                
                columnXYZ.push_back(colX + cellWidth);
                columnXYZ.push_back(maxY);
                columnXYZ.push_back(0.0);
                columnRGBA.push_back(highlightRGB[0]);
                columnRGBA.push_back(highlightRGB[1]);
                columnRGBA.push_back(highlightRGB[2]);
                columnRGBA.push_back(1.0);
                
                
                columnXYZ.push_back(colX);
                columnXYZ.push_back(maxY);
                columnXYZ.push_back(0.0);
                columnRGBA.push_back(highlightRGB[0]);
                columnRGBA.push_back(highlightRGB[1]);
                columnRGBA.push_back(highlightRGB[2]);
                columnRGBA.push_back(1.0);
            }
            
            CaretAssert((columnXYZ.size() / 3) == (columnRGBA.size() / 4));
            
            const int32_t numberOfVertices = static_cast<int32_t>(columnXYZ.size() / 3);
            const int32_t numberOfQuads = numberOfVertices / 4;
            CaretAssert((numberOfQuads * 4) == numberOfVertices);
            
            /*
             * As cells get larger, increase linewidth for selected row
             */
            const float highlightLineWidth = std::max(((cellHeight * zooming) * 0.20), 3.0);
            glLineWidth(highlightLineWidth);
            
            for (int32_t iQuad = 0; iQuad < numberOfQuads; iQuad++) {
                glBegin(GL_LINE_LOOP);
                for (int32_t iVert = 0; iVert < 4; iVert++) {
                    const int32_t rgbaOffset = (iQuad * 16) + (iVert * 4);
                    CaretAssertVectorIndex(columnRGBA, rgbaOffset + 3);
                    glColor4fv(&columnRGBA[rgbaOffset]);
                    
                    const int32_t xyzOffset = (iQuad * 12) + (iVert * 3);
                    CaretAssertVectorIndex(columnXYZ, xyzOffset + 2);
                    glVertex3fv(&columnXYZ[xyzOffset]);
                }
                glEnd();
            }
            glLineWidth(1.0);
        }
        
        glPolygonMode(GL_FRONT, GL_FILL);
    }
    
    if (m_identificationModeFlag) {
        processMatrixIdentification(matrixChart);
    }
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
 * @param barIndex
 *     Index of the histogram bar.
 * @param rgbaForColorIdentificationOut
 *    Encoded identification in RGBA color OUTPUT
 */
void
BrainOpenGLChartTwoDrawingFixedPipeline::addToHistogramIdentification(const int barIndex,
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
    m_identificationIndices.push_back(barIndex);
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
            const int32_t barIndex = m_identificationIndices[idIndex];
            
            if (m_selectionItemHistogram->isOtherScreenDepthCloserToViewer(depth)) {
                m_selectionItemHistogram->setHistogramChart(const_cast<ChartableTwoFileHistogramChart*>(histogramChart),
                                                            barIndex);
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

