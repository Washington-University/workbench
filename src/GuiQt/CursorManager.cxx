
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

#include <QBitmap>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QWidget>

#define __CURSOR_MANAGER_DECLARE__
#include "CursorManager.h"
#undef __CURSOR_MANAGER_DECLARE__

#include "CaretAssert.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::CursorManager 
 * \brief Manages cursors
 * \ingroup GuiQt
 * Provides cursors, some predefined by Qt and
 * others unique to workbench.
 */

/**
 * Constructor.
 */
CursorManager::CursorManager()
: CaretObject()
{
    m_fourArrowsCursor = createFourArrowsCursor();
    
    m_deleteCursor  = createDeleteCursor();
    
    m_penCursor     = this->loadCursor(":Cursor/pen_eraser.png",
                                           6, 
                                           32 - 7, 
                                           Qt::UpArrowCursor);
    
    m_rotationCursor = createRotationCursor();
    
    m_rotationCursor2D = createRotationCursorText("2");
}

/**
 * Destructor.
 */
CursorManager::~CursorManager()
{
    
}

/**
 * Set the given cursor for the given widget.
 * @param widget
 *    Widget that has its cursor set.
 * @param cursor
 *    Cursor for the widget.
 */
void 
CursorManager::setCursorForWidget(QWidget* widget,
                                  const CursorEnum::Enum cursor) const
{
    switch (cursor) {
        case CursorEnum::CURSOR_DEFAULT:
            widget->unsetCursor();
            break;
        case CursorEnum::CURSOR_DELETE:
            widget->setCursor(m_deleteCursor);
            break;
        case CursorEnum::CURSOR_DRAWING_PEN:
            widget->setCursor(m_penCursor);
            break;
        case CursorEnum::CURSOR_FOUR_ARROWS:
            widget->setCursor(m_fourArrowsCursor);
            break;
        case CursorEnum::CURSOR_HALF_ROTATION:
            widget->setCursor(m_rotationCursor2D);
            break;
        case CursorEnum::CURSOR_ROTATION:
            widget->setCursor(m_rotationCursor);
            break;
        case CursorEnum::CURSOR_ARROW:
        case CursorEnum::CURSOR_CLOSED_HAND:
        case CursorEnum::CURSOR_CROSS:
        case CursorEnum::CURSOR_POINTING_HAND:
        case CursorEnum::CURSOR_RESIZE_BOTTOM_LEFT_TOP_RIGHT:
        case CursorEnum::CURSOR_RESIZE_BOTTOM_RIGHT_TOP_LEFT:
        case CursorEnum::CURSOR_RESIZE_HORIZONTAL:
        case CursorEnum::CURSOR_RESIZE_VERTICAL:
        case CursorEnum::CURSOR_WAIT:
        case CursorEnum::CURSOR_WHATS_THIS:
            widget->setCursor(CursorEnum::toQtCursorShape(cursor));
            break;
    }
}


/**
 * Load an image and create a cursor using the image.
 *
 * @param filename
 *    Name of file containing the image.
 * @param hotSpotX
 *    Hot spot (location in cursor reported to GUI)
 * @param hotSpotY
 *    Hot spot (location in cursor reported to GUI)
 * @param cursorShapeIfImageLoadingFails
 *    Cursor shape used if loading the image fails.
 * @return 
 *    Cursor that was created.
 */
QCursor 
CursorManager::loadCursor(const QString& filename,
                          const int hotSpotX,
                          const int hotSpotY,
                          const Qt::CursorShape& cursorShapeIfImageLoadingFails) const
{
    QPixmap cursorPixmap;
    if (WuQtUtilities::loadPixmap(filename, cursorPixmap)) {
        QCursor cursor(cursorPixmap,
                       hotSpotX,
                       hotSpotY);
        return cursor;
    }
    
    return QCursor(cursorShapeIfImageLoadingFails);
}

/**
 * @return A four-arrows cursor
 */
QCursor
CursorManager::createFourArrowsCursor()
{
    /*
     * Create image into which to draw
     */
    const int width  = 32;
    const int height = width;
    QImage image(width,
                  height,
                  QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    
    /*
     * Create painter with black for filling and white for outline
     */
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing,
                           false);
    painter.setBackgroundMode(Qt::OpaqueMode);
    QPen pen = painter.pen();
    pen.setColor(Qt::white);
    pen.setWidth(1);
    painter.setPen(pen);
    QBrush brush(QColor(0, 0, 0),
                 Qt::SolidPattern);
    painter.setBrush(brush);
    
    /*
     * Four arrows symbol is symmetric
     */
    const int middle = width / 2;
    const int a = 2;
    const int b = 8;
    const int c = middle - 6;
    const int d = middle - 2;
    const int e = middle;
    const int f = width - d;
    const int g = width - c;
    const int h = width - b;
    const int i = width - a;
    
    /*
     * Points for polygon filled with black
     */
    QPolygon polygon;
    polygon.push_back(QPoint(a, e));
    polygon.push_back(QPoint(b, g));
    polygon.push_back(QPoint(b, f));
    polygon.push_back(QPoint(d, f));
    polygon.push_back(QPoint(d, h));
    polygon.push_back(QPoint(c, h));
    polygon.push_back(QPoint(e, i));
    polygon.push_back(QPoint(g, h));
    polygon.push_back(QPoint(f, h));
    polygon.push_back(QPoint(f, f));
    polygon.push_back(QPoint(h, f));
    polygon.push_back(QPoint(h, g));
    polygon.push_back(QPoint(i, e));
    polygon.push_back(QPoint(h, c));
    polygon.push_back(QPoint(h, d));
    polygon.push_back(QPoint(f, d));
    polygon.push_back(QPoint(f, b));
    polygon.push_back(QPoint(g, b));
    polygon.push_back(QPoint(e, a));
    polygon.push_back(QPoint(c, b));
    polygon.push_back(QPoint(d, b));
    polygon.push_back(QPoint(d, d));
    polygon.push_back(QPoint(b, d));
    polygon.push_back(QPoint(b, c));
    
    /*
     * Draw filled polygon.  
     * Note that last point is connected to first point by drawPolygon().
     */
    painter.drawPolygon(polygon);
    
    /*
     * Draw white around arrows.
     * Note that drawPolyline() DOES NOT connect last point to first point so add first point again at end.
     * Turn anti-aliasing on for lines
     */
    polygon.push_back(QPoint(a, e));
    painter.setRenderHint(QPainter::Antialiasing,
                          false);
    painter.drawPolyline(polygon);

    /*
     * Create the cursor
     */
    QPixmap cursorPixmap = QPixmap::fromImage(image);
    QCursor cursor(cursorPixmap);
    
    return cursor;
}

/**
 * @return A rotation cursor
 */
QCursor
CursorManager::createRotationCursor()
{
    /*
     * Create image into which to draw
     */
    const int width  = 32;
    const int height = width;
    QImage image(width,
                 height,
                 QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    
    /*
     * Create painter with black for filling and white for outline
     */
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing,
                          false);
    painter.setBackgroundMode(Qt::OpaqueMode);
    QPen pen = painter.pen();
    pen.setColor(Qt::white);
    pen.setWidth(1);
    painter.setPen(pen);
    QBrush brush(QColor(0, 0, 0),
                 Qt::SolidPattern);
    painter.setBrush(brush);
    
    /*
     * points left to right in rotation symbol
     */
    const int a = 2;
    const int b = 6;
    const int c = width - 14;
    const int d = width - 10;
    const int e = width - 8;
    const int f = width - 6;
    const int g = width - 2;
    
    /*
     * Points top to bottom in rotation symbol
     */
    const int h = 2;
    const int i = 6;
    const int j = height - 8;
    const int k = height - 2;
    
    /*
     * Points for polygon filled with black
     */
    QPolygon polygon;
    polygon.push_back(QPoint(a, k));
    polygon.push_back(QPoint(b, k));
    polygon.push_back(QPoint(b, i));
    polygon.push_back(QPoint(d, i));
    polygon.push_back(QPoint(d, j));
    polygon.push_back(QPoint(c, j));
    polygon.push_back(QPoint(e, k));
    polygon.push_back(QPoint(g, j));
    polygon.push_back(QPoint(f, j));
    polygon.push_back(QPoint(f, h));
    polygon.push_back(QPoint(a, h));
    
    /*
     * Draw filled polygon.
     * Note that last point is connected to first point by drawPolygon().
     */
    painter.drawPolygon(polygon);
    
    /*
     * Draw white around arrows.
     * Note that drawPolyline() DOES NOT connect last point to first point so add first point again at end.
     * Turn anti-aliasing on for lines
     */
    polygon.push_back(QPoint(a, k));
    painter.setRenderHint(QPainter::Antialiasing,
                          false);
    painter.drawPolyline(polygon);
    
    /*
     * Create the cursor
     */
    QPixmap cursorPixmap = QPixmap::fromImage(image);
    QCursor cursor(cursorPixmap);
    
    return cursor;
}

/**
 * @return A rotation cursor with a text character in the center
 * @param textCharacter;
 */
QCursor
CursorManager::createRotationCursorText(const AString& textCharacter)
{
    CaretAssert(textCharacter.length() <= 1);
    
    /*
     * Create image into which to draw
     */
    const int width  = 32;
    const int height = width;
    QImage image(width,
                 height,
                 QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    
    /*
     * Create painter with black for filling and white for outline
     */
    QPainter painter(&image);
    const QBrush defaultBrush(painter.brush());
    const QPen defaultPen(painter.pen());
    painter.setRenderHint(QPainter::Antialiasing,
                          false);
    painter.setBackgroundMode(Qt::TransparentMode);
    QPen pen = painter.pen();
    pen.setColor(Qt::white);
    pen.setWidth(1);
    painter.setPen(pen);
    QBrush brush(QColor(0, 0, 0),
                 Qt::SolidPattern);
    painter.setBrush(brush);
    
    /*
     * points left to right in rotation symbol
     */
    const int a = 2;
    const int b = 6;
    const int c = width - 14;
    const int d = width - 10;
    const int e = width - 8;
    const int f = width - 6;
    const int g = width - 2;
    
    /*
     * Points top to bottom in rotation symbol
     */
    const int h = 2;
    const int i = 6;
    const int j = height - 8;
    const int k = height - 2;
    
    /*
     * Points for polygon filled with black
     */
    QPolygon polygon;
    polygon.push_back(QPoint(a, k));
    polygon.push_back(QPoint(b, k));
    polygon.push_back(QPoint(b, i));
    polygon.push_back(QPoint(d, i));
    polygon.push_back(QPoint(d, j));
    polygon.push_back(QPoint(c, j));
    polygon.push_back(QPoint(e, k));
    polygon.push_back(QPoint(g, j));
    polygon.push_back(QPoint(f, j));
    polygon.push_back(QPoint(f, h));
    polygon.push_back(QPoint(a, h));
    
    /*
     * Draw filled polygon.
     * Note that last point is connected to first point by drawPolygon().
     */
    painter.drawPolygon(polygon);
    
    /*
     * Draw white around arrows.
     * Note that drawPolyline() DOES NOT connect last point to first point so add first point again at end.
     * Turn anti-aliasing on for lines
     */
    polygon.push_back(QPoint(a, k));
    painter.setRenderHint(QPainter::Antialiasing,
                          false);
    painter.drawPolyline(polygon);
    
    /*
     * Draw the text
     */
    if ( ! textCharacter.isEmpty()) {
        const QBrush savedBrush(painter.brush());
        const QPen   savedPen(painter.pen());
        
        QBrush brush(defaultBrush);
        brush.setColor(Qt::black);
        painter.setBackground(brush);

        QFont font;
        font.setPointSize(20);
        font.setBold(true);
        painter.setFont(font);
        
        /*
         * Text is drawn twice.  First time gets the bounds
         * rectangle that is used to draw a background before
         * the text is drawn a second time.
         */
        const int m(2);
        QRectF rectF(0, 4, width - m, height - m);
        QRectF boundsRect;
        painter.drawText(rectF, Qt::AlignCenter, textCharacter, &boundsRect);
        painter.fillRect(boundsRect, Qt::SolidPattern);
        pen.setColor(Qt::black);
        painter.drawText(rectF, Qt::AlignCenter, textCharacter);

        painter.setPen(savedPen);
        painter.setBrush(savedBrush);
    }
    
    /*
     * Create the cursor
     */
    QPixmap cursorPixmap = QPixmap::fromImage(image);
    QCursor cursor(cursorPixmap);
    
    return cursor;
}

/**
 * @return A delete cursor ("X" symbol)
 * @param textCharacter;
 */
QCursor
CursorManager::createDeleteCursor()
{
    /*
     * Create image into which to draw
     */
    const int width  = 32;
    const int height = width;
    QImage image(width,
                 height,
                 QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    
    /*
     * Create painter with black for filling and white for outline
     */
    QPainter painter(&image);
    const QBrush defaultBrush(painter.brush());
    const QPen defaultPen(painter.pen());
    painter.setRenderHint(QPainter::Antialiasing,
                          false);
    painter.setBackgroundMode(Qt::TransparentMode);
    QPen whitePen = painter.pen();
    whitePen.setColor(Qt::white);
    whitePen.setWidth(5);
    QPen blackPen = painter.pen();
    blackPen.setColor(Qt::black);
    blackPen.setWidth(2);
    
//    painter.setPen(pen);
//    QBrush brush(QColor(0, 0, 0),
//                 Qt::SolidPattern);
//    painter.setBrush(brush);
    
    /*
     * ends of line
     */
    const int center(width / 2);
    const int halfLength(5);
    int a(center - halfLength);
    int b(center + halfLength);
    
    painter.setPen(whitePen);
    painter.drawLine(a, a, b, b);
    painter.drawLine(a, b, b, a);

    --a;
    --b;
    painter.setPen(blackPen);
    painter.drawLine(a, a, b, b);
    painter.drawLine(a, b, b, a);

//    /*
//     * Draw white around arrows.
//     * Note that drawPolyline() DOES NOT connect last point to first point so add first point again at end.
//     * Turn anti-aliasing on for lines
//     */
//    polygon.push_back(QPoint(a, k));
//    painter.setRenderHint(QPainter::Antialiasing,
//                          false);
//    painter.drawPolyline(polygon);
    
    
    /*
     * Create the cursor
     */
    QPixmap cursorPixmap = QPixmap::fromImage(image);
    QCursor cursor(cursorPixmap);
    
    return cursor;
}
