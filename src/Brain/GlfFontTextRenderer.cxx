
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

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
    
    glfInit();
    
    m_CourierFont = glfLoadFont("/Users/john/caret7_development/caret7_source/src/GlfFont/courier1.glf");
    
    if (m_CourierFont == GLF_ERROR) {
        CaretAssert(0);
    }
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
    
    glfSetCurrentFont(m_CourierFont);
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
                                                      const TextStyle textStyle,
                                                      const int fontHeightIn,
                                                      const AString& fontName)
{
//    drawString((char*)text.toLatin1().constData());
//    return ;
    

    /* NEED TO DISABLE DEPTH */
    
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(viewport[0],
            (viewport[0] + viewport[2]),
            viewport[1],
            (viewport[1] + viewport[3]),
            -100,
             100);
    glMatrixMode(GL_MODELVIEW);
    
    const float fontHeight = ((fontHeightIn > 0) ? fontHeightIn : 14);
    const float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
    const float fontSizeY = fontHeight;
    const float fontSizeX = fontHeight * aspect;
   
    char* str = (char*)text.toLatin1().constData();
    glfSetCurrentFont(m_CourierFont);
    float xmin,ymin,xmax,ymax;
    glfGetStringBounds(str, &xmin,&ymin,&xmax,&ymax);
    const float width = (xmax - xmin) * fontSizeX;
    const float height = (ymax - ymin) * fontSizeY;
    
    
    int x = windowX;
    switch (alignmentX) {
        case X_LEFT:
            break;
        case X_CENTER:
            x -= width / 2;
            break;
        case X_RIGHT:
            x -= width;
            break;
    }

    /*
     * Y-Coordinate of text
     * Note that QGLWidget has origin at top left corner
     */
    int y = windowY;
    switch (alignmentY) {
        case Y_BOTTOM:
            break;
        case Y_CENTER:
            y -= height / 2;
            break;
        case Y_TOP:
            y -= height;
            break;
    }
    
    
    glMatrixMode(GL_MODELVIEW);
    glColor3f(1.0, 0.0, 0.0);
    
    glColor3f(1.0, 1.0, 1.0);
    
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x, y, 0.0);
    glScalef(fontSizeX / 2, fontSizeY / 2, 1.0);
    glfDrawSolidString((char*)str);
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    
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
                                                     const TextStyle textStyle,
                                                     const int fontHeight,
                                                     const AString& fontName)
{
    drawString((char*)text.toLatin1().constData());
    
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


