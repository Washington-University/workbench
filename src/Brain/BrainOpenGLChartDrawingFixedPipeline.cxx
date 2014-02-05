
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

#define __BRAIN_OPEN_G_L_CHART_DRAWING_FIXED_PIPELINE_DECLARE__
#include "BrainOpenGLChartDrawingFixedPipeline.h"
#undef __BRAIN_OPEN_G_L_CHART_DRAWING_FIXED_PIPELINE_DECLARE__

#include "CaretOpenGLInclude.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretAssert.h"
#include "ChartAxis.h"
#include "ChartData.h"
#include "ChartDataCartesian.h"
#include "ChartModelDataSeries.h"
#include "ChartPoint.h"

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
    if (chart == NULL) {
        return;
    }
    
    saveStateOfOpenGL();
    
    const int32_t vpX      = viewport[0];
    const int32_t vpY      = viewport[1];
    const int32_t vpWidth  = viewport[2];
    const int32_t vpHeight = viewport[3];
    
    const int32_t marginSize = 30;
    
    if ((vpX > (marginSize * 3))
        && (vpY > (marginSize * 3))) {
        
        /* Draw legends and grids */
    }
    
    const int32_t chartViewport[4] = {
        vpX + marginSize,
        vpY + marginSize,
        vpWidth  - (marginSize * 2),
        vpHeight - (marginSize * 2)
    };

    drawChartGraphics(chartViewport,
                      textRenderer,
                      chart);
    
    restoreStateOfOpenGL();
}

/**
 * Draw graphics for the given chart in the given viewport.
 *
 * @param viewport
 *     Viewport for the chart.
 * @param textRenderer
 *     Text rendering.
 * @param chart
 *     Chart that is drawn.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawChartGraphics(const int32_t viewport[4],
                     BrainOpenGLTextRenderInterface* textRenderer,
                     ChartModel* chart)
{
    CaretAssert(chart);
    
    glViewport(viewport[0],
               viewport[1],
               viewport[2],
               viewport[3]);
    
    glColor3f(1.0, 1.0, 1.0);
    
    float left   = viewport[0];
    float right  = left + viewport[2];
    float bottom = viewport[1];
    float top    = bottom + viewport[3];
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(left, right,
            bottom, top,
            -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    const float lineWidth = 2.0;
    glLineWidth(lineWidth);
    const float halfLineWidth = lineWidth / 2.0;
    const float boxLeft = left;
    const float boxRight = right;
    const float boxBottom = bottom;
    const float boxTop = top;
    
    
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);

    /* bottom line */
    glVertex3f(boxLeft,  boxBottom + halfLineWidth, 0.0);
    glVertex3f(boxRight, boxBottom + halfLineWidth, 0.0);
    
    /* right line */
    glVertex3f(boxRight - halfLineWidth, boxBottom, 0.0);
    glVertex3f(boxRight - halfLineWidth, boxTop,    0.0);
    
    /* top line */
    glVertex3f(boxRight, boxTop - halfLineWidth, 0.0);
    glVertex3f(boxLeft,  boxTop - halfLineWidth, 0.0);

    /* left line */
    glVertex3f(boxLeft + halfLineWidth, boxTop,    0.0);
    glVertex3f(boxLeft + halfLineWidth, boxBottom, 0.0);
    
    glEnd();
    
    const int32_t chartViewport[4] = {
        viewport[0] + lineWidth,
        viewport[1] + lineWidth,
        viewport[2] - (lineWidth * 2.0),
        viewport[3] - (lineWidth * 2.0)
    };
    glViewport(chartViewport[0],
               chartViewport[1],
               chartViewport[2],
               chartViewport[3]);
    
    switch (chart->getChartDataType()) {
        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_ADJACENCY_MATRIX:
            CaretAssert(0);
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
            drawChartGraphicsLineSeries(viewport,
                                        textRenderer,
                                        dynamic_cast<ChartModelDataSeries*>(chart));
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
            drawChartGraphicsLineSeries(viewport,
                                        textRenderer,
                                        dynamic_cast<ChartModelDataSeries*>(chart));
            break;
    }
}

/**
 * Draw graphics for the given line series chart in the given viewport.
 *
 * @param viewport
 *     Viewport for the chart.
 * @param textRenderer
 *     Text rendering.
 * @param chart
 *     Chart that is drawn.
 */
void
BrainOpenGLChartDrawingFixedPipeline::drawChartGraphicsLineSeries(const int32_t viewport[4],
                         BrainOpenGLTextRenderInterface* textRenderer,
                         ChartModelDataSeries* chart)
{
    CaretAssert(chart);
    
    std::vector<ChartData*> chartVector = chart->getChartDatasForDisplay();
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
    
    for (std::vector<ChartData*>::iterator chartIter = chartVector.begin();
         chartIter != chartVector.end();
         chartIter++) {
        ChartData* chartData = *chartIter;
        ChartDataCartesian* chartDataCart = dynamic_cast<ChartDataCartesian*>(chartData);
        CaretAssert(chartDataCart);
        
        CaretColorEnum::Enum color = chartDataCart->getColor();
        glColor3fv(CaretColorEnum::toRGB(color));
        
        glLineWidth(1.0);
        glBegin(GL_LINE_STRIP);
        const int32_t numPoints = chartDataCart->getNumberOfPoints();
        for (int32_t i = 0; i < numPoints; i++) {
            const ChartPoint* point = chartDataCart->getPointAtIndex(i);
            glVertex2fv(point->getXY());
        }
        glEnd();
    }
//    
//    glBegin(GL_LINE_STRIP);
//    glColor3f(1.0, 0.0, 0.0);
//    glVertex3f(xMin, (yMin + yMax) / 2.0, 0.0);
//    glVertex3f(xMax, (yMin + yMax) / 2.0, 0.0);
//    glColor3f(0.0, 1.0, 0.0);
//    glVertex3f((xMin + xMax) / 2.0, yMin, 0.0);
//    glVertex3f((xMin + xMax) / 2.0, yMax, 0.0);
//    glEnd();
    
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


