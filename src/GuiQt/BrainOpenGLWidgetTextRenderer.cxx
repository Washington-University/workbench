
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

using namespace caret;


    
/**
 * \class BrainOpenGLWidgetTextRenderer 
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
    this->glWidget = glWidget;
}

/**
 * Destructor.
 */
BrainOpenGLWidgetTextRenderer::~BrainOpenGLWidgetTextRenderer()
{
    
}

/**
 * Draw text at the given window coordinates.
 *
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
BrainOpenGLWidgetTextRenderer::drawTextAtWindowCoords(const int windowX,
                                                      const int windowY,
                                                      const QString& text,
                                                      const TextAlignment alignment,
                                                      const TextStyle textStyle,
                                                      const int fontHeight,
                                                      const AString& fontName)
{
    /*
     * Set font.
     */
    QFont font(fontName, fontHeight);
    QFontMetrics fontMetrics(font);

    switch (textStyle) {
        case BrainOpenGLTextRenderInterface::BOLD:
            font.setBold(true);
            break;
        case BrainOpenGLTextRenderInterface::ITALIC:
            font.setItalic(true);
            break;
        case BrainOpenGLTextRenderInterface::NORMAL:
            break;
    }
    
    /*
     * QGLWidget has origin at top left corner
     */
    const int y = glWidget->height() - windowY;
    
    /*
     * X-Coordinate of text
     */
    int x = windowX;
    switch (alignment) {
        case LEFT:
            break;
        case CENTER:
            x -= fontMetrics.width(text) / 2;
            break;
        case RIGHT:
            x -= fontMetrics.width(text);
            break;
    }
    
    this->glWidget->renderText(x,
                               y,
                               text,
                               font);
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
    QFont font(fontName, fontHeight);

    switch (textStyle) {
        case BrainOpenGLTextRenderInterface::BOLD:
            font.setBold(true);
            break;
        case BrainOpenGLTextRenderInterface::ITALIC:
            font.setItalic(true);
            break;
        case BrainOpenGLTextRenderInterface::NORMAL:
            break;
    }
    
    this->glWidget->renderText(modelX, 
                               modelY, 
                               modelZ, 
                               text,
                               font);
}

