
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

/**
 * Qt licence is included since some of its GLWidget API is copied.
 */
/****************************************************************************
 **
 ** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the QtOpenGL module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Digia.  For licensing terms and
 ** conditions see http://qt.digia.com/licensing.  For further information
 ** use the contact form at http://qt.digia.com/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Digia gives you certain additional
 ** rights.  These rights are described in the Digia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#define __FTGL_FONT_TEXT_RENDERER_DECLARE__
#include "FtglFontTextRenderer.h"
#undef __FTGL_FONT_TEXT_RENDERER_DECLARE__

#include <cmath>
#include <limits>

#include <QFile>

#include "AnnotationText.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOpenGLInclude.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"

#ifdef HAVE_FREETYPE
#include <FtglConfig.h>
#include <FTGL/ftgl.h>
using namespace FTGL;
#endif // HAVE_FREETYPE

using namespace caret;


    
/**
 * \class caret::FtglFontTextRenderer 
 * \brief Text rendering using FTGL
 * \ingroup Brain
 *
 * Draws text using the FTGL library.  
 *
 * FTGL's Texture font is used for drawing text.  
 * The texture font is positioned using the current OpenGL
 * model view matrix which allows rotation and scaling.
 * 
 * There are other font types such as Pixmap font.  The
 * pixmap font was used, but it cannot be rotated nor
 * scaled.  In addition, the pixmmap font drawing
 * requires a raster position and if the raster position 
 * is slightly outside the viewport all text is clipped.
 */
/**
 * Constructor.
 */
FtglFontTextRenderer::FtglFontTextRenderer()
: BrainOpenGLTextRenderInterface()
{
    m_defaultFont = NULL;
#ifdef HAVE_FREETYPE
    AnnotationText defaultAnnotationText;
    defaultAnnotationText.setFontSize(AnnotationFontSizeEnum::SIZE14);
    defaultAnnotationText.setFont(AnnotationFontNameEnum::VERA);
    defaultAnnotationText.setItalicEnabled(false);
    defaultAnnotationText.setBoldEnabled(false);
    defaultAnnotationText.setUnderlineEnabled(false);
    m_defaultFont = getFont(defaultAnnotationText,
                            true);
#endif // HAVE_FREETYPE
}

/**
 * Destructor.
 */
FtglFontTextRenderer::~FtglFontTextRenderer()
{
#ifdef HAVE_FREETYPE
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
#endif // HAVE_FREETYPE
}

/**
 * @return The font system is valid.
 */
bool
FtglFontTextRenderer::isValid() const
{
    return (m_defaultFont != NULL);
}

/*
 * Get the font with the given font attributes.
 * If the font is not created, return the default font.
 *
 * @param annotationText
 *   Annotation Text that is to be drawn.
 * @param creatingDefaultFontFlag
 *    True if creating the default font.
 * @return
 *    The FTGL font.  If there are errors this value will
 *    be NULL.
 */
FTFont*
FtglFontTextRenderer::getFont(const AnnotationText& annotationText,
                              const bool creatingDefaultFontFlag)
{
#ifdef HAVE_FREETYPE
    const AString fontName = annotationText.getFontRenderingEncodedName();
    
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
    FontData* fontData = new FontData(annotationText);
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
    
#else  // HAVE_FREETYPE
    CaretLogSevere("Trying to use FTGL Font rendering but FTGL is not valid.");
    return NULL;
#endif // HAVE_FREETYPE
}

/**
 * Draw annnotation text at the given viewport coordinates using
 * the the annotations attributes for the style of text.
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
FtglFontTextRenderer::drawTextAtViewportCoords(const double viewportX,
                                               const double viewportY,
                                               const double viewportZ,
                                               const AnnotationText& annotationText)
{
    //std::cout << "Drawing \"" << qPrintable(text) << "\" at " << windowX << ", " << windowY << std::endl;
    if (annotationText.getText().isEmpty()) {
        return;
    }
    
    switch (annotationText.getOrientation()) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
            drawHorizontalTextAtWindowCoords(viewportX,
                                             viewportY,
                                             annotationText);
            break;
        case AnnotationTextOrientationEnum::STACKED:
            drawVerticalTextAtWindowCoords(viewportX,
                                           viewportY,
                                           annotationText);
            break;
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
FtglFontTextRenderer::drawHorizontalTextAtWindowCoords(const double windowX,
                                                       const double windowY,
                                                       const AnnotationText& annotationText)
{
#ifdef HAVE_FREETYPE
    FTFont* font = getFont(annotationText,
                           false);
    if (! font) {
        return;
    }
    
    saveStateOfOpenGL();
    
    /*
     * Disable depth testing so text not occluded
     */
    glDisable(GL_DEPTH_TEST);
    
    /*
     * Get the viewport
     */
    GLint viewport[4];
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
    glLoadIdentity();
    
    const bool drawCrosshairsAtFontStartingCoordinate = false;
    if (drawCrosshairsAtFontStartingCoordinate) {
        GLfloat savedRGBA[4];
        glGetFloatv(GL_CURRENT_COLOR, savedRGBA);
        glColor3f(1.0, 0.0, 0.0);
        glLineWidth(1.0);
        glPushMatrix();
        glTranslatef(windowX, windowY, 0.0);
        glBegin(GL_LINES);
        glVertex2i(-20, 0);
        glVertex2i( 20, 0);
        glVertex2i(0, -20);
        glVertex2i(0,  20);
        glEnd();
        glPopMatrix();
        glColor3f(savedRGBA[0], savedRGBA[1], savedRGBA[2]);
    }
    
    
    
    
    
    double textX = 0.0;
    double textY = 0.0;
//    double textMaxX = 0.0;
//    double textMaxY = 0.0;
    
    double bottomLeftOut[3];
    double bottomRightOut[3];
    double topRightOut[3];
    double topLeftOut[3];
    getBoundsForHorizontalTextAtWindowCoords(annotationText,
                                             windowX, windowY, 0.0,
                                             bottomLeftOut, bottomRightOut, topRightOut, topLeftOut);
    textX = bottomLeftOut[0];
    textY = bottomLeftOut[1];
//    const double backgroundBounds[4] = {
//        textX,
//        textY,
//        textMaxX,
//        textMaxY
//    };
    
    const AString text = annotationText.getText();
    
//    const FTBBox  bbox  = font->BBox(text.toAscii().constData());
//    const FTPoint lower = bbox.Lower();
//    const FTPoint upper = bbox.Upper();
//    
//    double textOffsetX = 0;
//    switch (annotationText.getHorizontalAlignment()) {
//        case AnnotationTextAlignHorizontalEnum::CENTER:
//            textOffsetX = -((upper.X() - lower.X()) / 2.0);
//            break;
//        case AnnotationTextAlignHorizontalEnum::LEFT:
//            textOffsetX = -lower.X();
//            break;
//        case AnnotationTextAlignHorizontalEnum::RIGHT:
//            textOffsetX = -upper.X();
//            break;
//    }
//    
//    
//    double textOffsetY = 0;
//    switch (annotationText.getVerticalAlignment()) {
//        case AnnotationTextAlignVerticalEnum::BOTTOM:
//            textOffsetY = -lower.Y();
//            break;
//        case AnnotationTextAlignVerticalEnum::MIDDLE:
//            textOffsetY = -((upper.Y() - lower.Y()) / 2.0);
//            break;
//        case AnnotationTextAlignVerticalEnum::TOP:
//            textOffsetY = -upper.Y();
//            break;
//    }
//    
//    double textX = windowX + textOffsetX;
//    double textY = windowY + textOffsetY;
//    
//    const double backgroundBounds[4] = {
//        textX,
//        textY,
//        textX + (upper.X() - lower.X()),
//        textY + (upper.Y() - lower.Y())
//    };
    
//    if (drawCrosshairsAtFontStartingCoordinate) {
////        std::cout << "BBox: (" << lower.Xf() << " " << lower.Yf() << ") (" << upper.Xf() << ", " << upper.Yf() << ")" << std::endl;
////        
////        const float width  = upper.Xf() - lower.Xf();
////        const float height = upper.Yf() - lower.Yf();
//        
//        GLfloat savedRGBA[4];
//        glGetFloatv(GL_CURRENT_COLOR, savedRGBA);
//        glColor3f(1.0, 0.0, 1.0);
//        glLineWidth(1.0);
//        glPushMatrix();
//        glRectf(textX, textY, textMaxX, textMaxY);
////        glRectf(textX, textY, textX + width, textY + height);
//        glPopMatrix();
//        glColor3f(savedRGBA[0], savedRGBA[1], savedRGBA[2]);
//    }
    
    
    glPushMatrix();
    glLoadIdentity();
    applyBackgroundColoring(annotationText,
                            bottomLeftOut, bottomRightOut, topRightOut, topLeftOut);
    applyForegroundColoring(annotationText);
    
    const double rotationAngle = annotationText.getRotationAngle();
    if (rotationAngle != 0.0) {
        glTranslated(textX,
                     textY,
                     0.0);
        glRotated(rotationAngle, 0.0, 0.0, -1.0);
//        glTranslated(-textX,
//                     -textY,
//                     0.0);
        font->Render(text.toAscii().constData());
    }
    else {
        glTranslated(textX,
                     textY,
                     0.0);
        font->Render(text.toAscii().constData());
    }
    glPopMatrix();
    
    restoreStateOfOpenGL();
#else // HAVE_FREETYPE
    CaretLogSevere("Trying to use FTGL Font rendering but it cannot be used due to FreeType not found.");
#endif // HAVE_FREETYPE
}

/**
 * Get the bounds of text (in pixels) using the given text
 * attributes.  A margin is included around the text.
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
FtglFontTextRenderer::getBoundsForTextAtViewportCoords(const AnnotationText& annotationText,
                                                     const double viewportX,
                                                     const double viewportY,
                                                     const double viewportZ,
                                                       double bottomLeftOut[3],
                                                       double bottomRightOut[3],
                                                       double topRightOut[3],
                                                       double topLeftOut[3])
{
    switch (annotationText.getOrientation()) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
            getBoundsForHorizontalTextAtWindowCoords(annotationText,
                                                     viewportX,
                                                     viewportY,
                                                     viewportZ,
                                                     bottomLeftOut,
                                                     bottomRightOut,
                                                     topRightOut,
                                                     topLeftOut);
            break;
        case AnnotationTextOrientationEnum::STACKED:
        {
            std::vector<CharInfo> charInfo;
            getBoundsForVerticalTextAtWindowCoords(annotationText,
                                                   viewportX,
                                                   viewportY,
                                                   viewportZ,
                                                   bottomLeftOut,
                                                   bottomRightOut,
                                                   topRightOut,
                                                   topLeftOut,
                                                   charInfo);
        }
            break;
    }
    
//    xMinOut -= s_textMarginSize;
//    xMaxOut += s_textMarginSize;
//    yMinOut -= s_textMarginSize;
//    yMaxOut += s_textMarginSize;
}

/**
 * Get the bounds of horizontal text (in pixels) using the given text
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
FtglFontTextRenderer::getBoundsForHorizontalTextAtWindowCoords(const AnnotationText& annotationText,
                                                               const double viewportX,
                                                               const double viewportY,
                                                               const double viewportZ,
                                                               double bottomLeftOut[3],
                                                               double bottomRightOut[3],
                                                               double topRightOut[3],
                                                               double topLeftOut[3])
{
    FTFont* font = getFont(annotationText,
                           false);
    if (! font) {
        return;
    }
    
    const AString text = annotationText.getText();
    
    const FTBBox  bbox  = font->BBox(text.toAscii().constData());
    const FTPoint lower = bbox.Lower();
    const FTPoint upper = bbox.Upper();
    
    double textOffsetX = 0;
    switch (annotationText.getHorizontalAlignment()) {
        case AnnotationTextAlignHorizontalEnum::CENTER:
            textOffsetX = -((upper.X() - lower.X()) / 2.0);
            break;
        case AnnotationTextAlignHorizontalEnum::LEFT:
            textOffsetX = -lower.X();
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
            textOffsetX = -upper.X();
            break;
    }
    
    double textOffsetY = 0;
    switch (annotationText.getVerticalAlignment()) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            textOffsetY = -lower.Y();
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            textOffsetY = -((upper.Y() - lower.Y()) / 2.0);
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            textOffsetY = -upper.Y();
            break;
    }
    
    const double textMinX = viewportX + textOffsetX;
    const double textMaxX = textMinX + (upper.X() - lower.X());
    const double textMinY = viewportY + textOffsetY;
    const double textMaxY = textMinY + (upper.Y() - lower.Y());
    
//    const double = textX;
//    xMaxOut = textX + (upper.X() - lower.X());
//    yMinOut = textY;
//    yMaxOut = textY + (upper.Y() - lower.Y());
    
    bottomLeftOut[0]  = textMinX;
    bottomLeftOut[1]  = textMinY;
    bottomLeftOut[2]  = 0.0;
    bottomRightOut[0] = textMaxX;
    bottomRightOut[1] = textMinY;
    bottomRightOut[2] = 0.0;
    topRightOut[0]    = textMaxX;
    topRightOut[1]    = textMaxY;
    topRightOut[2]    = 0.0;
    topLeftOut[0]     = textMinX;
    topLeftOut[1]     = textMaxY;
    topLeftOut[2]     = 0.0;
    
    const double rotationAngle = annotationText.getRotationAngle();
    if (rotationAngle != 0.0) {
        Matrix4x4 matrix;
        matrix.translate(-bottomLeftOut[0], -bottomLeftOut[1], -bottomLeftOut[2]);
        matrix.rotateZ(-rotationAngle);
        matrix.translate(bottomLeftOut[0], bottomLeftOut[1], bottomLeftOut[2]);
        matrix.multiplyPoint3(bottomLeftOut);
        matrix.multiplyPoint3(bottomRightOut);
        matrix.multiplyPoint3(topRightOut);
        matrix.multiplyPoint3(topLeftOut);
    }
    
//    const double backgroundBounds[4] = {
//        textX,
//        textY,
//        textX + (upper.X() - lower.X()),
//        textY + (upper.Y() - lower.Y())
//    };
}

/**
 * Get the bounds of vertical text (in pixels) using the given text
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
 * @param bottomLeftOut
 *    The bottom left corner of the text bounds.
 * @param bottomRightOut
 *    The bottom right corner of the text bounds.
 * @param topRightOut
 *    The top right corner of the text bounds.
 * @param topLeftOut
 *    The top left corner of the text bounds.
 * @param textCharsToDraw
 *    Output character drawing information.
 */
void
FtglFontTextRenderer::getBoundsForVerticalTextAtWindowCoords(const AnnotationText& annotationText,
                                                             const double viewportX,
                                                             const double viewportY,
                                                             const double viewportZ,
                                                             double bottomLeftOut[3],
                                                             double bottomRightOut[3],
                                                             double topRightOut[3],
                                                             double topLeftOut[3],
                                                             std::vector<CharInfo>& textCharsToDraw)
{
    double textMinX   = 0.0;
    double textMaxX   = 0.0;
    double textHeight = 0.0;
    getVerticalTextCharInfo(annotationText,
                            textMinX,
                            textMaxX,
                            textHeight,
                            textCharsToDraw);
    const double textBoundsWidth  = textMaxX - textMinX;
    const double textBoundsHeight = textHeight;
    
    double textOffsetX = 0;
    switch (annotationText.getHorizontalAlignment()) {
        case AnnotationTextAlignHorizontalEnum::LEFT:
            textOffsetX = (textBoundsWidth / 2.0);
            break;
        case AnnotationTextAlignHorizontalEnum::CENTER:
            textOffsetX = 0;
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
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
            textOffsetY = textBoundsHeight;
            textBackgroundTopOffsetY = textBoundsHeight;
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            textOffsetY = (textBoundsHeight / 2.0);
            textBackgroundTopOffsetY = (textBoundsHeight / 2.0);
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            textOffsetY = 0.0;
            textBackgroundTopOffsetY = 0.0;
            break;
    }
    
    const double backMinX = viewportX - (textBoundsWidth   / 2.0) + textOffsetX;
    const double backMaxX = backMinX + textBoundsWidth;
    
    const double backMaxY = viewportY + textBackgroundTopOffsetY;
    const double backMinY = backMaxY - textBoundsHeight;
    
//    xMinOut = backMinX;
//    xMaxOut = backMaxX;
//    yMinOut = backMinY;
//    yMaxOut = backMaxY;
    
    bottomLeftOut[0]  = backMinX;
    bottomLeftOut[1]  = backMinY;
    bottomLeftOut[2]  = 0.0;
    bottomRightOut[0] = backMaxX;
    bottomRightOut[1] = backMinY;
    bottomRightOut[2] = 0.0;
    topRightOut[0]    = backMaxX;
    topRightOut[1]    = backMaxY;
    topRightOut[2]    = 0.0;
    topLeftOut[0]     = backMinX;
    topLeftOut[1]     = backMaxY;
    topLeftOut[2]     = 0.0;
    
    const double rotationAngle = annotationText.getRotationAngle();
    if (rotationAngle != 0.0) {
        float translateFirstChar[3] = { 0.0, 0.0, 0.0 };
        if (! textCharsToDraw.empty()) {
            translateFirstChar[0] = textCharsToDraw[0].m_x;
            translateFirstChar[1] = textCharsToDraw[0].m_y;
        }
//        const double zeroCharPos[3] = {
//            (topLeftOut[0] + topRightOut[0]) / 2.0,
//            (topLeftOut[1] + topRightOut[1]) / 2.0,
//            (topLeftOut[2] + topRightOut[2]) / 2.0,
//        };
        const double zeroCharPos[3] = {
            0.0,
            0.0,
            0.0
        };
        
//        const double translateValue[3] = {
//            -topRightOut[0] + translateFirstChar[0],
//            -topRightOut[1] + translateFirstChar[1],
//            -topRightOut[2] + translateFirstChar[2]
//        };
//        const double translateValue[3] = {
//            -topLeftOut[0] + translateFirstChar[0],
//            -topLeftOut[1] - translateFirstChar[1],
//            -topLeftOut[2] - translateFirstChar[2]
//        };
        const double translateValue[3] = {
            -(topLeftOut[0] + topRightOut[0]) / 2.0,
            -(topLeftOut[1] + topRightOut[1]) / 2.0,
            -(topLeftOut[2] + topRightOut[2]) / 2.0
        };
        
        Matrix4x4 matrix;
//        matrix.translate(-bottomLeftOut[0], -bottomLeftOut[1], -bottomLeftOut[2]);
//        matrix.rotateZ(-rotationAngle);
//        matrix.translate(bottomLeftOut[0], bottomLeftOut[1], bottomLeftOut[2]);
        matrix.translate(translateValue[0], translateValue[1], translateValue[2]);
        matrix.rotateZ(-rotationAngle);
        matrix.translate(-translateValue[0], -translateValue[1], -translateValue[2]);
        matrix.multiplyPoint3(bottomLeftOut);
        matrix.multiplyPoint3(bottomRightOut);
        matrix.multiplyPoint3(topRightOut);
        matrix.multiplyPoint3(topLeftOut);
        
        const int32_t numChars = static_cast<int32_t>(textCharsToDraw.size());
        if (numChars > 1) {
            Matrix4x4 characterRotationMatrix;
            characterRotationMatrix.rotateZ(-rotationAngle);
            
            
            std::vector<CharInfo> rotatedChars;
            double previousCharacterXYZ[3] = { zeroCharPos[0], zeroCharPos[1], zeroCharPos[2] };
            for (int32_t i = 0; i < numChars; i++) {
                const double x1 = ((i > 0) ? textCharsToDraw[i-1].m_x : zeroCharPos[0]);
                const double y1 = ((i > 0) ? textCharsToDraw[i-1].m_y : zeroCharPos[1]);
                const double z1 = zeroCharPos[2];
                const double x2 = textCharsToDraw[i].m_x;
                const double y2 = textCharsToDraw[i].m_y;
                const double dx = x2 - x1;
                const double dy = y2 - y1;
                const double dz = viewportZ;
                
                double dXYZ[3] = { dx, dy, dz };
                characterRotationMatrix.multiplyPoint3(dXYZ);
                const double rotXYZ[3] = {
                    previousCharacterXYZ[0] + dXYZ[0],
                    previousCharacterXYZ[1] + dXYZ[1],
                    previousCharacterXYZ[2] + dXYZ[2]
                };
                rotatedChars.push_back(CharInfo(textCharsToDraw[i].m_char, rotXYZ[0], rotXYZ[1]));
                
                
                previousCharacterXYZ[0] = rotXYZ[0];
                previousCharacterXYZ[1] = rotXYZ[1];
                previousCharacterXYZ[2] = rotXYZ[2];
            }
            
            CaretAssert(textCharsToDraw.size() == rotatedChars.size());
            CaretAssert(static_cast<int32_t>(rotatedChars.size()) == numChars);
            std::cout << std::endl;
            for (int32_t i = 0; i < numChars; i++) {
                std::cout << "Char '" << textCharsToDraw[i].m_char << "' ("
                << textCharsToDraw[i].m_x << ", " << textCharsToDraw[i].m_y << ") rotated to ("
                << rotatedChars[i].m_x << ", " << rotatedChars[i].m_y
                << ")" << std::endl;
            }
            
            textCharsToDraw.clear();
            textCharsToDraw.insert(textCharsToDraw.end(),
                                   rotatedChars.begin(),
                                   rotatedChars.end());
        }
    }
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
FtglFontTextRenderer::drawVerticalTextAtWindowCoords(const double windowX,
                                                     const double windowY,
                                                     const AnnotationText& annotationText)
{
#ifdef HAVE_FREETYPE
    FTFont* font = getFont(annotationText,
                           false);
    if ( ! font) {
        return;
    }
    
    saveStateOfOpenGL();
    
    /*
     * Disable depth testing so text not occluded
     */
    glDisable(GL_DEPTH_TEST);
    
    /*
     * Get the viewport
     */
    GLint viewport[4];
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
    glLoadIdentity();
    
    bool drawCrosshairsAtFontStartingCoordinate = false;
    if (drawCrosshairsAtFontStartingCoordinate) {
        GLfloat savedRGBA[4];
        glGetFloatv(GL_CURRENT_COLOR, savedRGBA);
        glColor3f(1.0, 0.0, 1.0);
        glLineWidth(1.0);
        glPushMatrix();
        //glTranslatef(windowX, windowY, 0.0);
        const double xStart = windowX - 50.0;
        const double xEnd   = windowX + 50.0;
        const double yStart = windowY - 50.0;
        const double yEnd   = windowY + 50.0;
        glBegin(GL_LINES);
        glVertex2d(windowX, yStart);
        glVertex2d(windowX, yEnd);
        glVertex2d(xStart,  windowY);
        glVertex2d(xEnd,    windowY);
        glEnd();
        glPopMatrix();
        glColor3f(savedRGBA[0], savedRGBA[1], savedRGBA[2]);
    }
    
//    double textMinX   = 0.0;
//    double textMaxX   = 0.0;
//    double textHeight = 0.0;
//    std::vector<CharInfo> textCharsToDraw;
//    getVerticalTextCharInfo(annotationText,
//                            textMinX,
//                            textMaxX,
//                            textHeight,
//                            textCharsToDraw);
//    const double textBoundsWidth  = textMaxX - textMinX;
//    const double textBoundsHeight = textHeight;
//    
//    double textOffsetX = 0;
//    switch (annotationText.getHorizontalAlignment()) {
//        case AnnotationTextAlignHorizontalEnum::LEFT:
//            textOffsetX = (textBoundsWidth / 2.0);
//            break;
//        case AnnotationTextAlignHorizontalEnum::CENTER:
//            textOffsetX = 0;
//            break;
//        case AnnotationTextAlignHorizontalEnum::RIGHT:
//            textOffsetX = -(textBoundsWidth / 2.0);
//            break;
//    }
//    
//    /*
//     * The character coordinates are set so that the top of the first
//     * will be at Y=0.
//     */
//    double textOffsetY = 0.0;
//    double textBackgroundTopOffsetY = 0.0;
//    switch (annotationText.getVerticalAlignment()) {
//        case AnnotationTextAlignVerticalEnum::BOTTOM:
//            textOffsetY = textBoundsHeight;
//            textBackgroundTopOffsetY = textBoundsHeight;
//            break;
//        case AnnotationTextAlignVerticalEnum::MIDDLE:
//            textOffsetY = (textBoundsHeight / 2.0);
//            textBackgroundTopOffsetY = (textBoundsHeight / 2.0);
//            break;
//        case AnnotationTextAlignVerticalEnum::TOP:
//            textOffsetY = 0.0;
//            textBackgroundTopOffsetY = 0.0;
//            break;
//    }
//    
//    const double backMinX = windowX - (textBoundsWidth   / 2.0) + textOffsetX;
//    const double backMaxX = backMinX + textBoundsWidth;
//    
//    const double backMaxY = windowY + textBackgroundTopOffsetY;
//    const double backMinY = backMaxY - textBoundsHeight;
//    const double backgroundBounds[4] = {
//        backMinX,
//        backMinY,
//        backMaxX,
//        backMaxY
//    };
    
    
    
    double bottomLeftOut[3];
    double bottomRightOut[3];
    double topRightOut[3];
    double topLeftOut[3];
    std::vector<CharInfo> textCharsToDraw;
//    double xMin = 0.0;
//    double xMax = 0.0;
//    double yMin = 0.0;
//    double yMax = 0.0;
    getBoundsForVerticalTextAtWindowCoords(annotationText,
                                           windowX,
                                           windowY,
                                           0.0,
                                           bottomLeftOut,
                                           bottomRightOut,
                                           topRightOut,
                                           topLeftOut,
                                           textCharsToDraw);
//    const double backgroundBounds[4] = {
//        xMin,
//        yMin,
//        xMax,
//        yMax
//    };
    
    
//    const double textBoundsWidth  = (xMax - xMin);
//    const double textBoundsHeight = (yMax - yMin);
    const double textBoundsWidth  = MathFunctions::distance3D(bottomLeftOut,
                                                              bottomRightOut);
    const double textBoundsHeight = MathFunctions::distance3D(bottomLeftOut,
                                                              topLeftOut);
    double textOffsetX = 0;
    switch (annotationText.getHorizontalAlignment()) {
        case AnnotationTextAlignHorizontalEnum::LEFT:
            textOffsetX = (textBoundsWidth / 2.0);
            break;
        case AnnotationTextAlignHorizontalEnum::CENTER:
            textOffsetX = 0;
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
            textOffsetX = -(textBoundsWidth / 2.0);
            break;
    }
    
    /*
     * The character coordinates are set so that the top of the first
     * will be at Y=0.
     */
    double textOffsetY = 0.0;
    switch (annotationText.getVerticalAlignment()) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            textOffsetY = textBoundsHeight;
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            textOffsetY = (textBoundsHeight / 2.0);
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            textOffsetY = 0.0;
            break;
    }
    
    
    
    applyBackgroundColoring(annotationText,
                            bottomLeftOut, bottomRightOut, topRightOut, topLeftOut);
    applyForegroundColoring(annotationText);
    
    const double rotationAngle = annotationText.getRotationAngle();
    
    for (std::vector<CharInfo>::const_iterator textIter = textCharsToDraw.begin();
         textIter != textCharsToDraw.end();
         textIter++) {
        const double charX = windowX + textIter->m_x + textOffsetX;
        const double charY = windowY + textIter->m_y + textOffsetY;
        
        glPushMatrix();
        glTranslated(charX,
                     charY,
                     0.0);
        if (rotationAngle != 0.0) {
            glRotated(rotationAngle, 0.0, 0.0, -1.0);
        }
        
        font->Render(textIter->m_char.toAscii().constData());
        glPopMatrix();
    }
    
    restoreStateOfOpenGL();
#else // HAVE_FREETYPE
    CaretLogSevere("Trying to use FTGL Font rendering but it cannot be used due to FreeType not found.");
#endif // HAVE_FREETYPE
}

///**
// * Draw vertical text at the given window coordinates.
// *
// * @param windowX
// *   X-coordinate in the window of first text character
// *   using the 'alignment'
// * @param windowY
// *   Y-coordinate in the window at which bottom of text is placed.
// * @param annotationText
// *   Text that is to be drawn.
// */
//void
//FtglFontTextRenderer::drawVerticalTextAtWindowCoords(const double windowX,
//                                                     const double windowY,
//                                                     const AnnotationText& annotationText)
//{
//#ifdef HAVE_FREETYPE
//    FTFont* font = getFont(annotationText,
//                           false);
//    if ( ! font) {
//        return;
//    }
//    
//    saveStateOfOpenGL();
//    
//    /*
//     * Disable depth testing so text not occluded
//     */
//    glDisable(GL_DEPTH_TEST);
//    
//    /*
//     * Get the viewport
//     */
//    GLint viewport[4];
//    glGetIntegerv(GL_VIEWPORT,
//                  viewport);
//    
//    /*
//     * Set the orthographic projection so that its origin is in the bottom
//     * left corner.  It needs to be there since we are drawing in window
//     * coordinates.  We do not know the true size of the window but that
//     * is okay since we can set the orthographic view so that the bottom
//     * left corner is the origin and the top right corner is the top
//     * right corner of the user's viewport.
//     */
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(0,
//            (viewport[2]),
//            0,
//            (viewport[3]),
//            -1,
//            1);
//    
//    /*
//     * Viewing projection is just the identity matrix since
//     * we are drawing in window coordinates.
//     */
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    
//    bool drawCrosshairsAtFontStartingCoordinate = false;
//    if (drawCrosshairsAtFontStartingCoordinate) {
//        GLfloat savedRGBA[4];
//        glGetFloatv(GL_CURRENT_COLOR, savedRGBA);
//        glColor3f(1.0, 0.0, 1.0);
//        glLineWidth(1.0);
//        glPushMatrix();
//        //glTranslatef(windowX, windowY, 0.0);
//        const double yStart = windowY - 50.0;
//        const double yEnd   = windowY + 50.0;
//        glBegin(GL_LINES);
//        glVertex2d(windowX, yStart);
//        glVertex2d(windowX, yEnd);
//        glVertex2d(-50.0, windowY);
//        glVertex2d( 50.0, windowY);
//        glEnd();
//        glPopMatrix();
//        glColor3f(savedRGBA[0], savedRGBA[1], savedRGBA[2]);
//    }
//    
//    double textMinX   = 0.0;
//    double textMaxX   = 0.0;
//    double textHeight = 0.0;
//    std::vector<CharInfo> textCharsToDraw;
//    getVerticalTextCharInfo(annotationText,
//                            textMinX,
//                            textMaxX,
//                            textHeight,
//                            textCharsToDraw);
//    const double textBoundsWidth  = textMaxX - textMinX;
//    const double textBoundsHeight = textHeight;
//    
//    double textOffsetX = 0;
//    switch (annotationText.getHorizontalAlignment()) {
//        case AnnotationTextAlignHorizontalEnum::LEFT:
//            textOffsetX = (textBoundsWidth / 2.0);
//            break;
//        case AnnotationTextAlignHorizontalEnum::CENTER:
//            textOffsetX = 0;
//            break;
//        case AnnotationTextAlignHorizontalEnum::RIGHT:
//            textOffsetX = -(textBoundsWidth / 2.0);
//            break;
//    }
//    
//    /*
//     * The character coordinates are set so that the top of the first
//     * will be at Y=0.
//     */
//    double textOffsetY = 0.0;
//    double textBackgroundTopOffsetY = 0.0;
//    switch (annotationText.getVerticalAlignment()) {
//        case AnnotationTextAlignVerticalEnum::BOTTOM:
//            textOffsetY = textBoundsHeight;
//            textBackgroundTopOffsetY = textBoundsHeight;
//            break;
//        case AnnotationTextAlignVerticalEnum::MIDDLE:
//            textOffsetY = (textBoundsHeight / 2.0);
//            textBackgroundTopOffsetY = (textBoundsHeight / 2.0);
//            break;
//        case AnnotationTextAlignVerticalEnum::TOP:
//            textOffsetY = 0.0;
//            textBackgroundTopOffsetY = 0.0;
//            break;
//    }
//    
//    const double backMinX = windowX - (textBoundsWidth   / 2.0) + textOffsetX;
//    const double backMaxX = backMinX + textBoundsWidth;
//    
//    const double backMaxY = windowY + textBackgroundTopOffsetY;
//    const double backMinY = backMaxY - textBoundsHeight;
//    const double backgroundBounds[4] = {
//        backMinX,
//        backMinY,
//        backMaxX,
//        backMaxY
//    };
//    
//    applyBackgroundColoring(annotationText,
//                            backgroundBounds);
//    applyForegroundColoring(annotationText);
//    
//    for (std::vector<CharInfo>::const_iterator textIter = textCharsToDraw.begin();
//         textIter != textCharsToDraw.end();
//         textIter++) {
//        const double charX = windowX + textIter->m_x + textOffsetX;
//        const double charY = windowY + textIter->m_y + textOffsetY;
//        
//        glPushMatrix();
//        glTranslated(charX,
//                     charY,
//                     0.0);
//        font->Render(textIter->m_char.toAscii().constData());
//        glPopMatrix();
//    }
//    
//    restoreStateOfOpenGL();
//#else // HAVE_FREETYPE
//    CaretLogSevere("Trying to use FTGL Font rendering but it cannot be used due to FreeType not found.");
//#endif // HAVE_FREETYPE
//}

/**
 * Apply the background coloring by drawing a rectangle in the background
 * color that encloses the text.  If the background color is
 * invalid (alpha => 0), no action is taken.
 *
 * @param annotationText
 *   Annotation Text that is to be drawn.
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
FtglFontTextRenderer::applyBackgroundColoring(const AnnotationText& annotationText,
                                              const double bottomLeftOut[3],
                                              const double bottomRightOut[3],
                                              const double topRightOut[3],
                                              const double topLeftOut[3])
{
    float backgroundColor[4];
    annotationText.getBackgroundColor(backgroundColor);
    if (backgroundColor[3] > 0.0) {
//        /*
//         * These spacings were created by drawing a box around
//         * the text and adjusting the spaceing until it appeared
//         * reasonable.
//         */
//        double spaceLeft   = 0.0;
//        double spaceRight  = 0.0;
//        double spaceBottom = 0.0;
//        double spaceTop    = 0.0;
//        switch (annotationText.getOrientation()) {
//            case AnnotationTextOrientationEnum::HORIZONTAL:
//                spaceLeft   =  3.0;
//                spaceRight  =  3.0;
//                spaceBottom =  3.0;
//                spaceTop    =  3.0;
//                break;
//            case AnnotationTextOrientationEnum::STACKED:
//                spaceLeft   =  3.0;
//                spaceRight  =  3.0;
//                spaceBottom =  3.0;
//                spaceTop    =  3.0;
//                break;
//        }
        
        glColor4fv(backgroundColor);
        glBegin(GL_TRIANGLE_STRIP);
        glVertex3dv(bottomLeftOut);
        glVertex3dv(bottomRightOut);
        glVertex3dv(topLeftOut);
        glVertex3dv(topRightOut);
        glEnd();
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
FtglFontTextRenderer::applyForegroundColoring(const AnnotationText& annotationText)
{
    const float* foregroundColor = annotationText.getForegroundColor();
    glColor4fv(foregroundColor);
}

/**
 * Get the bounds of text (in pixels) using the given text
 * attributes.
 *
 * See http://ftgl.sourceforge.net/docs/html/metrics.png
 *
 * @param annotationText
 *   Text that is to be drawn.
 * @param xMinOut
 *    Minimum X of text.
 * @param xMaxOut
 *    Maximum X of text.
 * @param yMinOut
 *    Minimum Y of text.
 * @param yMaxOut
 *    Maximum Y of text.
 */
void
FtglFontTextRenderer::getTextBoundsInPixels(const AnnotationText& annotationText,
                                            double& xMinOut,
                                            double& xMaxOut,
                                            double& yMinOut,
                                            double& yMaxOut)
{
    xMinOut = 0.0;
    xMaxOut = 0.0;
    yMinOut = 0.0;
    yMaxOut = 0.0;
    
    const AString text = annotationText.getText();
    if (text.isEmpty()) {
        return;
    }
#ifdef HAVE_FREETYPE
    FTFont* font = getFont(annotationText,
                           false);
    if (! font) {
        return;
    }
    
    switch (annotationText.getOrientation()) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
        {
            const FTBBox bbox   = font->BBox(text.toAscii().constData());
            const FTPoint lower = bbox.Lower();
            const FTPoint upper = bbox.Upper();
            
            xMinOut = lower.X();
            xMaxOut = upper.X();
            yMinOut = lower.Y();
            yMaxOut = upper.Y();
        }
            break;
        case AnnotationTextOrientationEnum::STACKED:
        {
            double textHeight = 0.0;
            std::vector<CharInfo> charInfo;
            getVerticalTextCharInfo(annotationText,
                                    xMinOut,
                                    xMaxOut,
                                    textHeight,
                                    charInfo);
            yMaxOut = textHeight;
        }
            break;
    }
#endif // HAVE_FREETYPE
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
FtglFontTextRenderer::getVerticalTextCharInfo(const AnnotationText& annotationText,
                                              double& xMinOut,
                                              double& xMaxOut,
                                              double& heightOut,
                                              std::vector<CharInfo>& charInfoOut)
{
    charInfoOut.clear();
    xMinOut = 0.0;
    xMaxOut = 0.0;
    heightOut = 0.0;
    
    const AString text = annotationText.getText();
    const int32_t numChars = static_cast<int32_t>(text.size());
    if (numChars <= 0) {
        return;
    }
#ifdef HAVE_FREETYPE
    
    FTFont* font = getFont(annotationText,
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
        const FTBBox bbox = font->BBox(oneChar.toAscii().constData());
        const FTPoint lower = bbox.Lower();
        const FTPoint upper = bbox.Upper();
        
        const double width  = bbox.Upper().X() - bbox.Lower().X();
        
        xMinOut = std::min(xMinOut, lower.X());
        xMaxOut = std::max(xMaxOut, upper.X());
        
        /*
         * Center the character horizontally.
         */
        const double xChar = -lower.X() - (width / 2.0);
        
        /*
         * Want the top of character at the Y-coordinate.
         */
        const double yChar = y - upper.Y();
        
        charInfoOut.push_back(CharInfo(oneChar,
                                       xChar, yChar));
        
        const double height = upper.Y() - lower.Y();
        if (i == lastCharIndex) {
            y -= height;
        }
        else {
            const double heightWithSpacing = height + s_textMarginSize;
            y -= heightWithSpacing;
        }
    }
    
    heightOut = std::fabs(y);
    
#endif // HAVE_FREETYPE
}

/**
 * Draw annnotation text at the given model coordinates using
 * the the annotations attributes for the style of text.
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
FtglFontTextRenderer::drawTextAtModelCoords(const double modelX,
                                          const double modelY,
                                          const double modelZ,
                                          const AnnotationText& annotationText)
{
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
                                 annotationText);
    }
    else {
        CaretLogSevere("gluProject() failed for drawing text at model coordinates.");
    }
}

/**
 * Save the state of OpenGL.
 * Copied from Qt's qgl.cpp, qt_save_gl_state().
 */
void
FtglFontTextRenderer::saveStateOfOpenGL()
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
FtglFontTextRenderer::restoreStateOfOpenGL()
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
 * Constructs invalid font data.
 */
FtglFontTextRenderer::FontData::FontData()
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
 */
FtglFontTextRenderer::FontData::FontData(const AnnotationText&  annotationText)
{
    m_valid    = false;
    m_font     = NULL;
    
#ifdef HAVE_FREETYPE
    const AnnotationFontNameEnum::Enum fontName = annotationText.getFont();
    
    AString fontFileName = AnnotationFontNameEnum::getResourceFontFileName(fontName);
    if (annotationText.isBoldEnabled()
        && annotationText.isItalicEnabled()) {
        fontFileName = AnnotationFontNameEnum::getResourceBoldItalicFontFileName(fontName);
    }
    else if (annotationText.isBoldEnabled()) {
        fontFileName = AnnotationFontNameEnum::getResourceBoldFontFileName(fontName);
        
    }
    else if (annotationText.isItalicEnabled()) {
        fontFileName = AnnotationFontNameEnum::getResourceItalicFontFileName(fontName);
    }
    
    CaretAssert( ! fontFileName.isEmpty());
    
    /*
     * Open the font file in the Workbench resources
     */
    QFile file(fontFileName);
    if (file.open(QFile::ReadOnly)) {
        /*
         * Read all data which MUST remain valid until the FTGL
         * font is no longer used.
         */
        m_fontData = file.readAll();
        const size_t numBytes = m_fontData.size();
        if (numBytes > 0) {
            /*
             * Create the FTGL font.
             */
            m_font = new FTTextureFont((const unsigned char*)m_fontData.data(),
                                       numBytes);
            
            CaretAssert(m_font);
            
            /*
             * Font created successfully ?
             */
            if ( ! m_font->Error()) {
                /*
                 * Font size successful ?
                 */
                const AnnotationFontSizeEnum::Enum fontSizeEnum = annotationText.getFontSize();
                const int32_t fontSizeInt = AnnotationFontSizeEnum::toSizeNumeric(fontSizeEnum);
                if (m_font->FaceSize(fontSizeInt)) {
                    m_valid = true;
                }
                else {
                    CaretLogSevere("Error creating font size "
                                   + AString::number(fontSizeInt)
                                   + " from font file "
                                   + file.fileName());
                }
            }
            else {
                CaretLogSevere("Error creating FTGL Font from "
                               + file.fileName()
                               + ".  Error code from FreeType is "
                               + AString::number(m_font->Error()));
            }
        }
        else {
            CaretLogSevere("Error reading data for FTGL Font from "
                           + file.fileName()
                           + " error: "
                           + file.errorString());
        }
    }
    else {
        CaretLogSevere("Unable to open FTGL Font File "
                       + file.fileName()
                       + " error: "
                       + file.errorString());
    }
    
    if ( ! m_valid) {
        m_fontData.clear();
        
        if (m_font != NULL) {
            delete m_font;
            m_font = NULL;
        }
    }
#endif // HAVE_FREETYPE
}

/**
 * Destructs font data.
 */
FtglFontTextRenderer::FontData::~FontData()
{
#ifdef HAVE_FREETYPE
    if (m_font != NULL) {
        delete m_font;
        m_font = NULL;
    }
#endif // HAVE_FREETYPE
}

/**
 * @return Name of the text renderer.
 */
AString
FtglFontTextRenderer::getName() const
{
    return "FTGL Text Renderer";
}

