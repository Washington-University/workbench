
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

#include "CaretOpenGLInclude.h"
#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretAssert.h"
#include "ChartAxis.h"
#include "ChartAxisCartesian.h"
#include "ChartData.h"
#include "ChartDataCartesian.h"
#include "CaretLogger.h"
#include "ChartModelDataSeries.h"
#include "ChartModelTimeSeries.h"
#include "ChartableMatrixInterface.h"
#include "CaretPreferences.h"
#include "ChartPoint.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "ConnectivityDataLoaded.h"
#include "IdentificationWithColor.h"
#include "SelectionItemChartDataSeries.h"
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
    m_chartModelTimeSeriesBeingDrawnForIdentification = dynamic_cast<ChartModelTimeSeries*>(cartesianChart);
    m_chartCartesianSelectionTypeForIdentification = selectionItemDataType;
    m_chartableMatrixInterfaceBeingDrawnForIdentification = NULL;
    CaretAssert(cartesianChart);
    if (cartesianChart->isEmpty()) {
        return;
    }
    
    saveStateOfOpenGL();
    
    SelectionItemChartDataSeries* chartDataSeriesID = m_brain->getSelectionManager()->getChartDataSeriesIdentification();
    SelectionItemChartTimeSeries* chartTimeSeriesID = m_brain->getSelectionManager()->getChartTimeSeriesIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    m_identificationModeFlag = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (chartDataSeriesID->isEnabledForSelection()) {
                m_identificationModeFlag = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else if (chartTimeSeriesID->isEnabledForSelection()) {
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
    
    const CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
    preferences->getColorForeground(m_foregroundColor);
    m_foregroundColor[3] = 1.0;
    
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
    const int32_t marginSize = 30;
    
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
                      marginSize,
                      textRenderer,
                      cartesianChart->getLeftAxis());
        
        drawChartAxis(vpX,
                      vpY,
                      vpWidth,
                      vpHeight,
                      marginSize,
                      textRenderer,
                      cartesianChart->getRightAxis());
        
        drawChartAxis(vpX,
                      vpY,
                      vpWidth,
                      vpHeight,
                      marginSize,
                      textRenderer,
                      cartesianChart->getBottomAxis());
        
        drawChartAxis(vpX,
                      vpY,
                      vpWidth,
                      vpHeight,
                      marginSize,
                      textRenderer,
                      cartesianChart->getTopAxis());
        
        
        drawChartAxesGrid(vpX,
                          vpY,
                          vpWidth,
                          vpHeight,
                          marginSize,
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
                                                      const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                                      const int32_t tabIndex)
{
    m_brain = brain;
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    m_tabIndex = tabIndex;
    m_chartModelDataSeriesBeingDrawnForIdentification = NULL;
    m_chartModelTimeSeriesBeingDrawnForIdentification = NULL;
    m_chartableMatrixInterfaceBeingDrawnForIdentification = chartMatrixInterface;
    m_chartableMatrixSelectionTypeForIdentification = selectionItemDataType;
    
    CaretAssert(chartMatrixInterface);
    
    int32_t numberOfRows = 0;
    int32_t numberOfColumns = 0;
    std::vector<float> matrixRGBA;
    chartMatrixInterface->getMatrixDataRGBA(numberOfRows,
                                            numberOfColumns,
                                            matrixRGBA);
    if ((numberOfRows <= 0)
        || (numberOfColumns <= 0)) {
        return;
    }
    
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

    const CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
    preferences->getColorForeground(m_foregroundColor);
    m_foregroundColor[3] = 1.0;
    
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
    //const int32_t marginSize = 30;
    
    
    glViewport(chartGraphicsDrawingViewport[0],
               chartGraphicsDrawingViewport[1],
               chartGraphicsDrawingViewport[2],
               chartGraphicsDrawingViewport[3]);
    
    drawChartGraphicsMatrix(textRenderer,
                            chartMatrixInterface);
    
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
 * @param marginSize
 *     Margin around grid/box
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
                                                    const float marginSize,
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
                                   marginSize,
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
 * @param marginSize
 *     Margin around grid/box
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
                                                    const float marginSize,
                                                    BrainOpenGLTextRenderInterface* textRenderer,
                                                    ChartAxisCartesian* axis)
{
    CaretAssert(axis);
    
    const float fontSizeInPixels = 14;
    float axisLength = 0.0;
    
    switch (axis->getAxisLocation()) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            axisLength = vpWidth - marginSize * 2.0;
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
            axisLength = vpHeight - marginSize * 2.0;
            break;
    }
    
    std::vector<float> labelOffsetInPixels;
    std::vector<AString> labelTexts;
    axis->getLabelsAndPositions(axisLength,
                                fontSizeInPixels,
                                labelOffsetInPixels,
                                labelTexts);
    
    const int32_t numLabelsToDraw = static_cast<int32_t>(labelTexts.size());
    if (numLabelsToDraw > 0) {
        float labelX = 0.0;
        float labelY = 0.0;
        float labelOffsetMultiplierX = 0.0;
        float labelOffsetMultiplierY = 0.0;
        BrainOpenGLTextRenderInterface::TextAlignmentX labelAlignmentX = BrainOpenGLTextRenderInterface::X_CENTER;
        BrainOpenGLTextRenderInterface::TextAlignmentY labelAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
        
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
                axisVpHeight = marginSize;
                labelX = marginSize;
                labelY = marginSize;
                labelOffsetMultiplierX = 1.0;
                labelOffsetMultiplierY = 0.0;
                labelAlignmentX = BrainOpenGLTextRenderInterface::X_CENTER;
                labelAlignmentY = BrainOpenGLTextRenderInterface::Y_TOP;
                tickDeltaXY[0] = 0.0;
                tickDeltaXY[1] = -tickLength;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                axisVpX = vpX;
                axisVpY = vpY;
                axisVpWidth = marginSize;
                axisVpHeight = vpHeight;
                labelX = marginSize;
                labelY = vpY - marginSize;
                labelOffsetMultiplierX = 1.0;
                labelOffsetMultiplierY = 0.0;
                labelAlignmentX = BrainOpenGLTextRenderInterface::X_CENTER;
                labelAlignmentY = BrainOpenGLTextRenderInterface::Y_BOTTOM;
                tickDeltaXY[0] = 0.0;
                tickDeltaXY[1] = tickLength;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                axisVpX = vpX;
                axisVpY = vpY;
                axisVpWidth = marginSize;
                axisVpHeight = vpHeight;
                labelX = marginSize;
                labelY = marginSize;
                labelOffsetMultiplierX = 0.0;
                labelOffsetMultiplierY = 1.0;
                labelAlignmentX = BrainOpenGLTextRenderInterface::X_RIGHT;
                labelAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
                tickDeltaXY[0] = -tickLength;
                tickDeltaXY[1] = 0.0;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                axisVpX = vpX;
                axisVpY = vpY + vpHeight - marginSize;
                axisVpWidth = vpWidth;
                axisVpHeight = marginSize;
                labelX = vpX - marginSize;
                labelY = marginSize;
                labelOffsetMultiplierX = 0.0;
                labelOffsetMultiplierY = 1.0;
                labelAlignmentX = BrainOpenGLTextRenderInterface::X_LEFT;
                labelAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
                tickDeltaXY[0] = tickLength;
                tickDeltaXY[1] = 0.0;
                break;
        }
        
        /*
         * Viewport for axis text and numeric values
         */
        glViewport(axisVpX,
                   axisVpY,
                   axisVpWidth,
                   axisVpHeight);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, axisVpWidth, 0, axisVpHeight, -1.0, 1.0);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        glColor3fv(m_foregroundColor);
        
        /*
         * Viewport in array for text rendering
         */
        const int viewport[4] = {
            axisVpX,
            axisVpY,
            axisVpWidth,
            axisVpHeight
        };
        
        const float lastIndex = numLabelsToDraw - 1;
        for (int32_t i = 0; i < numLabelsToDraw; i++) {
            const float tickStartX = labelX + labelOffsetInPixels[i] * labelOffsetMultiplierX;
            const float tickStartY = labelY + labelOffsetInPixels[i] * labelOffsetMultiplierY;

            const float tickEndX = tickStartX + tickDeltaXY[0];
            const float tickEndY = tickStartY + tickDeltaXY[1];

            if ((i > 0)
                && (i < lastIndex)) {
                glBegin(GL_LINES);
                glVertex2f(tickStartX,
                           tickStartY);
                glVertex2f(tickEndX,
                           tickEndY);
                glEnd();
            }
            
            const float textX = tickEndX;
            const float textY = tickEndY;
            textRenderer->drawTextAtWindowCoords(viewport,
                                                 textX,
                                                 textY,
                                                 labelTexts[i],
                                                 labelAlignmentX,
                                                 labelAlignmentY);
        }
        
        const AString axisText = axis->getText();
        if ( ! axisText.isEmpty()) {
            bool drawAxisTextVerticalFlag = false;
            float axisTextCenterX = 0.0;
            float axisTextCenterY = 0.0;
            switch (axis->getAxisLocation()) {
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                    axisTextCenterX = (vpWidth / 2.0);
                    axisTextCenterY = (marginSize / 2.0);
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                    axisTextCenterX = (vpWidth / 2.0);
                    axisTextCenterY = (marginSize / 2.0);
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                    axisTextCenterX = (marginSize / 2.0);
                    axisTextCenterY = (vpHeight / 2.0);
                    drawAxisTextVerticalFlag = true;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                    axisTextCenterX = (marginSize / 2.0);
                    axisTextCenterY = (vpHeight / 2.0);
                    drawAxisTextVerticalFlag = true;
                    break;
            }
            if (drawAxisTextVerticalFlag) {
                textRenderer->drawVerticalTextAtWindowCoords(viewport,
                                                             axisTextCenterX,
                                                             axisTextCenterY,
                                                             axisText,
                                                             BrainOpenGLTextRenderInterface::X_CENTER,
                                                             BrainOpenGLTextRenderInterface::Y_CENTER);
            }
            else {
                textRenderer->drawTextAtWindowCoords(viewport,
                                                     axisTextCenterX,
                                                     axisTextCenterY,
                                                     axisText,
                                                     BrainOpenGLTextRenderInterface::X_CENTER,
                                                     BrainOpenGLTextRenderInterface::Y_CENTER);
            }
        }
    }
}

/**
 * Draw the chart axes grid/box
 *
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
 *     the box/grid
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawChartAxesGrid(const float vpX,
                       const float vpY,
                       const float vpWidth,
                       const float vpHeight,
                       const float marginSize,
                       int32_t chartGraphicsDrawingViewportOut[4])
{
    
    const float gridLineWidth = 2;
    const float halfGridLineWidth = gridLineWidth / 2.0;
    
    const float gridLeft   = vpX + marginSize;
    const float gridRight  = vpX + vpWidth - marginSize;
    const float gridBottom = vpY + marginSize;
    const float gridTop    = vpY + vpHeight - marginSize;
    
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
    
    glColor3fv(m_foregroundColor);
    
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
    
    //std::vector<const ChartData*> chartVector = chart->getAllSelectedChartDatas(m_tabIndex);
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
                                   m_foregroundColor);
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
 * @param textRenderer
 *     Text rendering.
 * @param chart
 *     Chart that is drawn.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawChartGraphicsMatrix(BrainOpenGLTextRenderInterface* /*textRenderer*/,
                                                              ChartableMatrixInterface* chartMatrixInterface)
{
    CaretAssert(chartMatrixInterface);
    
    int32_t numberOfRows = 0;
    int32_t numberOfColumns = 0;
    std::vector<float> matrixRGBA;
    if (chartMatrixInterface->getMatrixDataRGBA(numberOfRows,
                                       numberOfColumns,
                                       matrixRGBA)) {
        int64_t loadedRowIndex = -1;
        CiftiMappableConnectivityMatrixDataFile* connMapFile = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(chartMatrixInterface);
        const ConnectivityDataLoaded* connDataLoaded = connMapFile->getConnectivityDataLoaded();
        if (connDataLoaded != NULL) {
            connDataLoaded->getRowLoading(loadedRowIndex);
        }
        
        const float xMin = -1;
        const float xMax = numberOfColumns + 1;
        const float yMin = -1;
        const float yMax = numberOfRows + 1;
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(xMin, xMax,
                yMin, yMax,
                -1.0, 1.0);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        int32_t rgbaOffset = 0;
        std::vector<float> quadVerticesXYZ;
        quadVerticesXYZ.reserve(numberOfRows * numberOfColumns * 3);
        std::vector<float> quadVerticesFloatRGBA;
        quadVerticesFloatRGBA.reserve(numberOfRows * numberOfColumns * 4);
        std::vector<uint8_t> quadVerticesByteRGBA;
        quadVerticesByteRGBA.reserve(numberOfRows * numberOfColumns * 4);
        
        std::vector<float> loadedRowHighlightVerticesXYZ;
        std::vector<float> loadedRowHighlightVerticesRGBA;
        bool loadedRowDataValid = false;
        
        float cellY = numberOfRows - 1;
        for (int32_t i = 0; i < numberOfRows; i++) {
            float cellX = 0;
            for (int32_t j = 0; j < numberOfColumns; j++) {
                CaretAssertVectorIndex(matrixRGBA, rgbaOffset+3);
                const float* rgba = &matrixRGBA[rgbaOffset];
                rgbaOffset += 4;
                
                uint8_t idRGBA[4];
                if (m_identificationModeFlag) {
                    addToChartMatrixIdentification(numberOfRows - i - 1, j, idRGBA);
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
                quadVerticesXYZ.push_back(cellX + 1);
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
                quadVerticesXYZ.push_back(cellX + 1);
                quadVerticesXYZ.push_back(cellY + 1);
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
                quadVerticesXYZ.push_back(cellY + 1);
                quadVerticesXYZ.push_back(0.0);
                
                
                cellX += 1;
            }
            
            if (! m_identificationModeFlag) {
                if (cellY == loadedRowIndex) {
                    loadedRowHighlightVerticesXYZ.push_back(0.0);
                    loadedRowHighlightVerticesXYZ.push_back(cellY);
                    loadedRowHighlightVerticesXYZ.push_back(0.0);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[0]);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[1]);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[2]);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[3]);
                    
                    loadedRowHighlightVerticesXYZ.push_back(numberOfColumns);
                    loadedRowHighlightVerticesXYZ.push_back(cellY);
                    loadedRowHighlightVerticesXYZ.push_back(0.0);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[0]);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[1]);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[2]);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[3]);
                    
                    loadedRowHighlightVerticesXYZ.push_back(numberOfColumns);
                    loadedRowHighlightVerticesXYZ.push_back(cellY + 1);
                    loadedRowHighlightVerticesXYZ.push_back(0.0);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[0]);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[1]);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[2]);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[3]);
                    
                    
                    loadedRowHighlightVerticesXYZ.push_back(0.0);
                    loadedRowHighlightVerticesXYZ.push_back(cellY + 1);
                    loadedRowHighlightVerticesXYZ.push_back(0.0);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[0]);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[1]);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[2]);
                    loadedRowHighlightVerticesRGBA.push_back(m_foregroundColor[3]);

                    CaretAssert(loadedRowHighlightVerticesXYZ.size() == 12);
                    CaretAssert(loadedRowHighlightVerticesRGBA.size() == 16);
                    
                    loadedRowDataValid = true;
                }
            }
            cellY -= 1;
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
            
            /*
             * Drawn an outline around the matrix elements using
             * the foreground color.
             */
            std::vector<float> outlineRGBA;
            outlineRGBA.reserve(numberQuadVertices * 4);
            for (int32_t i = 0; i < numberQuadVertices; i++) {
                outlineRGBA.push_back(m_foregroundColor[0]);
                outlineRGBA.push_back(m_foregroundColor[1]);
                outlineRGBA.push_back(m_foregroundColor[2]);
                outlineRGBA.push_back(m_foregroundColor[3]);
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
            
            if (loadedRowDataValid) {
                CaretAssert((loadedRowHighlightVerticesXYZ.size() / 3) == (loadedRowHighlightVerticesRGBA.size() / 4));
                
                const int32_t numberOfVertices = static_cast<int32_t>(loadedRowHighlightVerticesXYZ.size() / 3);
                glLineWidth(3.0);
                glBegin(GL_QUADS);
                for (int32_t i = 0; i < numberOfVertices; i++) {
                    CaretAssertVectorIndex(loadedRowHighlightVerticesRGBA, i*4 + 3);
                    glColor4fv(&loadedRowHighlightVerticesRGBA[i*4]);
                    CaretAssertVectorIndex(loadedRowHighlightVerticesXYZ, i*3 + 2);
                    glVertex3fv(&loadedRowHighlightVerticesXYZ[i*3]);
                }
                glEnd();
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
//    glDisable(GL_CULL_FACE);
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
