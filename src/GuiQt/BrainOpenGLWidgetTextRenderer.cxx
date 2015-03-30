
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

#include <cmath>
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
    
    m_normalFont.initialize("Arial", false);
    m_boldFont.initialize("Arial", false);
    
    CaretLogSevere("This text render is not functioning correctly and should not be used.  Use the FTGL text renderer.");
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
 * Draw text at the given VIEWPORT coordinates.
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
 * @param text
 *   Text that is to be drawn.
 * @param textAttributes
 *   Attributes for text drawing.
 */
void
BrainOpenGLWidgetTextRenderer::drawTextAtViewportCoords(const int viewport[4],
                                                      const double windowX,
                                                      const double windowY,
                                                      const QString& text,
                                                      const BrainOpenGLTextAttributes& textAttributes)
{
    switch (textAttributes.getOrientation()) {
        case BrainOpenGLTextAttributes::LEFT_TO_RIGHT:
            drawHorizontalTextAtWindowCoords(viewport,
                                             windowX,
                                             windowY,
                                             text,
                                             textAttributes);
            break;
        case BrainOpenGLTextAttributes::TOP_TO_BOTTOM:
            drawVerticalTextAtWindowCoords(viewport,
                                           windowX,
                                           windowY,
                                           text,
                                           textAttributes);
            break;
    }
    
//    double width, height;
//    getTextBoundsInPixels(width,
//                          height,
//                          text,
//                          textAttributes);
//    
//    
//    /*
//     * Find font
//     */
//    QFont* font = findFont(textAttributes.isBoldEnabled(),
//                           textAttributes.getFontHeight());
//    if (font == NULL) {
//        return;
//    }
//    
//    /*
//     * X-Coordinate of text
//     */
//    int x = windowX + viewport[0];
//    switch (textAttributes.getHorizontalAlignment()) {
//        case BrainOpenGLTextAttributes::X_LEFT:
//            break;
//        case BrainOpenGLTextAttributes::X_CENTER:
//            x -= width / 2;
//            break;
//        case BrainOpenGLTextAttributes::X_RIGHT:
//            x -= width;
//            break;
//    }
//    
//    /*
//     * Y-Coordinate of text
//     * Note that QGLWidget has origin at top left corner
//     */
//    int y = m_glWidget->height() - (windowY + viewport[1]);
//    switch (textAttributes.getVerticalAlignment()) {
//        case BrainOpenGLTextAttributes::Y_BOTTOM:
//            break;
//        case BrainOpenGLTextAttributes::Y_CENTER:
//            y += height / 2;
//            break;
//        case BrainOpenGLTextAttributes::Y_TOP:
//            y += height;
//            break;
//    }
//    
//    m_glWidget->renderText(x,
//                           y,
//                           text,
//                           *font);
}

/**
 * Draw horizontal text at the given window coordinates.
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
BrainOpenGLWidgetTextRenderer::drawHorizontalTextAtWindowCoords(const int viewport[4],
                                                      const double windowX,
                                                      const double windowY,
                                                      const QString& text,
                                                      const BrainOpenGLTextAttributes& textAttributes)
{
    std::cout << "Draw horiz string " << qPrintable(text) << " at " << windowX << ", " << windowY << std::endl;
    
    double textMinX = 0.0;
    double textMaxX = 0.0;
    double textMinY = 0.0;
    double textMaxY = 0.0;
    getTextBoundsInPixels(text,
                          textAttributes,
                          textMinX,
                          textMaxX,
                          textMinY,
                          textMaxY);
    const double width  = textMaxX - textMinX;
    const double height = textMaxY - textMinY;
    
    switch (textAttributes.getOrientation()) {
        case BrainOpenGLTextAttributes::LEFT_TO_RIGHT:
            break;
        case BrainOpenGLTextAttributes::TOP_TO_BOTTOM:
            break;
    }
    
    /*
     * Find font
     */
    QFont* font = findFont(textAttributes.isBoldEnabled(),
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
            x -= (width / 2.0);
            break;
        case BrainOpenGLTextAttributes::X_RIGHT:
            x -= width;
            break;
    }
    
    /*
     * Y-Coordinate of text
     * Note that QGLWidget has origin at top left corner
     */
    int y = windowY + viewport[1];
    switch (textAttributes.getVerticalAlignment()) {
        case BrainOpenGLTextAttributes::Y_BOTTOM:
            break;
        case BrainOpenGLTextAttributes::Y_CENTER:
            y -= (height / 2.0);
            break;
        case BrainOpenGLTextAttributes::Y_TOP:
            y -= height;
            break;
    }
    
    const double SPACING = 3;
    
    float backgroundRGBA[4];
    textAttributes.getBackgroundColor(backgroundRGBA);
    backgroundRGBA[0] = 0.7;
    backgroundRGBA[1] = 0.7;
    backgroundRGBA[2] = 0.7;
    backgroundRGBA[3] = 1.0;
    if (backgroundRGBA[3] > 0.0) {
//        GLint savedViewport[4];
//        glGetIntegerv(GL_VIEWPORT, savedViewport);
//        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
//        glMatrixMode(GL_PROJECTION);
//        glPushMatrix();
//        glLoadIdentity();
//        glOrtho(viewport[0], viewport[1], viewport[2], viewport[3], -1.0, 1.0);
//        glMatrixMode(GL_MODELVIEW);
//        glPushMatrix();
//        glLoadIdentity();
        glColor3fv(backgroundRGBA);
        const double minX = x - viewport[0] - SPACING;
        const double maxX = minX + width + SPACING;
        const double minY = y - viewport[1] - SPACING;
        const double maxY = minY + height + SPACING;
        glPushMatrix();
        glLoadIdentity();
        glRectf(minX, minY,
                maxX, maxY);
        glPopMatrix();
        std::cout << "Horiz box for " << qPrintable(text) << ": " << minX << ", " << minY << ", " << maxX << ", " << maxY << std::endl;
//        glPopMatrix();
//        glMatrixMode(GL_PROJECTION);
//        glPopMatrix();
//        glMatrixMode(GL_MODELVIEW);
//        glViewport(savedViewport[0], savedViewport[1], savedViewport[2], savedViewport[3]);
    }
    
    float foregroundRgba[4];
    textAttributes.getForegroundColor(foregroundRgba);
    glColor3fv(foregroundRgba);
    
    /*
     * Note: QGLWidget::renderText() has origin in TOP LEFT corner
     */
    m_glWidget->renderText(x,
                           m_glWidget->height() - y,
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
 * @param textAttributes
 *   Attributes for text drawing.
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
    
    /*
     * Find font
     */
    QFont* font = findFont(textAttributes.isBoldEnabled(),
                           textAttributes.getFontHeight());
    if (font == NULL) {
        return;
    }
    
    const double SPACING = 2;
    
    double totalWidth  = 0.0;
    std::vector<double> xChars;
    std::vector<double> yChars;
    
    double y = 0;
    const int32_t numChars = text.length();
    for (int32_t i = 0; i < numChars; i++) {
        const QString oneChar = text[i];
        double textMinX = 0.0;
        double textMaxX = 0.0;
        double textMinY = 0.0;
        double textMaxY = 0.0;
        getTextBoundsInPixels(oneChar,
                              textAttributes,
                              textMinX,
                              textMaxX,
                              textMinY,
                              textMaxY);
        const double width  = textMaxX - textMinX;
        const double height = textMaxY - textMinY;
        y -= height;
        xChars.push_back(0.0);
        yChars.push_back(y);
        y -= SPACING;
        
        totalWidth = std::max(totalWidth,
                              width);
        
    }
    double totalHeight = std::fabs(y);
    
    CaretAssert(numChars == static_cast<int32_t>(xChars.size()));
    CaretAssert(numChars == static_cast<int32_t>(yChars.size()));
    
    double xOffset = 0.0;
    switch (textAttributes.getHorizontalAlignment()) {
        case BrainOpenGLTextAttributes::X_CENTER:
            xOffset = -(totalWidth / 2.0);
            break;
        case BrainOpenGLTextAttributes::X_LEFT:
            break;
        case BrainOpenGLTextAttributes::X_RIGHT:
            xOffset = -totalWidth;
            break;
    }
    
    double yOffset = 0.0;
    switch (textAttributes.getVerticalAlignment()) {
        case BrainOpenGLTextAttributes::Y_BOTTOM:
            yOffset = totalHeight;
            break;
        case BrainOpenGLTextAttributes::Y_CENTER:
            yOffset = (totalHeight / 2.0);
            break;
        case BrainOpenGLTextAttributes::Y_TOP:
            break;
    }
    
    float backgroundRGBA[4];
    textAttributes.getBackgroundColor(backgroundRGBA);
    backgroundRGBA[0] = 0.7;
    backgroundRGBA[1] = 0.7;
    backgroundRGBA[2] = 0.7;
    backgroundRGBA[3] = 1.0;
    if (backgroundRGBA[3] > 0.0) {
        glColor3fv(backgroundRGBA);
        const double minX = windowX + xOffset - SPACING;
        const double maxX = minX + totalWidth + SPACING;
        const double maxY = windowY + yOffset;
        const double minY = maxY - totalHeight;
        std::cout << "Vert box for " << qPrintable(text) << ": " << minX << ", " << minY << ", " << maxX << ", " << maxY << std::endl;
        glRectf(minX, minY,
                maxX, maxY);
    }
    
    float foregroundRgba[4];
    textAttributes.getForegroundColor(foregroundRgba);
    glColor3fv(foregroundRgba);
    
    BrainOpenGLTextAttributes oneCharAtts = textAttributes;
    oneCharAtts.setHorizontalAlignment(BrainOpenGLTextAttributes::X_LEFT);
    oneCharAtts.setVerticalAlignment(BrainOpenGLTextAttributes::Y_BOTTOM);
    for (int32_t i = 0; i < numChars; i++) {
        const QString oneChar = text[i];
        
        /*
         * Note: QGLWidget::renderText() has origin in TOP LEFT corner
         */
        const double x = viewport[0] + windowX + xChars[i] + xOffset;
        const double y = m_glWidget->height() - (viewport[1] + windowY + yChars[i] + yOffset);
        std::cout << "Drawing: " << qPrintable(oneChar) << " at " << x << ", " << y << std::endl;
        m_glWidget->renderText(x,
                               y,
                               oneChar,
                               *font);
    }
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
    QFont* font = findFont(textAttributes.isBoldEnabled(),
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
 * Get the bounds of text (in pixels) using the given text
 * attributes.
 *
 * See http://ftgl.sourceforge.net/docs/html/metrics.png
 *
 * @param text
 *   Text that is to be drawn.
 * @param textAttributes
 *   Attributes for text drawing.
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
BrainOpenGLWidgetTextRenderer::getTextBoundsInPixels(const QString& text,
                                                     const BrainOpenGLTextAttributes& textAttributes,
                                                     double& xMinOut,
                                                     double& xMaxOut,
                                                     double& yMinOut,
                                                     double& yMaxOut)
{
    QFont* font = findFont(textAttributes.isBoldEnabled(),
                           textAttributes.getFontHeight());
    if (font == NULL) {
        return;
    }
    
    QFontMetricsF fontMetrics(*font);
    QRectF boundsRect = fontMetrics.boundingRect(text);
    xMinOut = boundsRect.left();
    xMaxOut = boundsRect.right();
    yMinOut = boundsRect.bottom();
    yMaxOut = boundsRect.top();
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
BrainOpenGLWidgetTextRenderer::findFont(const bool boldFlag,
                                        const int fontHeight)

{
    QFont* font = NULL;
    
    if (boldFlag) {
        if (m_boldFont.m_fontValid) {
            font = m_boldFont.m_font;
        }
    }
    else {
        if (m_normalFont.m_fontValid) {
            font = m_normalFont.m_font;
        }
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
 * @param boldFlag
 *    Bold enabled.
 */
void
BrainOpenGLWidgetTextRenderer::FontData::initialize(const AString& fontName,
                                                    const bool boldFlag)
{
    m_font = new QFont(fontName);
    
    if (boldFlag) {
        m_font->setBold(true);
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



