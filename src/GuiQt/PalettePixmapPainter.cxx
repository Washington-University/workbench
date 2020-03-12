
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __PALETTE_PIXMAP_PAINTER_DECLARE__
#include "PalettePixmapPainter.h"
#undef __PALETTE_PIXMAP_PAINTER_DECLARE__

#include <QBrush>
#include <QLinearGradient>
#include <QPainter>

#include "CaretAssert.h"
#include "MathFunctions.h"
#include "Palette.h"
#include "PaletteScalarAndColor.h"

using namespace caret;


    
/**
 * \class caret::PalettePixmapPainter 
 * \brief Generates a pixmap containing a palette's coloring
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param palette
 * Palette used to generate the pixmap
 * @param mode
 * Interpolation mode (on/off)
 */
PalettePixmapPainter::PalettePixmapPainter(const Palette* palette,
                                           const Mode mode)
: PalettePixmapPainter(palette,
                       QSize(100, 14),
                       mode)
{
}

/**
 * Constructor.
 * @param palette
 * Palette used to generate the pixmap
 * @param pixmapSize
 * Size for pixmap
 * @param mode
 * Interpolation mode (on/off)
 */
PalettePixmapPainter::PalettePixmapPainter(const Palette* palette,
                                           const QSize& pixmapSize,
                                           const Mode mode)
: CaretObject(),
m_mode(mode)
{
    CaretAssert(palette);
    if (palette == NULL) {
        return;
    }
    
    if (palette->getNumberOfScalarsAndColors() == 0) {
        return;
    }
    
    const qreal pixmapWidth(pixmapSize.width());
    const qreal pixmapHeight(pixmapSize.height());
    
    switch (m_mode) {
        case Mode::INTERPOLATE_OFF:
            createPixmapInterpolateOff(palette,
                                       pixmapWidth,
                                       pixmapHeight);
            break;
        case Mode::INTERPOLATE_ON:
            createPixmapInterpolateOn(palette,
                                      pixmapWidth,
                                      pixmapHeight);
            break;
    }
}

/**
 * Destructor.
 */
PalettePixmapPainter::~PalettePixmapPainter()
{
}

float
PalettePixmapPainter::nonInterpScalarToPixMap(const float pixmapWidth,
                        const float scalarIn)
{
    float scalar(MathFunctions::limitRange(scalarIn, -1.0f, 1.0f));
    float normalized((scalar + 1.0) / 2.0);
    float valueOut(normalized * pixmapWidth);
    return valueOut;
}

/**
 * Create the pixmap for the interpolate OFF mode
 * @param palette
 *     The palette
 * @param pixmapWidth
 *    Width of pixmap
 * @param pixmapHeight
 *    Height of pixmap
 */
void
PalettePixmapPainter::createPixmapInterpolateOff(const Palette* palette,
                                                 const qreal pixmapWidth,
                                                 const qreal pixmapHeight)
{
    m_pixmap = QPixmap(static_cast<int>(pixmapWidth),
                       static_cast<int>(pixmapHeight));
    
    QPainter painter(&m_pixmap);
    painter.setPen(Qt::NoPen);
    
    float rightX(pixmapWidth);
    const int32_t numPoints = palette->getNumberOfScalarsAndColors();
    for (int32_t i = 0; i < numPoints; i++) {
        const PaletteScalarAndColor* psac = palette->getScalarAndColor(i);
        CaretAssert(psac);
        
        float startX(0);
        if (i < (numPoints - 1)) {
            startX = nonInterpScalarToPixMap(pixmapWidth,
                                             palette->getScalarAndColor(i + 1)->getScalar());
        }
        
        /*
         * Extend to prevent gaps dues to rounding
         */
        QRect rect(startX - 1, 0,  /* X, Y */
                   rightX - startX + 1, pixmapHeight);  /* Width, Height */
        float rgba[4];
        psac->getColor(rgba);
        QColor color;
        color.setRgbF(rgba[0], rgba[1], rgba[2]);
        
        QBrush brush(color);
        painter.setBrush(brush);
        painter.drawRect(rect);
        
        rightX = startX;
    }
}


/**
 * Create the pixmap for the interpolate ON mode
 * @param palette
 *     The palette
 * @param pixmapWidth
 *    Width of pixmap
 * @param pixmapHeight
 *    Height of pixmap
 */
void
PalettePixmapPainter::createPixmapInterpolateOn(const Palette* palette,
                                                const qreal pixmapWidth,
                                                const qreal pixmapHeight)
{
    m_pixmap = QPixmap(static_cast<int>(pixmapWidth),
                       static_cast<int>(pixmapHeight));
    
    const qreal halfHeight(pixmapHeight / 2.0);
    QLinearGradient linearGradient(QPointF(0, halfHeight),
                                   QPointF(pixmapWidth, halfHeight));
    linearGradient.setCoordinateMode(QGradient::LogicalMode);
    linearGradient.setSpread(QGradient::PadSpread);

    const int32_t numPoints = palette->getNumberOfScalarsAndColors();
    for (int32_t i = 0; i < numPoints; i++) {
        const PaletteScalarAndColor* psac = palette->getScalarAndColor(i);
        CaretAssert(psac);
        
        const float scalar(MathFunctions::limitRange(psac->getScalar(), -1.0f, 1.0f));
        const float gradientValue((scalar / 2.0f) + 0.5f);
        float rgba[4];
        psac->getColor(rgba);
        QColor color;
        color.setRgbF(rgba[0], rgba[1], rgba[2]);
        
        linearGradient.setColorAt(gradientValue, color);
    }
    
    QPainter painter(&m_pixmap);
    painter.setPen(Qt::NoPen);
    painter.setBrush(linearGradient);
    painter.drawRect(m_pixmap.rect());
}

/**
 * @return Pixmap containing palettes colors.
 * Test validity with .isNull()
 */
QPixmap
PalettePixmapPainter::getPixmap() const
{
    return m_pixmap;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
PalettePixmapPainter::toString() const
{
    return "PalettePixmapPainter";
}

