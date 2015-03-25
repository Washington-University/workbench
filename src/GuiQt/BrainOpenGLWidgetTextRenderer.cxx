
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
    
    m_normalFont.initialize("Arial", BrainOpenGLTextAttributes::NORMAL);
    m_boldFont.initialize("Arial", BrainOpenGLTextAttributes::BOLD);
}

/**
 * Destructor.
 */
BrainOpenGLWidgetTextRenderer::~BrainOpenGLWidgetTextRenderer()
{
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
 * @param textAttributes
 *   Attributes for text drawing.
 */
void
BrainOpenGLWidgetTextRenderer::drawTextAtWindowCoords(const int viewport[4],
                                                      const double windowX,
                                                      const double windowY,
                                                      const QString& text,
                                                      const BrainOpenGLTextAttributes& textAttributes)
{
    double width, height;
    getTextBoundsInPixels(width,
                          height,
                          text,
                          textAttributes);
    
    /*
     * Find font
     */
    QFont* font = findFont(textAttributes.getStyle(),
                           textAttributes.getFontHeight());
    if (font == NULL) {
        return;
    }
    
    /*
     * X-Coordinate of text
     */
    int x = windowX + viewport[0];
    switch (textAttributes.getHorizontalAlignment()) {
        case BrainOpenGLTextAttributes::X_LEFT:
            break;
        case BrainOpenGLTextAttributes::X_CENTER:
            x -= width / 2;
            break;
        case BrainOpenGLTextAttributes::X_RIGHT:
            x -= width;
            break;
    }
    
    /*
     * Y-Coordinate of text
     * Note that QGLWidget has origin at top left corner
     */
    int y = m_glWidget->height() - (windowY + viewport[1]);
    switch (textAttributes.getVerticalAlignment()) {
        case BrainOpenGLTextAttributes::Y_BOTTOM:
            break;
        case BrainOpenGLTextAttributes::Y_CENTER:
            y += height / 2;
            break;
        case BrainOpenGLTextAttributes::Y_TOP:
            y += height;
            break;
    }
    
    m_glWidget->renderText(x,
                           y,
                           text,
                           *font);
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
BrainOpenGLWidgetTextRenderer::drawVerticalTextAtWindowCoords(const int viewport[4],
                                                              const double windowX,
                                                              const double windowY,
                                                              const QString& text,
                                                              const BrainOpenGLTextAttributes&  textAttributes)
{
    if (text.isEmpty()) {
        return;
    }
    
    QString textToDraw = text;
    const int32_t numChars = textToDraw.length();
    if (numChars > 1) {
        for (int32_t i = 0; i < numChars; i++) {
            textToDraw.append(text[i]);
            if (i < (numChars - 1)) {
                textToDraw.append("|");
            }
        }
    }
    
    drawTextAtWindowCoords(viewport,
                           windowX,
                           windowY,
                           textToDraw,
                           textAttributes);
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
BrainOpenGLWidgetTextRenderer::drawTextAtModelCoords(const double modelX,
                                                     const double modelY,
                                                     const double modelZ,
                                                     const QString& text,
                                                     const BrainOpenGLTextAttributes& textAttributes)
{
    /*
     * Find font
     */
    QFont* font = findFont(textAttributes.getStyle(),
                           textAttributes.getFontHeight());
    if (font == NULL) {
        return;
    }
    
    m_glWidget->renderText(modelX,
                               modelY, 
                               modelZ, 
                               text,
                               *font);
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
BrainOpenGLWidgetTextRenderer::getTextBoundsInPixels(double& widthOut,
                                                     double& heightOut,
                                                     const QString& text,
                                                     const BrainOpenGLTextAttributes& textAttributes)
{
    QFont* font = findFont(textAttributes.getStyle(),
                           textAttributes.getFontHeight());
    if (font == NULL) {
        return;
    }
    
    QFontMetricsF fontMetrics(*font);
    QRectF boundsRect = fontMetrics.boundingRect(text);
    widthOut  = boundsRect.width();
    heightOut = boundsRect.height();
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
BrainOpenGLWidgetTextRenderer::findFont(const BrainOpenGLTextAttributes::Style textStyle,
                                        const int fontHeight)

{
    QFont* font = NULL;
    
    switch (textStyle) {
        case BrainOpenGLTextAttributes::BOLD:
            if (m_boldFont.m_fontValid) {
                font = m_boldFont.m_font;
            }
            break;
        case BrainOpenGLTextAttributes::NORMAL:
            if (m_normalFont.m_fontValid) {
                font = m_normalFont.m_font;
            }
            break;
    }
    font->setPointSize(fontHeight);
    
    return font;
}

/**
 * Constructor.
 */
BrainOpenGLWidgetTextRenderer::FontData::FontData() {
    m_font = NULL;
    m_fontValid = false;
}

/**
 * Destructor.
 */
BrainOpenGLWidgetTextRenderer::FontData::~FontData() {
    delete m_font;
    m_font = NULL;
}

/**
 * @param fontName
 *    Name of font.
 * @param textStyle
 *    Style of font.
 */
void
BrainOpenGLWidgetTextRenderer::FontData::initialize(const AString& fontName,
                                                    const BrainOpenGLTextAttributes::Style textStyle)
{
    m_font = new QFont(fontName);
    
    switch (textStyle) {
        case BrainOpenGLTextAttributes::BOLD:
            m_font->setBold(true);
            break;
        case BrainOpenGLTextAttributes::NORMAL:
            break;
    }
    
    m_fontValid = true;
}

/**
 * @return Name of the text renderer.
 */
AString
BrainOpenGLWidgetTextRenderer::getName() const
{
    return "QT's QGLWidget";
}



