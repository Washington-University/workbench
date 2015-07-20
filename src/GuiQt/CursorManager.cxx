
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
    
    m_penCursor     = this->loadCursor(":Cursor/pen_eraser.png",
                                           6, 
                                           32 - 7, 
                                           Qt::UpArrowCursor);
    
    m_rotationCursor = createRotationCursor();
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
        case CursorEnum::CURSOR_DRAWING_PEN:
            widget->setCursor(m_penCursor);
            break;
        case CursorEnum::CURSOR_FOUR_ARROWS:
            widget->setCursor(m_fourArrowsCursor);
            break;
        case CursorEnum::CURSOR_ROTATION:
            widget->setCursor(m_rotationCursor);
            break;
        default:
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




