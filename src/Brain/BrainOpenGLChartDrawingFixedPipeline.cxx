
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
#include "ChartData.h"
#include "ChartDataCartesian.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "ChartModelDataSeries.h"
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
                                                ChartModel* chart)
{
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
                                                    const ChartAxis* axis)
{
    CaretAssert(axis);

    if ( ! axis->isVisible()) {
        return;
    }
    
    
    const float minValue = axis->getMinimumValue();
    const float maxValue = axis->getMaximumValue();
    
    const AString axisText = axis->getText();
    float axisTextCenterX = 0;
    float axisTextCenterY = 0;
    bool drawAxisTextVerticalFlag = false;
    
    /*
     * Viewport for axis name and numeric values
     */
    int32_t axisVpX = vpX;
    int32_t axisVpY = vpY;
    int32_t axisVpWidth = vpWidth;
    int32_t axisVpHeight = vpHeight;
    
    
    float minimumValueTextXY[2] = { 0.0, 0.0 };
    float maximumValueTextXY[2] = { 0.0, 0.0 };
    BrainOpenGLTextRenderInterface::TextAlignmentX minValueAlignmentX = BrainOpenGLTextRenderInterface::X_CENTER;
    BrainOpenGLTextRenderInterface::TextAlignmentY minValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
    BrainOpenGLTextRenderInterface::TextAlignmentX maxValueAlignmentX = BrainOpenGLTextRenderInterface::X_CENTER;
    BrainOpenGLTextRenderInterface::TextAlignmentY maxValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
    
    switch (axis->getAxis()) {
        case ChartAxis::AXIS_BOTTOM:
            axisVpX = vpX;
            axisVpY = vpY;
            axisVpWidth = vpWidth;
            axisVpHeight = marginSize;
            axisTextCenterX = (vpWidth / 2.0);
            axisTextCenterY = (marginSize / 2.0);
            
            minimumValueTextXY[0] = marginSize;
            minimumValueTextXY[1] = marginSize;
            maximumValueTextXY[0] = vpWidth - marginSize;
            maximumValueTextXY[1] = marginSize;
            
            minValueAlignmentX = BrainOpenGLTextRenderInterface::X_LEFT;
            minValueAlignmentY = BrainOpenGLTextRenderInterface::Y_TOP;
            maxValueAlignmentX = BrainOpenGLTextRenderInterface::X_RIGHT;
            maxValueAlignmentY = BrainOpenGLTextRenderInterface::Y_TOP;
            break;
        case ChartAxis::AXIS_LEFT:
            axisVpX = vpX;
            axisVpY = vpY;
            axisVpWidth = marginSize;
            axisVpHeight = vpHeight;
            axisTextCenterX = (marginSize / 2.0);
            axisTextCenterY = (vpHeight / 2.0);
            
            minimumValueTextXY[0] = marginSize;
            minimumValueTextXY[1] = marginSize;
            maximumValueTextXY[0] = marginSize;
            maximumValueTextXY[1] = vpHeight - marginSize;
            
            minValueAlignmentX = BrainOpenGLTextRenderInterface::X_RIGHT;
            minValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
            maxValueAlignmentX = BrainOpenGLTextRenderInterface::X_RIGHT;
            maxValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
            drawAxisTextVerticalFlag = true;
            break;
        case ChartAxis::AXIS_RIGHT:
            axisVpX = vpX + vpWidth - marginSize;
            axisVpY = vpY;
            axisVpWidth = marginSize;
            axisVpHeight = vpHeight;
            axisTextCenterX = (marginSize / 2.0);
            axisTextCenterY = (vpHeight / 2.0);
            
            minimumValueTextXY[0] = 0.0;
            minimumValueTextXY[1] = marginSize;
            maximumValueTextXY[0] = 0.0;
            maximumValueTextXY[1] = vpHeight - marginSize;
            
            minValueAlignmentX = BrainOpenGLTextRenderInterface::X_LEFT;
            minValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
            maxValueAlignmentX = BrainOpenGLTextRenderInterface::X_LEFT;
            maxValueAlignmentY = BrainOpenGLTextRenderInterface::Y_CENTER;
            drawAxisTextVerticalFlag = true;
            break;
        case ChartAxis::AXIS_TOP:
            axisVpX = vpX;
            axisVpY = vpY + vpHeight - marginSize;
            axisVpWidth = vpWidth;
            axisVpHeight = marginSize;
            axisTextCenterX = (vpWidth / 2.0);
            axisTextCenterY = (marginSize / 2.0);
            
            minimumValueTextXY[0] = marginSize;
            minimumValueTextXY[1] = 0.0;
            maximumValueTextXY[0] = vpWidth - marginSize;
            maximumValueTextXY[1] = 0.0;
            
            minValueAlignmentX = BrainOpenGLTextRenderInterface::X_LEFT;
            minValueAlignmentY = BrainOpenGLTextRenderInterface::Y_BOTTOM;
            maxValueAlignmentX = BrainOpenGLTextRenderInterface::X_RIGHT;
            maxValueAlignmentY = BrainOpenGLTextRenderInterface::Y_BOTTOM;
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
    
    if ( ! axisText.isEmpty()) {
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
    
    if (maxValue > minValue) {
        const AString minValueText = axisValueToText(minValue);
        const AString maxValueText = axisValueToText(maxValue);
        
        textRenderer->drawTextAtWindowCoords(viewport,
                                             minimumValueTextXY[0],
                                             minimumValueTextXY[1],
                                             minValueText,
                                             minValueAlignmentX,
                                             minValueAlignmentY);
        textRenderer->drawTextAtWindowCoords(viewport,
                                             maximumValueTextXY[0],
                                             maximumValueTextXY[1],
                                             maxValueText,
                                             maxValueAlignmentX,
                                             maxValueAlignmentY);
    }
}

/**
 * Convert an axis numeric value into text.
 * Limits usage of decimal point and digits right of decimal.
 *
 * @param axisValue
 *     Value on an axis.
 * @return 
 *     Value formatted as text.
 */
AString
BrainOpenGLChartDrawingFixedPipeline::axisValueToText(const float axisValue) const
{
    float value = axisValue;
    AString signSymbol = "";
    if (value < 0.0) {
        value = -value;
        signSymbol = "-";
    }
    
    int32_t digitsRightOfDecimal = 0;
    if (value < 10.0) {
        if (value < 1.0) {
            if (value < 0.001) {
                digitsRightOfDecimal = 0;
                value = 0.0;
            }
            else {
                digitsRightOfDecimal = 2;
            }
        }
        else {
            digitsRightOfDecimal = 1;
        }
    }
    
    AString numberText = AString::number(value, 'f', digitsRightOfDecimal);
    
    /*
     * Remove leading zero if there is a decimal
     */
    if ( ! numberText.isEmpty()) {
        if (numberText.indexOf('.') > 0) {
            if (numberText[0] == '0') {
                numberText = numberText.mid(1);
            }
        }
    }
    
    AString textOut = signSymbol + numberText;
    return textOut;
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
            CaretAssert(0);
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
    
    std::vector<const ChartData*> chartVector = chart->getAllSelectedChartDatas();
    if (chartVector.empty()) {
        return;
    }
 
    const ChartAxis* leftAxis = chart->getLeftAxis();
    const ChartAxis* bottomAxis = chart->getBottomAxis();
    
    const float xMin = bottomAxis->getMinimumValue();
    const float xMax = bottomAxis->getMaximumValue();
    const float yMin = leftAxis->getMinimumValue();
    const float yMax = leftAxis->getMaximumValue();
    
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
        if (chart->isChartDataSelected(chartData)) {
            const ChartDataCartesian* chartDataCart = dynamic_cast<const ChartDataCartesian*>(chartData);
            CaretAssert(chartDataCart);
            
            CaretColorEnum::Enum color = chartDataCart->getColor();
            drawChartDataCartesian(chartDataCart,
                                   1.0,
                                   CaretColorEnum::toRGB(color));
        }
    }
    
    if (chart->isAverageChartDisplaySelected()) {
        const ChartData* chartData = chart->getAverageChartDataForDisplay();
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
                                                              ChartModelMatrix* /*chart*/)
{
    CaretAssert(0);
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
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
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


