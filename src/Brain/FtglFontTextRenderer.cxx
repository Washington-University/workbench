
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
#ifdef HAVE_FREETYPE
    m_boldFont.initialize(":/FtglFonts/VeraBd.ttf");
    m_normalFont.initialize(":/FtglFonts/VeraSe.ttf");
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
 * @param textStyle
 *    Style of the text.
 * @param fontHeight
 *    Height of the font.
 * @return
 *    The font.  May be NULL due to 
 */
FTPixmapFont*
FtglFontTextRenderer::getFont(const TextStyle textStyle,
                              const int fontHeight)
{
#ifdef HAVE_FREETYPE
    FTPixmapFont* pixmapFont = NULL;
    switch (textStyle) {
        case BrainOpenGLTextRenderInterface::BOLD:
            if (m_boldFont.m_valid) {
                pixmapFont = m_boldFont.m_pixmapFont;
            }
            break;
        case BrainOpenGLTextRenderInterface::NORMAL:
            if (m_normalFont.m_valid) {
                pixmapFont = m_normalFont.m_pixmapFont;
            }
            break;
    }
    
    if (pixmapFont != NULL) {
        if ( ! pixmapFont->FaceSize(fontHeight)) {
            QString msg("Failed to set requested font height="
                        + AString::number(fontHeight)
                        + ".");
            if (pixmapFont->FaceSize(14)) {
                msg += "  Defaulting to font height=14";
            }
            else {
                msg += "  Defaulting to font height=14 also failed.";
            }
            CaretLogWarning(msg);
        }
        
        return pixmapFont;
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
 * @param alignment
 *   Alignment of text
 * @param textStyle
 *   Style of the text.
 * @param fontHeight
 *   Height of the text.
 */
void 
FtglFontTextRenderer::drawTextAtWindowCoords(const int viewport[4],
                                                      const int windowX,
                                                      const int windowY,
                                                      const QString& text,
                                                      const TextAlignmentX alignmentX,
                                                      const TextAlignmentY alignmentY,
                                                      const TextStyle textStyle,
                                                      const int fontHeight)
{
    if (text.isEmpty()) {
        return;
    }
    
#ifdef HAVE_FREETYPE
    FTPixmapFont* pixmapFont = getFont(textStyle, fontHeight);
    if (! pixmapFont) {
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
    }
    
    const FTBBox bbox = pixmapFont->BBox(text.toAscii().constData());
    const FTPoint lower = bbox.Lower();
    const FTPoint upper = bbox.Upper();
    
    float textOffsetX = 0;
    switch (alignmentX) {
        case BrainOpenGLTextRenderInterface::X_CENTER:
            textOffsetX = -((upper.X() - lower.X()) / 2.0);
            break;
        case BrainOpenGLTextRenderInterface::X_LEFT:
            textOffsetX = -lower.X();
            break;
        case BrainOpenGLTextRenderInterface::X_RIGHT:
            textOffsetX = -upper.X();
            break;
    }
    
    float textOffsetY = 0;
    switch (alignmentY) {
        case BrainOpenGLTextRenderInterface::Y_BOTTOM:
            textOffsetY = -lower.Y();
            break;
        case BrainOpenGLTextRenderInterface::Y_CENTER:
            textOffsetY = -((upper.Y() - lower.Y()) / 2.0);
            break;
        case BrainOpenGLTextRenderInterface::Y_TOP:
            textOffsetY = -upper.Y();
            break;
    }
    
    float textX = windowX + textOffsetX;
    float textY = windowY + textOffsetY;
    glRasterPos2f(textX,
                  textY);

    pixmapFont->Render(text.toAscii().constData());
    
    restoreStateOfOpenGL();
#else // HAVE_FREETYPE
    CaretLogSevere("Trying to use FTGL Font rendering but it cannot be used due to FreeType not found.");
#endif // HAVE_FREETYPE
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
 * @param textStyle
 *   Style of the text.
 * @param fontHeight
 *   Height of the text.
 */
void
FtglFontTextRenderer::getTextBoundsInPixels(int32_t& widthOut,
                                            int32_t& heightOut,
                                           const QString& text,
                                           const TextStyle textStyle,
                                           const int fontHeight)
{
    widthOut  = 0;
    heightOut = 0;
#ifdef HAVE_FREETYPE
    FTPixmapFont* pixmapFont = getFont(textStyle, fontHeight);
    if (! pixmapFont) {
        return;
    }

    const FTBBox bbox = pixmapFont->BBox(text.toAscii().constData());
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
 */
void 
FtglFontTextRenderer::drawTextAtModelCoords(const double modelX,
                                                     const double modelY,
                                                     const double modelZ,
                                                     const QString& text,
                                                     const TextStyle textStyle,
                                                     const int fontHeight)
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
                               X_CENTER,
                               Y_CENTER,
                               textStyle,
                               fontHeight);
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
    m_pixmapFont = NULL;
}

/**
 * Destructs font data.
 */
FtglFontTextRenderer::FontData::~FontData()
{
    if (m_pixmapFont != NULL) {
        delete m_pixmapFont;
        m_pixmapFont = NULL;
    }
}

/**
 * Initialize font data.    
 *
 * @param fontFileName
 *    Name of font file.
 */
void
FtglFontTextRenderer::FontData::initialize(const AString& fontFileName)
{
#ifdef HAVE_FREETYPE
    QFile file(fontFileName);
    if (file.open(QFile::ReadOnly)) {
        m_fontData = file.readAll();
        const size_t numBytes = m_fontData.size();
        if (numBytes > 0) {
            const unsigned char* data = (const unsigned char*)m_fontData.data();
            m_pixmapFont = new FTPixmapFont(data,
                                                 numBytes);
            if (m_pixmapFont->Error()) {
                delete m_pixmapFont;
                m_pixmapFont = NULL;
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

