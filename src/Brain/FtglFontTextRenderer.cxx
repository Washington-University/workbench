
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
    defaultAnnotationText.setFontHeight(14);
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
 * Draw annnotation text at the given VIEWPORT coordinates.
 *
 * The origin (0, 0) is at the bottom left corner
 * of the viewport and (viewport-width, viewport-height)
 * is at the top right corner of the viewport.
 *
 * @param viewport
 *   The current viewport.
 * @param windowX
 *   X-coordinate of the text.
 * @param windowY
 *   Y-coordinate of the text.
 * @param annotationText
 *   Annotation Text that is to be drawn.
 */
void
FtglFontTextRenderer::drawTextAtViewportCoords(const int viewport[4],
                                      const double windowX,
                                      const double windowY,
                                      const AnnotationText& annotationText)
{
    //std::cout << "Drawing \"" << qPrintable(text) << "\" at " << windowX << ", " << windowY << std::endl;
    if (annotationText.getText().isEmpty()) {
        return;
    }
    
    switch (annotationText.getOrientation()) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
            drawHorizontalTextAtWindowCoords(viewport,
                                             windowX,
                                             windowY,
                                             annotationText);
            break;
        case AnnotationTextOrientationEnum::STACKED:
            drawVerticalTextAtWindowCoords(viewport,
                                           windowX,
                                           windowY,
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
FtglFontTextRenderer::drawHorizontalTextAtWindowCoords(const int viewport[4],
                                                       const double windowX,
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
    
    const AString text = annotationText.getText();
    
    const FTBBox  bbox  = font->BBox(text.toAscii().constData());
    const FTPoint lower = bbox.Lower();
    const FTPoint upper = bbox.Upper();
    
    double textOffsetX = 0;
    switch (annotationText.getHorizontalAlignment()) {
        case AnnotationAlignHorizontalEnum::CENTER:
            textOffsetX = -((upper.X() - lower.X()) / 2.0);
            break;
        case AnnotationAlignHorizontalEnum::LEFT:
            textOffsetX = -lower.X();
            break;
        case AnnotationAlignHorizontalEnum::RIGHT:
            textOffsetX = -upper.X();
            break;
    }
    
    
    double textOffsetY = 0;
    switch (annotationText.getVerticalAlignment()) {
        case AnnotationAlignVerticalEnum::BOTTOM:
            textOffsetY = -lower.Y();
            break;
        case AnnotationAlignVerticalEnum::CENTER:
            textOffsetY = -((upper.Y() - lower.Y()) / 2.0);
            break;
        case AnnotationAlignVerticalEnum::TOP:
            textOffsetY = -upper.Y();
            break;
    }
    
    double textX = windowX + textOffsetX;
    double textY = windowY + textOffsetY;
    
    if (drawCrosshairsAtFontStartingCoordinate) {
        std::cout << "BBox: (" << lower.Xf() << " " << lower.Yf() << ") (" << upper.Xf() << ", " << upper.Yf() << ")" << std::endl;
        
        const float width  = upper.Xf() - lower.Xf();
        const float height = upper.Yf() - lower.Yf();
        
        GLfloat savedRGBA[4];
        glGetFloatv(GL_CURRENT_COLOR, savedRGBA);
        glColor3f(1.0, 0.0, 1.0);
        glLineWidth(1.0);
        glPushMatrix();
        glRectf(textX, textY, textX + width, textY + height);
        glPopMatrix();
        glColor3f(savedRGBA[0], savedRGBA[1], savedRGBA[2]);
    }
    
    //    const double backgroundBounds[4] = {
    //        textX,
    //        textY,
    //        textX + (upper.X() - lower.X()),
    //        textY + (upper.Y() - lower.Y())
    //    };
    const double backgroundBounds[4] = {
        textX,
        textY,
        textX + (upper.X() - lower.X()),
        textY + (upper.Y() - lower.Y())
    };
    
    glPushMatrix();
    glLoadIdentity();
    applyBackgroundColoring(annotationText,
                            backgroundBounds);
    applyForegroundColoring(annotationText);
    
    glTranslated(textX,
                 textY,
                 0.0);
    font->Render(text.toAscii().constData());
    glPopMatrix();
    
    restoreStateOfOpenGL();
#else // HAVE_FREETYPE
    CaretLogSevere("Trying to use FTGL Font rendering but it cannot be used due to FreeType not found.");
#endif // HAVE_FREETYPE
}

/**
 * Draw vertical text at the given window coordinates.
 *
 * @param viewport
 *   The current viewport.
 * @param windowX
 *   X-coordinate in the window of first text character
 *   using the 'alignment'
 * @param windowY
 *   Y-coordinate in the window at which bottom of text is placed.
 * @param annotationText
 *   Text that is to be drawn.
 */
void
FtglFontTextRenderer::drawVerticalTextAtWindowCoords(const int viewport[4],
                                                     const double windowX,
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
                            textMinX,
                            textMaxX,
                            textHeight,
                            textCharsToDraw);
    const double textBoundsWidth  = textMaxX - textMinX;
    const double textBoundsHeight = textHeight;
    
    double textOffsetX = 0;
    switch (annotationText.getHorizontalAlignment()) {
        case AnnotationAlignHorizontalEnum::LEFT:
            textOffsetX = (textBoundsWidth / 2.0);
            break;
        case AnnotationAlignHorizontalEnum::CENTER:
            textOffsetX = 0;
            break;
        case AnnotationAlignHorizontalEnum::RIGHT:
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
        case AnnotationAlignVerticalEnum::BOTTOM:
            textOffsetY = textBoundsHeight;
            textBackgroundTopOffsetY = textBoundsHeight;
            break;
        case AnnotationAlignVerticalEnum::CENTER:
            textOffsetY = (textBoundsHeight / 2.0);
            textBackgroundTopOffsetY = (textBoundsHeight / 2.0);
            break;
        case AnnotationAlignVerticalEnum::TOP:
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
        
        glPushMatrix();
        glTranslated(charX,
                     charY,
                     0.0);
        font->Render(textIter->m_char.toAscii().constData());
        glPopMatrix();
    }
    
    restoreStateOfOpenGL();
#else // HAVE_FREETYPE
    CaretLogSevere("Trying to use FTGL Font rendering but it cannot be used due to FreeType not found.");
#endif // HAVE_FREETYPE
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
FtglFontTextRenderer::applyBackgroundColoring(const AnnotationText& annotationText,
                                              const double textBoundsBox[4])
{
    float backgroundColor[4];
    annotationText.getBackgroundColor(backgroundColor);
    
    //    backgroundColor[0] = 1.0;
    //    backgroundColor[1] = 0.8;
    //    backgroundColor[2] = 0.6;
    //    backgroundColor[3] = 1.0;
    
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
 * Draw text at the given model coordinates.
 *
 * @param modelX
 *   X-coordinate in model space of first text character
 * @param modelY
 *   Y-coordinate in model space.
 * @param modelZ
 *   Z-coordinate in model space.
 * @param annotationText
 *   Text that is to be drawn.
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
        
        drawTextAtViewportCoords(viewport,
                                 windowX,
                                 windowY,
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
                if (m_font->FaceSize(annotationText.getFontHeight())) {
                    m_valid = true;
                }
                else {
                    CaretLogSevere("Error creating font height "
                                   + AString::number(annotationText.getFontHeight())
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
    return "FTGL Font";
}

