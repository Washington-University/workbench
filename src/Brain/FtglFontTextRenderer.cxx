
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
 * \brief Text rendering using QGLWidget.
 *
 * Draws text using methods in QGLWidget.
 * 
 * @param glWidget
 *   QGLWidget that does the text rendering.
 */
/**
 * Constructor.
 */
FtglFontTextRenderer::FtglFontTextRenderer()
: BrainOpenGLTextRenderInterface()
{
    m_arialPixmapFontValid = false;
    

#ifdef HAVE_FREETYPE
    //const AString fontFileName("/Library/Fonts/Arial.ttf");
    const AString fontFileName("/usr/X11R6/share/fonts/TTF/VeraSe.ttf");
    m_arialPixmapFont = new FTPixmapFont(fontFileName.toAscii().constData());
    if (m_arialPixmapFont->Error()) {
        CaretLogSevere("Unable to load font file " + fontFileName);
        return;
    }
    
    m_arialPixmapFont->FaceSize(14);
    
    m_arialPixmapFontValid = true;
#endif // HAVE_FREETYPE
}

/**
 * Destructor.
 */
FtglFontTextRenderer::~FtglFontTextRenderer()
{
#ifdef HAVE_FREETYPE
    if (m_arialPixmapFont == NULL) {
        delete m_arialPixmapFont;
        m_arialPixmapFont = NULL;
    }
#endif // HAVE_FREETYPE
}

void
FtglFontTextRenderer::drawString(char* str)
{
//    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
//    glLoadIdentity();
//    glOrtho(-100.0, 100.0, -100.0, 100.0, -1.0, 1.0);
//    
//    glMatrixMode(GL_MODELVIEW);
//    
//    glfSetCurrentFont(m_arialFont);
//    float xmin,ymin,xmax,ymax;
//    glfGetStringBounds(str, &xmin,&ymin,&xmax,&ymax);
//    
//    glColor3f(1.0, 0.0, 0.0);
//    
//    glColor3f(1.0, 1.0, 1.0);
//    
//    glPushMatrix();
//    glLoadIdentity();
//    glTranslatef(-(xmin+xmax) / 2.0, 0.0, 0.0);
//    glScalef(10.0, 10.0, 1.0);
//    glfDrawSolidString((char*)str);
//    glPopMatrix();
//    
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
//    glMatrixMode(GL_MODELVIEW);
    
}

/**
 * @return The font system is valid.
 */
bool
FtglFontTextRenderer::isValid() const
{
    return m_arialPixmapFontValid;
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
 * @param alignment
 *   Alignment of text
 * @param textStyle
 *   Style of the text.
 * @param fontHeight
 *   Height of the text.
 * @param fontName
 *   Name of the font.
 */
void 
FtglFontTextRenderer::drawTextAtWindowCoords(const int viewport[4],
                                                      const int windowX,
                                                      const int windowY,
                                                      const QString& text,
                                                      const TextAlignmentX alignmentX,
                                                      const TextAlignmentY alignmentY,
                                                      const TextStyle /*textStyle*/,
                                                      const int fontHeightIn,
                                                      const AString& /*fontName*/)
{
#ifdef HAVE_FREETYPE    
    if (! m_arialPixmapFontValid) {
        CaretLogSevere("Trying to use FTGL Font rendering but font is not valid.");
        return;
    }
    
    if (text.isEmpty()) {
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
//    glOrtho(0,
//               (viewport[0] + viewport[2]),
//               0,
//               (viewport[1] + viewport[3]),
//               -1,
//               1);
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
    
    GLfloat savedRGBA[4];
    glGetFloatv(GL_CURRENT_COLOR, savedRGBA);
    glColor3f(0.0, 0.0, 0.0);
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
    
    
    
    const FTBBox bbox = m_arialPixmapFont->BBox(text.toAscii().constData());
    const FTPoint lower = bbox.Lower();
    const FTPoint upper = bbox.Upper();
    std::cout << qPrintable(text) << std::endl;
    std::cout << "   LOWER: (" << lower.X() << ", " << lower.Y() << ", " << lower.Z() << ")" << std::endl;
    std::cout << "   UPPER: (" << upper.X() << ", " << upper.Y() << ", " << upper.Z() << ")" << std::endl;
    
    float textOffsetX = 0;
    switch (alignmentX) {
        case BrainOpenGLTextRenderInterface::X_CENTER:
            std::cout << "   X-CENTER ";
            textOffsetX = -((upper.X() - lower.X()) / 2.0);
            break;
        case BrainOpenGLTextRenderInterface::X_LEFT:
            std::cout << "   X-LEFT ";
            textOffsetX = -lower.X();
            break;
        case BrainOpenGLTextRenderInterface::X_RIGHT:
            std::cout << "   X-RIGHT ";
            textOffsetX = -upper.X();
            break;
    }
    std::cout << "Offset=" << textOffsetX << "   ";
    
    float textOffsetY = 0;
    switch (alignmentY) {
        case BrainOpenGLTextRenderInterface::Y_BOTTOM:
            std::cout << "Y-BOTTOM ";
            textOffsetY = -lower.Y();
            break;
        case BrainOpenGLTextRenderInterface::Y_CENTER:
            textOffsetY = -((upper.Y() - lower.Y()) / 2.0);
            std::cout << "Y-CENTER ";
            break;
        case BrainOpenGLTextRenderInterface::Y_TOP:
            textOffsetY = -upper.Y();
            std::cout << "Y-TOP ";
            break;
    }
    std::cout << "Offset=" << textOffsetY << std::endl;
    
    float textX = windowX + textOffsetX;
    float textY = windowY + textOffsetY;
    glRasterPos2f(textX,
                  textY);

    m_arialPixmapFont->Render(text.toAscii().constData());
    
    restoreStateOfOpenGL();
#else // HAVE_FREETYPE
    CaretLogSevere("Trying to use FTGL Font rendering but it cannot be used due to FreeType not found.");
#endif // HAVE_FREETYPE
}

/**
 * Get the bounds of the text (in pixels) using the given text
 * attributes.
 *
 * @param text
 *   Text that is to be drawn.
 * @param textStyle
 *   Style of the text.
 * @param fontHeight
 *   Height of the text.
 * @param fontName
 *   Name of the font.
 * @param widthOut
 *   Output containing width of text characters.
 * @param heightOut
 *   Output containing height of text characters.
 */
void
FtglFontTextRenderer::getTextBoundsInPixels(int32_t& widthOut,
                                           int32_t& heightOut,
                                           const QString& text,
                                           const TextStyle /*textStyle*/,
                                           const int fontHeight,
                                           const AString& /*fontName*/)
{
    widthOut  = 0;
    heightOut = 0;
#ifdef HAVE_FREETYPE
    const FTBBox bbox = m_arialPixmapFont->BBox(text.toAscii().constData());
    const FTPoint lower = bbox.Lower();
    const FTPoint upper = bbox.Upper();
    
    widthOut = upper.X() - lower.X();
    heightOut = upper.Y() - lower.Y();
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
 * @param textStyle
 *   Style of the text.
 * @param fontHeight
 *   Height of the text.
 * @param fontName
 *   Name of the font.
 */
void 
FtglFontTextRenderer::drawTextAtModelCoords(const double modelX,
                                                     const double modelY,
                                                     const double modelZ,
                                                     const QString& text,
                                                     const TextStyle textStyle,
                                                     const int fontHeight,
                                                     const AString& fontName)
{
    if (! m_arialPixmapFontValid) {
        return;
    }
    
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
        
        //saveStateOfOpenGL();
        
        drawTextAtWindowCoords(viewport,
                               windowX,
                               windowY,
                               text,
                               X_CENTER,
                               Y_CENTER,
                               textStyle,
                               fontHeight,
                               fontName);
        //restoreStateOfOpenGL();
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
