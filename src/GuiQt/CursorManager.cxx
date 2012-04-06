
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

#include <QPixmap>

#define __CURSOR_MANAGER_DECLARE__
#include "CursorManager.h"
#undef __CURSOR_MANAGER_DECLARE__

#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::CursorManager 
 * \brief Manages cursors
 *
 * Provides cursors, some predefined by Qt and
 * others unique to workbench.
 */

/**
 * Constructor.
 */
CursorManager::CursorManager()
: CaretObject()
{
    this->defaultCursor = QCursor();
    this->penCursor     = this->loadCursor(":/cursor_pen_eraser_32x32.png", 
                                           6, 
                                           32 - 7, 
                                           Qt::UpArrowCursor);
    this->pointingHandCursor = QCursor(Qt::PointingHandCursor);
    this->waitCursor = QCursor(Qt::WaitCursor);
}

/**
 * Destructor.
 */
CursorManager::~CursorManager()
{
    
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
 * @return The default cursor.
 */
const QCursor&
CursorManager::getDefaultCursor() const
{
    return this->defaultCursor;
}

/**
 * @return The pen cursor.
 */
const QCursor&
CursorManager::getPenCursor() const
{
    return this->penCursor;
}

/**
 * @return The pointing hand cursor.
 */
const QCursor&
CursorManager::getPointingHandCursor() const
{
    return this->pointingHandCursor;
}

/**
 * @return The wait cursor.
 */
const QCursor&
CursorManager::getWaitCursor() const
{
    return this->waitCursor;
}


