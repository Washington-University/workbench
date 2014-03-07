
/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <cmath>

#define __BRAIN_OPEN_G_L_CHART_DRAWING_FIXED_PIPELINE_DECLARE__
#include "BrainOpenGLChartDrawingFixedPipeline.h"
#undef __BRAIN_OPEN_G_L_CHART_DRAWING_FIXED_PIPELINE_DECLARE__

#include "CaretOpenGLInclude.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretAssert.h"
#include "ChartAxis.h"
#include "ChartAxisCartesian.h"
#include "ChartData.h"
#include "ChartDataCartesian.h"
#include "ChartDataMatrix.h"
#include "CaretLogger.h"
#include "ChartModelDataSeries.h"
#include "ChartModelMatrix.h"
#include "CaretPreferences.h"
#include "ChartPoint.h"
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
}

/**
 * Destructor.
 */
BrainOpenGLChartDrawingFixedPipeline::~BrainOpenGLChartDrawingFixedPipeline()
{
}

/**
 * Draw the given chart in the given viewport.
 *
 * @param viewport
 *     Viewport for the chart.
 * @param textRenderer
 *     Text rendering.
 * @param chart
 *     Chart that is drawn.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawChart(const int32_t viewport[4],
                                                BrainOpenGLTextRenderInterface* textRenderer,
                                                ChartModel* chart,
                                                const int32_t tabIndex)
{
    m_tabIndex = tabIndex;
    
    CaretAssert(chart);
    if (chart->isEmpty()) {
        return;
    }
    
    saveStateOfOpenGL();
    
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
                      chart->getLeftAxis());
        
        drawChartAxis(vpX,
                      vpY,
                      vpWidth,
                      vpHeight,
                      marginSize,
                      textRenderer,
                      chart->getRightAxis());
        
        drawChartAxis(vpX,
                      vpY,
                      vpWidth,
                      vpHeight,
                      marginSize,
                      textRenderer,
                      chart->getBottomAxis());
        
        drawChartAxis(vpX,
                      vpY,
                      vpWidth,
                      vpHeight,
                      marginSize,
                      textRenderer,
                      chart->getTopAxis());
        
        
        drawChartAxesGrid(vpX,
                          vpY,
                          vpWidth,
                          vpHeight,
                          marginSize,
                          chartGraphicsDrawingViewport);
        
//        const float gridLineWidth = 2;
//        const float halfGridLineWidth = gridLineWidth / 2.0;
//
//        const float gridLeft   = vpX + marginSize;
//        const float gridRight  = vpX + vpWidth - marginSize;
//        const float gridBottom = vpY + marginSize;
//        const float gridTop    = vpY + vpHeight - marginSize;
//        
//        glMatrixMode(GL_PROJECTION);
//        glLoadIdentity();
//        glOrtho(vpX, (vpX + vpWidth),
//                vpY, (vpY + vpHeight),
//                -1.0, 1.0);
//        
//        glMatrixMode(GL_MODELVIEW);
//        glLoadIdentity();
//        
//        glLineWidth(gridLineWidth);
//        
//        glColor3f(1.0, 1.0, 1.0);
//        glBegin(GL_LINES);
//        
//        /* bottom line */
//        glVertex3f(gridLeft,  gridBottom + halfGridLineWidth, 0.0);
//        glVertex3f(gridRight, gridBottom + halfGridLineWidth, 0.0);
//        
//        /* right line */
//        glVertex3f(gridRight - halfGridLineWidth, gridBottom, 0.0);
//        glVertex3f(gridRight - halfGridLineWidth, gridTop,    0.0);
//        
//        /* top line */
//        glVertex3f(gridRight, gridTop - halfGridLineWidth, 0.0);
//        glVertex3f(gridLeft,  gridTop - halfGridLineWidth, 0.0);
//        
//        /* left line */
//        glVertex3f(gridLeft + halfGridLineWidth, gridTop,    0.0);
//        glVertex3f(gridLeft + halfGridLineWidth, gridBottom, 0.0);
//        
//        glEnd();
//
//        /*
//         * Region inside the grid's box
//         */
//        const int32_t graphicsLeft   = static_cast<int32_t>(gridLeft   + std::ceil(gridLineWidth  + 1.0));
//        const int32_t graphicsRight  = static_cast<int32_t>(gridRight  - std::floor(gridLineWidth + 1.0));
//        const int32_t graphicsBottom = static_cast<int32_t>(gridBottom + std::ceil(gridLineWidth  + 1.0));
//        const int32_t graphicsTop    = static_cast<int32_t>(gridTop    - std::floor(gridLineWidth + 1.0));
//        
//        const int32_t graphicsWidth = graphicsRight - graphicsLeft;
//        const int32_t graphicsHeight = graphicsTop  - graphicsBottom;
//        chartGraphicsDrawingViewport[0] = graphicsLeft;
//        chartGraphicsDrawingViewport[1] = graphicsBottom;
//        chartGraphicsDrawingViewport[2] = graphicsWidth;
//        chartGraphicsDrawingViewport[3] = graphicsHeight;
    }
    
    glViewport(chartGraphicsDrawingViewport[0],
               chartGraphicsDrawingViewport[1],
               chartGraphicsDrawingViewport[2],
               chartGraphicsDrawingViewport[3]);
    
    if (chart != NULL) {
        drawChartGraphics(textRenderer,
                          chart);
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

///**
// * Draw the chart axes grid/box
// *
// * @param vpX
// *     Viewport X for all chart content
// * @param vpY
// *     Viewport Y for all chart content
// * @param vpWidth
// *     Viewport width for all chart content
// * @param vpHeight
// *     Viewport height for all chart content
// * @param marginSize
// *     Margin around grid/box
// * @param textRenderer
// *     Text rendering.
// * @param chartModelCartesian
// *     The chart cartesian model.
// * @param axis
// *     Axis that is drawn.
// */
//void
//BrainOpenGLChartDrawingFixedPipeline::drawChartAxisCartesian(const float vpX,
//                                                             const float vpY,
//                                                             const float vpWidth,
//                                                             const float vpHeight,
//                                                             const float marginSize,
//                                                             BrainOpenGLTextRenderInterface* textRenderer,
//                                                             const ChartAxisCartesian* axis)
//{
//    CaretAssert(axis);
//    
//    const float minValue = axis->getMinimumValue();
//    const float maxValue = axis->getMaximumValue();
//    const float stepValue = axis->getStepValue();
//    const int32_t digitsRightOfDecimal = axis->getDigitsRightOfDecimal();
//    
//    const AString axisText = axis->getText();
//    float axisTextCenterX = 0;
//    float axisTextCenterY = 0;
//    bool drawAxisTextVerticalFlag = false;
//    
//    /*
//     * Viewport for axis name and numeric values
//     */
//    int32_t axisVpX = vpX;
//    int32_t axisVpY = vpY;
//    int32_t axisVpWidth = vpWidth;
//    int32_t axisVpHeight = vpHeight;
//    
//    float minimumValueTextXY[2] = { 0.0, 0.0 };
//    float maximumValueTextXY[2] = { 0.0, 0.0 };
//    BrainOpenGLTextRenderInterface::TextAlignmentX minValueAlignmentX = BrainOpenGLTextRenderInterface::X_CENTER;
//    BrainOpenGLTextRenderInterface::TextAlignmentY minValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
//    BrainOpenGLTextRenderInterface::TextAlignmentX maxValueAlignmentX = BrainOpenGLTextRenderInterface::X_CENTER;
//    BrainOpenGLTextRenderInterface::TextAlignmentY maxValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
//    BrainOpenGLTextRenderInterface::TextAlignmentX zeroValueAlignmentX = BrainOpenGLTextRenderInterface::X_CENTER;
//    BrainOpenGLTextRenderInterface::TextAlignmentY zeroValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
//    
//    switch (axis->getAxisLocation()) {
//        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
//            axisVpX = vpX;
//            axisVpY = vpY;
//            axisVpWidth = vpWidth;
//            axisVpHeight = marginSize;
//            axisTextCenterX = (vpWidth / 2.0);
//            axisTextCenterY = (marginSize / 2.0);
//            
//            minimumValueTextXY[0] = marginSize;
//            minimumValueTextXY[1] = marginSize;
//            maximumValueTextXY[0] = vpWidth - marginSize;
//            maximumValueTextXY[1] = marginSize;
//            
//            minValueAlignmentX = BrainOpenGLTextRenderInterface::X_LEFT;
//            minValueAlignmentY = BrainOpenGLTextRenderInterface::Y_TOP;
//            maxValueAlignmentX = BrainOpenGLTextRenderInterface::X_RIGHT;
//            maxValueAlignmentY = BrainOpenGLTextRenderInterface::Y_TOP;
//            zeroValueAlignmentX = BrainOpenGLTextRenderInterface::X_CENTER;
//            zeroValueAlignmentY = BrainOpenGLTextRenderInterface::Y_TOP;
//            break;
//        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
//            axisVpX = vpX;
//            axisVpY = vpY;
//            axisVpWidth = marginSize;
//            axisVpHeight = vpHeight;
//            axisTextCenterX = (marginSize / 2.0);
//            axisTextCenterY = (vpHeight / 2.0);
//            
//            minimumValueTextXY[0] = marginSize;
//            minimumValueTextXY[1] = marginSize;
//            maximumValueTextXY[0] = marginSize;
//            maximumValueTextXY[1] = vpHeight - marginSize;
//            
//            minValueAlignmentX = BrainOpenGLTextRenderInterface::X_RIGHT;
//            minValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
//            maxValueAlignmentX = BrainOpenGLTextRenderInterface::X_RIGHT;
//            maxValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
//            zeroValueAlignmentX = BrainOpenGLTextRenderInterface::X_RIGHT;
//            zeroValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
//            drawAxisTextVerticalFlag = true;
//            break;
//        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
//            axisVpX = vpX + vpWidth - marginSize;
//            axisVpY = vpY;
//            axisVpWidth = marginSize;
//            axisVpHeight = vpHeight;
//            axisTextCenterX = (marginSize / 2.0);
//            axisTextCenterY = (vpHeight / 2.0);
//            
//            minimumValueTextXY[0] = 0.0;
//            minimumValueTextXY[1] = marginSize;
//            maximumValueTextXY[0] = 0.0;
//            maximumValueTextXY[1] = vpHeight - marginSize;
//            
//            minValueAlignmentX = BrainOpenGLTextRenderInterface::X_LEFT;
//            minValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
//            maxValueAlignmentX = BrainOpenGLTextRenderInterface::X_LEFT;
//            maxValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
//            zeroValueAlignmentX = BrainOpenGLTextRenderInterface::X_LEFT;
//            zeroValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
//            drawAxisTextVerticalFlag = true;
//            break;
//        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
//            axisVpX = vpX;
//            axisVpY = vpY + vpHeight - marginSize;
//            axisVpWidth = vpWidth;
//            axisVpHeight = marginSize;
//            axisTextCenterX = (vpWidth / 2.0);
//            axisTextCenterY = (marginSize / 2.0);
//            
//            minimumValueTextXY[0] = marginSize;
//            minimumValueTextXY[1] = 0.0;
//            maximumValueTextXY[0] = vpWidth - marginSize;
//            maximumValueTextXY[1] = 0.0;
//            
//            minValueAlignmentX = BrainOpenGLTextRenderInterface::X_LEFT;
//            minValueAlignmentY = BrainOpenGLTextRenderInterface::Y_BOTTOM;
//            maxValueAlignmentX = BrainOpenGLTextRenderInterface::X_RIGHT;
//            maxValueAlignmentY = BrainOpenGLTextRenderInterface::Y_BOTTOM;
//            zeroValueAlignmentX = BrainOpenGLTextRenderInterface::X_CENTER;
//            zeroValueAlignmentY = BrainOpenGLTextRenderInterface::Y_BOTTOM;
//            break;
//    }
//    
//    /*
//     * Viewport for axis text and numeric values
//     */
//    glViewport(axisVpX,
//               axisVpY,
//               axisVpWidth,
//               axisVpHeight);
//    
//    glMatrixMode(GL_PROJECTION);
//    glOrtho(0, axisVpWidth, 0, axisVpHeight, -1.0, 1.0);
//    
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    
//    glColor3fv(m_foregroundColor);
//    
//    /*
//     * Viewport in array for text rendering
//     */
//    const int viewport[4] = {
//        axisVpX,
//        axisVpY,
//        axisVpWidth,
//        axisVpHeight
//    };
//
//    if ( ! axisText.isEmpty()) {
//        if (drawAxisTextVerticalFlag) {
//            textRenderer->drawVerticalTextAtWindowCoords(viewport,
//                                                         axisTextCenterX,
//                                                         axisTextCenterY,
//                                                         axisText,
//                                                         BrainOpenGLTextRenderInterface::X_CENTER,
//                                                         BrainOpenGLTextRenderInterface::Y_CENTER);
//        }
//        else {
//            textRenderer->drawTextAtWindowCoords(viewport,
//                                                 axisTextCenterX,
//                                                 axisTextCenterY,
//                                                 axisText,
//                                                 BrainOpenGLTextRenderInterface::X_CENTER,
//                                                 BrainOpenGLTextRenderInterface::Y_CENTER);
//        }
//    }
//    
//    if (maxValue > minValue) {
//        const AString minValueText = AString::number(minValue, 'f', digitsRightOfDecimal); //axisValueToText(minValue);
//        const AString maxValueText = AString::number(maxValue, 'f', digitsRightOfDecimal); //axisValueToText(maxValue);
//        
//        textRenderer->drawTextAtWindowCoords(viewport,
//                                             minimumValueTextXY[0],
//                                             minimumValueTextXY[1],
//                                             minValueText,
//                                             minValueAlignmentX,
//                                             minValueAlignmentY);
//        textRenderer->drawTextAtWindowCoords(viewport,
//                                             maximumValueTextXY[0],
//                                             maximumValueTextXY[1],
//                                             maxValueText,
//                                             maxValueAlignmentX,
//                                             maxValueAlignmentY);
//        /*
//         * Determine if zero should be drawn.
//         * Make sure there is enough space from min and max labels
//         */
//        const float fontHeight = 20;
//        bool drawZeroFlag = false;
//        if ((minValue < 0.0)
//            && (maxValue > 0.0)) {
//            const float range = maxValue - minValue;
//            const float zeroPositionOffset = (0.0 - minValue) / range;
//            
//            
//            const float percentageOfRange = 0.2 * range;
//            //            if ((minValue + percentageOfRange) < 0.0) {
//            //                if ((maxValue - percentageOfRange) > 0.0) {
//            drawZeroFlag = true;
//            
//            float zeroValueTextXY[2] = { 0.0, 0.0 };
//            switch (axis->getAxisLocation()) {
//                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
//                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
//                {
//                    const float textRange = maximumValueTextXY[0] - minimumValueTextXY[0];
//                    zeroValueTextXY[0] = (minimumValueTextXY[0]
//                                          + (textRange * zeroPositionOffset));
//                    zeroValueTextXY[1] = minimumValueTextXY[1];
//                    if ((zeroValueTextXY[0] - minimumValueTextXY[0]) < (fontHeight * minValueText.length())) {
//                        drawZeroFlag = false;
//                    }
//                    if ((maximumValueTextXY[0] - zeroValueTextXY[0]) < (fontHeight * maxValueText.length())) {
//                        drawZeroFlag = false;
//                    }
//                }
//                    break;
//                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
//                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
//                {
//                    const float textRange = maximumValueTextXY[1] - minimumValueTextXY[1];
//                    zeroValueTextXY[0] = minimumValueTextXY[0];
//                    zeroValueTextXY[1] = (minimumValueTextXY[1]
//                                          + (textRange * zeroPositionOffset));
//                    if ((zeroValueTextXY[1] - minimumValueTextXY[1]) < fontHeight) {
//                        drawZeroFlag = false;
//                    }
//                    if ((maximumValueTextXY[1] - zeroValueTextXY[1]) < fontHeight) {
//                        drawZeroFlag = false;
//                    }
//                }
//                    break;
//            }
//            
//            if (drawZeroFlag) {
//                const AString zeroText = AString::number(0.0, 'f', digitsRightOfDecimal);
//                textRenderer->drawTextAtWindowCoords(viewport,
//                                                     zeroValueTextXY[0],
//                                                     zeroValueTextXY[1],
//                                                     zeroText,
//                                                     zeroValueAlignmentX,
//                                                     zeroValueAlignmentY);
//            }
//            //                }
//            //            }
//        }
//        
//    }
//}

///**
// * Convert an axis numeric value into text.
// * Limits usage of decimal point and digits right of decimal.
// *
// * @param axisValue
// *     Value on an axis.
// * @return
// *     Value formatted as text.
// */
//AString
//BrainOpenGLChartDrawingFixedPipeline::axisValueToText(const float axisValue) const
//{
//    float value = axisValue;
//    AString signSymbol = "";
//    if (value < 0.0) {
//        value = -value;
//        signSymbol = "-";
//    }
//    
//    int32_t digitsRightOfDecimal = 0;
//    if (value < 10.0) {
//        if (value < 1.0) {
//            if (value < 0.001) {
//                digitsRightOfDecimal = 0;
//                value = 0.0;
//            }
//            else {
//                digitsRightOfDecimal = 2;
//            }
//        }
//        else {
//            digitsRightOfDecimal = 1;
//        }
//    }
//    
//    AString numberText = AString::number(value, 'f', digitsRightOfDecimal);
//    
//    /*
//     * Remove leading zero if there is a decimal
//     */
//    if ( ! numberText.isEmpty()) {
//        if (numberText.indexOf('.') > 0) {
//            if (numberText[0] == '0') {
//                numberText = numberText.mid(1);
//            }
//        }
//    }
//    
//    AString textOut = signSymbol + numberText;
//    return textOut;
//}


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
 * Draw graphics for the given chart.
 *
 * @param textRenderer
 *     Text rendering.
 * @param chart
 *     Chart that is drawn.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawChartGraphics(BrainOpenGLTextRenderInterface* textRenderer,
                                                        ChartModel* chart)
{
    CaretAssert(chart);
        
    switch (chart->getChartDataType()) {
        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            drawChartGraphicsMatrix(textRenderer,
                                    dynamic_cast<ChartModelMatrix*>(chart));
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
            drawChartGraphicsLineSeries(textRenderer,
                                        dynamic_cast<ChartModelDataSeries*>(chart));
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
            drawChartGraphicsLineSeries(textRenderer,
                                        dynamic_cast<ChartModelDataSeries*>(chart));
            break;
    }
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
                                                                  ChartModelDataSeries* chart)
{
    CaretAssert(chart);
    
    std::vector<const ChartData*> chartVector = chart->getAllSelectedChartDatas(m_tabIndex);
    if (chartVector.empty()) {
        return;
    }
 
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
    
    /*
     * Use a reverse iterator to start at the oldest chart and end with
     * the newest chart.
     */
    for (std::vector<const ChartData*>::reverse_iterator chartIter = chartVector.rbegin();
         chartIter != chartVector.rend();
         chartIter++) {
        const ChartData* chartData = *chartIter;
        if (chartData->isSelected(m_tabIndex)) {
            const ChartDataCartesian* chartDataCart = dynamic_cast<const ChartDataCartesian*>(chartData);
            CaretAssert(chartDataCart);
            
            CaretColorEnum::Enum color = chartDataCart->getColor();
            drawChartDataCartesian(chartDataCart,
                                   1.0,
                                   CaretColorEnum::toRGB(color));
        }
    }
    
    if (chart->isAverageChartDisplaySelected()) {
        const ChartData* chartData = chart->getAverageChartDataForDisplay(m_tabIndex);
        if (chartData != NULL) {
            const ChartDataCartesian* chartDataCart = dynamic_cast<const ChartDataCartesian*>(chartData);
            CaretAssert(chartDataCart);
            drawChartDataCartesian(chartDataCart,
                                   2.0,
                                   m_foregroundColor);
        }
    }
}

/**
 * Draw the cartesian data with the given color.
 *
 * @param chartDataCartesian
 *   Cartesian data that is drawn.
 * @param color
 *   Color for the data.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawChartDataCartesian(const ChartDataCartesian* chartDataCartesian,
                                                             const float lineWidth,
                                                             const float rgb[3])
{
    glColor3fv(rgb);
    
    glLineWidth(lineWidth);
    glBegin(GL_LINE_STRIP);
    const int32_t numPoints = chartDataCartesian->getNumberOfPoints();
    for (int32_t i = 0; i < numPoints; i++) {
        const ChartPoint* point = chartDataCartesian->getPointAtIndex(i);
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
                                                              ChartModelMatrix* chart)
{
    CaretAssert(chart);
    
    std::vector<const ChartData*> chartVector = chart->getAllSelectedChartDatas(m_tabIndex);
    if (chartVector.empty()) {
        return;
    }
    
    if (chartVector.size() > 1) {
        CaretLogSevere("PROGRAM_ERROR: More than one matrix chart selected for display.");
    }
    
    const ChartDataMatrix* chartMatrix = dynamic_cast<const ChartDataMatrix*>(chartVector[0]);
    int32_t numberOfRows = 0;
    int32_t numberOfColumns = 0;
    std::vector<float> matrixRGBA;
    if (chartMatrix->getMatrixDataRGBA(numberOfRows,
                                       numberOfColumns,
                                       matrixRGBA)) {
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
        std::vector<float> quadVerticesRGBA;
        quadVerticesRGBA.reserve(numberOfRows * numberOfColumns * 4);
        float cellY = numberOfRows - 1;
        for (int32_t i = 0; i < numberOfRows; i++) {
            float cellX = 0;
            for (int32_t j = 0; j < numberOfColumns; j++) {
                CaretAssertVectorIndex(matrixRGBA, rgbaOffset+3);
                const float* rgba = &matrixRGBA[rgbaOffset];
                rgbaOffset += 4;
                
                quadVerticesRGBA.push_back(rgba[0]);
                quadVerticesRGBA.push_back(rgba[1]);
                quadVerticesRGBA.push_back(rgba[2]);
                quadVerticesRGBA.push_back(rgba[3]);
                quadVerticesXYZ.push_back(cellX);
                quadVerticesXYZ.push_back(cellY);
                quadVerticesXYZ.push_back(0.0);
                
                quadVerticesRGBA.push_back(rgba[0]);
                quadVerticesRGBA.push_back(rgba[1]);
                quadVerticesRGBA.push_back(rgba[2]);
                quadVerticesRGBA.push_back(rgba[3]);
                quadVerticesXYZ.push_back(cellX + 1);
                quadVerticesXYZ.push_back(cellY);
                quadVerticesXYZ.push_back(0.0);
                
                quadVerticesRGBA.push_back(rgba[0]);
                quadVerticesRGBA.push_back(rgba[1]);
                quadVerticesRGBA.push_back(rgba[2]);
                quadVerticesRGBA.push_back(rgba[3]);
                quadVerticesXYZ.push_back(cellX + 1);
                quadVerticesXYZ.push_back(cellY + 1);
                quadVerticesXYZ.push_back(0.0);
                
                quadVerticesRGBA.push_back(rgba[0]);
                quadVerticesRGBA.push_back(rgba[1]);
                quadVerticesRGBA.push_back(rgba[2]);
                quadVerticesRGBA.push_back(rgba[3]);
                quadVerticesXYZ.push_back(cellX);
                quadVerticesXYZ.push_back(cellY + 1);
                quadVerticesXYZ.push_back(0.0);
                
                cellX += 1;
            }
            cellY -= 1;
        }
        
        /*
         * Draw the matrix elements.
         */
        CaretAssert((quadVerticesXYZ.size() / 3) == (quadVerticesRGBA.size() / 4));
        const int32_t numberQuadVertices = static_cast<int32_t>(quadVerticesXYZ.size() / 3);
        glBegin(GL_QUADS);
        for (int32_t i = 0; i < numberQuadVertices; i++) {
            CaretAssertVectorIndex(quadVerticesRGBA, i*4 + 3);
            glColor4fv(&quadVerticesRGBA[i*4]);
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
        glPolygonMode(GL_FRONT, GL_FILL);
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


