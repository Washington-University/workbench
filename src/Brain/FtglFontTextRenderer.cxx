
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

#include <QFile>

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
#ifdef HAVE_FREETYPE
//    const FontType fontType = FONT_TYPE_PIXMAP;
    const FontType fontType = FONT_TYPE_TEXTURE;
    
    m_boldFont.initialize(":/FtglFonts/VeraBd.ttf",
                          fontType);
    m_normalFont.initialize(":/FtglFonts/VeraSe.ttf",
                            fontType);
#endif // HAVE_FREETYPE
}

/**
 * Destructor.
 */
FtglFontTextRenderer::~FtglFontTextRenderer()
{
#ifdef HAVE_FREETYPE
#endif // HAVE_FREETYPE
}

/**
 * @return The font system is valid.
 */
bool
FtglFontTextRenderer::isValid() const
{
    return m_normalFont.m_valid;
}

/*
 * Get the font with the given style and height.
 * Returned font not guaranteed to be desired size.
 *
 * @param textAttributes
 *   Attributes for text drawing.
 * @return
 *    The font.  May be NULL due to failure.
 */
FTFont*
FtglFontTextRenderer::getFont(const BrainOpenGLTextAttributes& textAttributes)
{
#ifdef HAVE_FREETYPE
    FTFont* font = NULL;
    
    if (textAttributes.isBoldEnabled()) {
        if (m_boldFont.m_valid) {
            font = m_boldFont.m_font;
        }
    }
    else {
        if (m_normalFont.m_valid) {
            font = m_normalFont.m_font;
        }
    }
    
    if (font != NULL) {
        if ( ! font->FaceSize(textAttributes.getFontHeight())) {
            QString msg("Failed to set requested font height="
                        + AString::number(textAttributes.getFontHeight())
                        + ".");
            if (font->FaceSize(14)) {
                msg += "  Defaulting to font height=14";
            }
            else {
                msg += "  Defaulting to font height=14 also failed.";
            }
            CaretLogWarning(msg);
        }
        
        return font;
    }
    
    CaretLogSevere("Trying to use FTGL Font rendering but font is not valid.");
    return NULL;
#else  // HAVE_FREETYPE
    return NULL;
#endif // HAVE_FREETYPE
}


/**
 * Draw text at the given window coordinates.
 *
 * @param viewport
 *   The current viewport.
 * @param windowX
 *   X-coordinate in the window of first text character
 *   using the 'alignment'
 * @param windowY
 *   Y-coordinate in the window at which bottom of text is placed.
 * @param text
 *   Text that is to be drawn.
 * @param textAttributes
 *   Attributes for text drawing.
 */
void
FtglFontTextRenderer::drawTextAtWindowCoords(const int viewport[4],
                                             const double windowX,
                                             const double windowY,
                                             const QString& text,
                                             const BrainOpenGLTextAttributes& textAttributes)
{
    if (text.isEmpty()) {
        return;
    }

#ifdef HAVE_FREETYPE
    FTFont* font = getFont(textAttributes);
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
    
    const FTBBox bbox = font->BBox(text.toAscii().constData());
    const FTPoint lower = bbox.Lower();
    const FTPoint upper = bbox.Upper();
    
    double textOffsetX = 0;
    switch (textAttributes.getHorizontalAlignment()) {
        case BrainOpenGLTextAttributes::X_CENTER:
            textOffsetX = -((upper.X() - lower.X()) / 2.0);
            break;
        case BrainOpenGLTextAttributes::X_LEFT:
            textOffsetX = -lower.X();
            break;
        case BrainOpenGLTextAttributes::X_RIGHT:
            textOffsetX = -upper.X();
            break;
    }
    
    double textOffsetY = 0;
    switch (textAttributes.getVerticalAlignment()) {
        case BrainOpenGLTextAttributes::Y_BOTTOM:
            textOffsetY = -lower.Y();
            break;
        case BrainOpenGLTextAttributes::Y_CENTER:
            textOffsetY = -((upper.Y() - lower.Y()) / 2.0);
            break;
        case BrainOpenGLTextAttributes::Y_TOP:
            textOffsetY = -upper.Y();
            break;
    }
    
    double textX = windowX + textOffsetX;
    double textY = windowY + textOffsetY;

//    if (drawCrosshairsAtFontStartingCoordinate) {
//        std::cout << "BBox: (" << lower.Xf() << " " << lower.Yf() << ") (" << upper.Xf() << ", " << upper.Yf() << ")" << std::endl;
//        
//        const float width  = upper.Xf() - lower.Xf();
//        const float height = upper.Yf() - lower.Yf();
//        
//        GLfloat savedRGBA[4];
//        glGetFloatv(GL_CURRENT_COLOR, savedRGBA);
//        glColor3f(1.0, 0.0, 1.0);
//        glLineWidth(1.0);
//        glPushMatrix();
//        //glTranslatef(windowX, windowY, 0.0);
//        glRectf(textX, textY, textX + width, textY + height);
//        glPopMatrix();
//        glColor3f(savedRGBA[0], savedRGBA[1], savedRGBA[2]);
//    }
    
    const double backgroundBounds[4] = {
        textX,
        textY,
        textX + (upper.X() - lower.X()),
        textY + (upper.Y() - lower.Y())
    };
    
//    GLint vp[4];
//    glGetIntegerv(GL_VIEWPORT, vp);
//    std::cout << "Drawing text (" << qPrintable(text) << " at x=" << textX << " y=" << textY << std::endl;
//    std::cout << "    VP: " << qPrintable(AString::fromNumbers(vp, 4, ",")) << "  win-x: " << windowX << "  win-y: " << windowY << std::endl;

    glPushMatrix();
    glLoadIdentity();
    applyBackgroundColoring(textAttributes,
                            backgroundBounds);
    applyForegroundColoring(textAttributes);
    
    glTranslated(textX,
                 textY,
                 0.0);
    font->Render(text.toAscii().constData());
    glPopMatrix();

//    glRasterPos2d(textX,
//                  textY);
//
//    font->Render(text.toAscii().constData());
    
    restoreStateOfOpenGL();
#else // HAVE_FREETYPE
    CaretLogSevere("Trying to use FTGL Font rendering but it cannot be used due to FreeType not found.");
#endif // HAVE_FREETYPE
}


/**
 * Draw text at the given window coordinates.
 *
 * @param viewport
 *   The current viewport.
 * @param windowX
 *   X-coordinate in the window of first text character
 *   using the 'alignment'
 * @param windowY
 *   Y-coordinate in the window at which bottom of text is placed.
 * @param text
 *   Text that is to be drawn.
 * @param textAttributes
 *   Attributes for text drawing.
 */
void
FtglFontTextRenderer::drawVerticalTextAtWindowCoords(const int viewport[4],
                                                     const double windowX,
                                                     const double windowY,
                                                     const QString& text,
                                                     const BrainOpenGLTextAttributes&  textAttributes)
{
    if (text.isEmpty()) {
        return;
    }
    
#ifdef HAVE_FREETYPE
    FTFont* font = getFont(textAttributes);
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
    
    
//    const double lineHeight = font->LineHeight();
//    
//    const int32_t numChars = text.length();

    double textBoundsWidth  = 0.0;
    double textBoundsHeight = 0.0;
    std::vector<CharInfo> textCharsToDraw;
    getVerticalTextCharInfo(text,
                            textAttributes,
                            textBoundsWidth,
                            textBoundsHeight,
                            textCharsToDraw);
    
//    double y =  windowY;   //+ ((lineHeight * numChars) / 2.0);
//    double textBoundsWidth = 0.0;
//    for (int32_t i = 0; i < numChars; i++) {
//        const QString oneChar = text[i];
//        double width  = 0;
//        double height = 0;
//        getTextBoundsInPixels(width,
//                              height,
//                              oneChar,
//                              textAttributes);
//        textBoundsWidth = std::max(textBoundsWidth,
//                                   width);
//        
//        
//        const FTBBox bbox = font->BBox(oneChar.toAscii().constData());
//        const FTPoint lower = bbox.Lower();
////        const FTPoint upper = bbox.Upper();
//        
//        const double xChar = windowX - lower.X() - (width / 2.0);
//        const double yChar = y + lower.Y();
//        
//        textCharsToDraw.push_back(CharInfo(text.at(i),
//                                           xChar, yChar));
//        
////        std::cout << "   " << qPrintable(oneChar)
////        << " x:" << xChar << " y:" << yChar
////        << " w:" << width << " h:" << height
////        << " x-min:" << lower.X() << " y-min:" << lower.Y()
////        << " x-max:" << upper.X() << " y-max:" << upper.Y() << std::endl;
//
//        y -= lineHeight;
//    }
//    
//    const double textBoundsHeight = std::fabs(windowY - y);
    
    double textOffsetX = 0;
    switch (textAttributes.getHorizontalAlignment()) {
        case BrainOpenGLTextAttributes::X_CENTER:
            textOffsetX = 0;
            break;
        case BrainOpenGLTextAttributes::X_LEFT:
            textOffsetX = (textBoundsWidth / 2.0);
            break;
        case BrainOpenGLTextAttributes::X_RIGHT:
            textOffsetX = -(textBoundsWidth / 2.0);
            break;
    }
    
    /*
     * By default, coordinate is a bottom of first character
     */
    const double oneCharHeight = (textBoundsHeight / static_cast<double>(textCharsToDraw.size()));
    double textOffsetY = 0.0;
    double textBackgroundTopOffsetY = 0.0;
    switch (textAttributes.getVerticalAlignment()) {
        case BrainOpenGLTextAttributes::Y_BOTTOM:
            textOffsetY = textBoundsHeight - oneCharHeight;
            textBackgroundTopOffsetY = textBoundsHeight;
            break;
        case BrainOpenGLTextAttributes::Y_CENTER:
            textOffsetY = (textBoundsHeight / 2.0) - (oneCharHeight / 2.0);
            textBackgroundTopOffsetY = (textBoundsHeight / 2.0);
            break;
        case BrainOpenGLTextAttributes::Y_TOP:
            textOffsetY = -oneCharHeight;
            textBackgroundTopOffsetY = 0.0;
            break;
    }
    
//    const double backgroundBounds[4] = {
//        windowX - (textBoundsWidth   / 2.0) + textOffsetX,
//        windowY - (textBoundsHeight  / 2.0) + textOffsetY,
//        windowX + (textBoundsWidth   / 2.0) + textOffsetX,
//        windowY + (textBoundsHeight  / 2.0) + textOffsetY
//    };
    const double backMinX = windowX - (textBoundsWidth   / 2.0) + textOffsetX;
    const double backMaxX = backMinX + textBoundsWidth;
    
    const double backMaxY = windowY + textBackgroundTopOffsetY;
    const double backMinY = backMaxY - textBoundsHeight;
//    const double backMinY = windowY  - textOffsetY;
//    const double backMaxY = backMinY + textBoundsHeight;
    const double backgroundBounds[4] = {
        backMinX,
        backMinY,
        backMaxX,
        backMaxY
    };
    
    applyBackgroundColoring(textAttributes,
                            backgroundBounds);
    applyForegroundColoring(textAttributes);
    
//    GLint vp[4];
//    glGetIntegerv(GL_VIEWPORT, vp);
    
    for (std::vector<CharInfo>::const_iterator textIter = textCharsToDraw.begin();
         textIter != textCharsToDraw.end();
         textIter++) {
        const double charX = windowX + textIter->m_x + textOffsetX;
        const double charY = windowY + textIter->m_y + textOffsetY;
//        std::cout << "Drawing vertical char (" << qPrintable(textIter->m_char) << " at x=" << charX << " y=" << charY << std::endl;
//        std::cout << "    VP: " << qPrintable(AString::fromNumbers(vp, 4, ",")) << "  win-x: " << windowX << "  win-y: " << windowY << std::endl;
//        std::cout <<      "Offset: " << textOffsetX << " " << textOffsetY << std::endl;

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

//void
//FtglFontTextRenderer::drawVerticalTextAtWindowCoords(const int viewport[4],
//                                                     const double windowX,
//                                                     const double windowY,
//                                                     const QString& text,
//                                                     const BrainOpenGLTextAttributes&  textAttributes)
//{
//    if (text.isEmpty()) {
//        return;
//    }
//    
//#ifdef HAVE_FREETYPE
//    FTFont* font = getFont(textAttributes);
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
//    
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
//    
//    const double lineHeight = font->LineHeight();
//    
//    const int32_t numChars = text.length();
//    
//    std::vector<CharInfo> textCharsToDraw;
//    
//    double y =  windowY;   //+ ((lineHeight * numChars) / 2.0);
//    double textBoundsWidth = 0.0;
//    for (int32_t i = 0; i < numChars; i++) {
//        const QString oneChar = text[i];
//        double width  = 0;
//        double height = 0;
//        getTextBoundsInPixels(width,
//                              height,
//                              oneChar,
//                              textAttributes);
//        textBoundsWidth = std::max(textBoundsWidth,
//                                   width);
//        
//        
//        const FTBBox bbox = font->BBox(oneChar.toAscii().constData());
//        const FTPoint lower = bbox.Lower();
//        //        const FTPoint upper = bbox.Upper();
//        
//        const double xChar = windowX - lower.X() - (width / 2.0);
//        const double yChar = y + lower.Y();
//        
//        textCharsToDraw.push_back(CharInfo(text.at(i),
//                                           xChar, yChar));
//        
//        //        std::cout << "   " << qPrintable(oneChar)
//        //        << " x:" << xChar << " y:" << yChar
//        //        << " w:" << width << " h:" << height
//        //        << " x-min:" << lower.X() << " y-min:" << lower.Y()
//        //        << " x-max:" << upper.X() << " y-max:" << upper.Y() << std::endl;
//        
//        y -= lineHeight;
//    }
//    
//    const double textBoundsHeight = std::fabs(windowY - y);
//    
//    double textOffsetX = 0;
//    switch (textAttributes.getHorizontalAlignment()) {
//        case BrainOpenGLTextAttributes::X_CENTER:
//            textOffsetX = 0;
//            break;
//        case BrainOpenGLTextAttributes::X_LEFT:
//            textOffsetX = (textBoundsWidth / 2.0);
//            break;
//        case BrainOpenGLTextAttributes::X_RIGHT:
//            textOffsetX = -(textBoundsWidth / 2.0);
//            break;
//    }
//    
//    /*
//     * By default, coordinate is a bottom of first character
//     */
//    const double oneCharHeight = (textBoundsHeight / static_cast<double>(textCharsToDraw.size()));
//    double textOffsetY = 0;
//    switch (textAttributes.getVerticalAlignment()) {
//        case BrainOpenGLTextAttributes::Y_BOTTOM:
//            textOffsetY = textBoundsHeight - oneCharHeight;
//            break;
//        case BrainOpenGLTextAttributes::Y_CENTER:
//            textOffsetY = (textBoundsHeight / 2.0) - (oneCharHeight / 2.0);
//            break;
//        case BrainOpenGLTextAttributes::Y_TOP:
//            textOffsetY = -oneCharHeight;
//            break;
//    }
//    
//    const double backgroundBounds[4] = {
//        windowX - (textBoundsWidth   / 2.0) + textOffsetX,
//        windowY - (textBoundsHeight  / 2.0) + textOffsetX,
//        windowX + (textBoundsWidth   / 2.0) + textOffsetY,
//        windowY + (textBoundsHeight  / 2.0) + textOffsetY
//    };
//    
//    applyColoring(textAttributes,
//                  backgroundBounds);
//    
//    GLint vp[4];
//    glGetIntegerv(GL_VIEWPORT, vp);
//    for (std::vector<CharInfo>::const_iterator textIter = textCharsToDraw.begin();
//         textIter != textCharsToDraw.end();
//         textIter++) {
//        const double charX = textIter->m_x + textOffsetX;
//        const double charY = textIter->m_y + textOffsetY;
//        
//        std::cout << "Drawing vertical char (" << qPrintable(textIter->m_char) << " at x=" << charX << " y=" << charY << std::endl;
//        std::cout << "    VP: " << qPrintable(AString::fromNumbers(vp, 4, ",")) << "  win-x: " << windowX << "  win-y: " << windowY << std::endl;
//        std::cout <<      "Offset: " << textOffsetX << " " << textOffsetY << std::endl;
//        glPushMatrix();
//        glTranslated(charX,
//                     charY,
//                     0.0);
//        font->Render(textIter->m_char.toAscii().constData());
//        glPopMatrix();
//        
//        //        glRasterPos2d(charX,
//        //                      charY);
//        //        font->Render(textIter->m_char.toAscii().constData());
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
 * @param textAttributes
 *   Attributes for text drawing.
 * @param textBoundsBox
 *   Bounding box for text (min-x, min-y, max-x, max-y)
 */
void
FtglFontTextRenderer::applyBackgroundColoring(const BrainOpenGLTextAttributes& textAttributes,
                                              const double textBoundsBox[4])
{
    float backgroundColor[4];
    textAttributes.getBackgroundColor(backgroundColor);
    
    if (backgroundColor[3] > 0.0) {
        glColor4fv(backgroundColor);
        const double margin = 2;
        glRectd(textBoundsBox[0] - margin,
                textBoundsBox[1] - margin,
                textBoundsBox[2] + margin,
                textBoundsBox[3] + margin);
    }
}

/**
 * Apply the foreground color.
 *
 * @param textAttributes
 *   Attributes for text drawing.
 * @param textBoundsBox
 *   Bounding box for text (min-x, min-y, max-x, max-y)
 */
void
FtglFontTextRenderer::applyForegroundColoring(const BrainOpenGLTextAttributes& textAttributes)
{
    const float* foregroundColor = textAttributes.getForegroundColor();
    glColor4fv(foregroundColor);
}

/**
 * Get the bounds of the text (in pixels) using the given text
 * attributes.
 *
 * @param widthOut
 *   Output containing width of text characters.
 * @param heightOut
 *   Output containing height of text characters.
 * @param text
 *   Text that is to be drawn.
 * @param textAttributes
 *   Attributes for text drawing.
 */
void
FtglFontTextRenderer::getTextBoundsInPixels(double& widthOut,
                                            double& heightOut,
                                           const QString& text,
                                           const BrainOpenGLTextAttributes& textAttributes)
{
    widthOut  = 0;
    heightOut = 0;
#ifdef HAVE_FREETYPE
    FTFont* font = getFont(textAttributes);
    if (! font) {
        return;
    }

    switch (textAttributes.getOrientation()) {
        case BrainOpenGLTextAttributes::LEFT_TO_RIGHT:
        {
            const FTBBox bbox = font->BBox(text.toAscii().constData());
            const FTPoint lower = bbox.Lower();
            const FTPoint upper = bbox.Upper();
            
            widthOut = upper.X() - lower.X();
            heightOut = upper.Y() - lower.Y();
        }
            break;
        case BrainOpenGLTextAttributes::TOP_TO_BOTTOM:
        {
            std::vector<CharInfo> charInfo;
            getVerticalTextCharInfo(text,
                                    textAttributes,
                                    widthOut,
                                    heightOut,
                                    charInfo);
        }
            break;
    }
#endif // HAVE_FREETYPE
}

/**
 * Get the character info for drawing vertical text which includes
 * position for each of the characters.
 *
 * @param text
 *    Text that is to be drawn.
 * @param textAttributes
 *    Attributes for text drawing.
 * @param widthOut
 *    Output containing width of text characters.
 * @param heightOut
 *    Output containing height of text characters.
 * @param charInfoOut
 *    Contains each character and its X, Y position
 *    for rendering vertically.
 */
void
FtglFontTextRenderer::getVerticalTextCharInfo(const QString& text,
                                              const BrainOpenGLTextAttributes& textAttributes,
                                              double& textWidthOut,
                                              double& textHeightOut,
                                              std::vector<CharInfo>& charInfoOut)
{
    charInfoOut.clear();
    textWidthOut  = 0.0;
    textHeightOut = 0.0;
    
#ifdef HAVE_FREETYPE
    const int32_t numChars = static_cast<int32_t>(text.size());
    
    FTFont* font = getFont(textAttributes);
    if ( ! font) {
        return;
    }
    const double lineHeight = font->LineHeight();
    
    double y =  0.0;
    for (int32_t i = 0; i < numChars; i++) {
        const QString oneChar = text[i];
        const FTBBox bbox = font->BBox(oneChar.toAscii().constData());
        const double width  = bbox.Upper().X() - bbox.Lower().X();
        
        textWidthOut = std::max(textWidthOut,
                                width);
        
        const FTPoint lower = bbox.Lower();
        const double xChar = -lower.X() - (width / 2.0);
        const double yChar = y + lower.Y();
        
        charInfoOut.push_back(CharInfo(oneChar,
                                           xChar, yChar));
        
        //        std::cout << "   " << qPrintable(oneChar)
        //        << " x:" << xChar << " y:" << yChar
        //        << " w:" << width << " h:" << height
        //        << " x-min:" << lower.X() << " y-min:" << lower.Y()
        //        << " x-max:" << upper.X() << " y-max:" << upper.Y() << std::endl;
        
        y -= lineHeight;
    }
    
    textHeightOut = std::fabs(y);
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
 * @param text
 *   Text that is to be drawn.
 * @param textAttributes
 *   Attributes for text drawing.
 */
void
FtglFontTextRenderer::drawTextAtModelCoords(const double modelX,
                                                     const double modelY,
                                                     const double modelZ,
                                                     const QString& text,
                                            const BrainOpenGLTextAttributes& textAttributes)
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
    
    GLdouble windowX, windowY, windowZ;
    if (gluProject(modelX, modelY, modelZ,
                   modelMatrix, projectionMatrix, viewport,
                   &windowX, &windowY, &windowZ) == GL_TRUE) {
        
        drawTextAtWindowCoords(viewport,
                               windowX,
                               windowY,
                               text,
                               textAttributes);
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
    m_valid = false;
    m_font  = NULL;
}

/**
 * Destructs font data.
 */
FtglFontTextRenderer::FontData::~FontData()
{
    if (m_font != NULL) {
        delete m_font;
        m_font = NULL;
    }
}

/**
 * Initialize font data.    
 *
 * @param fontFileName
 *    Name of font file.
 * @param fontType
 *    Type of font.
 */
void
FtglFontTextRenderer::FontData::initialize(const AString& fontFileName,
                                           const FontType fontType)
{
#ifdef HAVE_FREETYPE
    m_positionType = POSITION_TYPE_RASTER;
    
    QFile file(fontFileName);
    if (file.open(QFile::ReadOnly)) {
        m_fontData = file.readAll();
        const size_t numBytes = m_fontData.size();
        if (numBytes > 0) {
            const unsigned char* data = (const unsigned char*)m_fontData.data();
            
            switch (fontType) {
                case FONT_TYPE_PIXMAP:
                    m_font = new FTPixmapFont(data,
                                              numBytes);
                    m_positionType = POSITION_TYPE_RASTER;
                    break;
                case FONT_TYPE_TEXTURE:
                    m_font = new FTTextureFont(data,
                                               numBytes);
                    m_positionType = POSITION_TYPE_MATRIX;
                    break;
            }
            if (m_font->Error()) {
                delete m_font;
                m_font = NULL;
                CaretLogSevere("Unable to load font file " + file.fileName());
                return;
            }
        }
        else {
            CaretLogSevere("Error reading data from "
                           + file.fileName()
                           + " error: "
                           + file.errorString());
            return;
        }
    }
    else {
        CaretLogSevere("Unable to open "
                       + file.fileName()
                       + " error: "
                       + file.errorString());
        return;
    }
    
    m_valid = true;
#else // HAVE_FREETYPE
    CaretLogWarning("Trying to initialize FTGL fonts but program was compiled without FreeType.\n"
                    "Text labels may be missing in graphics windows.");
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

