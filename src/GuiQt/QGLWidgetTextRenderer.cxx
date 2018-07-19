
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

/*
 * QOpenGLWidget does not have renderText() methods.
 * renderText() methods are in deprecated QGLWidget.
 */
#ifndef WORKBENCH_USE_QT5_QOPENGL_WIDGET

#define __QT_OPEN_G_L_TEXT_RENDERER_DECLARE__
#include "QGLWidgetTextRenderer.h"
#undef __QT_OPEN_G_L_TEXT_RENDERER_DECLARE__

/*
 * When GLEW is used, CaretOpenGLInclude.h will include "Gl/glew.h".
 * Gl/glew.h MUST BE BEFORE Gl/gl.h and Gl/gl.h is included by
 * QGLWidget so, we must include CaretOpenGL.h before QGLWidget.
 */
#include "CaretOpenGLInclude.h"

#include <cmath>
#include <limits>

#include <QFont>
#include <QFontInfo>
#include <QFontMetrics>
#include <QGLWidget>

#include "AnnotationPointSizeText.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOpenGLInclude.h"

using namespace caret;

/**
 * \class caret::QGLWidgetTextRenderer 
 * \brief Draw text in OpenGL using Qt's GLWidget.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param glWidget
 *     Qt's GL Widget used for drawing text.
 */
QGLWidgetTextRenderer::QGLWidgetTextRenderer(QGLWidget* glWidget)
: BrainOpenGLTextRenderInterface(),
m_glWidget(glWidget)
{
    m_defaultFont = NULL;

    AnnotationPointSizeText defaultAnnotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
    defaultAnnotationText.setFontPointSize(AnnotationTextFontPointSizeEnum::SIZE14);
    defaultAnnotationText.setFont(AnnotationTextFontNameEnum::VERA);
    defaultAnnotationText.setItalicStyleEnabled(false);
    defaultAnnotationText.setBoldStyleEnabled(false);
    defaultAnnotationText.setUnderlineStyleEnabled(false);
    m_defaultFont = findFont(defaultAnnotationText,
                             true);
}

/**
 * Destructor.
 */
QGLWidgetTextRenderer::~QGLWidgetTextRenderer()
{
    for (FONT_MAP_ITERATOR iter = m_fontNameToFontMap.begin();
         iter != m_fontNameToFontMap.end();
         iter++) {
        delete iter->second;
    }
    m_fontNameToFontMap.clear();
    
    /*
     * Do not delete "m_defaultFont" since it points to a font
     * in m_fontNameToFontMap.  Doing so would cause
     * a double delete.
     */
}

/**
 * Draw annnotation text at the given viewport coordinates using
 * the the annotations attributes for the style of text.
 *
 * Depth testing is DISABLED when drawing text with this method.
 *
 * @param viewportX
 *     Viewport X-coordinate.
 * @param viewportY
 *     Viewport Y-coordinate.
 * @param viewportZ
 *     Viewport Z-coordinate.
 * @param annotationText
 *     Annotation text and attributes.
 */
void
QGLWidgetTextRenderer::drawTextAtViewportCoords(const double viewportX,
                                               const double viewportY,
                                               const AnnotationText& annotationText,
                                                const BrainOpenGLTextRenderInterface::DrawingFlags& flags)
{
    setViewportHeight();
    
    drawTextAtViewportCoords(viewportX,
                             viewportY,
                             0.0,
                             annotationText,
                             flags);
}

/**
 * Draw annnotation text at the given viewport coordinates using
 * the the annotations attributes for the style of text.
 *
 * Depth testing is ENABLED when drawing text with this method.
 *
 * @param viewportX
 *     Viewport X-coordinate.
 * @param viewportY
 *     Viewport Y-coordinate.
 * @param viewportZ
 *     Viewport Z-coordinate.
 * @param annotationText
 *     Annotation text and attributes.
 */
void
QGLWidgetTextRenderer::drawTextAtViewportCoords(const double viewportX,
                                               const double viewportY,
                                               const double /*viewportZ */,
                                               const AnnotationText& annotationText,
                                                const BrainOpenGLTextRenderInterface::DrawingFlags& flags)
{
    setViewportHeight();
    
    GLdouble modelMatrix[16];
    GLdouble projectionMatrix[16];
    GLint viewport[4];
    
    glGetDoublev(GL_MODELVIEW_MATRIX,
                 modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX,
                 projectionMatrix);
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    /*
     * Set the orthographic projection so that its origin is in the bottom
     * left corner.  It needs to be there since we are drawing in window
     * coordinates.  We do not know the true size of the window but that
     * is okay since we can set the orthographic view so that the bottom
     * left corner is the origin and the top right corner is the top
     * right corner of the user's viewport.
     */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,
            (viewport[2]),
            0,
            (viewport[3]),
            -1,
            1);
    /*
     * Viewing projection is just the identity matrix since
     * we are drawing in window coordinates.
     */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    //std::cout << "Drawing \"" << qPrintable(text) << "\" at " << windowX << ", " << windowY << std::endl;
    if (annotationText.getText().isEmpty()) {
        return;
    }
    
    switch (annotationText.getOrientation()) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
            drawHorizontalTextAtWindowCoords(viewportX,
                                             viewportY,
                                             annotationText,
                                             flags);
            break;
        case AnnotationTextOrientationEnum::STACKED:
            drawVerticalTextAtWindowCoords(viewportX,
                                           viewportY,
                                           annotationText,
                                           flags);
            break;
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Draw annnotation text at the given model coordinates using
 * the the annotations attributes for the style of text.
 *
 * Depth testing is ENABLED when drawing text with this method.
 *
 * @param modelX
 *     Model X-coordinate.
 * @param modelY
 *     Model Y-coordinate.
 * @param modelZ
 *     Model Z-coordinate.
 * @param annotationText
 *     Annotation text and attributes.
 */
void
QGLWidgetTextRenderer::drawTextAtModelCoords(const double modelX,
                                          const double modelY,
                                          const double modelZ,
                                          const AnnotationText& annotationText,
                                             const BrainOpenGLTextRenderInterface::DrawingFlags& flags)
{
    setViewportHeight();
    
    GLdouble modelMatrix[16];
    GLdouble projectionMatrix[16];
    GLint viewport[4];
    
    glGetDoublev(GL_MODELVIEW_MATRIX,
                 modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX,
                 projectionMatrix);
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    
    /*
     * Project model coordinate to a window coordinate.
     */
    GLdouble windowX, windowY, windowZ;
    if (gluProject(modelX, modelY, modelZ,
                   modelMatrix, projectionMatrix, viewport,
                   &windowX, &windowY, &windowZ) == GL_TRUE) {
        glViewport(viewport[0],
                   viewport[1],
                   viewport[2],
                   viewport[3]);
//        /*
//         * Set the orthographic projection so that its origin is in the bottom
//         * left corner.  It needs to be there since we are drawing in window
//         * coordinates.  We do not know the true size of the window but that
//         * is okay since we can set the orthographic view so that the bottom
//         * left corner is the origin and the top right corner is the top
//         * right corner of the user's viewport.
//         */
//        glMatrixMode(GL_PROJECTION);
//        glPushMatrix();
//        glLoadIdentity();
//        glOrtho(0,
//                (viewport[2]),
//                0,
//                (viewport[3]),
//                -1,
//                1);
//        /*
//         * Viewing projection is just the identity matrix since
//         * we are drawing in window coordinates.
//         */
//        glMatrixMode(GL_MODELVIEW);
//        glPushMatrix();
//        glLoadIdentity();
       
        
        //        std::cout << "VP:" << AString::fromNumbers(viewport, 4, ",")
        //        << "   Window: " << windowX << ", " << windowY << std::endl;
        /*
         * Convert window coordinate to viewport coordinatde
         */
        const double x = windowX - viewport[0];
        const double y = windowY - viewport[1];
        drawTextAtViewportCoords(x,
                                 y,
                                 0.0,
                                 annotationText,
                                 flags);
//        glPopMatrix();
//        glMatrixMode(GL_PROJECTION);
//        glPopMatrix();
//        glMatrixMode(GL_MODELVIEW);
    }
    else {
        CaretLogSevere("gluProject() failed for drawing text at model coordinates.");
    }
}

/**
 * Draw horizontal annotation text at the given window coordinates.
 *
 * @param viewport
 *   The current viewport.
 * @param windowX
 *   X-coordinate in the window of first text character
 *   using the 'alignment'
 * @param windowY
 *   Y-coordinate in the window at which bottom of text is placed.
 * @param annotationText
 *   Annotation Text that is to be drawn.
 */
void
QGLWidgetTextRenderer::drawHorizontalTextAtWindowCoords(const double windowX,
                                                       const double windowY,
                                                       const AnnotationText& annotationText,
                                                        const BrainOpenGLTextRenderInterface::DrawingFlags& flags)
{
    QFont* font = findFont(annotationText,
                           false);
    if (! font) {
        return;
    }
    
    /*
     * Get the viewport
     */
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    
    const bool drawCrosshairsAtFontStartingCoordinate = false;
    if (drawCrosshairsAtFontStartingCoordinate) {
        GLfloat savedRGBA[4];
        glGetFloatv(GL_CURRENT_COLOR, savedRGBA);
        glColor3f(1.0, 0.0, 0.0);
        glLineWidth(1.0);
        glPushMatrix();
        glTranslatef(windowX, windowY, 0.0);
        glBegin(GL_LINES);
        glVertex2i(-50, 0);
        glVertex2i( 50, 0);
        glVertex2i(0, -50);
        glVertex2i(0,  50);
        glEnd();
        glPopMatrix();
        glColor3f(savedRGBA[0], savedRGBA[1], savedRGBA[2]);
    }
    
    const AString text = annotationText.getText();
    
    double bottomLeft[3], bottomRight[3], topRight[3], topLeft[3];
    getBoundsForTextAtViewportCoords(annotationText, flags, windowX, windowY, 0.0, viewport[2], viewport[3], bottomLeft, bottomRight, topRight, topLeft);
    
    double left   = bottomLeft[0];
    double right  = bottomRight[0];
    double bottom = bottomLeft[1];
    double top    = topLeft[1];
    
//    getTextBoundsInPixels(annotationText,
//                          left,
//                          right,
//                          bottom,
//                          top);
    
    double textOffsetX = 0;
    switch (annotationText.getHorizontalAlignment()) {
        case AnnotationTextAlignHorizontalEnum::CENTER:
            textOffsetX = -((right - left) / 2.0);
            break;
        case AnnotationTextAlignHorizontalEnum::LEFT:
            textOffsetX = -left;
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
            textOffsetX = -right;
            break;
    }
    
    double textOffsetY = 0;
//    switch (annotationText.getVerticalAlignment()) {
//        case AnnotationTextAlignVerticalEnum::BOTTOM:
//            textOffsetY = 0.0; //-bottom;
//            break;
//        case AnnotationTextAlignVerticalEnum::CENTER:
//            textOffsetY = ((top - bottom) / 2.0);
//            break;
//        case AnnotationTextAlignVerticalEnum::TOP:
//            textOffsetY = top;
//            break;
//    }
    switch (annotationText.getVerticalAlignment()) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            textOffsetY = -bottom;
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            textOffsetY = -((top - bottom) / 2.0);
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            textOffsetY = -top;
            break;
    }
    
//    /*
//     * Qt seems to place coordinate at TOP of text
//     */
//    const double height = top - bottom;
//    if (height > 0.0) {
//        const double oneCharHeight = (height / text.length());
//        textOffsetY -= oneCharHeight;
//    }
    
    double textX = windowX + textOffsetX;
    double textY = windowY + textOffsetY;
    
    if (drawCrosshairsAtFontStartingCoordinate) {
        std::cout << "BBox: (" << left << " " << bottom << ") (" << right << ", " << top << ")" << std::endl;
        
//        const float width  = right - left;
//        const float height = top   - bottom;
//        
//        GLfloat savedRGBA[4];
//        glGetFloatv(GL_CURRENT_COLOR, savedRGBA);
//        glColor3f(1.0, 0.0, 1.0);
//        glLineWidth(1.0);
//        glPushMatrix();
//        glRectf(textX, textY, textX + width, textY + height);
//        glPopMatrix();
//        glColor3f(savedRGBA[0], savedRGBA[1], savedRGBA[2]);
    }
    
    const double backgroundBounds[4] = {
        textX,
        textY,
        textX + (right - left),
        textY + (top   - bottom)
    };
    
    glPushMatrix();
    //glLoadIdentity();
    applyBackgroundColoring(annotationText,
                            backgroundBounds);
    applyForegroundColoring(annotationText);
    glPopMatrix();

    /*
     * Qt places origin at TOP LEFT
     */
//    const int qtWindowX = textX + viewport[0];
//    const int qtWindowY = m_glWidget->height() - textY + viewport[1];

    const int qtWindowX = textX + viewport[0];
    const int qtWindowY = m_glWidget->height() - textY - viewport[1];
    
    
    if (drawCrosshairsAtFontStartingCoordinate) {
        std::cout << "Drawing " << qPrintable(text) << " at XY: "
        << textX << ", " << textY << " qtXY=" << qtWindowX << ", " << qtWindowY
        << " windowXY=" << windowX << ", " << windowY << std::endl;

        GLdouble modelMatrix[16];
        GLdouble projectionMatrix[16];
        GLint viewport[4];
        
        glGetDoublev(GL_MODELVIEW_MATRIX,
                     modelMatrix);
        glGetDoublev(GL_PROJECTION_MATRIX,
                     projectionMatrix);
        glGetIntegerv(GL_VIEWPORT,
                      viewport);
        
        std::cout << std::endl;
        std::cout << "Drawing Text: " << qPrintable(annotationText.getText()) << std::endl;
        std::cout << "   Model Matrix: " << AString::fromNumbers(modelMatrix, 16, ",") << std::endl;
        std::cout << "   Proj Matrix:  " << AString::fromNumbers(projectionMatrix, 16, ",") << std::endl;
        std::cout << "   Viewport:     " << AString::fromNumbers(viewport, 4, ",") << std::endl;
        std::cout << std::endl;
    }
    m_glWidget->renderText(qtWindowX,
                           qtWindowY,
                           text,
                           *font);
}

/**
 * Draw vertical text at the given window coordinates.
 *
 * @param windowX
 *   X-coordinate in the window of first text character
 *   using the 'alignment'
 * @param windowY
 *   Y-coordinate in the window at which bottom of text is placed.
 * @param annotationText
 *   Text that is to be drawn.
 */
void
QGLWidgetTextRenderer::drawVerticalTextAtWindowCoords(const double windowX,
                                                     const double windowY,
                                                     const AnnotationText& annotationText,
                                                      const BrainOpenGLTextRenderInterface::DrawingFlags& flags)
{
    QFont* font = findFont(annotationText,
                           false);
    if ( ! font) {
        return;
    }
    
    /*
     * Get the viewport
     */
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    
    bool drawCrosshairsAtFontStartingCoordinate = false;
    if (drawCrosshairsAtFontStartingCoordinate) {
        GLfloat savedRGBA[4];
        glGetFloatv(GL_CURRENT_COLOR, savedRGBA);
        glColor3f(1.0, 0.0, 1.0);
        glLineWidth(1.0);
        glPushMatrix();
        //glTranslatef(windowX, windowY, 0.0);
        const double yStart = windowY - 50.0;
        const double yEnd   = windowY + 50.0;
        glBegin(GL_LINES);
        glVertex2d(windowX, yStart);
        glVertex2d(windowX, yEnd);
        glVertex2d(-50.0, windowY);
        glVertex2d( 50.0, windowY);
        glEnd();
        glPopMatrix();
        glColor3f(savedRGBA[0], savedRGBA[1], savedRGBA[2]);
    }
    
    double textMinX   = 0.0;
    double textMaxX   = 0.0;
    double textHeight = 0.0;
    std::vector<CharInfo> textCharsToDraw;
    getVerticalTextCharInfo(annotationText,
                            flags,
                            textMinX,
                            textMaxX,
                            textHeight,
                            textCharsToDraw);
    const double textBoundsWidth  = textMaxX - textMinX;
    const double textBoundsHeight = textHeight;
    
    double textOffsetX = 0;
    switch (annotationText.getHorizontalAlignment()) {
        case AnnotationTextAlignHorizontalEnum::CENTER:
            textOffsetX = -((textMaxX - textMinX) / 2.0);
            textOffsetX = 0.0;
            break;
        case AnnotationTextAlignHorizontalEnum::LEFT:
            textOffsetX = -textMinX;
            textOffsetX = (textBoundsWidth / 2.0);
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
            textOffsetX = -textMaxX;
            textOffsetX = -(textBoundsWidth / 2.0);
            break;
    }
    
    /*
     * The character coordinates are set so that the top of the first
     * will be at Y=0.
     */
    double textOffsetY = 0.0;
    double textBackgroundTopOffsetY = 0.0;
    switch (annotationText.getVerticalAlignment()) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            textOffsetY = textHeight;
            textBackgroundTopOffsetY = textHeight;
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            textOffsetY = (textHeight / 2.0);
            textBackgroundTopOffsetY = (textHeight / 2.0);
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            textOffsetY = 0.0;
            textBackgroundTopOffsetY = 0.0;
            break;
    }
    
    const double backMinX = windowX - (textBoundsWidth   / 2.0) + textOffsetX;
    const double backMaxX = backMinX + textBoundsWidth;
    
    const double backMaxY = windowY + textBackgroundTopOffsetY;
    const double backMinY = backMaxY - textBoundsHeight;
    const double backgroundBounds[4] = {
        backMinX,
        backMinY,
        backMaxX,
        backMaxY
    };
    
    applyBackgroundColoring(annotationText,
                            backgroundBounds);
    applyForegroundColoring(annotationText);
    
    for (std::vector<CharInfo>::const_iterator textIter = textCharsToDraw.begin();
         textIter != textCharsToDraw.end();
         textIter++) {
        const double charX = windowX + textIter->m_x + textOffsetX;
        const double charY = windowY + textIter->m_y + textOffsetY;
        
        const int qtWindowX = charX + viewport[0];
        const int qtWindowY = m_glWidget->height() - charY - viewport[1];
        
        m_glWidget->renderText(qtWindowX,
                               qtWindowY,
                               textIter->m_char,
                               *font);
    }
}

/**
 * Apply the background coloring by drawing a rectangle in the background
 * color that encloses the text.  If the background color is
 * invalid (alpha => 0), no action is taken.
 *
 * @param annotationText
 *   Annotation Text that is to be drawn.
 * @param textBoundsBox
 *   Bounding box for text (min-x, min-y, max-x, max-y)
 */
void
QGLWidgetTextRenderer::applyBackgroundColoring(const AnnotationText& annotationText,
                                              const double textBoundsBox[4])
{
//    GLdouble modelMatrix[16];
//    GLdouble projectionMatrix[16];
//    GLint viewport[4];
//    
//    glGetDoublev(GL_MODELVIEW_MATRIX,
//                 modelMatrix);
//    glGetDoublev(GL_PROJECTION_MATRIX,
//                 projectionMatrix);
//    glGetIntegerv(GL_VIEWPORT,
//                  viewport);
//    
//    std::cout << std::endl;
//    std::cout << "Background for:  " << qPrintable(annotationText.getText()) << std::endl;
//    std::cout << "   Rectangle:    " << AString::fromNumbers(textBoundsBox, 4, ",") << std::endl;
//    std::cout << "   Model Matrix: " << AString::fromNumbers(modelMatrix, 16, ",") << std::endl;
//    std::cout << "   Proj Matrix:  " << AString::fromNumbers(projectionMatrix, 16, ",") << std::endl;
//    std::cout << "   Viewport:     " << AString::fromNumbers(viewport, 4, ",") << std::endl;
//    std::cout << std::endl;
    
    float backgroundColor[4];
    annotationText.getBackgroundColorRGBA(backgroundColor);
    
    if (backgroundColor[3] > 0.0) {
        glColor4fv(backgroundColor);
        const double margin = s_textMarginSize * 2;
        glRectd(textBoundsBox[0] - margin,
                textBoundsBox[1] - margin,
                textBoundsBox[2] + margin,
                textBoundsBox[3] + margin);
    }
}

/**
 * Apply the foreground color.
 *
 * @param annotationText
 *   Annotation Text that is to be drawn.
 * @param textBoundsBox
 *   Bounding box for text (min-x, min-y, max-x, max-y)
 */
void
QGLWidgetTextRenderer::applyForegroundColoring(const AnnotationText& annotationText)
{
    float rgba[4];
    annotationText.getLineColorRGBA(rgba);
    glColor4fv(rgba);
}


/**
 * Get the estimated width and height of text (in pixels) using the given text
 * attributes.
 *
 * See http://ftgl.sourceforge.net/docs/html/metrics.png
 *
 * @param annotationText
 *   Text for width and height estimation.
 * @param viewportWidth
 *    Height of the viewport needed for percentage height text.
 * @param viewportHeight
 *    Height of the viewport needed for percentage height text.
 * @param widthOut
 *    Estimated width of text.
 * @param heightOut
 *    Estimated height of text.
 */
void
QGLWidgetTextRenderer::getTextWidthHeightInPixels(const AnnotationText& annotationText,
                                                  const BrainOpenGLTextRenderInterface::DrawingFlags& flags,
                                                  const double viewportWidth,
                                                  const double viewportHeight,
                                                 double& widthOut,
                                                 double& heightOut)
{
    setViewportHeight();
    
    double xMin = 0.0;
    double xMax = 0.0;
    double yMin = 0.0;
    double yMax = 0.0;
    
    double bottomLeft[3], bottomRight[3], topRight[3], topLeft[3];
    getBoundsForTextAtViewportCoords(annotationText,
                                     flags,
                                     0.0,
                                     0.0,
                                     0.0,
                                     viewportWidth,
                                     viewportHeight,
                                     bottomLeft,
                                     bottomRight,
                                     topRight,
                                     topLeft);
    
    widthOut  = xMax - xMin;
    heightOut = yMax - yMin;
}

/**
 * Get the bounds of text (in pixels) using the given text
 * attributes.
 *
 * See http://ftgl.sourceforge.net/docs/html/metrics.png
 *
 * @param annotationText
 *   Text that is to be drawn.
 * @param viewportX
 *    Viewport X-coordinate.
 * @param viewportY
 *    Viewport Y-coordinate.
 * @param viewportZ
 *    Viewport Z-coordinate.
 * @param viewportWidth
 *    Height of the viewport needed for percentage height text.
 * @param viewportHeight
 *    Height of the viewport needed for percentage height text.
 * @param bottomLeftOut
 *    The bottom left corner of the text bounds.
 * @param bottomRightOut
 *    The bottom right corner of the text bounds.
 * @param topRightOut
 *    The top right corner of the text bounds.
 * @param topLeftOut
 *    The top left corner of the text bounds.
 */
void
QGLWidgetTextRenderer::getBoundsForTextAtViewportCoords(const AnnotationText& annotationText,
                                                        const BrainOpenGLTextRenderInterface::DrawingFlags& flags,
                                              const double viewportX,
                                              const double viewportY,
                                              const double viewportZ,
                                                        const double /*viewportWidth*/,
                                                        const double /*viewportHeight*/,
                                                       double bottomLeftOut[3],
                                                       double bottomRightOut[3],
                                                       double topRightOut[3],
                                                       double topLeftOut[3])
{
    setViewportHeight();
    
    QFont* font = findFont(annotationText,
                           false);
    if (font == NULL) {
        return;
    }
    
    double xMin = 0.0;
    double xMax = 0.0;
    double yMin = 0.0;
    double yMax = 0.0;
    
    switch (annotationText.getOrientation()) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
        {
            QFontMetricsF fontMetrics(*font);
            QRectF boundsRect = fontMetrics.boundingRect(annotationText.getText());
            
            /*
             * Note: sometimes boundsRect.top() is negative and
             * that screws things up.
             */
            xMin = boundsRect.left();
            xMax = boundsRect.right();
            yMin = boundsRect.bottom();
            yMax = boundsRect.bottom() + boundsRect.height();
        }
            break;
        case AnnotationTextOrientationEnum::STACKED:
        {
            double textHeight = 0.0;
            std::vector<CharInfo> charInfo;
            getVerticalTextCharInfo(annotationText,
                                    flags,
                                    xMin,
                                    xMax,
                                    textHeight,
                                    charInfo);
            yMax = textHeight;
        }
            break;
    }
    
    const double width = xMax - xMin;
    double left = 0.0;
    switch (annotationText.getHorizontalAlignment()) {
        case AnnotationTextAlignHorizontalEnum::LEFT:
            left = viewportX;
            break;
        case AnnotationTextAlignHorizontalEnum::CENTER:
            left = viewportX - (width / 2.0);
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
            left = viewportX - width;
            break;
    }
    const double right = left + width;
    
    const double height = yMax - yMin;
    double bottom = 0.0;
    switch (annotationText.getVerticalAlignment()) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            bottom = viewportY;
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            bottom = viewportY - (height / 2.0);
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            bottom = viewportY - height;
            break;
    }
    const double top = bottom + height;
    
    bottomLeftOut[0] = left;
    bottomLeftOut[1] = bottom;
    bottomLeftOut[2] = viewportZ;

    bottomRightOut[0] = right;
    bottomRightOut[0] = bottom;
    bottomRightOut[0] = viewportZ;
    
    topRightOut[0] = right;
    topRightOut[0] = top;
    topRightOut[0] = viewportZ;
    
    topLeftOut[0] = left;
    topLeftOut[0] = top;
    topLeftOut[0] = viewportZ;
}

/**
 * Get the bounds of text (in pixels) using the given text
 * attributes.  NO MARGIN is placed around the text.
 *
 * See http://ftgl.sourceforge.net/docs/html/metrics.png
 *
 * @param annotationText
 *   Text that is to be drawn.
 * @param viewportX
 *    Viewport X-coordinate.
 * @param viewportY
 *    Viewport Y-coordinate.
 * @param viewportZ
 *    Viewport Z-coordinate.
 * @param viewportWidth
 *    Height of the viewport needed for percentage height text.
 * @param viewportHeight
 *    Height of the viewport needed for percentage height text.
 * @param bottomLeftOut
 *    The bottom left corner of the text bounds.
 * @param bottomRightOut
 *    The bottom right corner of the text bounds.
 * @param topRightOut
 *    The top right corner of the text bounds.
 * @param topLeftOut
 *    The top left corner of the text bounds.
 */
void
QGLWidgetTextRenderer::getBoundsWithoutMarginForTextAtViewportCoords(const AnnotationText& annotationText,
                                                                     const BrainOpenGLTextRenderInterface::DrawingFlags& flags,
                                                                     const double viewportX,
                                                                     const double viewportY,
                                                                     const double viewportZ,
                                                                     const double /*viewportWidth*/,
                                                                     const double /*viewportHeight*/,
                                                                     double bottomLeftOut[3],
                                                                     double bottomRightOut[3],
                                                                     double topRightOut[3],
                                                                     double topLeftOut[3])
{
    setViewportHeight();
    
    QFont* font = findFont(annotationText,
                           false);
    if (font == NULL) {
        return;
    }
    
    double xMin = 0.0;
    double xMax = 0.0;
    double yMin = 0.0;
    double yMax = 0.0;
    
    switch (annotationText.getOrientation()) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
        {
            QFontMetricsF fontMetrics(*font);
            QRectF boundsRect = fontMetrics.boundingRect(annotationText.getText());
            
            /*
             * Note: sometimes boundsRect.top() is negative and
             * that screws things up.
             */
            xMin = boundsRect.left();
            xMax = boundsRect.right();
            yMin = boundsRect.bottom();
            yMax = boundsRect.bottom() + boundsRect.height();
        }
            break;
        case AnnotationTextOrientationEnum::STACKED:
        {
            double textHeight = 0.0;
            std::vector<CharInfo> charInfo;
            getVerticalTextCharInfo(annotationText,
                                    flags,
                                    xMin,
                                    xMax,
                                    textHeight,
                                    charInfo);
            yMax = textHeight;
        }
            break;
    }
    
    const double width = xMax - xMin;
    double left = 0.0;
    switch (annotationText.getHorizontalAlignment()) {
        case AnnotationTextAlignHorizontalEnum::LEFT:
            left = viewportX;
            break;
        case AnnotationTextAlignHorizontalEnum::CENTER:
            left = viewportX - (width / 2.0);
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
            left = viewportX - width;
            break;
    }
    const double right = left + width;
    
    const double height = yMax - yMin;
    double bottom = 0.0;
    switch (annotationText.getVerticalAlignment()) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            bottom = viewportY;
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            bottom = viewportY - (height / 2.0);
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            bottom = viewportY - height;
            break;
    }
    const double top = bottom + height;
    
    bottomLeftOut[0] = left;
    bottomLeftOut[1] = bottom;
    bottomLeftOut[2] = viewportZ;
    
    bottomRightOut[0] = right;
    bottomRightOut[0] = bottom;
    bottomRightOut[0] = viewportZ;
    
    topRightOut[0] = right;
    topRightOut[0] = top;
    topRightOut[0] = viewportZ;
    
    topLeftOut[0] = left;
    topLeftOut[0] = top;
    topLeftOut[0] = viewportZ;
}

/**
 * Get the character info for drawing vertical text which includes
 * position for each of the characters.  The TOP of the first
 * character will be at Y=0.
 *
 * @param annotationText
 *   Text that is to be drawn.
 * @param xMinOut
 *    Minimum X of text.
 * @param xMaxOut
 *    Maximum X of text.
 * @param heightOut
 *    Total height of text.
 * @param charInfoOut
 *    Contains each character and its X, Y position
 *    for rendering vertically.
 */
void
QGLWidgetTextRenderer::getVerticalTextCharInfo(const AnnotationText& annotationText,
                                               const BrainOpenGLTextRenderInterface::DrawingFlags& flags,
                                              double& xMinOut,
                                              double& xMaxOut,
                                              double& heightOut,
                                              std::vector<CharInfo>& charInfoOut)
{
    charInfoOut.clear();
    xMinOut = 0.0;
    xMaxOut = 0.0;
    heightOut = 0.0;
    
    const AString text = (flags.isDrawSubstitutedText()
                          ? annotationText.getTextWithSubstitutionsApplied()
                          : annotationText.getText());
    const int32_t numChars = static_cast<int32_t>(text.size());
    if (numChars <= 0) {
        return;
    }
    
    QFont* font = findFont(annotationText,
                           false);
    if ( ! font) {
        return;
    }
    
    xMinOut =  std::numeric_limits<float>::max();
    xMaxOut = -std::numeric_limits<float>::max();
    
    double y =  0.0;
    const int32_t lastCharIndex = numChars - 1;
    for (int32_t i = 0; i < numChars; i++) {
        const QString oneChar = text[i];
        
        QFontMetricsF fontMetrics(*font);
        QRectF boundsRect = fontMetrics.boundingRect(oneChar);
        
        /*
         * Note: sometimes boundsRect.top() is negative and
         * that screws things up.
         */
        const double lowerX = boundsRect.left();
        const double upperX = boundsRect.right();
        const double lowerY = boundsRect.bottom();
        const double upperY = boundsRect.bottom() + boundsRect.height();
        
        const double width  = upperX - lowerX;
        
        xMinOut = std::min(xMinOut, lowerX);
        xMaxOut = std::max(xMaxOut, upperX);
        
        /*
         * Center the character horizontally.
         */
        const double xChar = -lowerX - (width / 2.0);
        
        /*
         * Want the top of character at the Y-coordinate.
         */
        const double yChar = y - upperY;
        
        charInfoOut.push_back(CharInfo(oneChar,
                                       xChar, yChar));
        
        const double height = upperY - lowerY;
        if (i == lastCharIndex) {
            y -= height;
        }
        else {
            const double heightWithSpacing = height + s_textMarginSize;
            y -= heightWithSpacing;
        }
    }
    
    heightOut = std::fabs(y);
}

/**
 * Set the height of the viewport.  This method must be called
 * at the beginning of all public methods.
 */
void
QGLWidgetTextRenderer::setViewportHeight()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    
    m_viewportWidth  = viewport[2];
    m_viewportHeight = viewport[3];
}

/**
 * Find a font with the given name, height, and style.
 * Once a font is created it is cached so that it can be
 * used again and avoids font creation which may be
 * expensive.
 *
 * @return a font
 */
QFont*
QGLWidgetTextRenderer::findFont(const AnnotationText& annotationText,
                                        const bool creatingDefaultFontFlag)

{
    const AString fontName = annotationText.getFontRenderingEncodedName(m_viewportWidth,
                                                                        m_viewportHeight);
    
    /*
     * Has the font already has been created?
     */
    FONT_MAP_ITERATOR fontIter = m_fontNameToFontMap.find(fontName);
    if (fontIter != m_fontNameToFontMap.end()) {
        FontData* fontData = fontIter->second;
        CaretAssert(fontData);
        return fontData->m_font;
    }
    
    /*
     * Create and save the font
     */
    FontData* fontData = new FontData(annotationText,
                                      m_viewportWidth,
                                      m_viewportHeight);
    if (fontData->m_valid) {
        /*
         * Request font is valid.
         */
        m_fontNameToFontMap.insert(std::make_pair(fontName,
                                                  fontData));
        CaretLogFine("Created font with encoded name "
                     + fontName);
        return fontData->m_font;
    }
    else {
        /*
         * Error creating font
         */
        delete fontData;
        fontData = NULL;
        
        /*
         * Issue a message about failure to create font but
         * don't print same message more than once.
         */
        if (std::find(m_failedFontNames.begin(),
                      m_failedFontNames.end(),
                      fontName) == m_failedFontNames.end()) {
            m_failedFontNames.insert(fontName);
            CaretLogSevere("Failed to create font with encoded name "
                           + fontName);
        }
    }
    
    /*
     * Were we trying to create the default font?
     */
    if (creatingDefaultFontFlag) {
        return NULL;
    }
    
    /*
     * Failed so use the default font.
     */
    return m_defaultFont;
}

/**
 * @return The font system is valid.
 */
bool
QGLWidgetTextRenderer::isValid() const
{
    return true;
}


/**
 * @return Name of the text renderer.
 */
AString QGLWidgetTextRenderer::getName() const
{
    return AString("Qt OpenGL Text Renderer");
}



/**
 * Constructs invalid font data.
 */
QGLWidgetTextRenderer::FontData::FontData()
{
    m_valid    = false;
    m_font     = NULL;
}

/**
 * Constructs a font with the given attributes.
 * Called should verify that this instance is valid (construction was successful).
 *
 * @param annotationText
 *   Annotation Text that is to be drawn.
 * @param viewportHeight
 *   Height of viewport.
 */
QGLWidgetTextRenderer::FontData::FontData(const AnnotationText&  annotationText,
                                          const int32_t viewportWidth,
                                          const int32_t viewportHeight)
{
    m_valid    = false;
    m_font     = NULL;
    
    const AnnotationTextFontNameEnum::Enum fontEnumName = annotationText.getFont();
    
    AString fontFileName = AnnotationTextFontNameEnum::getResourceFontFileName(fontEnumName);
    if (annotationText.isBoldStyleEnabled()
        && annotationText.isItalicStyleEnabled()) {
        fontFileName = AnnotationTextFontNameEnum::getResourceBoldItalicFontFileName(fontEnumName);
    }
    else if (annotationText.isBoldStyleEnabled()) {
        fontFileName = AnnotationTextFontNameEnum::getResourceBoldFontFileName(fontEnumName);
        
    }
    else if (annotationText.isItalicStyleEnabled()) {
        fontFileName = AnnotationTextFontNameEnum::getResourceItalicFontFileName(fontEnumName);
    }
    
    switch (fontEnumName) {
        case AnnotationTextFontNameEnum::LIBERTINE:
        case AnnotationTextFontNameEnum::VERA:
            fontFileName = "Helvetica";
            break;
        case AnnotationTextFontNameEnum::VERA_MONOSPACE:
            fontFileName = "Monaco";
            break;
    }
    
    CaretAssert( ! fontFileName.isEmpty());
    
    const QString fontName = AnnotationTextFontNameEnum::toGuiName(fontEnumName);
    
    m_font = new QFont(fontName);
    CaretAssert(m_font);
    
    const int32_t fontSizePoints = annotationText.getFontSizeForDrawing(viewportWidth,
                                                                        viewportHeight);
    
    m_font->setPointSize(fontSizePoints);
    m_font->setBold(annotationText.isBoldStyleEnabled());
    m_font->setItalic(annotationText.isItalicStyleEnabled());
    m_font->setUnderline(annotationText.isUnderlineStyleEnabled());
    
    m_valid = true;
    
    QFontInfo fontInfo(*m_font);
    CaretLogWarning("Requested font \""
                   + fontName
                   + "\" and actual font is \""
                   + fontInfo.family()
                   + " exact match="
                   + AString::fromBool(m_font->exactMatch()));

    if ( ! m_valid) {
        if (m_font != NULL) {
            delete m_font;
            m_font = NULL;
        }
    }
}

/**
 * Destructs font data.
 */
QGLWidgetTextRenderer::FontData::~FontData()
{
    if (m_font != NULL) {
        delete m_font;
        m_font = NULL;
    }
}

#endif // not - WORKBENCH_USE_QT5_QOPENGL_WIDGET

