
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

#define __GLF_FONT_TEXT_RENDERER_DECLARE__
#include "GlfFontTextRenderer.h"
#undef __GLF_FONT_TEXT_RENDERER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOpenGLInclude.h"

using namespace caret;


    
/**
 * \class caret::GlfFontTextRenderer 
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
GlfFontTextRenderer::GlfFontTextRenderer()
: BrainOpenGLTextRenderInterface()
{
    m_fontFileValid = false;
    glfInit();
    
    const AString arialFontFileName(":/GlfFonts/FontAreal1.glf");
    m_arialFont = glfLoadFont((char*)arialFontFileName.toLatin1().constData()); // BEST MATCH
    
    if (m_arialFont == GLF_ERROR) {
        CaretLogSevere("Unable to load font file " + arialFontFileName);
        return;
    }
    
    m_fontFileValid = true;
    
//    /*
//     * Find default font to use when the desired font name is emtpy
//     */
//    std::vector<QString> fontNames;
//    fontNames.push_back("Helvetica");
//    fontNames.push_back("Arial");
//    fontNames.push_back("Times");
//    
//    for (std::vector<QString>::iterator iter = fontNames.begin();
//         iter != fontNames.end();
//         iter++) {
//        QString name = *iter;
//        
//        QFont font(name);
//        if (font.exactMatch()) {
//            m_emptyNameFont = name;
//            break;
//        }
//    }
//    
//    CaretLogFine("Default font is: "
//                   + m_emptyNameFont);
}

/**
 * Destructor.
 */
GlfFontTextRenderer::~GlfFontTextRenderer()
{
//    for (std::vector<FontData*>::iterator fontIter = m_fonts.begin();
//         fontIter != m_fonts.end();
//         fontIter++) {
//        delete *fontIter;
//    }
//    m_fonts.clear();
}

void
GlfFontTextRenderer::drawString(char* str)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-100.0, 100.0, -100.0, 100.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    
    glfSetCurrentFont(m_arialFont);
    float xmin,ymin,xmax,ymax;
    glfGetStringBounds(str, &xmin,&ymin,&xmax,&ymax);
    
    glColor3f(1.0, 0.0, 0.0);
    
    glColor3f(1.0, 1.0, 1.0);
    
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(-(xmin+xmax) / 2.0, 0.0, 0.0);
    glScalef(10.0, 10.0, 1.0);
    glfDrawSolidString((char*)str);
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
}

/**
 * @return The font system is valid.
 */
bool
GlfFontTextRenderer::isValid() const
{
    return m_fontFileValid;
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
GlfFontTextRenderer::drawTextAtWindowCoords(const int viewport[4],
                                                      const int windowX,
                                                      const int windowY,
                                                      const QString& text,
                                                      const TextAlignmentX alignmentX,
                                                      const TextAlignmentY alignmentY,
                                                      const TextStyle /*textStyle*/,
                                                      const int fontHeightIn,
                                                      const AString& /*fontName*/)
{
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
    glOrtho(0,
               (viewport[0] + viewport[2]),
               0,
               (viewport[1] + viewport[3]),
               0,
               1);
    
    /*
     * Viewing projection is just the identity matrix since
     * we are drawing in window coordinates.
     */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    /*
     * Set the viewport so that its origin is in the bottom left corner
     * at the top right corner is the top right corner of the user's 
     * viewport.
     */
    glViewport(0,
            0,
               (viewport[0] + viewport[2]),
            (viewport[1] + viewport[3]));

    const float fontHeight = ((fontHeightIn > 0) ? fontHeightIn : 14);
    //const float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
    //const float aspect = static_cast<float>(viewport[3]) / static_cast<float>(viewport[2]);
    const float aspect = 0.8;
    const float fontSizeY = fontHeight;
    const float fontSizeX = fontHeight * aspect;
   
    char* str = (char*)text.toLatin1().constData();
    glfSetCurrentFont(m_arialFont);
    float xmin,ymin,xmax,ymax;
    glfGetStringBounds(str, &xmin,&ymin,&xmax,&ymax);
//    const float width = (xmax - xmin) * (fontSizeX);
//    const float height = (ymax - ymin) * (fontSizeY);
    
    const float scaleX = fontSizeX / 2.0 * 1.2;
    const float scaleY = fontSizeY / 2.0 * 1.2;
    const float width = (xmax - xmin) * (scaleX / 1.2);
    const float height = (ymax - ymin) * (scaleY);
    
    /* 
     * By default, it appears that the center of the character is drawn
     * at the coordinate.
     */
    int x = windowX + viewport[0];
    switch (alignmentX) {
        case X_LEFT:
            x += (width / 2.0);
            break;
        case X_CENTER:
            break;
        case X_RIGHT:
            x -= (width / 2.0);
            break;
    }
//    switch (alignmentX) {
//        case X_LEFT:
//            x += (width / 2.0);
//            break;
//        case X_CENTER:
//            break;
//        case X_RIGHT:
//            x -= (width / 2.0);
//            break;
//    }

    /*
     * Y-Coordinate of text
     */
    int y = windowY + viewport[1];
    switch (alignmentY) {
        case Y_BOTTOM:
            y += (height / 2.0);
            break;
        case Y_CENTER:
            break;
        case Y_TOP:
            y -= (height / 2.0);
            break;
    }
    
//    glPushMatrix();
//    glLoadIdentity();
//    glTranslatef(x, y, 0.0);
//    glColor3f(1.0, 0.0, 0.0);
//    glBegin(GL_QUADS);
//    glVertex2f(((-width) / 2),
//               ((-height) / 2));
//    glVertex2f((( width) / 2),
//               ((-height) / 2));
//    glVertex2f((( width) / 2),
//               (( height) / 2));
//    glVertex2f(((-width) / 2),
//               (( height) / 2));
//    glEnd();
//    glPopMatrix();
    
    
//    glColor3f(1.0, 1.0, 1.0);
    
//    glPushMatrix();
//    glLoadIdentity();
    glTranslatef(x, y, 0.0);
//    const float scaleX = fontSizeX / 2.0 * 1.2;
//    const float scaleY = fontSizeY / 2.0 * 1.2;
    glScalef(scaleX, scaleY, 1.0);
    //glScalef(fontSizeX, fontSizeY, 1.0);
    glfDrawSolidString((char*)str);
//    glfDrawWiredString((char*)str);
//    glPopMatrix();
//    
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
//    glMatrixMode(GL_MODELVIEW);

    
//    drawString((char*)text.toLatin1().constData());

//    /*
//     * Find font
//     */
//    QFont* font = findFont(fontName,
//                           fontHeight,
//                           textStyle);
//    CaretAssert(font);
//    
//    QFontMetrics fontMetrics(*font);
//    
//    /*
//     * X-Coordinate of text
//     */
//    int x = windowX + viewport[0];
//    switch (alignmentX) {
//        case X_LEFT:
//            break;
//        case X_CENTER:
//            x -= fontMetrics.width(text) / 2;
//            break;
//        case X_RIGHT:
//            x -= fontMetrics.width(text);
//            break;
//    }
//    
//    /*
//     * Y-Coordinate of text
//     * Note that QGLWidget has origin at top left corner
//     */
//    int y = m_glWidget->height() - (windowY + viewport[1]);
//    switch (alignmentY) {
//        case Y_BOTTOM:
//            break;
//        case Y_CENTER:
//            y += fontMetrics.height() / 2;
//            break;
//        case Y_TOP:
//            y += fontMetrics.height();
//            break;
//    }
//    
//    m_glWidget->renderText(x,
//                               y,
//                               text,
//                               *font);
    
    restoreStateOfOpenGL();
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
GlfFontTextRenderer::getTextBoundsInPixels(const QString& text,
                                   const TextStyle textStyle,
                                   const int fontHeight,
                                           const AString& fontName,
                                           int32_t& widthOut,
                                           int32_t& heightOut)
{
    const float width = fontHeight * 0.80;
    const float numberOfCharacters = text.length();
    
    widthOut  = width * numberOfCharacters;
    heightOut = fontHeight * numberOfCharacters;
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
GlfFontTextRenderer::drawTextAtModelCoords(const double modelX,
                                                     const double modelY,
                                                     const double modelZ,
                                                     const QString& text,
                                                     const TextStyle /*textStyle*/,
                                                     const int fontHeight,
                                                     const AString& /*fontName*/)
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
        
        saveStateOfOpenGL();
        
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
                (viewport[0] + viewport[2]),
                0,
                (viewport[1] + viewport[3]),
                0,
                1);
        
        /*
         * Viewing projection is just the identity matrix since
         * we are drawing in window coordinates.
         */
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        /*
         * Set the viewport so that its origin is in the bottom left corner
         * at the top right corner is the top right corner of the user's
         * viewport.
         */
        glViewport(0,
                   0,
                   (viewport[0] + viewport[2]),
                   (viewport[1] + viewport[3]));
        
        glTranslatef(windowX, windowY, -windowZ);
        
        const float scaleXY = fontHeight / 2.0 * 1.2;
        glScalef(scaleXY, scaleXY, 1.0);
        //glScalef(fontSizeX, fontSizeY, 1.0);
        const char* str = text.toLatin1().constData();
        glfDrawSolidString((char*)str);
        
        
        restoreStateOfOpenGL();
    }
    else {
        CaretLogSevere("gluProject() failed for drawing text at model coordinates.");
    }
    
//    /*
//     * Find font
//     */
//    QFont* font = findFont(fontName,
//                           fontHeight,
//                           textStyle);
//    CaretAssert(font);
//    
//    m_glWidget->renderText(modelX,
//                               modelY, 
//                               modelZ, 
//                               text,
//                               *font);
}

///**
// * Find a font with the given name, height, and style.
// * Once a font is created it is cached so that it can be
// * used again and avoids font creation which may be 
// * expensive.
// *
// * @return a font
// */
//QFont*
//GlfFontTextRenderer::findFont(const QString& fontNameIn,
//                                        const int fontHeight,
//                                        const TextStyle textStyle)
//
//{
//    QString fontName = fontNameIn;
//    if (fontName.isEmpty()) {
//        fontName = m_emptyNameFont;
//    }
//    
//    for (std::vector<FontData*>::iterator fontIter = m_fonts.begin();
//         fontIter != m_fonts.end();
//         fontIter++) {
//        FontData* fd = *fontIter;
//        if ((fd->m_fontName == fontName)
//            && (fd->m_fontHeight == fontHeight)
//            && (fd->m_textStyle == textStyle)) {
//            return fd->m_font;
//        }
//    }
//    
//    /*
//     * Set font.
//     */
//    QFont* font = new QFont(fontName, fontHeight);
//    
//    switch (textStyle) {
//        case BrainOpenGLTextRenderInterface::BOLD:
//            font->setBold(true);
//            break;
//        case BrainOpenGLTextRenderInterface::ITALIC:
//            font->setItalic(true);
//            break;
//        case BrainOpenGLTextRenderInterface::NORMAL:
//            break;
//    }
//    
//    FontData* fd = new FontData(font,
//                                fontName,
//                                fontHeight,
//                                textStyle);
//    m_fonts.push_back(fd);
//
//    CaretLogFine("Created font: "
//                   + fontName
//                   + " size="
//                   + QString::number(fontHeight));
//    
//    return fd->m_font;
//}
//
//GlfFontTextRenderer::FontData::FontData(QFont* font,
//                                                  const QString fontName,
//                                                  const int fontHeight,
//                                                  const TextStyle textStyle) {
//    m_font = font;
//    m_fontName = fontName;
//    m_fontHeight = fontHeight;
//    m_textStyle = textStyle;
//}
//
//GlfFontTextRenderer::FontData::~FontData() {
//    delete m_font;
//}

/**
 * Save the state of OpenGL.
 * Copied from Qt's qgl.cpp, qt_save_gl_state().
 */
void
GlfFontTextRenderer::saveStateOfOpenGL()
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
GlfFontTextRenderer::restoreStateOfOpenGL()
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
