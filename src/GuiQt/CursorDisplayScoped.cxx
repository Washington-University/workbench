
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

#include <QApplication>

#define __CURSOR_DISPLAY_SCOPED_DECLARE__
#include "CursorDisplayScoped.h"
#undef __CURSOR_DISPLAY_SCOPED_DECLARE__

#include "CursorManager.h"
#include "GuiManager.h"

using namespace caret;

    
/**
 * \class caret::CursorDisplayScoped 
 * \brief Displays a override cursor
 * \ingroup GuiQt
 * 
 * When one of the 'show' methods is called, 
 * displays a cursor that overrides all other cursors in the
 * application until an instance of this goes out of scope or the
 * method restoreCursor() is called.
 */

/**
 * Constructor that displays a wait cursor.
 *
 */
CursorDisplayScoped::CursorDisplayScoped()
: CaretObject()
{
    this->isCursorActive = false;
}

/**
 * Destructor.
 */
CursorDisplayScoped::~CursorDisplayScoped()
{
    this->restoreCursor();
}

/**
 * Display the given cursor.
 *
 * @param cursor
 *   Cursor that is displayed.
 */
void
CursorDisplayScoped::showCursor(const QCursor& cursor)
{
    this->isCursorActive = true;
    
    QApplication::setOverrideCursor(cursor);
    QApplication::processEvents();
}

/**
 * Display the wait cursor.
 */
void 
CursorDisplayScoped::showWaitCursor()
{
    this->showCursor(Qt::WaitCursor);
}

/**
 * Restore the default cursor.
 */
void 
CursorDisplayScoped::restoreCursor()
{
    if (this->isCursorActive) {
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
        this->isCursorActive = false;
    }
}

