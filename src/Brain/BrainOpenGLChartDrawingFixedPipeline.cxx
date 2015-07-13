
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

#include <cmath>

#define __BRAIN_OPEN_G_L_CHART_DRAWING_FIXED_PIPELINE_DECLARE__
#include "BrainOpenGLChartDrawingFixedPipeline.h"
#undef __BRAIN_OPEN_G_L_CHART_DRAWING_FIXED_PIPELINE_DECLARE__

#include "AnnotationText.h"
#include "CaretOpenGLInclude.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretAssert.h"
#include "ChartAxis.h"
#include "ChartAxisCartesian.h"
#include "ChartData.h"
#include "ChartDataCartesian.h"
#include "CaretLogger.h"
#include "ChartMatrixDisplayProperties.h"
#include "ChartModelDataSeries.h"
#include "ChartModelFrequencySeries.h"
#include "ChartModelTimeSeries.h"
#include "ChartableMatrixInterface.h"
#include "CaretPreferences.h"
#include "ChartPoint.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiParcelLabelFile.h"
#include "CiftiParcelScalarFile.h"
#include "CiftiScalarDataSeriesFile.h"
#include "Brain.h"
#include "ConnectivityDataLoaded.h"
#include "EventCaretMappableDataFileMapsViewedInOverlays.h"
#include "EventManager.h"
#include "IdentificationWithColor.h"
#include "SelectionItemChartDataSeries.h"
#include "SelectionItemChartFrequencySeries.h"
#include "SelectionItemChartMatrix.h"
#include "SelectionItemChartTimeSeries.h"
#include "SelectionManager.h"
#include "SessionManager.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLChartDrawingFixedPipeline 
 * \brief Chart drawing using OpenGL's fixed pipeline.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLChartDrawingFixedPipeline::BrainOpenGLChartDrawingFixedPipeline()
: BrainOpenGLChartDrawingInterface()
{
    m_brain = NULL;
    m_fixedPipelineDrawing = NULL;
    m_identificationModeFlag = false;
}

/**
 * Destructor.
 */
BrainOpenGLChartDrawingFixedPipeline::~BrainOpenGLChartDrawingFixedPipeline()
{
}


/**
 * Draw a cartesian chart in the given viewport.
 *
 * @param brain
 *     Brain.
 * @param fixedPipelineDrawing
 *     The fixed pipeline OpenGL drawing.
 * @param viewport
 *     Viewport for the chart.
 * @param textRenderer
 *     Text rendering.
 * @param cartesianChart
 *     Cartesian Chart that is drawn.
 * @param selectionItemDataType
 *     Selected data type.
 * @param tabIndex
 *     Index of the tab.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawCartesianChart(Brain* brain,
                                                         BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                         const int32_t viewport[4],
                                                         BrainOpenGLTextRenderInterface* textRenderer,
                                                         ChartModelCartesian* cartesianChart,
                                                         const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                                         const int32_t tabIndex)
{
    m_brain = brain;
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    m_tabIndex = tabIndex;
    m_chartModelDataSeriesBeingDrawnForIdentification = dynamic_cast<ChartModelDataSeries*>(cartesianChart);
    m_chartModelFrequencySeriesBeingDrawnForIdentification = dynamic_cast<ChartModelFrequencySeries*>(cartesianChart);
    m_chartModelTimeSeriesBeingDrawnForIdentification = dynamic_cast<ChartModelTimeSeries*>(cartesianChart);
    m_chartCartesianSelectionTypeForIdentification = selectionItemDataType;
    m_chartableMatrixInterfaceBeingDrawnForIdentification = NULL;
    CaretAssert(cartesianChart);
    if (cartesianChart->isEmpty()) {
        return;
    }
    
    saveStateOfOpenGL();
    
    SelectionItemChartDataSeries* chartDataSeriesID = m_brain->getSelectionManager()->getChartDataSeriesIdentification();
    SelectionItemChartFrequencySeries* chartFrequencySeriesID = m_brain->getSelectionManager()->getChartFrequencySeriesIdentification();
    SelectionItemChartTimeSeries* chartTimeSeriesID = m_brain->getSelectionManager()->getChartTimeSeriesIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    m_identificationModeFlag = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (chartDataSeriesID->isEnabledForSelection()
                || chartFrequencySeriesID->isEnabledForSelection()
                || chartTimeSeriesID->isEnabledForSelection()) {
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
    
    resetIdentification();
    
    const int32_t vpX      = viewport[0];
    const int32_t vpY      = viewport[1];
    const int32_t vpWidth  = viewport[2];
    const int32_t vpHeight = viewport[3];
    
    int32_t chartGraphicsDrawingViewport[4] = {
        vpX,
        vpY,
        vpWidth,
        vpHeight
    };
    
    
    /*
     * Margin is region around the chart in which 
     * the axes legends, values, and ticks are drawn.
     */
    const double marginSize = 30;
    Margins margins(marginSize);
    
    double width, height;
    estimateCartesianChartAxisLegendsWidthHeight(textRenderer, cartesianChart->getLeftAxis(), width, height);
    margins.m_left = std::max(margins.m_left, width);
    estimateCartesianChartAxisLegendsWidthHeight(textRenderer, cartesianChart->getRightAxis(), width, height);
    margins.m_right = std::max(margins.m_right, width);
    estimateCartesianChartAxisLegendsWidthHeight(textRenderer, cartesianChart->getTopAxis(), width, height);
    margins.m_top = std::max(margins.m_top, height);
    estimateCartesianChartAxisLegendsWidthHeight(textRenderer, cartesianChart->getBottomAxis(), width, height);
    margins.m_bottom = std::max(margins.m_bottom, height);
    
    if (margins.m_left > marginSize) margins.m_left += 10;
    if (margins.m_right > marginSize) margins.m_right += 10;
    
    /*
     * Ensure that there is sufficient space for the axes data display.
     */
    if ((vpWidth > (marginSize * 3))
        && (vpHeight > (marginSize * 3))) {
        
        /* Draw legends and grids */
        drawChartAxis(vpX,
                      vpY,
                      vpWidth,
                      vpHeight,
                      margins,
                      textRenderer,
                      cartesianChart->getLeftAxis());
        
        drawChartAxis(vpX,
                      vpY,
                      vpWidth,
                      vpHeight,
                      margins,
                      textRenderer,
                      cartesianChart->getRightAxis());
        
        drawChartAxis(vpX,
                      vpY,
                      vpWidth,
                      vpHeight,
                      margins,
                      textRenderer,
                      cartesianChart->getBottomAxis());
        
        drawChartAxis(vpX,
                      vpY,
                      vpWidth,
                      vpHeight,
                      margins,
                      textRenderer,
                      cartesianChart->getTopAxis());
        
        
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

    drawChartGraphicsLineSeries(textRenderer,
                                cartesianChart);

    /*
     * Process selection
     */
    if (m_identificationModeFlag) {
        processIdentification();
    }
    
    restoreStateOfOpenGL();
}

/**
 * Draw a matrix chart in the given viewport.
 *
 * @param brain
 *     Brain.
 * @param fixedPipelineDrawing
 *     The fixed pipeline OpenGL drawing.
 * @param viewport
 *     Viewport for the chart.
 * @param textRenderer
 *     Text rendering.
 * @param chartMatrixInterface
 *     Chart matrix interface containing matrix data.
 * @param scalarDataSeriesMapIndex
 *     Selected map in scalar data series file.
 * @param selectionItemDataType
 *     Selected data type.
 * @param tabIndex
 *     Index of the tab.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawMatrixChart(Brain* brain,
                                                      BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                      const int32_t viewport[4],
                                                      BrainOpenGLTextRenderInterface* textRenderer,
                                                      ChartableMatrixInterface* chartMatrixInterface,
                                                      const int32_t scalarDataSeriesMapIndex,
                                                      const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                                      const int32_t tabIndex)
{
    m_brain = brain;
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    m_tabIndex = tabIndex;
    m_chartModelDataSeriesBeingDrawnForIdentification = NULL;
    m_chartModelFrequencySeriesBeingDrawnForIdentification = NULL;
    m_chartModelTimeSeriesBeingDrawnForIdentification = NULL;
    m_chartableMatrixInterfaceBeingDrawnForIdentification = chartMatrixInterface;
    m_chartableMatrixSelectionTypeForIdentification = selectionItemDataType;
    
    CaretAssert(chartMatrixInterface);
    
    saveStateOfOpenGL();
    
    SelectionItemChartMatrix* chartMatrixID = m_brain->getSelectionManager()->getChartMatrixIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    m_identificationModeFlag = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (chartMatrixID->isEnabledForSelection()) {
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
    
    resetIdentification();

    ChartMatrixDisplayProperties* matrixProperties = chartMatrixInterface->getChartMatrixDisplayProperties(m_tabIndex);
    const int32_t paletteHeight = (matrixProperties->isColorBarDisplayed()
                                   ? 40
                                   : 0);
    
    /*
     * Viewport is X, Y, Width, Height
     */
    const int32_t chartGraphicsDrawingViewport[4] = {
        viewport[0],
        viewport[1] + paletteHeight,
        viewport[2],
        viewport[3] - paletteHeight
    };
    
    
    /*
     * Margin is region around the chart in which
     * the axes legends, values, and ticks are drawn.
     */
    glViewport(chartGraphicsDrawingViewport[0],
               chartGraphicsDrawingViewport[1],
               chartGraphicsDrawingViewport[2],
               chartGraphicsDrawingViewport[3]);
    
    drawChartGraphicsMatrix(chartGraphicsDrawingViewport,
                            textRenderer,
                            chartMatrixInterface,
                            scalarDataSeriesMapIndex);
    
    /*
     * Process selection
     */
    if (m_identificationModeFlag) {
        processIdentification();
    }

    restoreStateOfOpenGL();
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
 * @param textRenderer
 *     Text rendering.
 * @param axis
 *     Axis that is drawn.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawChartAxis(const float vpX,
                                                    const float vpY,
                                                    const float vpWidth,
                                                    const float vpHeight,
                                                    Margins& margins,
                                                    BrainOpenGLTextRenderInterface* textRenderer,
                                                    ChartAxis* axis)
{
    if (axis == NULL) {
        return;
    }
    
    if ( ! axis->isVisible()) {
        return;
    }
    
    switch (axis->getAxisType()) {
        case ChartAxisTypeEnum::CHART_AXIS_TYPE_CARTESIAN:
            drawChartAxisCartesian(vpX,
                                   vpY,
                                   vpWidth,
                                   vpHeight,
                                   margins,
                                   textRenderer,
                                   dynamic_cast<ChartAxisCartesian*>(axis));
            break;
        case ChartAxisTypeEnum::CHART_AXIS_TYPE_NONE:
            break;
    }
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
 * @param textRenderer
 *     Text rendering.
 * @param chartModelCartesian
 *     The chart cartesian model.
 * @param axis
 *     Axis that is drawn.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawChartAxisCartesian(const float vpX,
                                                    const float vpY,
                                                    const float vpWidth,
                                                    const float vpHeight,
                                                    Margins& margins,
                                                    BrainOpenGLTextRenderInterface* textRenderer,
                                                    ChartAxisCartesian* axis)
{
    CaretAssert(axis);
    
    const float fontSizeInPixels = 14;
    float axisLength = 0.0;
    
    switch (axis->getAxisLocation()) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            axisLength = vpWidth - (margins.m_left + margins.m_right);
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
            axisLength = vpHeight - (margins.m_top + margins.m_bottom);
            break;
    }
    
    std::vector<float> labelOffsetInPixels;
    std::vector<AString> labelTexts;
    axis->getLabelsAndPositions(axisLength,
                                fontSizeInPixels,
                                labelOffsetInPixels,
                                labelTexts);
    
    const int32_t numLabelsToDraw = static_cast<int32_t>(labelTexts.size());
    AnnotationText annotationText;
    if (numLabelsToDraw > 0) {
        float labelX = 0.0;
        float labelY = 0.0;
        float labelOffsetMultiplierX = 0.0;
        float labelOffsetMultiplierY = 0.0;
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        
        /*
         * Viewport for axis name and numeric values
         */
        int32_t axisVpX = vpX;
        int32_t axisVpY = vpY;
        int32_t axisVpWidth = vpWidth;
        int32_t axisVpHeight = vpHeight;
        
        float tickDeltaXY[2] = { 0.0, 0.0 };
        const float tickLength = 5.0;
        switch (axis->getAxisLocation()) {
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                axisVpX = vpX;
                axisVpY = vpY;
                axisVpWidth = vpWidth;
                axisVpHeight = margins.m_bottom;
                labelX = margins.m_left;
                labelY = margins.m_bottom;
                labelOffsetMultiplierX = 1.0;
                labelOffsetMultiplierY = 0.0;
                annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
                annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                tickDeltaXY[0] = 0.0;
                tickDeltaXY[1] = -tickLength;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                axisVpX = vpX;
                axisVpY = vpY + vpHeight - margins.m_top;
                axisVpWidth = vpWidth;
                axisVpHeight = margins.m_top;
                labelX = margins.m_left;
                labelY = 0.0;
                labelOffsetMultiplierX = 1.0;
                labelOffsetMultiplierY = 0.0;
                annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
                annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                tickDeltaXY[0] = 0.0;
                tickDeltaXY[1] = tickLength;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                axisVpX = vpX;
                axisVpY = vpY;
                axisVpWidth = margins.m_left;
                axisVpHeight = vpHeight;
                labelX = margins.m_left;
                labelY = margins.m_bottom;
                labelOffsetMultiplierX = 0.0;
                labelOffsetMultiplierY = 1.0;
                annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
                annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
                tickDeltaXY[0] = -tickLength;
                tickDeltaXY[1] = 0.0;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                axisVpX = vpX + vpWidth - margins.m_right;
                axisVpY = vpY;
                axisVpWidth = margins.m_right;
                axisVpHeight = vpHeight;
                labelX = 0.0;
                labelY = margins.m_bottom;
                labelOffsetMultiplierX = 0.0;
                labelOffsetMultiplierY = 1.0;
                annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
                annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
                tickDeltaXY[0] = tickLength;
                tickDeltaXY[1] = 0.0;
                break;
        }
        
        /*
         * Viewport for axis text and numeric values
         */
        const int viewport[4] = {
            axisVpX,
            axisVpY,
            axisVpWidth,
            axisVpHeight
        };
        glViewport(viewport[0],
                   viewport[1],
                   viewport[2],
                   viewport[3]);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, axisVpWidth, 0, axisVpHeight, -1.0, 1.0);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        glColor3fv(m_fixedPipelineDrawing->m_foregroundColorFloat);
        
        for (int32_t i = 0; i < numLabelsToDraw; i++) {
            const float tickStartX = labelX + labelOffsetInPixels[i] * labelOffsetMultiplierX;
            const float tickStartY = labelY + labelOffsetInPixels[i] * labelOffsetMultiplierY;

            const float tickEndX = tickStartX + tickDeltaXY[0];
            const float tickEndY = tickStartY + tickDeltaXY[1];

            glBegin(GL_LINES);
            glVertex2f(tickStartX,
                       tickStartY);
            glVertex2f(tickEndX,
                       tickEndY);
            glEnd();
            
            const float textX = tickEndX;
            const float textY = tickEndY;
            annotationText.setText(labelTexts[i]);
            textRenderer->drawTextAtViewportCoords(textX,
                                                   textY,
                                                   0.0,
                                                   annotationText);
        }
        
        const AString axisText = axis->getText();
        if ( ! axisText.isEmpty()) {
            AnnotationText annotationText;
            annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
            annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
            
            bool drawAxisTextVerticalFlag = false;
            float axisTextCenterX = axisVpWidth / 2.0;
            float axisTextCenterY = axisVpHeight / 2.0;
            const float textMarginOffset = 5.0;
            switch (axis->getAxisLocation()) {
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                    axisTextCenterX = (vpWidth / 2.0);
                    axisTextCenterY = textMarginOffset;
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                    axisTextCenterX = (vpWidth / 2.0);
                    axisTextCenterY = margins.m_top - textMarginOffset;
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                    axisTextCenterX = textMarginOffset;
                    axisTextCenterY = (vpHeight / 2.0);
                    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
                    drawAxisTextVerticalFlag = true;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                    axisTextCenterX = margins.m_right - textMarginOffset;
                    axisTextCenterY = (vpHeight / 2.0);
                    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
                    drawAxisTextVerticalFlag = true;
                    break;
            }
            if (drawAxisTextVerticalFlag) {
                annotationText.setOrientation(AnnotationTextOrientationEnum::STACKED);
                annotationText.setText(axisText);
                textRenderer->drawTextAtViewportCoords(axisTextCenterX,
                                                       axisTextCenterY,
                                                       0.0,
                                                       annotationText);
            }
            else {
                annotationText.setOrientation(AnnotationTextOrientationEnum::HORIZONTAL);
                annotationText.setText(axisText);
                textRenderer->drawTextAtViewportCoords(axisTextCenterX,
                                                       axisTextCenterY,
                                                       0.0,
                                                       annotationText);
            }
        }
    }
}

/**
 * Estimate the size of the axis' text.
 *
 * @param textRenderer
 *     Text rendering.
 * @param axis
 *    The axis.
 * @param widthOut
 *    Width of text out.
 * @param heightOut
 *    Heigh of text out.
 */
void
BrainOpenGLChartDrawingFixedPipeline::estimateCartesianChartAxisLegendsWidthHeight(BrainOpenGLTextRenderInterface* textRenderer,
                                                                                   ChartAxis* axis,
                                                                                   double& widthOut,
                                                                                   double& heightOut)
{
    widthOut  = 0;
    heightOut = 0;
    
    if (axis == NULL) {
        return;
    }
    
    ChartAxisCartesian* cartesianAxis = dynamic_cast<ChartAxisCartesian*>(axis);
    
    if ( ! cartesianAxis->isVisible()) {
        return;
    }
    
    const float fontSizeInPixels = 14;
    const float axisLength = 1000.0;
    std::vector<float> labelOffsetInPixels;
    std::vector<AString> labelTexts;
    cartesianAxis->getLabelsAndPositions(axisLength,
                                fontSizeInPixels,
                                labelOffsetInPixels,
                                labelTexts);
    for (std::vector<AString>::iterator iter = labelTexts.begin();
         iter != labelTexts.end();
         iter++) {
        const AString text = *iter;
        if ( ! text.isEmpty()) {
            AnnotationText annotationText;
            annotationText.setText(text);
            double textWidth = 0.0;
            double textHeight = 0.0;
            textRenderer->getTextWidthHeightInPixels(annotationText, textWidth, textHeight);
            
            widthOut  = std::max(widthOut,  textWidth);
            heightOut = std::max(heightOut, textHeight);
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
BrainOpenGLChartDrawingFixedPipeline::drawChartGraphicsBoxAndSetViewport(const float vpX,
                       const float vpY,
                       const float vpWidth,
                       const float vpHeight,
                       const Margins& margins,
                       int32_t chartGraphicsDrawingViewportOut[4])
{
    
    const float gridLineWidth = 2;
    const float halfGridLineWidth = gridLineWidth / 2.0;
    
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
    
    glLineWidth(gridLineWidth);
    
    glColor3fv(m_fixedPipelineDrawing->m_foregroundColorFloat);
    
    glBegin(GL_LINES);
    
    /* bottom line */
    glVertex3f(gridLeft,  gridBottom + halfGridLineWidth, 0.0);
    glVertex3f(gridRight, gridBottom + halfGridLineWidth, 0.0);
    
    /* right line */
    glVertex3f(gridRight - halfGridLineWidth, gridBottom, 0.0);
    glVertex3f(gridRight - halfGridLineWidth, gridTop,    0.0);
    
    /* top line */
    glVertex3f(gridRight, gridTop - halfGridLineWidth, 0.0);
    glVertex3f(gridLeft,  gridTop - halfGridLineWidth, 0.0);
    
    /* left line */
    glVertex3f(gridLeft + halfGridLineWidth, gridTop,    0.0);
    glVertex3f(gridLeft + halfGridLineWidth, gridBottom, 0.0);
    
    glEnd();
    
    /*
     * Region inside the grid's box
     */
    const int32_t graphicsLeft   = static_cast<int32_t>(gridLeft   + std::ceil(gridLineWidth  + 1.0));
    const int32_t graphicsRight  = static_cast<int32_t>(gridRight  - std::floor(gridLineWidth + 1.0));
    const int32_t graphicsBottom = static_cast<int32_t>(gridBottom + std::ceil(gridLineWidth  + 1.0));
    const int32_t graphicsTop    = static_cast<int32_t>(gridTop    - std::floor(gridLineWidth + 1.0));
    
    const int32_t graphicsWidth = graphicsRight - graphicsLeft;
    const int32_t graphicsHeight = graphicsTop  - graphicsBottom;
    chartGraphicsDrawingViewportOut[0] = graphicsLeft;
    chartGraphicsDrawingViewportOut[1] = graphicsBottom;
    chartGraphicsDrawingViewportOut[2] = graphicsWidth;
    chartGraphicsDrawingViewportOut[3] = graphicsHeight;
}

/**
 * Draw graphics for the given line series chart.
 *
 * @param textRenderer
 *     Text rendering.
 * @param chart
 *     Chart that is drawn.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawChartGraphicsLineSeries(BrainOpenGLTextRenderInterface* /*textRenderer*/,
                                                                  ChartModelCartesian* chart)
{
    CaretAssert(chart);
    
    std::vector<ChartData*> chartVector = chart->getAllChartDatas();
    
    if (chartVector.empty()) {
        return;
    }
 
    m_fixedPipelineDrawing->enableLineAntiAliasing();
    
    const ChartAxisCartesian* leftAxis = dynamic_cast<ChartAxisCartesian*>(chart->getLeftAxis());
    CaretAssert(leftAxis);
    const ChartAxisCartesian* bottomAxis = dynamic_cast<ChartAxisCartesian*>(chart->getBottomAxis());
    CaretAssert(bottomAxis);
    
    float xMin = bottomAxis->getMinimumValue();
    float xMax = bottomAxis->getMaximumValue();

    float yMin = leftAxis->getMinimumValue();
    float yMax = leftAxis->getMaximumValue();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xMin, xMax,
            yMin, yMax,
            -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    const float lineWidth = chart->getLineWidth();
    /*
     * Start at the oldest chart and end with the newest chart.
     */
    const int32_t numChartData = static_cast<int32_t>(chartVector.size());
    for (int32_t chartDataIndex = (numChartData - 1); chartDataIndex >= 0; chartDataIndex--) {
        const ChartData* chartData = chartVector[chartDataIndex];
        if (chartData->isSelected(m_tabIndex)) {
            const ChartDataCartesian* chartDataCart = dynamic_cast<const ChartDataCartesian*>(chartData);
            CaretAssert(chartDataCart);
            
            CaretColorEnum::Enum color = chartDataCart->getColor();
            drawChartDataCartesian(chartDataIndex,
                                   chartDataCart,
                                   lineWidth,
                                   CaretColorEnum::toRGB(color));
        }
    }
    
    if (chart->isAverageChartDisplaySelected()) {
        const ChartData* chartData = chart->getAverageChartDataForDisplay(m_tabIndex);
        if (chartData != NULL) {
            const ChartDataCartesian* chartDataCart = dynamic_cast<const ChartDataCartesian*>(chartData);
            CaretAssert(chartDataCart);
            drawChartDataCartesian(-1,
                                   chartDataCart,
                                   lineWidth,
                                   m_fixedPipelineDrawing->m_foregroundColorFloat);
        }
    }
    
    m_fixedPipelineDrawing->disableLineAntiAliasing();
}

/**
 * Draw the cartesian data with the given color.
 *
 * @param chartDataIndex
 *   Index of chart data
 * @param chartDataCartesian
 *   Cartesian data that is drawn.
 * @param lineWidth
 *   Width of lines.
 * @param color
 *   Color for the data.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawChartDataCartesian(const int32_t chartDataIndex,
                                                             const ChartDataCartesian* chartDataCartesian,
                                                             const float lineWidth,
                                                             const float rgb[3])
{
    if (lineWidth <= 0.0) {
        return;
    }
    
    glColor3fv(rgb);
    
    glLineWidth(lineWidth);
    if (m_identificationModeFlag) {
        glLineWidth(5.0);
    }
    glBegin(GL_LINE_STRIP);
    const int32_t numPoints = chartDataCartesian->getNumberOfPoints();
    for (int32_t i = 0; i < numPoints; i++) {
        const ChartPoint* point = chartDataCartesian->getPointAtIndex(i);
        if (m_identificationModeFlag) {
            uint8_t rgbaForID[4];
            addToChartLineIdentification(chartDataIndex, i, rgbaForID);
            glColor4ubv(rgbaForID);
        }
        glVertex2fv(point->getXY());
    }
    glEnd();
}

/**
 * Draw graphics for the matrix chart..
 *
 * @param viewport
 *     The viewport.
 * @param textRenderer
 *     Text rendering.
 * @param chartMatrixInterface
 *     Chart that is drawn.
 * @param scalarDataSeriesMapIndex
 *     Selected map for scalar data series file.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawChartGraphicsMatrix(const int32_t viewport[4],
                                                              BrainOpenGLTextRenderInterface* /*textRenderer*/,
                                                              ChartableMatrixInterface* chartMatrixInterface,
                                                              const int32_t scalarDataSeriesMapIndex)
{
    CaretAssert(chartMatrixInterface);
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    uint8_t highlightRGBByte[3];
    prefs->getColorForegroundChartView(highlightRGBByte);
    const float highlightRGB[3] = {
        highlightRGBByte[0] / 255.0,
        highlightRGBByte[1] / 255.0,
        highlightRGBByte[2] / 255.0
    };
    
    int32_t numberOfRows = 0;
    int32_t numberOfColumns = 0;
    std::vector<float> matrixRGBA;
    if (chartMatrixInterface->getMatrixDataRGBA(numberOfRows,
                                       numberOfColumns,
                                       matrixRGBA)) {
        std::set<int32_t> selectedColumnIndices;
        std::set<int32_t> selectedRowIndices;
        
        CiftiMappableConnectivityMatrixDataFile* connMapFile = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(chartMatrixInterface);
        if (connMapFile != NULL) {
            const ConnectivityDataLoaded* connDataLoaded = connMapFile->getConnectivityDataLoaded();
            if (connDataLoaded != NULL) {
                int64_t loadedRowIndex = -1;
                int64_t loadedColumnIndex = -1;
                connDataLoaded->getRowColumnLoading(loadedRowIndex,
                                                    loadedColumnIndex);
                if (loadedRowIndex >= 0) {
                    selectedRowIndices.insert(loadedRowIndex);
                }
                else if (loadedColumnIndex >= 0) {
                    selectedColumnIndices.insert(loadedColumnIndex);
                }
            }
        }
        
        CiftiParcelScalarFile* parcelScalarFile = dynamic_cast<CiftiParcelScalarFile*>(chartMatrixInterface);
        if (parcelScalarFile != NULL) {
            EventCaretMappableDataFileMapsViewedInOverlays mapOverlayEvent(parcelScalarFile);
            EventManager::get()->sendEvent(mapOverlayEvent.getPointer());
            selectedColumnIndices = mapOverlayEvent.getSelectedMapIndices();
        }
        
        CiftiParcelLabelFile* parcelLabelFile = dynamic_cast<CiftiParcelLabelFile*>(chartMatrixInterface);
        if (parcelLabelFile != NULL) {
            EventCaretMappableDataFileMapsViewedInOverlays mapOverlayEvent(parcelLabelFile);
            EventManager::get()->sendEvent(mapOverlayEvent.getPointer());
            selectedColumnIndices = mapOverlayEvent.getSelectedMapIndices();
        }
        
        CiftiScalarDataSeriesFile* scalarDataSeriesFile = dynamic_cast<CiftiScalarDataSeriesFile*>(chartMatrixInterface);
        if (scalarDataSeriesFile != NULL) {
            if (scalarDataSeriesMapIndex >= 0) {
                selectedRowIndices.insert(scalarDataSeriesMapIndex);
            }
        }
        
        bool applyTransformationsFlag = false;
        float panningXY[2] = { 0.0, 0.0 };
        float zooming = 1.0;
        float cellWidth = 1.0;
        float cellHeight = 1.0;
        
        /*
         * Setup width/height of area in which matrix is drawn with a
         * small margin along all of the edges
         */
        float margin = 10.0;
        if ((viewport[2] < (margin * 3.0))
            || (viewport[3] < (margin * 3.0))) {
            margin = 0.0;
        }
        const float graphicsWidth  = viewport[2] - (margin * 2.0);
        const float graphicsHeight = viewport[3] - (margin * 2.0);

        /*
         * Set the width and neight of each matrix cell.
         */
        ChartMatrixDisplayProperties* matrixProperties = chartMatrixInterface->getChartMatrixDisplayProperties(m_tabIndex);
        CaretAssert(matrixProperties);
        const ChartMatrixScaleModeEnum::Enum scaleMode = matrixProperties->getScaleMode();
        switch (scaleMode) {
            case ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_AUTO:
                /*
                 * Auto scale 'fills' the matrix region
                 * and updates the width and height in the
                 * matrix properties for use in manual mode.
                 * There is NO zooming or panning for Auto scale.
                 */
                cellWidth  = graphicsWidth / numberOfColumns;
                cellHeight = graphicsHeight / numberOfRows;
                
                matrixProperties->setCellWidth(cellWidth);
                matrixProperties->setCellHeight(cellHeight);
                break;
            case ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_MANUAL:
                /*
                 * Use the cell width and height for manual mode
                 * and allow both panning and zooming.
                 */
                cellWidth = matrixProperties->getCellWidth();
                cellHeight = matrixProperties->getCellHeight();

                matrixProperties->getViewPanning(panningXY);
                zooming = matrixProperties->getViewZooming();
                applyTransformationsFlag = true;
                break;
        }
        
        const bool highlightSelectedRowColumnFlag = ( ( ! m_identificationModeFlag)
                                                     && matrixProperties->isSelectedRowColumnHighlighted() );
        const bool displayGridLinesFlag = ( ( ! m_identificationModeFlag)
                                           && matrixProperties->isGridLinesDisplayed() );
        
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
            glTranslatef(panningXY[0],
                         panningXY[1],
                         0.0);
            
            const float chartWidth  = cellWidth  * numberOfColumns;
            const float chartHeight = cellHeight * numberOfRows;
            const float halfWidth   = chartWidth  / 2.0;
            const float halfHeight  = chartHeight / 2.0;
            glTranslatef(halfWidth,
                         halfHeight,
                         0.0);
            glScalef(zooming,
                     zooming,
                     1.0);
            glTranslatef(-halfWidth,
                         -halfHeight,
                         0.0);
        }
        
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
                prefs->getColorChartMatrixGridLines(gridLineColorBytes);
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
                
                for (std::set<int32_t>::iterator rowIter = selectedRowIndices.begin();
                     rowIter != selectedRowIndices.end();
                     rowIter ++) {
                    const float rowIndex = * rowIter;
                    const float rowY = (numberOfRows - rowIndex - 1) * cellHeight;
                    
                    
                    rowXYZ.push_back(0.0);
                    rowXYZ.push_back(rowY);
                    rowXYZ.push_back(0.0);
                    rowRGBA.push_back(highlightRGB[0]);
                    rowRGBA.push_back(highlightRGB[1]);
                    rowRGBA.push_back(highlightRGB[2]);
                    rowRGBA.push_back(1.0);
                    
                    rowXYZ.push_back(0.0);
                    rowXYZ.push_back(rowY + cellHeight);
                    rowXYZ.push_back(0.0);
                    rowRGBA.push_back(highlightRGB[0]);
                    rowRGBA.push_back(highlightRGB[1]);
                    rowRGBA.push_back(highlightRGB[2]);
                    rowRGBA.push_back(1.0);
                    
                    rowXYZ.push_back(numberOfColumns * cellWidth);
                    rowXYZ.push_back(rowY + cellHeight);
                    rowXYZ.push_back(0.0);
                    rowRGBA.push_back(highlightRGB[0]);
                    rowRGBA.push_back(highlightRGB[1]);
                    rowRGBA.push_back(highlightRGB[2]);
                    rowRGBA.push_back(1.0);
                    
                    
                    rowXYZ.push_back(numberOfColumns * cellWidth);
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
                
                for (std::set<int32_t>::iterator colIter = selectedColumnIndices.begin();
                     colIter != selectedColumnIndices.end();
                     colIter++) {
                    const float columnIndex = *colIter;
                    const float colX = columnIndex * cellWidth;
                    
                    columnXYZ.push_back(colX);
                    columnXYZ.push_back(0.0);
                    columnXYZ.push_back(0.0);
                    columnRGBA.push_back(highlightRGB[0]);
                    columnRGBA.push_back(highlightRGB[1]);
                    columnRGBA.push_back(highlightRGB[2]);
                    columnRGBA.push_back(1.0);
                    
                    columnXYZ.push_back(colX + cellWidth);
                    columnXYZ.push_back(0.0);
                    columnXYZ.push_back(0.0);
                    columnRGBA.push_back(highlightRGB[0]);
                    columnRGBA.push_back(highlightRGB[1]);
                    columnRGBA.push_back(highlightRGB[2]);
                    columnRGBA.push_back(1.0);
                    
                    columnXYZ.push_back(colX + cellWidth);
                    columnXYZ.push_back(numberOfRows * cellHeight);
                    columnXYZ.push_back(0.0);
                    columnRGBA.push_back(highlightRGB[0]);
                    columnRGBA.push_back(highlightRGB[1]);
                    columnRGBA.push_back(highlightRGB[2]);
                    columnRGBA.push_back(1.0);
                    
                    
                    columnXYZ.push_back(colX);
                    columnXYZ.push_back(numberOfRows * cellHeight);
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
    }
}

/**
 * Save the state of OpenGL.
 * Copied from Qt's qgl.cpp, qt_save_gl_state().
 */
void
BrainOpenGLChartDrawingFixedPipeline::saveStateOfOpenGL()
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
BrainOpenGLChartDrawingFixedPipeline::restoreStateOfOpenGL()
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
 * Add an item for line identification.
 *
 * @param chartDataIndex
 *     Index of the chart data.
 * @param lineIndex
 *     Index of the line.
 * @param rgbaForColorIdentificationOut
 *    Encoded identification in RGBA color OUTPUT
 */
void
BrainOpenGLChartDrawingFixedPipeline::addToChartLineIdentification(const int32_t chartDataIndex,
                                                                   const int32_t chartLineIndex,
                                                                   uint8_t rgbaForColorIdentificationOut[4])
{
    const int32_t idIndex = m_identificationIndices.size() / IDENTIFICATION_INDICES_PER_CHART_LINE;
    
    m_fixedPipelineDrawing->colorIdentification->addItem(rgbaForColorIdentificationOut,
                                                         m_chartCartesianSelectionTypeForIdentification,
                                                         idIndex);
    rgbaForColorIdentificationOut[3] = 255;
    
    /*
     * If these items change, need to update reset and
     * processing of identification.
     */
    m_identificationIndices.push_back(chartDataIndex);
    m_identificationIndices.push_back(chartLineIndex);
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
BrainOpenGLChartDrawingFixedPipeline::addToChartMatrixIdentification(const int32_t matrixRowIndex,
                                                                     const int32_t matrixColumnIndex,
                                                                     uint8_t rgbaForColorIdentificationOut[4])
{
    const int32_t idIndex = m_identificationIndices.size() / IDENTIFICATION_INDICES_PER_MATRIX_ELEMENT;
    
    m_fixedPipelineDrawing->colorIdentification->addItem(rgbaForColorIdentificationOut,
                                                         m_chartableMatrixSelectionTypeForIdentification,
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
 * Reset identification.
 */
void
BrainOpenGLChartDrawingFixedPipeline::resetIdentification()
{
    m_identificationIndices.clear();
    
    if (m_identificationModeFlag) {
        const int32_t estimatedNumberOfItems = 1000;
        m_identificationIndices.reserve(estimatedNumberOfItems);
    }
}

/**
 * Process identification.
 */
void
BrainOpenGLChartDrawingFixedPipeline::processIdentification()
{
    int32_t identifiedItemIndex;
    float depth = -1.0;

    if (m_chartModelDataSeriesBeingDrawnForIdentification != NULL) {
        m_fixedPipelineDrawing->getIndexFromColorSelection(m_chartCartesianSelectionTypeForIdentification,
                                                           m_fixedPipelineDrawing->mouseX,
                                                           m_fixedPipelineDrawing->mouseY,
                                                           identifiedItemIndex,
                                                           depth);
        if (identifiedItemIndex >= 0) {
            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_CHART_LINE;
            const int32_t chartDataIndex = m_identificationIndices[idIndex];
            const int32_t chartLineIndex = m_identificationIndices[idIndex + 1];
            
            SelectionItemChartDataSeries* chartDataSeriesID = m_brain->getSelectionManager()->getChartDataSeriesIdentification();
            if (chartDataSeriesID->isOtherScreenDepthCloserToViewer(depth)) {
                ChartDataCartesian* chartDataCartesian =
                   dynamic_cast<ChartDataCartesian*>(m_chartModelDataSeriesBeingDrawnForIdentification->getChartDataAtIndex(chartDataIndex));
                CaretAssert(chartDataCartesian);
                chartDataSeriesID->setChart(m_chartModelDataSeriesBeingDrawnForIdentification,
                                            chartDataCartesian,
                                            chartLineIndex);
                
                const ChartPoint* chartPoint = chartDataCartesian->getPointAtIndex(chartLineIndex);
                const float lineXYZ[3] = {
                    chartPoint->getX(),
                    chartPoint->getY(),
                    0.0
                };
                
                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(chartDataSeriesID,
                                                                 lineXYZ);
            }
        }
    }
    else if (m_chartModelFrequencySeriesBeingDrawnForIdentification != NULL) {
        m_fixedPipelineDrawing->getIndexFromColorSelection(m_chartCartesianSelectionTypeForIdentification,
                                                           m_fixedPipelineDrawing->mouseX,
                                                           m_fixedPipelineDrawing->mouseY,
                                                           identifiedItemIndex,
                                                           depth);
        if (identifiedItemIndex >= 0) {
            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_CHART_LINE;
            const int32_t chartDataIndex = m_identificationIndices[idIndex];
            const int32_t chartLineIndex = m_identificationIndices[idIndex + 1];
            
            SelectionItemChartFrequencySeries* chartFrequencySeriesID = m_brain->getSelectionManager()->getChartFrequencySeriesIdentification();
            if (chartFrequencySeriesID->isOtherScreenDepthCloserToViewer(depth)) {
                ChartDataCartesian* chartDataCartesian =
                dynamic_cast<ChartDataCartesian*>(m_chartModelFrequencySeriesBeingDrawnForIdentification->getChartDataAtIndex(chartDataIndex));
                CaretAssert(chartDataCartesian);
                chartFrequencySeriesID->setChart(m_chartModelFrequencySeriesBeingDrawnForIdentification,
                                                 chartDataCartesian,
                                                 chartLineIndex);
                
                const ChartPoint* chartPoint = chartDataCartesian->getPointAtIndex(chartLineIndex);
                const float lineXYZ[3] = {
                    chartPoint->getX(),
                    chartPoint->getY(),
                    0.0
                };
                
                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(chartFrequencySeriesID,
                                                                 lineXYZ);
            }
        }
    }
    else if (m_chartModelTimeSeriesBeingDrawnForIdentification != NULL) {
        m_fixedPipelineDrawing->getIndexFromColorSelection(m_chartCartesianSelectionTypeForIdentification,
                                                           m_fixedPipelineDrawing->mouseX,
                                                           m_fixedPipelineDrawing->mouseY,
                                                           identifiedItemIndex,
                                                           depth);
        if (identifiedItemIndex >= 0) {
            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_CHART_LINE;
            const int32_t chartDataIndex = m_identificationIndices[idIndex];
            const int32_t chartLineIndex = m_identificationIndices[idIndex + 1];
            
            SelectionItemChartTimeSeries* chartTimeSeriesID = m_brain->getSelectionManager()->getChartTimeSeriesIdentification();
            if (chartTimeSeriesID->isOtherScreenDepthCloserToViewer(depth)) {
                ChartDataCartesian* chartDataCartesian =
                dynamic_cast<ChartDataCartesian*>(m_chartModelTimeSeriesBeingDrawnForIdentification->getChartDataAtIndex(chartDataIndex));
                CaretAssert(chartDataCartesian);
                chartTimeSeriesID->setChart(m_chartModelTimeSeriesBeingDrawnForIdentification,
                                            chartDataCartesian,
                                            chartLineIndex);
                
                const ChartPoint* chartPoint = chartDataCartesian->getPointAtIndex(chartLineIndex);
                const float lineXYZ[3] = {
                    chartPoint->getX(),
                    chartPoint->getY(),
                    0.0
                };
                
                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(chartTimeSeriesID,
                                                                 lineXYZ);
            }
        }
    }
    else if (m_chartableMatrixInterfaceBeingDrawnForIdentification != NULL) {
        m_fixedPipelineDrawing->getIndexFromColorSelection(m_chartableMatrixSelectionTypeForIdentification,
                                                           m_fixedPipelineDrawing->mouseX,
                                                           m_fixedPipelineDrawing->mouseY,
                                                           identifiedItemIndex,
                                                           depth);
        if (identifiedItemIndex >= 0) {
            const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_MATRIX_ELEMENT;
            const int32_t rowIndex = m_identificationIndices[idIndex];
            const int32_t columnIndex = m_identificationIndices[idIndex + 1];
            
            SelectionItemChartMatrix* chartMatrixID = m_brain->getSelectionManager()->getChartMatrixIdentification();
            if (chartMatrixID->isOtherScreenDepthCloserToViewer(depth)) {
                chartMatrixID->setChartMatrix(m_chartableMatrixInterfaceBeingDrawnForIdentification,
                                              rowIndex,
                                              columnIndex);
            }
        }
    }
}
