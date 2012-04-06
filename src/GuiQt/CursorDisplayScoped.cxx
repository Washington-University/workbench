
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
 *
 * Displays a cursor that overrides all other cursors in the
 * application until an instance goes out of scope or the
 * method restoreCursor() is called.
 */

/**
 * Constructor that displays a wait cursor.
 *
 */
CursorDisplayScoped::CursorDisplayScoped()
: CaretObject()
{
    QApplication::setOverrideCursor(GuiManager::get()->getCursorManager()->getWaitCursor());
    QApplication::processEvents();
}

/**
 * Constructor for display of a specific cursor.
 *
 * @param cursor
 *   Cursor that is displayed.
 */
CursorDisplayScoped::CursorDisplayScoped(const QCursor& cursor)
: CaretObject()
{
    QApplication::setOverrideCursor(cursor);
    QApplication::processEvents();
}

/**
 * Destructor.
 */
CursorDisplayScoped::~CursorDisplayScoped()
{
    this->restoreCursor();
}

/**
 * Restore the default cursor.
 */
void 
CursorDisplayScoped::restoreCursor()
{
    QApplication::restoreOverrideCursor();
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CursorDisplayScoped::toString() const
{
    return "CursorDisplay";
}
