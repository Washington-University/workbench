
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

#define __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER_DECLARE__
#include "BrainOpenGLWidgetTextRenderer.h"
#undef __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER_DECLARE__

#include <QGLWidget>

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLWidgetTextRenderer 
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
BrainOpenGLWidgetTextRenderer::BrainOpenGLWidgetTextRenderer(QGLWidget* glWidget)
: BrainOpenGLTextRenderInterface()
{
    m_glWidget = glWidget;
    
    /*
     * Find default font to use when the desired font name is emtpy
     */
    std::vector<QString> fontNames;
    fontNames.push_back("Helvetica");
    fontNames.push_back("Arial");
    fontNames.push_back("Times");
    
    for (std::vector<QString>::iterator iter = fontNames.begin();
         iter != fontNames.end();
         iter++) {
        QString name = *iter;
        
        QFont font(name);
        if (font.exactMatch()) {
            m_emptyNameFont = name;
            break;
        }
    }
    
    CaretLogFine("Default font is: "
                   + m_emptyNameFont);
}

/**
 * Destructor.
 */
BrainOpenGLWidgetTextRenderer::~BrainOpenGLWidgetTextRenderer()
{
    for (std::vector<FontData*>::iterator fontIter = m_fonts.begin();
         fontIter != m_fonts.end();
         fontIter++) {
        delete *fontIter;
    }
    m_fonts.clear();
}

/**
 * @return The font system is valid.
 */
bool
BrainOpenGLWidgetTextRenderer::isValid() const
{
    return true;
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
BrainOpenGLWidgetTextRenderer::drawTextAtWindowCoords(const int viewport[4],
                                                      const int windowX,
                                                      const int windowY,
                                                      const QString& text,
                                                      const TextAlignmentX alignmentX,
                                                      const TextAlignmentY alignmentY,
                                                      const TextStyle textStyle,
                                                      const int fontHeight,
                                                      const AString& fontName)
{
    /*
     * Find font
     */
    QFont* font = findFont(fontName,
                           fontHeight,
                           textStyle);
    CaretAssert(font);
    
    QFontMetrics fontMetrics(*font);
    
    /*
     * X-Coordinate of text
     */
    int x = windowX + viewport[0];
    switch (alignmentX) {
        case X_LEFT:
            break;
        case X_CENTER:
            x -= fontMetrics.width(text) / 2;
            break;
        case X_RIGHT:
            x -= fontMetrics.width(text);
            break;
    }
    
    /*
     * Y-Coordinate of text
     * Note that QGLWidget has origin at top left corner
     */
    int y = m_glWidget->height() - (windowY + viewport[1]);
    switch (alignmentY) {
        case Y_BOTTOM:
            break;
        case Y_CENTER:
            y += fontMetrics.height() / 2;
            break;
        case Y_TOP:
            y += fontMetrics.height();
            break;
    }
    
    m_glWidget->renderText(x,
                               y,
                               text,
                               *font);
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
BrainOpenGLWidgetTextRenderer::drawTextAtModelCoords(const double modelX,
                                                     const double modelY,
                                                     const double modelZ,
                                                     const QString& text,
                                                     const TextStyle textStyle,
                                                     const int fontHeight,
                                                     const AString& fontName)
{
    /*
     * Find font
     */
    QFont* font = findFont(fontName,
                           fontHeight,
                           textStyle);
    CaretAssert(font);
    
    m_glWidget->renderText(modelX,
                               modelY, 
                               modelZ, 
                               text,
                               *font);
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
BrainOpenGLWidgetTextRenderer::findFont(const QString& fontNameIn,
                                        const int fontHeight,
                                        const TextStyle textStyle)

{
    QString fontName = fontNameIn;
    if (fontName.isEmpty()) {
        fontName = m_emptyNameFont;
    }
    
    for (std::vector<FontData*>::iterator fontIter = m_fonts.begin();
         fontIter != m_fonts.end();
         fontIter++) {
        FontData* fd = *fontIter;
        if ((fd->m_fontName == fontName)
            && (fd->m_fontHeight == fontHeight)
            && (fd->m_textStyle == textStyle)) {
            return fd->m_font;
        }
    }
    
    /*
     * Set font.
     */
    QFont* font = new QFont(fontName, fontHeight);
    
    switch (textStyle) {
        case BrainOpenGLTextRenderInterface::BOLD:
            font->setBold(true);
            break;
        case BrainOpenGLTextRenderInterface::ITALIC:
            font->setItalic(true);
            break;
        case BrainOpenGLTextRenderInterface::NORMAL:
            break;
    }
    
    FontData* fd = new FontData(font,
                                fontName,
                                fontHeight,
                                textStyle);
    m_fonts.push_back(fd);

    CaretLogFine("Created font: "
                   + fontName
                   + " size="
                   + QString::number(fontHeight));
    
    return fd->m_font;
}

BrainOpenGLWidgetTextRenderer::FontData::FontData(QFont* font,
                                                  const QString fontName,
                                                  const int fontHeight,
                                                  const TextStyle textStyle) {
    m_font = font;
    m_fontName = fontName;
    m_fontHeight = fontHeight;
    m_textStyle = textStyle;
}

BrainOpenGLWidgetTextRenderer::FontData::~FontData() {
    delete m_font;
}


