
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
    this->penCursor     = this->loadCursor(":Cursor/pen_eraser.png",
                                           6, 
                                           32 - 7, 
                                           Qt::UpArrowCursor);
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
            widget->setCursor(this->penCursor);
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




