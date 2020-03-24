
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
#include "PaletteNew.h"
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
            createPalettePixmapInterpolateOff(palette,
                                       pixmapWidth,
                                       pixmapHeight);
            break;
        case Mode::INTERPOLATE_ON:
            createPalettePixmapInterpolateOn(palette,
                                      pixmapWidth,
                                      pixmapHeight);
            break;
    }
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
PalettePixmapPainter::PalettePixmapPainter(const PaletteNew* palette,
                                           const QSize& pixmapSize,
                                           const Mode mode)
: CaretObject(),
m_mode(mode)
{
    CaretAssert(palette);
    if (palette == NULL) {
        return;
    }
    
    const qreal pixmapWidth(pixmapSize.width());
    const qreal pixmapHeight(pixmapSize.height());
    
    switch (m_mode) {
        case Mode::INTERPOLATE_OFF:
            createPalettePixmapInterpolateOff(palette,
                                             pixmapWidth,
                                             pixmapHeight);
            break;
        case Mode::INTERPOLATE_ON:
            createPalettePixmapInterpolateOn(palette,
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
PalettePixmapPainter::paletteNonInterpScalarToPixMap(const float pixmapWidth,
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
PalettePixmapPainter::createPalettePixmapInterpolateOff(const Palette* palette,
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
            startX = paletteNonInterpScalarToPixMap(pixmapWidth,
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
PalettePixmapPainter::createPalettePixmapInterpolateOn(const Palette* palette,
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
 * Create the pixmap for the interpolate ON mode
 * @param palette
 *     The palette
 * @param pixmapWidth
 *    Width of pixmap
 * @param pixmapHeight
 *    Height of pixmap
 */
void
PalettePixmapPainter::createPalettePixmapInterpolateOn(const PaletteNew* palette,
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

    std::vector<PaletteNew::ScalarColor> posScalarsAndColors = palette->getPosRange();
    std::vector<PaletteNew::ScalarColor> scalarsAndColors = palette->getNegRange();
    scalarsAndColors.insert(scalarsAndColors.end(),
                            posScalarsAndColors.begin(),
                            posScalarsAndColors.end());
    
    
    const int32_t numPoints = static_cast<int32_t>(scalarsAndColors.size());
    for (int32_t i = 0; i < numPoints; i++) {
        CaretAssertVectorIndex(scalarsAndColors, i);
        const PaletteNew::ScalarColor& sc = scalarsAndColors[i];
        
        const float scalar(MathFunctions::limitRange(sc.scalar, -1.0f, 1.0f));
        const float gradientValue((scalar / 2.0f) + 0.5f);
        QColor color;
        color.setRgbF(sc.color[0], sc.color[1], sc.color[2]);
        
        linearGradient.setColorAt(gradientValue, color);
    }
    
    QPainter painter(&m_pixmap);
    QPen pen = painter.pen();
    painter.setPen(Qt::NoPen);
    painter.setBrush(linearGradient);
    painter.drawRect(m_pixmap.rect());
    
    const int32_t zeroPenWidth(std::min(pixmapWidth * 0.02,
                                        2.0));
    painter.setBrush(Qt::NoBrush);
    float zeroColor[3];
    palette->getZeroColor(zeroColor);
    QColor penColor;
    penColor.setRgbF(zeroColor[0], zeroColor[1], zeroColor[2]);
    pen.setColor(penColor);
    pen.setWidth(zeroPenWidth);
    
    painter.drawLine(pixmapWidth / 2, 0, pixmapWidth / 2, pixmapHeight);
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
PalettePixmapPainter::createPalettePixmapInterpolateOff(const PaletteNew* palette,
                                                        const qreal pixmapWidth,
                                                        const qreal pixmapHeight)
{
    m_pixmap = QPixmap(static_cast<int>(pixmapWidth),
                       static_cast<int>(pixmapHeight));
    
    QPainter painter(&m_pixmap);
    QPen pen = painter.pen();
    painter.setPen(Qt::NoPen);

    painter.fillRect(m_pixmap.rect(), QColor(255, 255, 255));
    
    {
        float rightX(pixmapWidth);
        std::vector<PaletteNew::ScalarColor> scalarsAndColors = palette->getPosRange();
        const int32_t numScalars = static_cast<int32_t>(scalarsAndColors.size());
        for (int32_t i = (numScalars - 1); i >= 0; i--) {
            float leftX(0);
            if (i == 0) {
                /*
                 * Special case for "zero" color that extends from zero to
                 * halfway to the "next" color
                 */
                CaretAssertVectorIndex(scalarsAndColors, i);
                leftX = paletteNonInterpScalarToPixMap(pixmapWidth,
                                                                scalarsAndColors[i].scalar);
                CaretAssertVectorIndex(scalarsAndColors, i + 1);
                const float x2 = paletteNonInterpScalarToPixMap(pixmapWidth,
                                                                scalarsAndColors[i + 1].scalar);
                rightX = ((leftX + x2) / 2.0);
            }
            else {
                CaretAssertVectorIndex(scalarsAndColors, i - 1);
                leftX = paletteNonInterpScalarToPixMap(pixmapWidth,
                                                       scalarsAndColors[i - 1].scalar);
            }
            
            /*
             * Extend to prevent gaps dues to rounding
             */
            const float x(leftX);
            const float y(0);
            const float w(rightX - leftX + 1);
            const float h(pixmapHeight);
            QRectF rect(x, y, w, h);

            QColor color;
            CaretAssertVectorIndex(scalarsAndColors, i);
            color.setRgbF(scalarsAndColors[i].color[0],
                          scalarsAndColors[i].color[1],
                          scalarsAndColors[i].color[2]);
            
            QBrush brush(color);
            painter.setBrush(brush);
            painter.drawRect(rect);
            
            rightX = leftX;
        }
    }

    {
        float leftX(0);
        std::vector<PaletteNew::ScalarColor> scalarsAndColors = palette->getNegRange();
        const int32_t numScalars = static_cast<int32_t>(scalarsAndColors.size());
        for (int32_t i = 0; i < numScalars; i++) {
            float rightX(0.0);
            if (i == (numScalars - 1)) {
                /*
                 * Special case for "zero"
                 */
                CaretAssertVectorIndex(scalarsAndColors, i - 1);
                const float x2 = paletteNonInterpScalarToPixMap(pixmapWidth,
                                                       scalarsAndColors[i - 1].scalar);
                CaretAssertVectorIndex(scalarsAndColors, i);
                rightX = paletteNonInterpScalarToPixMap(pixmapWidth,
                                                        scalarsAndColors[i].scalar);
                
                leftX = ((x2 + rightX) / 2.0);
            }
            else {
                CaretAssertVectorIndex(scalarsAndColors, i);
                leftX = paletteNonInterpScalarToPixMap(pixmapWidth,
                                                        scalarsAndColors[i].scalar);
                CaretAssertVectorIndex(scalarsAndColors, i + 1);
                rightX = paletteNonInterpScalarToPixMap(pixmapWidth,
                                                          scalarsAndColors[i + 1].scalar);
            }
            
            /*
             * Extend to prevent gaps dues to rounding
             */
            QRect rect(leftX, 0,  /* X, Y */
                       rightX - leftX + 1, pixmapHeight);  /* Width, Height */
            
            QColor color;
            CaretAssertVectorIndex(scalarsAndColors, i);
            color.setRgbF(scalarsAndColors[i].color[0],
                          scalarsAndColors[i].color[1],
                          scalarsAndColors[i].color[2]);
            
            QBrush brush(color);
            painter.setBrush(brush);
            painter.drawRect(rect);
            
            leftX = rightX;
        }
    }

//    {
//        float leftX(0);
//        std::vector<PaletteNew::ScalarColor> scalarsAndColors = palette->getNegRange();
//        const int32_t numScalars = static_cast<int32_t>(scalarsAndColors.size());
//        for (int32_t i = 0; i < (numScalars - 1); i++) {
//            CaretAssertVectorIndex(scalarsAndColors, i + 1);
//            float rightX = paletteNonInterpScalarToPixMap(pixmapWidth,
//                                                         scalarsAndColors[i + 1].scalar);
//
//            /*
//             * Extend to prevent gaps dues to rounding
//             */
//            QRect rect(leftX, 0,  /* X, Y */
//                       rightX - leftX + 1, pixmapHeight);  /* Width, Height */
//
//            QColor color;
//            CaretAssertVectorIndex(scalarsAndColors, i);
//            color.setRgbF(scalarsAndColors[i].color[0],
//                          scalarsAndColors[i].color[1],
//                          scalarsAndColors[i].color[2]);
//
//            QBrush brush(color);
//            painter.setBrush(brush);
//            painter.drawRect(rect);
//
//            leftX = rightX;
//        }
//    }

    float zeroColor[3];
    palette->getZeroColor(zeroColor);
    const int32_t zeroLineWidth(std::min(pixmapWidth * 0.02,
                                2.0));
    drawLineInColorBar(painter,
                       pen,
                       zeroColor,
                       (pixmapWidth / 2),
                       pixmapHeight,
                       zeroLineWidth);

//    QColor penColor;
//    penColor.setRgbF(zeroColor[0], zeroColor[1], zeroColor[2]);
//    painter.setPen(Qt::SolidLine);
//    pen.setColor(penColor);
//    pen.setWidth(zeroLineWidth);
//    painter.setPen(pen);
//
//    painter.drawLine(pixmapWidth / 2, 0, pixmapWidth / 2, pixmapHeight);
}

void
PalettePixmapPainter::drawLineInColorBar(QPainter& painter,
                                         QPen& pen,
                                         const float rgbFloat[3],
                                         const qreal x,
                                         const qreal pixmapHeight,
                                         const int32_t lineWidth)
{
    QColor penColor;
    penColor.setRgbF(rgbFloat[0], rgbFloat[1], rgbFloat[2]);
    painter.setPen(Qt::SolidLine);
    pen.setColor(penColor);
    pen.setWidth(lineWidth);
    painter.setPen(pen);
    
    painter.drawLine(x, 0, x, pixmapHeight);
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

